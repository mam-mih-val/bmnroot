// $Id: UniDbLinkDef.h,v

#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class UniDbConnection+;
#pragma link C++ class UniDbGeoConverter+;
#pragma link C++ class UniDbGenerateClasses+;
#pragma link C++ class TangoTimeParameter+;
#pragma link C++ class TangoTimeInterval+;
#pragma link C++ class UniDbTangoData+;
#pragma link C++ class UniDbSearchCondition+;
#pragma link C++ class UniDbParser+;
#pragma link C++ struct BeamSpillStructure++;
#pragma link C++ class vector<BeamSpillStructure*>+;

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

// the database structures
#pragma link C++ struct UniqueRunNumber++;
#pragma link C++ struct IIStructure++;
#pragma link C++ struct DchMapStructure++;
#pragma link C++ struct GemMapStructure++;
#pragma link C++ struct GemPedestalStructure++;
#pragma link C++ struct TriggerMapStructure++;
#pragma link C++ struct LorentzShiftStructure++;
// the database enums
#pragma link C++ enum enumColumns;
#pragma link C++ enum enumConditions;
#pragma link C++ enum enumParameterType;
#pragma link C++ enum UniConnectionType;

#endif

