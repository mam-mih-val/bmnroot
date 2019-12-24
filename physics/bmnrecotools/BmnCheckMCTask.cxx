#include "BmnCheckMCTask.h"

BmnCheckMCTask::BmnCheckMCTask(Long64_t nevents, Int_t minHits) {
    fMCTracksBranchName = "MCTrack";
    fGemPointsBranchName = "StsPoint";
    fSilPointsBranchName = "SiliconPoint";
    fCSCPointsBranchName = "CSCPoint";
    fPVertexName = "BmnVertex";
    fMCTracks = nullptr;
    fGemPoints = nullptr;
    fSilPoints = nullptr;
    fCSCPoints = nullptr;
    fRunSimInst = nullptr;
    fPVertexShow = nullptr;
    fMinHits = minHits;
    nMaxValidEvents = nevents;
    nValidEvents = 0;
}

BmnCheckMCTask::~BmnCheckMCTask() {
}

InitStatus BmnCheckMCTask::Init() {
    if (fVerbose > 0)
        printf("Recon Checker Init\n");

    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");
    fMCTracks = (TClonesArray*) ioman->GetObject(fMCTracksBranchName);
    fGemPoints = (TClonesArray*) ioman->GetObject(fGemPointsBranchName);
    fSilPoints = (TClonesArray*) ioman->GetObject(fSilPointsBranchName);
    fCSCPoints = (TClonesArray*) ioman->GetObject(fCSCPointsBranchName);
    fPVertexShow = (TClonesArray*) ioman->GetObject(fPVertexName);
}

void BmnCheckMCTask::Exec(Option_t* option) {
//    if (fVerbose > 1)
//        printf("\n%s : exec  fRunSimInst->GetSaveEvent() = %d\n", typeid (*this).name(), fRunSimInst->GetSaveEvent());
    if (fMCTracks == nullptr)
        return;
    for (Int_t iTrack = 0; iTrack < fPVertexShow->GetEntriesFast(); iTrack++) {
        CbmVertex* v = (CbmVertex*) fPVertexShow->UncheckedAt(iTrack);
        fVZ = v->GetZ();
//        if (fVerbose > 1)
//            printf(" Read PV ( %f %f %f)\n", v->GetX(), v->GetY(), v->GetZ());
    }
    if (fVZ < MinValidZ)
        return;
    if (BmnRecoTools::IsReconstructable(fMCTracks, fGemPoints, fSilPoints, fCSCPoints,
            3122,{2212, -211}, fMinHits)) {
        fRunSimInst->SetSaveEvent(kTRUE);
        nValidEvents++;
        if (fVerbose > 1)
            printf("Recon\n");
        if (nValidEvents >= nMaxValidEvents && nMaxValidEvents != 0)
            fRunSimInst->GetMCApp()->StopMCRun();
    } else {
        fRunSimInst->SetSaveEvent(kFALSE);
        if (fVerbose > 1)
            printf("NonRec\n");
    }
//    if (fVerbose > 1)
//        printf("\n%s : exec  SET = %d\n", typeid (*this).name(), fRunSimInst->GetSaveEvent());
    if (fVerbose > 1)
        printf("\nValid events: %lld\n", nValidEvents);
}

void BmnCheckMCTask::Finish() {

}

void BmnCheckMCTask::FinishEvent() {
    if (fVerbose > 2)
        printf("\n%s : finish event\n", typeid (*this).name());
}

ClassImp(BmnCheckMCTask)