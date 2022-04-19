#include "BmnTTBDigit.h"

BmnTTBDigit::BmnTTBDigit() {}

BmnTTBDigit::BmnTTBDigit(UInt_t iSerial,UInt_t iEvent,UInt_t iSlot,ULong_t lo,ULong_t hi){
  fSlot   = iSlot; 
  fSerial = iSerial; 
  fEvent  = iEvent;
  fT_lo   = lo;
  fT_hi   = hi;
}

BmnTTBDigit::~BmnTTBDigit(){}

ClassImp(BmnTTBDigit)
