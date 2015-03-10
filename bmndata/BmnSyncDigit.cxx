#include "BmnSyncDigit.h"

BmnSyncDigit::BmnSyncDigit() {}

BmnSyncDigit::BmnSyncDigit(UInt_t iSerial,UInt_t iEvent,ULong_t t_sec,ULong_t t_ns){
  fSerial = iSerial; 
  fEvent  = iEvent;
  fT_sec  = t_sec;
  fT_ns   = t_ns;
}

BmnSyncDigit::~BmnSyncDigit(){}

ClassImp(BmnSyncDigit)
