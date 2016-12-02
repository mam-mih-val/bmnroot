#include "BmnSiliconRaw2Digit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
    fPeriod = -1;
    fRun = -1;
    fEventId = -1;
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run) {

    fPeriod = period;
    fRun = run;
    fEventId = 0;
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon) {
    fEventId++;
//    if (fEventId == N_EV_FOR_PEDESTALS)
//        FindNoisyStrips();
//    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
//        BmnADC32Digit* adcDig = (BmnADC32Digit*) adc->At(iAdc);
//        if (fAdcProfiles.find(adcDig) == fAdcProfiles.end()) {
//            UInt_t* signals = new UInt_t[ADC_N_SAMPLES];
//            for (Int_t k = 0; k < ADC_N_SAMPLES; ++k) signals[k] = 0;
//            fAdcProfiles.insert(pair < BmnADC32Digit*, UInt_t*>(adcDig, signals));
//        }
//        UInt_t ch = adcDig->GetChannel() * ADC_N_SAMPLES;
//        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
//            GemMapStructure gemM = fMap[iMap];
//            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
//                ProcessDigit(adcDig, &gemM, gem);
//                break;
//            }
//        }
//    }
}

ClassImp(BmnSiliconRaw2Digit)
