//------------------------------------------------------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "BmnDchHitOriginal.h"
//------------------------------------------------------------------------------------------------------------------------

BmnDchHitOriginal::BmnDchHitOriginal()
: FairHit(),
fTrackID(0),
fFlag(0),
fNofDim(1),
fPhi(0.) {
    fMeas[1] = fError[1] = 0.;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHitOriginal::BmnDchHitOriginal(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t flag, Int_t trackIndex, UInt_t dchlayer)
: FairHit(detID, pos, dpos, index),
fDetectorID(detID),
fDchLayer(dchlayer),
fTrackID(trackIndex),
fFlag(flag),
fNofDim(1),
fPhi(0.) {
    fMeas[1] = fError[1] = 0.;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHitOriginal::BmnDchHitOriginal(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t flag)
: FairHit(detID, pos, dpos, index),
fFlag(flag),
fNofDim(1),
fPhi(0.) {
    fMeas[1] = fError[1] = 0.;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHitOriginal::BmnDchHitOriginal(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: FairHit(detID, pos, dpos, index),
fFlag(0),
fNofDim(1),
fPhi(0.) {
    fMeas[1] = fError[1] = 0.;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHitOriginal::~BmnDchHitOriginal() {
}
//------------------------------------------------------------------------------------------------------------------------

void BmnDchHitOriginal::Print(const Option_t* opt) const {
    cout << "-I- BmnDchHitOriginal" << endl;
    cout << "    DetectorID: " << fDetectorID << endl;
    cout << "    Position: (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Position error: (" << fDx << ", " << fDy << ", " << fDz << ") cm" << endl;
    cout << "    Flag: " << fFlag << endl;
}
//------------------------------------------------------------------------------------------------------------------------

void BmnDchHitOriginal::SetIndex(Int_t indx) {
    /// Add point index

    Int_t size = fIndex.GetSize();
    fIndex.Set(size + 1);
    fIndex[size] = indx;
}
//------------------------------------------------------------------------------------------------------------------------

Int_t BmnDchHitOriginal::Compare(const TObject* hit) const {
    /// "Compare" function to sort in ascending order in abs(Z)

    BmnDchHitOriginal *kHit = (BmnDchHitOriginal*) hit;
    if (GetLayer() < kHit->GetLayer()) return -1;
    else if (GetLayer() > kHit->GetLayer()) return 1;
    else {
        if (TMath::Abs(fZ) < TMath::Abs(kHit->GetZ())) return -1;
        else if (TMath::Abs(fZ) > TMath::Abs(kHit->GetZ())) return 1;
        else return 0;
    }
}
//-------------------------------------------------------------------------------------------------------------------------

ClassImp(BmnDchHitOriginal)
