#ifndef DB_STRUCTURES_H
#define DB_STRUCTURES_H

struct UniqueRunNumber
{
    int period_number;
    int run_number;
};

// enumeration of table columns for Search Conditions (UniDbRun::Search, UniDbDetectorParameters::Search function). *Begin and *End show the column range
enum enumColumns{columnPeriodNumber, columnRunNumber, columnFilePath, columnBeamParticle, columnTargetParticle, columnEnergy,   // UniDbRun
                 columnStartDatetime, columnEndDateTime, columnEventCount, columnFieldVoltage, columnFileSize,                  // UniDbRun
                 columnDetectorName, columnParameterName, columnStartPeriod, columnStartRun, columnEndPeriod, columnEndRun, columnDCSerial, columnChannel}; // UniDbDetectorParameter
// enumeration of comparison operators for Search Conditions
enum enumConditions{conditionLess, conditionLessOrEqual, conditionEqual, conditionNotEqual, conditionGreater, conditionGreaterOrEqual, conditionLike, conditionNull};

// enumeration 'enumParameterType' is corresponding parameter_type member UniDbParameter
// 0 - boolean, 1 - integer, 2 - double, 3 - string, 4 - int+int array, 5 - int array, 6 - double array, 7 - any binary array, 8 - unsigned int array
// 9 - array with DCH mapping, 10 - array with GEM mapping, 11 - array with GEM pedestal map, 12 - array with Trigger mapping
enum enumParameterType{BoolType, IntType, DoubleType, StringType, IIArrayType, IntArrayType, DoubleArrayType, BinaryArrayType, UIntArrayType,   // base types
                       DchMapArrayType, GemMapArrayType, GemPedestalArrayType, TriggerMapArrayType, ErrorType = 999};                           // detector-dependent types

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
};

struct GemMapStructure
{
    unsigned int serial;
    int id;
    int station;
    int channel_low;
    int channel_high;
    int hotZone;
};

struct GemPedestalStructure
{
    unsigned int serial;
    int channel;
    int pedestal;
    int noise;
};

struct TriggerMapStructure
{
    unsigned int serial;
    unsigned int slot;
    int channel;
};


#endif // DB_STRUCTURES_H
