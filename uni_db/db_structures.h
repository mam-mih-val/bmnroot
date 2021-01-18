#ifndef DB_STRUCTURES_H
#define DB_STRUCTURES_H

#include "UniValue.h"

#include <string>
#include <cstring>
#include <cstdint>
using namespace std;

// unique run number = period_number : run_number
struct UniqueRunNumber
{
    UniqueRunNumber() { period_number = 0; run_number = 0;}
    UniqueRunNumber(int period_num, int run_num) { period_number = period_num; run_number = run_num; }
    int period_number;
    int run_number;
};

// enumeration 'enumParameterType' is corresponding to parameter_type member UniDbParameter
// 0 - boolean, 1 - integer, 2 - double, 3 - string, 4 - int+int array, 5 - int array, 6 - double array, 7 - any binary array, 8 - unsigned int array
// 9 - array with DCH mapping, 10 - array with GEM mapping, 11 - array with GEM pedestal map, 12 - array with Trigger mapping, 13 - array with Lorentz shift
enum enumParameterType{BoolType, IntType, DoubleType, StringType, IIArrayType, IntArrayType, DoubleArrayType, BinaryArrayType, UIntArrayType,   // base types
                       DchMapArrayType, GemMapArrayType, GemPedestalArrayType, TriggerMapArrayType, LorentzShiftArrayType, ErrorType = 999};    // detector-dependent types

// enumeration 'enumValueType' is corresponding to 'parameter_type' member of the UniDbParameter class, types:
// 0 - boolean, 1 - integer, 2 - unsigned integer, 3 - double, 4 - string, 5 - binary, 6 - int+int,
// 7 - DCH mapping, 8 - GEM mapping, 9 - GEM pedestal mapping, 10 - trigger mapping, 11 - Lorentz shift,
// 12 - mapping with bool value (serial+channel+bool), 13 - mapping with int, 14 - mapping with double vector
enum enumValueType : unsigned int
{
    BoolTypeNew = 0, IntTypeNew, UIntTypeNew, DoubleTypeNew, StringTypeNew, BinaryTypeNew, IITypeNew,               // base types
    DchMapTypeNew, GemMapTypeNew, GemPedestalTypeNew, TriggerMapTypeNew, LorentzShiftTypeNew,                       // detector-dependent types
    MapBoolTypeNew, MapIntTypeNew, MapDVectorTypeNew, UndefinedType = 999                                           // detector-dependent types
};


/** STRUCTURES for parameter storing corresponding enumValueType list **/
struct BoolValue : public UniValue
{
    uint8_t value;

