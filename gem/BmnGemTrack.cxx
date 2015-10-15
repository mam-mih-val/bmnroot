
#include "BmnGemTrack.h"

using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------

BmnGemTrack::BmnGemTrack()
: TObject(),
fHits(),
fParamFirst(),
fParamLast(),
fFlag(0),
fChi2(0.),
fNDF(0),
fB(0.),
fRef(-1),
fUsed(kFALSE),
fHitMap() {
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------

BmnGemTrack::~BmnGemTrack() {
    fHitMap.clear();
}
// -------------------------------------------------------------------------



// -----   Public method AddStsHit   ---------------------------------------

void BmnGemTrack::AddHit(Int_t hitIndex, FairHit* hit) {
    fHitMap[hit->GetZ()] = hitIndex;
}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------

void BmnGemTrack::Print() {
    cout << " Number of attached GEM hits : " << fHits.GetSize() << endl;
    cout << "PARAM FIRST: " << endl;
    fParamFirst.Print();
    cout << "PARAM LAST: " << endl;
    fParamLast.Print();
    cout << " Chi2: " << fChi2 << ", Quality flag " << fFlag << endl;
}
// -------------------------------------------------------------------------



// -----   Public method SortHits   ----------------------------------------

void BmnGemTrack::SortHits() {

    Int_t index = 0;
    map<Float_t, Int_t>::iterator it;

    fHits.Reset();
    fHits.Set(fHitMap.size());
    index = 0;
    for (it = fHitMap.begin(); it != fHitMap.end(); it++) {
        fHits[index] = it->second;
        index++;
    }
}
// -------------------------------------------------------------------------


ClassImp(BmnGemTrack)
