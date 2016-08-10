
#include <TClonesArray.h>

#include "BmnGemTrack.h"
#include "BmnGemStripDigit.h"

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
fHitMap(),
fStoreHits(kFALSE),
fTrHits(NULL) {

    fTrHits = new TClonesArray("BmnGemStripHit");
}
// -------------------------------------------------------------------------

//// -----   Destructor   ----------------------------------------------------

BmnGemTrack::~BmnGemTrack() {
    fHitMap.clear();
    if (fStoreHits)
        delete fTrHits;
}

// -------------------------------------------------------------------------



// -----   Public method AddStsHit   ---------------------------------------

void BmnGemTrack::AddHit(Int_t hitIndex, FairHit* hit) {
    fHitMap[hit->GetZ()] = hitIndex;
}

void BmnGemTrack::AddHit(BmnGemStripHit* hit) {
    fStoreHits = kTRUE;
    new ((*fTrHits)[fTrHits->GetEntriesFast()]) BmnGemStripHit(*hit);
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
