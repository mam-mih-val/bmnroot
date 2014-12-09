//--------------------------------------------------------------------------
//-----                    BmnSttHit                           -----
//--------------------------------------------------------------------------

/**  BmnSttHit.cxx
 **
 **/

#include <iostream>
using namespace std;

#include "BmnSttHit.h"


// -----   Default constructor   -------------------------------------------

BmnSttHit::BmnSttHit()
  : FairHit(),
    fFlag(0),
    fNofDim(1),
    fPhi(0.)
{
  fMeas[1] = fError[1] = 0.;
}


// -----   Standard constructor   ------------------------------------------

BmnSttHit::BmnSttHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index, Int_t flag)
  : FairHit(detID, pos, dpos, index),
    fFlag(flag),
    fNofDim(1),
    fPhi(0.)
{
  fMeas[1] = fError[1] = 0.;
}

// -----   Constructor without flag  ------------------------------------------

BmnSttHit::BmnSttHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
  : FairHit(detID, pos, dpos, index),
    fFlag(0),
    fNofDim(1),
    fPhi(0.)
{
  fMeas[1] = fError[1] = 0.;
}

// -----   Destructor   ----------------------------------------------------
BmnSttHit::~BmnSttHit() { }


// -----  Print  -----------------------------------------------------------
void BmnSttHit::Print(const Option_t* opt) const
{
  cout << "-I- BmnSttHit" << endl;
  cout << "    DetectorID: " << fDetectorID << endl;
  cout << "    Position: (" << fX
       << ", " << fY
       << ", " << fZ << ") cm"
       << endl;
  cout << "    Position error: (" << fDx
       << ", " << fDy
       << ", " << fDz << ") cm"
       << endl;
  cout << "    Flag: " << fFlag
       << endl;
}
// -------------------------------------------------------------------------

//__________________________________________________________________________
void BmnSttHit::SetIndex(Int_t indx)
{
  /// Add point index

  Int_t size = fIndex.GetSize();
  fIndex.Set (size + 1);
  fIndex[size] = indx;
}
// -------------------------------------------------------------------------

//________________________________________________________________
Int_t BmnSttHit::Compare(const TObject* hit) const
{
  /// "Compare" function to sort in ascending order in abs(Z)

  BmnSttHit *kHit = (BmnSttHit*) hit;
  if (GetLayer() < kHit->GetLayer()) return -1;
  else if (GetLayer() > kHit->GetLayer()) return 1;
  else {
    if (TMath::Abs(fZ) < TMath::Abs(kHit->GetZ())) return -1;
    else if (TMath::Abs(fZ) > TMath::Abs(kHit->GetZ())) return 1;
    else return 0;
  }
}
// -------------------------------------------------------------------------

ClassImp(BmnSttHit)
