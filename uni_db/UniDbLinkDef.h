// $Id: UniDbLinkDef.h,v

#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class UniConnection+;
#pragma link C++ class UniDbGeoConverter+;
#pragma link C++ class UniDbGenerateClasses+;
#pragma link C++ class TangoTimeParameter+;
#pragma link C++ class TangoTimeInterval+;
#pragma link C++ class UniDbTangoData+;
#pragma link C++ class UniDbSearchCondition+;

#pragma link C++ class UniDbParser+;
//#pragma link C++ struct structParseSchema++;
//#pragma link C++ class vector<structParseSchema>+;
//#pragma link C++ struct structParseValue++;
//#pragma link C++ class vector<structParseValue*>+;

#pragma link C++ class UniDbRunPeriod+;
#pragma link C++ class UniDbRun+;
#pragma link C++ class UniDbRunGeometry+;
#pragma link C++ class UniDbDetector+;
#pragma link C++ class UniDbParameter+;
#pragma link C++ class UniDbDetectorParameter+;
#pragma link C++ class UniDbSimulationFile+;

#pragma link C++ class ElogDbPerson+;
#pragma link C++ class ElogDbType+;
#pragma link C++ class ElogDbTrigger+;
#pragma link C++ class ElogDbBeam+;
#pragma link C++ class ElogDbTarget+;
#pragma link C++ class ElogDbRecord+;
#pragma link C++ class ElogDbAttachment+;

// the database enums
#pragma link C++ enum enumColumns;
#pragma link C++ enum enumConditions;
#pragma link C++ enum enumParameterType;
#pragma link C++ enum UniConnectionType;

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

