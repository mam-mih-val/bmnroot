// -------------------------------------------------------------------------
// -----                    UniGenerateClasses header file           -----
// -----                  Created 18/08/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------

/** UniGenerateClasses.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for generating DB classes
 **/

#ifndef UNIGENERATECLASSES_H
#define UNIGENERATECLASSES_H 1

#include "UniConnection.h"

#include "TString.h"
#include "TObjArray.h"

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

// structure to join another table's fields (for read-only) - NOT IMPLEMENTED YET
struct structTableJoin
{
    TString strSourceTableName;
    TString strJoinTableName;
    structColumnInfo strJoinField;
    TObjArray* arrManualFieldNames;

    structTableJoin() { arrManualFieldNames = NULL; }
    ~structTableJoin() { if (arrManualFieldNames) delete arrManualFieldNames; }
};

class UniGenerateClasses
{
 public:
    UniGenerateClasses();             // Constructor
    virtual ~UniGenerateClasses();    // Destructor

    // Generate C++ classess - wrappers for DB tables
    // Parameters:
    //      connection_type - database from enumeration;
    //      class_prefix - prefix with directory name and prefix of classes' names
    //      isOnlyUpdate - whether only update of the existing classes or create new ones
    int GenerateClasses(UniConnectionType connection_type, TString class_prefix = "UniDb", bool isOnlyUpdate = true);

    TObjArray* arrTableJoin;

  ClassDef(UniGenerateClasses,1);
};

#endif
