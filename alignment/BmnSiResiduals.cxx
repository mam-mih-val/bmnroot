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
    gPathSiliconConfig += "/silicon/XMLConfigs/";

    //Create Silicon detector --------------------------------------------------
    fDetector = new BmnSiliconStationSet(gPathSiliconConfig + "SiliconRunSpring2017.xml");
    cout << "   Current Configuration : RunSpring2017" << "\n";

    fBranchSiHits = "BmnSiliconHit";
    fBranchGlobalTracks = "BmnGlobalTrack";
    fBranchResiduals = "BmnResiduals";
    fBranchFairEventHeader = "EventHeader.";

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetSiliconStation(iStat)->GetNModules(); iMod++)
            for (Int_t iRes = 0; iRes < 2; iRes++) {
                hRes[iStat][iMod][iRes] = new TH1F(Form("Stat %d Mod %d Res %d", iStat, iMod, iRes), Form("Stat %d Mod %d Res %d", iStat, iMod, iRes), 100, 0., 0.);
                //cout << iStat << " " << iMod << " " << iRes << endl;
            }
}

void BmnSiResiduals::Finish() {
    delete fDetector;

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetSiliconStation(iStat)->GetNModules(); iMod++)
            for (Int_t iRes = 0; iRes < 2; iRes++)
                delete hRes[iStat][iMod][iRes];
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
    fFairEventHeader->SetRunId((isMergedDigits) ? 0 : fNumber);
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

        Int_t silIdx = glTrack->GetSilHitIndex();
        if (silIdx == -1)
            return;

        BmnSiliconHit* hit = (BmnSiliconHit*) fSiHits->UncheckedAt(silIdx);
        Double_t x = hit->GetX();
        Double_t y = hit->GetY();
        Double_t z = hit->GetZ();

        Int_t stat = hit->GetStation();
        Int_t mod = hit->GetModule();

        cout << silIdx << " " << x << " " << y << " " << z << endl;
        //getchar(); // FIXME

        Double_t xRes = 0., yRes = 0.;
        if (!isField) {
            xRes = x - (X0 + Tx * (z - Z0));
            yRes = y - (Y0 + Ty * (z - Z0));
        } 
        
        else {

        }

        BmnResiduals* resid = new((*fSiResiduals)[fSiResiduals->GetEntriesFast()]) BmnResiduals(hit->GetStation(), hit->GetModule(), xRes, yRes, 0., isField);
        resid->SetTrackId(iGlobTrack);
        resid->SetHitId(silIdx);

        hRes[stat][mod][0]->Fill(xRes);
        hRes[stat][mod][1]->Fill(yRes);
    }


    //    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
    //        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);
    //
    //        Double_t tx = track->GetParamFirst()->GetTx();
    //        Double_t ty = track->GetParamFirst()->GetTy();
    //
    //        Double_t xFirst = track->GetParamFirst()->GetX();
    //        Double_t yFirst = track->GetParamFirst()->GetY();
    //        Double_t zFirst = track->GetParamFirst()->GetZ();
    //
    //        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
    //            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->At(track->GetHitIndex(iHit));
    //            Double_t x = hit->GetX();
    //            Double_t y = hit->GetY();
    //            Double_t z = hit->GetZ();
    //
    //            Double_t xRes = 0., yRes = 0.;
    //            if (!isField) {
    //                if (isResid) {
    //                    xRes = x - (xFirst + tx * (z - zFirst));
    //                    yRes = y - (yFirst + ty * (z - zFirst));
    //                } else {
    //                    Double_t a = 0., b = 0.;
    //                    LineFit(a, b, track, fGemHits, 1, iHit); // bmnbase/BmnMath.h
    //                    xRes = x - (a * z + b);
    //                    LineFit(a, b, track, fGemHits, 2, iHit);
    //                    yRes = y - (a * z + b);
    //                }
    //            } else {
    //                isResid = kFALSE;
    //                Double_t A = 0., B = 0., C = 0.;
    //                Pol2Fit(track, fGemHits, A, B, C, iHit); // XZ-plane
    //                xRes = x - A * z * z - B * z - C;
    //
    //                Double_t a = 0., b = 0.;
    //                LineFit(a, b, track, fGemHits, 2, iHit);
    //                yRes = y - (a * z + b); // YZ-plane               
    //            }

    //           BmnResiduals* resid = new((*fSiResiduals)[fSiResiduals->GetEntriesFast()]) BmnResiduals(hit->GetStation(), hit->GetModule(), xRes, yRes, 0., isField, isResid);
    //            resid->SetTrackId(iTrack);
    //            resid->SetHitId(track->GetHitIndex(iHit));
    //            resid->SetIsMergedDigits(isMergedDigits);
    //
    //            hRes[hit->GetStation()][hit->GetModule()][0]->Fill(xRes);
    //            hRes[hit->GetStation()][hit->GetModule()][1]->Fill(yRes);
    //        }
    //    }

}





