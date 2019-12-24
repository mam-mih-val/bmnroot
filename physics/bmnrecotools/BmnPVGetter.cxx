#include "BmnPVGetter.h"

BmnPVGetter::BmnPVGetter(TString fname) {
    fDstTreeName = "bmndata";
    fDstFileName = fname;
    fPVertexName = "BmnVertex";
    fDstFile = nullptr;
    fDstTree = nullptr;
    fPVertex = nullptr;
    iEv = -1;
    doNext = kTRUE;
}

BmnPVGetter::~BmnPVGetter() {
}

InitStatus BmnPVGetter::Init() {
    if (fVerbose > 0)
        printf("Primary Vertex Extractor init\n");

    fDstFile = new TFile(fDstFileName, "READ");
    if (fDstFile->IsOpen() == kFALSE) {
        printf("\n!!!!\ncannot open file %s !\n", fDstFileName.Data());
        return kERROR;
    }
    fDstTree = (TTree *) fDstFile->Get(fDstTreeName.Data());
    if (fDstTree->SetBranchAddress(fPVertexName.Data(), &fPVertex) < 0)
        return kERROR;
    nFEvents = fDstTree->GetEntries();

    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fPVertexShow = new TClonesArray(CbmVertex::Class(), 1); //out
    ioman->Register(fPVertexName, "GEM", fPVertexShow, kTRUE);// last arg: save to file
}

void BmnPVGetter::Exec(Option_t *option) {
    if (doNext)
        NextFileEvent();
    doNext = kFALSE;
}

void BmnPVGetter::Finish() {
    fDstFile->Close();
    delete fDstFile;
}

void BmnPVGetter::FinishEvent() {
    //    printf("PVG Finish:  fVZ %f   getsave = %d\n", fVZ, fRunSimInst->GetSaveEvent());
    if (fVZ > MinValidZ && fRunSimInst->GetSaveEvent() == kFALSE)
        return;
    doNext = kTRUE;
}

void BmnPVGetter::NextFileEvent() {
    //    do {
    fPVertexShow->Delete();
    fDstTree->GetEntry(++iEv);
    printf("\nPVs on event %lld\n", iEv);
    for (Int_t iTrack = 0; iTrack < fPVertex->GetEntriesFast(); iTrack++) {
        CbmVertex* v = (CbmVertex*) fPVertex->UncheckedAt(iTrack);
        fGen->SetBeam(v->GetX(), v->GetY(), 0.0, 0.0);
        fGen->SetTarget(v->GetZ(), 0.0);
        fVZ = v->GetZ();
    if (fVerbose > 1)
        printf("v( %f %f %f)\n", v->GetX(), v->GetY(), v->GetZ());
    }
    //    } while (vz < MinValidZ);
    fPVertexShow->AbsorbObjects(fPVertex);
//    if (fVZ < MinValidZ)
//        fRunSimInst->SetSaveEvent(kFALSE);
    if (iEv == nFEvents - 1)
        fRunSimInst->GetMCApp()->StopMCRun();
}

ClassImp(BmnPVGetter)