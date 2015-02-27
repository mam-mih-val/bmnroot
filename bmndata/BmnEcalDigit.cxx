#include "BmnEcalDigit.h"

BmnEcalDigit::BmnEcalDigit()
{
    fPlane = -1;
    fSignal = -1.0;
}

BmnEcalDigit::BmnEcalDigit(Short_t plane, Float_t signal)
{
    fPlane = plane;
    fSignal = signal;
}

BmnEcalDigit::~BmnEcalDigit()
{
}

ClassImp(BmnEcalDigit)
