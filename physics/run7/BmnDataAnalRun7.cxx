#include "BmnDataAnalRun7.h"

BmnDataAnalRun7::BmnDataAnalRun7(TString dir) : BmnEfficiency(),
mAnal(nullptr), fAna(nullptr), fGeomFile(""), fNFiles(0) {

    mAnal = new BmnMassSpectrumAnal(dir);
    fAna = new FairRunAna();
}

void BmnDataAnalRun7::doResidAnal() {

    // Setting inn. tracker geometry ...
    if (fGeomFile.IsNull())
        Fatal("BmnDataAnalRun7::doResidAnal()", "No geometry file passed!!!");
    else
        TGeoManager::Import(fGeomFile.Data());

    TClonesArray* rGems = new TClonesArray("Residuals");
    TClonesArray* rSils = new TClonesArray("Residuals");

    // Preparing containers to store residuals ...
    for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++)
            new ((*rGems)[rGems->GetEntriesFast()]) Residuals("GEM", iStat, iMod);

    for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++)
            new ((*rSils)[rSils->GetEntriesFast()]) Residuals("SILICON", iStat, iMod);

    // Setting necessary target / targets ...
    mAnal->SetTarget(fTarget);

    vector <TString> fList = mAnal->createFilelist();
    if (!fNFiles)
        fNFiles = fList.size();

    for (auto itFile = fList.begin(); itFile != fList.end(); itFile++) {
        if (fNFiles && distance(fList.begin(), itFile) == fNFiles)
            break;

        BmnEfficiency* eff = new BmnEfficiency(fAna, *itFile);

        // Doing assignment ...
        *(BmnEfficiency*)this = *eff;

        for (Int_t iEvent = 0; iEvent < ((fNEvents == 0) ? dstChain->GetEntries() : fNEvents); iEvent++) {
            dstChain->GetEntry(iEvent);

            if (iEvent % 1000 == 0)
                cout << "Event# " << iEvent << endl;

            BmnVertex* Vp = (BmnVertex*) fVertices->UncheckedAt(0);

            // Loop over glob. tracks ...
            for (Int_t iTrack = 0; iTrack < fGlobTracks->GetEntriesFast(); iTrack++) {
                BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iTrack);

                if (track->GetNHits() < fNHits)
                    continue;

                // cout << "Track#" << endl;

                vector <BmnHit> hits;

                // Loop over silicon hits ...
                if (track->GetSilTrackIndex() != -1) {
                    BmnTrack* silTrack = (BmnTrack*) fSiliconTracks->UncheckedAt(track->GetSilTrackIndex());

                    for (Int_t iHit = 0; iHit < silTrack->GetNHits(); iHit++) {
                        BmnHit* hit = (BmnHit*) fSiliconHits->UncheckedAt(silTrack->GetHitIndex(iHit));
                        hits.push_back(*hit);
                    }
                }

                // Loop over gem hits ...
                if (track->GetGemTrackIndex() != -1) {
                    BmnTrack* gemTrack = (BmnTrack*) fGemTracks->UncheckedAt(track->GetGemTrackIndex());

                    for (Int_t iHit = 0; iHit < gemTrack->GetNHits(); iHit++) {
                        BmnHit* hit = (BmnHit*) fGemHits->UncheckedAt(gemTrack->GetHitIndex(iHit));
                        hits.push_back(*hit);
                    }
                }

                // Loop over collected track hits ...
                for (auto it = hits.begin(); it != hits.end(); it++) {
                    BmnHit hit = *it;

                    Int_t stat = hit.GetStation();
                    Int_t mod = hit.GetModule();

                    Double_t Z = hit.GetZ();
                    TString det = (Z > 30.) ? "GEM" : "SILICON";

                    Double_t xOrig, yOrig = 0.;
                    doKalman(track, Z, xOrig, yOrig);

                    // Searching for necessary resid. container to be filled ...
                    TClonesArray* arr = (det.Contains("SIL")) ? rSils : rGems;

                    Residuals* resid = nullptr;

                    for (Int_t iRes = 0; iRes < arr->GetEntriesFast(); iRes++) {
                        resid = (Residuals*) arr->UncheckedAt(iRes);

                        if (resid->Detector() == det && stat == resid->Station() && mod == resid->Module())
                            break;
                    }

                    if (!resid)
                        Fatal("BmnDataAnalRun7::doResidAnal()", "Resid. container not found !!!");

                    // Excluding current hit from the track connected hits ...
                    vector <BmnHit> hits0;

                    for (auto it1 = hits.begin(); it1 != hits.end(); it1++) {
                        if (it == it1)
                            continue;

                        hits0.push_back(*it1);
                    }

                    // Trying to update track. params and get resid. with the hit excluded at Z position of the hit excluded ...
                    Double_t xUpdated, yUpdated = 0.;
                    doKalman(track, hits0, Z, xUpdated, yUpdated);

                    resid->SetXY(xOrig - xUpdated, yOrig - yUpdated); // tr - fit
                }
            }
        }
       
        delete eff;
    }

    const Int_t nRes = 2;

    TFile* f = new TFile("testResid.root", "recreate");
    TH1F**** hResGem = new TH1F***[nRes];
    TH1F**** hResSil = new TH1F***[nRes];

    for (Int_t iRes = 0; iRes < nRes; iRes++) {        
        TString res = (iRes == 0) ? TString::Format("X").Data() : TString::Format("Y").Data();
        
        // SILICON
        hResSil[iRes] = new TH1F**[silicon->GetNStations()];
        for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++) {
            hResSil[iRes][iStat] = new TH1F*[silicon->GetStation(iStat)->GetNModules()];

            for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++) {
                hResSil[iRes][iStat][iMod] = new TH1F(Form("SILICON, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod),
                        Form("SILICON, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod), 200, -1., +1.);

            }
        }

        // GEM
        hResGem[iRes] = new TH1F**[gem->GetNStations()];
        for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++) {
            hResGem[iRes][iStat] = new TH1F*[gem->GetStation(iStat)->GetNModules()];

            for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++) {
                hResGem[iRes][iStat][iMod] = new TH1F(Form("GEM, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod),
                        Form("GEM, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod), 200, -1., +1.);

            }
        }
    }

    // Filling histos ...
    for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++) {

            for (Int_t iRes = 0; iRes < rGems->GetEntriesFast(); iRes++) {
                Residuals* resid = (Residuals*) rGems->UncheckedAt(iRes);

                if (resid->Detector() != "GEM" || iStat != resid->Station() || iMod != resid->Module())
                    continue;
                
                for (auto x : resid->GetX())
                    hResGem[0][iStat][iMod]->Fill(x);
                
                for (auto y : resid->GetY())
                    hResGem[1][iStat][iMod]->Fill(y);
                
                // Doing normalisation ...
                DoNormalization(hResGem[0][iStat][iMod]);
                DoNormalization(hResGem[1][iStat][iMod]);
                                
                break;
            }
        }
    
     for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++) {

            for (Int_t iRes = 0; iRes < rSils->GetEntriesFast(); iRes++) {
                Residuals* resid = (Residuals*) rSils->UncheckedAt(iRes);

                if (resid->Detector() != "SILICON" || iStat != resid->Station() || iMod != resid->Module())
                    continue;
                
                for (auto x : resid->GetX())
                    hResSil[0][iStat][iMod]->Fill(x);
                
                for (auto y : resid->GetY())
                    hResSil[1][iStat][iMod]->Fill(y);
                
                // Doing normalisation ...
                DoNormalization(hResSil[0][iStat][iMod]);
                DoNormalization(hResSil[1][iStat][iMod]);
                                
                break;
            }
        }
    

    // Writing histos ...
    for (Int_t iRes = 0; iRes < nRes; iRes++) {

        for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++)
                hResSil[iRes][iStat][iMod]->Write();

        for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++)
                hResGem[iRes][iStat][iMod]->Write();
    }

    delete f;
}

