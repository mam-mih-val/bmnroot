#include "BmnZDCDigit.h"

BmnZDCDigit::BmnZDCDigit()
  : BmnCaloDigit()
{
   fIX = 0;
   fIY = 0;
   fSize = 0;
   fSignalMin = 0.;
   fSignalMax = 0.;
   fSignalPed = 0.;
   fSignalInt = 0.;
}

BmnZDCDigit::BmnZDCDigit(UChar_t ix, UChar_t iy, Float_t x, Float_t y, UChar_t size, UChar_t ch, Float_t amp)
  : BmnCaloDigit(x, y, ch, amp)
{
   fIX = ix;
   fIY = iy;
   fSize = size;
}

BmnZDCDigit::BmnZDCDigit(UChar_t ix, UChar_t iy, Float_t x, Float_t y, UChar_t size, UChar_t ch, Float_t amp,
                         Float_t sigMin, Float_t sigMax, Float_t sigPed, Float_t sigInt)
  : BmnCaloDigit(x, y, ch, amp)
{
   fIX = ix;
   fIY = iy;
   fSize = size;
   fSignalMin = sigMin;
   fSignalMax = sigMax;
   fSignalPed = sigPed;
   fSignalInt = sigInt;
}

BmnZDCDigit::~BmnZDCDigit()
{
}

ClassImp(BmnZDCDigit)
