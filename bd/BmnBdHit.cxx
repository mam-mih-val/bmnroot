// -------------------------------------------------------------------------
// -----                        BmnBdHit source file                   -----
// -----                  Created 12/07/18  by A. Zinchenko            -----
// -------------------------------------------------------------------------

#include "BmnBdHit.h"

using std::cout;
using namespace TMath;

// -------------------------------------------------------------------------

BmnBdHit::BmnBdHit() : FairHit() {
}

// -------------------------------------------------------------------------

BmnBdHit::BmnBdHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index)
  : FairHit(detID, pos, dpos, index) 
{
  fStrip = detID;
}

// -------------------------------------------------------------------------

BmnBdHit::~BmnBdHit() {
}

ClassImp(BmnBdHit)
