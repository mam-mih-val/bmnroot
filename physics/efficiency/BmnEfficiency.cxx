#include "BmnEfficiency.h"

#include "TFile.h"

BmnEfficiency::BmnEfficiency(FairRunAna* fAna, BmnInnerTrackerGeometryDraw* fInnTracker, TString dstFile, Int_t nEvents) :
isGoodDst(kFALSE),
fNEvents(nEvents),
gem(nullptr),
silicon(nullptr),
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
    cout << "runId = " << runId << endl;

    fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");

    if (runId && runId < 10000) {
        UniRun* runInfo = UniRun::GetRun(7, runId);

        if (!runInfo)
            return;

        fMagField->SetScale(*runInfo->GetFieldVoltage() / 55.87);
    } else
        fMagField->SetScale(1251.9 / 900.); // FIXME!!!

    fMagField->Init();

    fAna->SetField(fMagField);
    fField = fAna->GetField();

    fKalman = new BmnKalmanFilter();

    // Calculating boarders and stat (mod) positions to be sure whether we are in acceptance ...
    gem = fInnTracker->GetGemGeometry();
    silicon = fInnTracker->GetSiliconGeometry();

    TClonesArray* GEM = new TClonesArray("InnerTrackerParams");
    TClonesArray* SILICON = new TClonesArray("InnerTrackerParams");

    for (Int_t iEvent = 0; iEvent < 2000; iEvent++) {
        dstChain->GetEntry(iEvent);

        // Loop over GEM hits ...
        for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*) fGemHits->UncheckedAt(iHit);

            new ((*GEM)[GEM->GetEntriesFast()])
                    InnerTrackerParams((Int_t) hit->GetStation(), hit->GetModule(), hit->GetX(), hit->GetY(), hit->GetZ());
        }

        // Loop over SILICON hits ...
        for (Int_t iHit = 0; iHit < fSiliconHits->GetEntriesFast(); iHit++) {
            BmnHit* hit = (BmnHit*) fSiliconHits->UncheckedAt(iHit);

            new ((*SILICON)[SILICON->GetEntriesFast()])
                    InnerTrackerParams((Int_t) hit->GetStation(), hit->GetModule(), hit->GetX(), hit->GetY(), hit->GetZ());
        }
    }

    FillAcceptanceMaps(GEM);
    FillAcceptanceMaps(GEM, SILICON);

    delete GEM;
    delete SILICON;
}

void BmnEfficiency::FillAcceptanceMaps(TClonesArray* GEMs, TClonesArray* SILs) {
    Int_t nStats = 0;
    Int_t nMods = 0;

    if (!SILs) {
        nStats = gem->GetNStations();
        nMods = gem->GetStation(0)->GetNModules();
    } else {
        nStats = silicon->GetNStations();
        nMods = silicon->GetStation(2)->GetNModules();
    }

    set <Double_t>*** x = new set <Double_t>**[nStats];
    set <Double_t>*** y = new set <Double_t>**[nStats];
    set <Double_t>*** z = new set <Double_t>**[nStats];

    for (Int_t iStat = 0; iStat < nStats; iStat++) {
        x[iStat] = new set <Double_t>*[nMods];
        y[iStat] = new set <Double_t>*[nMods];
        z[iStat] = new set <Double_t>*[nMods];

        for (Int_t iMod = 0; iMod < nMods; iMod++) {
            x[iStat][iMod] = new set <Double_t>();
            y[iStat][iMod] = new set <Double_t>();
            z[iStat][iMod] = new set <Double_t>();
        }
    }

    TClonesArray* arr = (!SILs) ? GEMs : SILs;

    for (Int_t iStat = 0; iStat < nStats; iStat++)
        for (Int_t iMod = 0; iMod < nMods; iMod++)
            for (Int_t iEle = 0; iEle < arr->GetEntriesFast(); iEle++) {
                InnerTrackerParams* g = (InnerTrackerParams*) arr->UncheckedAt(iEle);
                if (g->station() == iStat && g->module() == iMod) {
                    x[iStat][iMod]->insert(g->xyz().X());
                    y[iStat][iMod]->insert(g->xyz().Y());
                    z[iStat][iMod]->insert(g->xyz().Z());
                }
            }

    Int_t shift = (!SILs) ? silicon->GetNStations() : 0;

    for (Int_t iStat = 0; iStat < nStats; iStat++)
        for (Int_t iMod = 0; iMod < nMods; iMod++) {

            Double_t minX = *min_element(x[iStat][iMod]->begin(), x[iStat][iMod]->end());
            Double_t maxX = *max_element(x[iStat][iMod]->begin(), x[iStat][iMod]->end());

            Double_t minY = *min_element(y[iStat][iMod]->begin(), y[iStat][iMod]->end());
            Double_t maxY = *max_element(y[iStat][iMod]->begin(), y[iStat][iMod]->end());

            vector <Double_t> borders{minX, maxX, minY, maxY};

            if (z[iStat][iMod]->size()) {
                fStatZ[make_pair(iStat + shift, iMod)] = *z[iStat][iMod]->begin();
                fStatAcceptance[make_pair(iStat + shift, iMod)] = borders;
            }
        }

    // Deleting allocated memory ...
    for (Int_t iStat = 0; iStat < nStats; iStat++)
        for (Int_t iMod = 0; iMod < nMods; iMod++) {
            delete x[iStat][iMod];
            delete y[iStat][iMod];
            delete z[iStat][iMod];
        }

    delete [] x;
    delete [] y;
    delete [] z;
}

