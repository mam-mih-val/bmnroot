#include "BmnEfficiency.h"

BmnEfficiency::BmnEfficiency(FairRunAna* fAna, BmnInnerTrackerGeometryDraw* fInnTracker, TString dstFile, Int_t nEvents) :
isGoodDst(kFALSE),
fNEvents(nEvents),
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
fMagField(nullptr),
fNHits(6), fNHitsSilicon(2), fNHitsGem(4) {

    // Open dst file ...
    dstChain = new TChain("bmndata");

    // Adding dst file to the chain ...
    TFile f(dstFile);
    if (f.IsOpen() && !f.IsZombie()) {
        dstChain->Add(dstFile.Data());
        isGoodDst = kTRUE;
    }

    if (!isGoodDst)
        return;

    cout << "Num. of events to be used# " << dstChain->GetEntries() << endl;

    dstChain->SetBranchAddress("DstEventHeader.", &fHeader);
    dstChain->SetBranchAddress("BmnInnerHits", &fInnerHits);
    dstChain->SetBranchAddress("BmnGemStripHit", &fGemHits);
    dstChain->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
    dstChain->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);
    dstChain->SetBranchAddress("BmnGemTrack", &fGemTracks);
    dstChain->SetBranchAddress("BmnSiliconTrack", &fSiliconTracks);
    dstChain->SetBranchAddress("BmnVertex", &fVertices);

    dstChain->GetEntry();

    UInt_t runId = fHeader->GetRunId();

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");

    if (runId < 10000) {
        UniDbRun* runInfo = UniDbRun::GetRun(7, runId);

        if (!runInfo)
            return;

        fMagField->SetScale(*runInfo->GetFieldVoltage() / 55.87);
    }
    else
        fMagField->SetScale(1200. / 900.);

    fMagField->Init();

    fAna->SetField(fMagField);
    fField = fAna->GetField();

    fKalman = new BmnKalmanFilter();

    gem = fInnTracker->GetGemGeometry();
    silicon = fInnTracker->GetSiliconGeometry();

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

BmnEfficiency::BmnEfficiency(FairRunAna* fAna, TString dstFile, Int_t nEvents) {
    isGoodDst = kFALSE;
    fNEvents = nEvents;
    gem = nullptr;
    silicon = nullptr;
    effGem = nullptr;
    effSilicon = nullptr;
    fHeader = nullptr;
    fInnerHits = nullptr;
    fGemHits = nullptr;
    fSiliconHits = nullptr;
    fGlobTracks = nullptr;
    fGemTracks = nullptr;
    fSiliconTracks = nullptr;
    fVertices = nullptr;
    dstChain = nullptr;
    fKalman = nullptr;
    fMagField = nullptr;
    fNHits = 6;
    fNHitsSilicon = 2;
    fNHitsGem = 4;

    // Open dst file ...
    dstChain = new TChain("bmndata");

    // Adding dst file to the chain ...
    TFile f(dstFile);
    if (f.IsOpen() && !f.IsZombie()) {
        dstChain->Add(dstFile.Data());
        isGoodDst = kTRUE;
    }

    if (!isGoodDst)
        return;

    cout << "Num. of events to be used# " << dstChain->GetEntries() << endl;

    dstChain->SetBranchAddress("DstEventHeader.", &fHeader);
    dstChain->SetBranchAddress("BmnInnerHits", &fInnerHits);
    dstChain->SetBranchAddress("BmnGemStripHit", &fGemHits);
    dstChain->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
    dstChain->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);
    dstChain->SetBranchAddress("BmnGemTrack", &fGemTracks);
    dstChain->SetBranchAddress("BmnSiliconTrack", &fSiliconTracks);
    dstChain->SetBranchAddress("BmnVertex", &fVertices);

    dstChain->GetEntry();

    UInt_t runId = fHeader->GetRunId();

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");

    if (runId < 10000) {
        UniDbRun* runInfo = UniDbRun::GetRun(7, runId);

        if (!runInfo)
            return;

        fMagField->SetScale(*runInfo->GetFieldVoltage() / 55.87);
    }
    else
        fMagField->SetScale(1200. / 900.);

    fMagField->Init();

    fAna->SetField(fMagField);
    fField = fAna->GetField();

    fKalman = new BmnKalmanFilter();
}

