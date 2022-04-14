//* $Id: */

// -------------------------------------------------------------------------
// -----                     CbmStsRealFindHits header file            -----
// -----                  Created 26/06/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsRealFindHits
 *@author Volker Friese <v.friese@gsi.de>
 *@since 11.09.06
 *@version 1.0
 **
 ** CBM task class for finding hits in the STS
 ** Task level RECO
 ** Produces objects of type CbmStsHits out of CbmStsDigi.
 **/


#ifndef CBMSTSREALFINDHITS_H
#define CBMSTSREALFINDHITS_H 1


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



class CbmStsRealFindHits : public FairTask
{


 public :

  /** Default constructor **/
  CbmStsRealFindHits();


  /** Standard constructor **/
  CbmStsRealFindHits(Int_t iVerbose);


  /** Constructor with task name **/
  CbmStsRealFindHits(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~CbmStsRealFindHits();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Finish **/
  virtual void Finish();


 private:

  CbmGeoStsPar*     fGeoPar;      /** Geometry parameters **/
  CbmStsDigiPar*    fDigiPar;     /** Digitisation parameters **/
  CbmStsDigiScheme* fDigiScheme;  /** Digitisation scheme **/
  TClonesArray*     fClusters;    /** Input array of CbmStsCluster **/
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


  ClassDef(CbmStsRealFindHits,1);

};

#endif
