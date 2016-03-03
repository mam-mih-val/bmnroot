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

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, double value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 2;
    dValue = value;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, TString value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 3;
    tstrValue = value;
}

UniDbSearchCondition::UniDbSearchCondition(enumColumns column, enumConditions condition, TDatime value)
{
    eColumn = column;
    eCondition = condition;

    iValueType = 4;
    dtValue = value;
}

// -------------------------------------------------------------------
ClassImp(UniDbSearchCondition);
