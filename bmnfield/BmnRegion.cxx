#include "BmnRegion.h"

// -------------   Default constructor  ----------------------------------
BmnRegion::BmnRegion( Double_t Zmin, Double_t Zmax)
  :TObject()
{
  fZmin=Zmin;
  fZmax=Zmax;
}

// ------------   Destructor   --------------------------------------------
BmnRegion::~BmnRegion() { }

// ------------   Check if inside this region-------------------------------
Bool_t BmnRegion::IsInside(Double_t Z){
  if( Z>=fZmin && Z<=fZmax ) return kTRUE;
  else return kFALSE;
}

ClassImp(BmnRegion)
