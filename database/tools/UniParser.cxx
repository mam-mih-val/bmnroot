#include "UniParser.h"
#include "UniRun.h"
#include "UniParameter.h"
#include "UniDetectorParameter.h"
#include "ElogConnection.h"
#include "UniConnection.h"
#define ONLY_DECLARATIONS
#include "function_set.h"

#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TSQLStatement.h"
#include "TPRegexp.h"
#include "TSystem.h"

// XML
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

UniParser::UniParser()
{
}

UniParser::~UniParser()
{
}

int parse_cycle_statement(xmlNodePtr &cur_schema_node, vector<structParseSchema>& vecElements, int &skip_count, char &delimiter_char, int &column_count)
{
    vecElements.clear();

    // define count of the elements to skip
    skip_count = 0;
    if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"skip") != 0)
        skip_count = atoi((char*)xmlGetProp(cur_schema_node, (unsigned char*)"skip"));

    // define delimiter char
    delimiter_char = ';';
    if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"delimiter") != 0)
    {
        string delimiter_string = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"delimiter");
        if (delimiter_string[0] == '\\')
        {
            switch (delimiter_string[1])
            {
                case 't':
                {
                    delimiter_char = '\t';
                    break;
                }
            }
        }
        else
            delimiter_char = delimiter_string[0];
    }

    // write cycle structure to array vecElements
    column_count = 0;
    for (xmlNodePtr cycle_child = cur_schema_node->children; cycle_child; cycle_child = cycle_child->next)
    {
        TString strAction = (char*)xmlGetProp(cycle_child, (unsigned char*)"action");
        TString column_name = (char*)xmlGetProp(cycle_child, (unsigned char*)"column_name");
        TString statement_type = (char*)xmlGetProp(cycle_child, (unsigned char*)"type");
        //cout<<"The current scheme line properties:"<<endl<<"Action: "<<strAction<<endl<<"Column name: "<<column_name<<endl<<"Type: "<<statement_type<<endl<<endl;

        if (strAction == "skip")
        {
            structParseSchema par(true);

            vecElements.push_back(par);
        }
        else if (strAction == "update")
        {
            structParseSchema par(false, column_name, statement_type);
            par.isUpdate = true;

            vecElements.push_back(par);
            column_count++;
        }// if (strAction == "update")
        else if (strAction == "write")
        {
            structParseSchema par(false, column_name, statement_type);

            vecElements.push_back(par);
            column_count++;
        }// if (strAction == "write")
        else if (strAction == "parse")
        {
            structParseSchema par;
            par.isSkip = false;

            int start_index = 0;
            if ((char*)xmlGetProp(cycle_child, (unsigned char*)"start_index") != 0)
                start_index = atoi((char*)xmlGetProp(cycle_child, (unsigned char*)"start_index"));
            TString parse_type = (char*)xmlGetProp(cycle_child, (unsigned char*)"parse_type");
            TString delimiter = (char*)xmlGetProp(cycle_child, (unsigned char*)"delimiter");
            structParseRow row(column_name, statement_type, true, start_index, parse_type, delimiter);
            par.vecRows.push_back(row);

            vecElements.push_back(par);
            column_count++;
        }// if (strAction == "parse")
        else if (strAction == "multi")
        {
            structParseSchema par;
            par.isSkip = false;

            for (xmlNodePtr cycle_sub_child = cycle_child->children; cycle_sub_child; cycle_sub_child = cycle_sub_child->next)
            {
                strAction = (char*)xmlGetProp(cycle_sub_child, (unsigned char*)"action");
                column_name = (char*)xmlGetProp(cycle_sub_child, (unsigned char*)"column_name");
                statement_type = (char*)xmlGetProp(cycle_sub_child, (unsigned char*)"type");

                if (strAction == "write")
                {
                    structParseRow row(column_name, statement_type);
                    par.vecRows.push_back(row);
                }
                else if (strAction == "parse")
                {
                    int start_index = 0;
                    if ((char*)xmlGetProp(cycle_sub_child, (unsigned char*)"start_index") != 0)
                        start_index = atoi((char*)xmlGetProp(cycle_sub_child, (unsigned char*)"start_index"));
                    TString parse_type = (char*)xmlGetProp(cycle_sub_child, (unsigned char*)"parse_type");
                    TString delimiter = (char*)xmlGetProp(cycle_sub_child, (unsigned char*)"delimiter");
                    structParseRow row(column_name, statement_type, true, start_index, parse_type, delimiter);
                    par.vecRows.push_back(row);
                }
            }

            vecElements.push_back(par);
            column_count++;
        }// if (strAction == "multi")
    }// write cycle structure to array vecElements

    return 0;
}

TString prepare_sql_code(vector<structParseSchema> vecElements, TString strTableName, bool isUpdate)
{
    TString sql;
    if (!isUpdate)
    {
        sql = "insert into " + strTableName + "(";
        int count = 0;
        for (vector<structParseSchema>::iterator it = vecElements.begin(); it != vecElements.end(); ++it)
        {
            structParseSchema schema = *it;
            if (schema.isSkip)
                continue;

            for (int j = 0; j < schema.vecRows.size(); j++)
            {
                structParseRow row = schema.vecRows[j];
                if (count == 0)
                    sql += row.strColumnName;
                else
                    sql += ", " + row.strColumnName;

                count++;
            }
        }
        sql += ") values(";
        for (int i = 1; i <= count; i++)
        {
            if (i == 1)
                sql += TString::Format("$%d", i);
            else
                sql += TString::Format(", $%d", i);
        }
        sql += ")";
    }
    else
    {
        sql = "update " + strTableName + " set ";
        int count = 0;
        for (vector<structParseSchema>::iterator it = vecElements.begin(); it != vecElements.end(); ++it)
        {
            structParseSchema schema = *it;
            if ((schema.isSkip) || (schema.isUpdate))
                continue;

            for (int j = 0; j < schema.vecRows.size(); j++)
            {
                structParseRow row = schema.vecRows[j];
                if (count == 0)
                    sql += TString::Format("%s = $%d", row.strColumnName.Data(), count+2);
                else
                    sql += TString::Format(", %s = $%d", row.strColumnName.Data(), count+2);

                count++;
            }
        }
        sql += " where ";
        count = 0;
        for (vector<structParseSchema>::iterator it = vecElements.begin(); it != vecElements.end(); ++it)
        {
            structParseSchema schema = *it;
            if (!schema.isUpdate)
                continue;

            for (int j = 0; j < schema.vecRows.size(); j++)
            {
                structParseRow row = schema.vecRows[j];
                if (count == 0)
                    sql += TString::Format("%s = $1", row.strColumnName.Data());
                //else
                //    sql += TString::Format(", %s = $%d", row.strColumnName, count+2);

                count++;
            }
        }
    }// prepare SQL query for cycle

    return sql;
}