BmnEfficiency::BmnEfficiency(FairRunAna* fAna, TString dstFile, Int_t nEvents) {
    isGoodDst = kFALSE;
    fNEvents = nEvents;
    gem = nullptr;
    silicon = nullptr;
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

    if (runId && runId < 10000) {
        UniRun* runInfo = UniRun::GetRun(7, runId);

        if (!runInfo)
            return;

        fMagField->SetScale(*runInfo->GetFieldVoltage() / 55.87);
    } else
        fMagField->SetScale(1251.9 / 900.); // FIXME !!!

    fMagField->Init();

    fAna->SetField(fMagField);
    fField = fAna->GetField();

    fKalman = new BmnKalmanFilter();
}

void BmnEfficiency::Efficiency(TClonesArray* effGem,      
        TClonesArray* effSilicon,
        map <Int_t, vector <pair <Double_t, Double_t>>> gYr,
        map <Int_t, vector <pair <Double_t, Double_t>>> sYr) {

    if (!isGoodDst)
        return;

    auto silFirst = fStatZ.begin();
    auto gemLast = fStatZ.rbegin();

    Int_t nHitsMax = gem->GetNStations() + silicon->GetNStations();
    Double_t zBegin = silFirst->second - 5.; // first SILICON - 5 cm
    Double_t zEnd = gemLast->second + 5.; // last GEM + 5 cm

    Int_t nModsSilicon = 0;
    for (Int_t iStat = 0; iStat < silicon->GetNStations(); iStat++)
        for (Int_t iMod = 0; iMod < silicon->GetStation(iStat)->GetNModules(); iMod++)
            nModsSilicon++;

    auto gemFirst = next(fStatZ.begin(), nModsSilicon);
    auto silLast = next(fStatZ.begin(), nModsSilicon - 1);

    Double_t borderSilGem = .5 * (gemFirst->second + silLast->second);

    gStyle->SetOptStat(0);

    for (Int_t iEvent = 0; iEvent < ((fNEvents == 0) ? dstChain->GetEntries() : fNEvents); iEvent++) {
        dstChain->GetEntry(iEvent);

        if (iEvent % 100000 == 0)
            cout << "Event# " << iEvent << endl;

        BmnVertex* Vp = (BmnVertex*) fVertices->UncheckedAt(0);
        if (Vp->GetNTracks() < 3) // FIXME !!!
            continue;

        // Whether we skip it or not ...
        if (TMath::Abs(Vp->GetZ()) > 6.) // FIXME !!!
            continue;

        // Loop over glob. tracks ...
        for (Int_t iTrack = 0; iTrack < fGlobTracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iTrack);

            if (TMath::Abs(track->GetP()) < .5 || TMath::Abs(track->GetP()) > 5.) /// FIXME !!!
                continue;

            vector <BmnHit*> hits;

            // Loop over silicon hits ...
            if (track->GetSilTrackIndex() != -1) {
                BmnTrack* silTrack = (BmnTrack*) fSiliconTracks->UncheckedAt(track->GetSilTrackIndex());

                for (Int_t iHit = 0; iHit < silTrack->GetNHits(); iHit++)
                    hits.push_back((BmnHit*) fSiliconHits->UncheckedAt(silTrack->GetHitIndex(iHit)));
            }

            // Loop over gem hits ... 
            if (track->GetGemTrackIndex() != -1) {
                BmnTrack* gemTrack = (BmnTrack*) fGemTracks->UncheckedAt(track->GetGemTrackIndex());

                for (Int_t iHit = 0; iHit < gemTrack->GetNHits(); iHit++)
                    hits.push_back((BmnHit*) fGemHits->UncheckedAt(gemTrack->GetHitIndex(iHit)));
            }

            Int_t nHits = 0;

            // Let us calculate number of hits in [zBegin, zEnd] ...
            for (BmnHit* hit : hits) {
                Double_t Z = hit->GetZ();
                if (Z > zBegin && Z < zEnd)
                    nHits++;
            }

            if (nHits < fNHits)
                continue;

            // Also, a track should satisfy the condition: SilHits >= 2 && GemHits >= 4
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

            const Double_t distCut = 2.; // FIXME!!!
            Double_t distXY = TMath::Sqrt((xPredictedAtVp - x) * (xPredictedAtVp - x) + (yPredictedAtVp - y) * (yPredictedAtVp - y));

            if (distXY > distCut)
                continue;

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

            inputForEfficiency input;
            // Setting  for GEM ...
            input.hits = hits;

            vector <Bool_t> v;
            v.assign(hitsPerStation, hitsPerStation + nHitsMax);
            input.usedStats = v;

            input.sfIndices.first = silicon->GetNStations();
            input.sfIndices.second = nHitsMax;

            input.flParams = trackParams;
            input.eContainers = effGem;

            input.gPairMap = gYr;
            input.sPairMap = sYr;

            fillEfficiency(std::move(input));

            inputForEfficiency input1;
            input1 = std::move(input); // Right now input becomes not fully valid since is in unspecified state !!! ...

            input1.det = "SILICON";
            input1.sfIndices.first = 0;
            input1.sfIndices.second = silicon->GetNStations();
            input1.eContainers = effSilicon;

            fillEfficiency(std::move(input1));
        }        
    }
}

