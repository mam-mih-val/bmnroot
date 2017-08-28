#include "UniDbParser.h"
#include "UniDbConnection.h"
#include "db_classes/UniDbRun.h"
#include "db_classes/UniDbParameter.h"
#include "db_classes/UniDbDetectorParameter.h"
#define ONLY_DECLARATIONS
#include "function_set.h"

#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TSQLStatement.h"
#include "TDatime.h"

// XML
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
using namespace std;

UniDbParser::UniDbParser()
{
}

UniDbParser::~UniDbParser()
{
}

int parse_cycle_statement(xmlNodePtr &cur_schema_node, vector<structParseSchema> &vecElements, int &skip_count, char &delimiter_char, int &column_count)
{
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

        unsigned char* pArray = NULL;
        Long_t size_array = -1;
        if (row.isParse)
        {
            if (row.iStartIndex > 0)
                token = token.substr(row.iStartIndex, token.length()-row.iStartIndex);

            if (row.strParseType != "")
            {
                if (row.strParseType == "counter")
                    token = convert_int_to_string(cycle_counter);
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
    if (node == NULL)
    {
        cout<<"XML document is empty!"<<endl;
        return NULL;
    }

    while (node != NULL)
    {
        if (node->type != XML_ELEMENT_NODE)
        {
            node = node->next;
            continue;
        }

        if ((node->name != NULL) && (strcmp((char*)node->name, nodename) == 0))
            return node;
        else
        {
            if (node->children != NULL)
            {
                xmlNodePtr intNode = findNodeByName(node->children, nodename);
                if (intNode != NULL)
                    return intNode;
            }
        }

        node = node->next;
    }

    return NULL;
}

int UniDbParser::ParseXml2Db(TString xmlName, TString schemaPath, bool isUpdate)
{
    // pointer to XML document
    xmlDocPtr docXML = xmlReadFile(xmlName, NULL, 0);
    if (!docXML)
    {
        cout<<"Error: reading XML file '"<<xmlName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, NULL, 0);
    if (!docSchema)
    {
        cout<<"Error: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        xmlFreeDoc(docXML);
        return - 2;
    }

    xmlNodePtr cur_xml_node = xmlDocGetRootElement(docXML);
    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"Error: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docXML);
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "unidbparser_schema") != 0)
    {
        cout<<"Error: it is not UniDbParser schema"<<endl;
        xmlFreeDoc(docXML);
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
    {
        xmlFreeDoc(docXML);
        xmlFreeDoc(docSchema);
        return -3;
    }
    TSQLServer* uni_db = connUniDb->GetSQLServer();

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
            if (cur_xml_node == NULL)
            {
                cout<<"Error: end of the XML document was reached while parsing (search for)"<<endl;
                delete connUniDb;
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

                        if (cur_xml_node == NULL)
                        {
                            cout<<"Error: end of the XML document was reached while parsing (move - down)"<<endl;
                            xmlFree(value);
                            delete connUniDb;
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
                cout<<"Error: no columns were chosen for insert or update"<<endl;
                delete connUniDb;
                xmlFreeDoc(docXML);
                xmlFreeDoc(docSchema);
                return -7;
            }
            TString sql = prepare_sql_code(vecElements, strTableName, isUpdate);
            cout<<"SQL code: "<<sql<<endl;

            TSQLStatement* stmt = uni_db->Statement(sql);

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

    delete connUniDb;
    xmlFreeDoc(docXML);
    xmlFreeDoc(docSchema);

    return 0;
}

int UniDbParser::ParseCsv2Db(TString csvName, TString schemaPath, bool isUpdate)
{
    ifstream csvFile;
    csvFile.open(csvName, ios::in);
    if (!csvFile.is_open())
    {
        cout<<"Error: reading CSV file '"<<csvName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, NULL, 0);
    if (!docSchema)
    {
        cout<<"Error: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"Error: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "unidbparser_schema") != 0)
    {
        cout<<"Error: it is not UniDbParser schema"<<endl;
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return -3;
    TSQLServer* uni_db = connUniDb->GetSQLServer();

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

            TSQLStatement* stmt = uni_db->Statement(sql);

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

    delete connUniDb;
    csvFile.close();
    xmlFreeDoc(docSchema);

    return 0;
}

int UniDbParser::ParseTxt2Db(TString txtName, TString schemaPath, bool isUpdate)
{
    ifstream txtFile;
    txtFile.open(txtName, ios::in);
    if (!txtFile.is_open())
    {
        cout<<"Error: reading TXT file '"<<txtName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, NULL, 0);
    if (!docSchema)
    {
        cout<<"Error: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"Error: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "unidbparser_schema") != 0)
    {
        cout<<"Error: it is not UniDbParser schema"<<endl;
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
        return -3;

    TSQLServer* uni_db = connUniDb->GetSQLServer();

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

            TSQLStatement* stmt = uni_db->Statement(sql);

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

    delete connUniDb;
    txtFile.close();
    xmlFreeDoc(docSchema);

    return 0;
}


int UniDbParser::ParseTxtNoise2Db(int period_number, TString txtName, TString schemaPath)
{
    ifstream txtFile;
    txtFile.open(txtName, ios::in);
    if (!txtFile.is_open())
    {
        cout<<"Error: reading TXT file '"<<txtName<<"' was failed"<<endl;
        return -1;
    }

    // read schema
    xmlDocPtr docSchema = xmlReadFile(schemaPath, NULL, 0);
    if (!docSchema)
    {
        cout<<"Error: reading schema file '"<<schemaPath<<"' was failed"<<endl;
        return - 2;
    }

    xmlNodePtr cur_schema_node = xmlDocGetRootElement(docSchema);
    if (!cur_schema_node)
    {
        cout<<"Error: schema of XML parsing is empty"<<endl;
        xmlFreeDoc(docSchema);
        return -4;
    }
    if (strcmp((char*)cur_schema_node->name, "unidbparser_schema") != 0)
    {
        cout<<"Error: it is not UniDbParser schema"<<endl;
        xmlFreeDoc(docSchema);
        return -5;
    }
    cur_schema_node = cur_schema_node->children;

    // open connection to database
    UniDbConnection* connUniDb = UniDbConnection::Open(UNIFIED_DB);
    if (connUniDb == 0x00)
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
        vector<IIStructure> arr;
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

                        IIStructure st;
                        st.int_1 = slot_number;
                        st.int_2 = channel_number;
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
                            IIStructure st;
                            st.int_1 = slot_number;
                            st.int_2 = j;
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
                cout<<"Critical Error: file format isn't correct, current line:"<<reduce_line<<endl;
                txtFile.close();
                return -4;
            }
        }

        // copy vector to dynamic array
        int size_arr = arr.size();
        //cout<<"Size array: "<<size_arr<<endl;
        IIStructure* pValues = new IIStructure[size_arr];
        for (int i = 0; i < size_arr; i++)
            pValues[i] = arr[i];

        /*
        // print array
        cout<<"Slot:Channel"<<endl;
        for (int i = 0; i < size_arr; i++)
        {
            cout<<pValues[i].int_1<<":"<<pValues[i].int_2<<endl;
        }
        cout<<endl;
        */

        UniDbDetectorParameter* pDetectorParameter = UniDbDetectorParameter::CreateDetectorParameter("DCH1", "noise", period_number, run_number, period_number, run_number, pValues, size_arr); //(detector_name, parameter_name, start_run, end_run, parameter_value, size_parameter_value)
        if (pDetectorParameter == NULL)
            continue;

        // clean memory after work
        delete [] pValues;
        if (pDetectorParameter)
            delete pDetectorParameter;
    }

    txtFile.close();
    delete connUniDb;

    return 0;
}

