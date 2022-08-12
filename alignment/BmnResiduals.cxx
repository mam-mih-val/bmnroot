#include "BmnResiduals.h"
#include "BmnKalmanFilter.h"

#include "FairLogger.h"

BmnResiduals::BmnResiduals(Int_t period, Int_t number) : fGlobalTracks(nullptr),
                                                                           fGemTracks(nullptr),
                                                                           fSilTracks(nullptr),
                                                                           fGemHits(nullptr),
                                                                           fSilHits(nullptr) {
    fPeriod = period;
    fNumber = number;

    fBranchGemHits = "BmnGemStripHit";
    fBranchSilHits = "BmnSiliconHit";

    fBranchGemTracks = "BmnGemTrack";
    fBranchSilTracks = "BmnSiliconTrack";
    fBranchGlobTracks = "BmnGlobalTrack";

    fBranchFairEventHeader = "DstEventHeader.";
}

InitStatus BmnResiduals::Init() {
    FairRootManager* ioman = FairRootManager::Instance();
    fFairEventHeader = (FairEventHeader*)ioman->GetObject(fBranchFairEventHeader.Data());

    fGemTracks = (TClonesArray*)ioman->GetObject(fBranchGemTracks.Data());
    fSilTracks = (TClonesArray*)ioman->GetObject(fBranchSilTracks.Data());
    fGlobalTracks = (TClonesArray*)ioman->GetObject(fBranchGlobTracks.Data());

    fGemHits = (TClonesArray*)ioman->GetObject(fBranchGemHits.Data());
    fSilHits = (TClonesArray*)ioman->GetObject(fBranchSilHits.Data());

    if (fFairEventHeader != nullptr)
        fFairEventHeader->SetRunId(fNumber);
    else
        LOG(debug)<<__PRETTY_FUNCTION__<<": fFairEventHeader is not initalized. fBranchFairEventHeader == "<<fBranchFairEventHeader;

    return kSUCCESS;
}

void BmnResiduals::Exec(Option_t* opt) {
    if (!fGlobalTracks || !IsActive())
        return;

    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); iTrack++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)fGlobalTracks->UncheckedAt(iTrack);

        vector<BmnHit*> allHits;

        BmnTrack* gemTr = nullptr;
        BmnTrack* silTr = nullptr;

        if (glTrack->GetGemTrackIndex() != -1)
            gemTr = (BmnTrack*)fGemTracks->UncheckedAt(glTrack->GetGemTrackIndex());

        if (glTrack->GetSilTrackIndex() != -1)
            silTr = (BmnTrack*)fSilTracks->UncheckedAt(glTrack->GetSilTrackIndex());

        // GEM track
        if (glTrack->GetGemTrackIndex() != -1)
            for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                BmnGemStripHit* hit = (BmnGemStripHit*)fGemHits->UncheckedAt(gemTr->GetHitIndex(iHit));
                allHits.push_back(hit);
            }

        // SILICON track
        if (glTrack->GetSilTrackIndex() != -1)
            for (Int_t iHit = 0; iHit < silTr->GetNHits(); iHit++) {
                BmnSiliconHit* hit = (BmnSiliconHit*)fSilHits->UncheckedAt(silTr->GetHitIndex(iHit));
                allHits.push_back(hit);
            }

        Double_t xRes = 0., yRes = 0.;
        Double_t a = 0., b = 0.;

        // for (Int_t iHit = 0; iHit < allHits.size(); iHit++) {
        //     Double_t x = allHits[iHit]->GetX();
        //     Double_t y = allHits[iHit]->GetY();
        //     Double_t z = allHits[iHit]->GetZ();

        //     // YZ-plane, linear fit
        //     a = LineFit(allHits, iHit, "ZY").X();
        //     b = LineFit(allHits, iHit, "ZY").Y();
        //     yRes = y - (a * z + b);

        //     // XZ-plane
        //     if (fIsField) {
        //         Double_t A = Pol2Fit(allHits, iHit).X();
        //         Double_t B = Pol2Fit(allHits, iHit).Y();
        //         Double_t C = Pol2Fit(allHits, iHit).Z();
        //         xRes = x - (A * z * z + B * z + C);
        //     } else {
        //         a = LineFit(allHits, iHit, "ZX").X();
        //         b = LineFit(allHits, iHit, "ZX").Y();
        //         xRes = x - (a * z + b);
        //     }
        //     allHits[iHit]->SetResXY(xRes, yRes);
        // }

        BmnKalmanFilter* fKalman = new BmnKalmanFilter();
        FairTrackParam par = *(glTrack->GetParamFirst());

        for (Int_t iHit = 0; iHit < allHits.size(); iHit++) {
            Double_t x = allHits[iHit]->GetX();
            Double_t y = allHits[iHit]->GetY();
            Double_t z = allHits[iHit]->GetZ();
            fKalman->TGeoTrackPropagate(&par, z, 2212, nullptr, nullptr);
            xRes = x - par.GetX();
            yRes = y - par.GetY();
            allHits[iHit]->SetResXY(xRes, yRes);
        }
        delete fKalman;
    }
}

void BmnResiduals::Finish() {
}
