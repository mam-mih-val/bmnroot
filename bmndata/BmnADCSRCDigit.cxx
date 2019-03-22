#include "BmnADCSRCDigit.h"

BmnADCSRCDigit::BmnADCSRCDigit() :  BmnADCDigit() {
    fTrigTimestamp = 0;
    fAdcTimestamp = 0;
}

BmnADCSRCDigit::BmnADCSRCDigit(
        UInt_t iSerial, UInt_t iChannel, UInt_t n, Short_t *iValue, UShort_t TrigTS, UShort_t AdcTS) :
BmnADCDigit(iSerial, iChannel, n, iValue)
{
    fTrigTimestamp = TrigTS;
    fAdcTimestamp = AdcTS;
}

BmnADCSRCDigit::~BmnADCSRCDigit() {
}

ClassImp(BmnADCSRCDigit)