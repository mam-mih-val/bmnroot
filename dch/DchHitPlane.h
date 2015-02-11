#ifndef DCH_HIT_PLANE_H
#define DCH_HIT_PLANE_H

// *************************************************************************
// Author: Jan Fedorishin, e-mail: fedorisin@jinr.ru
//   
// hit plane creation
//   
//
// Created: 07-11-2014
// Modified: 16-12-2014
//
// *************************************************************************

//#include "FairTask.h"
#include "TObject.h"
#include "ScalarD.h"
#include "ScalarI.h"
#include "ScalarUI.h"
#include "TClonesArray.h"
#include <TMath.h>
#include "DCHgeometryconstants_bmn.h"

//class DchHitPlane : public FairTask {
class DchHitPlane : public TObject {

protected:
  
  //UInt_t ijkl[numChambers][numLayers];  // number of drift chambers, number of planes in dch
  UInt_t ijkl[numLayers];  // numlayers = number of planes in dch

public:    

  TClonesArray  *fDchHitPlane1;      //! pointer to TClonesArray in Dch plane of hits
  TClonesArray  *fDchHitPlane01;      //! pointer to TClonesArray in Dch plane of hits
  TClonesArray  *fDchHitPlane2;      //! pointer to TClonesArray in Dch plane of hits
  TClonesArray  *fDchHitPlane4;      //! pointer to TClonesArray in Dch plane of hits
  TClonesArray  *fDchHitPlane5;      //! pointer to TClonesArray in Dch plane of hits
  TClonesArray  *fDchHitPlane6;      //! pointer to TClonesArray in Dch plane of hits
  TClonesArray  *fDchHitPlane7;      //! pointer to TClonesArray in Dch plane of hits
  DchHitPlane();
  ~DchHitPlane();

  //Bool_t checkxx; 

  void SetDchPlaneHit(Int_t l, Double_t x, Double_t y, Int_t m, Int_t ll, UInt_t kk, Double_t dl, Int_t jj);
  void SetDchPlaneHitsNumber(UInt_t lkji, UShort_t k);
  UInt_t GetDchPlaneHitsNumber(UShort_t k);
  ClassDef(DchHitPlane,1)      // Dch plane of hits

};

#endif