    enumValueType GetType() { return BoolTypeNew; }
    size_t GetStorageSize() { return 1; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct IntValue : public UniValue
{
    int32_t value;

    enumValueType GetType() { return IntTypeNew; }
    size_t GetStorageSize() { return 4; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct UIntValue : public UniValue
{
    uint32_t value;

    enumValueType GetType() { return UIntTypeNew; }
    size_t GetStorageSize() { return 4; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct DoubleValue : public UniValue
{
    double value;

    enumValueType GetType() { return DoubleTypeNew; }
    size_t GetStorageSize() { return 8; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct StringValue : public UniValue
{
    string value;

    enumValueType GetType() { return StringTypeNew; }
    size_t GetStorageSize() { return value.length()+1; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct BinaryValue : public UniValue
{
    uint64_t size;
    unsigned char* value;

    enumValueType GetType() { return BinaryTypeNew; }
    size_t GetStorageSize() { return size + 8; }
    void ReadValue(unsigned char* source)       { Read(source, value, size); }
    void WriteValue(unsigned char* destination) { Write(destination, value, size); }
};

struct IIValue : public UniValue
{
    int32_t value1;
    int32_t value2;

    enumValueType GetType() { return IITypeNew; }
    size_t GetStorageSize() { return 8; }
    void ReadValue(unsigned char* source)       { Read(source, value1);       Read(source, value2); }
    void WriteValue(unsigned char* destination) { Write(destination, value1); Write(destination, value2); }
};

struct DchMapValue : public UniValue
{
    int32_t plane;
    int32_t group;
    uint32_t crate;
    int32_t slot;
    int32_t channel_low;
    int32_t channel_high;

    enumValueType GetType() { return DchMapTypeNew; }
    size_t GetStorageSize() { return 24; }
    void ReadValue(unsigned char* source)       { Read(source, plane);       Read(source, group);       Read(source, crate);       Read(source, slot);       Read(source, channel_low);       Read(source, channel_high); }
    void WriteValue(unsigned char* destination) { Write(destination, plane); Write(destination, group); Write(destination, crate); Write(destination, slot); Write(destination, channel_low); Write(destination, channel_high); }
};

struct GemMapValue : public UniValue
{
    uint32_t serial;
    int32_t id;
    int32_t station;
    int32_t channel_low;
    int32_t channel_high;
    int32_t hotZone;

    enumValueType GetType() { return GemMapTypeNew; }
    size_t GetStorageSize() { return 24; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, id);       Read(source, station);       Read(source, channel_low);       Read(source, channel_high);       Read(source, hotZone); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, id); Write(destination, station); Write(destination, channel_low); Write(destination, channel_high); Write(destination, hotZone); }
};

struct GemPedestalValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    int32_t pedestal;
    int32_t noise;

    enumValueType GetType() { return GemPedestalTypeNew; }
    size_t GetStorageSize() { return 16; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, pedestal);       Read(source, noise); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, pedestal); Write(destination, noise); }
};

struct TriggerMapValue : public UniValue
{
    uint32_t serial;
    uint32_t slot;
    int32_t channel;

    enumValueType GetType() { return TriggerMapTypeNew; }
    size_t GetStorageSize() { return 12; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, slot);       Read(source, channel); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, slot); Write(destination, channel); }
};

struct MapBoolValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    uint8_t value;

    enumValueType GetType() { return MapBoolTypeNew; }
    size_t GetStorageSize() { return 9; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, value); }
};

struct MapIntValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    int32_t value;

    enumValueType GetType() { return MapIntTypeNew; }
    size_t GetStorageSize() { return 12; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, value); }
};

struct MapDVectorValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    vector<double> value;   // vector is stored with the size

    enumValueType GetType() { return MapDVectorTypeNew; }
    size_t GetStorageSize() { return value.size()*8 + 16; } // (value.size()*8+8) + 8
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, value); }
};

struct LorentzShiftValue : public UniValue
{
    int32_t number;
    double ls[3];

    enumValueType GetType() { return LorentzShiftTypeNew; }
    size_t GetStorageSize() { return 28; }
    void ReadValue(unsigned char* source)       { Read(source, number);       Read(source, ls, 3); }
    void WriteValue(unsigned char* destination) { Write(destination, number); Write(destination, ls, 3); }
};

#ifndef CREATE_PARAMETER_VALUE_H
#define CREATE_PARAMETER_VALUE_H
// global function for creating value according to a given type name (parameter_type)
inline UniValue* CreateParameterValue(enumValueType parameter_type)
{
    switch (parameter_type)
    {
        case BoolTypeNew: return new BoolValue;
        case IntTypeNew: return new IntValue;
        case UIntTypeNew: return new UIntValue;
        case DoubleTypeNew: return new DoubleValue;
        case StringTypeNew: return new StringValue;
        case BinaryTypeNew: return new BinaryValue;
        case IITypeNew: return new IIValue;
        case DchMapTypeNew: return new DchMapValue;
        case GemMapTypeNew: return new GemMapValue;
        case GemPedestalTypeNew: return new GemPedestalValue;
        case TriggerMapTypeNew: return new TriggerMapValue;
        case LorentzShiftTypeNew: return new LorentzShiftValue;
        case MapBoolTypeNew: return new MapBoolValue;
        case MapIntTypeNew: return new MapIntValue;
        case MapDVectorTypeNew: return new MapDVectorValue;
        default: break;
    }

    cout<<"ERROR: The given value type is not supported!"<<endl;
    return NULL;
}
#endif

// OLD PART (to delete)
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

struct LorentzShiftStructure
{
    int number;
    double ls[3];
};


#endif // DB_STRUCTURES_H
