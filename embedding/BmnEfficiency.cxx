#include "BmnEfficiency.h"
#include "BmnSilicon.h"

BmnEfficiency::BmnEfficiency(TString dst) :
gem(nullptr),
silicon(nullptr),
effGem(nullptr),
effSilicon(nullptr),
fHeader(nullptr),
fInnerHits(nullptr),
fGemHits(nullptr),
fSiliconHits(nullptr),
fGlobTracks(nullptr),
fGemTracks(nullptr),
fSiliconTracks(nullptr),
fVertices(nullptr),
dstChain(nullptr),
fKalman(nullptr),
isTrackFromVp(kFALSE),
fNHits(7), fNHitsSilicon(2), fNHitsGem(5) {

    // Open dst file ...
    dstChain = new TChain("bmndata");
    dstChain->Add(dst.Data());

    dstChain->SetBranchAddress("DstEventHeader.", &fHeader);
    dstChain->SetBranchAddress("BmnInnerHits", &fInnerHits);
    dstChain->SetBranchAddress("BmnGemStripHit", &fGemHits);
    dstChain->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
    dstChain->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);
    dstChain->SetBranchAddress("BmnGemTrack", &fGemTracks);
    dstChain->SetBranchAddress("BmnSiliconTrack", &fSiliconTracks);
    dstChain->SetBranchAddress("BmnVertex", &fVertices);

    dstChain->GetEntry();

    BmnInnerTrackerGeometryDraw* innTracker = new BmnInnerTrackerGeometryDraw();

    Char_t* geoFileName = (Char_t*) "current_geo_file.root";
    Int_t res_code = UniDbRun::ReadGeometryFile(7, fHeader->GetRunId(), geoFileName);
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-1);
    }
    TGeoManager::Import(geoFileName);

    UniDbRun* runInfo = UniDbRun::GetRun(7, fHeader->GetRunId());
    if (!runInfo)
        return;

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    fMagField->SetScale(*runInfo->GetFieldVoltage() / 55.87);
    fMagField->Init();

    FairRunAna* fAna = new FairRunAna();
    fAna->SetField(fMagField);
    fField = fAna->GetField();

    fKalman = new BmnKalmanFilter();

    gem = innTracker->GetGemGeometry();
    silicon = innTracker->GetSiliconGeometry();

    // Creating eff. stores for all elements of the Central Tracker ...
    const Int_t nZones = 2;
    TString zones[nZones] = {"hot", "big"};

    // GEM ...
    effGem = new TClonesArray("EffStore");

    for (Int_t iStat = 0; iStat < gem->GetNStations(); iStat++) {

        fStatZ[iStat + silicon->GetNStations()] = gem->GetStation(iStat)->GetZPosition();
        Double_t xMax = -gem->GetStation(iStat)->GetXMinStation();
        Double_t xMin = -gem->GetStation(iStat)->GetXMaxStation();
        Double_t yMin = gem->GetStation(iStat)->GetYMinStation();
        Double_t yMax = gem->GetStation(iStat)->GetYMaxStation();

        vector <Double_t> borders{xMin, xMax, yMin, yMax};
        fStatAcceptance[iStat + silicon->GetNStations()] = borders;

        for (Int_t iMod = 0; iMod < gem->GetStation(iStat)->GetNModules(); iMod++)
            for (Int_t iZone = 0; iZone < nZones; iZone++)
                new ((*effGem)[effGem->GetEntriesFast()]) EffStore("GEM", iStat, iMod, zones[iZone]);
    }

    // SILICON ...
    effSilicon = new TClonesArray("EffStore");

    for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++) {
        Double_t delta = 0.;
        Int_t nMods = silicon->GetStation(iStat)->GetNModules();
        Double_t z = silicon->GetStation(iStat)->GetZPosition();

        for (Int_t iMod = 0; iMod < nMods; iMod++) {
            delta += (silicon->GetStation(iStat)->GetModule(iMod)->GetZPositionRegistered() - z);
            new ((*effSilicon)[effSilicon->GetEntriesFast()]) EffStore("SILICON", iStat, iMod);
        }

        fStatZ[iStat] = z + (delta / nMods);
        Double_t xMax = -silicon->GetStation(iStat)->GetXMinStation();
        Double_t xMin = -silicon->GetStation(iStat)->GetXMaxStation();
        Double_t yMin = silicon->GetStation(iStat)->GetYMinStation();
        Double_t yMax = silicon->GetStation(iStat)->GetYMaxStation();

        vector <Double_t> borders{xMin, xMax, yMin, yMax};
        fStatAcceptance[iStat] = borders;
    }
}

