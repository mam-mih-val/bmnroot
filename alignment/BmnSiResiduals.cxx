#include "BmnSiResiduals.h"

BmnSiResiduals::BmnSiResiduals(Int_t run_period, Int_t run_number, Double_t fieldScale) :
isField(kFALSE),
fDebug(kFALSE),
outRes(NULL),
isPrintToFile(kFALSE),
isMergedDigits(kFALSE) {
    fPeriod = run_period;
    fNumber = run_number;

    if (Abs(fieldScale) > DBL_EPSILON)
        isField = kTRUE;

    TString gPathSiliconConfig = gSystem->Getenv("VMCWORKDIR");
    gPathSiliconConfig += "/parameters/silicon/XMLConfigs/";

    //Create Silicon detector --------------------------------------------------
    TString config = (fPeriod == 6) ? "SiliconRunSpring2017.xml" : (fPeriod == 7) ? "SiliconRunSpring2018.xml" : "";

    fDetector = new BmnSiliconStationSet(gPathSiliconConfig + config);

    fBranchSiHits = "BmnSiliconHit";
    fBranchGlobalTracks = "BmnGlobalTrack";
    fBranchResiduals = "BmnResiduals";
    fBranchFairEventHeader = "EventHeader.";
}

void BmnSiResiduals::Finish() {
    delete fDetector;
}

InitStatus BmnSiResiduals::Init() {
    FairRootManager* ioman = FairRootManager::Instance();

    fSiHits = (TClonesArray*) ioman->GetObject(fBranchSiHits.Data());
    fGlobalTracks = (TClonesArray*) ioman->GetObject(fBranchGlobalTracks.Data());
    fFairEventHeader = (FairEventHeader*) ioman->GetObject(fBranchFairEventHeader.Data());

    fSiResiduals = new TClonesArray(fBranchResiduals.Data());

    ioman->Register("BmnSiResiduals", "RESID", fSiResiduals, kTRUE);
    if (TString(ioman->GetInFile()->GetName()).Contains("merge"))
        isMergedDigits = kTRUE;

}

void BmnSiResiduals::Exec(Option_t* opt) {
   //  fFairEventHeader->SetRunId((isMergedDigits) ? 0 : fNumber);
    fSiResiduals->Delete();
    Residuals();
}

void BmnSiResiduals::Residuals() {
    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobalTracks->GetEntriesFast(); iGlobTrack++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobalTracks->UncheckedAt(iGlobTrack);

        Double_t Tx = glTrack->GetParamFirst()->GetTx();
        Double_t X0 = glTrack->GetParamFirst()->GetX();
        Double_t Ty = glTrack->GetParamFirst()->GetTy();
        Double_t Y0 = glTrack->GetParamFirst()->GetY();

        Double_t Z0 = glTrack->GetParamFirst()->GetZ();

        //        Int_t silIdx = glTrack->GetSilHitIndex();
        //        if (silIdx == -1)
        //            return;
        vector <Int_t> silIndices = glTrack->GetSilHitIndices();
        if (silIndices.size() == 0)
            return;

        for (Int_t iHitIdx = 0; iHitIdx < silIndices.size(); iHitIdx++) {
            Double_t xRes = 0., yRes = 0.;
            BmnSiliconHit* hit = (BmnSiliconHit*) fSiHits->UncheckedAt(silIndices.at(iHitIdx));
            Double_t x = hit->GetX();
            Double_t y = hit->GetY();
            Double_t z = hit->GetZ();

            Int_t stat = hit->GetStation();
            Int_t mod = hit->GetModule();

            if (!isField) {
                xRes = x - (X0 + Tx * (z - Z0));
                yRes = y - (Y0 + Ty * (z - Z0));
            }

            BmnResiduals* resid = new((*fSiResiduals)[fSiResiduals->GetEntriesFast()]) BmnResiduals(stat, mod, xRes, yRes, 0., isField);
            resid->SetTrackId(iGlobTrack);
            resid->SetHitId(silIndices.at(iHitIdx));
        }
    }
}





