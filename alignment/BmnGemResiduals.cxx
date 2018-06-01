#include <BmnGemResiduals.h>
#include <Fit/FitResult.h>
#include <TSystem.h>

BmnGemResiduals::BmnGemResiduals(Int_t run_period, Int_t run_number, Double_t fieldScale) :
isField(kFALSE),
isResid(kTRUE),
fDebug(kFALSE),
outRes(NULL),
isPrintToFile(kFALSE),
isMergedDigits(kFALSE),
fGeometry(BmnGemStripConfiguration::RunSpring2017) {
    //fGeometry(BmnGemStripConfiguration::RunWinter2016) {
    fPeriod = run_period;
    fNumber = run_number;

    if (Abs(fieldScale) > DBL_EPSILON)
        isField = kTRUE;

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/gem/XMLConfigs/";
    TString confGem = (fPeriod == 7) ? "GemRunSpring2018.xml" : (fPeriod == 6) ? "GemRunSpring2017.xml" : "";
    if (confGem == "") {
        printf(ANSI_COLOR_RED "No GEM geometry defined!\n" ANSI_COLOR_RESET);
        throw;
    }
    fDetector = new BmnGemStripStationSet(gPathGemConfig + confGem);

    fBranchGemHits = "BmnGemStripHit";
    fBranchGemTracks = "BmnGemTrack";
    fBranchResiduals = "BmnResiduals";
    fBranchFairEventHeader = "EventHeader.";

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
            for (Int_t iRes = 0; iRes < 2; iRes++)
                hRes[iStat][iMod][iRes] = new TH1F(Form("Stat %d Mod %d Res %d", iStat, iMod, iRes), Form("Stat %d Mod %d Res %d", iStat, iMod, iRes), 100, 0., 0.);
}

InitStatus BmnGemResiduals::Init() {
    FairRootManager* ioman = FairRootManager::Instance();

    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());
    fFairEventHeader = (FairEventHeader*) ioman->GetObject(fBranchFairEventHeader.Data());

    fGemResiduals = new TClonesArray(fBranchResiduals.Data());

    ioman->Register("BmnGemResiduals", "RESID", fGemResiduals, kTRUE);
    if (TString(ioman->GetInFile()->GetName()).Contains("merge"))
        isMergedDigits = kTRUE;
}

void BmnGemResiduals::Exec(Option_t* opt) {
    fFairEventHeader->SetRunId((isMergedDigits) ? 0 : fNumber);
    fGemResiduals->Delete();
    ResidualsAndDistances();
}

void BmnGemResiduals::ResidualsAndDistances() {
    if (isMergedDigits) {
        // To be used for rough alignment (1205 + 1233 in RUN6 as an example)
        // 2 tracks simultaneously separated and satisfying Tx1 * Tx2 < 0
        if (fGemTracks->GetEntriesFast() != 2)
            return;
        Double_t Tx1 = ((BmnGemTrack*) fGemTracks->UncheckedAt(0))->GetParamFirst()->GetTx();
        Double_t Tx2 = ((BmnGemTrack*) fGemTracks->UncheckedAt(1))->GetParamFirst()->GetTx();
        if (Tx1 * Tx2 > 0)
            return;
    }

    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);

        Double_t tx = track->GetParamFirst()->GetTx();
        Double_t ty = track->GetParamFirst()->GetTy();

        Double_t xFirst = track->GetParamFirst()->GetX();
        Double_t yFirst = track->GetParamFirst()->GetY();
        Double_t zFirst = track->GetParamFirst()->GetZ();

        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->At(track->GetHitIndex(iHit));
            Double_t x = hit->GetX();
            Double_t y = hit->GetY();
            Double_t z = hit->GetZ();

            Double_t xRes = 0., yRes = 0.;
            if (!isField) {
                if (isResid) {
                    xRes = x - (xFirst + tx * (z - zFirst));
                    yRes = y - (yFirst + ty * (z - zFirst));
                } else {
                    Double_t a = 0., b = 0.;
                    LineFit(a, b, track, fGemHits, 1, iHit); // bmnbase/BmnMath.h
                    xRes = x - (a * z + b);
                    LineFit(a, b, track, fGemHits, 2, iHit);
                    yRes = y - (a * z + b);
                }
            } else {
                isResid = kFALSE;
                Double_t A = 0., B = 0., C = 0.;
                Pol2Fit(track, fGemHits, A, B, C, iHit); // XZ-plane
                xRes = x - A * z * z - B * z - C;

                Double_t a = 0., b = 0.;
                LineFit(a, b, track, fGemHits, 2, iHit);
                yRes = y - (a * z + b); // YZ-plane               
            }

            BmnResiduals* resid = new((*fGemResiduals)[fGemResiduals->GetEntriesFast()]) BmnResiduals(hit->GetStation(), hit->GetModule(), xRes, yRes, 0., isField, isResid);
            resid->SetTrackId(iTrack);
            resid->SetHitId(track->GetHitIndex(iHit));
            resid->SetIsMergedDigits(isMergedDigits);

            hRes[hit->GetStation()][hit->GetModule()][0]->Fill(xRes);
            hRes[hit->GetStation()][hit->GetModule()][1]->Fill(yRes);
        }
    }
}

void BmnGemResiduals::Finish() {
    if (isPrintToFile) {
        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++) {
                fprintf(outRes, "Stat %d Mod %d", iStat, iMod);
                for (Int_t iRes = 0; iRes < 2; iRes++) {
                    TFitResultPtr fitRes = hRes[iStat][iMod][iRes]->Fit("gaus", "SQww");
                    fprintf(outRes, " misAlign%d %G sigma%d %G ", iRes, fitRes->Parameter(1), iRes, fitRes->Parameter(2));
                }
                fprintf(outRes, "\n");
            }

        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
                for (Int_t iRes = 0; iRes < 2; iRes++)
                    delete hRes[iStat][iMod][iRes];
    }
    if (outRes)
        fclose(outRes);
}
