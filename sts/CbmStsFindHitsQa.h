//* $Id: */

// -------------------------------------------------------------------------
// -----                     CbmStsFindHitsQa header file              -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsFindHitsQa
 *@author Volker Friese <v.friese@gsi.de>
 *@since 11.09.06
 *@version 1.0
 **
 ** CBM task class for finding hits in the STS
 ** Task level RECO
 ** Produces objects of type CbmStsHits out of CbmStsDigi.
 **/


#ifndef CBMSTSFINDHITSQA_H
#define CBMSTSFINDHITSQA_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>


class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsSector;
class CbmStsStation;

class TCanvas;
class TPad;
class TH1;
class TH1F;
class TH2F;
class TH3F;
class TList;


class CbmStsFindHitsQa : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsFindHitsQa();


  /** Standard constructor **/
  CbmStsFindHitsQa(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsFindHitsQa(const char* name, Int_t iVerbose);

  
  CbmStsFindHitsQa(Bool_t visualizeBool, Int_t iVerbose = 1);

  /** Destructor **/
  virtual ~CbmStsFindHitsQa();

  /** Create histograms **/
  void CreateHistos();


  /** Reset histograms and counters **/
  void Reset();

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();


 private:

  /** Pointers to data arrays **/
  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/

  TClonesArray* fMCTracks;           // MCtrack
  TClonesArray* fStsPoints;          // StsPoints
  TClonesArray* fStsHits;            // StsHits
  TClonesArray* fMatches;            // StsTrackMatch
  TClonesArray* fStsDigis;           // StsDigi
  TClonesArray* fStsClusters;        // StsCluster

  TStopwatch fTimer;


  TH2F* fhHitPointCorrelation [100];
  TH2F* fhHitPointCorrelationU[100];
  TH2F* fhHitPointCorrelationB[100];
//   TH2F* fhHitPointPuls [100];

  TH3F* fhEnergyLoss[20];
  TH3F* fhIncAngle[20];

  TH2F* fhPoints    [20];
  TH2F* fhRecoPoints[20];

  TH1F* fhHitFindingEfficiency;
  TH2F* fhEffIncAng;
  TH2F* fhEffMom;
  TH2F* fhEffPdgSec;
  TH2F* fhEffPdgPrim;
  TH2F* fhHitPointPull;
  TH2F* fhHitPointCorr;
  
  TH1F* fhNofHits      [20][300][3];     // per station, sector, sensor

  /** List of histograms **/
  TList* fHistoList;
  TList* fHistoListPS[20];

  Int_t    fNStations;
  Int_t    fNEvents;        /** Number of events with success **/
   /** Total real time used for good events **/
   Double_t  fTime1;     
 
  Int_t fNofHits      [20][300][3];     // per station, sector, sensor
   Int_t fNofDigisPChip[20][300][2][20];
  Int_t fNofPoints    [20][300][3];
  Int_t fNofRecoPoints[20][300][3];
  Int_t fNofPointsIncAng     [1000];
  Int_t fNofRecoPointsIncAng [1000];
  Int_t fNofPointsMom        [1000];
  Int_t fNofRecoPointsMom    [1000];
  Int_t fNofRecoPdgSec       [20000];
  Int_t fNofPointsPdgSec     [20000];
  Int_t fNofRecoPdgPrim      [20000];
  Int_t fNofPointsPdgPrim    [20000];
 
  Int_t fNofPointsPrim;
  Int_t fNofPointsSec;
  Int_t fNofRecoPrim;
  Int_t fNofRecoSec;
  Int_t fNofPointsMomSum;
  Int_t fNofRecoPointsMomSum;
  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();
  
  Bool_t fOnlineAnalysis;
  TCanvas* recoCanvas;
  TPad*    recoPad[20];

  CbmStsFindHitsQa(const CbmStsFindHitsQa&);
  CbmStsFindHitsQa operator=(const CbmStsFindHitsQa&);

  ClassDef(CbmStsFindHitsQa,1);

};

#endif