void BmnEfficiency::Efficiency() {

    // Some char. numbers to be used below ...
    Int_t nHitsMax = gem->GetNStations() + silicon->GetNStations();
    Double_t zBegin = silicon->GetStation(0)->GetZPosition() - 5.; // first SILICON - 5 cm
    Double_t zEnd = gem->GetStation(5)->GetZPosition() + 5.; // last GEM + 5 cm

    Double_t borderSilGem = .5 * (gem->GetStation(0)->GetZPosition() + silicon->GetStation(2)->GetZPosition());

    gStyle->SetOptStat(0);
    TH1F* hDistVp = new TH1F("dist. at Vp", "dist. at Vp; distXY at VpZ [cm]", 100, 0., 5.);

    for (Int_t iEvent = 0; iEvent < dstChain->GetEntries(); iEvent++) {
        dstChain->GetEntry(iEvent);

        if (iEvent % 100 == 0)
            cout << "Event# " << iEvent << endl;

        // Gathering all inner hits in one place ...
        vector <BmnHit*> innerHits;
        for (Int_t iHit = 0; iHit < fInnerHits->GetEntriesFast(); iHit++)
            innerHits.push_back((BmnHit*) fInnerHits->UncheckedAt(iHit));

        // Loop over glob. tracks ...
        for (Int_t iTrack = 0; iTrack < fGlobTracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iTrack);
            Bool_t isTrackGood = kTRUE;

            vector <BmnHit*> hits;

            // Loop over silicon hits ...
            if (track->GetSilTrackIndex() != -1) {
                BmnTrack* silTrack = (BmnTrack*) fSiliconTracks->UncheckedAt(track->GetSilTrackIndex());

                for (Int_t iHit = 0; iHit < silTrack->GetNHits(); iHit++)
                    hits.push_back((BmnHit*) fSiliconHits->UncheckedAt(silTrack->GetHitIndex(iHit)));
            }

            // Loop over gem hits ...
            BmnTrack* gemTrack = (BmnTrack*) fGemTracks->UncheckedAt(track->GetGemTrackIndex());

            for (Int_t iHit = 0; iHit < gemTrack->GetNHits(); iHit++)
                hits.push_back((BmnHit*) fGemHits->UncheckedAt(gemTrack->GetHitIndex(iHit)));

            Int_t nHits = 0;
            // Let us calculate number of hits in [zBegin, zLast] ...

            for (BmnHit* hit : hits) {
                Double_t Z = hit->GetZ();
                if (Z > zBegin && Z < zEnd)
                    nHits++;
            }

            if (nHits < fNHits)
                continue;

            // Also, a track should satisfy the condition: SilHits >= 2 && GemHits >= 5
            Int_t nHitsPerSilicon = 0;
            Int_t nHitsPerGem = 0;

            for (BmnHit* hit : hits) {
                if (hit->GetZ() < borderSilGem)
                    nHitsPerSilicon++;
                else
                    nHitsPerGem++;
            }

            if (nHitsPerSilicon < fNHitsSilicon || nHitsPerGem < fNHitsGem)
                continue;

            if (isTrackFromVp) {
                CbmVertex* Vp = (CbmVertex*) fVertices->UncheckedAt(0); // Getting Vp ...

                Double_t x = Vp->GetX();
                Double_t y = Vp->GetY();
                Double_t z = Vp->GetZ();

                // Skipping events with no Vp reconstructed or being reconstructed outside the range of Z = (-3. ... +3. cm) ...
                if (z < -3. || z > +3.)
                    continue;

                // Updating track params. for all hits connected to the track ...
                FairTrackParam* first = track->GetParamFirst();
                FairTrackParam* last = track->GetParamLast();

                FairTrackParam par = *first;
                for (BmnHit* hit : hits) {
                    BmnStatus status = fKalman->TGeoTrackPropagate(&par, hit->GetZ(), last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
                    if (status == kBMNERROR)
                        continue;

                    Double_t chi2 = 0.;
                    fKalman->Update(&par, hit, chi2);
                }

                // Going to the VpZ with the updated params ...
                BmnStatus status = fKalman->TGeoTrackPropagate(&par, z, last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
                if (status == kBMNERROR)
                    continue;

                Double_t xPredictedAtVp = par.GetX();
                Double_t yPredictedAtVp = par.GetY();

                const Double_t distCut = 0.3;
                Double_t distXY = TMath::Sqrt((xPredictedAtVp - x) * (xPredictedAtVp - x) + (yPredictedAtVp - y) * (yPredictedAtVp - y));

                hDistVp->Fill(distXY);

                if (distXY > distCut)
                    continue;
            }

            // Let us check that the zone of GEM plane is defined correctly ...
            for (BmnHit* hit : hits) {
                TString det = GetDetector(hit);
                TString zone = "";

                if (det.Contains("GEM")) {
                    zone = GetGemZone(hit);

                    // Zone should not be undefined, so if yes then skipping such tracks ...
                    if (zone.IsNull())
                        isTrackGood = kFALSE;
                }

                if (!isTrackGood)
                    break;
            }

            if (!isTrackGood)
                continue;
            FairTrackParam* first = track->GetParamFirst();
            FairTrackParam* last = track->GetParamLast();

            FairTrackParam parPredicted = *first;

            // Loop over all hits per track ...
            // Array to store info on hits per SILICON and GEM part of tracker
            Bool_t hitsPerStation[nHitsMax];
            for (Int_t iHit = 0; iHit < nHitsMax; iHit++)
                hitsPerStation[iHit] = kFALSE;

            // Hits that were found on the track
            for (BmnHit* hit : hits) {
                TString det = GetDetector(hit);
                Int_t shift = (det.Contains("GEM")) ? silicon->GetNStations() : 0;

                Int_t stat = hit->GetStation();
                hitsPerStation[stat + shift] = kTRUE;

                Int_t mod = hit->GetModule();

                TString zone = GetGemZone(hit);

                Bool_t isContainerFound = kFALSE;

                for (Int_t iCont = 0; iCont < effGem->GetEntriesFast(); iCont++) {

                    EffStore* effCont = (EffStore*) effGem->UncheckedAt(iCont);

                    if (effCont->Detector() != det || effCont->Station() != stat || effCont->Module() != mod || effCont->Zone() != zone)
                        continue;

                    // Setting GEM hit coordinates ...
                    effCont->AddHitCoordinates(hit);

                    effCont->IncreaseNominatorByUnity();
                    effCont->IncreaseDenominatorByUnity();

                    isContainerFound = kTRUE;

                    break;
                }

                if (!isContainerFound)
                    for (Int_t iCont = 0; iCont < effSilicon->GetEntriesFast(); iCont++) {

                        EffStore* effCont = (EffStore*) effSilicon->UncheckedAt(iCont);

                        if (effCont->Detector() != det || effCont->Station() != stat || effCont->Module() != mod || effCont->Zone() != zone)
                            continue;

                        // Setting SILICON hit coordinates ...
                        effCont->AddHitCoordinates(hit);

                        effCont->IncreaseNominatorByUnity();
                        effCont->IncreaseDenominatorByUnity();

                        break;
                    }
            }

            // Let us see what is missing ...
            for (Int_t iHit = 0; iHit < nHitsMax; iHit++) {

                if (hitsPerStation[iHit])
                    continue;

                // cout << iHit << endl;

                Double_t Z = FindZ(iHit);

                vector <BmnHit*> hitsBeforeCurrentZ;

                for (BmnHit* hit : hits) {
                    Double_t zHit = hit->GetZ();

                    if (zHit > Z)
                        continue;

                    hitsBeforeCurrentZ.push_back(hit);
                }

                for (BmnHit* hit : hitsBeforeCurrentZ) {
                    BmnStatus status = fKalman->TGeoTrackPropagate(&parPredicted, hit->GetZ(), last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
                    if (status == kBMNERROR)
                        continue;

                    Double_t chi2 = 0.;
                    fKalman->Update(&parPredicted, hit, chi2);
                }

                BmnStatus status = fKalman->TGeoTrackPropagate(&parPredicted, Z, last->GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
                if (status == kBMNERROR)
                    continue;

                if (isInAcceptance(iHit, parPredicted.GetX(), parPredicted.GetY())) {

                    // Creating a hyp. missing hit ...
                    BmnHit* missingHit = new BmnHit();
                    missingHit->SetXYZ(parPredicted.GetX(), parPredicted.GetY(), Z);

                    // SILICON or GEM
                    if (iHit < silicon->GetNStations()) {
                        missingHit->SetStation(iHit);
                        missingHit->SetModule(GetSiliconStatModule(missingHit));

                        Int_t stat = missingHit->GetStation();
                        Int_t mod = missingHit->GetModule();

                        for (Int_t iCont = 0; iCont < effSilicon->GetEntriesFast(); iCont++) {

                            EffStore* effCont = (EffStore*) effSilicon->UncheckedAt(iCont);

                            if (effCont->Station() != stat || effCont->Module() != mod || !effCont->Zone().IsNull())
                                continue;
                            
                            // Setting SILICON hit coordinates ...
                            effCont->AddHitCoordinates(missingHit, 0.);

                            effCont->IncreaseDenominatorByUnity();

                            break;
                        }
                    } else {
                        missingHit->SetStation(iHit - silicon->GetNStations());
                        missingHit->SetModule(GetGemStatModule(missingHit));

                        Int_t stat = missingHit->GetStation();
                        Int_t mod = missingHit->GetModule();
                        TString zone = GetGemZone(missingHit);

                        // Looking for appropriate container ...
                        for (Int_t iCont = 0; iCont < effGem->GetEntriesFast(); iCont++) {

                            EffStore* effCont = (EffStore*) effGem->UncheckedAt(iCont);

                            if (effCont->Station() != stat || effCont->Module() != mod || effCont->Zone() != zone)
                                continue;

                            // Setting GEM hit coordinates ...
                            effCont->AddHitCoordinates(missingHit, 0.);

                            effCont->IncreaseDenominatorByUnity();

                            break;
                        }
                    }
                    delete missingHit;
                }
            }
            // getchar();
        }
    }

    // Getting efficiencies and saving to output file ...
    TFile* f = nullptr;
    TTree* tree = nullptr;

    f = new TFile(Form("effGem_%d_nHitsMinGlobTrack_%d_nHitsMinSilicon_%d_nHitsMinGem_%d.root",
            fHeader->GetRunId(), fNHits, fNHitsSilicon, fNHitsGem), "recreate");
    tree = new TTree("bmndata", "bmndata");

    TClonesArray* gEff = new TClonesArray("EffStore");

    tree->Branch("GemEff", &gEff);

    for (Int_t iCont = 0; iCont < effGem->GetEntriesFast(); iCont++) {
        gEff->Delete();
        TClonesArray* arr = (TClonesArray*) effGem->UncheckedAt(iCont);

        new ((*gEff)[gEff->GetEntriesFast()]) EffStore(*((EffStore*) arr));
        tree->Fill();
    }

    tree->Write();

    delete gEff;
    delete f;
    if (tree)
        tree = nullptr;

    f = new TFile(Form("effSil_%d_nHitsMinGlobTrack_%d_nHitsMinSilicon_%d_nHitsMinGem_%d.root",
            fHeader->GetRunId(), fNHits, fNHitsSilicon, fNHitsGem), "recreate");
    tree = new TTree("bmndata", "bmndata");

    TClonesArray* sEff = new TClonesArray("EffStore");

    tree->Branch("SiliconEff", &sEff);

    for (Int_t iCont = 0; iCont < effSilicon->GetEntriesFast(); iCont++) {
        sEff->Delete();
        TClonesArray* arr = (TClonesArray*) effSilicon->UncheckedAt(iCont);

        new ((*sEff)[sEff->GetEntriesFast()]) EffStore(*((EffStore*) arr));
        tree->Fill();
    }

    tree->Write();

    delete sEff;
    delete f;

    TCanvas* c = new TCanvas("c", "c", 800, 800);
    c->cd();
    hDistVp->Draw();
    hDistVp->SetLineWidth(2);

    TLine* cutLine = new TLine(0.4, 0., 0.4, hDistVp->GetMaximum());
    cutLine->Draw("same");
    cutLine->SetLineWidth(2);
    cutLine->SetLineStyle(kDashed);
    c->SaveAs("distXY.pdf");

    delete c;

    //    for (Int_t iCont = 0; iCont < effGem->GetEntriesFast(); iCont++) {
    //        EffStore* effCont = (EffStore*) effGem->UncheckedAt(iCont);
    //        cout << "Stat# " << effCont->Station() << " Mod# " << effCont->Module() << " Zone# " << effCont->Zone() << " Eff# " << effCont->Efficiency() << endl;
    //    }
    //
    //    for (Int_t iCont = 0; iCont < effSilicon->GetEntriesFast(); iCont++) {
    //        EffStore* effCont = (EffStore*) effSilicon->UncheckedAt(iCont);
    //        cout << "Stat# " << effCont->Station() << " Mod# " << effCont->Module() << " Zone# " << effCont->Zone() << " Eff# " << effCont->Efficiency() << endl;
    //    }
}

Int_t BmnEfficiency::GetSiliconStatModule(BmnHit* hit) {
    Int_t stat = hit->GetStation();
    Double_t x = hit->GetX();
    Double_t y = hit->GetY();
    // cout << x << " and " << y << endl;

    Int_t nMods = silicon->GetStation(stat)->GetNModules();

    Int_t module = -1;

    for (Int_t iMod = 0; iMod < nMods; iMod++) {
        BmnSiliconModule* mod = silicon->GetStation(stat)->GetModule(iMod);

        Double_t xMax = -mod->GetXMinModule();
        Double_t xMin = -mod->GetXMaxModule();
        Double_t yMin = mod->GetYMinModule();
        Double_t yMax = mod->GetYMaxModule();

        // cout << xMin << " " << xMax << " " << yMin << " " << yMax << endl;

        if (x > xMin && x < xMax && y > yMin && y < yMax) {
            module = iMod;
            break;
        }
    }

    // If we did not get a module due to a case when the Kalman propagation to an averaged Z-position is not so precise to get its correct number ...
    if (module == -1) {

        map <Double_t, Int_t> distMod;

        Double_t z = hit->GetZ();

        for (Int_t iMod = 0; iMod < nMods; iMod++) {
            BmnSiliconModule* mod = silicon->GetStation(stat)->GetModule(iMod);

            Double_t xMax = -mod->GetXMinModule();
            Double_t xMin = -mod->GetXMaxModule();
            Double_t yMin = mod->GetYMinModule();
            Double_t yMax = mod->GetYMaxModule();

            Double_t xMid = .5 * (xMin + xMax);
            Double_t yMid = .5 * (yMin + yMax);
            Double_t zReg = mod->GetZPositionRegistered();

            Double_t dist = TMath::Sqrt((x - xMid) * (x - xMid) + (y - yMid) * (y - yMid) + (z - zReg) * (z - zReg));
            distMod[dist] = iMod;
        }

        for (auto it : distMod) {
            // cout << it.first << " ... " << it.second << endl;
            module = it.second;
            break;
        }
    }

    return module;
}

Int_t BmnEfficiency::GetGemStatModule(BmnHit* hit) {
    Int_t stat = hit->GetStation();
    Double_t x = hit->GetX();
    Double_t y = hit->GetY();

    Int_t nMods = gem->GetStation(stat)->GetNModules();

    Int_t module = -1;

    for (Int_t iMod = 0; iMod < nMods; iMod++) {
        BmnGemStripModule* mod = gem->GetStation(stat)->GetModule(iMod);

        Double_t xMax = -mod->GetXMinModule();
        Double_t xMin = -mod->GetXMaxModule();
        Double_t yMin = mod->GetYMinModule();
        Double_t yMax = mod->GetYMaxModule();

        if (x > xMin && x < xMax && y > yMin && y < yMax) {
            module = iMod;
            break;
        }
    }

    return module;
}

TString BmnEfficiency::GetGemZone(BmnHit* hit) {
    TString zone = "";

    if (hit->GetZ() < gem->GetStation(0)->GetZPosition() - 5.)
        return zone;

    Int_t stat = hit->GetStation();
    Int_t mod = hit->GetModule();

    Double_t x = hit->GetX();
    Double_t y = hit->GetY();

    const Int_t nLayers = gem->GetStation(stat)->GetModule(mod)->GetNStripLayers();
    Bool_t isHitInside[nLayers];

    for (Int_t iLayer = 0; iLayer < nLayers; iLayer++) {
        BmnGemStripLayer layer = gem->GetStation(stat)->GetModule(mod)->GetStripLayer(iLayer);
        isHitInside[iLayer] = layer.IsPointInsideStripLayer(-x, y);
    }

    if (isHitInside[0] && isHitInside[1])
        zone = "big";
    else if (isHitInside[2] && isHitInside[3])
        zone = "hot";

    return zone;
}

Bool_t BmnEfficiency::isInAcceptance(Int_t stat, Double_t x, Double_t y) {

    vector <Double_t> borders = fStatAcceptance.find(stat)->second;
    Bool_t flag = kFALSE;

    Double_t xMin = borders[0];
    Double_t xMax = borders[1];
    Double_t yMin = borders[2];
    Double_t yMax = borders[3];

    if (x > xMin && x < xMax && y > yMin && y < yMax)
        flag = kTRUE;

    return flag;
}