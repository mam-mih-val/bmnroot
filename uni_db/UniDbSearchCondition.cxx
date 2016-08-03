#include "UniDbSearchCondition.h"

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 0;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, int value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 1;
    iValue = value;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, unsigned int value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 2;
    uiValue = value;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, double value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 3;
    dValue = value;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, TString value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 4;
    tstrValue = value;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, TDatime value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 5;
    dtValue = value;
}

// -------------------------------------------------------------------
ClassImp(UniDbSearchCondition);
