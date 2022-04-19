//------------------------------------------------------------------------------------------------------------------------

#include <iostream>
using namespace std;

#include "BmnDchHit.h"
//------------------------------------------------------------------------------------------------------------------------

BmnDchHit::BmnDchHit()
: BmnHit(),
fTrackID(0),
fID(0),
fFlag(0),
fNofDim(1),
fPhi(0.),
fUsing(kFALSE) {
    fMeas[1] = fError[1] = 0.;
    fDchLayerType = "";
    fDchIdNumber = -1;
    fWireNumber = -1;
    fDistanceToWire = 0.0;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHit::BmnDchHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t flag, Int_t trackIndex, UChar_t dchlayer)
: BmnHit(detID, pos, dpos, index),
fDetectorID(detID),
fDchLayer(dchlayer),
fTrackID(trackIndex),
fFlag(flag),
fNofDim(1),
fPhi(0.),
fUsing(kFALSE) {
    fMeas[1] = fError[1] = 0.;
    fDchLayerType = "";
    fDchIdNumber = -1;
    fWireNumber = -1;
    fDistanceToWire = 0.0;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHit::BmnDchHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t flag)
: BmnHit(detID, pos, dpos, index),
fFlag(flag),
fNofDim(1),
fPhi(0.),
fUsing(kFALSE) {
    fMeas[1] = fError[1] = 0.;
    fDchLayerType = "";
    fDchIdNumber = -1;
    fWireNumber = -1;
    fDistanceToWire = 0.0;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHit::BmnDchHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
: BmnHit(detID, pos, dpos, index),
fFlag(0),
fNofDim(1),
fPhi(0.),
fUsing(kFALSE) {
    fMeas[1] = fError[1] = 0.;
    fDchLayerType = "";
    fDchIdNumber = -1;
    fWireNumber = -1;
    fDistanceToWire = 0.0;
}
//------------------------------------------------------------------------------------------------------------------------

BmnDchHit::~BmnDchHit() {
}
//------------------------------------------------------------------------------------------------------------------------

void BmnDchHit::Print(const Option_t* opt) const {
    cout << "-I- BmnDchHit" << endl;
    cout << "    DetectorID: " << fDetectorID << endl;
    cout << "    Position: (" << fX << ", " << fY << ", " << fZ << ") cm" << endl;
    cout << "    Position error: (" << fDx << ", " << fDy << ", " << fDz << ") cm" << endl;
    cout << "    Flag: " << fFlag << endl;
}
//------------------------------------------------------------------------------------------------------------------------
/*
void BmnDchHit::SetIndex(Int_t indx) {
    /// Add point index

    Int_t size = fIndex.GetSize();
    fIndex.Set(size + 1);
    fIndex[size] = indx;
}*/
//------------------------------------------------------------------------------------------------------------------------

Int_t BmnDchHit::Compare(const TObject* hit) const {
    /// "Compare" function to sort in ascending order in abs(Z)

    BmnDchHit *kHit = (BmnDchHit*) hit;
    if (GetLayer() < kHit->GetLayer()) return -1;
    else if (GetLayer() > kHit->GetLayer()) return 1;
    else {
        if (TMath::Abs(fZ) < TMath::Abs(kHit->GetZ())) return -1;
        else if (TMath::Abs(fZ) > TMath::Abs(kHit->GetZ())) return 1;
        else return 0;
    }
}
//-------------------------------------------------------------------------------------------------------------------------

ClassImp(BmnDchHit)
