//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsIdealMatchHits header file                -----
// -----                  Created 27/11/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsIdealMatchHits
 *@author Volker Friese <v.friese@gsi.de>
 *@since 27.11.06
 **
 ** CBM task class for matching StsHit with StsPoint.
 ** Task level RECO
 ** Sets the reference index of CbmStsHit.
 **/


#ifndef CBMSTSIDEALMATCHHIT_H
#define CBMSTSIDEALMATCHHIT_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <map>

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsDigiMatch;
class CbmStsHit;



class CbmStsIdealMatchHits : public FairTask 
{

 public:

  /** Default constructor **/
  CbmStsIdealMatchHits();


  /** Standard constructor **/
  CbmStsIdealMatchHits(Int_t iVerbose);


  /** Constructor with name **/
  CbmStsIdealMatchHits(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsIdealMatchHits();


  /** Execution **/
  virtual void Exec(Option_t* opt);



 private:

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Run summary **/
  virtual void Finish();


  // -----  Data members ----------
  CbmGeoStsPar*     fGeoPar;      /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fPoints;      /** Array of CbmStsPoint **/
  TClonesArray*     fDigis;       /** Array of CbmStsDigi **/
  TClonesArray*     fDigiMatches; /** Array of CbmStsDigiMatch **/
  TClonesArray*     fHits;        /** Array of CbmStsHit **/
  TStopwatch        fTimer;       /** Timer **/
  Int_t    fNEvents;        /** Number of events with success **/
  Int_t    fNEventsFailed;  /** Number of events with failure **/
  Double_t fTime;           /** Total real time used for good events **/
  Double_t fNHits;          /** Total number of hits **/
  Double_t fNMatched;       /** Total number of matched hits **/
  Double_t fNDistant;       /** Total number of displaced hits **/
  Double_t fNBackgrd;       /** Total number of background hits **/

  /** Map from candidate point to distance to hit **/
  std::map<Double_t, Int_t> fCandMap;            //!
  std::map<Double_t, Int_t>::iterator fIter;     //!

  CbmStsIdealMatchHits(const CbmStsIdealMatchHits&);
  CbmStsIdealMatchHits operator=(const CbmStsIdealMatchHits&);

  ClassDef(CbmStsIdealMatchHits,1);

};

#endif

