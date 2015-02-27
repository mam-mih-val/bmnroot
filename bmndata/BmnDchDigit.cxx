#include "BmnDchDigit.h"

BmnDchDigit::BmnDchDigit()
{
    fPlane = -1;
    fSignal = -1.0;
}

BmnDchDigit::BmnDchDigit(Short_t plane, Float_t signal)
{
    fPlane = plane;
    fSignal = signal;
}

BmnDchDigit::~BmnDchDigit()
{
}

ClassImp(BmnDchDigit)
