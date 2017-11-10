#include "BmnLANDDigit.h"
#include "BmnTacquilaDigit.h"

BmnLANDDigit::BmnLANDDigit():
  fPlane(-1),
  fBar(-1),
  fTdc(),
  fQdc(),
  fTime(),
  fPosition(-1)
{
}

BmnLANDDigit::BmnLANDDigit(UChar_t plane, UChar_t bar, BmnTacquilaDigit const
    &a_tacq0, BmnTacquilaDigit const &a_tacq1):
  fPlane(plane),
  fBar(bar),
  fTdc(),
  fQdc(),
  fTime(),
  fPosition()
{
#define COPY(name, i) f##name[i] = a_tacq##i.Get##name()
  COPY(Tdc, 0);
  COPY(Qdc, 0);
  COPY(Time, 0);
  fPosition = fTime[1] - fTime[0];
}

BmnLANDDigit::~BmnLANDDigit()
{
}

Float_t BmnLANDDigit::GetBarPosition() const
{
  return fPosition;
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

Float_t BmnLANDDigit::GetPosition() const
{
  return fPosition;
}

Float_t BmnLANDDigit::GetX() const
{
  return IsVertical() ? GetBarPosition() : fPosition;
}

Float_t BmnLANDDigit::GetY() const
{
  return IsVertical() ? fPosition : GetBarPosition();
}

UChar_t BmnLANDDigit::GetTdc(UChar_t a_i) const
{
  return fTdc[a_i];
}

UChar_t BmnLANDDigit::GetQdc(UChar_t a_i) const
{
  return fQdc[a_i];
}

UChar_t BmnLANDDigit::GetTime(UChar_t a_i) const
{
  return fTime[a_i];
}

ClassImp(BmnLANDDigit)
