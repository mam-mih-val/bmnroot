#include <TChain.h>
#include <zlib.h>
#include <TRandom.h>
#include "TSystem.h"

#include "BmnProfAsic2Raw.h"

#include "BmnEventHeader.h"
#include "FairRunAna.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

BmnProfAsic2Raw::BmnProfAsic2Raw() {
    fInputAdcBranchName = "ADC192ASIC";
    fWorkAdcBranchName = "ADC32PROF";
    fOutputProfBranchName = "BmnProfDigit";

    fPedEvCntr = 0;
    fNoiseEvCntr = 0;
    fEvForPedestals = 500;
    fPedEnough = kFALSE;

    fVerbose = 1;
}

BmnProfAsic2Raw::BmnProfAsic2Raw(Int_t run_period, Int_t run_number) : BmnProfAsic2Raw() {
}

BmnProfAsic2Raw::~BmnProfAsic2Raw() {
}

InitStatus BmnProfAsic2Raw::Init() {

    if (fVerbose > 1) cout << "=================== BmnProfAsic2Raw::Init() started ====================" << endl;

    FairRootManager* ioman = FairRootManager::Instance();

    fInAdcArray = (TClonesArray*) ioman->GetObject(fInputAdcBranchName);
    if (!fInAdcArray) {
        cout << "BmnProfAsic2Raw::Init(): branch " << fInputAdcBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }

    fWorkAdcArray = new TClonesArray(BmnADCDigit::Class());
    ioman->Register(fWorkAdcBranchName, "ADC32Prof", fWorkAdcArray, kTRUE);

    fBmnProfDigitArray = new TClonesArray(BmnSiBTDigit::Class());
    ioman->Register(fOutputProfBranchName, "Prof", fBmnProfDigitArray, kTRUE);

    TString gPathCSCConfig = gSystem->Getenv("VMCWORKDIR");
    gPathCSCConfig += "/parameters/csc/XMLConfigs/";

    //--------------------------------------------------------------------------
    fMapper = new BmnProfRaw2Digit(8, 0);
    vector<uint32_t> vSer;
    for (auto &it : fMapper->GetGlobalMap())
        vSer.push_back(it.first.first);
    fMapper->SetSerials(vSer);
    if (fVerbose > 1) cout << "=================== BmnProfAsic2Raw::Init() finished ===================" << endl;

    return kSUCCESS;
}

void BmnProfAsic2Raw::Exec(Option_t* opt) {
    TStopwatch sw;
    sw.Start();
    if (!IsActive())
        return;
    fBmnProfDigitArray->Delete();

    if (fVerbose > 1) cout << "=================== BmnProfAsic2Raw::Exec() started ====================" << endl;
    if (fVerbose > 1) cout << " BmnProfAsic2Raw::Exec(), Number of BmnAdcDigits = " << fInAdcArray->GetEntriesFast() << "\n";

    ProcessDigits();

    if (fVerbose > 1) cout << "=================== BmnProfAsic2Raw::Exec() finished ===================" << endl;

    sw.Stop();
    workTime += sw.RealTime();
    return;
}

void BmnProfAsic2Raw::ProcessDigits() {

    fMapper->FillExtractedBits(fInAdcArray, fWorkAdcArray);

//    if (!fPedEnough) {
//        if (fPedEvCntr != fEvForPedestals) {
//            CopyDataToPedMap(fWorkAdcArray, fPedEvCntr);
//            fPedEvCntr++;
//            if (fVerbose == 1) {
//                if (fPedEvCntr % 100 == 0 && fPedEvCntr > 0) cout << "Pedestal event #" << fPedEvCntr << "/" << fEvForPedestals << ";" << endl;
//            } else if (fVerbose == 0)
//                DrawBar(fPedEvCntr, fEvForPedestals);
//        } else {
//            fPedEnough = kTRUE;
//            fMapper->RecalculatePedestalsAugmented();
//        }
//    } else
        fMapper->FillEvent(fWorkAdcArray, fBmnProfDigitArray);
    return;
}

void BmnProfAsic2Raw::Finish() {
    delete fMapper;
    printf("Work time of BmnProfAsic2Raw: %4.2f sec.\n", workTime);
    return;
}

void BmnProfAsic2Raw::CopyDataToPedMap(TClonesArray* adcGem, UInt_t ev) {
    Double_t**** pedData = fMapper->GetPedData();
    for (UInt_t iAdc = 0; iAdc < adcGem->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adcGem->At(iAdc);
        for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl) {
            pedData[adcDig->GetSerial()][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16.0;
        }
        break;
    }
    return;
}

ClassImp(BmnProfAsic2Raw)
