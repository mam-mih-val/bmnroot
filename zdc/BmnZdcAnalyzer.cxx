/* 
 * File:   BmnZdcAnalyzer.cxx
 * Author: pnaleks
 * 
 * Created on 5 июня 2020 г., 18:58
 */

#include "BmnZdcAnalyzer.h"
#include "FairLogger.h"

BmnZdcAnalyzer::BmnZdcAnalyzer() {
}

BmnZdcAnalyzer::~BmnZdcAnalyzer() {
}

InitStatus BmnZdcAnalyzer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfZdcDigits = (TClonesArray*) ioman->GetObject("ZdcDigi");
    if(fArrayOfZdcDigits == nullptr)
    {
        LOG(ERROR)<<"BmnZdcAnalyzer::Init() branch 'ZdcDigi' not found! Task will be deactivated";
        SetActive(kFALSE);
        return kERROR;
    }

    fBmnZDCEventData = new BmnZDCEventData();
    ioman->Register("ZDCEventData.", "Zdc", fBmnZDCEventData, kTRUE);
    
    Info(__func__,"ZDC analyzer ready");
    return kSUCCESS;
}

void BmnZdcAnalyzer::Exec(Option_t* opt) {
    if (!IsActive())
        return;

    fBmnZDCEventData->Set(fArrayOfZdcDigits, fModuleScale, fModuleThreshold);
}

ClassImp(BmnZdcAnalyzer)