Bool_t BmnEfficiency::isVirtualHitInAcceptance(Int_t iHit, vector <BmnHit*> hits, pair <FairTrackParam, FairTrackParam> params, pair <Double_t, Double_t>& xyPred) {
    // Averaged Z by all modules for station ... ???
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
        return kFALSE;

    Int_t mod0 = -1;
    Double_t xPred = params.first.GetX();
    Double_t yPred = params.first.GetY();

    xyPred.first = xPred;
    xyPred.second = yPred;

    for (auto it : fStatAcceptance) {
        Int_t stat = it.first.first;
        Int_t mod = it.first.second;

        Double_t xMin = it.second[0];
        Double_t xMax = it.second[1];
        Double_t yMin = it.second[2];
        Double_t yMax = it.second[3];

        if (xPred > xMin && xPred < xMax && yPred > yMin && yPred < yMax && (stat == iHit)) {
            mod0 = mod;
            break;
        }
    }

    return (mod0 > -1) ? kTRUE : kFALSE;
}

void BmnEfficiency::fillEfficiency(inputForEfficiency&& input) {
    const Int_t hitCut = (input.det.Contains("GEM") ? input.nGemCut : input.nSilCut);
    const Int_t shift = (input.det.Contains("GEM") ? -silicon->GetNStations() : 0);

    // Getting nHits per detector ...
    Int_t nHitsPerDetector = 0;

    auto itBegin = input.usedStats.begin();
    auto itEnd = input.usedStats.end();
    auto itMiddle = next(input.usedStats.begin(), silicon->GetNStations());

    if (input.det.Contains("SILICON"))
        for (auto it = itBegin; it < itMiddle; it++) {
            if (*it)
                nHitsPerDetector++;
        } else
        for (auto it = itMiddle; it != itEnd; it++) {
            if (*it)
                nHitsPerDetector++;
        }

    if (!nHitsPerDetector)
        return;

    for (Int_t iHit = input.sfIndices.first; iHit < input.sfIndices.second; iHit++) {
        // Skipping tracks with four hits having a hit in considering station ...
        if (nHitsPerDetector == hitCut && input.usedStats.at(iHit))
            continue;

        if (input.usedStats.at(iHit) && nHitsPerDetector != hitCut) {
            // Trying to get required hit ...
            BmnHit* hit0 = nullptr;

            Int_t stat = -1;
            for (BmnHit* hit : input.hits) {
                if (!GetDetector(hit).Contains(input.det.Data()))
                    continue;
                stat = hit->GetStation();

                if (stat == (iHit + shift)) {
                    hit0 = hit;
                    break;
                }
            }

            if (hit0) {
                Int_t eCont = -1;
                for (Int_t iCont = 0; iCont < input.eContainers->GetEntriesFast(); iCont++) {
                    EffStore2D* effCont = (EffStore2D*) input.eContainers->UncheckedAt(iCont);

                    if (effCont->Detector() == input.det && effCont->Station() == (iHit + shift)) {
                        eCont = iCont;
                        break;
                    }
                }

                if (eCont > -1) {
                    EffStore2D* eff = (EffStore2D*) input.eContainers->UncheckedAt(eCont);
                    eff->efficiency(((input.flParams.second.GetQp() > 0.) ? 0 : 1), "", input.det)->Fill(kTRUE, hit0->GetX(), hit0->GetY());

                    if (input.det.Contains("SILICON"))
                        eff->efficiency(((input.flParams.second.GetQp() > 0.) ? 0 : 1), "xP", input.det)->Fill(kTRUE, hit0->GetX(),
                            TMath::Abs(1. / input.flParams.first.GetQp()));

                    Int_t yBin = FindBin(hit0->GetY(), stat, ((input.det.Contains("GEM")) ? input.gPairMap : input.sPairMap));
                    if (yBin != -1)
                        eff->efficiency4range(yBin, ((input.flParams.second.GetQp() > 0.) ? 0 : 1), input.det)->Fill(kTRUE, hit0->GetX());
                }
            }
        } else {
            pair <Double_t, Double_t> xyPred;

            if (isVirtualHitInAcceptance(iHit, input.hits, input.flParams, xyPred)) {
                Int_t yBin = FindBin(xyPred.second, iHit + shift, ((input.det.Contains("GEM")) ? input.gPairMap : input.sPairMap));
                if (yBin == -1)
                    continue;

                Int_t eCont = -1;
                for (Int_t iCont = 0; iCont < input.eContainers->GetEntriesFast(); iCont++) {
                    EffStore2D* effCont = (EffStore2D*) input.eContainers->UncheckedAt(iCont);

                    if (effCont->Detector() == input.det && effCont->Station() == (iHit + shift)) {
                        eCont = iCont;
                        break;
                    }
                }

                if (eCont > -1) {
                    EffStore2D* eff = (EffStore2D*) input.eContainers->UncheckedAt(eCont);
                    eff->efficiency(((input.flParams.second.GetQp() > 0.) ? 0 : 1), "", input.det)->Fill(kFALSE, xyPred.first, xyPred.second);

                    if (input.det.Contains("SILICON"))
                        eff->efficiency(((input.flParams.second.GetQp() > 0.) ? 0 : 1), "xP", input.det)->Fill(kFALSE, xyPred.first,
                            TMath::Abs(1. / input.flParams.first.GetQp()));

                    eff->efficiency4range(yBin, ((input.flParams.second.GetQp() > 0.) ? 0 : 1), input.det)->Fill(kFALSE, xyPred.first);
                }
            }
        }
    }
}

TString BmnEfficiency::GetGemZone(BmnHit * hit) {
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
