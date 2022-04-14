/**
 ** \file BmnSsdHit.cxx
 ** \author Volker Friese <v.friese@gsi.de>
 ** \author D. Baranov
 ** \since 20.12.2018
 **/

#include "BmnSsdHit.h"

#include <iomanip>
#include <sstream>

using namespace std;

// -----   Default constructor
BmnSsdHit::BmnSsdHit()
    : BmnPixelHit()
    , fFrontClusterId(-1)
    , fBackClusterId(-1)
    , fDu(-1.)
    , fDv(-1.)
{
	SetTime(-1.);
	SetTimeError(-1.);
}



// -----   Constructor with parameters
BmnSsdHit::BmnSsdHit(Int_t address, const TVector3& pos, const TVector3& dpos,
		                 Double_t dxy, Int_t frontClusterId, Int_t backClusterId,
		                 Double_t time, Double_t timeError, Double_t du,
		                 Double_t dv)
    : BmnPixelHit(address, pos, dpos, dxy, -1)
    , fFrontClusterId(frontClusterId)
    , fBackClusterId(backClusterId)
    , fDu(du)
    , fDv(dv)
{
	SetTime(time);
	SetTimeError(timeError);
}



// -----   Destructor
BmnSsdHit::~BmnSsdHit()
{
}


// --- String output
string BmnSsdHit::ToString() const {
   stringstream ss;
   ss << "SsdHit: address " << GetAddress() << " | time " << GetTime()
      << " +- " << GetTimeError()
      << " | Position (" << std::setprecision(6) << GetX() << ", " << GetY()
	  << ", " << GetZ() << ") cm | Error (" << GetDx() << ", " << GetDy()
	  << ", " << GetDz() << ") cm";
   return ss.str();
}


ClassImp(BmnSsdHit)
