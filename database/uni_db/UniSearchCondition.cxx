#include "UniSearchCondition.h"

UniSearchCondition::UniSearchCondition(enumColumns column, enumConditions condition) :
    eColumn(column),
    eCondition(condition),
    iValueType(0)
{}

UniSearchCondition::UniSearchCondition(enumColumns column, enumConditions condition, int value) :
    eColumn(column),
    eCondition(condition),
    iValueType(1),
    iValue(value)
{}

UniSearchCondition::UniSearchCondition(enumColumns column, enumConditions condition, unsigned int value) :
    eColumn(column),
    eCondition(condition),
    iValueType(2),
    uiValue(value)
{}

UniSearchCondition::UniSearchCondition(enumColumns column, enumConditions condition, double value) :
    eColumn(column),
    eCondition(condition),
    iValueType(3),
    dValue(value)
{}

UniSearchCondition::UniSearchCondition(enumColumns column, enumConditions condition, TString value) :
    eColumn(column),
    eCondition(condition),
    iValueType(4),
    tstrValue(value)
{}

UniSearchCondition::UniSearchCondition(enumColumns column, enumConditions condition, TDatime value) :
    eColumn(column),
    eCondition(condition),
    iValueType(5),
    dtValue(value)
{}

// private default constructor
UniSearchCondition::UniSearchCondition() :
    eColumn(columnError),
    eCondition(conditionEqual),
    iValueType(0)
{}

ClassImp(UniSearchCondition);
