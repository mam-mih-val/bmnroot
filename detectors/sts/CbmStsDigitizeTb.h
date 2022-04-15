/** @file CbmStsDigitizeTb.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 10 December 2012
 */

#ifndef CBMSTSDIGITIZETB_H
#define CBMSTSDIGITIZETB_H 1




#include <set>
#include "TStopwatch.h"
#include "TClonesArray.h"
#include "FairTask.h"

using std::set;
using std::map;
using std::pair;

class CbmGeostsPar;
class CbmStsDigiPar;
class CbmStsStation;

class CbmStsPoint;


/** CbmStsDigitiseTb
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 10 December 2012
 ** @brief CBM task for STS detector response simulation
 **
 ** Version for time-based simulation
 ** Based on earlier developments by R. Karabowicz and A. Kotynia.
 **/
class CbmStsDigitizeTb : public FairTask
{

 public:

  /** Default constructor **/
  CbmStsDigitizeTb();

  /** Destructor **/
  virtual ~CbmStsDigitizeTb();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  virtual void SetRealisticResponse(Bool_t real=kTRUE) {fRealistic = real;}

  void SetThreshold (Double_t threshold =0.) { fThreshold = threshold; }
  
  void SetNAdcBits (Int_t nBits) { fNAdcBits = nBits; }

  void SetStripDeadTime  (Double_t  StripDeadTime=0.) {fStripDeadTime =  StripDeadTime;}


 private:

  CbmGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  TClonesArray*     fPoints;       /** Input array of CbmStsPoint **/
  TClonesArray*     fDigis;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of CbmStsDigiMatches**/

  // statistics
  Int_t             fNDigis;
  Int_t             fNMulti;

  Double_t          fNEvents;
  Double_t          fNPoints;     
  Double_t          fNOutside;
  Double_t          fNDigisFront;
  Double_t          fNDigisBack;
  Double_t          fTime;

  // settings
  Double_t          fStepSize;    /** Step size for ionisation points **/

  TStopwatch        fTimer;

  Bool_t    fRealistic;
  Double_t  fPairCreationEnergy;

  Double_t  fFNoiseWidth;
  Double_t  fBNoiseWidth;

  Double_t  fStripDeadTime;
  
  // Digitisation properties
  Double_t fQMax;          /** Dynamic range in e **/
  Double_t fThreshold;     /** Threshold charge in e **/
  Int_t fNAdcBits;         /** Number of bits for ADC **/
  Double_t fNAdcChannels;  /** Number of ADC channels **/

//  Int_t     fFNofBits;
//  Int_t     fBNofBits;
//  Double_t  fFMinStep;
//  Double_t  fBMinStep;
//  Int_t     fFNofSteps;
//  Int_t     fBNofSteps;


  // map of the point indices contributing to a fired strip
  map<Int_t, set<Int_t> > fFChannelPointsMap;
  map<Int_t, set<Int_t> > fBChannelPointsMap;
 


  /** Perform digitisation of one MCPoint
   ** @param point   Pointer to CbmStsPoint
   ** @param nFront  Number of digis on front side (return)
   ** @param nBack   Number of digis on back side {return)
   **/
  void DigitizePoint(const CbmStsPoint* point, Int_t& nFront, Int_t& nBack);


  /** Get parameter containers **/
  virtual void SetParContainers();




  /** Intialisation
   ** @return kSUCCESS if successful
   **/
  virtual InitStatus Init();



  /** Re-initialisation **/
  virtual InitStatus ReInit();

  /** Virtual method Finish **/
  virtual void Finish();


  /** Reset eventwise counters **/
  void Reset();

  CbmStsDigitizeTb(const CbmStsDigitizeTb&);
  CbmStsDigitizeTb operator=(const CbmStsDigitizeTb&);

  ClassDef(CbmStsDigitizeTb,1);

};

#endif


