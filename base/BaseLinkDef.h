// $Id: BmnBaseLinkDef.h,v
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class CbmStsPoint+;
#pragma link C++ class CbmDigi+;
#pragma link C++ class CbmBaseHit+;
#pragma link C++ class CbmHit+;
#pragma link C++ class CbmStripHit+;
#pragma link C++ class CbmVertex+;

#pragma link C++ class CbmStsTrack+;
#pragma link C++ class CbmStsTrackFinder+;
#pragma link C++ class CbmStsTrackFitter+;
#pragma link C++ class CbmTofMerger+;
#pragma link C++ class CbmTrackMerger+;
#pragma link C++ class CbmPrimaryVertexFinder+;
#pragma link C++ class CbmFindPrimaryVertex+;
#pragma link C++ class CbmTrackMatch+;
#pragma link C++ class CbmGlobalTrack+;

#pragma link C++ class CbmMvdDetectorId+;
#pragma link C++ class CbmMvdGeoPar+;
#pragma link C++ class CbmMvdPoint+;
#pragma link C++ class CbmMvdHit+;
#pragma link C++ class CbmMvdHitMatch+;

#pragma link C++ class CbmTofPoint+;
#pragma link C++ class CbmTofHit+;
#pragma link C++ class CbmTofTrack+;
#pragma link C++ class CbmGeoSttPar+;

#pragma link C++ class FitWLSQ+;
#pragma link C++ class BmnGeoNavigator+;
#pragma link C++ class BmnKalmanFilter+;

#pragma link C++ class BmnFunctionSet+;

#pragma link C++ class BmnFileSource+;
#pragma link C++ class BmnDecoSource+;
#pragma link C++ class BmnProfRawTools++;
#pragma link C++ class BmnProfilometerSource++;
#pragma link C++ class BmnOnlineShmSource+;

#pragma link C++ class BmnEventHeader+;
#pragma link C++ class BmnSpillHeader+;
#pragma link C++ class map<UInt_t, Long64_t>+;
#pragma link C++ class DigiRunHeader+;
#pragma link C++ class DstRunHeader+;
#pragma link C++ class DstEventHeader+;

#pragma link C++ class BmnADCDigit+;
#pragma link C++ class BmnTDCDigit+;
#pragma link C++ class BmnTQDCADCDigit+;
#pragma link C++ class BmnTTBDigit+;
#pragma link C++ class BmnMSCDigit+;
#pragma link C++ class BmnHRBDigit+;
#pragma link C++ class BmnSyncDigit+;
#pragma link C++ class BmnTrigDigit+;
#pragma link C++ class BmnTrigWaveDigit+;
#pragma link C++ class BmnTrigInfo+;
#pragma link C++ enum BmnTrigStructPeriod7SetupBMN;
#pragma link C++ enum BmnTrigStructPeriod7SetupSRC;
#pragma link C++ enum BmnTrigStructPeriod6;
#pragma link C++ class DigiArrays+;
#pragma link C++ class vector<TClonesArray*>+;

#pragma link C++ class CbmStack+;
#pragma link C++ class CbmMCTrack+;
#pragma link C++ class BmnStripDigit+;
#pragma link C++ class BmnCaloDigit+;
#pragma link C++ class BmnTacquilaDigit+;
#pragma link C++ class BmnDigiContainerTemplate+;
#pragma link C++ class StripCluster+;
#pragma link C++ class BmnHit+;
#pragma link C++ class BmnLink+;
#pragma link C++ class BmnMatch+;
#pragma link C++ class BmnTrack+;
#pragma link C++ class BmnIdentifiableTrack+;
#pragma link C++ class BmnTrackMatch+;
#pragma link C++ class BmnVertex+;
#pragma link C++ class BmnEventQuality+;
#pragma link C++ class BmnFitNode+;

#pragma link C++ class BmnTask+;
#pragma link C++ class BmnDetectorList+;
#pragma link C++ enum BmnSetup;

#endif
