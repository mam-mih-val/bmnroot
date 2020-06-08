/* 
 * File:   BmnZdcAnalyzer.cxx
 * Author: pnaleks
 * 
 * Created on 5 июня 2020 г., 18:58
 */

#include "BmnZdcAnalyzer.h"

BmnZdcAnalyzer::BmnZdcAnalyzer() {
}

BmnZdcAnalyzer::~BmnZdcAnalyzer() {
}

InitStatus BmnZdcAnalyzer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfZdcDigits = (TClonesArray*) ioman->GetObject("ZDC");
    fBmnZDCEventData = new BmnZDCEventData();
    ioman->Register("ZDCEventData.", "Zdc", fBmnZDCEventData, kTRUE);
    
    Info(__func__,"ZDC analyzer ready");
    return kSUCCESS;
}

void BmnZdcAnalyzer::Exec(Option_t* opt) {
    fBmnZDCEventData->Set(fArrayOfZdcDigits);
}

//ClassImp(BmnZdcAnalyzer)
