
#include "BmnEcalDigit.h"

BmnEcalDigit::BmnEcalDigit(){
   fX=-1;
   fY=-1;
   fSize=0;
   fChannel=-1;
   fSamples=0;
}

BmnEcalDigit::BmnEcalDigit(Char_t x,Char_t y,Char_t size,Char_t ch,Short_t samples,UShort_t *data){
   fX=x;
   fY=y;
   fSize=size;
   fChannel=ch;
   fSamples=samples;
   fWaveform.Set(samples);
   for(int i=0;i<samples;i++) fWaveform.AddAt(data[i],i);
}

void BmnEcalDigit::SetSamples(Short_t samples){
   fSamples=samples;
   fWaveform.Set(samples);
}
void BmnEcalDigit::SetWaveform(Short_t sample,Float_t val){
  if(sample<fSamples && sample>=0) fWaveform[sample]=val;
}
unsigned short *BmnEcalDigit::GetWaveform(){ return (unsigned short *)fWaveform.GetArray();}

BmnEcalDigit::~BmnEcalDigit()
{
}

ClassImp(BmnEcalDigit)
