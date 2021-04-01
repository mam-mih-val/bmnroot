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
  CbmStsFindHits(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsFindHits();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();

 
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

  CbmStsFindHits(const CbmStsFindHits&) = delete;
  CbmStsFindHits operator=(const CbmStsFindHits&) = delete;


  ClassDef(CbmStsFindHits,1);
};

#endif
