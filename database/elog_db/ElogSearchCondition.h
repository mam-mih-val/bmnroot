#ifndef ELOGSEARCHCONDITION_H
#define ELOGSEARCHCONDITION_H 1

#include "TString.h"
#include "TDatime.h"

class ElogSearchCondition
{
  public:
    // enumeration of table columns for Search Conditions (ion). *Begin and *End show the column range
    enum enumColumns{columnRecordDate, columnType, columnShiftLeader, columnPeriodNumber, columnRunNumber, columnTrigger, columnDaqStatus,
                     columnBeamParticle, columnEnergy, columnTargetParticle,
                     columnSp41, columnSp57, columnVkm2, columnError};
    enum enumConditions {conditionLess, conditionLessOrEqual, conditionEqual, conditionNotEqual, conditionGreater,
                         conditionGreaterOrEqual, conditionLike, conditionNull, conditionNotNull};

    ElogSearchCondition(enumColumns column, enumConditions condition);
    ElogSearchCondition(enumColumns column, enumConditions condition, int value);
    ElogSearchCondition(enumColumns column, enumConditions condition, unsigned int value);
    ElogSearchCondition(enumColumns column, enumConditions condition, double value);
    ElogSearchCondition(enumColumns column, enumConditions condition, TString value);
    ElogSearchCondition(enumColumns column, enumConditions condition, TDatime value);
    virtual ~ElogSearchCondition(){}

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
    ElogSearchCondition();

    enumColumns eColumn;
    enumConditions eCondition;

    // 1 - int, 2 - unsigned int, 3 - double, 4 - TString, 5 - TDatime
    int iValueType;
    int iValue;
    unsigned int uiValue;
    double dValue;
    TString tstrValue;
    TDatime dtValue;

  ClassDef(ElogSearchCondition,1)
};

#endif
