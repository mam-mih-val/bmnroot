#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class BmnGemHit+;
#pragma link C++ class BmnGemHitProducer+;
#pragma link C++ class BmnGemGeo+;
#pragma link C++ class BmnGemDigit+;
#pragma link C++ class BmnGemDigitizerTask+;
#pragma link C++ class BmnGemGas+;
#pragma link C++ class BmnGemDigitizerQAHistograms+;
#pragma link C++ class BmnGemTrack+;
#pragma link C++ class BmnGemSeedFinder+;
#pragma link C++ class BmnGemSeedFinderQA+;
#pragma link C++ class BmnGemTrackFinderQA+;
#pragma link C++ class BmnGemTrackFinder+;
#pragma link C++ class BmnGemStripLayer+;
#pragma link C++ class BmnGemStripModule+;
#pragma link C++ class BmnGemStripStation+;
#pragma link C++ class BmnGemStripStationSet+;
#pragma link C++ class BmnGemStripStation_RunSummer2016+;
#pragma link C++ class BmnGemStripStationSet_RunSummer2016+;
#pragma link C++ class BmnGemStripStation_RunWinter2016+;
#pragma link C++ class BmnGemStripStationSet_RunWinter2016+;
#pragma link C++ class BmnGemStripStation_RunSpring2017+;
#pragma link C++ class BmnGemStripStationSet_RunSpring2017+;
#pragma link C++ class BmnGemStripDigit+;
#pragma link C++ class BmnGemStripDigitizer+;
#pragma link C++ class BmnGemStripHit+;
#pragma link C++ class BmnGemStripHitMaker+;
#pragma link C++ class BmnKalmanFilter_tmp+;
#pragma link C++ class BmnGemVertexFinder+;

#pragma link C++ namespace BmnGemStripConfiguration;
#pragma link C++ enum ElectronDriftDirectionInModule;
#pragma link C++ enum StripNumberingDirection;
#pragma link C++ enum StripBorderPoint;
#pragma link C++ enum StripLayerType;

#pragma link C++ class DeadZoneOfStripLayer;
#pragma link C++ class vector<DeadZoneOfStripLayer>;
#pragma link C++ class vector<StripCluster>; //test
#pragma link C++ class vector<vector<StripCluster> >; //test
#pragma link C++ class vector<BmnGemStripLayer>; //test
#pragma link C++ class StripCluster; //test

#endif
