#include <TClonesArray.h>
#include "BmnTrack.h"

// -----   Default constructor   -------------------------------------------

BmnTrack::BmnTrack()
: TObject(),
fUsing(kFALSE),       
fHits(),
fNhits(0),
fParamFirst(),
fParamLast(),
fFlag(-1),
fLength(0),
fChi2(0.),
fNDF(0),
fB(0.),
fHitMap() {

}
// -------------------------------------------------------------------------

//// -----   Destructor   ----------------------------------------------------

BmnTrack::~BmnTrack() {
    fHitMap.clear();
}

// -------------------------------------------------------------------------



// -----   Public method AddStsHit   ---------------------------------------

void BmnTrack::AddHit(Int_t hitIndex, FairHit* hit) {
    fHitMap[hit->GetZ()] = hitIndex;
}

// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------

void BmnTrack::Print() {
    cout << " Number of attached hits : " << fNhits << endl;
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

    fHits.Reset();
    fHits.Set(fHitMap.size());
    index = 0;
    for (it = fHitMap.begin(); it != fHitMap.end(); it++) {
        fHits[index] = it->second;
        index++;
    }
    fNhits = fHits.GetSize();
}
// -------------------------------------------------------------------------


ClassImp(BmnTrack)