int write_string_to_db(string &write_string, TSQLStatement* stmt, structParseSchema &schema, int &count, int cycle_counter)
{
    int tmp_count;

    if (schema.isUpdate)
    {
        tmp_count = count;
        count = 0;
    }

    // cycle for schema
    string token;
    for (int i = 0; i < schema.vecRows.size(); i++)
    {
        token = write_string;
        structParseRow row = schema.vecRows[i];

        unsigned char* pArray = nullptr;
        Long_t size_array = -1;
        if (row.isParse)
        {
            if (row.iStartIndex > 0)
                token = token.substr(row.iStartIndex, token.length()-row.iStartIndex);

            if (row.strParseType != "")
            {
                if (row.strParseType == "counter")
                    token = int_to_string(cycle_counter);
                if (row.strParseType(0,5) == "value")
                    token = row.strParseType(6,row.strParseType.Length()-6).Data();

                if (row.strParseType == "int")
                {
                    int last_digit;
                    for (last_digit = 0; last_digit < token.length(); last_digit++)
                    {
                        if (!isdigit(token[last_digit]))
                            break;
                    }
                    last_digit++;

                    if (last_digit > 1)
                        token = token.substr(0, last_digit-1);
                    else
                        token = "";
                }
                if (row.strParseType == "int_array")
                {
                    // parse token and form integer array
                    istringstream token_stream(token);
                    string subtoken;
                    vector<int> vecInt;
                    while (getline(token_stream, subtoken, row.strDelimiter[0]))
                    {
                        int cur_int = atoi(subtoken.c_str());
                        vecInt.push_back(cur_int);
                    }

                    int size_int = vecInt.size();
                    int* pIntArray = new int[size_int];
                    for (int j = 0; j < size_int; j++)
                        pIntArray[j] = vecInt[j];
                    size_array = size_int * sizeof(int);

                    pArray = new unsigned char[size_array];
                    memcpy(pArray, pIntArray, size_array);
                    delete [] pIntArray;
                    vecInt.clear();
                }
                if (row.strParseType == "double_array")
                {
                    // parse token and form double array
                    token = trim(token);
                    istringstream token_stream(token);
                    string subtoken;
                    vector<double> vecDouble;
                    while (getline(token_stream, subtoken, row.strDelimiter[0]))
                    {
                        double cur_double = atof(subtoken.c_str());
                        //cout<<". CurDouble: "<<cur_double;
                        vecDouble.push_back(cur_double);
                    }
                    //cout<<endl;

                    int size_double = vecDouble.size();
                    double* pDoubleArray = new double[size_double];
                    for (int j = 0; j < size_double; j++)
                        pDoubleArray[j] = vecDouble[j];
                    size_array = size_double * sizeof(double);

                    pArray = new unsigned char[size_array];
                    memcpy(pArray, pDoubleArray, size_array);
                    delete [] pDoubleArray;
                    vecDouble.clear();
                }

            }
        }// if row.isParse


        if (row.strStatementType == "int")
        {
            stmt->SetInt(count, atoi(token.c_str()));
            cout<<"SetInt: "<<token<<endl;
            count++;
        }
        else
        {
            if (row.strStatementType == "hex")
            {

                stmt->SetInt(count, hex_string_to_int(token.c_str()));
                cout<<"SetHex: "<<token<<endl;
                count++;
            }
            else
            {
                if (row.strStatementType == "double")
                {
                    // replace ',' by '.' if present
                    replace_string_in_text(token, ",", ".");

                    stmt->SetDouble(count, atof(token.c_str()));
                    cout<<"SetDouble: "<<token<<endl;
                    count++;
                }
                else
                {
                    if (row.strStatementType == "string")
                    {
                        stmt->SetString(count, token.c_str());
                        cout<<"SetString: "<<token<<endl;
                        count++;
                    }
                    else
                    {
                        if (row.strStatementType == "datetime")
                        {
                            TDatime d(token.c_str());
                            stmt->SetDatime(count, d);
                            cout<<"SetDatime: "<<token<<endl;
                            count++;
                        }
                        else
                        {
                            if (row.strStatementType == "binary")
                            {
                                cout<<"SetBinary: "<<(void*)pArray<<" with size: "<<size_array<<endl;
                                stmt->SetLargeObject(count, (void*)pArray, size_array);
                                count++;
                                delete [] pArray;
                            }// "binary"
                        }// "datetime"
                    }// "string"
                }// "double"
            }// "hex"
        }// "int"
    }// // cycle by schema rows because it can consist multiple writing

    if (schema.isUpdate)
        count = tmp_count;

    return 0;
}

// recursive search for a node in XML document from root node by given node name
xmlNodePtr findNodeByName(xmlNodePtr rootnode, const char* nodename)
{
    xmlNodePtr node = rootnode;
    if (node == nullptr)
    {
        cout<<"XML document is empty!"<<endl;
        return nullptr;
    }

    while (node != nullptr)
    {
        if (node->type != XML_ELEMENT_NODE)
        {
            node = node->next;
            continue;
        }

        if ((node->name != nullptr) && (strcmp((char*)node->name, nodename) == 0))
            return node;
        else
        {
            if (node->children != nullptr)
            {
                xmlNodePtr intNode = findNodeByName(node->children, nodename);
                if (intNode != nullptr)
                    return intNode;
            }
        }

        node = node->next;
    }

    return nullptr;
}

int UniParser::ParseXml2Db(TString xmlName, TString schemaPath, bool isUpdate)
{
    // pointer to XML document
    xmlDocPtr docXML = xmlReadFile(xmlName, nullptr, 0);
    if (!docXML)
    {
        cout<<"ERROR: reading XML file '"<<xmlName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, nullptr, 0);
    if (!docSchema)
    {
        cout<<"ERROR: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        xmlFreeDoc(docXML);
        return - 2;
    }

    xmlNodePtr cur_xml_node = xmlDocGetRootElement(docXML);
    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"ERROR: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docXML);
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "uniparser") != 0)
    {
        cout<<"ERROR: it is not 'uniparser' scheme: "<<(char*)cur_schema_node->name<<endl;
        xmlFreeDoc(docXML);
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
    {
        xmlFreeDoc(docXML);
        xmlFreeDoc(docSchema);
        return -3;
    }
    TSQLServer* db_server = connDb->GetSQLServer();

    string strTableName = "";
    // parse SCHEMA file
    while (cur_schema_node)
    {
        //cout<<"Current schema node: "<<(char*)cur_schema_node->name<<endl;

        // parse table name if exists
        if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name") != 0)
        {
            strTableName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name");
            cout<<"Current database table: "<<strTableName<<endl;
        }

        if (strcmp((char*)cur_schema_node->name, "search") == 0)
        {
            string strSearchName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"name");
            // search for XML node with given name and move cursor to the new position
            cur_xml_node = findNodeByName(cur_xml_node, strSearchName.c_str());
            if (cur_xml_node == nullptr)
            {
                cout<<"ERROR: end of the XML document was reached while parsing (search for)"<<endl;
                delete connDb;
                xmlFreeDoc(docXML);
                xmlFreeDoc(docSchema);
                return -8;
            }

            cout<<"Current node after search: "<<(char*)cur_xml_node->name<<endl;
        }
        else if (strcmp((char*)cur_schema_node->name, "move") == 0)
        {
            xmlAttr* attribute = cur_schema_node->properties;
            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp((char*)attribute->name, "down") == 0)
                {
                    xmlChar* value = xmlNodeListGetString(cur_schema_node->doc, attribute->children, 1);

                    int count = atoi((char*)value);
                    for (int i = 0; i < count; i++)
                    {
                        cur_xml_node = cur_xml_node->children;
                        if (cur_xml_node->type != XML_ELEMENT_NODE)
                            cur_xml_node = cur_xml_node->next;

                        if (cur_xml_node == nullptr)
                        {
                            cout<<"ERROR: end of the XML document was reached while parsing (move - down)"<<endl;
                            xmlFree(value);
                            delete connDb;
                            xmlFreeDoc(docXML);
                            xmlFreeDoc(docSchema);
                            return -6;
                        }
                    }

                    xmlFree(value);
                }// if attribute name is "down"

                attribute = attribute->next;
            }

            cout<<"Current node after move: "<<(char*)cur_xml_node->name<<endl;
        }
        // PARSE CYCLE
        else if (strcmp((char*)cur_schema_node->name, "cycle") == 0)
        {
            TString strChildName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"child");

            // parse CYCLE attributes to vector of Elements
            int skip_count, column_count; char delimiter_char;
            vector<structParseSchema> vecElements;
            parse_cycle_statement(cur_schema_node, vecElements, skip_count, delimiter_char, column_count);

            // prepare SQL query for TXT cycle
            if (column_count == 0)
            {
                cout<<"ERROR: no columns were chosen for insert or update"<<endl;
                delete connDb;
                xmlFreeDoc(docXML);
                xmlFreeDoc(docSchema);
                return -7;
            }
            TString sql = prepare_sql_code(vecElements, strTableName, isUpdate);
            cout<<"SQL code: "<<sql<<endl;

            TSQLStatement* stmt = db_server->Statement(sql);

            // run XML file cycle and write the fields to DB
            int cycle_counter = 0;
            for (cur_xml_node = cur_xml_node->children; cur_xml_node; cur_xml_node = cur_xml_node->next)
            {
                if (strcmp((char*)cur_xml_node->name, strChildName.Data()))
                    continue;

                if (skip_count > 0)
                {
                    skip_count--;
                    continue;
                }

                stmt->NextIteration();
                cycle_counter++;

                int count;
                if (!isUpdate)
                    count = 0;
                else
                    count = 1;

                int i = 0;
                // cycle for XML child elements
                cout<<"Cur node: "<<cur_xml_node->name<<endl;
                for (xmlNodePtr cycle_child = cur_xml_node->children; cycle_child; cycle_child = cycle_child->next, i++)
                {
                    cout<<"cycle_child node: "<<cycle_child->name<<endl;
                    structParseSchema schema = vecElements[i];
                    if (schema.isSkip)
                        continue;

                    TString xml_child_value = (char*)cycle_child->children->content;
                    string token = xml_child_value.Data();

                    write_string_to_db(token, stmt, schema, count, cycle_counter);
                }// cycle for XML child elements
            }// run XML file cycle and write the fields to DB

            stmt->Process();
            delete stmt;
        }// CYCLE PROCESSING

        cur_schema_node = cur_schema_node->next;
    }// for docSchema level 0

    delete connDb;
    xmlFreeDoc(docXML);
    xmlFreeDoc(docSchema);

    return 0;
}

