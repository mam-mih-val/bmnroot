/**
 * \file BmnCustomQa.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \author Ilnur Gabdrakhmanov <ilnur@jinr.ru> - disentangle Exp and MC processing
 * \date 2007-2022
 */

#include "BmnCustomQa.h"

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

BmnCustomQa::BmnCustomQa(string name, string padConfFile) : BmnQaBase("BmnCustomQa", 1),
fTreeTemp(nullptr),
fPadGenerator(nullptr),
fPadTree(nullptr) {
    fPadConfFileName = padConfFile;
}

BmnCustomQa::~BmnCustomQa() {
}

InitStatus BmnCustomQa::Init() {
    fHM = new BmnHistManager();
    CreateHistograms();
    ReadDataBranches();
    if (fMonitorMode) {
        //        fReport = new BmnCustomQaReport(fOutName);
        //        fReport->SetHM(fHM);
        //        fReport->SetOnlyPrimes(fPrimes);
        //        fReport->SetMonitorMode(fMonitorMode);
        //        fReport->SetObjServer(fServer);
        //        fTicksLastUpdate = chrono::system_clock::now();
        //        fReport->CallDraw();
        //        fReport->Register("/");
        //        //        fServer->SetTimer(50, kFALSE);
    } else {

    }
    return kSUCCESS;
}

void BmnCustomQa::Exec(Option_t* opt) {
    if (fEventNo % 100 == 0) printf("Event: %d\n", fEventNo);
    fEventNo++;
    // Int_t nHits = 0;
    // if (fInnerTrackerSetup[kGEM]) nHits += fGemHits->GetEntriesFast();
    // if (fInnerTrackerSetup[kSILICON]) nHits += fSilHits->GetEntriesFast();
    // if (fInnerTrackerSetup[kSSD]) nHits += fSsdHits->GetEntriesFast();
    // if (nHits > fNHitsCut || nHits == 0) return;
    // Increase event counter
    //    fHM->H1("hen_EventNo_TrackingQa")->Fill(0.5);
    ReadEventHeader();
    ProcessGlobal();
    if (fMonitorMode) {
        fNItersSinceUpdate++;
        fServer->ProcessRequests();
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
        chrono::seconds time = chrono::duration_cast<chrono::seconds>(now - fTicksLastUpdate);
        time_t tt = chrono::system_clock::to_time_t(now);
        //        printf("time %s\n", ctime(&tt));
        if ((time > fTimeToUpdate) || (fNItersSinceUpdate > fNItersToUpdate)) {
            BmnHist::FillPadTree(fPadTree, fTreeTemp);
            fTreeTemp->Reset();
            BmnHist::DrawPadTree(fPadTree);
            if (fVerbose)
                printf("Draw! iters %d\n", fNItersSinceUpdate);
            fTicksLastUpdate = now;
            fNItersSinceUpdate = 0;
        }
    }
}

void BmnCustomQa::Finish() {
    fHM->WriteToFile();
    if (!fMonitorMode) {
        //        fReport = new BmnCustomQaReport(fOutName);
        //        fReport->SetOnlyPrimes(fPrimes);
        //        fReport->Create(fHM, fOutputDir);
    }
    can->SaveAs("custom.pdf");
}

void BmnCustomQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman) Fatal("Init", "BmnRootManager is not instantiated");
    fTreeTemp = new TTree(fName + "_tree", "qq");
    TList* brList = ioman->GetBranchNameList();
    for (Int_t i = 0; i < brList->GetEntries(); i++) {
        TString str = static_cast<TObjString*> (brList->At(i))->GetString();
        cout << str << endl;
        if (str.Contains(".")) {
            if (str.Contains("ZDCEventData.") || str.Contains("DstEventHeader."))
                continue;
            TNamed* inObj = static_cast<TNamed *> (ioman->GetObject(str));
            TClass *cl = TClass::GetClass(inObj->ClassName());
            printf("cl name %s\n", cl->GetName());
            ROOT::NewFunc_t funcNew = cl->GetNew();
            TNamed* workObj = static_cast<TNamed*> (funcNew(0));
            //            workObj->SetNameTitle(inObj->GetName(), inObj->GetTitle());
            workObj->SetNameTitle((string(inObj->GetName()) + "_clone").c_str(), inObj->GetTitle());
            fTreeTemp->Branch(str.Data(), &workObj);
            fNamVecIn.push_back(inObj);
            fNamVec.push_back(workObj);
        } else {

            TClonesArray * inTCA = static_cast<TClonesArray*> (ioman->GetObject(str));
            TClass *cl = inTCA->GetClass();
            printf("cl name %s\n", cl->GetName());
            TClonesArray * newTCA = new TClonesArray(inTCA->GetClass());
//            newTCA->SetName(inTCA->GetName());
            fTreeTemp->Branch(str.Data(), &newTCA);
            fArrVecIn.push_back(inTCA);
            fArrVec.push_back(newTCA);

        }
    }
    printf("\nBRANCHES READ!\n\n");
}

void BmnCustomQa::ReadEventHeader() {
    //    FairMCEventHeader* evHead = (FairMCEventHeader*)FairRootManager::Instance()->GetObject("MCEventHeader.");
    //    fHM->H1("Impact parameter")->Fill(evHead->GetB());
    //    fHM->H1("Multiplicity")->Fill(evHead->GetNPrim());
    //    fHM->H2("Impact_Mult")->Fill(evHead->GetB(), evHead->GetNPrim());
}

void BmnCustomQa::CreateHistograms() {


    // Create number of object histograms
    UInt_t nofBinsC = 100000;
    Double_t maxXC = (Double_t) nofBinsC;

    fPadGenerator = new BmnPadGenerator();
    fPadGenerator->LoadPTFrom(fPadConfFileName);
    fPadTree = fPadGenerator->GetPadBranch();
    can = new TCanvas("CustomCan", "Custom Canvas", 1920, 1080);
    fPadGenerator->PadTree2Canvas(fPadTree, can);
    fServer->Register("/", can);
    printf("\nHISTOGRAMS CREATED!\n\n");

}

void BmnCustomQa::ProcessGlobal() {
    for (UInt_t iAr = 0; iAr < fArrVec.size(); iAr++) {
        fArrVec[iAr]->Delete();
        fArrVec[iAr]->AbsorbObjects(fArrVecIn[iAr]);
        //                    cout << "Count of " << fArrVec[iAr]->GetName() << " digits: " << fArrVec[iAr]->GetEntriesFast() << endl;
    }
    for (UInt_t iAr = 0; iAr < fNamVec.size(); iAr++) {
        //        printf("ClassName %20s  Class_Name %20s  GetName %20s\n", fNamVec[iAr]->ClassName(), fNamVec[iAr]->Class_Name(), fNamVec[iAr]->GetName());
        if (!strcmp(fNamVec[iAr]->ClassName(), "DstEventHeader")) {
            static_cast<DstEventHeader*> (fNamVec[iAr])->CopyFrom(
                    static_cast<DstEventHeader*> (fNamVecIn[iAr]));
            //                cout << "Object " << fNamVec[iAr]->GetName() << endl;
            cout << "EventID: " << static_cast<DstEventHeader*> (fNamVec[iAr])->GetEventId() << endl;
        }
        if (!strcmp(fNamVec[iAr]->ClassName(), "CbmVertex")) {
            static_cast<CbmVertex*> (fNamVec[iAr])->CopyFrom(
                    static_cast<CbmVertex*> (fNamVecIn[iAr]));
            //                            cout << "VZ " << static_cast<CbmVertex*> (fNamVec[iAr])->GetZ() << endl;
        }
    }
    fTreeTemp->Fill();
    //    printf("tree len %lld\n", fTreeTemp->GetEntriesFast());
}



ClassImp(BmnCustomQa);
