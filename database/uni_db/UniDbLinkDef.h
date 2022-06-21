// $Id: UniDbLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class UniDbRunPeriod+;
#pragma link C++ class UniDbRun+;
#pragma link C++ class UniDbRunGeometry+;
#pragma link C++ class UniDbDetector+;
#pragma link C++ class UniDbParameter+;
#pragma link C++ class UniDbDetectorParameter+;
#pragma link C++ class UniDbSimulationFile+;

#pragma link C++ class UniConnection+;
#pragma link C++ class UniSearchCondition+;
//#pragma link C++ struct structParseSchema++;
//#pragma link C++ class vector<structParseSchema>+;
//#pragma link C++ struct structParseValue++;
//#pragma link C++ class vector<structParseValue*>+;

// the database enums
#pragma link C++ enum UniColumns;
#pragma link C++ enum enumValueType;
#pragma link C++ enum ConnectionType;
//#pragma link C++ enum enumParameterType;
#ifndef enumConditions_link
#define enumConditions_link 1
#pragma link C++ enum enumConditions;
#endif

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
