#include <TClonesArray.h>
#include "BmnTrack.h"

// -----   Default constructor   -------------------------------------------

BmnTrack::BmnTrack()
: TObject(),
fGemHits(),
fSilHits(),
fNhits(0),
fNGemHits(0),
fNSilHits(0),
fParamFirst(),
fParamLast(),
fFlag(0),
fLength(0),
fChi2(0.),
fNDF(0),
fB(0.),
fGemHitMap(),
fSilHitMap() {

}
// -------------------------------------------------------------------------

//// -----   Destructor   ----------------------------------------------------

BmnTrack::~BmnTrack() {
    fGemHitMap.clear();
    fSilHitMap.clear();
}

// -------------------------------------------------------------------------



// -----   Public method AddStsHit   ---------------------------------------

void BmnTrack::AddGemHit(Int_t hitIndex, FairHit* hit) {
    fGemHitMap[hit->GetZ()] = hitIndex;
}

void BmnTrack::AddSilHit(Int_t hitIndex, FairHit* hit) {
    fSilHitMap[hit->GetZ()] = hitIndex;
}

// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------

void BmnTrack::Print() {
    cout << " Number of attached GEM hits : " << fGemHits.GetSize() << endl;
    cout << " Number of attached SILICON hits : " << fSilHits.GetSize() << endl;
    cout << "PARAM FIRST: " << endl;
    fParamFirst.Print();
    cout << "PARAM LAST: " << endl;
    fParamLast.Print();
    cout << " Length: " << fLength << " Chi2: " << fChi2 << ", Quality flag " << fFlag << endl;
}
// -------------------------------------------------------------------------



// -----   Public method SortHits   ----------------------------------------

void BmnTrack::SortHits() {

    Int_t index = 0;
    map<Float_t, Int_t>::iterator it;

    fGemHits.Reset();
    fGemHits.Set(fGemHitMap.size());
    index = 0;
    for (it = fGemHitMap.begin(); it != fGemHitMap.end(); it++) {
        fGemHits[index] = it->second;
        index++;
    }
    fNGemHits = fGemHits.GetSize();
    
    fSilHits.Reset();
    fSilHits.Set(fSilHitMap.size());
    index = 0;
    for (it = fSilHitMap.begin(); it != fSilHitMap.end(); it++) {
        fSilHits[index] = it->second;
        index++;
    }
    fNSilHits = fSilHits.GetSize();
    fNhits = fNGemHits + fNSilHits;
}
// -------------------------------------------------------------------------


ClassImp(BmnTrack)