int UniParser::ParseCsv2Db(TString csvName, TString schemaPath, bool isUpdate)
{
    ifstream csvFile;
    csvFile.open(csvName, ios::in);
    if (!csvFile.is_open())
    {
        cout<<"ERROR: reading CSV file '"<<csvName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, nullptr, 0);
    if (!docSchema)
    {
        cout<<"ERROR: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"ERROR: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "uniparser") != 0)
    {
        cout<<"ERROR: it is not 'uniparser' scheme: "<<(char*)cur_schema_node->name<<endl;
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
        return -3;
    TSQLServer* db_server = connDb->GetSQLServer();

    // parse SCHEMA file
    string strTableName = "", cur_line;
    while (cur_schema_node)
    {
        //cout<<"Current schema node: "<<cur_schema_node->name<<endl;

        // parse table name if exists
        if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name") != 0)
        {
            strTableName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name");
            cout<<"Current database table: "<<strTableName<<endl;
        }

        if (strcmp((char*)cur_schema_node->name, "search") == 0)
        {
            string strSearchName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"name");
        }
        // PARSE CYCLE
        else if (strcmp((char*)cur_schema_node->name, "cycle") == 0)
        {
            int skip_count, column_count; char delimiter_char;
            vector<structParseSchema> vecElements;

            // parse CYCLE attributes to vector of Elements
            parse_cycle_statement(cur_schema_node, vecElements, skip_count, delimiter_char, column_count);

            // prepare SQL query for TXT cycle
            TString sql = prepare_sql_code(vecElements, strTableName, isUpdate);
            cout<<"SQL code: "<<sql<<endl;

            for (int i = 0; i < skip_count; i++)
                getline(csvFile, cur_line);

            TSQLStatement* stmt = db_server->Statement(sql);

            // run CSV file cycle and write the fields to DB
            int cycle_counter = 0;
            while (getline(csvFile, cur_line))
            {
                if (cur_line == "")
                    continue;

                // parse current line
                string trim_line = trim(cur_line);

                istringstream line_stream(trim_line);

                stmt->NextIteration();
                cycle_counter++;

                // parse tokens by symbol separated
                int count;
                if (!isUpdate)
                    count = 0;
                else
                    count = 1;

                int i = 0;
                string token;
                while (getline(line_stream, token, ';'))
                {
                    structParseSchema schema = vecElements[i];
                    if (schema.isSkip)
                    {
                        i++;
                        continue;
                    }

                    write_string_to_db(token, stmt, schema, count, cycle_counter);

                    i++;
                }// parse CSV line by tokens separated by symbols
            }// run CSV file cycle and write the fields to DB

            stmt->Process();
            delete stmt;
        }// CYCLE PROCESSING

        cur_schema_node = cur_schema_node->next;
    }// for docSchema level 0

    delete connDb;
    csvFile.close();
    xmlFreeDoc(docSchema);

    return 0;
}

int UniParser::ParseTxt2Db(TString txtName, TString schemaPath, bool isUpdate)
{
    ifstream txtFile;
    txtFile.open(txtName, ios::in);
    if (!txtFile.is_open())
    {
        cout<<"ERROR: reading TXT file '"<<txtName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, nullptr, 0);
    if (!docSchema)
    {
        cout<<"ERROR: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"ERROR: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "uniparser") != 0)
    {
        cout<<"ERROR: it is not 'uniparser' scheme: "<<(char*)cur_schema_node->name<<endl;
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
        return -3;

    TSQLServer* db_server = connDb->GetSQLServer();

    // parse SCHEMA file
    string strTableName = "", cur_line;
    while (cur_schema_node)
    {
        cout<<"Current schema node: "<<cur_schema_node->name<<endl;

        // parse table name if exists
        if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name") != 0)
        {
            strTableName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name");
            cout<<"Current database table: "<<strTableName<<endl;
        }

        if (strcmp((char*)cur_schema_node->name, "skip") == 0)
        {
            int skip_line_count = 0;
            string strLineCount = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"line_count");
            if (strLineCount != "")
                skip_line_count = atoi(strLineCount.c_str());

            for (int i = 0; i < skip_line_count; i++)
                getline(txtFile, cur_line);
        }

        if (strcmp((char*)cur_schema_node->name, "search") == 0)
        {
            string strSearchName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"name");
        }
        // PARSE CYCLE
        else if (strcmp((char*)cur_schema_node->name, "cycle") == 0)
        {
            int skip_count, column_count; char delimiter_char;
            vector<structParseSchema> vecElements;

            // parse CYCLE attributes to vector of Elements
            parse_cycle_statement(cur_schema_node, vecElements, skip_count, delimiter_char, column_count);

            // prepare SQL query for TXT cycle
            TString sql = prepare_sql_code(vecElements, strTableName, isUpdate);
            cout<<"SQL code: "<<sql<<endl;

            for (int i = 0; i < skip_count; i++)
                getline(txtFile, cur_line);

            TSQLStatement* stmt = db_server->Statement(sql);

            // run TXT file cycle and write the fields to DB
            int cycle_counter = 0;
            while (getline(txtFile, cur_line))
            {
                string trim_line = trim(cur_line);

                if (trim_line == "")
                    continue;

                // parse current line
                istringstream line_stream(cur_line);

                stmt->NextIteration();
                cycle_counter++;

                // parse tokens by symbol separated
                int count;
                if (!isUpdate)
                    count = 0;
                else
                    count = 1;

                int i = 0;
                string token;
                while (getline(line_stream, token, delimiter_char))
                {
                    structParseSchema schema = vecElements[i];
                    if (schema.isSkip)
                    {
                        i++;
                        continue;
                    }

                    write_string_to_db(token, stmt, schema, count, cycle_counter);

                    i++;
                }// parse TXT line by tokens separated by symbols

                cout<<endl;
            }// run TXT file cycle and write the fields to DB

            stmt->Process();
            delete stmt;
        }// CYCLE PROCESSING

        cur_schema_node = cur_schema_node->next;
    }// parse SCHEMA file

    delete connDb;
    txtFile.close();
    xmlFreeDoc(docSchema);

    return 0;
}


