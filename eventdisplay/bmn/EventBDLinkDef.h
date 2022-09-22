// $Id: EventBDLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class MpdXMLAttrib;
#pragma link C++ class MpdXMLNode;
#pragma link C++ class MpdXMLFile;
#pragma link C++ class MpdEventManager;
#pragma link C++ class MpdMCTracks;
#pragma link C++ class MpdMCStack;
#pragma link C++ class MpdMCTracksEditor;
#pragma link C++ class MpdEventManagerEditor;
#pragma link C++ class MpdBoxSetDraw;
#pragma link C++ class MpdPointSetDraw;
#pragma link C++ class MpdHitDraw;
#pragma link C++ class MpdBoxSet;
#pragma link C++ class MpdBoxSetEditor;
#pragma link C++ class MpdHitPointSetDraw;
#pragma link C++ class MpdMCPointDraw;
#pragma link C++ class MpdWebScreenshots;

// BmnRoot
#pragma link C++ class FairMCModuleDraw;
#pragma link C++ class BmnTrackDrawH;
#pragma link C++ class BmnTrackDrawP;
#pragma link C++ class BmnGlobalTrackDraw;
#pragma link C++ class BmnCaloTowerDraw;

// TDAQ
#pragma link C++ class BmnEventPull+;
//#pragma link C++ class BmnEventMonitor+;
#pragma link C++ class BmnTdaqSource+;

#endif
