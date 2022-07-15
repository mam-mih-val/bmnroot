// -------------------------------------------------------------------------
// -----                    UniGenerateClasses header file             -----
// -----                  Created 18/08/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------

/** UniGenerateClasses.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for autogenerating database classes based on the database structure (tables)
**/

#ifndef UNIGENERATECLASSES_H
#define UNIGENERATECLASSES_H 1

#include "TString.h"
#include "TObjArray.h"

enum ConnectionType{UNIFIED_DB, ELOG_DB};

struct structColumnInfo
{
    // column name (e.g. run_number)
    TString strColumnName;
    // <column name> for 'not null' (e.g. run_number),  *<column_name> for null (e.g. *run_number)
    TString strColumnPointer;
    // column name with spaces instead of '_' (e.g. run number)
    TString strColumnNameSpace;
    // C++/ROOT variable type corresponding column (e.g. int and int*)
    TString strVariableType;
    // <variable type> for 'not null', <variable type> without '*' at the end for null (e.g. int and int)
    TString strVariableTypePointer;
    TString strStatementType;
    // symbol corresponding variable type as it presented in printf (or TString::Format)
    TString strPrintfType;
    // member class variable name corresponding table column
    TString strVariableName;
    // temp variable name for tempopary copy this variable
    TString strTempVariableName;
    // short variable name (e.g. ComponentName for 'component_name' column
    TString strShortVariableName;
    bool isIdentity;
    bool isPrimary;
    bool isCompositeKey;
    bool isUnique;
    bool isNullable;
    bool isBinary;
    bool isDateTime;
    bool isTimeStamp;
};

// structure to join fields of another table
struct structTableJoin
{
    TString strSourceTableName;
    TString strJoinTableName;
    structColumnInfo strJoinField;
    TObjArray* arrManualFieldNames;

    structTableJoin()  { arrManualFieldNames = nullptr; }
    ~structTableJoin() { if (arrManualFieldNames) delete arrManualFieldNames; }
};


class UniGenerateClasses
{
 public:
    UniGenerateClasses();             // Constructor
    virtual ~UniGenerateClasses();    // Destructor

    // Generate C++ classes - wrappers for all tables of the given database
    // Parameters:
    //      json_configuration_file - path to JSON file with the configuration of the class generator
    static int GenerateClasses(TString json_configuration_file);

  ClassDef(UniGenerateClasses, 2);
};

#endif
