/**
 * \file BmnQaBase.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#include "BmnQaBase.h"

#include <fstream>
#include <iostream>

#include "BmnAcceptanceFunction.h"
#include "BmnDchHit.h"
#include "BmnDchTrack.h"
#include "BmnEnums.h"
#include "BmnGemStripHit.h"
#include "BmnGemTrack.h"
#include "BmnMCPoint.h"
#include "BmnMatch.h"
#include "BmnMath.h"
#include "BmnSiliconHit.h"
#include "BmnTrackMatch.h"
#include "BmnUtils.h"
#include "CbmBaseHit.h"
#include "CbmGlobalTrack.h"
#include "BmnGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsTrack.h"
#include "CbmTofHit.h"
#include "BmnVertex.h"
#include "FairMCEventHeader.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TFitResult.h"
#include "TH1.h"
#include "TH2F.h"
#include "BmnHistManager.h"
#include "BmnSiliconPoint.h"
#include "CbmStsPoint.h"

using namespace std;
using namespace TMath;
using lit::FindAndReplace;
using lit::Split;

BmnQaBase::BmnQaBase(const char* name, Int_t iVerbose) : FairTask(name, iVerbose),
fHM(nullptr),
fReport(nullptr),
fMonitorMode(kFALSE),
fOutputDir("./"),
fOutName(name),
fEventNo(0),
fServer(nullptr),
fNItersSinceUpdate(0) {
}

BmnQaBase::~BmnQaBase() {
    printf("~BmnQaBase()\n");
    if (fHM) delete fHM;
    if (fReport) delete fReport;
}

InitStatus BmnQaBase::Init() {
    fHM = new BmnHistManager();
    CreateHistograms();
    ReadDataBranches();
    if (fMonitorMode) {
        fReport = new BmnTrackingQaExpReport(fOutName);
        fReport->SetHM(fHM);
        fReport->SetMonitorMode(fMonitorMode);
        fReport->SetObjServer(fServer);
        fTicksLastUpdate = chrono::system_clock::now();
        fReport->CallDraw();
        fReport->Register("/");
        //        fServer->SetTimer(50, kFALSE);
    } else {

    }
    return kSUCCESS;
}

void BmnQaBase::Exec(Option_t* opt) {
    if (fEventNo % 100 == 0) printf("Event: %d\n", fEventNo);
    fEventNo++;
    // Increase event counter
    fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
    ReadEventHeader();
    ProcessGlobal();
    if (fMonitorMode) {
        fNItersSinceUpdate++;
//        fServer->ProcessRequests();
        gSystem->ProcessEvents();
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
        chrono::seconds time = chrono::duration_cast<chrono::seconds>(now - fTicksLastUpdate);
        time_t tt = chrono::system_clock::to_time_t(now);
        printf("\ntime %s\n", ctime(&tt));
        if ((time > fTimeToUpdate) || (fNItersSinceUpdate > fNItersToUpdate)) {
        fReport->CallDraw();
            printf("\n\nDraw! iters %d\n\n", fNItersSinceUpdate);
            fTicksLastUpdate = now;
            fNItersSinceUpdate = 0;
    }
}
}

void BmnQaBase::Finish() {
    fHM->WriteToFile();
    if (!fMonitorMode) {
        fReport = new BmnTrackingQaExpReport(fOutName);
        fReport->Create(fHM, fOutputDir);
    }
}

void BmnQaBase::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman) Fatal("Init", "BmnRootManager is not instantiated");
    printf("\nBRANCHES READ!\n\n");
}

void BmnQaBase::ReadEventHeader() {
    //    FairMCEventHeader* evHead = (FairMCEventHeader*)FairRootManager::Instance()->GetObject("MCEventHeader.");
    //    fHM->H1("Impact parameter")->Fill(evHead->GetB());
    //    fHM->H1("Multiplicity")->Fill(evHead->GetNPrim());
    //    fHM->H2("Impact_Mult")->Fill(evHead->GetB(), evHead->GetNPrim());
}

void BmnQaBase::CreateH1(const string& name, const string& xTitle, const string& yTitle, Int_t nofBins, Double_t minBin, Double_t maxBin) {
    TH1F* h = new TH1F(name.c_str(), string(name + ";" + xTitle + ";" + yTitle).c_str(), nofBins, minBin, maxBin);
    fHM->Add(name, h);
}

void BmnQaBase::CreateH2(
        const string& name,
        const string& xTitle,
        const string& yTitle,
        const string& zTitle,
        Int_t nofBinsX,
        Double_t minBinX,
        Double_t maxBinX,
        Int_t nofBinsY,
        Double_t minBinY,
        Double_t maxBinY) {
    TH2F* h = new TH2F(name.c_str(), (name + ";" + xTitle + ";" + yTitle + ";" + zTitle).c_str(), nofBinsX, minBinX, maxBinX, nofBinsY, minBinY, maxBinY);
    fHM->Add(name, h);
}
//
//
//void BmnQaBase::CreateHistograms() {
//
//
//    printf("\nHISTOGRAMS CREATED!\n\n");
//
//}
//
//void BmnQaBase::ProcessGlobal() {
//}

ClassImp(BmnQaBase);
