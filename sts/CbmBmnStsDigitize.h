//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmBmnStsDigitize header file             -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsDigitise
 *@author Volker Friese <v.friese@gsi.de>
 *@since 30.08.06
 *@version 1.0
 **
 ** CBM task class for digitising STS
 ** Task level SIM
 ** Produces objects of type CbmStsDigi out of CbmStsPoint.
 **/


#ifndef CBMBMNSTSDIGITIZE_H
#define CBMBMNSTSDIGITIZE_H 1

#include <set>
#include "TStopwatch.h"
#include "FairTask.h"
#include "CbmStsDigiScheme.h"
#include "BmnEventHeader.h"
#include <sstream>
//#include "CbmBmnStsDigiScheme.h"

using std::set;
using std::map;
using std::pair;

class TClonesArray;
class TGeoNode;
class CbmGeostsPar;
class CbmBmnStsDigiPar;
class CbmStsStation;
class BmnEventHeader;

class CbmStsPoint;


class CbmBmnStsDigitize : public FairTask
{

 public:

  /** Default constructor **/
  CbmBmnStsDigitize();


  /** Standard constructor **/
  CbmBmnStsDigitize(Int_t iVerbose);


  /** Constructor with name **/
  CbmBmnStsDigitize(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmBmnStsDigitize();

  void SetStep(Double_t tStep) {fStep = tStep;}

  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();

  virtual void SetRealisticResponse(Bool_t real=kTRUE) {fRealistic = real;}

  void FindFiredStrips(CbmStsPoint* pnt,Int_t& nofStr,Int_t*& strips,Double_t*& signals,Int_t side);
  void ProduceHitResponse(CbmStsSensor* sensor);

  void SetFrontThreshold (Double_t  frontThr=0.)      {fFThreshold    =  frontThr;}
  void SetBackThreshold  (Double_t  backThr=0.)       {fBThreshold    =  backThr;}
  void SetFrontNoiseWidth(Double_t  frontNoW=0.)      {fFNoiseWidth   =  frontNoW;}
  void SetBackNoiseWidth (Double_t  backNoW=0.)       {fBNoiseWidth   =  backNoW;}

  void SetFrontNofBits   (Int_t     frontNB=0 ) {fFNofBits    =  frontNB;}
  void SetBackNofBits    (Int_t      backNB=0 ) {fBNofBits    =   backNB;}
  void SetFrontNofElPerAdc   (Double_t  frontMS=0.) {fFNofElPerAdc    =  frontMS;}
  void SetBackNofElPerAdc    (Double_t   backMS=0.) {fBNofElPerAdc    =   backMS;}

  void SetStripDeadTime  (Double_t  StripDeadTime=0.) {fStripDeadTime =  StripDeadTime;}
  void SetGeoFile  (TString  geoFile) {fGeoFile =  geoFile;} //GP
  void ApplyAlignment(); //AZ
  Bool_t MC=false;

    static const Int_t nStaGEMs = 6;
  map<Int_t,Double_t> digiMap[nStaGEMs][2];
  map<Int_t,Double_t>::iterator  mit;


 private:

  CbmGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  CbmStsDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  CbmStsDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  TClonesArray*     fPoints;       /** Input array of CbmStsDigi from GEM **/
  TClonesArray*     fPointsSI;      /** Input array of CbmStsDigi from Silicon **/
  TClonesArray*     fPointsT0;      /** Input array of CbmStsDigi from T0 **/
  //TClonesArray*     fPointsBC2;      /** Input array of CbmStsDigi from BC2 **/
  //TClonesArray*     fPointsVeto;      /** Input array of CbmStsDigi from Veto **/
  //TClonesArray*     fPointsBD;      /** Input array of CbmStsDigi from Veto **/
   //TClonesArray*     fPointsSi;      /** Input array of CbmStsDigi from Veto **/
  TClonesArray*     fDigis;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigisCSC;        /** Output array of CbmStsDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of CbmStsDigiMatches**/
  TClonesArray*     fEventHeader;  /** Input array of raw EventHeader**/
  TClonesArray*     fBMNEventHeader;  /** Output array of raw bmn EventHeader**/

   TClonesArray* fPointsBC1;
  TClonesArray* fPointsBC2;
  TClonesArray* fPointsVeto;
  TClonesArray* fPointsBD;
  TClonesArray*  fPointsSi;
  

  // statistics
  Int_t             fNDigis;
  Int_t             fNMulti;
  Int_t             fch=0;
    Int_t             fchtrig=0;

  Double_t          fNEvents;
  Double_t          fNPoints;
  Double_t          fNDigisFront;
  Double_t          fNDigisBack;
  Double_t          fTime;

  // settings
  Double_t          fStep;

  TStopwatch        fTimer;

  Bool_t    fRealistic;
  Double_t  fEnergyLossToSignal;

  Double_t  fFThreshold;
  Double_t  fBThreshold;
  Double_t  fFNoiseWidth;
  Double_t  fBNoiseWidth;

  Double_t  fStripDeadTime;
  TString  fGeoFile;

  Int_t     fFNofBits;
  Int_t     fBNofBits;
  Double_t  fFNofElPerAdc;
  Double_t  fBNofElPerAdc;
  Int_t     fFNofSteps;
  Int_t     fBNofSteps;

  Double_t* fStripSignalF;
  Double_t* fStripSignalB;

  // map of the point indices contributing to a fired strip
  map<Int_t, set<Int_t> > fFChannelPointsMap;
  map<Int_t, set<Int_t> > fBChannelPointsMap;



  map<CbmStsSensor*, set<Int_t> > fPointMap;  /** sensor points **/
  Float_t occupancy [20][1000][20] ;

  /** Make sensorwise set for points **/
  void MakeSets();
  void MakeSets1();


  Bool_t GetBmnGeom(const char* fileName);

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();

  /** AZ Check if particle goes thru the spacer (dead space) **/
  Bool_t CrossSpacer(const TGeoNode *node, const CbmStsPoint *point);

  CbmBmnStsDigitize(const CbmBmnStsDigitize&);
  CbmBmnStsDigitize operator=(const CbmBmnStsDigitize&);

  ClassDef(CbmBmnStsDigitize,2);

};

#endif