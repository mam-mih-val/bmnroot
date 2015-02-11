#include "DchHitPlane.h"

#include "Riostream.h"
#include <iomanip>

ClassImp(DchHitPlane)
ClassImp(ScalarD)
ClassImp(ScalarI)
ClassImp(ScalarUI)
//ClassImp(ScalarBool)

//_________________________________________________________________
//DchHitPlane::DchHitPlane(UShort_t j, UShort_t k) : TObject() {
DchHitPlane::DchHitPlane() : TObject() {
//DchHitPlane::DchHitPlane():checkxx(false) {

  fDchHitPlane1 = new TClonesArray("ScalarD");
  fDchHitPlane01 = new TClonesArray("ScalarD");
  fDchHitPlane2 = new TClonesArray("ScalarI");
  fDchHitPlane4 = new TClonesArray("ScalarI");
  fDchHitPlane5 = new TClonesArray("ScalarUI");
  fDchHitPlane6 = new TClonesArray("ScalarD");
  fDchHitPlane7 = new TClonesArray("ScalarI");

}

//_________________________________________________________________
DchHitPlane::~DchHitPlane() {


  fDchHitPlane1->Delete();   
  delete fDchHitPlane1;   
  fDchHitPlane1=0;   
  fDchHitPlane01->Delete();   
  delete fDchHitPlane01;   
  fDchHitPlane01=0;   
  fDchHitPlane2->Delete();   
  delete fDchHitPlane2;   
  fDchHitPlane2=0;   
  fDchHitPlane4->Delete();   
  delete fDchHitPlane4;   
  fDchHitPlane4=0;   
  fDchHitPlane5->Delete();   
  delete fDchHitPlane5;   
  fDchHitPlane5=0;   
  fDchHitPlane6->Delete();   
  delete fDchHitPlane6;   
  fDchHitPlane6=0;   
  fDchHitPlane7->Delete();   
  delete fDchHitPlane7;   
  fDchHitPlane7=0;   
  
}

//_________________________________________________________________
void DchHitPlane::SetDchPlaneHit(Int_t l, Double_t x, Double_t y, Int_t m, Int_t ll, UInt_t kk, Double_t dl, Int_t jj) {

ScalarD* wireX = (ScalarD*)fDchHitPlane1->ConstructedAt(l);
wireX->SetSV(x);
ScalarD* Y = (ScalarD*)fDchHitPlane01->ConstructedAt(l);
Y->SetSV(y);
ScalarI* trackId = (ScalarI*)fDchHitPlane2->ConstructedAt(l);
trackId->SetSV(m);
ScalarI* detId = (ScalarI*)fDchHitPlane4->ConstructedAt(l);
detId->SetSV(ll);
ScalarUI* hitwire = (ScalarUI*)fDchHitPlane5->ConstructedAt(l);
hitwire->SetSV(kk);
ScalarD* driftlen = (ScalarD*)fDchHitPlane6->ConstructedAt(l);
driftlen->SetSV(dl);
ScalarI* pointind = (ScalarI*)fDchHitPlane7->ConstructedAt(l);
pointind->SetSV(jj);

}

//________________________________________________________________
void DchHitPlane::SetDchPlaneHitsNumber(UInt_t lkji, UShort_t k){

// setting number of hits in Dch plane

ijkl[k]=lkji;

}

//________________________________________________________________
UInt_t DchHitPlane::GetDchPlaneHitsNumber(UShort_t k){

// returns number of hits in Dch plane

return ijkl[k];

}
//________________________________________________________________
