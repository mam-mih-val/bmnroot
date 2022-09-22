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
    if (fEventNo % 100 == 0) printf("Events: %d\n", fEventNo);
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
            Redraw();
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
        Redraw();
        //        fReport = new BmnCustomQaReport(fOutName);
        //        fReport->SetOnlyPrimes(fPrimes);
        //        fReport->Create(fHM, fOutputDir);
    }
    can->SaveAs("custom.pdf");
}

void BmnCustomQa::Redraw() {
    BmnHist::FillPadFromTree(fPadTree, fTreeTemp);
    fTreeTemp->Reset();
    BmnHist::DrawPadFromTree(fPadTree);
}

void BmnCustomQa::ReadDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (nullptr == ioman) Fatal("Init", "BmnRootManager is not instantiated");
    fTreeTemp = new TTree(fName + "_tree", "qq");
    TList* brList = ioman->GetBranchNameList();
    for (Int_t i = 0; i < brList->GetEntries(); i++) {
        TString str = static_cast<TObjString*> (brList->At(i))->GetString();
        TObject *obj = ioman->GetObject(str);
        TClass *cl = TClass::GetClass(obj->ClassName());
//        printf("ClassName: %s\n", obj->ClassName());
        if (cl == TClonesArray::Class()) {
            TClonesArray * inTCA = static_cast<TClonesArray*> (obj);
            TClonesArray * newTCA = new TClonesArray(inTCA->GetClass());
            fTreeTemp->Branch(str.Data(), &newTCA);
            fArrVecIn.push_back(inTCA);
            fArrVec.push_back(newTCA);

        } else {
            TNamed* inObj = static_cast<TNamed *> (obj);
//            printf(" in obj n: %s t: %s\n", inObj->GetName(), inObj->GetTitle());
            TObject* workObj = inObj->Clone(/*TString(inObj->GetName()) + "_clone." + str*/)/*funcNew(0)*/;
//            printf("new obj n: %s t: %s\n", workObj->GetName(), workObj->GetTitle());
            TBranch *b = fTreeTemp->Branch(str.Data(), "TObject", workObj);
//            printf(" branch 0x%016lX\n", (uint64_t)b);
            fNamVecIn.push_back(inObj);
            fNamVec.push_back(workObj);
        }
    }
    printf("\nBRANCHES READ!\n\n");
}

void BmnCustomQa::CreateHistograms() {
    // Create number of object histograms

    fPadGenerator = new BmnPadGenerator();
    fPadGenerator->LoadPTFrom(fPadConfFileName);
    fPadTree = fPadGenerator->GetPadBranch();
    can = new TCanvas("CustomCan", "Custom Canvas", 1920, 1080);
    BmnPadGenerator::PadTree2Canvas(fPadTree, can);
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
        
        printf("Title: %10s ClassName: %18s Class_Name: %12s GetName: %12s\n",
                fNamVec[iAr]->GetTitle(), fNamVec[iAr]->ClassName(), fNamVec[iAr]->Class_Name(), fNamVec[iAr]->GetName());
        if (!strcmp(fNamVec[iAr]->ClassName(), "DstEventHeader")) {
            static_cast<DstEventHeader*> (fNamVec[iAr])->CopyFrom(
                    static_cast<DstEventHeader*> (fNamVecIn[iAr]));
            //                cout << "Object " << fNamVec[iAr]->GetName() << endl;
            cout << "EventID: " << static_cast<DstEventHeader*> (fNamVec[iAr])->GetEventId() << endl;
        }
        //        if (!strcmp(fNamVec[iAr]->ClassName(), "CbmVertex")) {
        //            static_cast<CbmVertex*> (fNamVec[iAr])->CopyFrom(
        //                    static_cast<CbmVertex*> (fNamVecIn[iAr]));
        //            //                            cout << "VZ " << static_cast<CbmVertex*> (fNamVec[iAr])->GetZ() << endl;
        //        }
    }
    fTreeTemp->Fill();
    printf("tree len %lld\n", fTreeTemp->GetEntriesFast());
}

ClassImp(BmnCustomQa);
