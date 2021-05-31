#include "BmnDataAnalRun7.h"

BmnDataAnalRun7::BmnDataAnalRun7(TString dir) : BmnEfficiency(),
mAnal(nullptr), fAna(nullptr), fGeomFile(""), fNFiles(0) {

    mAnal = new BmnMassSpectrumAnal(dir);
    fAna = new FairRunAna();
}

void BmnDataAnalRun7::resetHistos() {
    const Int_t nRes = 2;

    for (Int_t iRes = 0; iRes < nRes; iRes++) {
        for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++)
                hResSil[iRes][iStat][iMod]->Reset();

        for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++)
                hResGem[iRes][iStat][iMod]->Reset();
    }
}

void BmnDataAnalRun7::writeHistos(TString out) {
    const Int_t nRes = 2;

    TFile* f = new TFile(out.Data(), "recreate");
    for (Int_t iRes = 0; iRes < nRes; iRes++) {
        for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++) {
                DoNormalization(hResSil[iRes][iStat][iMod]);
                hResSil[iRes][iStat][iMod]->Write();
            }

        for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++)
            for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++) {
                DoNormalization(hResGem[iRes][iStat][iMod]);
                hResGem[iRes][iStat][iMod]->Write();
            }
    }

    delete f;
}

void BmnDataAnalRun7::calcResiduals(TString dst) {
    BmnEfficiency* eff = new BmnEfficiency(fAna, dst);

    // Doing assignment ...
    *(BmnEfficiency*)this = *eff;

    cout << "Processing file# " << dst << endl;

    for (Int_t iEvent = 0; iEvent < ((fNEvents == 0) ? dstChain->GetEntries() : fNEvents); iEvent++) {
        dstChain->GetEntry(iEvent);

        if (iEvent % 1000 == 0)
            cout << "Event# " << iEvent << endl;

        BmnVertex* Vp = (BmnVertex*) fVertices->UncheckedAt(0);
        
        Double_t zVp = Vp->GetZ();
        Int_t nTracks = Vp->GetNTracks();
       
        if (TMath::Abs(zVp) > 6. || nTracks < 2)
            continue;

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

                // Filling histos ...
                TH1F**** h = det.Contains("GEM") ? hResGem : hResSil;

                h[0][stat][mod]->Fill(xOrig - xUpdated); // tr - fit
                h[1][stat][mod]->Fill(yOrig - yUpdated);
            }
        }
    }

    delete eff;
}

void BmnDataAnalRun7::doResidAnal() {
    const Int_t nRes = 2;

    hResGem = new TH1F***[nRes];
    hResSil = new TH1F***[nRes];

    for (Int_t iRes = 0; iRes < nRes; iRes++) {
        TString res = (iRes == 0) ? TString::Format("X").Data() : TString::Format("Y").Data();

        // SILICON
        hResSil[iRes] = new TH1F**[silicon->GetNStations()];
        for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++) {
            hResSil[iRes][iStat] = new TH1F*[silicon->GetStation(iStat)->GetNModules()];

            for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++)
                hResSil[iRes][iStat][iMod] = new TH1F(Form("SILICON, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod),
                    Form("SILICON, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod), 200, -1., +1.);
        }

        // GEM
        hResGem[iRes] = new TH1F**[gem->GetNStations()];
        for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++) {
            hResGem[iRes][iStat] = new TH1F*[gem->GetStation(iStat)->GetNModules()];

            for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++)
                hResGem[iRes][iStat][iMod] = new TH1F(Form("GEM, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod),
                    Form("GEM, Res %s, Stat %d, Mod %d", res.Data(), iStat, iMod), 200, -1., +1.);
        }
    }

    // Setting inn. tracker geometry ...
    if (fGeomFile.IsNull())
        Fatal("BmnDataAnalRun7::doResidAnal()", "No geometry file passed!!!");
    else
        TGeoManager::Import(fGeomFile.Data());

    vector <TString> fList;

    // Processing monte Carlo if needed and passed ...
    if (!fMcDataPath.IsNull()) {
        resetHistos(); // Not needed but for sure since it is fast:)
        BmnMassSpectrumAnal* mcAnal = new BmnMassSpectrumAnal(fMcDataPath);

        fList = mcAnal->GetFileList();

        for (auto itFile = fList.begin(); itFile != fList.end(); itFile++) {
            if (fNFiles && distance(fList.begin(), itFile) == fNFiles)
                break;

            calcResiduals(*itFile);
        }

        writeHistos("residMc.root");

        delete mcAnal;
    }

    // Setting necessary target / targets ...
    mAnal->SetTarget(fTarget);

    fList = mAnal->createFilelist();
    if (!fNFiles)
        fNFiles = fList.size();

    // Processing data ...
    resetHistos();
    for (auto itFile = fList.begin(); itFile != fList.end(); itFile++) {
        if (fNFiles && distance(fList.begin(), itFile) == fNFiles)
            break;

        calcResiduals(*itFile);
    }

    // Writing histos ...
    writeHistos(TString::Format("residData_%s.root", fTarget.Data()));
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

