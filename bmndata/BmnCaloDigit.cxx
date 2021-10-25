#include "BmnCaloDigit.h"

BmnCaloDigit::BmnCaloDigit()
  : TNamed(),
    fX(0),
    fY(0),
    fChannel(0),
    fAmp(0)
{}

BmnCaloDigit::BmnCaloDigit(Float_t x, Float_t y, UShort_t ch, Float_t amp)
  : TNamed(),
    fX(x),
    fY(y),
    fChannel(ch),
    fAmp(amp)
{}

ClassImp(BmnCaloDigit)