bool check_element(const string& str, size_t pos, string element, string element_sql, string& strFirstParticle, string& strSecondParticle, int& iError)
{
    size_t str_length = str.length(), element_length = element.length();
    if ((pos + element_length) > str_length)
        return false;

    for (int i = 1; i < element_length; i++)
    {
        if (str[pos+i] != element[i])
            return false;
    }

    if (((pos == 0) || (str[pos-1] == ' ')) && ((pos == (str_length-element_length)) || (str[pos+element_length] == ' ')))
    {
        if (strFirstParticle.empty())
            strFirstParticle = element_sql;
        else
        {
            if (strSecondParticle.empty())
                strSecondParticle = element_sql;
            else
            {
                cout<<"ERROR: more than 2 particles found"<<endl;
                iError = 1;
            }
        }
        return true;
    }

    return false;
}

// specific functions for experiments (BM@N)
int UniDbParser::ParseElogCsv(int period_number, TString csvName, char separate_symbol)
{
    ifstream csvFile;
    csvFile.open(csvName, ios::in);
    if (!csvFile.is_open())
    {
        cout<<"Error: reading CSV file '"<<csvName<<"' was failed"<<endl;
        return -1;
    }

    // run and parse CSV file, and update the fields in DB
    int updated_count = 0;
    string cur_line;
    while (getline(csvFile, cur_line))
    {
        if (cur_line == "")
            continue;

        // parse current line
        string trim_line = trim(cur_line);
        istringstream line_stream(trim_line);

        // parse tokens by symbol separated
        string token, run_number = "", elog_field = "";
        int ind = 1, iError = 0;
        while (getline(line_stream, token, separate_symbol))
        {
            switch (ind)
            {
                case 1:
                    run_number = token;
                    break;
                case 2:
                    elog_field = token;
                    break;
                default:
                {
                    cout<<"ERROR: more then two tokens separated by '"<<separate_symbol<<"' for run_number: "<<run_number<<endl;
                    iError = 2;
                    break;
                }
            }
            ind++;
        }// parse tokens by symbol separated
        if (iError != 0)
            continue;

        // is string an integer number?
        if (!is_string_number(run_number))
        {
            cout<<"ERROR: run number is not number: "<<run_number<<endl;
            continue;
        }
        int iRunNumber = atoi(run_number.c_str());

        // parse elog field
        // find energy
        double dEnergy = -1;
        string lower_field = elog_field;
        transform(lower_field.begin(), lower_field.end(), lower_field.begin(), ::tolower);
        size_t found, next_found;
        found = lower_field.find("gev");
        if (found != std::string::npos)
        {
            next_found = lower_field.find("gev", found+1);
            if (next_found != std::string::npos)
            {
                cout<<"ERROR: more than one 'gev' found: "<<elog_field<<" for run: "<<iRunNumber<<endl;
                continue;
            }

            string energy = elog_field.substr(0, found);
            energy = find_last_double_number(energy);
            if (energy.empty())
            {
                cout<<"ERROR: no energy before 'gev' found: "<<elog_field<<" for run: "<<iRunNumber<<endl;
                continue;
            }
            dEnergy = atof(energy.c_str());
        }
        else
        {
            cout<<"ERROR: no 'gev' found: "<<elog_field<<" for run: "<<iRunNumber<<endl;
            continue;
        }

        // parse particles: 0 - C, 1 - Cu
        string strFirstParticle, strSecondParticle;
        int last_sybmbol = lower_field.length() - 1;
        lower_field.erase(remove(lower_field.begin(), lower_field.end(), '"'), lower_field.end());
        for (int i = 0; i <= last_sybmbol; i++)
        {
            switch (lower_field[i])
            {
                case 'c':
                {
                    // if "C"
                    if (!check_element(lower_field, i, "c", "C", strFirstParticle, strSecondParticle, iError))
                    {
                        // if Cu
                        if (!check_element(lower_field, i, "cu", "Cu", strFirstParticle, strSecondParticle, iError))
                        {
                            // if C12
                            if (!check_element(lower_field, i, "c12", "C", strFirstParticle, strSecondParticle, iError))
                            {
                                // if CH2
                                if (!check_element(lower_field, i, "ch2", "C2H4", strFirstParticle, strSecondParticle, iError))
                                {
                                    // if C2H4
                                    check_element(lower_field, i, "c2h4", "C2H4", strFirstParticle, strSecondParticle, iError);
                                }
                            }
                        }
                    }

                    break;
                }//case 'c':
                case 'p':
                {
                    // if "Pb"
                    check_element(lower_field, i, "pb", "Pb", strFirstParticle, strSecondParticle, iError);

                    break;
                }//case 'p':
                case 'a':
                {
                    // if "Al"
                    check_element(lower_field, i, "al", "Al", strFirstParticle, strSecondParticle, iError);

                    break;
                }//case 'a':
            }

            if (iError != 0)
                break;
        }

        if (iError != 0) continue;

        if ((strFirstParticle.empty()) && (strSecondParticle.empty()))
        {
            cout<<"ERROR: no particles found: "<<elog_field<<" for run: "<<iRunNumber<<endl;
            continue;
        }

        // write to DB
        UniDbRun* pRun = UniDbRun::GetRun(period_number, iRunNumber);
        if  (pRun == NULL)
        {
            cout<<"ERROR: no Run found in DB: "<<iRunNumber<<endl;
            continue;
        }

        if (pRun->SetEnergy(&dEnergy) != 0)
            cout<<"ERROR: while writing Energy to DB for run: "<<iRunNumber<<endl;
        if (pRun->SetBeamParticle((TString)strFirstParticle.c_str()) != 0)
            cout<<"ERROR: while writing beam particle to DB for run: "<<iRunNumber<<endl;
        if (!strSecondParticle.empty())
            if (pRun->SetTargetParticle(new TString(strSecondParticle.c_str())) != 0)
                cout<<"ERROR: while writing target to DB for run: "<<iRunNumber<<endl;

        updated_count++;
        delete pRun;
    }// run CSV file

    csvFile.close();
    return updated_count;
}

// -------------------------------------------------------------------
ClassImp(UniDbParser);
