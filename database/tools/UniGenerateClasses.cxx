// -------------------------------------------------------------------------
// -----                      UniGenerateClasses cxx file            -----
// -----                  Created 18/08/15 by K. Gertsenberger         -----
// -------------------------------------------------------------------------
#include "UniGenerateClasses.h"
#include "function_set.h"

#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TSQLColumnInfo.h"
#include "TSQLTableInfo.h"
#include "TList.h"
#include "TClass.h"
#include "TMethodCall.h"
#include "TSystem.h"

#include "json.hpp"
#include <fstream>
#include <iostream>
using namespace std;
using json = nlohmann::json;

// -----   Constructor   -------------------------------
UniGenerateClasses::UniGenerateClasses()
{
    /*arrTableJoin = new TObjArray();

    structTableJoin* stTableJoin = new structTableJoin();
    stTableJoin->strSourceTableName = "detector_parameter";
    stTableJoin->strJoinTableName = "parameter_";

    stTableJoin->strJoinField.strColumnName = "parameter_id";
    stTableJoin->strJoinField.strColumnPointer = "parameter_id";
    stTableJoin->strJoinField.strColumnNameSpace = "parameter id";
    stTableJoin->strJoinField.strVariableType = "int";
    stTableJoin->strJoinField.strVariableTypePointer = "int";
    stTableJoin->strJoinField.strStatementType = "Int";
    stTableJoin->strJoinField.strPrintfType = "d";
    stTableJoin->strJoinField.strVariableName = "i_parameter_id";
    stTableJoin->strJoinField.strTempVariableName = "tmp__parameter_id";
    stTableJoin->strJoinField.strShortVariableName = "ParameterId";
    stTableJoin->strJoinField.isIdentity = false;
    stTableJoin->strJoinField.isPrimary = false;
    stTableJoin->strJoinField.isCompositeKey = false;
    stTableJoin->strJoinField.isUnique = false;
    stTableJoin->strJoinField.isNullable = false;
    stTableJoin->strJoinField.isBinary = false;
    stTableJoin->strJoinField.isDateTime = false;
    stTableJoin->strJoinField.isTimeStamp = false;

    structColumnInfo* pNameColumn = new structColumnInfo();
    pNameColumn->strColumnName = "parameter_name";
    pNameColumn->strColumnPointer = "parameter_name";
    pNameColumn->strColumnNameSpace = "parameter name";
    pNameColumn->strVariableType = "TString";
    pNameColumn->strVariableTypePointer = "TString";
    pNameColumn->strStatementType = "String";
    pNameColumn->strPrintfType = "s";
    pNameColumn->strVariableName = "str_parameter_name";
    pNameColumn->strTempVariableName = "tmp__parameter_name";
    pNameColumn->strShortVariableName = "ParameterName";
    pNameColumn->isIdentity = false;
    pNameColumn->isPrimary = false;
    pNameColumn->isCompositeKey = false;
    pNameColumn->isUnique = false;
    pNameColumn->isNullable = false;
    pNameColumn->isBinary = false;
    pNameColumn->isDateTime = false;
    pNameColumn->isTimeStamp = false;
    stTableJoin->arrManualFieldNames->Add((TObject*)pNameColumn);

    structColumnInfo* pTypeColumn = new structColumnInfo();
    pTypeColumn->strColumnName = "parameter_type";
    pTypeColumn->strColumnPointer = "parameter_type";
    pTypeColumn->strColumnNameSpace = "parameter type";
    pTypeColumn->strVariableType = "int";
    pTypeColumn->strVariableTypePointer = "int";
    pTypeColumn->strStatementType = "Int";
    pTypeColumn->strPrintfType = "d";
    pTypeColumn->strVariableName = "i_parameter_type";
    pTypeColumn->strTempVariableName = "tmp__parameter_type";
    pTypeColumn->strShortVariableName = "ParameterType";
    pTypeColumn->isIdentity = false;
    pTypeColumn->isPrimary = false;
    pTypeColumn->isCompositeKey = false;
    pTypeColumn->isUnique = false;
    pTypeColumn->isNullable = false;
    pTypeColumn->isBinary = false;
    pTypeColumn->isDateTime = false;
    pTypeColumn->isTimeStamp = false;
    stTableJoin->arrManualFieldNames->Add((TObject*)pTypeColumn);

    arrTableJoin->Add((TObject*)stTableJoin);

    stTableJoin->arrManualFieldNames->SetOwner(kTRUE);
    arrTableJoin->SetOwner(kTRUE);*/
}

// -----   Destructor   -------------------------------
UniGenerateClasses::~UniGenerateClasses()
{
}

