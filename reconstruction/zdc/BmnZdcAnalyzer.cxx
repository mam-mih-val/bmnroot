/* 
 * File:   BmnZdcAnalyzer.cxx
 * Author: pnaleks
 * 
 * Created on 5 июня 2020 г., 18:58
 */

#include "BmnZdcAnalyzer.h"
#include "FairLogger.h"
#include <TStopwatch.h>

using namespace std;
static Float_t workTime = 0.0;

BmnZdcAnalyzer::BmnZdcAnalyzer() :
  FairTask("BmnZdcAnalyzer"),

  fArrayOfZdcDigits(nullptr),
  fBmnZDCEventData(nullptr),

  fModuleScale(nullptr),
  fModuleThreshold(nullptr)
{}

BmnZdcAnalyzer::~BmnZdcAnalyzer() {
}

InitStatus BmnZdcAnalyzer::Init()
{
    FairRootManager* ioman = FairRootManager::Instance();
    fArrayOfZdcDigits = (TClonesArray*) ioman->GetObject("ZdcDigit");
    if (fArrayOfZdcDigits == nullptr)
    {
        // check if there is an old name of the branch ("ZDC")
        fArrayOfZdcDigits = (TClonesArray*) ioman->GetObject("ZDC");
        if (fArrayOfZdcDigits == nullptr)
        {
            LOG(error)<<"BmnZdcAnalyzer::Init() branch 'ZdcDigit' or old 'ZDC' not found! Task will be deactivated";
            SetActive(kFALSE);
            return kERROR;
        }
    }

    fBmnZDCEventData = new BmnZDCEventData();
    ioman->Register("ZDCEventData.", "Zdc", fBmnZDCEventData, kTRUE);
    
    Info(__func__,"ZDC analyzer ready");
    return kSUCCESS;
}

void BmnZdcAnalyzer::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();
    
    if (!IsActive())
        return;

    fBmnZDCEventData->Set(fArrayOfZdcDigits, fModuleScale, fModuleThreshold);

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnZdcAnalyzer::Finish() {
    printf("Work time of BmnZdcAnalyzer: %4.2f sec.\n", workTime);
}


ClassImp(BmnZdcAnalyzer)
