#include "BmnTof2Digit.h"

BmnTof2Digit::BmnTof2Digit()
{
   fPlane=-1;
   fStrip=-1;
   fAmplitude=-1;
   fTime=-1;
   fDiff=-1;
}

BmnTof2Digit::BmnTof2Digit(Short_t plane, Short_t strip, Float_t t, Float_t a, Float_t d)
{
   fPlane=plane;
   fStrip=strip;
   fAmplitude=a;
   fTime=t;
   fDiff=d;
}

BmnTof2Digit::~BmnTof2Digit()
{
}

ClassImp(BmnTof2Digit)
