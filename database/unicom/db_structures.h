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

// enumeration 'enumValueType' is corresponding to 'parameter_type' member of the UniDbParameter class, types:
// 0 - boolean, 1 - integer, 2 - unsigned integer, 3 - double, 4 - string, 5 - binary, 6 - int+int,
// 7 - DCH mapping, 8 - GEM mapping, 9 - GEM pedestal mapping, 10 - trigger mapping, 11 - Lorentz shift,
// 12 - mapping with bool value (serial+channel+bool), 13 - mapping with int, 14 - mapping with double vector, 15 - alignment type, 16 - TDC INL values
enum enumValueType : unsigned int
{
    BoolType = 0, IntType, UIntType, DoubleType, StringType, BinaryType, IIType,        // base types
    DchMapType, GemMapType, GemPedestalType, TriggerMapType, LorentzShiftType,          // detector-dependent types
    MapBoolType, MapIntType, MapDVectorType, AlignmentType, TdcInlType,                 // detector-dependent types
    UndefinedType = 999
};


/** STRUCTURES for parameter storing corresponding enumValueType list **/
struct BoolValue : public UniValue
{
    uint8_t value;

    enumValueType GetType() { return BoolType; }
    size_t GetStorageSize() { return 1; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct IntValue : public UniValue
{
    int32_t value;

    enumValueType GetType() { return IntType; }
    size_t GetStorageSize() { return 4; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct UIntValue : public UniValue
{
    uint32_t value;

    enumValueType GetType() { return UIntType; }
    size_t GetStorageSize() { return 4; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct DoubleValue : public UniValue
{
    double value;

    enumValueType GetType() { return DoubleType; }
    size_t GetStorageSize() { return 8; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct StringValue : public UniValue
{
    string value;

    enumValueType GetType() { return StringType; }
    size_t GetStorageSize() { return value.length()+1; }
    void ReadValue(unsigned char* source)       { Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, value); }
};

struct BinaryValue : public UniValue
{
    uint64_t size;
    unsigned char* value;   // char array is stored with the size variable (+8)

    enumValueType GetType() { return BinaryType; }
    size_t GetStorageSize() { return size + 8; }
    void ReadValue(unsigned char* source)       { Read(source, value, size); }
    void WriteValue(unsigned char* destination) { Write(destination, value, size); }
};

struct IIValue : public UniValue
{
    int32_t value1;
    int32_t value2;

    enumValueType GetType() { return IIType; }
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

    enumValueType GetType() { return DchMapType; }
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

    enumValueType GetType() { return GemMapType; }
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

    enumValueType GetType() { return GemPedestalType; }
    size_t GetStorageSize() { return 16; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, pedestal);       Read(source, noise); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, pedestal); Write(destination, noise); }
};

struct TriggerMapValue : public UniValue
{
    uint32_t serial;
    uint32_t slot;
    int32_t channel;

    enumValueType GetType() { return TriggerMapType; }
    size_t GetStorageSize() { return 12; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, slot);       Read(source, channel); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, slot); Write(destination, channel); }
};

struct MapBoolValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    uint8_t value;

    enumValueType GetType() { return MapBoolType; }
    size_t GetStorageSize() { return 9; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, value); }
};

struct MapIntValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    int32_t value;

    enumValueType GetType() { return MapIntType; }
    size_t GetStorageSize() { return 12; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, value); }
};

struct MapDVectorValue : public UniValue
{
    uint32_t serial;
    int32_t channel;
    vector<double> value;   // vector is stored with the size (+8)

    enumValueType GetType() { return MapDVectorType; }
    size_t GetStorageSize() { return value.size()*8 + 16; } // (value.size()*8+8) + 8
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, channel);       Read(source, value); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, channel); Write(destination, value); }
};

struct LorentzShiftValue : public UniValue
{
    int32_t number;
    double ls[3];

    enumValueType GetType() { return LorentzShiftType; }
    size_t GetStorageSize() { return 28; }
    void ReadValue(unsigned char* source)       { Read(source, number);       Read(source, ls, 3); }
    void WriteValue(unsigned char* destination) { Write(destination, number); Write(destination, ls, 3); }
};

struct AlignmentValue : public UniValue
{
    uint8_t station;
    uint8_t module;
    double value[3];

    enumValueType GetType() { return AlignmentType; }
    size_t GetStorageSize() { return 26; }
    void ReadValue(unsigned char* source)       { Read(source, station); Read(source, module); Read(source, value, 3); }
    void WriteValue(unsigned char* destination) { Write(destination, station); Write(destination, module); Write(destination, value, 3); }
};

struct TdcInlValue : public UniValue
{
    uint32_t serial;
    double inl[72][1024];

    enumValueType GetType() { return TdcInlType; }
    size_t GetStorageSize() { return 589828; }
    void ReadValue(unsigned char* source)       { Read(source, serial);       Read(source, inl); }
    void WriteValue(unsigned char* destination) { Write(destination, serial); Write(destination, inl); }
};

/*struct AlignmentValue : public UniValue
{
    uint8_t n_stations;
    uint8_t n_modules;
    uint8_t n_space;
    double*** value;

    enumValueType GetType() { return AlignmentType; }
    size_t GetStorageSize() { return n_stations*n_modules*n_space*8+3; }
    void ReadValue(unsigned char* source)       { Read(source, value, n_stations, n_modules, n_space); }
    void WriteValue(unsigned char* destination) { Write(destination, value, n_stations, n_modules, n_space); }
};*/

#ifndef CREATE_PARAMETER_VALUE_H
#define CREATE_PARAMETER_VALUE_H
// global function for creating value according to a given type name (parameter_type)
inline UniValue* CreateParameterValue(enumValueType parameter_type)
{
    switch (parameter_type)
    {
        case BoolType: return new BoolValue;
        case IntType: return new IntValue;
        case UIntType: return new UIntValue;
        case DoubleType: return new DoubleValue;
        case StringType: return new StringValue;
        case BinaryType: return new BinaryValue;
        case IIType: return new IIValue;
        case DchMapType: return new DchMapValue;
        case GemMapType: return new GemMapValue;
        case GemPedestalType: return new GemPedestalValue;
        case TriggerMapType: return new TriggerMapValue;
        case LorentzShiftType: return new LorentzShiftValue;
        case MapBoolType: return new MapBoolValue;
        case MapIntType: return new MapIntValue;
        case MapDVectorType: return new MapDVectorValue;
        case AlignmentType: return new AlignmentValue;
        case TdcInlType: return new TdcInlValue;
        default: break;
    }

    cout<<"ERROR: The given value type is not supported!"<<endl;
    return NULL;
}
#endif

#endif // DB_STRUCTURES_H
