#include "BmnTof1Digit.h"

BmnTof1Digit::BmnTof1Digit()
{
   fPlane=-1;
   fStrip=-1;
   fSide=-1;
   fAmplitude=-1;
   fTime=-1;
}

BmnTof1Digit::BmnTof1Digit(Short_t plane, Short_t strip, Short_t side,Float_t t,Float_t a)
{
   fPlane=plane;
   fStrip=strip;
   fSide=side;
   fAmplitude=a;
   fTime=t;
}

BmnTof1Digit::~BmnTof1Digit()
{
}

ClassImp(BmnTof1Digit)
