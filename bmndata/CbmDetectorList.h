/** @file CbmDetectorList.h
 ** @author V.Friese  <V.Friese@Gsi.De>
 ** @date 12.06.2007
 ** @brief Defines unique identifiers (enum) for all CBM detector systems
 **/


#ifndef CBMDETECTORLIST_H
#define CBMDETECTORLIST_H 1


/**  BMN DetectorID enumerator  **/
enum DetectorId {kREF,  // Reference plane
         kMVD,          // future BMN silicone detector
         kGEM,          // GEMS detectors
         kTOF1,         // Front Time-of-flight Detector
         kDCH,          // DCH
         kTOF,          // Rear Time-of-flight Detector
         kZDC,          // ZDC
         kRECOIL,       // Recoil
         kMWPC,         // MWPC
         kBD,           // Barrel
         kECAL,         // ECAL
         kSILICON,      // Silicon
         kCSC,          // CSC
         kFD,           // FD
         kSSD,          // SSD (Strip Silicon Detector)
         kARMTRIG,      // ARMTRIG(X1, Y1, X2, Y2 for SRC arms)
         kNOFDETS}; // Number of elements (e.g. for loops)


/**   Data type enumerator  **/
enum DataType {kMCTrack,
               kMvdPoint, kMvdDigi, kMvdCluster, kMvdHit,             // MVD
               kStsPoint, kStsDigi, kStsCluster, kStsHit, kStsTrack,  // STS
               kSsdPoint, kSsdDigi, kSsdCluster, kSsdHit, kSsdTrack,  // SSD
               kRichPoint, kRichDigi, kRichHit, kRichRing,            // RICH
               kMuchDigi, kMuchPixelHit, kMuchStrawHit, kMuchTrack,   // MUCH
               kTrdDigi, kTrdHit, kTrdTrack,                          // TRD
               kTofPoint, kTofDigi, kTofCalDigi, kTofHit, kTofTrack,  // TOF
               kPsdPoint, kPsdDigi,                                   // PSD
               kGlobalTrack                                           // Global
};

#include "TObject.h"


/** @class CbmDetectorList
 ** @author V.Friese  <V.Friese@Gsi.De>
 ** @date 29.04.2010
 **
 ** @brief Provides some utility functions for DetectorId
 **/
class CbmDetectorList : public TObject
{

 public:

  /**   Constructor   **/
  CbmDetectorList();


  /**   Destructor  **/
  virtual ~CbmDetectorList() { }


  /**   Get system name
   *@param det   System identifier (type DetectorId)
   *@param name  (return) System name (lower case)
   **/
  static void GetSystemName(DetectorId det, TString& name);
  static void GetSystemName(Int_t det, TString& name);


  /** Get system name in capitals
   ** @param det   System identifier (type DetectorId)
   ** @param name  (return) System name (lower case)
   **/
  static void GetSystemNameCaps(DetectorId det, TString& name);
  static void GetSystemNameCaps(Int_t det, TString& name);


  ClassDef(CbmDetectorList, 1);
};


#endif
