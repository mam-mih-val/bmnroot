
#include "BmnZDCDigit.h"

BmnZDCDigit::BmnZDCDigit(){
   fX=-1;
   fY=-1;
   fSize=0;
   fChannel=-1;
   fSamples=0;
}

BmnZDCDigit::BmnZDCDigit(Char_t x,Char_t y,Char_t size,Char_t ch,Short_t samples,UShort_t *data){
   fX=x;
   fY=y;
   fSize=size;
   fChannel=ch;
   fSamples=samples;
   fWaveform.Set(samples);
   for(int i=0;i<samples;i++) fWaveform.AddAt(data[i],i);
}

void BmnZDCDigit::SetSamples(Short_t samples){
   fSamples=samples;
   fWaveform.Set(samples);
}
void BmnZDCDigit::SetWaveform(Short_t sample,Float_t val){
  if(sample<fSamples && sample>=0) fWaveform[sample]=val;
}
unsigned short *BmnZDCDigit::GetWaveform(){ return (unsigned short *)fWaveform.GetArray();}

BmnZDCDigit::~BmnZDCDigit()
{
}

ClassImp(BmnZDCDigit)
