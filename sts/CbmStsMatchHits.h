//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsMatchHits header file            -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsMatchHits
 *@author Volker Friese <v.friese@gsi.de>
 *@since 27.11.06
 **
 ** CBM task class for matching StsHit with StsPoint.
 ** Task level RECO
 ** Sets the reference index of CbmStsHit.
 **/


#ifndef CBMSTSMATCHHIT_H
#define CBMSTSMATCHHIT_H 1

#include <set>
//#include <map>
#include "TStopwatch.h"
#include "FairTask.h"

using std::set;
using std::map;
using std::pair;


class TClonesArray;
class FairGeoPassivePar;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsDigiMatch;
class CbmStsHit;



class CbmStsMatchHits : public FairTask 
{

 public:

  /** Default constructor **/
  CbmStsMatchHits();


  /** Standard constructor **/
  CbmStsMatchHits(Int_t iVerbose);


  /** Constructor with name **/
  CbmStsMatchHits(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsMatchHits();


  /** Execution **/
  virtual void Exec(Option_t* opt);
  virtual void ExecReal(Option_t* opt);

  virtual void SetRealisticResponse(Bool_t real=kTRUE) {fRealistic = real;}


 private:

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();

  // -----   Private method GetGeometry   ------------------------------------
  InitStatus GetGeometry();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Run summary **/
  virtual void Finish();


  // -----  Data members ----------
  CbmGeoStsPar*     fGeoPar;      /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fPoints;      /** Array of CbmStsPoint **/
  TClonesArray*     fPointsSi;    /** Array of BmnSiliconPoint AZ-280322 **/
  TClonesArray*     fDigis;       /** Array of CbmStsDigi **/
  TClonesArray*     fDigiMatches; /** Array of CbmStsDigiMatch **/
  TClonesArray*     fHits;        /** Array of CbmStsHit **/
  TClonesArray*     fClusters;    /*AZ* Array of CbmStsClusters **/
  TStopwatch        fTimer;       /** Timer **/
  Int_t    fNEvents;        /** Number of events with success **/
  Int_t    fNEventsFailed;  /** Number of events with failure **/
  Double_t fTime;           /** Total real time used for good events **/
  Double_t fNHits;          /** Total number of hits **/
  Double_t fNMatched;       /** Total number of matched hits **/
  Double_t fNDistant;       /** Total number of displaced hits **/
  Double_t fNBackgrd;       /** Total number of background hits **/

  /** Geometry parameters **/
  FairGeoPassivePar* fPassGeo;             // Passive geometry parameters
  TVector3 fTargetPos;                    // Target centre position
  Int_t fNStations;                       // Number of STS stations
  Int_t fStationNrFromMcId[10000];         // station number from mc id

  Bool_t fRealistic;

  /** Map from candidate point to distance to hit **/
  map<Double_t, Int_t> fCandMap;            //!
  map<Double_t, Int_t>::iterator fIter;     //!

  CbmStsMatchHits(const CbmStsMatchHits&);
  CbmStsMatchHits operator=(const CbmStsMatchHits&);

  ClassDef(CbmStsMatchHits,1);

};

#endif

