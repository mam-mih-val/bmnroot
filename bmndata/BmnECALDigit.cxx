#include "BmnECALDigit.h"
#include "TError.h"
#include <iostream>

using namespace std;

void BmnECALMapElement::SetAdcMap(Int_t chan, ULong_t adcId, UShort_t adcChan) {
    if (fChan > -1 && fChan != chan) {
        Error(__func__,"ECAL channel does't match: was %d, become %d, reinitializing", fChan, chan);
    }
    fChan = chan;
    fAdcId = adcId;
    fAdcChan = adcChan;
}

void BmnECALMapElement::SetCoeff(Int_t chan, Double_t coeff) {
    if (fChan > -1 && fChan != chan) {
        Error(__func__,"ECAL channel does't match: was %d, become %d, reinitializing", fChan, chan);
    }
    fChan = chan;
    fCoeff = coeff;
}

void BmnECALMapElement::SetCoords(TGeoNode* cell, TGeoNode* mother) {
    Int_t chan = cell->GetNumber();
    if (fChan > -1 && fChan != chan) {
        Fatal(__func__,"ECAL channel does't match: was %d, become %d, fatal!", fChan, chan);
    }
    static const Double_t base[] = {0.,0.,0.};
    cell->LocalToMaster(base, EcalCoords());
    mother->LocalToMaster(EcalCoords(),LabCoords());
}

void BmnECALMapElement::PrintTitle() {
    cout << "Id#" 
            << "\t" << "fAdcChan" 
            << "\t" << "fChan"
            << "\t" << "fCoeff"
            << "\t" << "fX" 
            << "\t" << "fY"
            << "\t" << "fLabX"
            << "\t" << "fLabY"
            << "\t" << "fLabZ" << "\n";
}

void BmnECALMapElement::Print() {
    cout << "0x" << std::hex << fAdcId << std::dec 
            << "\t" << fAdcChan 
            << "\t" << fChan
            << "\t" << fCoeff
            << "\t" << fX 
            << "\t" << fY
            << "\t" << fLabX
            << "\t" << fLabY
            << "\t" << fLabZ << "\n";
}

BmnECALDigit::BmnECALDigit()
  : BmnCaloDigit()
{
    fPeakAmp = 0;
    fPeakTime = 0;
    fStartTime = 0;
    fLabX = fLabY = fLabZ = 0;
}

BmnECALDigit::BmnECALDigit(Float_t x, Float_t y, UChar_t ch, Float_t amp)
  : BmnCaloDigit(x, y, ch, amp)
{
    fPeakAmp = 0;
    fPeakTime = 0;
    fStartTime = 0;
    fLabX = fLabY = fLabZ = 0;
}

BmnECALDigit::BmnECALDigit(BmnECALMapElement* e) {
    Set(e);
}

void BmnECALDigit::Set(BmnECALMapElement* e, Float_t amp, Float_t peakAmp, Float_t startTime, Float_t peakTime) {
    if (e) {
        fChannel = e->GetChan();
        fX = e->GetX();
        fY = e->GetY();
        fLabX = e->GetLabX();
        fLabY = e->GetLabY();
        fLabZ = e->GetLabZ();
    }
    fAmp = amp;
    fPeakAmp = peakAmp;
    fPeakTime = peakTime;
    fStartTime = startTime;
}


ClassImp(BmnECALDigit)