int UniParser::ParseTxtNoise2Db(int period_number, TString txtName, TString schemaPath)
{
    ifstream txtFile;
    txtFile.open(txtName, ios::in);
    if (!txtFile.is_open())
    {
        cout<<"ERROR: reading TXT file '"<<txtName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, nullptr, 0);
    if (!docSchema)
    {
        cout<<"ERROR: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"ERROR: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "UniParser_schema") != 0)
    {
        cout<<"ERROR: it is not a UniParser schema"<<endl;
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniConnection* connDb = UniConnection::Open();
    if (connDb == nullptr)
        return -3;

    // parse SCHEMA file
    string strTableName = "";
    int skip_line_count = 0;
    while (cur_schema_node)
    {
        // parse table name if exists
        if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name") != 0)
        {
            strTableName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name");
            cout<<"Current database table: "<<strTableName<<endl;
        }

        if (strcmp((char*)cur_schema_node->name, "skip") == 0)
        {
            string strLineCount = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"line_count");
            if (strLineCount != "")
                skip_line_count = atoi(strLineCount.c_str());
        }

        cur_schema_node = cur_schema_node->next;
    }

    string cur_line;
    for (int i = 0; i < skip_line_count; i++)
        getline(txtFile, cur_line);

    while (getline(txtFile, cur_line))
    {
        // parse run and row count
        string reduce_line = reduce(cur_line);

        cout<<"Current run and count line: "<<reduce_line<<endl;
        int run_number = -1, row_count = -1;
        istringstream line_stream(reduce_line);
        int num = 1;
        string token;
        // parse tokens by space separated
        while(getline(line_stream, token, ' '))
        {
            if (num == 1)
                run_number = atoi(token.c_str());
            if (num == 2)
                row_count = atoi(token.c_str());

            num++;
        }

        // parse slots and channels
        vector<UniValue*> arr;
        for (int i = 0; i < row_count; i++)
        {
            getline(txtFile, cur_line);
            cout<<"Current run: "<<run_number<<", row: "<<i<<", line: "<<cur_line<<endl;
            reduce_line = reduce(cur_line);

            line_stream.str(reduce_line);
            num = 1;
            int slot_number;
            // parse tokens by space separated
            while (getline(line_stream, token, ' '))
            {
                if (num == 1)
                    slot_number = atoi(token.c_str());

                if (num > 1)
                {
                    size_t index_sym = token.find_first_of('-');
                    if (index_sym == string::npos)
                    {
                        int channel_number = atoi(token.c_str());
                        if (channel_number == 0)
                        {
                            if (!is_string_number(token))
                                continue;
                        }

                        IIValue* st = new IIValue;
                        st->value1 = slot_number;
                        st->value2 = channel_number;
                        arr.push_back(st);
                    }
                    else
                    {
                        string strFirst = token.substr(0, index_sym);
                        string strSecond = token.substr(index_sym + 1, token.length() - index_sym - 1);
                        int num_first = atoi(strFirst.c_str());
                        int num_second = atoi(strSecond.c_str());
                        for (int j = num_first; j <= num_second; j++)
                        {
                            IIValue* st = new IIValue;
                            st->value1 = slot_number;
                            st->value2 = j;
                            arr.push_back(st);
                        }
                    }
                }

                num++;
            }// parse tokens by space separated
        }// for (int i = 0; i < row_count; i++)

        // skip empty line
        if (getline(txtFile, cur_line))
        {
            reduce_line = trim(cur_line, " \t\r");
            if (reduce_line != "")
            {
                cout<<"CRITICAL ERROR: file format is not correct, current line:"<<reduce_line<<endl;
                txtFile.close();
                return -4;
            }
        }

        /*
        // print array
        cout<<"Slot:Channel"<<endl;
        for (int i = 0; i < arr.size(); i++)
        {
            IIValue* pValue = (IIValue*)arr[i];
            cout<<pValues->value1<<":"<<pValues->value2<<endl;
        }
        cout<<endl;
        */

        UniDetectorParameter* pDetectorParameter = UniDetectorParameter::CreateDetectorParameter("DCH1", "noise", period_number, run_number, period_number, run_number, arr); //(detector_name, parameter_name, start_run, end_run, parameter_value)
        if (pDetectorParameter == nullptr)
            continue;

        // clean memory after work
        for (int i = 0; i < arr.size(); i++) delete arr[i];
        if (pDetectorParameter)
            delete pDetectorParameter;
    }

    txtFile.close();
    delete connDb;

    return 0;
}

int UniParser::ParseDb2Db()
{
    TSQLServer* source_db = TSQLServer::Connect("pgsql://nc13.jinr.ru/bmn_elog", "login", "password");
    if (source_db == nullptr)
    {
        cout<<"ERROR: source database connection was not established (nc13.jinr.ru) for bmn_elog"<<endl;
        return -1;
    }

    TString sql_source = TString::Format(
        "select record_id, field_comment "
        "from record_");
    TSQLStatement* stmt_source = source_db->Statement(sql_source);
     //cout<<"SQL code: "<<sql<<endl;

    // get record from the database
    if (!stmt_source->Process())
    {
        cout<<"ERROR: getting records from the database has been failed"<<endl;

        delete stmt_source;
        delete source_db;

        return -2;
    }

    // store result of statement in buffer
    stmt_source->StoreResult();

    TSQLServer* dest_db = TSQLServer::Connect("pgsql://nc13.jinr.ru/bmn_elog", "login", "password");
    if (dest_db == nullptr)
    {
        cout<<"ERROR: destination database connection was not established (nc13.jinr.ru) for bmn_elog"<<endl;
        return -3;
    }

    // extract rows one after another
    while (stmt_source->NextResultRow())
    {
        if (stmt_source->IsNull(1)) continue;

        int record_id = stmt_source->GetInt(0);
        TString field_comment = stmt_source->GetString(1);

        // search for SP-57 and VKM2 magnetic field in A
        size_t beg_pos, end_pos;
        string field_lower(field_comment.Data()), sp57 = "", vkm2 = "";
        int iField = -1;
        transform(field_lower.begin(), field_lower.end(), field_lower.begin(), ::tolower);

        size_t ind = field_lower.find("sp-57");
        if (ind != string::npos)
        {
            beg_pos = ind + 5;
            sp57 = find_first_number(field_lower, beg_pos, end_pos, false);
            iField = atoi(sp57.c_str());

            if (field_lower[end_pos+1] != 'a')
            {
                cout<<"ERROR: field is not ended with A"<<field_lower<<endl;
                continue;
            }

            TString sql_dest = TString::Format(
                "update record_ "
                "set sp_57 = $1 "
                "where record_id = $2");
            TSQLStatement* stmt_dest = dest_db->Statement(sql_dest);

            stmt_dest->NextIteration();
            stmt_dest->SetInt(0, iField);
            stmt_dest->SetInt(1, record_id);

            // write new value to the database
            if (!stmt_dest->Process())
            {
                cout<<"ERROR: updating information has been failed"<<endl;

                delete stmt_dest;
                continue;
            }

            ind = end_pos;
            delete stmt_dest;
        }
        else
            ind = 0;

        ind = field_lower.find("vkm2", ind);
        if (ind != string::npos)
        {
            beg_pos = ind + 4;
            vkm2 = find_first_number(field_lower, beg_pos, end_pos, false);
            iField = atoi(vkm2.c_str());

            if (field_lower[end_pos+1] != 'a')
            {
                cout<<"ERROR: field is not ended with A"<<field_lower<<endl;
                continue;
            }

            TString sql_dest = TString::Format(
                "update record_ "
                "set vkm2 = $1 "
                "where record_id = $2");
            TSQLStatement* stmt_dest = dest_db->Statement(sql_dest);

            stmt_dest->NextIteration();
            stmt_dest->SetInt(0, iField);
            stmt_dest->SetInt(1, record_id);

            // write new value to the database
            if (!stmt_dest->Process())
            {
                cout<<"ERROR: updating information has been failed"<<endl;

                delete stmt_dest;
                continue;
            }

            delete stmt_dest;
        }
    }

    delete stmt_source;
    delete source_db;
    delete dest_db;

    return 0;
}

