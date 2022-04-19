// -------------------------------------------------------------------------
// -----                      CbmStsTrack source file                  -----
// -----                  Created 26/01/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsTrack.h"

#include "FairHit.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
CbmStsTrack::CbmStsTrack()
  : TObject(),
    fStsHits(),
    fMvdHits(),
    fPidHypo(0),
    fParamFirst(),
    fParamLast(),
    fFlag(0),
    fChi2(0.),
    fNDF(0),
    fB(0.),
    fStsHitMap(),
    fMvdHitMap(),
    fnEv(),
    fHitsArr("CbmStsHit", 10)
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsTrack::~CbmStsTrack() {
  fStsHitMap.clear();
  fMvdHitMap.clear();
}
// -------------------------------------------------------------------------



// -----   Public method AddStsHit   ---------------------------------------
void CbmStsTrack::AddStsHit(Int_t hitIndex, FairHit* hit) {
  fStsHitMap[hit->GetZ()] = hitIndex;
}
// -------------------------------------------------------------------------

// -----  SM   -------------------------------------------------------------
void CbmStsTrack::AddPixelHit(Int_t hitIndex, CbmBaseHit* hit) {
  fStsHitMap[hit->GetZ()] = hitIndex;
}
// -------------------------------------------------------------------------


// -----   Public method AddMvdHit   ---------------------------------------
void CbmStsTrack::AddMvdHit(Int_t hitIndex, FairHit* hit) {
  fMvdHitMap[hit->GetZ()] = hitIndex;
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsTrack::Print() {
  cout << " Number of attached STS hits : "
       << fStsHits.GetSize()  << endl;
  cout << " Number of attached MVD hits : "
       << fMvdHits.GetSize()  << endl;
  fParamFirst.Print();
  cout << " Chi2: " << fChi2 << ", Quality flag " << fFlag << endl;
}
// -------------------------------------------------------------------------



// -----   Public method SortHits   ----------------------------------------
void CbmStsTrack::SortHits() {

  Int_t index=0;
  map<Double_t, Int_t>::iterator it;

  fStsHits.Reset();
  fStsHits.Set(fStsHitMap.size());
  index = 0;
  for (it = fStsHitMap.begin(); it != fStsHitMap.end(); it++) {
    fStsHits[index] = it->second;
    index++;
  }

  fMvdHits.Reset();
  fMvdHits.Set(fMvdHitMap.size());
  index = 0;
  for (it = fMvdHitMap.begin(); it != fMvdHitMap.end(); it++) {
    fMvdHits[index] = it->second;
    index++;
  }

}
// -------------------------------------------------------------------------

// -----   Public method SortMvdHits   ----------------------------------------
void CbmStsTrack::SortMvdHits() {

  Int_t index=0;
  map<Double_t, Int_t>::iterator it;

  fMvdHits.Reset();
  fMvdHits.Set(fMvdHitMap.size());
  for (it = fMvdHitMap.begin(); it != fMvdHitMap.end(); it++) {
    fMvdHits[index] = it->second;
    index++;
  }

}
// -------------------------------------------------------------------------


ClassImp(CbmStsTrack)
