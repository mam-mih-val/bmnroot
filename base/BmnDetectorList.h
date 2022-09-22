/** @file BmnDetectorList.h
 ** @brief Defines unique identifiers (enum) for all BM@N detector systems
 **/

#ifndef BMNDETECTORLIST_H
#define BMNDETECTORLIST_H 1

#include "TObject.h"

/**  BM@N DetectorID enumerator  **/
enum DetectorId {kREF,  // Reference plane
         kBD,           // Barrel Detector
         kGEM,          // GEMs detector
         kTOF1,         // Front Time-of-Flight Detector
         kDCH,          // Drift Chambers
         kTOF,          // Rear Time-of-Flight Detector
         kZDC,          // Zero Degree Calorimeter
         kSSD,          // SSD (Strip Silicon Detector)
         kMWPC,         // MWPC
         kECAL,         // Electromagnetic Calorimeter
         kCSC,          // Cathode Strip Chambers
         kSILICON,      // Silicon Detectors
         kLAND,         // Large-Area Neutron Detector
         kTOFCAL,       // SRC ToF-Calorimeter
         kFD,           // Forward Detector (= SiMD until run8), Fragment Detector since run8
         kSiMD,         // SiMD (Silicon Multiplicity Detector, since run8)
         kSiBT,         // SiBD (Silicon Beam Tracker)
         kARMTRIG,      // ARMTRIG(X1, Y1, X2, Y2 for SRC arms)
         kBC,           // Beam Counter triggers
         kSCWALL,       // SCintillator WALL
         kHODO,         // HODOscope
         kFHCAL,        // Forward Hadron Calorimeter
         kNDET,         // Neutron detector
         kNOFDETS       // Number of elements (e.g. for loops)
};


/**   Data type enumerator  **/
enum DataType {kMCTrack,
               kGemPoint, kGemDigi, kGemCluster, kGemHit, kGemTrack,  // GEM
               kTof1Point, kTof1Digi, kTof1Hit, kTof1Track,           // TOF1
               kDchPoint, kDchDigi, kDchHit, kDchTrack,               // DCH
               kTofPoint, kTofDigi, kTofHit, kTofTrack,               // TOF
               kZdcPoint, kZdcDigi,                                   // ZDC
               kSsdPoint, kSsdDigi, kSsdCluster, kSsdHit, kSsdTrack,  // SSD
               kEcalPoint, kEcalDigi,                                 // ECAL
               kCscPoint, kCscDigi, kCscHit, kCscTrack,               // CSC
               kStsPoint, kStsDigi, kStsCluster, kStsHit, kStsTrack,  // STS
               kLandPoint, kLandDigi, kLandHit,                       // LAND
               kTofCalPoint, kTofCalDigi, kTofCalHit,                 // ToF-Cal
               kFHCalPoint, kFHCalDigi,                               // FHCAL
               kNdetPoint, kNdetDigi,                                 // NDET
               kGlobalTrack                                           // Global
};


/** @class BmnDetectorList
 ** @brief Provides some utility functions for DetectorId
 **/
class BmnDetectorList : public TObject
{
  public:
    /**   Constructor   **/
    BmnDetectorList() {}

    /**   Destructor  **/
    virtual ~BmnDetectorList() {}


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

    ClassDef(BmnDetectorList, 1);
};

#endif
