#include <BmnGemResiduals.h>
#include <Fit/FitResult.h>

BmnGemResiduals::BmnGemResiduals(Double_t fieldScale) :
isField(kFALSE),
isResid(kTRUE),
fDebug(kFALSE),
fGeometry(BmnGemStripConfiguration::RunSpring2017) {
    if (Abs(fieldScale) > DBL_EPSILON)
        isField = kTRUE;

    // Create GEM detector ------------------------------------------------------
    switch (fGeometry) {
        case BmnGemStripConfiguration::RunWinter2016:
            fDetector = new BmnGemStripStationSet_RunWinter2016(fGeometry);
            cout << "   Current Configuration : RunWinter2016" << "\n";
            break;

        case BmnGemStripConfiguration::RunSpring2017:
            fDetector = new BmnGemStripStationSet_RunSpring2017(fGeometry);
            cout << "   Current Configuration : RunSpring2017" << "\n";
            break;

        default:
            fDetector = NULL;
    }

    fBranchGemHits = "BmnGemStripHit";
    fBranchGemTracks = "BmnGemTrack";
    fBranchResiduals = "BmnResiduals";

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
            for (Int_t iRes = 0; iRes < 2; iRes++)
                hRes[iStat][iMod][iRes] = new TH1F(Form("Stat %d Mod %d", iStat, iMod), Form("Stat %d Mod %d", iStat, iMod), 100, 0., 0.);



}

InitStatus BmnGemResiduals::Init() {
    FairRootManager* ioman = FairRootManager::Instance();

    fGemHits = (TClonesArray*) ioman->GetObject(fBranchGemHits.Data());
    fGemTracks = (TClonesArray*) ioman->GetObject(fBranchGemTracks.Data());

    fGemResiduals = new TClonesArray(fBranchResiduals.Data());

    ioman->Register("BmnResiduals", "RESID", fGemResiduals, kTRUE);
}

void BmnGemResiduals::Exec(Option_t* opt) {
    if (isField)
        return;

    fGemResiduals->Delete();

    if (isResid)
        Residuals();
    else
        Distances();
}

void BmnGemResiduals::Residuals() {
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);

        Double_t tx = track->GetParamFirst()->GetTx();
        Double_t ty = track->GetParamFirst()->GetTy();

        Double_t xFirst = track->GetParamFirst()->GetX();
        Double_t yFirst = track->GetParamFirst()->GetY();
        Double_t zFirst = track->GetParamFirst()->GetZ();

        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->At(track->GetHitIndex(iHit));

            Double_t xRes = hit->GetX() - (xFirst + tx * (hit->GetZ() - zFirst));
            Double_t yRes = hit->GetY() - (yFirst + ty * (hit->GetZ() - zFirst));

            BmnResiduals* resid = new((*fGemResiduals)[fGemResiduals->GetEntriesFast()]) BmnResiduals(hit->GetStation(), hit->GetModule(), xRes, yRes, 0., isField, isResid);
            resid->SetTrackId(iTrack);
            resid->SetHitId(iHit);

            if (fDebug) {
                hRes[hit->GetStation()][hit->GetModule()][0]->Fill(xRes);
                hRes[hit->GetStation()][hit->GetModule()][1]->Fill(yRes);
            }
        }
    }
}

void BmnGemResiduals::Distances() {
    for (Int_t iTrack = 0; iTrack < fGemTracks->GetEntriesFast(); iTrack++) {
        BmnGemTrack* track = (BmnGemTrack*) fGemTracks->UncheckedAt(iTrack);

        for (Int_t iHit = 0; iHit < track->GetNHits(); iHit++) {
            BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->At(track->GetHitIndex(iHit));

            Double_t a = 0., b = 0.;

            LineFit(a, b, track, fGemHits, 1, iHit);
            Double_t xRes = hit->GetX() - (a * hit->GetZ() + b);

            LineFit(a, b, track, fGemHits, 2, iHit);
            Double_t yRes = hit->GetY() - (a * hit->GetZ() + b);

            BmnResiduals* resid = new((*fGemResiduals)[fGemResiduals->GetEntriesFast()]) BmnResiduals(hit->GetStation(), hit->GetModule(), xRes, yRes, 0., isField, isResid);
            resid->SetTrackId(iTrack);
            resid->SetHitId(iHit);

            if (fDebug) {
                hRes[hit->GetStation()][hit->GetModule()][0]->Fill(xRes);
                hRes[hit->GetStation()][hit->GetModule()][1]->Fill(yRes);
            }
        }
    }
}

void BmnGemResiduals::Finish() {
    if (isField)
        return;

    if (fDebug) {
        Double_t misAlign = -LDBL_MAX;
        Double_t resolution = -LDBL_MAX;

        for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
                for (Int_t iRes = 0; iRes < 2; iRes++) {
                    TFitResultPtr fitRes = hRes[iStat][iMod][iRes]->Fit("gaus", "SQww");
                    cout << "Stat = " << iStat << " Mod = " << iMod << " mean = " << fitRes->Parameter(1) << " sigma = " << fitRes->Parameter(2) << endl;
                    if (Abs(fitRes->Parameter(1)) > misAlign)
                        misAlign = Abs(fitRes->Parameter(1));
                    if (fitRes->Parameter(2) > resolution)
                        resolution = fitRes->Parameter(2);
                }


        cout << "misAlign = " << misAlign << " resolut. = " << resolution << endl;
    }

    for (Int_t iStat = 0; iStat < fDetector->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < fDetector->GetGemStation(iStat)->GetNModules(); iMod++)
            for (Int_t iRes = 0; iRes < 2; iRes++)
                delete hRes[iStat][iMod][iRes];

}