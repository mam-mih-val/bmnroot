//* $Id: */

// -------------------------------------------------------------------------
// -----                   CbmStsDigiScheme header file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsDigiScheme
 **@author Volker Friese <v.friese@gsi.de>
 **@since 31.08.06
 **@version 1.0
 **
 ** Auxiliary class for STS digitisation and hit finding.
 ** Controls the digitisation scheme of the stations and sectors.
 **/


#ifndef CBMSTSDIGISCHEME_H
#define CBMSTSDIGISCHEME_H 1

#include "TObjArray.h"
#include "TObject.h"

#include <map>

class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsSector;
class CbmStsSensor;
class CbmStsStation;



class CbmStsDigiScheme : public TObject
{


 public:

  /** Constructor **/
  CbmStsDigiScheme();


  /** Destructor **/
  virtual ~CbmStsDigiScheme();


  /** Initialisation **/
  Bool_t Init(CbmGeoStsPar* geoPar, CbmStsDigiPar* digiPar);


  /** Clear digitisation scheme **/
  void Clear();


  /** Reset eventwise counters **/
  void Reset();


  /** Accessors **/
  Int_t GetNStations() { return fStations->GetEntries(); }
  Int_t GetNSectors()  { return fNSectors; }
  Int_t GetNSensors()  { return fNSensors; }
  Int_t GetNChannels() { return fNChannels; }
  TObjArray*     GetStations() { return fStations; }
  CbmStsStation* GetStation(Int_t iStation);                                       /** Station by index **/
  CbmStsStation* GetStationByNr(Int_t stationNr);                                  /** Station by number **/
  CbmStsSector*  GetSector     (Int_t stationNr, Int_t sectorNr);                  /** Sector  by number **/
  CbmStsSensor*  GetSensor     (Int_t stationNr, Int_t sectorNr, Int_t sensorNr);  /** Sensor  by number **/
  Int_t GetDetectorIdByName(TString sensorName);
  CbmStsSensor* GetSensorByName    (TString sensorName);

  /** Check for new geometry **/
  Bool_t IsNewGeometry() const { return fIsNewGeometry; }

  /** Construct full path to a node **/
  TString GetCurrentPath();

  /** Screen output  **/
  void Print(Bool_t kLong = kFALSE);

 private:

  TObjArray* fStations;       /** Array of CbmStsStation **/
  Int_t fNSectors;            /** Total number of sectors **/
  Int_t fNSensors;            /** Total number of sensors **/
  Int_t fNChannels;           /** Total number of channels **/
  Bool_t fIsNewGeometry;      /** New geometry ( v12 or later ) **/

  std::map<Int_t,  CbmStsStation*> fStationMap; //! Map from number to station
  std::map<TString, Int_t>          fDetIdByName;
  std::map<TString, CbmStsSensor*> fSensorByName;

  CbmStsDigiScheme(const CbmStsDigiScheme&);
  CbmStsDigiScheme operator=(const CbmStsDigiScheme&);


  /** Old init method (up to v11b) **/
  Bool_t InitOld(CbmGeoStsPar* geoPar, CbmStsDigiPar* digiPar);


  /** New init method (from V12a on) **/
  Bool_t InitNew(CbmGeoStsPar* geoPar, CbmStsDigiPar* digiPar);

  ClassDef(CbmStsDigiScheme,1);
};



#endif

