#include "BmnLANDDigit.h"
#include <iostream>
#include "BmnTacquilaDigit.h"

BmnLANDDigit::BmnLANDDigit():
  fPlane(-1),
  fBar(-1),
  fGlobBar(-1),
  fTdc0(-1),
  fTdc1(-1),
  fQdc0(-1),
  fQdc1(-1),
  fTDiff0(-1),
  fTDiff1(-1),
  fTime0(-1),
  fTime1(-1),
  fTime(-1),
  fEnergy0(-1),
  fEnergy1(-1),
  fEnergy(-1),
  fPosition(-1),
  fX(0),
  fY(0)
{
}

BmnLANDDigit::BmnLANDDigit(UChar_t plane, UChar_t bar, BmnTacquilaDigit const
    &a_tacq0, BmnTacquilaDigit const &a_tacq1, Float_t a_time0, Float_t
    a_time1, Float_t a_energy0, Float_t a_energy1, Float_t a_position):
  fPlane(plane),
  fBar(bar),
  fGlobBar(plane * 20 + bar),
  fTdc0(),
  fTdc1(),
  fQdc0(),
  fQdc1(),
  fTDiff0(),
  fTDiff1(),
  fTime0(a_time0),
  fTime1(a_time1),
  fTime(0.5 * (a_time0 + a_time1)),
  fEnergy0(a_energy0),
  fEnergy1(a_energy1),
  fEnergy(sqrt(a_energy0 * a_energy1)),
  fPosition(a_position),
  fX(),
  fY()
{
#define COPY(name, i) f##name##i = a_tacq##i.Get##name()
#define COPY_BOTH(name) COPY(name, 0); COPY(name, 1)
  COPY_BOTH(Tdc);
  COPY_BOTH(Qdc);
  COPY_BOTH(TDiff);
  fX = IsVertical() ? GetBarPosition() : fPosition;
  fY = IsVertical() ? fPosition : GetBarPosition();
}

BmnLANDDigit::~BmnLANDDigit()
{
}

Float_t BmnLANDDigit::GetBarPosition() const
{
  return 10 * (fBar - 9.5);
}

Bool_t  BmnLANDDigit::IsVertical() const
{
  return 5 == fPlane || 0 == (1 & fPlane);
}

UChar_t BmnLANDDigit::GetPlane() const
{
  return fPlane;
}

UChar_t BmnLANDDigit::GetBar() const
{
  return fBar;
}

UChar_t BmnLANDDigit::GetGlobBar() const
{
  return fGlobBar;
}

Float_t BmnLANDDigit::GetPosition() const
{
  return fPosition;
}

Float_t BmnLANDDigit::GetX() const
{
  return fX;
}

Float_t BmnLANDDigit::GetY() const
{
  return fY;
}

UShort_t BmnLANDDigit::GetTdc(Char_t a_i) const
{
  switch (a_i) {
    case 0: return fTdc0;
    case 1: return fTdc1;
    default:
	    std::cerr << __func__ << ": Invalid TDC ID=" << a_i << ".\n";
	      return -1;
  }
}

UShort_t BmnLANDDigit::GetQdc(Char_t a_i) const
{
  switch (a_i) {
    case 0: return fQdc0;
    case 1: return fQdc1;
    default:
	    std::cerr << __func__ << ": Invalid QDC ID=" << a_i << ".\n";
	      return -1;
  }
}

Float_t BmnLANDDigit::GetTDiff(Char_t a_i) const
{
  switch (a_i) {
    case 0: return fTDiff0;
    case 1: return fTDiff1;
    default:
	    std::cerr << __func__ << ": Invalid TDiff ID=" << a_i << ".\n";
	      return -1;
  }
}

Float_t BmnLANDDigit::GetTime(Char_t a_i) const
{
  switch (a_i) {
    case -1: return fTime;
    case 0: return fTime0;
    case 1: return fTime1;
    default:
	    std::cerr << __func__ << ": Invalid time ID=" << a_i << ".\n";
	      return -1;
  }
}

Float_t BmnLANDDigit::GetEnergy(Char_t a_i) const
{
  switch (a_i) {
    case -1: return fEnergy;
    case 0: return fEnergy0;
    case 1: return fEnergy1;
    default:
	    std::cerr << __func__ << ": Invalid energy ID=" << a_i << ".\n";
	      return -1;
  }
}

ClassImp(BmnLANDDigit)
