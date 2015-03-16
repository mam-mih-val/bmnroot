#include "BmnTof2Digit.h"

BmnTof2Digit::BmnTof2Digit()
{
   fPlane=-1;
   fStrip=-1;
   fSide=-1;
   fAmplitude=-1;
   fTime=-1;
}

BmnTof2Digit::BmnTof2Digit(Short_t plane, Short_t strip, Short_t side,Float_t t,Float_t a)
{
   fPlane=plane;
   fStrip=strip;
   fSide=side;
   fAmplitude=a;
   fTime=t;
}

BmnTof2Digit::~BmnTof2Digit()
{
}

ClassImp(BmnTof2Digit)
