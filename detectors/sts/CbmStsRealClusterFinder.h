//* $Id: */

// -------------------------------------------------------------------------
// -----                     CbmStsRealClusterFinder header file       -----
// -----                  Created 26/06/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsRealClusterFinder
 *@author Volker Friese <v.friese@gsi.de>
 *@since 11.09.06
 *@version 1.0
 **
 ** CBM task class for finding hits in the STS
 ** Task level RECO
 ** Produces objects of type CbmStsHits out of CbmStsDigi.
 **/


#ifndef CBMSTSREALCLUSTERFINDER_H
#define CBMSTSREALCLUSTERFINDER_H 1


#include <map>
#include <set>
#include "TStopwatch.h"
#include "FairTask.h"

using std::set;
using std::map;
using std::pair;

class TClonesArray;
class CbmGeoStsPar;
class CbmStsDigiPar;
class CbmStsDigiScheme;
class CbmStsSector;
class CbmStsStation;



class CbmStsRealClusterFinder : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsRealClusterFinder();


  /** Standard constructor **/
  CbmStsRealClusterFinder(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsRealClusterFinder(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsRealClusterFinder();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();

 private:

  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fDigis;       /** Input array of CbmStsDigi **/
  TClonesArray*     fClusters;    /** Input array of CbmStsDigi **/
  Int_t fNofDigis;
  map<CbmStsSector*, set<Int_t> > fDigiMapF;  /** sector digis (front) **/
  map<CbmStsSector*, set<Int_t> > fDigiMapB;  /** sector digis (back)  **/
  TStopwatch fTimer;

  Int_t fNofClusters;
  Int_t fNofClustersGood;
  Int_t fNofClustersWP;
  Int_t fNofClustersWM;
  Int_t fLongestCluster;
  Int_t fLongestGoodCluster;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Make sectorwise sets for sigis  **/
  void MakeSets();

  void AnalyzeClusters();
  void AnalyzeCluster(Int_t iclus);

  /** Sort digis sectorwise  **/
  void SortDigis();

  /** Find hits in one sector **/
/*   Int_t FindClusters(CbmStsStation* station, CbmStsSector* sector, */
/* 		     set<Int_t>& fSet, set<Int_t>& bSet); */

//  void FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, set<Int_t>& digiSet);
  Int_t FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, set<Int_t>& digiSet);

  ClassDef(CbmStsRealClusterFinder,1);

};

#endif
