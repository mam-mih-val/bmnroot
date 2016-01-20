#include "BmnSyncDigit.h"

BmnSyncDigit::BmnSyncDigit() {}

BmnSyncDigit::BmnSyncDigit(UInt_t iSerial,UInt_t iEvent,long long t_sec,long long t_ns){
  fSerial = iSerial; 
  fEvent  = iEvent;
  fT_sec  = t_sec;
  fT_ns   = t_ns;
}

BmnSyncDigit::~BmnSyncDigit(){}

ClassImp(BmnSyncDigit)
