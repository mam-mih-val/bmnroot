#ifndef DB_STRUCTURES_H
#define DB_STRUCTURES_H

// enumeration 'enumParameterType' is corresponding parameter_type member UniDbParameter
// 0 - boolean, 1 - integer, 2 - double, 3 - string, 4 - int+int array, 5 - int array, 6 - double array
enum enumParameterType{BoolType, IntType, DoubleType, StringType, IIArrayType, IntArrayType, DoubleArrayType, BinaryArrayType, UIntArrayType, DchMapArrayType, GemMapArrayType, ErrorType = 999};

struct IIStructure
{
    int int_1;
    int int_2;
};//__attribute__((packed));

struct DchMapStructure
{
    int plane;
    int group;
    unsigned int crate;
    int slot;
    int channel_low;
    int channel_high;
};//__attribute__((packed));

struct GemMapStructure
{
    unsigned int serial;
    int id;
    int station;
    int channel_low;
    int channel_high;
    int hotZone;
};//__attribute__((packed));


// enumeration of table columns for Search Conditions (UniDbRun::Search, UniDbDetectorParameters::Search function). *Begin and *End show the column range
enum enumColumns{columnPeriodNumber, columnRunNumber, columnFilePath, columnBeamParticle, columnTargetParticle, columnEnergy,   // UniDbRun
                 columnStartDatetime, columnEndDateTime, columnEventCount, columnFieldCurrent, columnFileSize,                  // UniDbRun
                 columnDetectorName, columnParameterName, columnStartPeriod, columnStartRun, columnEndPeriod, columnEndRun, columnDCSerial, columnChannel}; // UniDbDetectorParameter
// enumeration of comparison operators for Search Conditions
enum enumConditions{conditionLess, conditionLessOrEqual, conditionEqual, conditionNotEqual, conditionGreater, conditionGreaterOrEqual, conditionLike, conditionNull};


struct UniqueRunNumber
{
    int period_number;
    int run_number;
};

#endif // DB_STRUCTURES_H
