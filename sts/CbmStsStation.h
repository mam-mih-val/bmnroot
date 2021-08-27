// -------------------------------------------------------------------------
// -----                      CbmStsStation header file                -----
// -----                  Created 31/03/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsStation.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** This class holds the transport geometry parameters 
 ** of one STS tracking station necessary for digitisation.
 **/


#ifndef CBMSTSSTATION_H
#define CBMSTSSTATION_H 1


#include "TObjArray.h"
#include "TNamed.h"
#include "TString.h"

#include <map>

class CbmStsSector;
class CbmStsSectorDigiPar;


class CbmStsStation : public TNamed
{

 public:

  /** Default constructor **/
  CbmStsStation();


  /** Standard constructor 
  *@param name      Volume name of station
  *@param detId     Detector ID 
  *@param z         z position of station centre [cm]
  *@param d         thickness of station [cm]
  *@param rl        radiation length of material [cm]
  *@param rmin      Inner radius [cm]
  *@param rmax      Outer radius [cm]
  *@param rotation  Rotation angle in global c.s. [rad]
  **/
  CbmStsStation(const char* name, Int_t detId, Double_t z, Double_t d,
		Double_t rl, Double_t rmin, Double_t rmax, 
		Double_t rotation);


  /** Destructor **/
  virtual ~CbmStsStation();


  /** Accessors **/
  Int_t    GetDetectorId() const { return fDetectorId; }
  Int_t    GetSystemId()   const { 
    return ( ( fDetectorId & (15<<24) ) >> 24); }
  Int_t    GetStationNr()  const { 
    return ( ( fDetectorId & (255<<16) ) >> 16 ); }
  Double_t GetZ(Int_t it=0);
  Double_t GetD()          const { return fD; }
  Double_t GetRadLength()  const { return fRadLength; }
  Double_t GetRmin()       const { return fRmin; }
  Double_t GetRmax()       const { return fRmax; }
  Int_t    GetNSectors()   const { return fSectors->GetEntriesFast(); }
  Int_t    GetNChannels();
  TObjArray* GetSectors() { return fSectors; }
  CbmStsSector* GetSector(Int_t iSector) { 
    return (CbmStsSector*) fSectors->At(iSector); }
  CbmStsSector* GetSectorByNr(Int_t sectorNr);

  /** Add one sector to the array **/
  void AddSector(CbmStsSectorDigiPar* sectorPar);
  void AddSector(CbmStsSector* sector);

  Int_t GetNofZ();

  /** Reset all eventwise counters **/
  void Reset();

  /** Output to screen **/
  virtual void Print(Bool_t kLong);


 protected:

  TString    fName;             // Station name
  Int_t      fDetectorId;       // Unique detector ID
  Double32_t fZ;                // z position of station centre [cm]
  Double32_t fD;                // Thickness of station
  Double32_t fRadLength;        // Radiation length of material [cm]
  Double32_t fRmin;             // Inner radius [cm]
  Double32_t fRmax;             // Outer radius [cm]
  Double32_t fRotation;         // Rotation angle in global c.s [rad]
  TObjArray* fSectors;          // Array of CbmStsSectors
  std::map<Int_t, Int_t> fSectorMap; //! Map from sector number to index
  std::map<Int_t, Int_t> fMcIdMap;   //! Map from McId to index
  
  Double_t fSensorZ[30];
 
 private:

  CbmStsStation(const CbmStsStation&);
  CbmStsStation operator=(const CbmStsStation&);

  ClassDef(CbmStsStation,1);

};



#endif
