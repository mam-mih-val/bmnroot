#include "BmnADCDigit.h"

BmnADCDigit::BmnADCDigit() {}

BmnADCDigit::BmnADCDigit(UInt_t iSerial,UChar_t iChannel,UChar_t iSamples,UShort_t *iValue){
  fSerial = iSerial; 
  fChannel= iChannel;
  fSamples=0;
  for(int i=0;i<iSamples;i++) SetSample(iValue[i]); 
}
void  BmnADCDigit::SetSample(UShort_t val){
  if(fSamples<200) fValue[fSamples++]=val; 
}
BmnADCDigit::~BmnADCDigit(){}

ClassImp(BmnADCDigit)