void BmnEfficiency::Efficiency(UInt_t& nEffEvs, TH1F* h) {
    if (!isGoodDst)
        return;

    // Some char. numbers to be used below ...
    Int_t nHitsMax = gem->GetNStations() + silicon->GetNStations();
    Double_t zBegin = silicon->GetStation(0)->GetZPosition() - 5.; // first SILICON - 5 cm
    Double_t zEnd = gem->GetStation(5)->GetZPosition() + 5.; // last GEM + 5 cm

    Double_t borderSilGem = .5 * (gem->GetStation(0)->GetZPosition() + silicon->GetStation(2)->GetZPosition());

    gStyle->SetOptStat(0);
    TH1F* hDistVp = new TH1F("dist. at Vp", "dist. at Vp; distXY at VpZ [cm]", 100, 0., 5.);

    for (Int_t iEvent = 0; iEvent < ((fNEvents == 0) ? dstChain->GetEntries() : fNEvents); iEvent++) {
        dstChain->GetEntry(iEvent);

        if (iEvent % 1000 == 0)
            cout << "Event# " << iEvent << endl;

        //        CbmVertex* Vp = (CbmVertex*) fVertices->UncheckedAt(0);
        BmnVertex* Vp = (BmnVertex*) fVertices->UncheckedAt(0);

        // Whether we skip it or not ...
        if (TMath::Abs(Vp->GetZ()) < 3.)
            nEffEvs++;
        else
            continue;

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

            // Let us calculate number of hits in [zBegin, zEnd] ...
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

            // 1. Selecting tracks assumed to be from Vp ...
            Double_t x = Vp->GetX();
            Double_t y = Vp->GetY();
            Double_t z = Vp->GetZ();

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

            // 2. Let us check that the zone of GEM plane is defined correctly ...
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

            // FairTrackParam* first = track->GetParamFirst();
            // FairTrackParam* last = track->GetParamLast();

            FairTrackParam parPredicted = *track->GetParamFirst();
            pair <FairTrackParam, FairTrackParam> trackParams = make_pair(parPredicted, *track->GetParamLast());

            // 3. Array to store info on hits per SILICON and GEM part of tracker
            Bool_t hitsPerStation[nHitsMax];
            for (Int_t iHit = 0; iHit < nHitsMax; iHit++)
                hitsPerStation[iHit] = kFALSE;

            // 4. Collecting hits found on the track in one array ...
            for (BmnHit* hit : hits) {
                TString det = GetDetector(hit);
                Int_t shift = (det.Contains("GEM")) ? silicon->GetNStations() : 0;

                Int_t stat = hit->GetStation();
                hitsPerStation[stat + shift] = kTRUE;
            }

            for (BmnHit* hit : hits) {
                TString det = GetDetector(hit);
                Int_t stat = hit->GetStation();
                Int_t mod = hit->GetModule();
                TString zone = GetGemZone(hit);

                Bool_t isContainerFound = kFALSE;

                for (Int_t iCont = 0; iCont < effGem->GetEntriesFast(); iCont++) {
                    EffStore* effCont = (EffStore*) effGem->UncheckedAt(iCont);

                    if (effCont->Detector() != det || effCont->Station() != stat || effCont->Module() != mod || effCont->Zone() != zone)
                        continue;

                    // 4a. Processing four-hit tracks in GEM ...
                    if (nHitsPerGem == 4) {
                        // Nominator and denominator are always increased by zero
                        // for existing hits per track!

                        // Let's check what is missing for GEM ...
                        for (Int_t iHit = silicon->GetNStations(); iHit < nHitsMax; iHit++) {

                            if (hitsPerStation[iHit])
                                continue;

                            BmnHit* hit0 = virtualHitIfInAcceptance(iHit, hits, trackParams);
                            if (hit0)
                                effCont->IncreaseDenominatorByUnity();
                        }
                    }
                        // 4b. Processing six-hit tracks in GEM ... 
                    else if (nHitsPerGem == gem->GetNStations()) {
                        // GEM is always considered to be efficient ...
                        effCont->IncreaseNominatorByUnity();
                        effCont->IncreaseDenominatorByUnity();
                    }
                        // 4c. Other tracks (five-hit) in GEM ...
                    else {
                        for (Int_t iHit = silicon->GetNStations(); iHit < nHitsMax; iHit++) {
                            if (hitsPerStation[iHit]) {
                                effCont->IncreaseNominatorByUnity();
                                effCont->IncreaseDenominatorByUnity();
                            } else {
                                BmnHit* hit0 = virtualHitIfInAcceptance(iHit, hits, trackParams);
                                if (hit0)
                                    effCont->IncreaseDenominatorByUnity();
                            }
                        }
                    }

                    isContainerFound = kTRUE;

                    break;
                }
                //
                if (!isContainerFound)
                    for (Int_t iCont = 0; iCont < effSilicon->GetEntriesFast(); iCont++) {

                        EffStore* effCont = (EffStore*) effSilicon->UncheckedAt(iCont);
                        // cout << effCont->Efficiency() << endl;

                        if (effCont->Detector() != det || effCont->Station() != stat || effCont->Module() != mod || effCont->Zone() != zone)
                            continue;

                        if (nHitsPerSilicon == 2) {
                            // Nominator and denominator are always increased by zero
                            // for existing hits per track!

                            // Let's check what is missing for GEM ...
                            for (Int_t iHit = 0; iHit < silicon->GetNStations(); iHit++) {

                                if (hitsPerStation[iHit])
                                    continue;

                                BmnHit* hit0 = virtualHitIfInAcceptance(iHit, hits, trackParams);
                                if (hit0)
                                    effCont->IncreaseDenominatorByUnity();
                            }
                        } else {
                            // SILICON is always considered to be efficient ...
                            effCont->IncreaseNominatorByUnity();
                            effCont->IncreaseDenominatorByUnity();
                        }
                        break;
                    }
            }
        }
    }
    delete hDistVp;
}

