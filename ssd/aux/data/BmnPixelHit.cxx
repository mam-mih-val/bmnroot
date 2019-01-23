/**
 * \file BmnPixelHit.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \author D. Baranov
 * \date 2018
 **/
#include "BmnPixelHit.h"

#include <sstream>
using std::stringstream;
using std::endl;

BmnPixelHit::BmnPixelHit()
  : BmnPixelHit(-1, 0., 0., 0.,
                0., 0., 0., 0., -1)
{
}

BmnPixelHit::BmnPixelHit(
		Int_t address,
		const TVector3& pos,
		const TVector3& err,
		Double_t dxy,
		Int_t refId,
                Double_t time,
                Double_t timeError)
  : BmnPixelHit(address, pos.X(), pos.Y(), pos.Z(),
		err.X(), err.Y(), err.Z(), dxy, refId,
                time, timeError)
{
}

BmnPixelHit::BmnPixelHit(
		Int_t address,
		Double_t x,
		Double_t y,
		Double_t z,
		Double_t dx,
		Double_t dy,
		Double_t dz,
		Double_t dxy,
		Int_t refId,
                Double_t time,
                Double_t timeError)
  : BmnBaseHit(kPIXELHIT, z, dz, refId, address, time, timeError),
    fX(x),
    fY(y),
    fDx(dx),
    fDy(dy),
    fDxy(dxy)
{
}

BmnPixelHit::~BmnPixelHit()
{
}

std::string BmnPixelHit::ToString() const
{
   stringstream ss;
   ss << "BmnPixelHit: address=" << GetAddress()
       << " pos=(" << GetX() << "," << GetY() << "," << GetZ()
       << ") err=(" << GetDx() << "," << GetDy() << "," << GetDz()
       << ") dxy=" << GetDxy() << " refId=" << GetRefId() << endl;
   return ss.str();
}

void BmnPixelHit::Position(TVector3& pos) const
{
   pos.SetXYZ(GetX(), GetY(), GetZ());
}

void BmnPixelHit::PositionError(TVector3& dpos) const
{
   dpos.SetXYZ(GetDx(), GetDy(), GetDz());
}

void BmnPixelHit::SetPosition(const TVector3& pos)
{
	SetX(pos.X());
	SetY(pos.Y());
	SetZ(pos.Z());
}

void BmnPixelHit::SetPositionError(const TVector3& dpos)
{
	SetDx(dpos.X());
	SetDy(dpos.Y());
	SetDz(dpos.Z());
}

ClassImp(BmnPixelHit);