// -----  Generate C++ classess - wrappers for database tables  -------------------------------
int UniGenerateClasses::GenerateClasses(TString json_configuration_file)
{
    TObjArray* arrTableJoin = nullptr;

    bool isOnlyUpdate = true;
    TString strConnectionName = "", strClassPrefix = "", strClassDirectory = "";

    string file_path = gSystem->ExpandPathName(json_configuration_file.Data());
    ifstream file(file_path);
    if (file.is_open())
    {
        json data;
        file >> data;

        try
        {
            strConnectionName = ((string)data["settings"]["connectionName"]).c_str();
            strClassPrefix = ((string)data["settings"]["classPrefix"]).c_str();
        }
        catch(const json::type_error &e)
        {
            cout<<"ERROR: Required settings were not found in the JSON file: settings->connectionName and settings->classPrefix"<<endl;
            file.close();
            return -1;
        }

        try
        {
            isOnlyUpdate = data["settings"]["onlyUpdateClasses"];
        }
        catch(const json::type_error &e)
        {
            isOnlyUpdate = true;
        }

        try
        {
            strClassDirectory = ((string)data["settings"]["classDirectory"]).c_str();
            if ((strClassDirectory != "") && (strClassDirectory[strClassDirectory.Length()-1] != '/')) strClassDirectory += '/';
            strClassDirectory = gSystem->ExpandPathName(strClassDirectory.Data());
        }
        catch(const json::type_error &e)
        {
            strClassDirectory = "";
        }
    }
    else
    {
        cout<<"ERROR: JSON file with configuration was not found: "<<file_path<<endl;
        return -2;
    }
    file.close();

    TClass* connection_class = TClass::GetClass(strConnectionName.Data());
    TMethodCall open_method(connection_class, "Open", "");
    Long_t method_result = 0;
    open_method.Execute(method_result);
    TObject* pConnectionObject = (TObject*)method_result;
    if (pConnectionObject == nullptr)
    {
        cout<<"ERROR: connection to the database was not established: "<<strConnectionName<<endl;
        return -3;
    }

    TMethodCall server_method(connection_class, "GetSQLServer", "");
    method_result = 0;
    server_method.Execute(pConnectionObject, "", method_result);
    TSQLServer* db_server = (TSQLServer*)method_result;
    if (db_server == nullptr)
    {
        cout<<"ERROR: pointer to database server was not obtained"<<endl;
        return -4;
    }

    // define DBMS: MySQL (0) or Postgres (1)
    int currentDBMS = -1;
    if (strcmp(db_server->GetDBMS(), "MySQL") == 0) currentDBMS = 0;
    else if (strcmp(db_server->GetDBMS(), "PgSQL") == 0) currentDBMS = 1;
    else
    {
        cout<<"ERROR: this type of DBMS is not supported: "<<db_server->GetDBMS()<<endl;
        return -5;
    }

    //int res_code = create_directory(strClassDir.Data());

    // get list of all database tables
    TList* lst = db_server->GetTablesList();
    TIter next(lst);
    TObject* obj;
    // cycle for all database tables
    TString sql;
    while (obj = next())
    {
        // define current table name
        TString strTableName = obj->GetName();

        // exclude system tables
        if ((strTableName.BeginsWith("_")) || ((currentDBMS == 1) && (strTableName.BeginsWith("pg_") || strTableName.BeginsWith("sql_"))))
            continue;

        cout<<"Parsing table: "<<strTableName<<endl;
        TSQLTableInfo* pTableInfo = db_server->GetTableInfo(strTableName.Data());

        // GET LIST OF COLUMNS FOR THE CURRENT TABLE (pTableInfo->GetColumns() doesn't provide required info)
        vector<structColumnInfo*> vecColumns;
        if (currentDBMS == 0)
        {
            sql = TString::Format("SELECT ordinal_position, column_name, data_type, (is_nullable = 'YES') AS is_nullable, "
                                  "(extra = 'auto_increment') AS is_identity, (column_key = 'PRI') AS is_primary, (column_key = 'UNI') AS is_unique "
                                  "FROM INFORMATION_SCHEMA.COLUMNS "
                                  "WHERE table_name = '%s' "
                                  "ORDER BY ordinal_position", strTableName.Data());
        }
        else if (currentDBMS == 1)
        {
            sql = TString::Format("SELECT DISTINCT a.attnum as ordinal_position, a.attname as column_name, format_type(a.atttypid, a.atttypmod) as data_type, "
                                  "a.attnotnull as is_nullable, pg_get_expr(def.adbin, def.adrelid) as is_default, coalesce(i.indisprimary, false) as is_primary, coalesce(i.indisunique, false) as is_unique, atttypmod as type_parameter "
                                  "FROM pg_attribute a JOIN pg_class pgc ON pgc.oid = a.attrelid "
                                  "LEFT JOIN pg_index i ON (pgc.oid = i.indrelid AND a.attnum = ANY(i.indkey)) "
                                  "LEFT JOIN pg_description com on (pgc.oid = com.objoid AND a.attnum = com.objsubid) "
                                  "LEFT JOIN pg_attrdef def ON (a.attrelid = def.adrelid AND a.attnum = def.adnum) "
                                  "WHERE a.attnum > 0 AND pgc.oid = a.attrelid AND pg_table_is_visible(pgc.oid) "
                                  "AND NOT a.attisdropped AND pgc.relname = '%s' "
                                  "ORDER BY a.attnum;", strTableName.Data());
        }

        TSQLResult* res = db_server->Query(sql);
        int nrows = res->GetRowCount();
        if (nrows == 0)
        {
            cout<<"CRITICAL ERROR: table with no attributes (columns) was found: "<<strTableName<<endl;
            return -5;
        }

        // cycle all columns in the current table
        TSQLRow* row;
        while (row = res->Next())
        {
            structColumnInfo* sColumnInfo = new structColumnInfo();
            TString strColumnName = row->GetField(1);
            sColumnInfo->strColumnName = strColumnName;
            sColumnInfo->isBinary = false;
            sColumnInfo->isDateTime = false;
            sColumnInfo->isTimeStamp = false;

            // remove last '_' in the column name in case of presence
            TString strColumnNameWO = strColumnName;
            if (strColumnNameWO[strColumnNameWO.Length()-1] == '_')
                strColumnNameWO = strColumnNameWO.Remove(strColumnNameWO.Length()-1);

            TString strColumnNameSpace = strColumnNameWO;
            Ssiz_t char_under;
            while ((char_under = strColumnNameSpace.First('_')) != kNPOS)
            {
                strColumnNameSpace = strColumnNameSpace.Replace(char_under, 1, ' ');
            }
            sColumnInfo->strColumnNameSpace = strColumnNameSpace;

            // define column properties depended on column type
            TSQLColumnInfo* pColumnInfo = pTableInfo->FindColumn(strColumnName);
            switch (pColumnInfo->GetSQLType())
            {
                case TSQLServer::kSQL_CHAR:
                {
                    sColumnInfo->strVariableType = "TString";
                    sColumnInfo->strStatementType = "String";
                    sColumnInfo->strPrintfType = "s";
                    sColumnInfo->strVariableName = "chr_"+strColumnNameWO;

                    break;
                }
                case TSQLServer::kSQL_VARCHAR:
                {
                    sColumnInfo->strVariableType = "TString";
                    sColumnInfo->strStatementType = "String";
                    sColumnInfo->strPrintfType = "s";
                    sColumnInfo->strVariableName = "str_"+strColumnNameWO;

                    break;
                }
                case TSQLServer::kSQL_INTEGER:
                {
                    TString strDataType = row->GetField(2);
                    if (strDataType == "uint")
                    {
                        sColumnInfo->strVariableType = "unsigned int";
                        sColumnInfo->strStatementType = "UInt";
                        sColumnInfo->strPrintfType = "u";
                        sColumnInfo->strVariableName = "ui_"+strColumnNameWO;
                    }
                    else
                    {
                        if (strDataType.BeginsWith("bool"))
                        {
                            sColumnInfo->strVariableType = "bool";
                            sColumnInfo->strStatementType = "Int";
                            sColumnInfo->strPrintfType = "d";
                            sColumnInfo->strVariableName = "b_"+strColumnNameWO;
                        }
                        else
                        {
                            if (strDataType.BeginsWith("bigint"))
                            {
                                sColumnInfo->strVariableType = "int64_t";
                                sColumnInfo->strStatementType = "Long64";
                                sColumnInfo->strPrintfType = "ld";
                                sColumnInfo->strVariableName = "i64_"+strColumnNameWO;
                            }
                            else
                            {
                                sColumnInfo->strVariableType = "int";
                                sColumnInfo->strStatementType = "Int";
                                sColumnInfo->strPrintfType = "d";
                                sColumnInfo->strVariableName = "i_"+strColumnNameWO;
                            }
                        }
                    }

                    break;
                }
                case TSQLServer::kSQL_FLOAT:
                {
                    sColumnInfo->strVariableType = "float";
                    sColumnInfo->strStatementType = "Double";
                    sColumnInfo->strPrintfType = "f";
                    sColumnInfo->strVariableName = "f_"+strColumnNameWO;

                    break;
                }
                case TSQLServer::kSQL_DOUBLE:
                {
                    sColumnInfo->strVariableType = "double";
                    sColumnInfo->strStatementType = "Double";
                    sColumnInfo->strPrintfType = "f";
                    sColumnInfo->strVariableName = "d_"+strColumnNameWO;

                    break;
                }
                case TSQLServer::kSQL_BINARY:
                {
                    sColumnInfo->strVariableType = "unsigned char";
                    sColumnInfo->strStatementType = "Binary";   //LargeObject (old OID version)
                    sColumnInfo->strPrintfType = "p";
                    sColumnInfo->strVariableName = "blob_"+strColumnNameWO;
                    sColumnInfo->isBinary = true;

                    break;
                }
                case TSQLServer::kSQL_TIMESTAMP:
                {
                    //cout<<"TimeStamp. Size: "<<pColumnInfo->GetSize()<<". Length: "<<pColumnInfo->GetSize()<<". Scale: "<<pColumnInfo->GetScale()<<endl;
                    if ((row->GetField(7))[0] == '0')
                    {
                        sColumnInfo->strVariableType = "TDatime";
                        sColumnInfo->strStatementType = "Datime";
                        sColumnInfo->strPrintfType = "s";
                        sColumnInfo->strVariableName = "dt_"+strColumnNameWO;
                        sColumnInfo->isDateTime = true;
                    }
                    else
                    {
                        //sColumnInfo->strVariableType = "TTimeStamp";
                        //sColumnInfo->strStatementType = "Timestamp";
                        sColumnInfo->strVariableType = "TDatime";
                        sColumnInfo->strStatementType = "Datime";
                        sColumnInfo->strPrintfType = "s";
                        sColumnInfo->strVariableName = "ts_"+strColumnNameWO;
                        //sColumnInfo->isTimeStamp = true;
                        sColumnInfo->isDateTime = true;
                    }

                    break;
                }
                default:
                {
                    TString strDataType = row->GetField(2);
                    if (strDataType == "bit")
                    {
                        sColumnInfo->strVariableType = "bool";
                        sColumnInfo->strStatementType = "Int";
                        sColumnInfo->strPrintfType = "d";
                        sColumnInfo->strVariableName = "b_"+strColumnNameWO;
                    }
                    else
                    {
                        if (strDataType == "datetime")
                        {
                            sColumnInfo->strVariableType = "TDatime";
                            sColumnInfo->strStatementType = "Datime";
                            sColumnInfo->strPrintfType = "s";
                            sColumnInfo->strVariableName = "dt_"+strColumnNameWO;
                            sColumnInfo->isDateTime = true;
                        }
                        else
                        {
                            if (strDataType.BeginsWith("character"))
                            {
                                sColumnInfo->strVariableType = "TString";
                                sColumnInfo->strStatementType = "String";
                                sColumnInfo->strPrintfType = "s";
                                sColumnInfo->strVariableName = "chr_"+strColumnNameWO;
                            }
                            else
                            {
                                cout<<"ERROR: no corresponding column type: "<<row->GetField(2)<<". SQLType: "<<pColumnInfo->GetSQLType()<<endl;
                                return -6;
                            }
                        }
                    }
                }
            }// switch (pColumnInfo->GetSQLType())

            // form short variable name (e.g. ComponentName for 'component_name' column)
            TString strShortVar = strColumnNameWO;
            strShortVar = strShortVar.Replace(0, 1, toupper(strShortVar[0]));
            while ((char_under = strShortVar.First('_')) != kNPOS)
            {
                strShortVar = strShortVar.Remove(char_under,1);
                if (strShortVar.Length() > char_under)
                    strShortVar = strShortVar.Replace(char_under, 1, toupper(strShortVar[char_under]));
            }
            sColumnInfo->strShortVariableName = strShortVar;

            if (currentDBMS == 0)
            {
                sColumnInfo->isNullable = ((row->GetField(3))[0] == '1');
                sColumnInfo->isIdentity = ((row->GetField(4))[0] == '1');
                sColumnInfo->isPrimary = ((row->GetField(5))[0] == '1');
                sColumnInfo->isUnique = ((row->GetField(6))[0] == '1');
            }
            else if (currentDBMS == 1)
            {
                string def_value = row->GetField(4);
                sColumnInfo->isNullable = ((row->GetField(3))[0] == 'f');
                sColumnInfo->isIdentity = (def_value.find("nextval") == 0);
                sColumnInfo->isPrimary = ((row->GetField(5))[0] == 't');
                if (sColumnInfo->isPrimary)
                    sColumnInfo->isUnique = false;
                else
                    sColumnInfo->isUnique = ((row->GetField(6))[0] == 't');
            }

            sColumnInfo->strTempVariableName = "tmp_" + strColumnNameWO;
            sColumnInfo->strVariableTypePointer = sColumnInfo->strVariableType;
            if ((sColumnInfo->isNullable) || (sColumnInfo->isBinary))
            {
                sColumnInfo->strVariableType += "*";
                sColumnInfo->strColumnPointer = "*" + strColumnNameWO;
            }
            else
                sColumnInfo->strColumnPointer = strColumnNameWO;

            vecColumns.push_back(sColumnInfo);

            if (sColumnInfo->isBinary)
            {
                structColumnInfo* sColumnInfoBinary = new structColumnInfo();
                sColumnInfoBinary->strVariableName = "sz_" + strColumnNameWO;
                sColumnInfoBinary->strTempVariableName = "tmp_sz_" + strColumnNameWO;
                sColumnInfoBinary->strVariableType = "Long_t";
                sColumnInfoBinary->strColumnName = "size_" + strColumnNameWO;
                sColumnInfoBinary->strColumnNameSpace = "size of " + strColumnNameSpace;
                sColumnInfoBinary->strShortVariableName = sColumnInfo->strShortVariableName + "Size";
                sColumnInfoBinary->strStatementType = "";

                vecColumns.push_back(sColumnInfoBinary);
            }
        }// cycle for all columns of current table

        delete row;
        delete res;

        // if join with another table is required - get structure with additional info
        bool isJoin = false;
        TIter nextJoin(arrTableJoin);
        structTableJoin* curTableJoin;
        while (curTableJoin = (structTableJoin*) nextJoin())
        {
            if (curTableJoin->strSourceTableName == strTableName)
            {
                isJoin = true;
                break;
            }
        }

        // generating class name corresponding current table name
        TString strClassName = strTableName;
        strClassName = strClassName.Replace(0, 1, toupper(strClassName[0]));
        Ssiz_t char_under;
        while ((char_under = strClassName.First('_')) != kNPOS)
        {
            strClassName = strClassName.Remove(char_under,1);
            if (strClassName.Length() > char_under)
                strClassName = strClassName.Replace(char_under, 1, toupper(strClassName[char_under]));
        }
        TString strShortTableName = strClassName;
        strClassName = strClassPrefix + strClassName;

        TString strTableNameSpace = strTableName;
        if (strTableNameSpace[strTableNameSpace.Length()-1] == '_')
            strTableNameSpace = strTableNameSpace.Remove(strTableNameSpace.Length()-1);
        while ((char_under = strTableNameSpace.First('_')) != kNPOS)
            strTableNameSpace = strTableNameSpace.Replace(char_under, 1, ' ');

        // CREATING OR CHANGING HEADER FILE
        TString strFileName = strClassDirectory + strClassName + ".h"; // set header file name
        // open and write to file
        ifstream oldFile;
        TString strTempFileName;
        ofstream hFile;
        if (isOnlyUpdate)
        {
            oldFile.open(strFileName, ios::in);
            if (!oldFile.is_open())
            {
                cout<<"ERROR: could not open existing header file: "<<strFileName<<endl;
                return -7;
            }

            strTempFileName = strFileName + "_tmp";
            hFile.open(strTempFileName, ios::out);
            if (!hFile.is_open())
            {
                cout<<"ERROR: could not create temporary header file: "<<strTempFileName<<endl;
                return -8;
            }

            string cur_line;
            while (getline(oldFile, cur_line))
            {
                string trim_line = trim(cur_line);
                if (trim_line.substr(0, 20) == "/* GENERATED PRIVATE")
                    break;
                else
                   hFile<<cur_line<<endl;
            }
        }
        else
        {
            hFile.open(strFileName, ios::out);
            if (!hFile.is_open())
            {
                cout<<"ERROR: could not create header file: "<<strFileName<<endl;
                return -9;
            }

            hFile<<"// ----------------------------------------------------------------------\n";
            hFile<<(TString::Format("//                    %s header file \n", strClassName.Data())).Data();
            hFile<<(TString::Format("//                      Generated %s \n", get_current_date().c_str())).Data();
            hFile<<"// ----------------------------------------------------------------------\n\n";

            hFile<<TString::Format("/** %s \n", strFileName.Data());
            hFile<<(TString::Format(" ** Class for the table: %s \n", strTableName.Data())).Data();
            hFile<<" **/ \n\n";

            TString strClassNameUpper = strClassName;
            strClassNameUpper.ToUpper();
            hFile<<(TString::Format("#ifndef %s_H \n", strClassNameUpper.Data())).Data();
            hFile<<(TString::Format("#define %s_H 1 \n\n", strClassNameUpper.Data())).Data();

            hFile<<"#include \"TString.h\"\n";
            hFile<<"#include \"TDatime.h\"\n";
            hFile<<"#include \"TTimeStamp.h\"\n";
            hFile<<"\n#include \""<<strConnectionName.Data()<<".h\"\n\n";

            hFile<<(TString::Format("class %s\n", strClassName.Data())).Data();
            hFile<<"{\n";
            hFile<<" private:\n";
        }

        hFile<<"    /* GENERATED PRIVATE MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */\n";

        hFile<<"    /// connection to the database\n";
        hFile<<"    "<<strConnectionName.Data()<<"* connectionDB;\n\n";

        // adding member variables corresponding table columns
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            hFile<<(TString::Format("    /// %s\n", cur_col->strColumnNameSpace.Data())).Data();
            hFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), cur_col->strVariableName.Data())).Data();
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                hFile<<(TString::Format("    /// Table: %s - column %s (read-only)\n", curTableJoin->strJoinTableName.Data(), cur_col->strColumnNameSpace.Data())).Data();
                hFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), cur_col->strVariableName.Data())).Data();
            }
        }

        // CONSTRUCTOR - DECLARATION
        hFile<<"\n    //Constructor\n";
        hFile<<(TString::Format("    %s(%s* db_connect", strClassName.Data(), strConnectionName.Data())).Data();
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
                hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
        }// for join table
        hFile<<");\n";
        hFile<<"    /* END OF PRIVATE GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */\n";

        if (isOnlyUpdate)
        {
            string cur_line;
            while (getline(oldFile, cur_line))
            {
                string trim_line = trim(cur_line);
                if (trim_line.substr(0, 27) == "/* END OF PRIVATE GENERATED")
                    break;
            }

            while (getline(oldFile, cur_line))
            {
                string trim_line = trim(cur_line);
                if (trim_line.substr(0, 19) == "/* GENERATED PUBLIC")
                    break;
                else
                   hFile<<cur_line<<endl;
            }
        }
        else
            hFile<<"\n public:\n";

        hFile<<"    /* GENERATED PUBLIC MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */\n";
        hFile<<(TString::Format("    virtual ~%s(); // Destructor\n\n", strClassName.Data())).Data();

        hFile<<"    // static class functions\n";
        // CREATE NEW RECORD - DECLARATION
        hFile<<(TString::Format("    /// add new %s to the database\n", strTableNameSpace.Data())).Data();
        hFile<<(TString::Format("    static %s* Create%s(", strClassName.Data(), strShortTableName.Data())).Data();
        int count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isIdentity)
                continue;

            if (count == 0)
                hFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        hFile<<");\n";

        // GET RECORD - DECLARATION
        hFile<<(TString::Format("    /// get %s from the database\n", strTableNameSpace.Data())).Data();
        hFile<<(TString::Format("    static %s* Get%s(", strClassName.Data(), strShortTableName.Data())).Data();
        count = 0;
        bool is_flag = false;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isUnique)
                is_flag = true;
            if (!cur_col->isPrimary)
                continue;

            if (count == 0)
                hFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        hFile<<");\n";

        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col= *it;
                if (!cur_col->isUnique)
                    continue;

                hFile<<(TString::Format("    /// get %s from the database\n", strTableNameSpace.Data())).Data();
                hFile<<(TString::Format("    static %s* Get%s(%s %s);\n", strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // CHECK RECORD EXISTS - DECLARATION
        hFile<<(TString::Format("    /// check %s exists in the database: 1- true, 0 - false, <0 - database operation error\n", strTableNameSpace.Data())).Data();
        hFile<<(TString::Format("    static int Check%sExists(", strShortTableName.Data())).Data();
        count = 0;
        is_flag = false;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isUnique)
                is_flag = true;
            if (!cur_col->isPrimary)
                continue;

            if (count == 0)
                hFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        hFile<<");\n";

        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col= *it;
                if (!cur_col->isUnique)
                    continue;

                hFile<<(TString::Format("    /// check %s exists in the database: 1- true, 0 - false, <0 - database operation error\n", strTableNameSpace.Data())).Data();
                hFile<<(TString::Format("    static int Check%sExists(%s %s);\n", strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // DELETE RECORD - DECLARATION
        hFile<<(TString::Format("    /// delete %s from the database\n", strTableNameSpace.Data())).Data();
        hFile<<(TString::Format("    static int Delete%s(", strShortTableName.Data())).Data();
        count = 0;
        is_flag = false;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isUnique)
                is_flag = true;
            if (!cur_col->isPrimary)
                continue;

            if (count == 0)
                hFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        hFile<<");\n";

        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col= *it;
                if (!cur_col->isUnique)
                    continue;

                hFile<<(TString::Format("    /// delete %s from the database\n", strTableNameSpace.Data())).Data();
                hFile<<(TString::Format("    static int Delete%s(%s %s);\n", strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // PRINT ALL ROWS -DECLARATION
        hFile<<(TString::Format("    /// print all %ss\n", strTableNameSpace.Data())).Data();
        hFile<<"    static int PrintAll();\n";

        // GETTERS FUNCTIONS - IMPLEMENTATIONS
        hFile<<"\n    // Getters\n";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            hFile<<(TString::Format("    /// get %s of the current %s\n", cur_col->strColumnNameSpace.Data(), strTableNameSpace.Data())).Data();
            hFile<<(TString::Format("    %s Get%s() {", cur_col->strVariableType.Data(), cur_col->strShortVariableName.Data())).Data();

            if (cur_col->isNullable)
            {
                hFile<<(TString::Format("if (%s == nullptr) return nullptr; else ", cur_col->strVariableName.Data())).Data();
                if (cur_col->isBinary)
                {
                    structColumnInfo* next_col = *(it+1);
                    hFile<<(TString::Format("{%s %s = new %s[%s]; ", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strVariableTypePointer.Data(), next_col->strVariableName.Data())).Data();
                    hFile<<(TString::Format("memcpy(%s, %s, %s); ", cur_col->strTempVariableName.Data(), cur_col->strVariableName.Data(), next_col->strVariableName.Data())).Data();
                    hFile<<(TString::Format("return %s;}", cur_col->strTempVariableName.Data())).Data();
                }
                else
                    hFile<<(TString::Format("return new %s(*%s);", cur_col->strVariableTypePointer.Data(), cur_col->strVariableName.Data())).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    structColumnInfo* next_col = *(it+1);
                    hFile<<(TString::Format("%s %s = new %s[%s]; ", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strVariableTypePointer.Data(), next_col->strVariableName.Data())).Data();
                    hFile<<(TString::Format("memcpy(%s, %s, %s); ", cur_col->strTempVariableName.Data(), cur_col->strVariableName.Data(), next_col->strVariableName.Data())).Data();
                    hFile<<(TString::Format("return %s;", cur_col->strTempVariableName.Data())).Data();
                }
                else
                    hFile<<(TString::Format("return %s;", cur_col->strVariableName.Data())).Data();
            }

            hFile<<"}\n";
        }
        // for join table
        if (isJoin)
        {
            structColumnInfo* cur_col;
            for (int i = 0; i < curTableJoin->arrManualFieldNames->GetEntriesFast(); i++)
            {
                cur_col = (structColumnInfo*) curTableJoin->arrManualFieldNames->At(i);

                hFile<<(TString::Format("    /// get %s of %s\n", cur_col->strColumnNameSpace.Data(), curTableJoin->strJoinTableName.Data())).Data();
                hFile<<(TString::Format("    %s Get%s() {", cur_col->strVariableType.Data(), cur_col->strShortVariableName.Data())).Data();

                if (cur_col->isNullable)
                {
                    hFile<<(TString::Format("if (%s == nullptr) return nullptr; else ", cur_col->strVariableName.Data())).Data();
                    if (cur_col->isBinary)
                    {
                        structColumnInfo* next_col = (structColumnInfo*) curTableJoin->arrManualFieldNames->At(i+1);
                        hFile<<(TString::Format("{%s %s = new %s[%s]; ", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strVariableTypePointer.Data(), next_col->strVariableName.Data())).Data();
                        hFile<<(TString::Format("memcpy(%s, %s, %s); ", cur_col->strTempVariableName.Data(), cur_col->strVariableName.Data(), next_col->strVariableName.Data())).Data();
                        hFile<<(TString::Format("return %s;}", cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                        hFile<<(TString::Format("return new %s(*%s);", cur_col->strVariableTypePointer.Data(), cur_col->strVariableName.Data())).Data();
                }
                else
                {
                    if (cur_col->isBinary)
                    {
                        structColumnInfo* next_col = (structColumnInfo*) curTableJoin->arrManualFieldNames->At(i+1);
                        hFile<<(TString::Format("%s %s = new %s[%s]; ", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strVariableTypePointer.Data(), next_col->strVariableName.Data())).Data();
                        hFile<<(TString::Format("memcpy(%s, %s, %s); ", cur_col->strTempVariableName.Data(), cur_col->strVariableName.Data(), next_col->strVariableName.Data())).Data();
                        hFile<<(TString::Format("return %s;", cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                        hFile<<(TString::Format("return %s;", cur_col->strVariableName.Data())).Data();
                }

                hFile<<"}\n";
            }
        }// for join table

        // SETTERS FUNCTIONS - DECLARATIONS
        hFile<<"\n    // Setters\n";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isIdentity)
                continue;

            if (!cur_col->isBinary)
            {
                hFile<<(TString::Format("    /// set %s of the current %s\n", cur_col->strColumnNameSpace.Data(), strTableNameSpace.Data())).Data();
                hFile<<(TString::Format("    int Set%s(%s %s);\n", cur_col->strShortVariableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
            else
            {
                hFile<<(TString::Format("    /// set %s of the current %s\n", cur_col->strColumnNameSpace.Data(), strTableNameSpace.Data())).Data();
                hFile<<(TString::Format("    int Set%s(%s %s, ", cur_col->strShortVariableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
                ++it;
                cur_col= *it;
                hFile<<(TString::Format("%s %s);\n", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // PRINT VALUES -DECLARATION
        hFile<<(TString::Format("\n    /// print information about current %s\n", strTableNameSpace.Data())).Data();
        hFile<<"    void Print();\n";
        hFile<<"    /* END OF PUBLIC GENERATED PART (SHOULD NOT BE CHANGED MANUALLY) */\n";

        if (isOnlyUpdate)
        {
            string cur_line;
            while (getline(oldFile, cur_line))
            {
                string trim_line = trim(cur_line);
                if (trim_line.substr(0, 26) == "/* END OF PUBLIC GENERATED")
                    break;
            }

            while (getline(oldFile, cur_line))
                   hFile<<cur_line<<endl;
        }
        else
        {
            hFile<<(TString::Format("\n ClassDef(%s,1);\n", strClassName.Data())).Data();
            hFile<<"};\n";
            hFile<<"\n#endif\n";
        }

        hFile.close();

        if (isOnlyUpdate)
        {
            oldFile.close();
            // delete the original file
            remove(strFileName);
            // rename temporary file to original
            rename(strTempFileName, strFileName);
        }

        // CREATING OR CHANGING CXX FILE
        strFileName = strClassDirectory + strClassName + ".cxx";
        // open and write to file
        ofstream cxxFile;
        if (isOnlyUpdate)
        {
            oldFile.open(strFileName, ios::in);
            if (!oldFile.is_open())
            {
                cout<<"ERROR: could not open existing cxx file: "<<strFileName<<endl;
                return -10;
            }

            strTempFileName = strFileName + "_tmp";
            cxxFile.open(strTempFileName, ios::out);
            if (!cxxFile.is_open())
            {
                cout<<"ERROR: could not create temporary cxx file: "<<strTempFileName<<endl;
                return -11;
            }

            string cur_line;
            while (getline(oldFile, cur_line))
            {
                string trim_line = trim(cur_line);
                if (trim_line.substr(0, 18) == "/* GENERATED CLASS")
                    break;
                else
                   cxxFile<<cur_line<<endl;
            }
        }
        else
        {
            cxxFile.open(strFileName, ios::out);
            if (!cxxFile.is_open())
            {
                cout<<"ERROR: could not create cxx file: "<<strFileName<<endl;
                return -12;
            }

            cxxFile<<"// ----------------------------------------------------------------------\n";
            cxxFile<<(TString::Format("//                    %s cxx file \n", strClassName.Data())).Data();
            cxxFile<<(TString::Format("//                      Generated %s \n", get_current_date().c_str())).Data();
            cxxFile<<"// ----------------------------------------------------------------------\n\n";

            cxxFile<<"#include \"TSQLServer.h\"\n";
            cxxFile<<"#include \"TSQLStatement.h\"\n";
            cxxFile<<(TString::Format("\n#include \"%s.h\"\n\n", strClassName.Data())).Data();

            cxxFile<<"#include <iostream>\n";
            cxxFile<<"using namespace std;\n\n";
        }

        cxxFile<<"/* GENERATED CLASS MEMBERS (SHOULD NOT BE CHANGED MANUALLY) */\n";

        // CONSTRUCTOR - IMPLEMENTATION
        cxxFile<<"// -----   Constructor with database connection   -----------------------\n";
        cxxFile<<(TString::Format("%s::%s(%s* db_connect", strClassName.Data(), strClassName.Data(), strConnectionName.Data())).Data();
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
                cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
        }// for join table
        cxxFile<<")\n{\n";

        cxxFile<<"    connectionDB = db_connect;\n\n";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            cxxFile<<(TString::Format("    %s = %s;\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
                cxxFile<<(TString::Format("    %s = %s;\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
        }// for join table
        cxxFile<<"}\n\n";

        // DESTRUCTOR - IMPLEMENTATION
        cxxFile<<"// -----   Destructor   -------------------------------------------------\n";
        cxxFile<<(TString::Format("%s::~%s()\n{\n    if (connectionDB)\n        delete connectionDB;\n", strClassName.Data(), strClassName.Data())).Data();

        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("    if (%s)\n        delete %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
            if (cur_col->isBinary)
                cxxFile<<(TString::Format("    if (%s)\n        delete [] %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                if (cur_col->isNullable)
                    cxxFile<<(TString::Format("    if (%s)\n        delete %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                if (cur_col->isBinary)
                    cxxFile<<(TString::Format("    if (%s)\n        delete [] %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
            }
        }// for join table
        cxxFile<<"}\n\n";

        // CREATE NEW RECORD - INPLEMENTATION
        cxxFile<<(TString::Format("// -----   Creating new %s in the database  ---------------------------\n", strTableNameSpace.Data())).Data();
        cxxFile<<(TString::Format("%s* %s::Create%s(", strClassName.Data(), strClassName.Data(), strShortTableName.Data())).Data();
        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isIdentity)
                continue;

            if (count == 0)
                cxxFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                if (cur_col->isIdentity)
                    continue;

                if (count == 0)
                    cxxFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

                count++;
            }
        }// for join table
        cxxFile<<")\n{\n";
        cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
        cxxFile<<"    if (connDb == nullptr) return nullptr;\n\n";

        cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"insert into %s(", strTableName.Data())).Data();
        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isIdentity)
                continue;

            if (count == 0)
                cxxFile<<(TString::Format("%s", cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();

            count++;
            if (cur_col->isBinary)
                ++it;
        }
        cxxFile<<") \"\n        \"values (";
        for (int i = 1 ; i <= count; i++)
        {
            if (currentDBMS == 0)
            {
                if (i == 1)
                    cxxFile<<"?";
                else
                    cxxFile<<", ?";
            }
            else if (currentDBMS == 1)
            {
                if (i == 1)
                    cxxFile<<"$1";
                else
                    cxxFile<<(TString::Format(", $%d", i)).Data();
            }

        }
        cxxFile<<")\");\n";

        cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n\n";

        cxxFile<<"    stmt->NextIteration();\n";
        count = 0;
        TString strIdentityColumnName = "";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isIdentity)
            {
                strIdentityColumnName = cur_col->strColumnName.Data();
                continue;
            }

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("    if (%s == nullptr)\n        stmt->SetNull(%d);\n    else\n    ", cur_col->strColumnName.Data(), count)).Data();

            if (!cur_col->isBinary)
                cxxFile<<(TString::Format("    stmt->Set%s(%d, %s);\n", cur_col->strStatementType.Data(), count, cur_col->strColumnPointer.Data())).Data();
            else
            {
                cxxFile<<(TString::Format("    stmt->Set%s(%d, %s, ", cur_col->strStatementType.Data(), count, cur_col->strColumnName.Data())).Data();
                ++it;
                cur_col= *it;
                cxxFile<<(TString::Format("%s, 0x4000000);\n", cur_col->strColumnName.Data())).Data();
            }

            count++;
        }

        cxxFile<<(TString::Format("\n    // inserting new %s to the Database\n", strTableNameSpace.Data())).Data();
        cxxFile<<"    if (!stmt->Process())\n    {\n";
        cxxFile<<(TString::Format("        cout<<\"ERROR: inserting new %s to the Database has been failed\"<<endl;\n", strTableNameSpace.Data())).Data();
        cxxFile<<"        delete stmt;\n        delete connDb;\n        return nullptr;\n    }\n\n";

        cxxFile<<"    delete stmt;\n\n";

        if (strIdentityColumnName != "")
        {
            cxxFile<<"    // getting last inserted ID\n";
            cxxFile<<(TString::Format("    int %s;\n", strIdentityColumnName.Data())).Data();
            if (currentDBMS == 0)
            {
                cxxFile<<"    TSQLStatement* stmt_last = db_server->Statement(\"SELECT LAST_INSERT_ID()\");\n";
            }
            else if (currentDBMS == 1)
            {
                cxxFile<<(TString::Format("    TSQLStatement* stmt_last = db_server->Statement(\"SELECT currval(pg_get_serial_sequence('%s','%s'))\");\n",
                                          strTableName.Data(), strIdentityColumnName.Data())).Data();
            }

            cxxFile<<"\n    // process getting last id\n"
                     "    if (stmt_last->Process())\n    {\n"
                     "        // store result of statement in buffer\n"
                     "        stmt_last->StoreResult();\n\n";
            cxxFile<<"        // if there is no last id then exit with error\n"
                     "        if (!stmt_last->NextResultRow())\n        {\n"
                     "            cout<<\"ERROR: no last ID in DB!\"<<endl;\n"
                     "            delete stmt_last;\n"
                     "            return nullptr;\n        }\n"
                    "        else\n        {\n";
            cxxFile<<(TString::Format("            %s = stmt_last->GetInt(0);\n", strIdentityColumnName.Data())).Data();
            cxxFile<<"            delete stmt_last;\n        }\n    }\n"
                     "    else\n    {\n"
                     "        cout<<\"ERROR: getting last ID has been failed!\"<<endl;\n"
                     "        delete stmt_last;\n"
                     "        return nullptr;\n    }\n\n";
        }

        // for join table
        if (isJoin)
        {
            cxxFile<<"    sql = TString::Format(\n        \"select";
            count = 0;
            TIter nextSel(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextSel())
            {
                if (count == 0)
                    cxxFile<<(TString::Format(" %s", cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();

                count++;
                if (cur_col->isBinary)
                    nextSel();
            }
            cxxFile<<(TString::Format(" \"\n        \"from %s \"\n        \"where", curTableJoin->strJoinTableName.Data())).Data();
            if (curTableJoin->strJoinField.strStatementType == "String")
                cxxFile<<(TString::Format(" lower(%s) = lower('%%%s')", curTableJoin->strJoinField.strColumnName.Data(), curTableJoin->strJoinField.strPrintfType.Data())).Data();
            else
                cxxFile<<(TString::Format(" %s = %%%s", curTableJoin->strJoinField.strColumnName.Data(), curTableJoin->strJoinField.strPrintfType.Data())).Data();
            cxxFile<<"\"";
            if (cur_col->strStatementType == "String")
                cxxFile<<(TString::Format(", %s.Data()", curTableJoin->strJoinField.strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s", curTableJoin->strJoinField.strColumnName.Data())).Data();
            cxxFile<<");\n";

            cxxFile<<"    stmt = db_server->Statement(sql);\n";

            cxxFile<<"\n    // get join table record from DB\n";
            cxxFile<<"    if (!stmt->Process())\n    {\n";
            cxxFile<<(TString::Format("        cout<<\"ERROR: getting join record from DB has been failed for '%s' table\"<<endl;\n\n", curTableJoin->strJoinTableName.Data())).Data();
            cxxFile<<"        delete stmt;\n";
            cxxFile<<"        delete connDb;\n";
            cxxFile<<"        return nullptr;\n    }\n\n";

            cxxFile<<"    // store result of statement in buffer\n";
            cxxFile<<"    stmt->StoreResult();\n\n";

            cxxFile<<"    // extract join row\n";
            cxxFile<<"    if (!stmt->NextResultRow())\n    {\n";
            cxxFile<<(TString::Format("        cout<<\"ERROR: the record was not found in '%s' table\"<<endl;\n\n", curTableJoin->strJoinTableName.Data())).Data();
            cxxFile<<"        delete stmt;\n";
            cxxFile<<"        delete connDb;\n";

            count = 0;
            TIter nextGet(curTableJoin->arrManualFieldNames);
            while (cur_col = (structColumnInfo*) nextGet())
            {
                TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
                cxxFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

                if (cur_col->isNullable)
                {
                    cxxFile<<(TString::Format("    if (stmt->IsNull(%d)) %s = nullptr;\n    else\n    ", count, TempVar.Data())).Data();
                    if (cur_col->isBinary)
                    {
                        cxxFile<<(TString::Format("    {\n        %s = nullptr;\n", TempVar.Data())).Data();
                        cur_col= (structColumnInfo*) nextGet();
                        cxxFile<<(TString::Format("        %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n    }\n", StatementType.Data(), count,
                                                   TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                        cxxFile<<(TString::Format("    %s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(), StatementType.Data(), count)).Data();
                }
                else
                {
                    if (cur_col->isBinary)
                    {
                        cxxFile<<(TString::Format("    %s = nullptr;\n", TempVar.Data())).Data();
                        cur_col= (structColumnInfo*) nextGet();
                        cxxFile<<(TString::Format("    %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("    stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                        cxxFile<<(TString::Format("    %s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
                }

                count++;
            }
            cxxFile<<"\n    delete stmt;\n\n";

        }// for join table

        // write to temporary variable to create separate memory copy of original variables
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            TString TempColumnName = cur_col->strColumnName, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
            cxxFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("    if (%s == nullptr) %s = nullptr;\n    else\n    ", cur_col->strColumnName.Data(), TempVar.Data())).Data();
                if (cur_col->isBinary)
                {
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("    {\n        %s %s = %s;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("        %s = new %s[%s];\n", TempVar.Data(), VariableTypePointer.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("        memcpy(%s, %s, %s);\n    }\n", TempVar.Data(), TempColumnName.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("    %s = new %s(*%s);\n", TempVar.Data(), VariableTypePointer.Data(), cur_col->strColumnName.Data())).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("    %s %s = %s;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("    %s = new %s[%s];\n", TempVar.Data(), VariableTypePointer.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("    memcpy(%s, %s, %s);\n", TempVar.Data(), TempColumnName.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("    %s = %s;\n", TempVar.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        cxxFile<<(TString::Format("\n    return new %s(connDb", strClassName.Data())).Data();
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
            }
        }// for join table
        cxxFile<<");\n}\n\n";

        // GET RECORD - IMPLEMENTATION
        cxxFile<<(TString::Format("// -----  Get %s from the database  ---------------------------\n", strTableNameSpace.Data())).Data();
        cxxFile<<(TString::Format("%s* %s::Get%s(", strClassName.Data(), strClassName.Data(), strShortTableName.Data())).Data();
        count = 0;
        is_flag = false;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isUnique)
                is_flag = true;
            if (!cur_col->isPrimary)
                continue;

            if (count == 0)
                cxxFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        cxxFile<<")\n{\n";

        cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
        cxxFile<<"    if (connDb == nullptr) return nullptr;\n\n";

        cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
        cxxFile<<"    TString sql = TString::Format(\n        \"select";
        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (count == 0)
                cxxFile<<(TString::Format(" %s", cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();

            count++;
            if (cur_col->isBinary)
                ++it;
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                if (count == 0)
                    cxxFile<<(TString::Format(" %s", cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();

                count++;
                if (cur_col->isBinary)
                    nextCol();
            }
        }// for join table
        cxxFile<<(TString::Format(" \"\n        \"from %s", strTableName.Data())).Data();
        // for join table
        if (isJoin)
        {
            cxxFile<<(TString::Format(" join %s on %s.%s=%s.%s", curTableJoin->strJoinTableName.Data(), strTableName.Data(), curTableJoin->strJoinField.strColumnName.Data(),
                                      curTableJoin->strJoinTableName.Data(), curTableJoin->strJoinField.strColumnName.Data())).Data();
        }// for join table
        cxxFile<<" \"\n        \"where";
        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            if (count > 0)
                cxxFile<<" and";


            if (cur_col->strStatementType == "String")
                cxxFile<<(TString::Format(" lower(%s) = lower('%%%s')", cur_col->strColumnName.Data(), cur_col->strPrintfType.Data())).Data();
            else
                cxxFile<<(TString::Format(" %s = %%%s", cur_col->strColumnName.Data(), cur_col->strPrintfType.Data())).Data();

            count++;
        }

        cxxFile<<"\"";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            if (cur_col->strStatementType == "String")
                cxxFile<<(TString::Format(", %s.Data()", cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();
        }
        cxxFile<<");\n";

        cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n";

        cxxFile<<(TString::Format("\n    // get %s from the database\n", strTableNameSpace.Data())).Data();
        cxxFile<<"    if (!stmt->Process())\n    {\n";
        cxxFile<<(TString::Format("        cout<<\"ERROR: getting %s from the database has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
        cxxFile<<"        delete stmt;\n";
        cxxFile<<"        delete connDb;\n";
        cxxFile<<"        return nullptr;\n    }\n\n";

        cxxFile<<"    // store result of statement in buffer\n";
        cxxFile<<"    stmt->StoreResult();\n\n";

        cxxFile<<"    // extract row\n";
        cxxFile<<"    if (!stmt->NextResultRow())\n    {\n";
        cxxFile<<(TString::Format("        cout<<\"ERROR: %s was not found in the database\"<<endl;\n\n", strTableNameSpace.Data())).Data();
        cxxFile<<"        delete stmt;\n";
        cxxFile<<"        delete connDb;\n";
        cxxFile<<"        return nullptr;\n    }\n\n";

        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
            cxxFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("    if (stmt->IsNull(%d)) %s = nullptr;\n    else\n    ", count, TempVar.Data())).Data();
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("    {\n        %s = nullptr;\n", TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("        %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n    }\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("    %s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(), StatementType.Data(), count)).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("    %s = nullptr;\n", TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("    %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("    stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                           TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("    %s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
            }

            count++;
        }

        // for join table
        if (isJoin)
        {
            TIter nextGet(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextGet())
            {
                TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
                cxxFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

                if (cur_col->isNullable)
                {
                    cxxFile<<(TString::Format("    if (stmt->IsNull(%d)) %s = nullptr;\n    else\n    ", count, TempVar.Data())).Data();
                    if (cur_col->isBinary)
                    {
                        cxxFile<<(TString::Format("    {\n        %s = nullptr;\n", TempVar.Data())).Data();
                        cur_col= (structColumnInfo*) nextGet();
                        cxxFile<<(TString::Format("        %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n    }\n", StatementType.Data(), count,
                                                   TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                        cxxFile<<(TString::Format("    %s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(), StatementType.Data(), count)).Data();
                }
                else
                {
                    if (cur_col->isBinary)
                    {
                        cxxFile<<(TString::Format("    %s = nullptr;\n", TempVar.Data())).Data();
                        cur_col= (structColumnInfo*) nextGet();
                        cxxFile<<(TString::Format("    %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("    stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                        cxxFile<<(TString::Format("    %s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
                }

                count++;
            }

        }// for join table

        cxxFile<<"\n    delete stmt;\n\n";

        cxxFile<<(TString::Format("    return new %s(connDb", strClassName.Data())).Data();
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
            if (cur_col->isBinary)
            {
                ++it;
                cur_col= *it;
                cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
            }
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
                if (cur_col->isBinary)
                {
                    cur_col= (structColumnInfo*) nextCol();
                    cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
                }
            }
        }// for join table
        cxxFile<<");\n}\n\n";

        // static get functions for unique fields
        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col = *it;
                if (!cur_col->isUnique)
                    continue;

                cxxFile<<(TString::Format("// -----  Get %s from the database by unique key  --------------\n", strTableNameSpace.Data())).Data();
                cxxFile<<(TString::Format("%s* %s::Get%s(%s %s)\n{\n" ,
                                          strClassName.Data(), strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
                cxxFile<<"    if (connDb == nullptr) return nullptr;\n\n";

                cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
                cxxFile<<"    TString sql = TString::Format(\n        \"select";
                count = 0;
                for (vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
                {
                    structColumnInfo* current_col= *it_inner;
                    if (count == 0)
                        cxxFile<<(TString::Format(" %s", current_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format(", %s", current_col->strColumnName.Data())).Data();

                    count++;
                    if (current_col->isBinary)
                        ++it_inner;
                }

                cxxFile<<(TString::Format(" \"\n        \"from %s", strTableName.Data())).Data();
                // for join table
                if (isJoin)
                {
                    cxxFile<<(TString::Format(" join %s on %s.%s=%s.%s", curTableJoin->strJoinTableName.Data(), strTableName.Data(), curTableJoin->strJoinField.strColumnName.Data(),
                                              curTableJoin->strJoinTableName.Data(), curTableJoin->strJoinField.strColumnName.Data())).Data();
                }// for join table
                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" \"\n        \"where lower(%s) = lower('%%%s')\", %s.Data());\n",
                                              cur_col->strColumnName.Data(), cur_col->strPrintfType.Data(), cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(" \"\n        \"where %s = %%%s\", %s);\n",
                                              cur_col->strColumnName.Data(), cur_col->strPrintfType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n";

                cxxFile<<(TString::Format("\n    // get %s from the database\n", strTableNameSpace.Data())).Data();
                cxxFile<<"    if (!stmt->Process())\n    {\n";
                cxxFile<<(TString::Format("        cout<<\"ERROR: getting %s from the database has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
                cxxFile<<"        delete stmt;\n";
                cxxFile<<"        delete connDb;\n";
                cxxFile<<"        return nullptr;\n    }\n\n";

                cxxFile<<"    // store result of statement in buffer\n";
                cxxFile<<"    stmt->StoreResult();\n\n";

                cxxFile<<"    // extract row\n";
                cxxFile<<"    if (!stmt->NextResultRow())\n    {\n";
                cxxFile<<(TString::Format("        cout<<\"ERROR: %s was not found in the database\"<<endl;\n\n", strTableNameSpace.Data())).Data();
                cxxFile<<"        delete stmt;\n";
                cxxFile<<"        delete connDb;\n";
                cxxFile<<"        return nullptr;\n    }\n\n";

                count = 0;
                for (vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
                {
                    structColumnInfo* current_col= *it_inner;

                    TString StatementType = current_col->strStatementType, TempVar = current_col->strTempVariableName,
                            VariableTypePointer = current_col->strVariableTypePointer;
                    cxxFile<<(TString::Format("    %s %s;\n", current_col->strVariableType.Data(), TempVar.Data())).Data();

                    if (current_col->isNullable)
                    {
                        cxxFile<<(TString::Format("    if (stmt->IsNull(%d)) %s = nullptr;\n    else\n    ", count, TempVar.Data())).Data();
                        if (current_col->isBinary)
                        {
                            cxxFile<<(TString::Format("    {\n        %s = nullptr;\n", TempVar.Data())).Data();
                            ++it_inner;
                            current_col= *it_inner;
                            cxxFile<<(TString::Format("        %s %s = 0;\n", current_col->strVariableType.Data(), current_col->strTempVariableName.Data())).Data();
                            cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n    }\n", StatementType.Data(), count,
                                                       TempVar.Data(), current_col->strTempVariableName.Data())).Data();
                        }
                        else
                            cxxFile<<(TString::Format("    %s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(),
                                                      StatementType.Data(), count)).Data();
                    }
                    else
                    {
                        if (current_col->isBinary)
                        {
                            cxxFile<<(TString::Format("    %s = nullptr;\n", TempVar.Data())).Data();
                            ++it_inner;
                            current_col= *it_inner;
                            cxxFile<<(TString::Format("    %s %s = 0;\n", current_col->strVariableType.Data(), current_col->strTempVariableName.Data())).Data();
                            cxxFile<<(TString::Format("    stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                                   TempVar.Data(), current_col->strTempVariableName.Data())).Data();
                        }
                        else
                            cxxFile<<(TString::Format("    %s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
                    }

                    count++;
                }

                // for join table
                if (isJoin)
                {
                    TIter nextGet(curTableJoin->arrManualFieldNames);
                    while (cur_col = (structColumnInfo*) nextGet())
                    {
                        TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
                        cxxFile<<(TString::Format("    %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

                        if (cur_col->isNullable)
                        {
                            cxxFile<<(TString::Format("    if (stmt->IsNull(%d)) %s = nullptr;\n    else\n    ", count, TempVar.Data())).Data();
                            if (cur_col->isBinary)
                            {
                                cxxFile<<(TString::Format("    {\n        %s = nullptr;\n", TempVar.Data())).Data();
                                cur_col= (structColumnInfo*) nextGet();
                                cxxFile<<(TString::Format("        %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                                cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n    }\n", StatementType.Data(), count,
                                                           TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                            }
                            else
                                cxxFile<<(TString::Format("    %s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(), StatementType.Data(), count)).Data();
                        }
                        else
                        {
                            if (cur_col->isBinary)
                            {
                                cxxFile<<(TString::Format("    %s = nullptr;\n", TempVar.Data())).Data();
                                cur_col= (structColumnInfo*) nextGet();
                                cxxFile<<(TString::Format("    %s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                                cxxFile<<(TString::Format("    stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                                       TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                            }
                            else
                                cxxFile<<(TString::Format("    %s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
                        }

                        count++;
                    }

                }// for join table

                cxxFile<<"\n    delete stmt;\n\n";

                cxxFile<<(TString::Format("    return new %s(connDb", strClassName.Data())).Data();
                for(vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
                {
                    structColumnInfo* current_col = *it_inner;

                    cxxFile<<(TString::Format(", %s", current_col->strTempVariableName.Data())).Data();
                    if (current_col->isBinary)
                    {
                        ++it_inner;
                        current_col= *it_inner;
                        cxxFile<<(TString::Format(", %s", current_col->strColumnName.Data())).Data();
                    }
                }
                // for join table
                if (isJoin)
                {
                    TIter nextCol(curTableJoin->arrManualFieldNames);
                    structColumnInfo* current_col;
                    while (current_col = (structColumnInfo*) nextCol())
                    {
                        cxxFile<<(TString::Format(", %s", current_col->strTempVariableName.Data())).Data();
                        if (current_col->isBinary)
                        {
                            current_col= (structColumnInfo*) nextCol();
                            cxxFile<<(TString::Format(", %s", current_col->strColumnName.Data())).Data();
                        }
                    }
                }// for join table
                cxxFile<<");\n}\n\n";
            }// for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        }// static get functions for unique fields

        // CHECK RECORD EXISTS - IMPLEMENTATION
        cxxFile<<(TString::Format("// -----  Check %s exists in the database  ---------------------------\n", strTableNameSpace.Data())).Data();
        cxxFile<<(TString::Format("int %s::Check%sExists(", strClassName.Data(), strShortTableName.Data())).Data();
        count = 0;
        is_flag = false;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isUnique)
                is_flag = true;
            if (!cur_col->isPrimary)
                continue;

            if (count == 0)
                cxxFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        cxxFile<<")\n{\n";

        cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
        cxxFile<<"    if (connDb == nullptr) return -1;\n\n";

        cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"select 1 \"\n        \"from %s", strTableName.Data())).Data();
        // for join table
        if (isJoin)
        {
            cxxFile<<(TString::Format(" join %s on %s.%s=%s.%s", curTableJoin->strJoinTableName.Data(), strTableName.Data(), curTableJoin->strJoinField.strColumnName.Data(),
                                      curTableJoin->strJoinTableName.Data(), curTableJoin->strJoinField.strColumnName.Data())).Data();
        }// for join table
        cxxFile<<" \"\n        \"where";
        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            if (count > 0)
                cxxFile<<" and";


            if (cur_col->strStatementType == "String")
                cxxFile<<(TString::Format(" lower(%s) = lower('%%%s')", cur_col->strColumnName.Data(), cur_col->strPrintfType.Data())).Data();
            else
                cxxFile<<(TString::Format(" %s = %%%s", cur_col->strColumnName.Data(), cur_col->strPrintfType.Data())).Data();

            count++;
        }

        cxxFile<<"\"";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            if (cur_col->strStatementType == "String")
                cxxFile<<(TString::Format(", %s.Data()", cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();
        }
        cxxFile<<");\n";

        cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n";

        cxxFile<<(TString::Format("\n    // get %s from the database\n", strTableNameSpace.Data())).Data();
        cxxFile<<"    if (!stmt->Process())\n    {\n";
        cxxFile<<(TString::Format("        cout<<\"ERROR: getting %s from the database has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
        cxxFile<<"        delete stmt;\n";
        cxxFile<<"        delete connDb;\n";
        cxxFile<<"        return -2;\n    }\n\n";

        cxxFile<<"    // store result of statement in buffer\n";
        cxxFile<<"    stmt->StoreResult();\n\n";

        cxxFile<<"    // extract row\n";
        cxxFile<<"    if (!stmt->NextResultRow())\n    {\n";
        cxxFile<<"        delete stmt;\n";
        cxxFile<<"        delete connDb;\n";
        cxxFile<<"        return 0;\n    }\n\n";

        cxxFile<<"    delete stmt;\n";
        cxxFile<<"    delete connDb;\n\n";
        cxxFile<<"    return 1;\n}\n\n";

        // static get functions for unique fields
        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col = *it;
                if (!cur_col->isUnique)
                    continue;

                cxxFile<<(TString::Format("// -----  Check %s exists in the database by unique key  --------------\n", strTableNameSpace.Data())).Data();
                cxxFile<<(TString::Format("int %s::Check%sExists(%s %s)\n{\n" ,
                                          strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
                cxxFile<<"    if (connDb == nullptr) return -1;\n\n";

                cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
                cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"select 1 \"\n        \"from %s", strTableName.Data())).Data();
                // for join table
                if (isJoin)
                {
                    cxxFile<<(TString::Format(" join %s on %s.%s=%s.%s", curTableJoin->strJoinTableName.Data(), strTableName.Data(), curTableJoin->strJoinField.strColumnName.Data(),
                                              curTableJoin->strJoinTableName.Data(), curTableJoin->strJoinField.strColumnName.Data())).Data();
                }// for join table
                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" \"\n        \"where lower(%s) = lower('%%%s')\", %s.Data());\n",
                                              cur_col->strColumnName.Data(), cur_col->strPrintfType.Data(), cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(" \"\n        \"where %s = %%%s\", %s);\n",
                                              cur_col->strColumnName.Data(), cur_col->strPrintfType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n";

                cxxFile<<(TString::Format("\n    // get %s from the database\n", strTableNameSpace.Data())).Data();
                cxxFile<<"    if (!stmt->Process())\n    {\n";
                cxxFile<<(TString::Format("        cout<<\"ERROR: getting %s from the database has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
                cxxFile<<"        delete stmt;\n";
                cxxFile<<"        delete connDb;\n";
                cxxFile<<"        return -2;\n    }\n\n";

                cxxFile<<"    // store result of statement in buffer\n";
                cxxFile<<"    stmt->StoreResult();\n\n";

                cxxFile<<"    // extract row\n";
                cxxFile<<"    if (!stmt->NextResultRow())\n    {\n";
                cxxFile<<"        delete stmt;\n";
                cxxFile<<"        delete connDb;\n";
                cxxFile<<"        return 0;\n    }\n\n";

                cxxFile<<"    delete stmt;\n";
                cxxFile<<"    delete connDb;\n\n";
                cxxFile<<"    return 1;\n}\n\n";
            }// for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        }// static get functions for Unique fields

        // DELETE RECORD - IMPLEMENTATION
        cxxFile<<(TString::Format("// -----  Delete %s from the database  ---------------------------\n", strTableNameSpace.Data())).Data();
        cxxFile<<(TString::Format("int %s::Delete%s(", strClassName.Data(), strShortTableName.Data())).Data();
        count = 0;
        is_flag = false;
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isUnique)
                is_flag = true;
            if (!cur_col->isPrimary)
                continue;

            if (count == 0)
                cxxFile<<(TString::Format("%s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

            count++;
        }
        cxxFile<<")\n{\n";

        cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
        cxxFile<<"    if (connDb == nullptr) return -1;\n\n";

        cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"delete from %s \"\n        \"where", strTableName.Data())).Data();
        count = 0;
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            if (count > 0)
                cxxFile<<" and";

            if (currentDBMS == 0)
            {
                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" lower(%s) = lower(?)", cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(" %s = ?", cur_col->strColumnName.Data())).Data();
            }
            else if (currentDBMS == 1)
            {
                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" lower(%s) = lower($%d)", cur_col->strColumnName.Data(), count+1)).Data();
                else
                    cxxFile<<(TString::Format(" %s = $%d", cur_col->strColumnName.Data(), count+1)).Data();
            }

            count++;
        }
        cxxFile<<"\");\n";

        cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n\n";

        cxxFile<<"    stmt->NextIteration();\n";
        count = 0;
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            cxxFile<<(TString::Format("    stmt->Set%s(%d, %s);\n", cur_col->strStatementType.Data(), count, cur_col->strColumnName.Data())).Data();
            count++;
        }

        cxxFile<<(TString::Format("\n    // delete %s from the dataBase\n", strTableNameSpace.Data())).Data();
        cxxFile<<"    if (!stmt->Process())\n    {\n";
        cxxFile<<(TString::Format("        cout<<\"ERROR: deleting %s from the dataBase has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
        cxxFile<<"        delete stmt;\n";
        cxxFile<<"        delete connDb;\n";
        cxxFile<<"        return -2;\n    }\n\n";

        cxxFile<<"    delete stmt;\n";
        cxxFile<<"    delete connDb;\n";
        cxxFile<<"    return 0;\n}\n\n";

        // static Delete functions for unique fields
        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col = *it;
                if (!cur_col->isUnique)
                    continue;

                cxxFile<<(TString::Format("// -----  Delete %s from the database by unique key  --------------\n", strTableNameSpace.Data())).Data();
                cxxFile<<(TString::Format("int %s::Delete%s(%s %s)\n{\n" ,
                                          strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
                cxxFile<<"    if (connDb == nullptr) return -1;\n\n";

                cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";

                if (currentDBMS == 0)
                {
                    if (cur_col->strStatementType == "String")
                        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"delete from %s \"\n        \"where lower(%s) = lower(?)\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"delete from %s \"\n        \"where %s = ?\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                }
                else if (currentDBMS == 1)
                {
                    if (cur_col->strStatementType == "String")
                        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"delete from %s \"\n        \"where lower(%s) = lower($1)\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"delete from %s \"\n        \"where %s = $1\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                }

                cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n\n";

                cxxFile<<"    stmt->NextIteration();\n";
                cxxFile<<(TString::Format("    stmt->Set%s(0, %s);\n", cur_col->strStatementType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<(TString::Format("\n    // delete %s from the dataBase\n", strTableNameSpace.Data())).Data();
                cxxFile<<"    if (!stmt->Process())\n    {\n";
                cxxFile<<(TString::Format("        cout<<\"ERROR: deleting %s from the DataBase has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
                cxxFile<<"        delete stmt;\n";
                cxxFile<<"        delete connDb;\n";
                cxxFile<<"        return -2;\n    }\n\n";

                cxxFile<<"    delete stmt;\n";
                cxxFile<<"    delete connDb;\n";
                cxxFile<<"    return 0;\n}\n\n";
            }// for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        }// static Delete functions for unique fields

        // PRINT ALL ROWS - IMPLEMENTATION
        cxxFile<<(TString::Format("// -----  Print all '%ss'  ---------------------------------\n", strTableNameSpace.Data())).Data();
        cxxFile<<(TString::Format("int %s::PrintAll()\n{\n", strClassName.Data())).Data();

        cxxFile<<(TString::Format("    %s* connDb = %s::Open();\n", strConnectionName.Data(), strConnectionName.Data())).Data();
        cxxFile<<"    if (connDb == nullptr) return -1;\n\n";

        cxxFile<<"    TSQLServer* db_server = connDb->GetSQLServer();\n\n";
        cxxFile<<"    TString sql = TString::Format(\n        \"select";
        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (count == 0)
                cxxFile<<(TString::Format(" %s", cur_col->strColumnName.Data())).Data();
            else
                cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();

            count++;
            if (cur_col->isBinary)
                ++it;
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                if (count == 0)
                    cxxFile<<(TString::Format(" %s", cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(", %s", cur_col->strColumnName.Data())).Data();

                count++;
                if (cur_col->isBinary)
                    nextCol();
            }
        }// for join table
        cxxFile<<(TString::Format(" \"\n        \"from %s\");\n", strTableName.Data())).Data();

        cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n";

        cxxFile<<(TString::Format("\n    // get all '%ss' from the database\n", strTableNameSpace.Data())).Data();
        cxxFile<<"    if (!stmt->Process())\n    {\n";
        cxxFile<<(TString::Format("        cout<<\"ERROR: getting all '%ss' from the dataBase has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
        cxxFile<<"        delete stmt;\n";
        cxxFile<<"        delete connDb;\n";
        cxxFile<<"        return -2;\n    }\n\n";

        cxxFile<<"    // store result of statement in buffer\n";
        cxxFile<<"    stmt->StoreResult();\n\n";

        cxxFile<<"    // print rows\n";
        cxxFile<<(TString::Format("    cout<<\"Table '%s':\"<<endl;\n", strTableName.Data())).Data();
        cxxFile<<"    while (stmt->NextResultRow())\n    {\n";

        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            cxxFile<<"        cout<<\"";
            if (count > 0) cxxFile<<", ";

            TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName;
            cxxFile<<(TString::Format("%s: \";\n", cur_col->strColumnName.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("        if (stmt->IsNull(%d)) cout<<\"nullptr\";\n        else\n    ", count)).Data();
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("        %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();
                    cxxFile<<(TString::Format("        {\n            %s = nullptr;\n", TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("            %s %s=0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("            stmt->Get%s(%d, (void*&)%s, %s);\n        }\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("            cout<<(void*)%s<<\", binary size: \"<<%s;\n", TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                {
                    cxxFile<<(TString::Format("        cout<<stmt->Get%s(%d)", StatementType.Data(), count)).Data();
                    if (cur_col->isDateTime)
                        cxxFile<<".AsSQLString()";
                    if (cur_col->isTimeStamp)
                        cxxFile<<".AsString(\"s\")";
                    cxxFile<<";\n";
                }
            }
            else
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("        %s %s = nullptr;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("        %s %s=0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("        cout<<(void*)%s<<\", binary size: \"<<%s;\n", TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                {
                    cxxFile<<(TString::Format("        cout<<(stmt->Get%s(%d))", StatementType.Data(), count)).Data();
                    if (cur_col->isDateTime)
                        cxxFile<<".AsSQLString()";
                    if (cur_col->isTimeStamp)
                        cxxFile<<".AsString(\"s\")";
                    cxxFile<<";\n";
                }
            }

            count++;
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                cxxFile<<"        cout<<\"";
                if (count > 0) cxxFile<<", ";

                TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName;
                cxxFile<<(TString::Format("%s: \";\n", cur_col->strColumnName.Data())).Data();

                if (cur_col->isNullable)
                {
                    cxxFile<<(TString::Format("        if (stmt->IsNull(%d)) cout<<\"nullptr\";\n        else\n    ", count)).Data();
                    if (cur_col->isBinary)
                    {
                        cxxFile<<(TString::Format("        %s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();
                        cxxFile<<(TString::Format("        {\n            %s = nullptr;\n", TempVar.Data())).Data();
                        cur_col= (structColumnInfo*) nextCol();
                        cxxFile<<(TString::Format("            %s %s=0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("            stmt->Get%s(%d, (void*&)%s, %s);\n        }\n", StatementType.Data(), count,
                                                   TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("            cout<<(void*)%s<<\", binary size: \"<<%s;\n", TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                    {
                        cxxFile<<(TString::Format("        cout<<stmt->Get%s(%d)", StatementType.Data(), count)).Data();
                        if (cur_col->isDateTime)
                            cxxFile<<".AsSQLString()";
                        if (cur_col->isTimeStamp)
                            cxxFile<<".AsString(\"s\")";
                        cxxFile<<";\n";
                    }
                }
                else
                {
                    if (cur_col->isBinary)
                    {
                        cxxFile<<(TString::Format("        %s %s = nullptr;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();
                        cur_col= (structColumnInfo*) nextCol();
                        cxxFile<<(TString::Format("        %s %s=0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("        stmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                                   TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                        cxxFile<<(TString::Format("        cout<<(void*)%s<<\", binary size: \"<<%s;\n", TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    }
                    else
                    {
                        cxxFile<<(TString::Format("        cout<<(stmt->Get%s(%d))", StatementType.Data(), count)).Data();
                        if (cur_col->isDateTime)
                            cxxFile<<".AsSQLString()";
                        if (cur_col->isTimeStamp)
                            cxxFile<<".AsString(\"s\")";
                        cxxFile<<";\n";
                    }
                }

                count++;
            }
        }// for join table

        cxxFile<<"        cout<<\".\"<<endl;\n    }\n\n";

        cxxFile<<"    delete stmt;\n";
        cxxFile<<"    delete connDb;\n\n";
        cxxFile<<"    return 0;\n}\n\n";

        // SETTERS FUNCTIONS - IMPLEMENTATION
        structColumnInfo* temp_col;
        cxxFile<<"\n// Setters functions\n";
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (cur_col->isIdentity)
                continue;

            if (!cur_col->isBinary)
                cxxFile<<(TString::Format("int %s::Set%s(%s %s)\n{\n", strClassName.Data(), cur_col->strShortVariableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
            {
                cxxFile<<(TString::Format("int %s::Set%s(%s %s, ", strClassName.Data(), cur_col->strShortVariableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
                ++it;
                temp_col = *it;
                cxxFile<<(TString::Format("%s %s)\n{\n", temp_col->strVariableType.Data(), temp_col->strColumnName.Data())).Data();
            }

            cxxFile<<"    if (!connectionDB)\n    {\n        cout<<\"CRITICAL ERROR: Connection object is null\"<<endl;\n        return -1;\n    }\n\n";

            cxxFile<<"    TSQLServer* db_server = connectionDB->GetSQLServer();\n\n";

            if (currentDBMS == 0)
            {
                cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"update %s \"\n        \"set %s = ? \"\n        \"where", strTableName.Data(), cur_col->strColumnName.Data())).Data();
            }
            else if (currentDBMS == 1)
            {
                cxxFile<<(TString::Format("    TString sql = TString::Format(\n        \"update %s \"\n        \"set %s = $1 \"\n        \"where", strTableName.Data(), cur_col->strColumnName.Data())).Data();
            }
            count = 0;
            for(vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
            {
                structColumnInfo* current_col= *it_inner;
                if (!current_col->isPrimary)
                    continue;

                if (currentDBMS == 0)
                {
                    if (count == 0)
                        cxxFile<<(TString::Format(" %s = ?", current_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format(" and %s = ?", current_col->strColumnName.Data())).Data();
                }
                else if (currentDBMS == 1)
                {
                    if (count == 0)
                        cxxFile<<(TString::Format(" %s = $2", current_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format(" and %s = $%d", current_col->strColumnName.Data(), count+2)).Data();
                }

                count++;
            }
            cxxFile<<"\");\n";

            cxxFile<<"    TSQLStatement* stmt = db_server->Statement(sql);\n\n";

            cxxFile<<"    stmt->NextIteration();\n";

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("    if (%s == nullptr)\n        stmt->SetNull(0);\n    else\n    ", cur_col->strColumnName.Data())).Data();

            if (!cur_col->isBinary)
                cxxFile<<(TString::Format("    stmt->Set%s(0, %s);\n", cur_col->strStatementType.Data(), cur_col->strColumnPointer.Data())).Data();
            else
            {
                cxxFile<<(TString::Format("    stmt->Set%s(0, %s, ", cur_col->strStatementType.Data(), cur_col->strColumnName.Data())).Data();
                cxxFile<<(TString::Format("%s, 0x4000000);\n", temp_col->strColumnName.Data())).Data();
            }

            count = 1;
            for(vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
            {
                structColumnInfo* current_col= *it_inner;
                if (!current_col->isPrimary)
                    continue;

                cxxFile<<(TString::Format("    stmt->Set%s(%d, %s);\n", current_col->strStatementType.Data(), count, current_col->strVariableName.Data())).Data();
                count++;
            }

            cxxFile<<"\n    // write new value to the database\n";
            cxxFile<<"    if (!stmt->Process())\n    {\n";
            cxxFile<<(TString::Format("        cout<<\"ERROR: updating information about %s has been failed\"<<endl;\n\n", strTableNameSpace.Data())).Data();
            cxxFile<<"        delete stmt;\n";
            cxxFile<<"        return -2;\n    }\n\n";

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("    if (%s)\n        delete %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
            if (cur_col->isBinary)
                cxxFile<<(TString::Format("    if (%s)\n        delete [] %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("    if (%s == nullptr) %s = nullptr;\n    else\n    ", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("    {\n        %s %s = %s;\n", temp_col->strVariableType.Data(), temp_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("        %s = new %s[%s];\n", cur_col->strVariableName.Data(), cur_col->strVariableTypePointer.Data(), temp_col->strVariableName.Data())).Data();
                    cxxFile<<(TString::Format("        memcpy(%s, %s, %s);\n    }\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data(), temp_col->strVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("    %s = new %s(*%s);\n", cur_col->strVariableName.Data(), cur_col->strVariableTypePointer.Data(), cur_col->strColumnName.Data())).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("    %s = %s;\n", temp_col->strVariableName.Data(), temp_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("    %s = new %s[%s];\n", cur_col->strVariableName.Data(), cur_col->strVariableTypePointer.Data(), temp_col->strVariableName.Data())).Data();
                    cxxFile<<(TString::Format("    memcpy(%s, %s, %s);\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data(), temp_col->strVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("    %s = %s;\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
            }

            cxxFile<<"\n    delete stmt;\n";
            cxxFile<<"    return 0;\n}\n\n";
        }// setters functions - implementation

        // PRINT VALUES - IMPLEMENTATION
        cxxFile<<(TString::Format("// -----  Print current %s  ---------------------------------------\n", strTableNameSpace.Data())).Data();
        cxxFile<<(TString::Format("void %s::Print()\n{\n", strClassName.Data())).Data();

        cxxFile<<(TString::Format("    cout<<\"Table '%s'\";\n    cout", strTableName.Data())).Data();
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            if (cur_col->isBinary)
            {
                cxxFile<<(TString::Format("<<\". %s: \"<<(void*)%s", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                ++it;
                cur_col= *it;
                cxxFile<<(TString::Format("<<\", binary size: \"<<%s", cur_col->strVariableName.Data())).Data();
            }
            else
            {
                if (cur_col->isNullable)
                {
                    if (cur_col->isDateTime)
                        cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": (*%s).AsSQLString())",
                                                  cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                    else
                    {
                        if (cur_col->isTimeStamp)
                            cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": (*%s).AsString(\"s\"))",
                                                      cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                        else
                        {
                            if (cur_col->strPrintfType == "s")
                                cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": *%s)",
                                                          cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                            else
                                cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": TString::Format(\"%%%s\", *%s))",
                                                          cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strPrintfType.Data(), cur_col->strVariableName.Data())).Data();
                        }


                    }
                }
                else
                {
                    cxxFile<<(TString::Format("<<\". %s: \"<<%s", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                    if (cur_col->isDateTime)
                        cxxFile<<".AsSQLString()";
                    if (cur_col->isTimeStamp)
                        cxxFile<<".AsString(\"s\")";
                }
            }
        }
        // for join table
        if (isJoin)
        {
            TIter nextCol(curTableJoin->arrManualFieldNames);
            structColumnInfo* cur_col;
            while (cur_col = (structColumnInfo*) nextCol())
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("<<\". %s: \"<<(void*)%s", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                    cur_col= (structColumnInfo*) nextCol();
                    cxxFile<<(TString::Format("<<\", binary size: \"<<%s", cur_col->strVariableName.Data())).Data();
                }
                else
                {
                    if (cur_col->isNullable)
                    {
                        if (cur_col->isDateTime)
                            cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": (*%s).AsSQLString())",
                                                      cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                        else
                        {
                            if (cur_col->isTimeStamp)
                                cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": (*%s).AsString(\"s\"))",
                                                          cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                            else
                            {
                                if (cur_col->strPrintfType == "s")
                                    cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": *%s)",
                                                              cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                                else
                                    cxxFile<<(TString::Format("<<\". %s: \"<<(%s == nullptr? \"nullptr\": TString::Format(\"%%%s\", *%s))",
                                                              cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strPrintfType.Data(), cur_col->strVariableName.Data())).Data();
                            }
                        }
                    }
                    else
                    {
                        cxxFile<<(TString::Format("<<\". %s: \"<<%s", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                        if (cur_col->isDateTime)
                            cxxFile<<".AsSQLString()";
                        if (cur_col->isTimeStamp)
                            cxxFile<<".AsString(\"s\")";
                    }
                }
            }
        }// for join table
        cxxFile<<"<<endl;\n\n";

        cxxFile<<"    return;\n}\n";
        cxxFile<<"/* END OF GENERATED CLASS PART (SHOULD NOT BE CHANGED MANUALLY) */\n";

        if (isOnlyUpdate)
        {
            string cur_line;
            while (getline(oldFile, cur_line))
            {
                string trim_line = trim(cur_line);
                if (trim_line.substr(0, 25) == "/* END OF GENERATED CLASS")
                    break;
            }

            while (getline(oldFile, cur_line))
                   cxxFile<<cur_line<<endl;
        }
        else
        {
            cxxFile<<"\n// -------------------------------------------------------------------\n";
            cxxFile<<(TString::Format("ClassImp(%s);\n", strClassName.Data())).Data();
        }

        cxxFile.close();

        if (isOnlyUpdate)
        {
            oldFile.close();
            // delete the original file
            remove(strFileName);
            // rename temporary file to original
            rename(strTempFileName, strFileName);
        }
    }// cycle for all database tables

    delete lst;
    if (arrTableJoin)
        delete arrTableJoin;

    return 0;
}

ClassImp(UniGenerateClasses)
