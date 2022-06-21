// $Id: SsdLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Setup
#pragma link C++ class BmnSsdAddress;
#pragma link C++ class BmnSsdElement;
#pragma link C++ class BmnSsdModule;
#pragma link C++ class BmnSsdSensor;
#pragma link C++ class BmnSsdSensorConditions;
#pragma link C++ class BmnSsdSensorPoint;
#pragma link C++ class BmnSsdSetup;
#pragma link C++ class BmnSsdStation;

// Monte-Carlo
//#pragma link C++ class BmnSsdAcceptance;
#pragma link C++ class BmnSsdMC;
//#pragma link C++ class BmnSsdSensorFactory;
#pragma link C++ class BmnSsdTrackStatus;
//#pragma link C++ class BmnSsdMCQa;

// Digitisation
#pragma link C++ class BmnSsdDigitize;
#pragma link C++ class BmnSsdDigitizeParameters+;
#pragma link C++ class BmnSsdPhysics;
#pragma link C++ class BmnSsdSensorDssd;
//#pragma link C++ class BmnSsdSensorDssdOrtho;
#pragma link C++ class BmnSsdSensorDssdStereo;
#pragma link C++ class BmnSsdSignal;

// Reconstruction
//#pragma link C++ class BmnSsdClusterAnalysis;
//#pragma link C++ class BmnSsdClusterFinderModule;
//#pragma link C++ class BmnSsdFindClusters;
//#pragma link C++ class BmnSsdFindHits;
//#pragma link C++ class BmnSsdFindHitsEvents;
//#pragma link C++ class BmnSsdFindTracksEvents;
//#pragma link C++ class BmnSsdMatchReco;
//#pragma link C++ class BmnSsdRecoQa;
//#pragma link C++ class BmnSsdTestQa;

// Others
//#pragma link C++ class BmnSsdContFact;
//#pragma link C++ class BmnSsdFindTracks+;
//#pragma link C++ class BmnSsdFindTracksQa+;
//#pragma link C++ class BmnSsdFitTracks+;
//#pragma link C++ class BmnSsdHitProducerIdeal+;
//#pragma link C++ class BmnSsdMatchTracks+;
//#pragma link C++ class BmnSsdRadTool;
//#pragma link C++ class BmnSsdTrackFinderIdeal+;
//#pragma link C++ class BmnSsdTrackFitterIdeal+;
//#pragma link C++ class BmnSsdSimulationQa+;
//#pragma link C++ class BmnSsdDigitizeQa+;
//#pragma link C++ class BmnSsdDigitizeQaReport+;
//#pragma link C++ class BmnSsdTimeBasedQa;
//#pragma link C++ class BmnSsdTimeBasedQaReport;

#pragma link C++ class BmnModuleList;
#pragma link C++ class BmnAddress;
#pragma link C++ class BmnSsdCluster;
#pragma link C++ class BmnCluster;
#pragma link C++ class BmnSsdDigi;
#pragma link C++ class BmnDigi;
#pragma link C++ class BmnEvent;
#pragma link C++ class BmnVertex;
#pragma link C++ class BmnSsdPoint;
#pragma link C++ class BmnDigitize;
#pragma link C++ class BmnDaqBuffer;
#pragma link C++ class BmnSsdHit;
#pragma link C++ class BmnPixelHit;
#pragma link C++ class BmnBaseHit;

#endif /* __CINT__ */

