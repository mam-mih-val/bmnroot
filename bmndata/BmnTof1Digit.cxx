#include "BmnTof1Digit.h"

BmnTof1Digit::BmnTof1Digit()
{
    fPlane = -1;
    fSignal = -1.0;
}

BmnTof1Digit::BmnTof1Digit(Short_t plane, Float_t signal)
{
    fPlane = plane;
    fSignal = signal;
}

BmnTof1Digit::~BmnTof1Digit()
{
}

ClassImp(BmnTof1Digit)
