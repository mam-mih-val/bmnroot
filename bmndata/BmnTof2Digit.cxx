#include "BmnTof2Digit.h"

BmnTof2Digit::BmnTof2Digit()
{
    fPlane = -1;
    fSignal = -1.0;
}

BmnTof2Digit::BmnTof2Digit(Short_t plane, Float_t signal)
{
    fPlane = plane;
    fSignal = signal;
}

BmnTof2Digit::~BmnTof2Digit()
{
}

ClassImp(BmnTof2Digit)
