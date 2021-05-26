// $Id: UniCommonLinkDef.h,v

#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class UniConnection+;
#pragma link C++ class UniGeoConverter+;
#pragma link C++ class UniGenerateClasses+;
#pragma link C++ class UniSearchCondition+;

// the database enums
#pragma link C++ enum enumColumns;
#pragma link C++ enum enumConditions;
#pragma link C++ enum enumParameterType;
#pragma link C++ enum UniConnectionType;
#pragma link C++ enum enumValueType;

// the database structures
#pragma link C++ struct UniqueRunNumber++;
#pragma link C++ struct UniValue++;
#pragma link C++ struct BoolValue++;
#pragma link C++ struct IntValue++;
#pragma link C++ struct UIntValue++;
#pragma link C++ struct DoubleValue++;
#pragma link C++ struct StringValue++;
#pragma link C++ struct BinaryValue++;
#pragma link C++ struct IIValue++;
#pragma link C++ struct DchMapValue++;
#pragma link C++ struct GemMapValue++;
#pragma link C++ struct GemPedestalValue++;
#pragma link C++ struct TriggerMapValue++;
#pragma link C++ struct LorentzShiftValue++;
#pragma link C++ struct MapBoolValue++;
#pragma link C++ struct MapIntValue++;
#pragma link C++ struct MapDVectorValue++;

#endif

