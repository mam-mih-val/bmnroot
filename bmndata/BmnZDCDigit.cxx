
#include "BmnZDCDigit.h"

BmnZDCDigit::BmnZDCDigit(){
   fIX=0;
   fIY=0;
   fX=0;
   fY=0;
   fSize=0;
   fChannel=0;
   fAmp=0;
}

BmnZDCDigit::BmnZDCDigit(UChar_t ix,UChar_t iy,Float_t x,Float_t y,UChar_t size,UChar_t ch,Float_t amp){
   fIX=ix;
   fIY=iy;
   fX=x;
   fY=y;
   fSize=size;
   fChannel=ch;
   fAmp=amp;
}

BmnZDCDigit::~BmnZDCDigit()
{
}

ClassImp(BmnZDCDigit)