void BmnDataAnalRun7::doKalman(BmnGlobalTrack* track, Double_t Z, Double_t& xOrig, Double_t& yOrig) {
    FairTrackParam* first = track->GetParamFirst();
    FairTrackParam* last = track->GetParamLast();

    FairTrackParam par = *first;
    BmnStatus status = fKalman->TGeoTrackPropagate(&par, Z, last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
    if (status == kBMNSUCCESS) {
        xOrig = par.GetX();
        yOrig = par.GetY();
    }
}

void BmnDataAnalRun7::doKalman(BmnGlobalTrack* track, vector <BmnHit> hits, Double_t Z, Double_t& xUpdated, Double_t& yUpdated) {

    FairTrackParam* first = track->GetParamFirst();
    FairTrackParam* last = track->GetParamLast();

    FairTrackParam par = *first;

    // Updating track params ...
    for (BmnHit hit : hits) {
        BmnStatus status = fKalman->TGeoTrackPropagate(&par, hit.GetZ(), last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
        if (status == kBMNERROR)
            continue;

        Double_t chi2 = 0.;
        fKalman->Update(&par, &hit, chi2);
    }

    // Prediction at Z of the excluded hit ...
    if (fKalman->TGeoTrackPropagate(&par, Z, last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE) == kBMNSUCCESS) {
        xUpdated = par.GetX();
        yUpdated = par.GetY();
    }
}

void BmnDataAnalRun7::DoNormalization(TH1F* h) {
    if (h->GetEntries() == 0)
        return;

    // Collecting all bin contents ...
    Double_t contentAll = 0.;

    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++)
        contentAll += h->GetBinContent(iBin);

    // Normalizing histo ...
    for (Int_t iBin = 1; iBin < h->GetNbinsX() + 1; iBin++) {
        h->SetBinContent(iBin, h->GetBinContent(iBin) / contentAll);
        h->SetBinError(iBin, 0.);
    }

    h->GetYaxis()->SetRangeUser(0., 1.1 * h->GetMaximum());
}