// parse text file to the C++ structure
int UniParser::ParseTxt2Struct(TString txtName, TString schemaPath, vector<structParseValue*>& parse_values, vector<structParseSchema>& vecElements, int iVerbose)
{
    gSystem->ExpandPathName(txtName);
    gSystem->ExpandPathName(schemaPath);

    ifstream txtFile;
    txtFile.open(txtName.Data(), ios::in);
    if (!txtFile.is_open())
    {
        if (iVerbose > 0) cout<<"ERROR: reading TXT file '"<<txtName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath.Data(), nullptr, 0);
    if (!docSchema)
    {
        if (iVerbose > 0) cout<<"ERROR: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        if (iVerbose > 0) cout<<"ERROR: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -3;
    }
    if (strcmp((char*)cur_schema_node->name, "uniparser") != 0)
    {
        if (iVerbose > 0) cout<<"ERROR: it is not 'uniparser' scheme: "<<(char*)cur_schema_node->name<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    cur_schema_node = cur_schema_node->children;

    // parse SCHEMA file
    string strTableName = "", cur_line;
    TDatime dtSpillPrevious;
    while (cur_schema_node)
    {
        if (iVerbose > 1) cout<<"Current schema node: "<<cur_schema_node->name<<endl;

        // parse table name if exists
        if ((char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name") != 0)
        {
            strTableName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"table_name");
            if (iVerbose > 0) cout<<"Current database table: "<<strTableName<<endl;
        }

        if (strcmp((char*)cur_schema_node->name, "skip") == 0)
        {
            int skip_line_count = 0;
            string strLineCount = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"line_count");
            if (strLineCount != "")
                skip_line_count = atoi(strLineCount.c_str());

            for (int i = 0; i < skip_line_count; i++)
                getline(txtFile, cur_line);
        }

        if (strcmp((char*)cur_schema_node->name, "search") == 0)
        {
            string strSearchName = (char*)xmlGetProp(cur_schema_node, (unsigned char*)"name");
        }
        // PARSE CYCLE
        else if (strcmp((char*)cur_schema_node->name, "cycle") == 0)
        {
            int skip_count, column_count; char delimiter_char;
            // parse CYCLE attributes to vector of Elements
            parse_cycle_statement(cur_schema_node, vecElements, skip_count, delimiter_char, column_count);

            for (int i = 0; i < skip_count; i++)
                getline(txtFile, cur_line);

            // run TXT file cycle and make the action
            int cycle_counter = 0;
            bool notFirstLine = false, isSkipLines = false;
            while (getline(txtFile, cur_line))
            {
                //string trim_line = trim(cur_line);
                string reduce_line = reduce(cur_line);
                if (iVerbose > 1) cout<<"Current line: "<<reduce_line<<endl;

                if (reduce_line == "")
                    continue;

                // parse current line
                istringstream line_stream(reduce_line);
                cycle_counter++;

                // parse tokens by symbol separated
                TString strDate = "", strTime = "", strSpillEnd = "";
                string token; int num = 1, i = 0;
                structParseValue* st = new structParseValue();
                while (getline(line_stream, token, delimiter_char))
                {
                    if (i >= vecElements.size())
                    {
                        if (iVerbose > 0) cout<<"WARNING: Some columns were not parsed (starting with "<<token<<")"<<endl;
                        break;
                    }
                    structParseSchema schema = vecElements[i];
                    if (schema.isSkip)
                    {
                        i++;
                        if (iVerbose > 1) cout<<"schema.isSkip: "<<schema.isSkip<<endl;
                        continue;
                    }

                    if (iVerbose > 1) cout<<"The current type of the column '"<<schema.vecRows[0].strColumnName<<"': "<<schema.vecRows[0].strStatementType<<endl;
                    // parse columns
                    if (schema.vecRows[0].strStatementType == "datetime")
                    {
                        strDate = token.c_str();
                        TPRegexp date_prefix("^[0-9][0-9]?[.][0-9][0-9]?[.][0-9][0-9][0-9]?[0-9]?");
                        if (!strDate.Contains(date_prefix))
                        {
                            TString strSQLDate = dtSpillPrevious.AsSQLString();
                            strDate = TString::Format("%s.%s.%s", strSQLDate(8,2).Data(), strSQLDate(5,2).Data(), strSQLDate(0,4).Data());
                            if (iVerbose > 0) cout<<"WARNING: recovery date: "<<strDate<<endl;
                        }
                        else
                            getline(line_stream, token, delimiter_char);

                        strTime = token.c_str();
                        TPRegexp time_prefix("^[0-9][0-9]?[:][0-9][0-9]?[:][0-9][0-9]?");
                        if (!strTime.Contains(time_prefix))
                        {
                            strTime = "00:00:00";
                        }
                        else
                            getline(line_stream, token, delimiter_char);

                        i++;
                        schema = vecElements[i];
                        if (schema.isSkip)
                        {
                            i++;
                            continue;
                        }
                    }

                    if (schema.vecRows[0].strStatementType == "int")
                    {
                        st->arrValues.push_back(atoi(token.c_str()));
                    }
                    else {
                        if (schema.vecRows[0].strStatementType == "double")
                        {
                            st->arrValues.push_back(atof(token.c_str()));
                        }
                        else {
                            if (iVerbose > 0) cout<<"ERROR: type of the column is not supported: "<<schema.vecRows[0].strStatementType<<endl;
                            return -5;
                        }
                    }

                    i++;
                }// parse TXT line by tokens separated by symbols

                strSpillEnd = TString::Format("%s %s", strDate.Data(), strTime.Data());
                tm tmbuf[1] = {{0}};
                strptime(strSpillEnd.Data(), "%d.%m.%Y %H:%M:%S", tmbuf);
                TDatime dtSpillEnd(tmbuf->tm_year, tmbuf->tm_mon+1, tmbuf->tm_mday, tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec);
                st->dtSpillEnd = dtSpillEnd;
                if ((notFirstLine) && (dtSpillPrevious > dtSpillEnd))
                {
                    if ((!isSkipLines) && (iVerbose > 0)) cout<<endl<<"ERROR: The time sequence of the lines is corrupted. The following lines is skipped:"<<endl;
                    cout<<reduce_line<<endl;
                    isSkipLines = true;
                    delete st;
                    continue;
                }
                else
                {
                    if (isSkipLines)
                    {
                        if (iVerbose > 0) cout<<"Skipped fragment is finished"<<endl<<endl;
                        isSkipLines = false;
                    }
                }
                dtSpillPrevious = dtSpillEnd;
                notFirstLine = true;

                //if (iVerbose > 1) cout<<"Spill End: "<<st->dtSpillEnd.AsSQLString()<<". Beam DAQ: "<<st->beam_daq<<". Beam All: "<<st->beam_all<<". Trigger DAQ: "<<st->trigger_daq<<". Trigger All: "<<st->trigger_all<<endl;
                parse_values.push_back(st);

                if (iVerbose > 1) cout<<endl;
            }// run TXT file cycle and write the fields to the structure
        }// CYCLE PROCESSING

        cur_schema_node = cur_schema_node->next;
    }// (while) parse SCHEMA file

    txtFile.close();
    xmlFreeDoc(docSchema);

    return 0;
}

bool check_element(const string& str, size_t pos, string element)
{
    size_t str_length = str.length(), element_length = element.length();
    if ((pos + element_length) > str_length)
        return false;

    for (int i = 1; i < element_length; i++)
    {
        if (str[pos+i] != element[i])
            return false;
    }

    if (((pos == 0) || (str[pos-1] == ' ') || (str[pos-1] == ',') || (str[pos-1] == '.') || (str[pos-1] == ')')) &&
       ((pos == (str_length-element_length)) || (str[pos+element_length] == ' ') || (str[pos+element_length] == ',') || (str[pos+element_length] == '.') || (str[pos+element_length] == '(')))
    {
        return true;
    }

    return false;
}

// convert datetime string "Day  DD Mon YYYY HH:MM:SS +ZZZZ" to TDatime without zone, e.g. "Fri  20 Feb 2015 12:03:41 +0300"
TDatime stringToDatime(string str_time)
{
    tm tmbuf[1] = {{0}};

    //tm tmbuf;
    //sscanf(str_time.Data(), "%2d.%2d.%4d %2d:%2d:%2d", &tmbuf.tm_mday, &tmbuf.tm_mon, &tmbuf.tm_year, &tmbuf.tm_hour, &tmbuf.tm_min, &tmbuf.tm_sec);
    //str>>get_time(&tmbuf, "%a %d %b %Y %H:%M:%S +");
    strptime(str_time.c_str(), "%a  %d %b %Y %H:%M:%S %z", tmbuf);
    TDatime ttime(tmbuf->tm_year, tmbuf->tm_mon+1, tmbuf->tm_mday, tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec);

    return ttime;
}


// specific functions for experiments (BM@N)
int UniParser::ConvertElogCsv(TString csvName, char separate_symbol)
{
    ifstream csvFile;
    csvFile.open(csvName, ios::in);
    if (!csvFile.is_open())
    {
        cout<<"ERROR: reading CSV file '"<<csvName<<"' was failed"<<endl;
        return -1;
    }

    ElogConnection* connDb = ElogConnection::Open();
    if (connDb == nullptr) return -1;
    TSQLServer* elog_server = connDb->GetSQLServer();
    if (elog_server == nullptr)
    {
        cout<<"ERROR: ELOG connection was not established"<<endl;
        return -2;
    }
    //cout<<"Server info: "<<pSQLServer->ServerInfo()<<endl;
    TSQLStatement* stmt = nullptr;

    // run and parse CSV file, and update the fields in DB
    int skip_line = 1, updated_count = 0;
    string cur_line;
    while (getline(csvFile, cur_line))
    {
        if (cur_line == "")
            continue;
        if (skip_line > 0)
        {
            skip_line--;
            continue;
        }

        // parse current line
        string trim_line = trim(cur_line);
        istringstream line_stream(trim_line);

        // parse tokens by symbol separated
        string token, record_id = "", record_date = "", author = "", record_type = "", subject = "", run_number = "", shift_leader = "", trigger_info = "",
                      daq_status = "", magnet_field = "", beam = "", record_comment = "";
        int index = 0, iError = 0;
        while (getline(line_stream, token, separate_symbol))
        {
            switch (index)
            {
                case 0:
                    record_id = token;
                    break;
                case 1:
                    record_date = token;
                    break;
                case 2:
                    author = trim(token, "\"");
                    break;
                case 3:
                    record_type = trim(token, "\"");
                    break;
                case 4:
                    break;
                case 5:
                    subject = trim(token, "\"");
                    break;
                case 6:
                    run_number = trim(token, "\"");
                    break;
                case 7:
                    shift_leader = trim(token, "\"");
                    break;
                case 8:
                    trigger_info = trim(token, "\"");
                    break;
                case 9:
                    daq_status = trim(token, "\"");
                    break;
                case 10:
                    magnet_field = trim(token, "\"");
                    break;
                case 11:
                    beam = trim(token, "\"");
                    break;
                case 12:
                    record_comment = trim(token, "\"");
                    break;
                default:
                {
                    cout<<"ERROR: more tokens separated by '"<<separate_symbol<<"' for record_id: "<<record_id<<endl;
                    iError = 2;
                    break;
                }
            }
            index++;
        }// parse tokens by symbol separated
        if (iError != 0)
            continue;

        int iRecordId = -1, iAuthor = -1, iRecordType = -1, iRunNumber = -1, iLastRunNumber = -1, iShiftLeader = -1, iTriggerInfo = -1;
        // RECORD ID
        if (!is_string_number(record_id))
        {
            cout<<"ERROR: record id is not number: "<<record_id<<endl;
            continue;
        }
        iRecordId = atoi(record_id.c_str());
        if (iRecordId < 1)
        {
            cout<<"ERROR: record id should be greater zero: "<<iRecordId<<endl;
            continue;
        }

        cout<<"RECORD ID: "<<iRecordId<<endl;

        // RECORD DATE
        if (record_date == "")
        {
            cout<<"ERROR: record date is empty: "<<record_date<<endl;
            continue;
        }
        TDatime dtRecordDate = stringToDatime(record_date);

        // AUTHOR
        TString strAuthorLower = author;
        strAuthorLower.ToLower();
        if ((strAuthorLower == "shift") || (strAuthorLower == "none"))
            author = "";
        if (author != "")
        {
            TString sql_author = TString::Format(
                                    "select person_id "
                                    "from person_ "
                                    "where position(lower(person_name) in lower('%s')) > 0", author.c_str());
            stmt = elog_server->Statement(sql_author);

            if (!stmt->Process())
            {
                cout<<"ERROR: getting author from the database has been failed: "<<author<<endl;
                delete stmt;
                continue;
            }

            // store result of statement in buffer
            stmt->StoreResult();

            // extract row
            if (!stmt->NextResultRow())
            {
                cout<<"ERROR: author was not found in the database: "<<author<<endl;
                delete stmt;
                continue;
            }

            iAuthor = stmt->GetInt(0);
            delete stmt;
        }

        // TYPE
        string subject_lower = subject, type_lower = record_type;
        transform(subject_lower.begin(), subject_lower.end(), subject_lower.begin(), ::tolower);
        transform(type_lower.begin(), type_lower.end(), type_lower.begin(), ::tolower);

        size_t ind;
        if (type_lower == "other")
        {
            ind = subject_lower.find("new run");
            if (ind != string::npos)
            {
                type_lower = "new run";
                if (subject_lower.length() == 7)
                    subject = "";
            }
        }
        TString sql_type = TString::Format(
                               "select type_id "
                                "from type_ "
                                "where lower(type_text) = lower('%s')", type_lower.c_str());
        stmt = elog_server->Statement(sql_type);

        if (!stmt->Process())
        {
            cout<<"ERROR: getting type from the database has been failed: "<<record_type<<endl;
            delete stmt;
            continue;
        }

        // store result of statement in buffer
        stmt->StoreResult();

        // extract row
        if (!stmt->NextResultRow())
        {
            cout<<"ERROR: type was not found in the database: "<<record_type<<endl;
            delete stmt;
            continue;
        }

        iRecordType = stmt->GetInt(0);
        delete stmt;

        // RUN NUMBER
        string strRunNumber = run_number, strLastRunNumber = "";
        transform(strRunNumber.begin(), strRunNumber.end(), strRunNumber.begin(), ::tolower);
        if (strRunNumber == "none") strRunNumber = "";

        if (strRunNumber != "")
        {
            if (strRunNumber.substr(0,3) == "run")
                strRunNumber = strRunNumber.substr(3);
            ind = strRunNumber.find_first_of('-');
            if (ind != string::npos)
            {
                strLastRunNumber = strRunNumber.substr(ind+1);
                if (!is_string_number(strLastRunNumber))
                {
                    cout<<"ERROR: last run number is not number: "<<run_number<<endl;
                    continue;
                }
                iLastRunNumber = atoi(strLastRunNumber.c_str());
                if (iLastRunNumber < 1)
                {
                    cout<<"ERROR: last run number should be greater than zero: "<<iLastRunNumber<<endl;
                    continue;
                }
                strRunNumber = strRunNumber.substr(0,ind);
            }

            if (!is_string_number(strRunNumber))
            {
                cout<<"ERROR: run number is not number: "<<run_number<<endl;
                continue;
            }
            iRunNumber = atoi(strRunNumber.c_str());
            if (iRunNumber < 1)
            {
                cout<<"ERROR: run number should be greater than zero: "<<iRunNumber<<endl;
                continue;
            }

            if ((iLastRunNumber > 0) && (iLastRunNumber <= iRunNumber))
            {
                cout<<"ERROR: last run number ("<<iLastRunNumber<<") should be greater than first run number ("<<iRunNumber<<")."<<endl;
                continue;
            }
        }

        // SHIFT_LEADER
        TString strLeaderLower = shift_leader;
        strLeaderLower.ToLower();
        if ((strLeaderLower == "shift") || (strLeaderLower == "none"))
            shift_leader = "";
        if (shift_leader != "")
        {
            TString sql_leader = TString::Format(
                                    "select person_id "
                                    "from person_ "
                                    "where position(lower(person_name) in lower('%s')) > 0", shift_leader.c_str());
            stmt = elog_server->Statement(sql_leader);

            if (!stmt->Process())
            {
                cout<<"ERROR: getting shift leader from the database has been failed: "<<shift_leader<<endl;
                delete stmt;
                continue;
            }

            // store result of statement in buffer
            stmt->StoreResult();

            // extract row
            if (!stmt->NextResultRow())
            {
                cout<<"ERROR: shift leader was not found in the database: "<<shift_leader<<endl;
                delete stmt;
                continue;
            }

            iShiftLeader = stmt->GetInt(0);
            delete stmt;
        }

        // TRIGGER
        TString strTriggerLower = trigger_info;
        strTriggerLower.ToLower();
        if (strTriggerLower == "none")
            trigger_info = "";
        if (trigger_info != "")
        {
            TString sql_trigger = TString::Format(
                                    "select trigger_id "
                                    "from trigger_ "
                                    "where lower(trigger_info) = lower('%s')", trigger_info.c_str());
            stmt = elog_server->Statement(sql_trigger);

            if (!stmt->Process())
            {
                cout<<"ERROR: getting trigger from the database has been failed: "<<trigger_info<<endl;
                delete stmt;
                continue;
            }

            // store result of statement in buffer
            stmt->StoreResult();

            // extract row
            if (!stmt->NextResultRow())
            {
                cout<<"ERROR: trigger was not found in the database: "<<trigger_info<<endl;
                delete stmt;
                continue;
            }

            iTriggerInfo = stmt->GetInt(0);
            delete stmt;
        }

        // MAGNET FIELD
        // search for SP-41, SP-57, VKM2 and magnetic field in mvolt
        size_t beg_pos, end_pos;
        string field_lower = magnet_field, sp41 = "", sp57 = "", vkm2 = "", field_mv = "";
        int iSP41 = -1, iSP57 = -1, iVKM2 = -1;
        double dVoltage = -1;
        transform(field_lower.begin(), field_lower.end(), field_lower.begin(), ::tolower);
        // replace sp41 by sp-41 and sp57 by sp-57
        replace_string_in_text(field_lower, "sp41", "sp-41");
        replace_string_in_text(field_lower, "sp57", "sp-57");
        ind = field_lower.find("sp-41");
        if (ind != string::npos)
        {
            beg_pos = ind + 5;
            sp41 = find_first_number(field_lower, beg_pos, end_pos);
        }
        else
        {
            beg_pos = 0;
            sp41 = find_first_number(field_lower, beg_pos, end_pos);
        }
        if (sp41 != "")
        {
            iSP41 = atoi(sp41.c_str());
            if (iSP41 < 0)
            {
                cout<<"ERROR: SP-41 field can't be negative: "<<iSP41<<endl;
                continue;
            }
        }

        ind = field_lower.find("sp-57");
        if (ind != string::npos)
        {
            beg_pos = ind + 5;
            sp57 = find_first_number(field_lower, beg_pos, end_pos, false);
        }
        if (sp57 != "")
            iSP57 = atoi(sp57.c_str());

        ind = field_lower.find("vkm2");
        if (ind != string::npos)
        {
            beg_pos = ind + 4;
            vkm2 = find_first_number(field_lower, beg_pos, end_pos);
        }
        if (vkm2 != "")
        {
            iVKM2 = atoi(vkm2.c_str());
            if (iVKM2 < 0)
            {
                cout<<"ERROR: VKM2 field can't be negative: "<<iVKM2<<endl;
                continue;
            }
        }

        ind = field_lower.find("mv");
        if (ind != string::npos)
        {
            end_pos = ind - 1;
            field_mv = find_last_double_number(field_lower, beg_pos, end_pos);
            if (field_mv.empty())
            {
                cout<<"ERROR: no field voltage before 'mV' found: "<<field_lower<<" for record id: "<<record_id<<endl;
                continue;
            }
        }
        if (field_mv != "")
        {
            dVoltage = atof(field_mv.c_str());
            if (dVoltage < -1)
            {
                cout<<"ERROR: VKM2 field can't be less or equal -1: "<<dVoltage<<endl;
                continue;
            }
        }

        // BEAM
        // find energy
        double dEnergy = -1;
        string beam_lower = beam;
        transform(beam_lower.begin(), beam_lower.end(), beam_lower.begin(), ::tolower);
        size_t found, next_found;
        found = beam_lower.find("gev");
        if (found != std::string::npos)
        {
            next_found = beam_lower.find("gev", found+1);
            if (next_found != std::string::npos)
            {
                cout<<"ERROR: more than one 'gev' found: "<<beam<<" for record id: "<<record_id<<endl;
                continue;
            }

            end_pos = found - 1;
            string energy = find_last_double_number(beam_lower, beg_pos, end_pos);
            if (energy.empty())
            {
                cout<<"ERROR: no energy before 'gev' found: "<<beam<<" for record id: "<<record_id<<endl;
                continue;
            }
            dEnergy = atof(energy.c_str());
            if (dEnergy <= 0)
            {
                cout<<"ERROR: energy should be greater than 0: "<<dEnergy<<endl;
                continue;
            }
        }

        // parse particles: beam - d or C; target - C|C12, Al, Cu, Pb, C2H4|CH2
        string strBeam = "", strTarget = "", strTargetWidth = "";
        double dTargetWidth = -1;
        /*do
        {
            ind = beam_lower.find("c");
            if (ind != string::npos)
            {
                if (((ind == 0) || (beam_lower[ind-1] == " ") || (beam_lower[ind-1] == ",") || (beam_lower[ind-1] == ".")) &&
                   ((ind == beam_lower.length()-1) || (beam_lower[ind+1] == " ") || (beam_lower[ind+1] == ",") || (beam_lower[ind+1] == ".")))
                    strBeam = "C";
            }
        } while ((ind != string::npos) && (strBeam != ""));*/
        for (int i = 0; i < beam_lower.length(); i++)
        {
            if (strBeam.empty())
            {
                switch (beam_lower[i])
                {
                    case 'c':
                    {
                        // if "C"
                        if (check_element(beam_lower, i, "c"))
                            strBeam = "C";

                        break;
                    }//case 'c':
                    case 'd':
                    {
                        // if "d"
                        if (check_element(beam_lower, i, "d"))
                            strBeam = "d";

                        break;
                    }//case 'd':
                }
            }
            else
            {
                switch (beam_lower[i])
                {
                    case 'c':
                    {
                        // if "C"
                        if (check_element(beam_lower, i, "c"))
                        {
                            strTarget = "C";
                        }
                        else
                        {
                            // if Cu
                            if (check_element(beam_lower, i, "cu"))
                            {
                                strTarget = "Cu";
                            }
                            else
                            {
                                // if C12
                                if (check_element(beam_lower, i, "c12"))
                                {
                                    strTarget = "C";
                                }
                                else
                                {
                                    // if CH2
                                    if (check_element(beam_lower, i, "ch2"))
                                    {
                                        strTarget = "C2H4";
                                    }
                                    else
                                    {
                                        // if C2H4
                                        if (check_element(beam_lower, i, "c2h4"))
                                        {
                                            strTarget = "C2H4";
                                        }
                                    }
                                }
                            }
                        }

                        break;
                    }
                    case 'p':
                    {
                        // if "Pb"
                        if (check_element(beam_lower, i, "pb"))
                            strTarget = "Pb";

                        break;
                    }//case 'p':
                    case 'a':
                    {
                        // if "Al"
                        if (check_element(beam_lower, i, "al"))
                            strTarget = "Al";

                        break;
                    }//case 'a':
                }// switch (beam_lower[i])
            }// else if (strBeam.empty())

            if (strTarget != "")
            {
                ind = beam_lower.find("mm", i);
                if (ind != string::npos)
                {
                    end_pos = ind - 1;
                    strTargetWidth = find_last_double_number(beam_lower, beg_pos, end_pos);
                    if (strTargetWidth.empty())
                    {
                        cout<<"ERROR: no target width before 'mm' found: "<<beam_lower<<" for record id: "<<record_id<<endl;
                        continue;
                    }
                }
                if (strTargetWidth != "")
                {
                    dTargetWidth = atof(strTargetWidth.c_str());
                    if (dTargetWidth < 0)
                    {
                        cout<<"ERROR: target field can't be less or equal 0: "<<dTargetWidth<<endl;
                        break;
                    }
                }

                break;
            }
        }

        TString strComment = "";
        if (subject != "")
        {
            strComment += TString::Format("%s", subject.c_str());
            if (record_comment != "") strComment +=". ";
        }
        strComment += record_comment;

        do{
        // FORM INSERT STATEMENT
        TString sql_insert = TString::Format(
                "insert into log_record(record_date, author, record_type, run_number, "
                "shift_leader, trigger_config, daq_status, sp_41, field_comment, "
                "beam, energy, target, target_width, record_comment) "
                "values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14)");
        stmt = elog_server->Statement(sql_insert);
        stmt->NextIteration();
        int cur_index = 0;
        stmt->SetDatime(cur_index++, dtRecordDate);
        //stmt->SetString(cur_index++, record_date.c_str());
        if (iAuthor < 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetInt(cur_index++, iAuthor);
        /*if (author == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, author.c_str());*/
        if (iRecordType < 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetInt(cur_index++, iRecordType);
        /*if (record_type == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, record_type.c_str());*/
        if (iRunNumber < 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetInt(cur_index++, iRunNumber);
        /*if (run_number == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, run_number.c_str());*/
        if (iShiftLeader < 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetInt(cur_index++, iShiftLeader);
        /*if (shift_leader == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, shift_leader.c_str());*/
        if (iTriggerInfo < 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetInt(cur_index++, iTriggerInfo);
        /*if (trigger_info == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, trigger_info.c_str());*/
        if (daq_status == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, daq_status.c_str());
        //stmt->SetString(cur_index++, daq_status.c_str());
        if (iSP41 < 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetInt(cur_index++, iSP41);
        TString strFieldComment = "";
        int iter = 0;
        if (dVoltage > -1)
        {
            strFieldComment += TString::Format("SP-41 = %smV", field_mv.c_str());
            iter++;
        }
        if (sp57 != "")
        {
            if (iter > 0) strFieldComment += ", ";
            strFieldComment += TString::Format("SP-57 = %dA", iSP57);
            iter++;
        }
        if (iVKM2 >= 0)
        {
            if (iter > 0) strFieldComment += ", ";
            strFieldComment += TString::Format("VKM2 = %dA", iVKM2);
            iter++;
        }
        if (strFieldComment == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, strFieldComment);
        /*if (magnet_field == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, magnet_field.c_str());*/
        if (strBeam == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, strBeam.c_str());
        if (dEnergy <= 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetDouble(cur_index++, dEnergy);
        if (strTarget == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, strTarget.c_str());
        if (dTargetWidth <= 0)
            stmt->SetNull(cur_index++);
        else
            stmt->SetDouble(cur_index++, (Double_t)dTargetWidth);
        /*if (beam == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, beam.c_str());*/
        if (strComment == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, strComment);
        /*if (subject == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, subject.c_str());
        if (record_comment == "")
            stmt->SetNull(cur_index++);
        else
            stmt->SetString(cur_index++, record_comment.c_str());*/

        // inserting new ELog record to the ELog Database
        if (!stmt->Process())
        {
                cout<<"ERROR: inserting a new Elog record to the ELog Database has been failed"<<endl;
                delete stmt;
                return -3;
        }
        delete stmt;

        updated_count++;
        if (iLastRunNumber > 0) iRunNumber++;
        } while ((iLastRunNumber > 0) && (iRunNumber <= iLastRunNumber));
    }// run CSV file

    delete elog_server;
    csvFile.close();
    return updated_count;
}

// -------------------------------------------------------------------
ClassImp(UniParser);
