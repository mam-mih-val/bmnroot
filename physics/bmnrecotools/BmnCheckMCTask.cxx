#include "BmnCheckMCTask.h"

BmnCheckMCTask::BmnCheckMCTask(
        Long64_t nevents,
        Int_t minHits,
        Int_t code,
        vector<Int_t> outCodes) {
    fMCTracksBranchName = "MCTrack";
    fGemPointsBranchName = "StsPoint";
    fSilPointsBranchName = "SiliconPoint";
    fCSCPointsBranchName = "CSCPoint";
    fPVertexName = "BmnVertex";
    fEvHeaderName = "DstEventHeader.";
    fMCEvHeaderName = "MCEventHeader.";
    fMCTracks = nullptr;
    fGemPoints = nullptr;
    fSilPoints = nullptr;
    fCSCPoints = nullptr;
    fRunSimInst = nullptr;
    fPVertexShow = nullptr;
    fEvHeaderShow = nullptr;
    fMCEvHeader = nullptr;
    fMinHits = minHits;
    fPDGCode = code;
    fPDGOutCodes = outCodes;
    nMaxValidEvents = nevents;
    nValidEvents = 0;
    nVertices = 0;
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
    fEvHeaderShow = (DstEventHeader*) ioman->GetObject(fEvHeaderName);
    //    fMCEvHeader = (FairMCEventHeader*) ioman->GetObject(fMCEvHeaderName);

    return kSUCCESS;
}

void BmnCheckMCTask::Exec(Option_t* option) {
    if (!fMCEvHeader) { // surely it should be in the init (if it would worked with FairRunSim =( )
        FairRootManager* ioman = FairRootManager::Instance();
        fMCEvHeader = (FairMCEventHeader*) ioman->GetObject(fMCEvHeaderName);
    }
    if (fVerbose > 1)
        printf("\nBmnCheckMCTask::Exec eventID = %u  GetSaveEvent() = %d\n",
                fEvHeaderShow->GetEventId(), fRunSimInst->GetSaveEvent());
    if (fMCTracks == nullptr)
        return;
    for (Int_t iTrack = 0; iTrack < fPVertexShow->GetEntriesFast(); iTrack++) {
        CbmVertex* v = (CbmVertex*) fPVertexShow->UncheckedAt(iTrack);
        fVZ = v->GetZ();
        //        if (fVerbose > 1)
        //            printf(" Read PV ( %f %f %f)\n", v->GetX(), v->GetY(), v->GetZ());
    }
    fRunSimInst->SetSaveEvent(kFALSE);
    if (fVZ < CutExistentVertex) {
        return;
    }
    if (Abs(fVZ) > Abs(CutValidZ)) {
        nVertices++;
        return;
    }
    if (BmnRecoTools::IsReconstructable(fMCTracks, fGemPoints, fSilPoints, fCSCPoints,
            fPDGCode, fPDGOutCodes, fMinHits)) {
        fRunSimInst->SetSaveEvent(kTRUE);
        nValidEvents++;
        nVertices++;
        fMCEvHeader->SetEventID(fEvHeaderShow->GetEventId());
        if (fVerbose > 1)
            printf("Recon\n");
        if (nValidEvents >= nMaxValidEvents && nMaxValidEvents != 0)
            fRunSimInst->GetMCApp()->StopMCRun();
    } else {
        if (fVerbose > 1)
            printf("NonRec\n");
    }
    //    if (fVerbose > 1)
    //        printf("\n%s : exec  SET = %d\n", typeid (*this).name(), fRunSimInst->GetSaveEvent());
    if (fVerbose > 1)
        printf("\nValid events: %lld\n", nValidEvents);
}

void BmnCheckMCTask::Finish() {
    printf("\nBmnCheckMCTask: Found: \n\t%lld vertices, \n\t%lld valid events\n",
            nVertices, nValidEvents);

}

void BmnCheckMCTask::FinishEvent() {
    if (fVerbose > 1)
        printf("\n%s : finish event\n", typeid (*this).name());
}

ClassImp(BmnCheckMCTask)
