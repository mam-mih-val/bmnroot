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

#ifndef CbmStsDigiScheme_H
#define CbmStsDigiScheme_H 1

#include <iostream>
#include <map>

#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"

#include "TObjArray.h"
#include "TObject.h"
#include <TGeoCompositeShape.h> //AZ-180322

class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsSector;
class CbmStsSensor;
class CbmStsStation;
class CbmStsStationDigiPar;


class CbmStsDigiScheme : public TObject
{
public:
    /** Destructor **/
    virtual ~CbmStsDigiScheme();


    /** Initialisation **/
    Bool_t Init();
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

    //AZ-290322
    static CbmStsDigiScheme* Instance(int version = 1);

private:
    static CbmStsDigiScheme* fgInstance;    //-> AZ - Static instance of this class
    static int fVersion;                    // AZ-290322 - configuration version (CBM if 0, else BM@N)
    TObjArray* fStations;                   //! Array of CbmStsStation
    Int_t fNSectors;            /** Total number of sectors **/
    Int_t fNSensors;            /** Total number of sensors **/
    Int_t fNChannels;           /** Total number of channels **/
    Bool_t fIsNewGeometry;      /** New geometry ( v12 or later ) **/

    std::map<Int_t,  CbmStsStation*> fStationMap; //! Map from number to station
    std::map<TString, Int_t>          fDetIdByName;
    std::map<TString, CbmStsSensor*> fSensorByName;

    /** Constructor **/
    CbmStsDigiScheme();
    CbmStsDigiScheme(const CbmStsDigiScheme&);
    CbmStsDigiScheme operator=(const CbmStsDigiScheme&);


    /** New init method (from V12a on) **/
    Bool_t InitNew(CbmGeoStsPar* geoPar, CbmStsDigiPar* digiPar); // CBM configuration
    Bool_t InitNewNew(CbmGeoStsPar* geoPar, CbmStsDigiPar* digiPar); //AZ-290322 - BM@N configuration

    CbmStsSector* SetSensor(Int_t moduleNr, Int_t statNr, CbmStsStation* station,
                            CbmStsStationDigiPar* stationPar); //GP
    Int_t SetSensorsCbm(Int_t moduleNr, Int_t statNr, CbmStsStation* station,
                        CbmStsStationDigiPar* stationPar); //AZ
    Int_t AddHotZone(Int_t moduleNr, Int_t statNr, CbmStsStation* station, CbmStsStationDigiPar* stationPar); //AZ-170322
    void AddHotZones(); //AZ-180322

    BmnGemStripStationSet *GemStationSet;       //! Entire GEM detector
    BmnSiliconStationSet *SilStationSet;        //! Entire SILICON detector

  ClassDef(CbmStsDigiScheme,2);
};

#endif
