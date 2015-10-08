// -------------------------------------------------------------------------
// -----                      UniDbGenerateClasses header file         -----
// -----                  Created 18/08/15  by K. Gertsenberger        -----
// -------------------------------------------------------------------------

/** UniDbGenerateClasses.h
 *@author K.Gertsenberger <gertsen@jinr.ru>
 **
 ** Class for generating MPD DB classes
 **/

#ifndef UNIDBGENERATECLASSES_H
#define UNIDBGENERATECLASSES_H 1

#include "TString.h"

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
    TString strShortVariableName;
    bool isIdentity;
    bool isPrimary;
    bool isCompositeKey;
    bool isUnique;
    bool isNullable;
    bool isBinary;
    bool isDateTime;
};

class UniDbGenerateClasses
{
 public:
    UniDbGenerateClasses();             // Constructor
    virtual ~UniDbGenerateClasses();    // Destructor

    // generate C++ classess - wrappers for DB tables
    int GenerateClasses(TString connection_string = "", TString class_prefix = "UniDb", bool isOnlyUpdate = false);

  ClassDef(UniDbGenerateClasses,1);
};

#endif
