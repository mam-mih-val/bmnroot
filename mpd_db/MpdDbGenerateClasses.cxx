// -------------------------------------------------------------------------
// -----                      MpdDbGenerateClassesr cxx file           -----
// -----                  Created 18/08/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------
#include "MpdDbGenerateClasses.h"
#include "MpdDbConnection.h"
#include "../macro/mpd_scheduler/src/function_set.h"

#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TSQLColumnInfo.h"
#include "TSQLTableInfo.h"
#include "TList.h"

#include <fstream>
#include <iostream>
using namespace std;

enum enumDBMS{MySQL, PgSQL};

// -----   Constructor   -------------------------------
MpdDbGenerateClasses::MpdDbGenerateClasses()
{
}
// -------------------------------------------------------------------------

// -----  generate C++ classess - wrappers for DB tables  -------------------------------
int MpdDbGenerateClasses::GenerateClasses(TString connection_string, TString class_prefix, bool isOnlyUpdate)
{
    MpdDbConnection* connectionUniDb;
    if (connection_string == "")
        connectionUniDb = MpdDbConnection::Open(UNIFIED_DB);
    //else
    //    connectionUniDb = MpdDbConnection::Open(connection_string);

    if (connectionUniDb == 0x00)
    {
        cout<<"Error: connection to the database can't be established"<<endl;
        return -1;
    }

    TSQLServer* uni_db = connectionUniDb->GetSQLServer();

    // define DBMS: MySQL or Postgres
    enumDBMS curDBMS;
    if (strcmp(uni_db->GetDBMS(), "MySQL") == 0) curDBMS = MySQL;
    else if (strcmp(uni_db->GetDBMS(), "PgSQL") == 0) curDBMS = PgSQL;
    else
    {
        cout<<"Error: this type of DBMS isn't supported: "<<uni_db->GetDBMS()<<endl;
        return -2;
    }

    // get list of database tables
    TList* lst = uni_db->GetTablesList();
    TIter next(lst);
    TObject* obj;

    // cycle for all database tables
    while (obj = next())
    {
        // define current table name
        TString strTableName = obj->GetName();

        // exclude system tables
        if ((curDBMS == PgSQL) && (strTableName.BeginsWith("pg_") || strTableName.BeginsWith("sql_")))
            continue;

        cout<<"Parsing table: "<<strTableName<<endl;
        TSQLTableInfo* pTableInfo = uni_db->GetTableInfo(strTableName);

        // GET LIST OF COLUMNS FOR THE CURRENT TABLE
        vector<structColumnInfo*> vecColumns;
        /*TList* lstColumns = pTableInfo->GetColumns();
        TIter nextColumn(lstColumns);
        TObject* objColumn;
        // cycle for all columns
        while (objColumn = nextColumn())
        {
            TString strColumnName = objColumn->GetName();
            cout<<"Parsing column: "<<strColumnName<<endl;
            TSQLColumnInfo* pColumnInfo = pTableInfo->FindColumn(strColumnName);
        }*/

        TString sql;
        if (curDBMS == MySQL)
        {
            sql = TString::Format("SELECT ordinal_position, column_name, data_type, (is_nullable = 'YES') AS is_nullable, "
                                  "(extra = 'auto_increment') AS is_identity, (column_key = 'PRI') AS is_primary, (column_key = 'UNI') AS is_unique "
                                  "FROM INFORMATION_SCHEMA.COLUMNS "
                                  "WHERE table_name = '%s' "
                                  "ORDER BY ordinal_position", strTableName.Data());
        }
        else if (curDBMS == PgSQL)
        {
            sql = TString::Format("SELECT DISTINCT a.attnum as ordinal_position, a.attname as column_name, format_type(a.atttypid, a.atttypmod) as data_type, "
                                  "a.attnotnull as is_nullable, def.adsrc as is_default, coalesce(i.indisprimary, false) as is_primary, coalesce(i.indisunique, false) as is_unique "
                                  "FROM pg_attribute a JOIN pg_class pgc ON pgc.oid = a.attrelid "
                                  "LEFT JOIN pg_index i ON (pgc.oid = i.indrelid AND a.attnum = ANY(i.indkey)) "
                                  "LEFT JOIN pg_description com on (pgc.oid = com.objoid AND a.attnum = com.objsubid) "
                                  "LEFT JOIN pg_attrdef def ON (a.attrelid = def.adrelid AND a.attnum = def.adnum) "
                                  "WHERE a.attnum > 0 AND pgc.oid = a.attrelid AND pg_table_is_visible(pgc.oid) "
                                  "AND NOT a.attisdropped AND pgc.relname = '%s' "
                                  "ORDER BY a.attnum;", strTableName.Data());
        }

        TSQLResult* res = uni_db->Query(sql);
        int nrows = res->GetRowCount();
        if (nrows == 0)
        {
            cout<<"Critical error: table with no rows was found: "<<strTableName<<endl;
            return -3;
        }

        // parse all columns in current table in cycle
        TSQLRow* row;
        while (row = res->Next())
        {
            structColumnInfo* sColumnInfo = new structColumnInfo();
            TString strColumnName = row->GetField(1);
            sColumnInfo->strColumnName = strColumnName;
            sColumnInfo->isBinary = false;
            sColumnInfo->isDateTime = false;

            // remove last '_'
            TString strColumnNameWO = strColumnName;
            if (strColumnNameWO[strColumnNameWO.Length()-1] == '_')
                strColumnNameWO = strColumnNameWO.Remove(strColumnNameWO.Length()-1);

            // define column properties depended on column type
            TSQLColumnInfo* pColumnInfo = pTableInfo->FindColumn(strColumnName);
            switch (pColumnInfo->GetSQLType())
            {
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
                    sColumnInfo->strVariableType = "int";
                    sColumnInfo->strStatementType = "Int";
                    sColumnInfo->strPrintfType = "d";
                    sColumnInfo->strVariableName = "i_"+strColumnNameWO;

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
                    sColumnInfo->strStatementType = "LargeObject";
                    sColumnInfo->strPrintfType = "p";
                    sColumnInfo->strVariableName = "blob_"+strColumnNameWO;
                    sColumnInfo->isBinary = true;

                    break;
                }
                case TSQLServer::kSQL_TIMESTAMP:
                {
                    sColumnInfo->strVariableType = "TDatime";
                    sColumnInfo->strStatementType = "Datime";
                    sColumnInfo->strPrintfType = "s";
                    sColumnInfo->strVariableName = "dt_"+strColumnNameWO;
                    sColumnInfo->isDateTime = true;

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
                    else if (strDataType == "datetime")
                    {
                        sColumnInfo->strVariableType = "TDatime";
                        sColumnInfo->strStatementType = "Datime";
                        sColumnInfo->strPrintfType = "s";
                        sColumnInfo->strVariableName = "dt_"+strColumnNameWO;
                        sColumnInfo->isDateTime = true;
                    }
                    else
                    {
                        cout<<"Error: no corresponding column type: "<<row->GetField(2)<<". SQLType: "<<pColumnInfo->GetSQLType()<<endl;
                        return -4;
                    }
                }
            }// switch (pColumnInfo->GetSQLType())

            // form short variable name (e.g. ComponentName for 'component_name' column)
            TString strShortVar = strColumnNameWO;
            strShortVar = strShortVar.Replace(0, 1, toupper(strShortVar[0]));
            Ssiz_t char_under;
            while ((char_under = strShortVar.First('_')) != kNPOS)
            {
                strShortVar = strShortVar.Remove(char_under,1);
                if (strShortVar.Length() > char_under)
                    strShortVar = strShortVar.Replace(char_under, 1, toupper(strShortVar[char_under]));
            }
            sColumnInfo->strShortVariableName = strShortVar;

            if (curDBMS == MySQL)
            {
                sColumnInfo->isNullable = ((row->GetField(3))[0] == '1');
                sColumnInfo->isIdentity = ((row->GetField(4))[0] == '1');
                sColumnInfo->isPrimary = ((row->GetField(5))[0] == '1');
                sColumnInfo->isUnique = ((row->GetField(6))[0] == '1');
            }
            else if (curDBMS == PgSQL)
            {
                sColumnInfo->isNullable = ((row->GetField(3))[0] == 'f');
                sColumnInfo->isIdentity = ((row->GetField(4))[0] == 'n');
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
                sColumnInfo->strColumnValue = "*" + strColumnNameWO;
            }
            else
                sColumnInfo->strColumnValue = strColumnNameWO;

            vecColumns.push_back(sColumnInfo);

            if (sColumnInfo->isBinary)
            {
                structColumnInfo* sColumnInfoBinary = new structColumnInfo();
                sColumnInfoBinary->strVariableName = "sz_" + strColumnNameWO;
                sColumnInfoBinary->strTempVariableName = "tmp_sz_" + strColumnNameWO;
                sColumnInfoBinary->strVariableType = "Long_t";
                sColumnInfoBinary->strColumnName = "size_" + strColumnNameWO;
                sColumnInfoBinary->strShortVariableName = sColumnInfo->strShortVariableName + "Size";
                sColumnInfoBinary->strStatementType = "";

                vecColumns.push_back(sColumnInfoBinary);
            }
        }// cycle for all columns of current table

        delete row;
        delete res;

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
        strClassName = class_prefix + strClassName;

        // CREATING OR CHANGING HEADER FILE
        TString strFileName = "db_classes/" + strClassName + ".h"; // set header file name
        // open and write to file
        ifstream oldFile;
        TString strTempFileName;
        ofstream hFile;
        if (isOnlyUpdate)
        {
            oldFile.open(strFileName, ios::in);
            if (!oldFile.is_open())
            {
                cout<<"Error: could not open existing header file: "<<strFileName<<endl;
                return -5;
            }

            strTempFileName = strFileName + "_tmp";
            hFile.open(strTempFileName, ios::out);
            if (!hFile.is_open())
            {
                cout<<"Error: could not create temporary header file: "<<strTempFileName<<endl;
                return -6;
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
                cout<<"Error: could not create header file: "<<strFileName<<endl;
                return -7;
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
            hFile<<"\n#include \"MpdDbConnection.h\"\n\n";

            hFile<<(TString::Format("class %s\n", strClassName.Data())).Data();
            hFile<<"{\n";
            hFile<<" private:\n";
        }

        hFile<<"\t/* GENERATED PRIVATE MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */\n";
        hFile<<"\tMpdDbConnection* connectionUniDb;\n\n";

        // adding member variables corresding table columns
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            hFile<<(TString::Format("\t%s %s;\n", cur_col->strVariableType.Data(), cur_col->strVariableName.Data())).Data();
        }

        hFile<<"\n\t//Constructor\n";
        hFile<<(TString::Format("\t%s(MpdDbConnection* connUniDb", strClassName.Data())).Data();
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            hFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
        }
        hFile<<");\n";
        hFile<<"\t/* END OF PRIVATE GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */\n";

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

        hFile<<"\t/* GENERATED PUBLIC MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */\n";
        hFile<<(TString::Format("\tvirtual ~%s(); // Destructor\n\n", strClassName.Data())).Data();

        hFile<<"\t// static class functions\n";
        hFile<<(TString::Format("\tstatic %s* Create%s(", strClassName.Data(), strShortTableName.Data())).Data();
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

        // // GET RECORD - DECLARATION
        hFile<<(TString::Format("\tstatic %s* Get%s(", strClassName.Data(), strShortTableName.Data())).Data();
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

                hFile<<(TString::Format("\tstatic %s* Get%s(%s %s);\n", strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // DELETE RECORD - DECLARATION
        hFile<<(TString::Format("\tstatic int Delete%s(", strShortTableName.Data())).Data();
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

                hFile<<(TString::Format("\tstatic int Delete%s(%s %s);\n", strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // PRINT ALL ROWS -DECLARATION
        hFile<<"\tstatic int PrintAll();\n";

        // GETTERS FUNCTIONS - IMPLEMENTATIONS
        hFile<<"\n\t// Getters\n";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            hFile<<(TString::Format("\t%s Get%s() {", cur_col->strVariableType.Data(), cur_col->strShortVariableName.Data())).Data();

            if (cur_col->isNullable)
            {
                hFile<<(TString::Format("if (%s == NULL) return NULL; else ", cur_col->strVariableName.Data())).Data();
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

        hFile<<"\n\t// Setters\n";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isBinary)
                hFile<<(TString::Format("\tint Set%s(%s %s);\n", cur_col->strShortVariableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            else
            {
                hFile<<(TString::Format("\tint Set%s(%s %s, ", cur_col->strShortVariableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
                ++it;
                cur_col= *it;
                hFile<<(TString::Format("%s %s);\n", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        // PRINT VALUES -DECLARATION
        hFile<<"\tvoid Print();\n";
        hFile<<"\t/* END OF PUBLIC GENERATED PART (SHOULDN'T BE CHANGED MANUALLY) */\n";

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
        strFileName = "db_classes/" + strClassName + ".cxx";
        // open and write to file
        ofstream cxxFile;
        if (isOnlyUpdate)
        {
            oldFile.open(strFileName, ios::in);
            if (!oldFile.is_open())
            {
                cout<<"Error: could not open existing cxx file: "<<strFileName<<endl;
                return -8;
            }

            strTempFileName = strFileName + "_tmp";
            cxxFile.open(strTempFileName, ios::out);
            if (!cxxFile.is_open())
            {
                cout<<"Error: could not create temporary cxx file: "<<strTempFileName<<endl;
                return -9;
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
                cout<<"Error: could not create cxx file: "<<strFileName<<endl;
                return -10;
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

        cxxFile<<"/* GENERATED CLASS MEMBERS (SHOULDN'T BE CHANGED MANUALLY) */\n";

        cxxFile<<"// -----   Constructor with database connection   -----------------------\n";
        cxxFile<<(TString::Format("%s::%s(MpdDbConnection* connUniDb", strClassName.Data(), strClassName.Data())).Data();
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            cxxFile<<(TString::Format(", %s %s", cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();
        }
        cxxFile<<")\n{\n";

        cxxFile<<"\tconnectionUniDb = connUniDb;\n\n";
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            cxxFile<<(TString::Format("\t%s = %s;\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
        }
        cxxFile<<"}\n\n";

        // DESTRUCTOR - IMPLEMENTATION
        cxxFile<<"// -----   Destructor   -------------------------------------------------\n";
        cxxFile<<(TString::Format("%s::~%s()\n{\n\tif (connectionUniDb)\n\t\tdelete connectionUniDb;\n", strClassName.Data(), strClassName.Data())).Data();

        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("\tif (%s)\n\t\tdelete %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
            if (cur_col->isBinary)
                cxxFile<<(TString::Format("\tif (%s)\n\t\tdelete [] %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
        }
        cxxFile<<"}\n\n";

        // CREATE NEW RECORD - INPLEMENTATION
        cxxFile<<"// -----   Creating new record in class table ---------------------------\n";
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
        cxxFile<<")\n{\n";
        cxxFile<<"\tMpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);\n";
        cxxFile<<"\tif (connUniDb == 0x00) return 0x00;\n\n";

        cxxFile<<"\tTSQLServer* uni_db = connUniDb->GetSQLServer();\n\n";
        cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"insert into %s(", strTableName.Data())).Data();
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
        cxxFile<<") \"\n\t\t\"values (";
        for (int i = 1 ; i <= count; i++)
        {
            if (curDBMS == MySQL)
            {
                if (i == 1)
                    cxxFile<<"?";
                else
                    cxxFile<<", ?";
            }
            else if (curDBMS == PgSQL)
            {
                if (i == 1)
                    cxxFile<<"$1";
                else
                    cxxFile<<(TString::Format(", $%d", i)).Data();
            }

        }
        cxxFile<<")\");\n";

        cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n\n";

        cxxFile<<"\tstmt->NextIteration();\n";
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
                cxxFile<<(TString::Format("\tif (%s == NULL)\n\t\tstmt->SetNull(%d);\n\telse\n\t", cur_col->strColumnName.Data(), count)).Data();

            if (!cur_col->isBinary)
                cxxFile<<(TString::Format("\tstmt->Set%s(%d, %s);\n", cur_col->strStatementType.Data(), count, cur_col->strColumnValue.Data())).Data();
            else
            {
                cxxFile<<(TString::Format("\tstmt->Set%s(%d, %s, ", cur_col->strStatementType.Data(), count, cur_col->strColumnName.Data())).Data();
                ++it;
                cur_col= *it;
                cxxFile<<(TString::Format("%s, 0x4000000);\n", cur_col->strColumnName.Data())).Data();
            }

            count++;
        }

        cxxFile<<"\n\t// inserting new record to DB\n";
        cxxFile<<"\tif (!stmt->Process())\n\t{\n";
        cxxFile<<"\t\tcout<<\"Error: inserting new record to DB has been failed\"<<endl;\n";
        cxxFile<<"\t\tdelete stmt;\n\t\tdelete connUniDb;\n\t\treturn 0x00;\n\t}\n\n";

        cxxFile<<"\tdelete stmt;\n\n";

        if (strIdentityColumnName != "")
        {
            cxxFile<<"\t// getting last inserted ID\n";
            cxxFile<<(TString::Format("\tint %s;\n", strIdentityColumnName.Data())).Data();
            if (curDBMS == MySQL)
            {
                cxxFile<<"\tTSQLStatement* stmt_last = uni_db->Statement(\"SELECT LAST_INSERT_ID()\");\n";
            }
            else if (curDBMS == PgSQL)
            {
                cxxFile<<(TString::Format("\tTSQLStatement* stmt_last = uni_db->Statement(\"SELECT currval(pg_get_serial_sequence('%s','%s'))\");\n",
                                          strTableName.Data(), strIdentityColumnName.Data())).Data();
            }

            cxxFile<<"\n\t// process getting last id\n"
                     "\tif (stmt_last->Process())\n\t{\n"
                     "\t\t// store result of statement in buffer\n"
                     "\t\tstmt_last->StoreResult();\n\n";
            cxxFile<<"\t\t// if there is no last id then exit with error\n"
                     "\t\tif (!stmt_last->NextResultRow())\n\t\t{\n"
                     "\t\t\tcout<<\"Error: no last ID in DB!\"<<endl;\n"
                     "\t\t\tdelete stmt_last;\n"
                     "\t\t\treturn 0x00;\n\t\t}\n"
                    "\t\telse\n\t\t{\n";
            cxxFile<<(TString::Format("\t\t\t%s = stmt_last->GetInt(0);\n", strIdentityColumnName.Data())).Data();
            cxxFile<<"\t\t\tdelete stmt_last;\n\t\t}\n\t}\n"
                     "\telse\n\t{\n"
                     "\t\tcout<<\"Error: getting last ID has been failed!\"<<endl;\n"
                     "\t\tdelete stmt_last;\n"
                     "\t\treturn 0x00;\n\t}\n\n";
        }

        // write to temporary variable to create separate memory copy of original variables
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            TString TempColumnName = cur_col->strColumnName, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
            cxxFile<<(TString::Format("\t%s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("\tif (%s == NULL) %s = NULL;\n\telse\n\t", cur_col->strColumnName.Data(), TempVar.Data())).Data();
                if (cur_col->isBinary)
                {
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("\t{\n\t\t%s %s = %s;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("\t\t%s = new %s[%s];\n", TempVar.Data(), VariableTypePointer.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\tmemcpy(%s, %s, %s);\n\t}\n", TempVar.Data(), TempColumnName.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("\t%s = new %s(*%s);\n", TempVar.Data(), VariableTypePointer.Data(), cur_col->strColumnName.Data())).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("\t%s %s = %s;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data(), cur_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("\t%s = new %s[%s];\n", TempVar.Data(), VariableTypePointer.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\tmemcpy(%s, %s, %s);\n", TempVar.Data(), TempColumnName.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("\t%s = %s;\n", TempVar.Data(), cur_col->strColumnName.Data())).Data();
            }
        }

        cxxFile<<(TString::Format("\n\treturn new %s(connUniDb", strClassName.Data())).Data();
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            cxxFile<<(TString::Format(", %s", cur_col->strTempVariableName.Data())).Data();
        }
        cxxFile<<");\n}\n\n";

        // GET RECORD - IMPLEMENTATION
        cxxFile<<"// -----   Get table record from database ---------------------------\n";
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

        cxxFile<<"\tMpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);\n";
        cxxFile<<"\tif (connUniDb == 0x00) return 0x00;\n\n";

        cxxFile<<"\tTSQLServer* uni_db = connUniDb->GetSQLServer();\n\n";
        cxxFile<<"\tTString sql = TString::Format(\n\t\t\"select";
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
        cxxFile<<(TString::Format(" \"\n\t\t\"from %s \"\n\t\t\"where", strTableName.Data())).Data();
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

        cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n";

        cxxFile<<"\n\t// get table record from DB\n";
        cxxFile<<"\tif (!stmt->Process())\n\t{\n";
        cxxFile<<"\t\tcout<<\"Error: getting record from DB has been failed\"<<endl;\n\n";
        cxxFile<<"\t\tdelete stmt;\n";
        cxxFile<<"\t\tdelete connUniDb;\n";
        cxxFile<<"\t\treturn 0x00;\n\t}\n\n";

        cxxFile<<"\t// store result of statement in buffer\n";
        cxxFile<<"\tstmt->StoreResult();\n\n";

        cxxFile<<"\t// extract row\n";
        cxxFile<<"\tif (!stmt->NextResultRow())\n\t{\n";
        cxxFile<<"\t\tcout<<\"Error: table record wasn't found\"<<endl;\n\n";
        cxxFile<<"\t\tdelete stmt;\n";
        cxxFile<<"\t\tdelete connUniDb;\n";
        cxxFile<<"\t\treturn 0x00;\n\t}\n\n";

        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName, VariableTypePointer = cur_col->strVariableTypePointer;
            cxxFile<<(TString::Format("\t%s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("\tif (stmt->IsNull(%d)) %s = NULL;\n\telse\n\t", count, TempVar.Data())).Data();
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("\t{\n\t\t%s = NULL;\n", TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("\t\t%s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\tstmt->Get%s(%d, (void*&)%s, %s);\n\t}\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("\t%s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(), StatementType.Data(), count)).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("\t%s = NULL;\n", TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("\t%s %s = 0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\tstmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                           TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("\t%s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
            }

            count++;
        }

        cxxFile<<"\n\tdelete stmt;\n\n";

        cxxFile<<(TString::Format("\treturn new %s(connUniDb", strClassName.Data())).Data();
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
        cxxFile<<");\n}\n\n";

        // static get functions for Unique fields
        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col = *it;
                if (!cur_col->isUnique)
                    continue;

                cxxFile<<"// -----   Get table record from database for unique key--------------\n";
                cxxFile<<(TString::Format("%s* %s::Get%s(%s %s)\n{\n" ,
                                          strClassName.Data(), strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<"\tMpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);\n";
                cxxFile<<"\tif (connUniDb == 0x00) return 0x00;\n\n";

                cxxFile<<"\tTSQLServer* uni_db = connUniDb->GetSQLServer();\n\n";
                cxxFile<<"\tTString sql = TString::Format(\n\t\t\"select";
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

                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" \"\n\t\t\"from %s \"\n\t\t\"where lower(%s) = lower('%%%s')\", %s.Data());\n",
                                              strTableName.Data(), cur_col->strColumnName.Data(), cur_col->strPrintfType.Data(), cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(" \"\n\t\t\"from %s \"\n\t\t\"where %s = %%%s\", %s);\n",
                                              strTableName.Data(), cur_col->strColumnName.Data(), cur_col->strPrintfType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n";

                cxxFile<<"\n\t// get table record from DB\n";
                cxxFile<<"\tif (!stmt->Process())\n\t{\n";
                cxxFile<<"\t\tcout<<\"Error: getting record from DB has been failed\"<<endl;\n\n";
                cxxFile<<"\t\tdelete stmt;\n";
                cxxFile<<"\t\tdelete connUniDb;\n";
                cxxFile<<"\t\treturn 0x00;\n\t}\n\n";

                cxxFile<<"\t// store result of statement in buffer\n";
                cxxFile<<"\tstmt->StoreResult();\n\n";

                cxxFile<<"\t// extract row\n";
                cxxFile<<"\tif (!stmt->NextResultRow())\n\t{\n";
                cxxFile<<"\t\tcout<<\"Error: table record wasn't found\"<<endl;\n\n";
                cxxFile<<"\t\tdelete stmt;\n";
                cxxFile<<"\t\tdelete connUniDb;\n";
                cxxFile<<"\t\treturn 0x00;\n\t}\n\n";

                count = 0;
                for (vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
                {
                    structColumnInfo* current_col= *it_inner;

                    TString StatementType = current_col->strStatementType, TempVar = current_col->strTempVariableName,
                            VariableTypePointer = current_col->strVariableTypePointer;
                    cxxFile<<(TString::Format("\t%s %s;\n", current_col->strVariableType.Data(), TempVar.Data())).Data();

                    if (current_col->isNullable)
                    {
                        cxxFile<<(TString::Format("\tif (stmt->IsNull(%d)) %s = NULL;\n\telse\n\t", count, TempVar.Data())).Data();
                        if (current_col->isBinary)
                        {
                            cxxFile<<(TString::Format("\t{\n\t\t%s = NULL;\n", TempVar.Data())).Data();
                            ++it_inner;
                            current_col= *it_inner;
                            cxxFile<<(TString::Format("\t\t%s %s = 0;\n", current_col->strVariableType.Data(), current_col->strTempVariableName.Data())).Data();
                            cxxFile<<(TString::Format("\t\tstmt->Get%s(%d, (void*&)%s, %s);\n\t}\n", StatementType.Data(), count,
                                                       TempVar.Data(), current_col->strTempVariableName.Data())).Data();
                        }
                        else
                            cxxFile<<(TString::Format("\t%s = new %s(stmt->Get%s(%d));\n", TempVar.Data(), VariableTypePointer.Data(),
                                                      StatementType.Data(), count)).Data();
                    }
                    else
                    {
                        if (current_col->isBinary)
                        {
                            cxxFile<<(TString::Format("\t%s = NULL;\n", TempVar.Data())).Data();
                            ++it_inner;
                            current_col= *it_inner;
                            cxxFile<<(TString::Format("\t%s %s = 0;\n", current_col->strVariableType.Data(), current_col->strTempVariableName.Data())).Data();
                            cxxFile<<(TString::Format("\tstmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                                   TempVar.Data(), current_col->strTempVariableName.Data())).Data();
                        }
                        else
                            cxxFile<<(TString::Format("\t%s = stmt->Get%s(%d);\n", TempVar.Data(), StatementType.Data(), count)).Data();
                    }

                    count++;
                }

                cxxFile<<"\n\tdelete stmt;\n\n";

                cxxFile<<(TString::Format("\treturn new %s(connUniDb", strClassName.Data())).Data();
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
                cxxFile<<");\n}\n\n";
            }// for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        }// static get functions for Unique fields

        // DELETE RECORD - IMPLEMENTATION
        cxxFile<<"// -----   Delete record from class table ---------------------------\n";
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

        cxxFile<<"\tMpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);\n";
        cxxFile<<"\tif (connUniDb == 0x00) return 0x00;\n\n";

        cxxFile<<"\tTSQLServer* uni_db = connUniDb->GetSQLServer();\n\n";
        cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"delete from %s \"\n\t\t\"where", strTableName.Data())).Data();
        count = 0;
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            if (count > 0)
                cxxFile<<" and";

            if (curDBMS == MySQL)
            {
                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" lower(%s) = lower(?)", cur_col->strColumnName.Data())).Data();
                else
                    cxxFile<<(TString::Format(" %s = ?", cur_col->strColumnName.Data())).Data();
            }
            else if (curDBMS == PgSQL)
            {
                if (cur_col->strStatementType == "String")
                    cxxFile<<(TString::Format(" lower(%s) = lower($%d)", cur_col->strColumnName.Data(), count+1)).Data();
                else
                    cxxFile<<(TString::Format(" %s = $%d", cur_col->strColumnName.Data(), count+1)).Data();
            }

            count++;
        }
        cxxFile<<"\");\n";

        cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n\n";

        cxxFile<<"\tstmt->NextIteration();\n";
        count = 0;
        for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;
            if (!cur_col->isPrimary)
                continue;

            cxxFile<<(TString::Format("\tstmt->Set%s(%d, %s);\n", cur_col->strStatementType.Data(), count, cur_col->strColumnName.Data())).Data();
            count++;
        }

        cxxFile<<"\n\t// delete table record from DB\n";
        cxxFile<<"\tif (!stmt->Process())\n\t{\n";
        cxxFile<<"\t\tcout<<\"Error: deleting record from DB has been failed\"<<endl;\n\n";
        cxxFile<<"\t\tdelete stmt;\n";
        cxxFile<<"\t\tdelete connUniDb;\n";
        cxxFile<<"\t\treturn -1;\n\t}\n\n";

        cxxFile<<"\tdelete stmt;\n";
        cxxFile<<"\tdelete connUniDb;\n";
        cxxFile<<"\treturn 0;\n}\n\n";

        // static Delete functions for Unique fields
        if (is_flag)
        {
            for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
            {
                structColumnInfo* cur_col = *it;
                if (!cur_col->isUnique)
                    continue;

                cxxFile<<"// -----   Delete table record from database for unique key--------------\n";
                cxxFile<<(TString::Format("int %s::Delete%s(%s %s)\n{\n" ,
                                          strClassName.Data(), strShortTableName.Data(), cur_col->strVariableType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<"\tMpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);\n";
                cxxFile<<"\tif (connUniDb == 0x00) return 0x00;\n\n";

                cxxFile<<"\tTSQLServer* uni_db = connUniDb->GetSQLServer();\n\n";

                if (curDBMS == MySQL)
                {
                    if (cur_col->strStatementType == "String")
                        cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"delete from %s \"\n\t\t\"where lower(%s) = lower(?)\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"delete from %s \"\n\t\t\"where %s = ?\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                }
                else if (curDBMS == PgSQL)
                {
                    if (cur_col->strStatementType == "String")
                        cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"delete from %s \"\n\t\t\"where lower(%s) = lower($1)\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"delete from %s \"\n\t\t\"where %s = $1\");\n", strTableName.Data(), cur_col->strColumnName.Data())).Data();
                }

                cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n\n";

                cxxFile<<"\tstmt->NextIteration();\n";
                cxxFile<<(TString::Format("\tstmt->Set%s(0, %s);\n", cur_col->strStatementType.Data(), cur_col->strColumnName.Data())).Data();

                cxxFile<<"\n\t// delete table record from DB\n";
                cxxFile<<"\tif (!stmt->Process())\n\t{\n";
                cxxFile<<"\t\tcout<<\"Error: deleting record from DB has been failed\"<<endl;\n\n";
                cxxFile<<"\t\tdelete stmt;\n";
                cxxFile<<"\t\tdelete connUniDb;\n";
                cxxFile<<"\t\treturn -1;\n\t}\n\n";

                cxxFile<<"\tdelete stmt;\n";
                cxxFile<<"\tdelete connUniDb;\n";
                cxxFile<<"\treturn 0;\n}\n\n";
            }// for(vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        }// static Delete functions for Unique fields

        // PRINT ALL ROWS - IMPLEMENTATION
        cxxFile<<"// -----   Print all table records ---------------------------------\n";
        cxxFile<<(TString::Format("int %s::PrintAll()\n{\n", strClassName.Data())).Data();

        cxxFile<<"\tMpdDbConnection* connUniDb = MpdDbConnection::Open(UNIFIED_DB);\n";
        cxxFile<<"\tif (connUniDb == 0x00) return 0x00;\n\n";

        cxxFile<<"\tTSQLServer* uni_db = connUniDb->GetSQLServer();\n\n";
        cxxFile<<"\tTString sql = TString::Format(\n\t\t\"select";
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
        cxxFile<<(TString::Format(" \"\n\t\t\"from %s\");\n", strTableName.Data())).Data();

        cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n";

        cxxFile<<"\n\t// get table record from DB\n";
        cxxFile<<"\tif (!stmt->Process())\n\t{\n";
        cxxFile<<"\t\tcout<<\"Error: getting all records from DB has been failed\"<<endl;\n\n";
        cxxFile<<"\t\tdelete stmt;\n";
        cxxFile<<"\t\tdelete connUniDb;\n";
        cxxFile<<"\t\treturn -1;\n\t}\n\n";

        cxxFile<<"\t// store result of statement in buffer\n";
        cxxFile<<"\tstmt->StoreResult();\n\n";

        cxxFile<<"\t// print rows\n";
        cxxFile<<(TString::Format("\tcout<<\"Table '%s'\"<<endl;\n", strTableName.Data())).Data();
        cxxFile<<"\twhile (stmt->NextResultRow())\n\t{\n";

        count = 0;
        for (vector<structColumnInfo*>::iterator it = vecColumns.begin(); it != vecColumns.end(); ++it)
        {
            structColumnInfo* cur_col= *it;

            TString StatementType = cur_col->strStatementType, TempVar = cur_col->strTempVariableName;
            cxxFile<<(TString::Format("\t\tcout<<\". %s: \";\n", cur_col->strColumnName.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("\t\tif (stmt->IsNull(%d)) cout<<\"NULL\";\n\t\telse\n\t", count)).Data();
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("\t\t%s %s;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();
                    cxxFile<<(TString::Format("\t\t{\n\t\t\t%s = NULL;\n", TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("\t\t\t%s %s=0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\t\tstmt->Get%s(%d, (void*&)%s, %s);\n\t\t}\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\t\tcout<<(void*)%s<<\", binary size: \"<<%s;\n", TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                {
                    cxxFile<<(TString::Format("\t\tcout<<stmt->Get%s(%d)", StatementType.Data(), count)).Data();
                    if (cur_col->isDateTime)
                        cxxFile<<".AsSQLString()";
                    cxxFile<<";\n";
                }
            }
            else
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("\t\t%s %s = NULL;\n", cur_col->strVariableType.Data(), TempVar.Data())).Data();
                    ++it;
                    cur_col= *it;
                    cxxFile<<(TString::Format("\t\t%s %s=0;\n", cur_col->strVariableType.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\tstmt->Get%s(%d, (void*&)%s, %s);\n", StatementType.Data(), count,
                                               TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\tcout<<(void*)%s<<\", binary size: \"<<%s;\n", TempVar.Data(), cur_col->strTempVariableName.Data())).Data();
                }
                else
                {
                    cxxFile<<(TString::Format("\t\tcout<<(stmt->Get%s(%d))", StatementType.Data(), count)).Data();
                    if (cur_col->isDateTime)
                        cxxFile<<".AsSQLString()";
                    cxxFile<<";\n";
                }
            }

            count++;
        }

        cxxFile<<"\t\tcout<<endl;\n\t}\n\n";

        cxxFile<<"\tdelete stmt;\n";
        cxxFile<<"\tdelete connUniDb;\n\n";
        cxxFile<<"\treturn 0;\n}\n\n";

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

            cxxFile<<"\tif (!connectionUniDb)\n\t{\n\t\tcout<<\"Connection object is null\"<<endl;\n\t\treturn -1;\n\t}\n\n";

            cxxFile<<"\tTSQLServer* uni_db = connectionUniDb->GetSQLServer();\n\n";

            if (curDBMS == MySQL)
            {
                cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"update %s \"\n\t\t\"set %s = ? \"\n\t\t\"where", strTableName.Data(), cur_col->strColumnName.Data())).Data();
            }
            else if (curDBMS == PgSQL)
            {
                cxxFile<<(TString::Format("\tTString sql = TString::Format(\n\t\t\"update %s \"\n\t\t\"set %s = $1 \"\n\t\t\"where", strTableName.Data(), cur_col->strColumnName.Data())).Data();
            }
            count = 0;
            for(vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
            {
                structColumnInfo* current_col= *it_inner;
                if (!current_col->isPrimary)
                    continue;

                if (curDBMS == MySQL)
                {
                    if (count == 0)
                        cxxFile<<(TString::Format(" %s = ?", current_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format(" and %s = ?", current_col->strColumnName.Data())).Data();
                }
                else if (curDBMS == PgSQL)
                {
                    if (count == 0)
                        cxxFile<<(TString::Format(" %s = $2", current_col->strColumnName.Data())).Data();
                    else
                        cxxFile<<(TString::Format(" and %s = $%d", current_col->strColumnName.Data(), count+2)).Data();
                }

                count++;
            }
            cxxFile<<"\");\n";

            cxxFile<<"\tTSQLStatement* stmt = uni_db->Statement(sql);\n\n";

            cxxFile<<"\tstmt->NextIteration();\n";

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("\tif (%s == NULL)\n\t\tstmt->SetNull(0);\n\telse\n\t", cur_col->strColumnName.Data())).Data();

            if (!cur_col->isBinary)
                cxxFile<<(TString::Format("\tstmt->Set%s(0, %s);\n", cur_col->strStatementType.Data(), cur_col->strColumnValue.Data())).Data();
            else
            {
                cxxFile<<(TString::Format("\tstmt->Set%s(0, %s, ", cur_col->strStatementType.Data(), cur_col->strColumnName.Data())).Data();
                cxxFile<<(TString::Format("%s, 0x4000000);\n", temp_col->strColumnName.Data())).Data();
            }

            count = 1;
            for(vector<structColumnInfo*>::iterator it_inner = vecColumns.begin(); it_inner != vecColumns.end(); ++it_inner)
            {
                structColumnInfo* current_col= *it_inner;
                if (!current_col->isPrimary)
                    continue;

                cxxFile<<(TString::Format("\tstmt->Set%s(%d, %s);\n", current_col->strStatementType.Data(), count, current_col->strVariableName.Data())).Data();
                count++;
            }

            cxxFile<<"\n\t// write new value to database\n";
            cxxFile<<"\tif (!stmt->Process())\n\t{\n";
            cxxFile<<"\t\tcout<<\"Error: updating the record has been failed\"<<endl;\n\n";
            cxxFile<<"\t\tdelete stmt;\n";
            cxxFile<<"\t\treturn -2;\n\t}\n\n";

            if (cur_col->isNullable)
                cxxFile<<(TString::Format("\tif (%s)\n\t\tdelete %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
            if (cur_col->isBinary)
                cxxFile<<(TString::Format("\tif (%s)\n\t\tdelete [] %s;\n", cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();

            if (cur_col->isNullable)
            {
                cxxFile<<(TString::Format("\tif (%s == NULL) %s = NULL;\n\telse\n\t", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("\t{\n\t\t%s %s = %s;\n", temp_col->strVariableType.Data(), temp_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("\t\t%s = new %s[%s];\n", cur_col->strVariableName.Data(), cur_col->strVariableTypePointer.Data(), temp_col->strVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\t\tmemcpy(%s, %s, %s);\n\t}\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data(), temp_col->strVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("\t%s = new %s(*%s);\n", cur_col->strVariableName.Data(), cur_col->strVariableTypePointer.Data(), cur_col->strColumnName.Data())).Data();
            }
            else
            {
                if (cur_col->isBinary)
                {
                    cxxFile<<(TString::Format("\t%s = %s;\n", temp_col->strVariableName.Data(), temp_col->strColumnName.Data())).Data();
                    cxxFile<<(TString::Format("\t%s = new %s[%s];\n", cur_col->strVariableName.Data(), cur_col->strVariableTypePointer.Data(), temp_col->strVariableName.Data())).Data();
                    cxxFile<<(TString::Format("\tmemcpy(%s, %s, %s);\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data(), temp_col->strVariableName.Data())).Data();
                }
                else
                    cxxFile<<(TString::Format("\t%s = %s;\n", cur_col->strVariableName.Data(), cur_col->strColumnName.Data())).Data();
            }

            cxxFile<<"\n\tdelete stmt;\n";
            cxxFile<<"\treturn 0;\n}\n\n";
        }// setters functions - implementation

        // PRINT VALUES - IMPLEMENTATION
        cxxFile<<"// -----   Print current record ---------------------------------------\n";
        cxxFile<<(TString::Format("void %s::Print()\n{\n", strClassName.Data())).Data();

        cxxFile<<(TString::Format("\tcout<<\"Table '%s'\";\n\tcout", strTableName.Data())).Data();
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
                        cxxFile<<(TString::Format("<<\". %s: \"<<(%s == NULL? \"NULL\": (*%s).AsSQLString())",
                                                  cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                    else
                    {
                        if (cur_col->strPrintfType == "s")
                            cxxFile<<(TString::Format("<<\". %s: \"<<(%s == NULL? \"NULL\": *%s)",
                                                      cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strVariableName.Data())).Data();
                        else
                            cxxFile<<(TString::Format("<<\". %s: \"<<(%s == NULL? \"NULL\": TString::Format(\"%%%s\", *%s))",
                                              cur_col->strColumnName.Data(), cur_col->strVariableName.Data(), cur_col->strPrintfType.Data(), cur_col->strVariableName.Data())).Data();
                    }
                }
                else
                {
                    cxxFile<<(TString::Format("<<\". %s: \"<<%s", cur_col->strColumnName.Data(), cur_col->strVariableName.Data())).Data();
                    if (cur_col->isDateTime)
                        cxxFile<<".AsSQLString()";
                }
            }
        }
        cxxFile<<"<<endl;\n\n";

        cxxFile<<"\treturn;\n}\n";
        cxxFile<<"/* END OF GENERATED CLASS PART (SHOULDN'T BE CHANGED MANUALLY) */\n";

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
    delete connectionUniDb;

    return 0;
}

// -------------------------------------------------------------------
MpdDbGenerateClasses::~MpdDbGenerateClasses()
{
}

// -------------------------------------------------------------------
ClassImp(MpdDbGenerateClasses)
