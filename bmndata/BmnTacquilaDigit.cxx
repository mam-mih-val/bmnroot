#include "BmnTacquilaDigit.h"
#include <iostream>

/* Magic! */
#define TACQUILA_CLOCK_MHZ 40.002903

BmnTacquilaDigit::BmnTacquilaDigit():
  fSam(-1),
  fGtb(-1),
  fModule(-1),
  fChannel(-1),
  fTdc(-1),
  fClock(-1),
  fQdc(-1),
  fTCal(-1),
  fTDiff(-1)
{
}

BmnTacquilaDigit::BmnTacquilaDigit(UInt_t sam, UInt_t gtb, UInt_t module,
    UInt_t channel, UInt_t tdc, UInt_t clock, UInt_t qdc):
  fSam(sam),
  fGtb(gtb),
  fModule(module),
  fChannel(channel),
  fTdc(tdc),
  fClock(clock),
  fQdc(qdc),
  fTCal(-1),
  fTDiff(-1)
{
}

BmnTacquilaDigit::~BmnTacquilaDigit()
{
}

UInt_t BmnTacquilaDigit::GetGtb() const
{
  return fGtb;
}

UInt_t BmnTacquilaDigit::GetModule() const
{
  return fModule;
}

UInt_t BmnTacquilaDigit::GetChannel() const
{
  return fChannel;
}

UInt_t BmnTacquilaDigit::GetTdc() const
{
  return fTdc;
}

UInt_t BmnTacquilaDigit::GetClock() const
{
  return fClock;
}

UInt_t BmnTacquilaDigit::GetQdc() const
{
  return fQdc;
}

Float_t BmnTacquilaDigit::GetTCal() const
{
  return fTCal;
}

Float_t BmnTacquilaDigit::GetTDiff() const
{
  return fTDiff;
}

void BmnTacquilaDigit::SetTCal(Float_t tcal)
{
  fTCal = tcal;
}

void BmnTacquilaDigit::SetTDiff(BmnTacquilaDigit const &a_c17)
{
  fTDiff = fTCal + (1000. * fClock / TACQUILA_CLOCK_MHZ) - a_c17.fTCal;
}

ClassImp(BmnTacquilaDigit)
