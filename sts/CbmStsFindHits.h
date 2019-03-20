//* $Id: */

// -------------------------------------------------------------------------
// -----                     CbmStsFindHits header file            -----
// -----                  Created 26/06/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsFindHits
 *@author Volker Friese <v.friese@gsi.de>
 *@since 11.09.06
 *@version 1.0
 **
 ** CBM task class for finding hits in the STS
 ** Task level RECO
 ** Produces objects of type CbmStsHits out of CbmStsDigi.
 **/


#ifndef CBMSTSFINDHITS_H
#define CBMSTSFINDHITS_H 1

#include <TH1D.h>
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
class CbmStsCluster; //AZ
class CbmStsDigiScheme; //GP
class BmnNewFieldMap; //GP


class CbmStsFindHits : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsFindHits();


  /** Standard constructor **/
  CbmStsFindHits(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsFindHits(const char* name, Int_t iVerbose,Bool_t CheckTrigSi=false);


  /** Destructor **/
  virtual ~CbmStsFindHits();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();

  Double_t fLor;
  Bool_t MC=false;
  /*
  TH1D *h1p = new TH1D("h1p","", 62,0.14,0.17);
  TH1D *h2p = new TH1D("h2p","", 60, 0.13,0.17);

  TH1D *h3p = new TH1D("h3p","", 64, 0.14,0.18);

  TH1D *h4p = new TH1D("h4p","", 50, 0.10,0.15);
  TH1D *h5p = new TH1D("h5p","", 62, 0.13,0.18);

  TH1D *h6p = new TH1D("h6p","", 52, 0.11,0.15);
  TH1D *h7p = new TH1D("h7p","", 54, 0.12,0.16); */
 private:

  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/ // FIX GP
  TClonesArray*     fClusters;    /** Input array of CbmStsCluster **/
  TClonesArray*     fDigis;    /**AZ - Input array of CbmStsDigis **/
  TClonesArray*     fHits;        /** Output array of CbmStsHit **/
  std::map<CbmStsSector*, std::set<Int_t> > fClusterMapF;  /** sector clusters (front) **/
  std::map<CbmStsSector*, std::set<Int_t> > fClusterMapB;  /** sector clusters (back)  **/
  TStopwatch fTimer;
  BmnNewFieldMap* fMap;

  Bool_t fCheckTrigSi;

  Int_t fNHits;
 

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Make sectorwise sets for sigis  **/
  void MakeSets();


  /** Sort digis sectorwise  **/
  void SortClusters();


  /** Find hits in one sector **/
  Int_t FindHits(CbmStsStation* station, CbmStsSector* sector,
		 std::set<Int_t>& fSet, std::set<Int_t>& bSet);


  /** AZ-Apply charge correlation cut **/
  //Bool_t ChargeCorrelation(CbmStsCluster *clusterF, CbmStsCluster *clusterB);
  Double_t ChargeCorrelation(CbmStsCluster *clusterF, CbmStsCluster *clusterB);

  CbmStsFindHits(const CbmStsFindHits&);
  CbmStsFindHits operator=(const CbmStsFindHits&);


  ClassDef(CbmStsFindHits,1);

};

#endif