//delete hDistVp;
//}

BmnHit* BmnEfficiency::virtualHitIfInAcceptance(Int_t iHit, vector <BmnHit*> hits, pair <FairTrackParam, FairTrackParam> params) {
    BmnHit* hit0 = nullptr;

    Double_t Z = FindZ(iHit);

    vector <BmnHit*> hitsBeforeCurrentZ;

    for (BmnHit* hit : hits) {
        Double_t zHit = hit->GetZ();

        if (zHit > Z)
            continue;

        hitsBeforeCurrentZ.push_back(hit);
    }

    for (BmnHit* hit : hitsBeforeCurrentZ) {
        BmnStatus status = fKalman->TGeoTrackPropagate(&(params.first), hit->GetZ(), params.second.GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
        if (status == kBMNERROR)
            continue;

        Double_t chi2 = 0.;
        fKalman->Update(&(params.first), hit, chi2);
    }

    BmnStatus status = fKalman->TGeoTrackPropagate(&(params.first), Z, params.second.GetQp() > 0. ? 2212 : -211, nullptr, nullptr, kTRUE);
    if (status == kBMNERROR)
        return hit0;

    if (isInAcceptance(iHit, params.first.GetX(), params.first.GetY())) {

        // Creating a hyp. missing hit ...
        hit0 = new BmnHit();
        hit0->SetXYZ(params.first.GetX(), params.first.GetY(), Z);

        //delete missingHit;
        return hit0;
    }
    return hit0;
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