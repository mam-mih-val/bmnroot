#ifndef UNIDBSEARCHCONDITION_H
#define UNIDBSEARCHCONDITION_H 1

#include "db_structures.h"

#include "TString.h"
#include "TDatime.h"

#include <vector>
using namespace std;

class UniDbSearchCondition
{
 public:
    UniDbSearchCondition(enumColumns column, enumConditions condition);
    UniDbSearchCondition(enumColumns column, enumConditions condition, int value);
    UniDbSearchCondition(enumColumns column, enumConditions condition, unsigned int value);
    UniDbSearchCondition(enumColumns column, enumConditions condition, double value);
    UniDbSearchCondition(enumColumns column, enumConditions condition, TString value);
    UniDbSearchCondition(enumColumns column, enumConditions condition, TDatime value);
    ~UniDbSearchCondition(){}

    // getters
    enumColumns GetColumn() {return eColumn;}
    enumConditions GetCondition() {return eCondition;}
    int GetValueType() {return iValueType;}
    int GetIntValue() {return iValue;}
    unsigned int GetUIntValue() {return uiValue;}
    double GetDoubleValue() {return dValue;}
    TString GetStringValue() {return tstrValue;}
    TDatime GetDatimeValue() {return dtValue;}

 private:
    UniDbSearchCondition(){}

    enumColumns eColumn;
    enumConditions eCondition;

    // 1 - int, 2 - unsigned int, 3 - double, 4 - TString, 5 - TDatime
    int iValueType;
    int iValue;
    unsigned int uiValue;
    double dValue;
    TString tstrValue;
    TDatime dtValue;

 ClassDef(UniDbSearchCondition,1)
};

#endif
