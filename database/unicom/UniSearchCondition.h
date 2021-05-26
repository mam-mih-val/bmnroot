#ifndef UNISEARCHCONDITION_H
#define UNISEARCHCONDITION_H 1

//#include "db_structures.h"

#include "TString.h"
#include "TDatime.h"

#include <vector>
using namespace std;

// enumeration of table columns for Search Conditions (UniDbRun::Search, UniDbDetectorParameters::Search function). *Begin and *End show the column range
enum enumColumns{columnPeriodNumber, columnRunNumber, columnFilePath, columnBeamParticle, columnTargetParticle, columnEnergy,   // UniDbRun
                 columnStartDatetime, columnEndDateTime, columnEventCount, columnFieldVoltage, columnFileSize,                  // UniDbRun
                 columnDetectorName, columnParameterName, columnStartPeriod, columnStartRun, columnEndPeriod, columnEndRun, columnValueKey,  // UniDbDetectorParameter
                 columnRecordDate, columnShiftLeader, columnType, columnTrigger, columnDaqStatus, columnSp41, columnSp57, columnVkm2,   // ElogDbRecord + {columnPeriodNumber,columnRunNumber,columnBeamParticle,columnEnergy,columnTargetParticle}
                 columnGeneratorName, columnCentrality, columnFileDesc, columnError};   // UniDbSimulationFile + {columnFilePath,columnBeamParticle,columnTargetParticle,columnEnergy,columnEventCount,columnFileSize}

// enumeration of comparison operators for Search Conditions
enum enumConditions{conditionLess, conditionLessOrEqual, conditionEqual, conditionNotEqual, conditionGreater, conditionGreaterOrEqual, conditionLike, conditionNull, conditionNotNull};


class UniSearchCondition
{
 public:
    UniSearchCondition(enumColumns column, enumConditions condition);
    UniSearchCondition(enumColumns column, enumConditions condition, int value);
    UniSearchCondition(enumColumns column, enumConditions condition, unsigned int value);
    UniSearchCondition(enumColumns column, enumConditions condition, double value);
    UniSearchCondition(enumColumns column, enumConditions condition, TString value);
    UniSearchCondition(enumColumns column, enumConditions condition, TDatime value);
    virtual ~UniSearchCondition(){}

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
    UniSearchCondition();

    enumColumns eColumn;
    enumConditions eCondition;

    // 1 - int, 2 - unsigned int, 3 - double, 4 - TString, 5 - TDatime
    int iValueType;
    int iValue;
    unsigned int uiValue;
    double dValue;
    TString tstrValue;
    TDatime dtValue;

 ClassDef(UniSearchCondition,1)
};

#endif
