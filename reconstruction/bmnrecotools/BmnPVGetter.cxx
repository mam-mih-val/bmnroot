#include "BmnPVGetter.h"

BmnPVGetter::BmnPVGetter(TString fname, Long64_t nEvs) {
    fDstTreeName = "bmndata";
    fDstFileName = fname;
    fPVertexName = "BmnVertex";
    fEvHeaderName = "DstEventHeader.";
    fDstFile = nullptr;
    fDstTree = nullptr;
    fPVertex = nullptr;
    fEvHeader = nullptr;
    iEv = -1;
    doNext = kTRUE;
    if (nEvs == 0LL)
        nFEvents = LONG_LONG_MAX;
    else
        nFEvents = nEvs;
}

BmnPVGetter::~BmnPVGetter() {
}

InitStatus BmnPVGetter::Init() {
    if (fVerbose > 0)
        printf("Primary Vertex Extractor init\n");

    fDstFile = new TFile(fDstFileName, "READ");
    if (fDstFile->IsOpen() == kFALSE) {
        printf("\n!!!!\ncannot open file %s !\n", fDstFileName.Data());
        return kFATAL;
    }
    fDstTree = (TTree *) fDstFile->Get(fDstTreeName.Data());
    if (fDstTree->SetBranchAddress(fPVertexName.Data(), &fPVertex) < 0)
        return kFATAL;
    if (fDstTree->SetBranchAddress(fEvHeaderName.Data(), &fEvHeader) < 0)
        return kFATAL;
    nFEvents = Min(fDstTree->GetEntries(), nFEvents);
    if (fVerbose > 0)
        printf("Primary Vertex Extractor : will run up to %lld events.\n", nFEvents);

    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fPVertexShow = new TClonesArray(CbmVertex::Class(), 1); //out
    ioman->Register(fPVertexName, "GEM", fPVertexShow, kTRUE); // last arg: save to file
    fEvHeaderShow = new DstEventHeader(); //out
    ioman->Register(fEvHeaderName, "", fEvHeaderShow, kTRUE); // last arg: save to file

    return kSUCCESS;
}

void BmnPVGetter::Exec(Option_t *option) {
    if (doNext)
        NextFileEvent();
    doNext = kFALSE;
}

void BmnPVGetter::Finish() {
    printf("\n%s: Iterated over %lld of %lld events\n", typeid (*this).name(), iEv + 1, nFEvents);
    fDstFile->Close();
    delete fDstFile;
}

void BmnPVGetter::FinishEvent() {
    if (fVerbose > 1)
        printf("PVG Finish:  fVZ %f   getsave = %d\n", fVZ, fRunSimInst->GetSaveEvent());
    if (Abs(fVZ) < Abs(CutValidZ) && fRunSimInst->GetSaveEvent() == kFALSE)
        return;
    doNext = kTRUE;
}

void BmnPVGetter::NextFileEvent() {
    //    do {
    fPVertexShow->Delete();
    fDstTree->GetEntry(++iEv);
    printf("\nPVs on event %lld eventID = %u\n", iEv, fEvHeader->GetEventId());
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
    fEvHeaderShow->SetEventId(fEvHeader->GetEventId());
    if (iEv == nFEvents - 1)
        fRunSimInst->GetMCApp()->StopMCRun();
}

ClassImp(BmnPVGetter)
