#include "BmnLambdaEmbeddingQa.h"
#include "BmnSiliconPoint.h"

#include "TObjString.h"

BmnLambdaEmbeddingQa::BmnLambdaEmbeddingQa() :
fRunAna(nullptr),
geoms(nullptr),
fDigi(nullptr),
fDst(nullptr),
fEmb(nullptr),
fPath(nullptr),
fMon(nullptr),
hDst(nullptr),
hDigi(nullptr),
fGemHits(nullptr),
fGemDigits(nullptr),
fSiliconDigits(nullptr),
fSiliconHits(nullptr),
fSiliconTracks(nullptr),
fGlobTracks(nullptr),
fMcTracks(nullptr),
fSiliconPoints(nullptr),
fGemPoints(nullptr),
fMagField(nullptr),
hXzMC(nullptr),
hXzReco(nullptr),
hYzMC(nullptr),
hYzReco(nullptr),
hGemXYProfiles(nullptr),
hSiliconXYProfiles(nullptr),
hXzRecoFromTracks(nullptr),
hProtonMomentaEmb(nullptr),
hPionMomentaEmb(nullptr),
hProtonMomentaReco(nullptr),
hPionMomentaReco(nullptr),
hProtonNhitsRecoAll(nullptr),
hPionNhitsRecoAll(nullptr),
hProtonNhitsEmb(nullptr),
hPionNhitsEmb(nullptr),
hProtonNhitsReco(nullptr),
hPionNhitsReco(nullptr),
pEffProton(nullptr),
pEffPion(nullptr),
hStripChannel(nullptr),
fPrefix(""),
histoMan(nullptr) {
    // Get inn. tracker geometry ...
    geoms = new BmnInnerTrackerGeometryDraw();
}

BmnLambdaEmbeddingQa::BmnLambdaEmbeddingQa(TString list) :
fRunAna(nullptr),
geoms(nullptr),
fDigi(nullptr),
fDst(nullptr),
fEmb(nullptr),
fPath(nullptr),
fMon(nullptr),
hDst(nullptr),
hDigi(nullptr),
fGemHits(nullptr),
fGemDigits(nullptr),
fSiliconDigits(nullptr),
fSiliconHits(nullptr),
fSiliconTracks(nullptr),
fGlobTracks(nullptr),
fMcTracks(nullptr),
fSiliconPoints(nullptr),
fGemPoints(nullptr),
fMagField(nullptr),
hXzMC(nullptr),
hXzReco(nullptr),
hYzMC(nullptr),
hYzReco(nullptr),
hGemXYProfiles(nullptr),
hSiliconXYProfiles(nullptr),
hXzRecoFromTracks(nullptr),
hProtonMomentaEmb(nullptr),
hPionMomentaEmb(nullptr),
hProtonMomentaReco(nullptr),
hPionMomentaReco(nullptr),
hProtonNhitsRecoAll(nullptr),
hPionNhitsRecoAll(nullptr),
hProtonNhitsEmb(nullptr),
hPionNhitsEmb(nullptr),
hProtonNhitsReco(nullptr),
hPionNhitsReco(nullptr),
pEffProton(nullptr),
pEffPion(nullptr),
hStripChannel(nullptr),
fPrefix(""),
histoMan(nullptr) {
    // Get inn. tracker geometry ...
    geoms = new BmnInnerTrackerGeometryDraw();

    // Get histo manager ...
    histoMan = new BmnLambdaEmbeddingDrawHistos();

    nInputs = -1;
    drawFoundTracks = kFALSE;

    // Strip lays to be shown by default ...
    aLaysToShow[0] = 0;
    aLaysToShow[1] = 1;

    // Signal windows by default ...
    fSignalWindow[GEM][0] = 0;
    fSignalWindow[GEM][1] = LDBL_MAX;
    fSignalWindow[SILICON][0] = 0;
    fSignalWindow[SILICON][1] = LDBL_MAX;

    // 1. Process list of passed files ...
    ifstream f(list.Data());
    Int_t nLines = 0;
    for (string line; getline(f, line);)
        nLines++;
    f.close();

    cout << "List file contains " << nLines << " inputs" << endl;

    if (nLines != 0) {
        fDigi = new TString[nLines];
        fDst = new TString[nLines];
        fEmb = new TString[nLines];
        fPath = new TString[nLines];
    }

    nLines = 0;
    ifstream g(list.Data());
    while (!g.eof()) {
        g >> fDigi[nLines] >> fDst[nLines] >> fEmb[nLines] >> fPath[nLines];
        nLines++;
    }
    g.close();

    nInputs = nLines;

    for (Int_t iLine = 0; iLine < nLines; iLine++)
        cout << fDigi[iLine] << " " << fDst[iLine] << " " << fEmb[iLine] << " " << fPath[iLine] << endl;
}

void BmnLambdaEmbeddingQa::DoEffAnalysis() {
    histoMan->SetActive(1);

    BmnGemStripStationSet* GEM = geoms->GetGemGeometry();
    BmnSiliconStationSet* SILICON = geoms->GetSiliconGeometry();

    TProfile2D* pEmbSilHitEff = histoMan->GetSilHitEff();
    TProfile2D** pEmbGemHitEff = histoMan->GetGemHitEff();

    TProfile2D** pEffSilStatXY = histoMan->GetSilHitEff2D();
    TProfile2D** pEffGemStatXY = histoMan->GetGemHitEff2D();

    TH1F*** hEtaLambda = histoMan->GetEtaLambda();
    TH2F** hNhitsEmbReco = histoMan->GetNHitsEmbReco();

    enum {
        proton, pion
    };

    if (nInputs != -1)
        for (Int_t iInput = 0; iInput < nInputs; iInput++) {
            Double_t etaMin = 0.;
            Double_t etaMax = 0.;

            // Trying to get pseudorapidity range of lambda ...
            TObjArray* tx = fPath[iInput].Tokenize("/");
            for (Int_t i = 0; i < tx->GetEntries(); i++) {
                TString str = ((TObjString*) (tx->At(i)))->String();

                if (!str.Contains("lambda"))
                    continue;

                TObjArray* ty = str.Tokenize("_");

                etaMin = ((TObjString*) (ty->At(2)))->String().Atof();
                etaMax = ((TObjString*) (ty->At(3)))->String().Atof();

                break;
            }

            TChain* emb = new TChain("bmndata");
            emb->Add(fEmb[iInput].Data());

            emb->SetBranchAddress("EmbeddingMonitor.", &fMon);

            TChain* dst = new TChain("bmndata");
            dst->Add(fDst[iInput].Data());

            dst->SetBranchAddress("DstEventHeader.", &hDst);
            dst->SetBranchAddress("BmnGemStripHit", &fGemHits);
            dst->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
            dst->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);

            const Int_t nCutBins = 4; // 0, 20, 40, 50 %
            Double_t cutsNHitsFrac[nCutBins] = {0., 0.2, 0.4, 0.5};
            Int_t nRecProtons[nCutBins];
            Int_t nRecPions[nCutBins];

            for (Int_t iBin = 0; iBin < nCutBins; iBin++) {
                nRecProtons[iBin] = 0;
                nRecPions[iBin] = 0;
            }

            Int_t nEmbeddedProtons = 0;
            Int_t nEmbeddedPions = 0;

            // Loop over events ...
            Int_t counter = 0;
            for (Int_t iEmb = 0; iEmb < emb->GetEntries(); iEmb++) {
                emb->GetEntry(iEmb);

                if (iEmb % 1000 == 0)
                    cout << "Event# " << iEmb << endl;

                if (!fMon->IsEmbedded())
                    continue;

                nEmbeddedProtons++;
                nEmbeddedPions++;

                UInt_t idEmb = fMon->GetEventId();

                for (Int_t iDst = idEmb - 1; iDst < idEmb + 1; iDst++) {

                    dst->GetEntry(iDst);

                    if (hDst->GetEventId() != idEmb)
                        continue;

                    counter++;

                    // Get event info ...
                    Int_t store = fMon->GetStoreVertexEvent()[0];
                    Int_t vertex = fMon->GetStoreVertexEvent()[1];
                    Int_t event = fMon->GetStoreVertexEvent()[2];

                    // Open lambda file ...
                    TChain* lambda = new TChain("bmndata");
                    lambda->Add(Form("%s/lambda%d_vertex%d.root", fPath[iInput].Data(), store, vertex));

                    lambda->SetBranchAddress("StsPoint", &fGemPoints);
                    lambda->SetBranchAddress("SiliconPoint", &fSiliconPoints);

                    lambda->GetEntry(event);

                    map <Int_t, vector < CbmStsPoint>> gemsMc;
                    for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++) {
                        vector <CbmStsPoint> tmp;
                        gemsMc[iStat] = tmp;
                    }

                    map <Int_t, vector < BmnSiliconPoint>> siliconsMc;
                    for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++) {
                        vector <BmnSiliconPoint> tmp;
                        siliconsMc[iStat] = tmp;
                    }

                    // Get MC point info (GEM) ...
                    for (Int_t iPoint = 0; iPoint < fGemPoints->GetEntriesFast(); iPoint++) {
                        CbmStsPoint* point = (CbmStsPoint*) fGemPoints->UncheckedAt(iPoint);

                        gemsMc.find(point->GetStation())->second.push_back(*point);
                    }

                    // Get MC point info (SILICON) ...
                    for (Int_t iPoint = 0; iPoint < fSiliconPoints->GetEntriesFast(); iPoint++) {
                        BmnSiliconPoint* point = (BmnSiliconPoint*) fSiliconPoints->UncheckedAt(iPoint);

                        siliconsMc.find(point->GetStation())->second.push_back(*point);
                    }

                    map <Int_t, vector < BmnHit>> gemsReco;
                    map <Int_t, vector < BmnHit>> siliconsReco;

                    for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++) {
                        vector <BmnHit> tmp;
                        gemsReco[iStat] = tmp;
                    }

                    for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++) {
                        vector <BmnHit> tmp;
                        siliconsReco[iStat] = tmp;
                    }

                    // Get GEM hit info ...
                    if (fGemHits)
                        for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
                            BmnHit* hit = (BmnHit*) fGemHits->UncheckedAt(iHit);

                            gemsReco.find(hit->GetStation())->second.push_back(*hit);
                        }

                    // Get SILICON hit info ...
                    if (fSiliconHits)
                        for (Int_t iHit = 0; iHit < fSiliconHits->GetEntriesFast(); iHit++) {
                            BmnHit* hit = (BmnHit*) fSiliconHits->UncheckedAt(iHit);

                            siliconsReco.find(hit->GetStation())->second.push_back(*hit);
                        }

                    // Loop over siliconMc and siliconReco maps to find embedded MC-points (in digits) and reconstructed hits ...
                    for (auto mc : siliconsMc)
                        for (auto reco : siliconsReco) {

                            // Stations should be the same ...
                            if (mc.first != reco.first)
                                continue;

                            vector <BmnSiliconPoint> points = mc.second;
                            vector <BmnHit> hits = reco.second;

                            for (auto mcPoint : points) {
                                Bool_t isEmbedded = kFALSE;

                                vector <Double_t> distances;

                                if (hits.size() != 0)
                                    for (auto recoHit : hits) {
                                        Double_t xMC = mcPoint.GetX();
                                        Double_t xReco = recoHit.GetX();

                                        // Calculate distance between a hit and a point in X-direction ...
                                        distances.push_back(Abs(xMC - xReco));
                                    }

                                if (distances.size() != 0) {
                                    Double_t min = *min_element(distances.begin(), distances.end());
                                    if (min < 0.3)
                                        isEmbedded = kTRUE;
                                }

                                pEmbSilHitEff->Fill(mcPoint.GetStation(), mcPoint.GetModule(), isEmbedded ? 1. : 0.);
                                pEffSilStatXY[mcPoint.GetStation()]->Fill(mcPoint.GetX(), mcPoint.GetY(), isEmbedded ? 1. : 0.);
                            }
                        }

                    // Loop over gemMc and gemReco maps to find embedded and reconstructed hits ...
                    for (auto mc : gemsMc)
                        for (auto reco : gemsReco) {

                            // Stations should be the same ...
                            if (mc.first != reco.first)
                                continue;

                            vector <CbmStsPoint> points = mc.second;
                            vector <BmnHit> hits = reco.second;

                            if (hits.size() == 0)
                                continue;

                            for (auto mcPoint : points) {
                                Bool_t isEmbedded = kFALSE;

                                // Taking into account correct Z-readout position for GEM ...
                                BmnGemStripModule* mod = GEM->GetStation(mcPoint.GetStation())->GetModule(mcPoint.GetModule());
                                Double_t driftCenterShift = 0.;
                                if (mod->GetElectronDriftDirection() == ForwardZAxisEDrift)
                                    driftCenterShift = 0.15;
                                else
                                    driftCenterShift = 0.75;

                                vector <Double_t> distances;

                                if (hits.size() != 0)
                                    for (auto recoHit : hits) {
                                        Double_t xMC = mcPoint.GetX(mcPoint.GetZ() + driftCenterShift);
                                        Double_t xReco = recoHit.GetX();

                                        // Calculate distance between a hit and a point in X-direction ...
                                        distances.push_back(Abs(xMC - xReco));
                                    }

                                if (distances.size() != 0) {
                                    Double_t min = *min_element(distances.begin(), distances.end());
                                    if (min < 0.5)
                                        isEmbedded = kTRUE;
                                }

                                // Defining a zone on the stat, module we are considering ...
                                const Int_t nLayers = 4;
                                Bool_t isLayerActive[nLayers] = {kFALSE, kFALSE, kFALSE, kFALSE};

                                Bool_t isHotZone = kFALSE;
                                Bool_t isBigZone = kFALSE;

                                for (Int_t iLayer = 0; iLayer < GEM->GetStation(mcPoint.GetStation())->GetModule(mcPoint.GetModule())->GetNStripLayers(); iLayer++) {
                                    BmnGemStripLayer layer = GEM->GetStation(mcPoint.GetStation())->GetModule(mcPoint.GetModule())->GetStripLayer(iLayer);
                                    isLayerActive[iLayer] = layer.IsPointInsideStripLayer((-1.) * mcPoint.GetX(mcPoint.GetZ() + driftCenterShift), mcPoint.GetY(mcPoint.GetZ() + driftCenterShift));
                                }

                                if (isLayerActive[0] && isLayerActive[1])
                                    isBigZone = kTRUE;
                                else if (isLayerActive[2] && isLayerActive[3])
                                    isHotZone = kTRUE;

                                pEffGemStatXY[mcPoint.GetStation()]->Fill(mcPoint.GetX(mcPoint.GetZ() + driftCenterShift), mcPoint.GetY(mcPoint.GetZ() + driftCenterShift), isEmbedded ? 1. : 0.);

                                Int_t idx = isBigZone ? 1 : isHotZone ? 0 : -1;

                                if (idx != -1)
                                    pEmbGemHitEff[idx]->Fill(mcPoint.GetStation(), mcPoint.GetModule(), isEmbedded ? 1. : 0.);
                            }
                        }

                    delete lambda;

                    vector <BmnGlobalTrack*> protonCands;
                    vector <BmnGlobalTrack*> pionCands;

                    
                    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobTracks->GetEntriesFast(); iGlobTrack++) {
                        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iGlobTrack);

                        if (glTrack->GetParamFirst()->GetQp() > 0.) {
                            protonCands.push_back(glTrack);
                        }
                        else
                            pionCands.push_back(glTrack);
                    }

                    // Soft check (just momRes)
                    for (BmnGlobalTrack* cand : protonCands) {
                        if (!wasReconstructed(cand, 0.05))
                            continue;

                        hNhitsEmbReco[proton]->Fill(cand->GetNHits(), fMon->GetNHitsProton());

                        break;
                    }                    

                    // Soft check (just momRes)
                    for (BmnGlobalTrack* cand : pionCands) {
                        if (!wasReconstructed(cand, 0.05))
                            continue;

                        hNhitsEmbReco[pion]->Fill(cand->GetNHits(), fMon->GetNHitsPion());

                        break;
                    }

                    // Checking selected candidates once more ...
                     for (Int_t iCut = 0; iCut < nCutBins; iCut++) {
                        /* Protons */
                        // Strong check (momRes + nHits)
                        for (BmnGlobalTrack* cand : protonCands) {
                            if (!wasReconstructed(cand, cutsNHitsFrac[iCut], 0.05))
                                continue;

                            nRecProtons[iCut]++;
                            break;
                        }

                        /* Pions */
                        // Strong check (momRes + nHits)
                        for (BmnGlobalTrack* cand : pionCands) {
                            if (!wasReconstructed(cand, cutsNHitsFrac[iCut], 0.05))
                                continue;

                            nRecPions[iCut]++;
                            break;
                        }
                    }

                    break;
                }
            }

            for (Int_t iCut = 0; iCut < nCutBins; iCut++) {
                hEtaLambda[proton][iCut]->SetBinContent(hEtaLambda[proton][iCut]->FindBin(.5 * (etaMax + etaMin)), 1. * nRecProtons[iCut] / nEmbeddedProtons);
                hEtaLambda[pion][iCut]->SetBinContent(hEtaLambda[pion][iCut]->FindBin(.5 * (etaMax + etaMin)), 1. * nRecPions[iCut] / nEmbeddedPions);
            }

            cout << "Events with embedded particles# " << counter << endl;
            delete emb;
            delete dst;
        }
}

Bool_t BmnLambdaEmbeddingQa::wasReconstructed(BmnGlobalTrack* glTrack, Double_t lostOrWrongConnectedHitsCut, Double_t momResCut) {
    Double_t pReco = 1. / glTrack->GetParamFirst()->GetQp();
    Int_t nHitsReco = glTrack->GetNHits();

    Int_t sign = (pReco > 0.) ? +1. : -1.;

    Double_t pMc = (pReco > 0.) ? fMon->GetProtonP() : fMon->GetPionP();
    Int_t nHitsMc = (pReco > 0.) ? fMon->GetNHitsProton() : fMon->GetNHitsPion();

    Double_t momRes = TMath::Abs(pReco * sign - pMc) / pMc;

    Double_t lostOrWrongConnectedHits = 1. * TMath::Abs(nHitsReco - nHitsMc) / nHitsMc;

    if (momRes < momResCut && lostOrWrongConnectedHits <= lostOrWrongConnectedHitsCut)
        return kTRUE;
    else
        return kFALSE;
}

Bool_t BmnLambdaEmbeddingQa::wasReconstructed(BmnGlobalTrack* glTrack, Double_t momResCut) {
    Double_t pReco = 1. / glTrack->GetParamFirst()->GetQp();

    Int_t sign = (pReco > 0.) ? +1. : -1.;

    Double_t pMc = (pReco > 0.) ? fMon->GetProtonP() : fMon->GetPionP();

    Double_t momRes = TMath::Abs(pReco * sign - pMc) / pMc;

    if (momRes < momResCut)
        return kTRUE;
    else
        return kFALSE;
}

void BmnLambdaEmbeddingQa::DoDigiAnalysis() {
    histoMan->SetActive(0);

    TH1F***** hGemStripInfo = histoMan->GetGemStripInfo();
    TH1F***** hSiliconStripInfo = histoMan->GetSiliconStripInfo();

    if (nInputs != -1)
        for (Int_t iInput = 0; iInput < nInputs; iInput++) {

            TChain* emb = new TChain("bmndata");
            emb->Add(fEmb[iInput].Data());
            emb->SetBranchAddress("EmbeddingMonitor.", &fMon);

            TChain* digi = new TChain("bmndata");
            digi->Add(fDigi[iInput].Data());
            digi->SetBranchAddress("BmnEventHeader.", &hDigi);
            digi->SetBranchAddress("GEM", &fGemDigits);
            digi->SetBranchAddress("SILICON", &fSiliconDigits);

            for (Int_t iEmb = 0; iEmb < emb->GetEntries(); iEmb++) {
                emb->GetEntry(iEmb);

                if (iEmb % 1000 == 0)
                    cout << "Event# " << iEmb << endl;

                if (!fMon->IsEmbedded())
                    continue;

                Int_t store = fMon->GetStoreVertexEvent()[0];
                Int_t vertex = fMon->GetStoreVertexEvent()[1];
                Int_t event = fMon->GetStoreVertexEvent()[2];

                TChain* lambda = new TChain("bmndata");
                lambda->Add(Form("%s/lambda%d_vertex%d.root", fPath[iInput].Data(), store, vertex));

                lambda->SetBranchAddress("BmnGemStripDigit", &fGemDigits);
                lambda->SetBranchAddress("BmnSiliconDigit", &fSiliconDigits);

                lambda->GetEntry(event);

                // Looping over GEM-digits ...
                for (Int_t iDig = 0; iDig < fGemDigits->GetEntriesFast(); iDig++) {
                    BmnStripDigit* dig = (BmnStripDigit*) fGemDigits->UncheckedAt(iDig);

                    Int_t stat = dig->GetStation();
                    Int_t mod = dig->GetModule();
                    Int_t lay = dig->GetStripLayer();
                    Int_t strip = dig->GetStripNumber();

                    hGemStripInfo[0][stat][mod][lay]->Fill(strip);
                }

                // Looping over SILICON-digits ...
                for (Int_t iDig = 0; iDig < fSiliconDigits->GetEntriesFast(); iDig++) {
                    BmnStripDigit* dig = (BmnStripDigit*) fSiliconDigits->UncheckedAt(iDig);

                    Int_t stat = dig->GetStation();
                    Int_t mod = dig->GetModule();
                    Int_t lay = dig->GetStripLayer();
                    Int_t strip = dig->GetStripNumber();

                    hSiliconStripInfo[0][stat][mod][lay]->Fill(strip);
                }

                delete lambda;

                // Analyzing events with embedded digits ...
                UInt_t idEmb = fMon->GetEventId();

                for (Int_t iEvent = idEmb - 1; iEvent < idEmb + 1; iEvent++) {
                    digi->GetEntry(iEvent);

                    for (Int_t iDigi = 0; iDigi < fGemDigits->GetEntriesFast(); iDigi++) {
                        BmnStripDigit* dig = (BmnStripDigit*) fGemDigits->UncheckedAt(iDigi);

                        Int_t stat = dig->GetStation();
                        Int_t mod = dig->GetModule();
                        Int_t lay = dig->GetStripLayer();
                        Int_t strip = dig->GetStripNumber();
                        Double_t signal = dig->GetStripSignal();

                        // Skipping digits not satisfied the signal window chosen ...
                        if (signal < fSignalWindow[GEM][0] || signal > fSignalWindow[GEM][1])
                            continue;

                        hGemStripInfo[1][stat][mod][lay]->Fill(strip);
                    }

                    for (Int_t iDigi = 0; iDigi < fSiliconDigits->GetEntriesFast(); iDigi++) {
                        BmnStripDigit* dig = (BmnStripDigit*) fSiliconDigits->UncheckedAt(iDigi);

                        Int_t stat = dig->GetStation();
                        Int_t mod = dig->GetModule();
                        Int_t lay = dig->GetStripLayer();
                        Int_t strip = dig->GetStripNumber();
                        Double_t signal = dig->GetStripSignal();

                        // Skipping digits not satisfied the signal window chosen ...
                        if (signal < fSignalWindow[SILICON][0] || signal > fSignalWindow[SILICON][1])
                            continue;

                        hSiliconStripInfo[1][stat][mod][lay]->Fill(strip);
                    }
                }
            }

            delete emb;
            delete digi;
        }
}

void BmnLambdaEmbeddingQa::DoInnerTrackerRecoEfficiency() {
    hProtonMomentaEmb = new TH1F("pMomentaEmb", "pMomentaEmb", 100, 1., 7.);
    hPionMomentaEmb = new TH1F("pionMomentaEmb", "pionMomentaEmb", 100, 0., 2.);

    hProtonMomentaReco = new TH1F("pMomentaReco", "pMomentaReco", 100, 1., 7.);
    hPionMomentaReco = new TH1F("pionMomentaReco", "pionMomentaReco", 100, 0., 2.);

    hProtonNhitsEmb = new TH1F("pNhitsEmb", "pNhitsEmb", 10, 0., 10.);
    hPionNhitsEmb = new TH1F("pionNhitsEmb", "pionNhitsEmb", 10, 0., 10.);

    hProtonNhitsReco = new TH1F("pNhitsReco", "pNhitsReco", 10, 0., 10.);
    hPionNhitsReco = new TH1F("pionNhitsReco", "pionNhitsReco", 10, 0., 10.);

    hProtonNhitsRecoAll = new TH1F("pNhitsRecoAll", "pNhitsRecoAll", 10, 0., 10.);
    hPionNhitsRecoAll = new TH1F("pionNhitsRecoAll", "pionNhitsRecoAll", 10, 0., 10.);

    const Int_t nHitsCut = 4;
    const Double_t deltaPtoP = 0.05;

    pEffProton = new TProfile("effProton vs. p", Form("effProton vs. p, nHits > %d, Delta p / p < %G", nHitsCut, deltaPtoP), 100, 1., 7.);
    pEffPion = new TProfile("effPion vs. p", Form("effPion vs. p, nHits > %d, Delta p / p < %G", nHitsCut, deltaPtoP), 100, 0., 2.);

    if (nInputs != -1)
        for (Int_t iInput = 0; iInput < nInputs; iInput++) {
            TChain* emb = new TChain("bmndata");
            emb->Add(fEmb[iInput].Data());

            TChain* dst = new TChain("bmndata");
            dst->Add(fDst[iInput].Data());

            emb->SetBranchAddress("EmbeddingMonitor.", &fMon);
            dst->SetBranchAddress("DstEventHeader.", &hDst);
            dst->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);
            dst->SetBranchAddress("BmnGemStripHit", &fGemHits);
            dst->SetBranchAddress("BmnSiliconHit", &fSiliconHits);

            for (Int_t iEmb = 0; iEmb < emb->GetEntries(); iEmb++) {
                emb->GetEntry(iEmb);

                if (iEmb % 1000 == 0)
                    cout << "Event# " << iEmb << endl;

                if (!fMon->IsEmbedded())
                    continue;

                UInt_t idEmb = fMon->GetEventId();

                for (Int_t iDst = idEmb - 1; iDst < idEmb + 1; iDst++) {

                    dst->GetEntry(iDst);

                    if (hDst->GetEventId() != idEmb)
                        continue;

                    // Get event info ...
                    Int_t store = fMon->GetStoreVertexEvent()[0];
                    Int_t vertex = fMon->GetStoreVertexEvent()[1];
                    Int_t event = fMon->GetStoreVertexEvent()[2];

                    // Open lambda file ...
                    TChain* lambda = new TChain("bmndata");
                    lambda->Add(Form("%s/lambda%d_vertex%d.root", fPath[iInput].Data(), store, vertex));

                    lambda->SetBranchAddress("StsPoint", &fGemPoints);
                    lambda->SetBranchAddress("SiliconPoint", &fSiliconPoints);
                    lambda->SetBranchAddress("MCTrack", &fMcTracks);

                    lambda->GetEntry(event);

                    // [0] --> GEM, [1] --> SILICON
                    Int_t nRecoHitsProton[2] = {0, 0};
                    Int_t nRecoHitsPion[2] = {0, 0};

                    // Loop over GEM-points and reconstructed hits
                    for (Int_t iPoint = 0; iPoint < fGemPoints->GetEntriesFast(); iPoint++) {
                        CbmStsPoint* point = (CbmStsPoint*) fGemPoints->UncheckedAt(iPoint);

                        Bool_t isProton = kFALSE;
                        Bool_t isPion = kFALSE;

                        for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
                            BmnHit* hit = (BmnHit*) fGemHits->UncheckedAt(iHit);

                            if (hit->GetStation() != point->GetStation())
                                continue;

                            Double_t dist = Abs(hit->GetX() - point->GetXIn());

                            if (dist < 0.5) {
                                // Probably this hit corresponds to the considering MC-point ...
                                // Here we should decide whether we consider proton or pion right now ...
                                CbmMCTrack* track = (CbmMCTrack*) fMcTracks->UncheckedAt(point->GetTrackID());
                                if (track->GetPdgCode() > 0)
                                    isProton = kTRUE;
                                else
                                    isPion = kTRUE;

                                break;
                            }
                        }

                        if (isProton)
                            nRecoHitsProton[0]++;
                        else if (isPion)
                            nRecoHitsPion[0]++;
                    }

                    // Loop over SILICON-points and reconstructed hits
                    for (Int_t iPoint = 0; iPoint < fSiliconPoints->GetEntriesFast(); iPoint++) {
                        BmnSiliconPoint* point = (BmnSiliconPoint*) fSiliconPoints->UncheckedAt(iPoint);

                        Bool_t isProton = kFALSE;
                        Bool_t isPion = kFALSE;

                        for (Int_t iHit = 0; iHit < fSiliconHits->GetEntriesFast(); iHit++) {
                            BmnHit* hit = (BmnHit*) fSiliconHits->UncheckedAt(iHit);

                            if (hit->GetStation() != point->GetStation())
                                continue;

                            Double_t dist = Abs(hit->GetX() - point->GetX());

                            if (dist < 0.3) {
                                // Probably this hit corresponds to the considering MC-point ...
                                // Here we should decide whether we consider proton or pion right now ...
                                CbmMCTrack* track = (CbmMCTrack*) fMcTracks->UncheckedAt(point->GetTrackID());
                                if (track->GetPdgCode() > 0)
                                    isProton = kTRUE;
                                else
                                    isPion = kTRUE;

                                break;
                            }
                        }

                        if (isProton)
                            nRecoHitsProton[1]++;
                        else if (isPion)
                            nRecoHitsPion[1]++;
                    }

                    hProtonMomentaEmb->Fill(fMon->GetProtonP());
                    hPionMomentaEmb->Fill(fMon->GetPionP());

                    hProtonNhitsEmb->Fill(fMon->GetNHitsProton());
                    hPionNhitsEmb->Fill(fMon->GetNHitsPion());

                    hProtonNhitsRecoAll->Fill(nRecoHitsProton[0] + nRecoHitsProton[1]);
                    hPionNhitsRecoAll->Fill(nRecoHitsPion[0] + nRecoHitsPion[1]);

                    // Loop over reconstructed glob. tracks ...
                    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobTracks->GetEntriesFast(); iGlobTrack++) {
                        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iGlobTrack);

                        Double_t p = glTrack->GetP();
                        Int_t nHits = glTrack->GetNHits();

                        if (p > 0.) {
                            hProtonMomentaReco->Fill(p);
                            hProtonNhitsReco->Fill(nHits);
                        } else {
                            hPionMomentaReco->Fill(-p);
                            hPionNhitsReco->Fill(nHits);
                        }

                        // Efficiency of track reconstruction ...
                        // Looking for a reconstr. track to be a proton ...
                        if (p > 0. && Abs((p - fMon->GetProtonP()) / p) < deltaPtoP && nHits > nHitsCut)
                            pEffProton->Fill(p, (1. * nHits) / fMon->GetNHitsProton());

                        if (p < 0. && Abs((-p - fMon->GetPionP()) / -p) < deltaPtoP && nHits > nHitsCut)
                            pEffPion->Fill(-p, (1. * nHits) / fMon->GetNHitsPion());
                    }
                    delete lambda;
                }
            }
            delete emb;
            delete dst;
        }

    // DrawHistos3();
}

void BmnLambdaEmbeddingQa::DoDrawEventsWithEmbeddedSignals() {
    if (nInputs != 1) {
        cout << endl;
        cout << "*****************************************************" << endl;
        cout << "*.list file contains more than one line! Exiting ...." << endl;
        cout << "*****************************************************" << endl;
        throw;
    }

    BmnGemStripStationSet* GEM = geoms->GetGemGeometry();
    BmnSiliconStationSet* SILICON = geoms->GetSiliconGeometry();

    TBox*** gemModBoxes = nullptr;
    TBox**** gemLayBoxes = nullptr;
    TPolyLine**** gemDeadZones = nullptr;
    geoms->GetGemBorders(gemModBoxes, gemLayBoxes, gemDeadZones);

    TBox*** siliconModBoxes = nullptr;
    TBox**** siliconLayBoxes = nullptr;
    TPolyLine**** siliconDeadZones = nullptr;
    geoms->GetSiliconBorders(siliconModBoxes, siliconLayBoxes, siliconDeadZones);

    TChain* emb = new TChain("bmndata");
    emb->Add(fEmb[0].Data());

    TChain* dst = new TChain("bmndata");
    dst->Add(fDst[0].Data());

    emb->SetBranchAddress("EmbeddingMonitor.", &fMon);
    dst->SetBranchAddress("DstEventHeader.", &hDst);
    dst->SetBranchAddress("BmnGemStripHit", &fGemHits);
    dst->SetBranchAddress("BmnSiliconHit", &fSiliconHits);
    dst->SetBranchAddress("BmnSiliconTrack", &fSiliconTracks);
    dst->SetBranchAddress("BmnGlobalTrack", &fGlobTracks);

    hXzMC = new TH2F("", "", 500, 0., 200., 500, -80., 20.);
    hXzReco = new TH2F("", "", 500, 0., 200., 500, -80., 20.);
    hYzMC = new TH2F("", "", 500, 0., 0., 500, 0., 0.);
    hYzReco = new TH2F("", "", 500, 0., 0., 500, 0., 0.);

    hGemXYProfiles = new TH2F*[GEM->GetNStations()];
    for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++) {
        hGemXYProfiles[iStat] = new TH2F(Form("GEM: X vs. Y, stat# %d", iStat), Form("GEM: X vs. Y, stat# %d", iStat), 500, -90., +90., 500, -10., +40.);
        hGemXYProfiles[iStat]->GetXaxis()->SetLabelSize(0.1);
        hGemXYProfiles[iStat]->GetXaxis()->SetLabelSize(0.1);
        hGemXYProfiles[iStat]->GetYaxis()->SetLabelSize(0.1);
    }

    hSiliconXYProfiles = new TH2F*[SILICON->GetNStations()];
    for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++) {
        Double_t xLow = (iStat == 0) ? -8. : (iStat == 1) ? -8. : -13.;
        Double_t xUp = -xLow;

        Double_t yLow = (iStat == 0) ? -12. : (iStat == 1) ? -13. : -20.;
        Double_t yUp = (iStat == 0) ? +3. : (iStat == 1) ? +4. : +10.;

        hSiliconXYProfiles[iStat] = new TH2F(Form("SILICON: X vs. Y, stat# %d", iStat), Form("SILICON: X vs. Y, stat# %d", iStat), 500, xLow, xUp, 500, yLow, yUp);
        hSiliconXYProfiles[iStat]->GetXaxis()->SetLabelSize(0.1);
        hSiliconXYProfiles[iStat]->GetXaxis()->SetLabelSize(0.1);
        hSiliconXYProfiles[iStat]->GetYaxis()->SetLabelSize(0.1);
    }

    if (drawFoundTracks) {
        fRunAna = new FairRunAna();

        // Read current geometry from database
        dst->GetEntry(0);
        Char_t* geoFileName = (Char_t*) "current_geo_file.root";
        Int_t res_code = UniRun::ReadGeometryFile(7, hDst->GetRunId(), geoFileName);
        if (res_code != 0) {
            cout << "Geometry file can't be read from the database" << endl;
            exit(-1);
        }

        TGeoManager::Import(geoFileName);

        // Setting appropriate mag. field
        fMagField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
        UniRun* pCurrentRun = UniRun::GetRun(7, hDst->GetRunId());
        fMagField->SetScale(*pCurrentRun->GetFieldVoltage() / 55.7);
        fRunAna->SetField(fMagField);
        fMagField->Init();

        hXzRecoFromTracks = new TH2F("", "", 500, 0., 200., 500, 0., 0.);
    }

    if (drawFoundTracks) {
        for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++)
            fSilGemZ[iStat] = SILICON->GetStation(iStat)->GetZPosition();

        for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++)
            fSilGemZ[3 + iStat] = GEM->GetStation(iStat)->GetZPosition();
    }

    // Loop over events ...
    for (Int_t iDst = 0; iDst < dst->GetEntries(); iDst++) {
        dst->GetEntry(iDst);

        hXzMC->Reset();
        hXzReco->Reset();
        hYzMC->Reset();
        hYzReco->Reset();

        if (drawFoundTracks)
            hXzRecoFromTracks->Reset();

        UInt_t idDst = hDst->GetEventId();

        for (Int_t iEmb = 0; iEmb < emb->GetEntries(); iEmb++) {
            emb->GetEntry(iEmb);

            UInt_t idEmb = fMon->GetEventId();

            if (idDst != idEmb)
                continue;

            UInt_t id = idDst;

            if (!fMon->IsEmbedded())
                continue;

            hXzMC->SetTitle(Form(" XZ SI-GEM profile, id = %d", id));
            hXzReco->SetTitle(Form("XZ SI-GEM profile, id = %d", id));
            hYzMC->SetTitle(Form(" YZ SI-GEM profile, id = %d", id));
            hYzReco->SetTitle(Form("YZ SI-GEM profile, id = %d", id));

            if (drawFoundTracks)
                DrawFoundTracks();

            // Get MC-event info from file ...
            Int_t store = fMon->GetStoreVertexEvent()[0];
            Int_t vertex = fMon->GetStoreVertexEvent()[1];
            Int_t event = fMon->GetStoreVertexEvent()[2];

            TChain* lambda = new TChain("bmndata");
            lambda->Add(Form("%s/lambda%d_vertex%d.root", fPath[0].Data(), store, vertex));

            lambda->SetBranchAddress("StsPoint", &fGemPoints);
            lambda->SetBranchAddress("SiliconPoint", &fSiliconPoints);

            lambda->GetEntry(event);

            for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++) {
                vector <pair <Double_t, Double_t>> tmp;
                gems[0][iStat] = tmp;
            }

            for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++) {
                vector <pair <Double_t, Double_t>> tmp;
                silicons[0][iStat] = tmp;
            }

            // Get MC point info (GEM) ...
            for (Int_t iPoint = 0; iPoint < fGemPoints->GetEntriesFast(); iPoint++) {
                CbmStsPoint* point = (CbmStsPoint*) fGemPoints->UncheckedAt(iPoint);
                Double_t x = point->GetXIn();
                Double_t y = point->GetYIn();
                Double_t z = point->GetZIn();
                hXzMC->Fill(z, x);
                hYzMC->Fill(z, y);

                gems[0].find(point->GetStation())->second.push_back(make_pair(x, y));
            }

            // Get MC point info (SILICON) ...
            for (Int_t iPoint = 0; iPoint < fSiliconPoints->GetEntriesFast(); iPoint++) {
                BmnSiliconPoint* point = (BmnSiliconPoint*) fSiliconPoints->UncheckedAt(iPoint);
                Double_t x = point->GetX();
                Double_t y = point->GetY();
                Double_t z = point->GetZ();
                hXzMC->Fill(z, x);
                hYzMC->Fill(z, y);

                silicons[0].find(point->GetStation())->second.push_back(make_pair(x, y));
            }

            for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++) {
                vector <pair <Double_t, Double_t>> tmp;
                gems[1][iStat] = tmp;
            }

            for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++) {
                vector <pair <Double_t, Double_t>> tmp;
                silicons[1][iStat] = tmp;
            }

            // Get GEM hit info ...
            if (fGemHits)
                for (Int_t iHit = 0; iHit < fGemHits->GetEntriesFast(); iHit++) {
                    BmnGemStripHit* hit = (BmnGemStripHit*) fGemHits->UncheckedAt(iHit);

                    Double_t x = hit->GetX();
                    Double_t y = hit->GetY();
                    Double_t z = hit->GetZ();
                    hXzReco->Fill(z, x);
                    hYzReco->Fill(z, y);

                    gems[1].find(hit->GetStation())->second.push_back(make_pair(x, y));
                }

            // Get SILICON hit info ...
            if (fSiliconHits)
                for (Int_t iHit = 0; iHit < fSiliconHits->GetEntriesFast(); iHit++) {
                    BmnHit* hit = (BmnHit*) fSiliconHits->UncheckedAt(iHit);
                    Double_t x = hit->GetX();
                    Double_t y = hit->GetY();
                    Double_t z = hit->GetZ();
                    hXzReco->Fill(z, x);
                    hYzReco->Fill(z, y);

                    silicons[1].find(hit->GetStation())->second.push_back(make_pair(x, y));
                }

            delete lambda;

            DrawHistos1();
        }
    }
}

BmnLambdaEmbeddingQa::~BmnLambdaEmbeddingQa() {
    if (fRunAna)
        delete fRunAna;

    if (fMagField)
        delete fMagField;

    if (geoms)
        delete geoms;

    if (hXzMC)
        delete hXzMC;
    if (hXzReco)
        delete hXzReco;
    if (hYzMC)
        delete hYzMC;
    if (hYzReco)
        delete hYzReco;
    if (hGemXYProfiles)
        delete hGemXYProfiles;
    if (hSiliconXYProfiles)
        delete hSiliconXYProfiles;

    if (hProtonMomentaEmb)
        delete hProtonMomentaEmb;
    if (hPionMomentaEmb)
        delete hPionMomentaEmb;
    if (hProtonMomentaReco)
        delete hProtonMomentaReco;
    if (hPionMomentaReco)
        delete hPionMomentaReco;
    if (hProtonNhitsEmb)
        delete hProtonNhitsEmb;
    if (hPionNhitsEmb)
        delete hPionNhitsEmb;
    if (hProtonNhitsReco)
        delete hProtonNhitsReco;
    if (hPionNhitsReco)
        delete hPionNhitsReco;

    if (pEffProton)
        delete pEffProton;
    if (pEffPion)
        delete pEffPion;

    if (histoMan) {
        histoMan->ProcessHistos();
        delete histoMan;
    }
}

void BmnLambdaEmbeddingQa::DrawFoundTracks() {
    for (Int_t iGlobTrack = 0; iGlobTrack < fGlobTracks->GetEntriesFast(); iGlobTrack++) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobTracks->UncheckedAt(iGlobTrack);

        BmnKalmanFilter* kalman = new BmnKalmanFilter();

        FairTrackParam* first = glTrack->GetParamFirst();
        FairTrackParam* last_par = glTrack->GetParamLast();

        Double_t zStart = first->GetZ();
        hXzRecoFromTracks->Fill(first->GetZ(), first->GetX());

        map <Int_t, Int_t> statMod0;
        if (zStart < geoms->GetGemGeometry()->GetStation(0)->GetZPosition()) {
            BmnTrack* silTrack = (BmnTrack*) fSiliconTracks->UncheckedAt(glTrack->GetSilTrackIndex());

            for (Int_t iHit = 0; iHit < silTrack->GetNHits(); iHit++) {
                BmnHit* silHit = (BmnHit*) fSiliconHits->UncheckedAt(silTrack->GetHitIndex(iHit));
                statMod0[silHit->GetStation()] = silHit->GetModule();
            }
        }

        Double_t zFinish = last_par->GetZ();

        Int_t pdg = (first->GetQp() > 0.) ? 2212 : -211;

        // Try to locate a detector where the track has first point
        // in order to go forward by propagator ...
        Int_t idx = -1;
        if (statMod0.size() == 0)
            for (auto it : fSilGemZ) {
                if (TMath::Abs(it.second - zStart) < 1.) {
                    idx = it.first;
                    break;
                }
            } else {
            for (auto it : statMod0) {
                idx = it.first;
                break;
            }
        }

        for (Int_t iStep = idx + 1; iStep < 9; iStep++) {
            Double_t zStat = -1000.;
            if (iStep < 3) {
                for (auto it : statMod0) {
                    if (it.first != iStep)
                        continue;

                    zStat = geoms->GetSiliconGeometry()->GetStation(it.first)->GetModule(it.second)->GetZPositionRegistered();
                }
            } else
                zStat = fSilGemZ.find(iStep)->second;

            if (zStat > zFinish)
                break;

            kalman->TGeoTrackPropagate(first, zStat, pdg, nullptr, nullptr, kTRUE);
            hXzRecoFromTracks->Fill(first->GetZ(), first->GetX());
        }

        delete kalman;
    }
}

void BmnLambdaEmbeddingQa::DrawHistos1() {
    gStyle->SetOptStat(0);
    TBox*** gemModBoxes = nullptr;
    TBox**** gemLayBoxes = nullptr;
    TPolyLine**** gemDeadZones = nullptr;
    geoms->GetGemBorders(gemModBoxes, gemLayBoxes, gemDeadZones);

    TBox*** siliconModBoxes = nullptr;
    TBox**** siliconLayBoxes = nullptr;
    TPolyLine**** siliconDeadZones = nullptr;
    geoms->GetSiliconBorders(siliconModBoxes, siliconLayBoxes, siliconDeadZones);

    TCanvas* c = new TCanvas("c1", "c1", 1200, 1200);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.015);
    c->Divide(1, 2);
    TVirtualPad* pad = c->cd(1);
    pad->Divide(2, 1);
    pad->cd(1);
    hXzMC->Draw("P*");
    hXzMC->SetMarkerColor(kRed);
    hXzMC->SetMarkerSize(1.5);
    hXzMC->SetMarkerStyle(20);
    if (hXzReco->GetEntries() > 0.) {
        hXzReco->Draw("sameP*");
        hXzReco->SetMarkerColor(kBlue);
        hXzReco->SetMarkerSize(1.);
        hXzReco->SetMarkerStyle(19);
    }

    if (hXzRecoFromTracks && hXzRecoFromTracks->GetEntries() > 0.) {
        hXzRecoFromTracks->Draw("sameP*");
        hXzRecoFromTracks->SetMarkerColor(kBlack);
        hXzRecoFromTracks->SetMarkerSize(2.5);
        hXzRecoFromTracks->SetMarkerStyle(kOpenStar);
    }

    pad->cd(2);
    hYzMC->Draw("P*");
    hYzMC->SetMarkerColor(kRed);
    hYzMC->SetMarkerSize(1.5);
    hYzMC->SetMarkerStyle(20);
    if (hYzReco->GetEntries() > 0.) {
        hYzReco->Draw("sameP*");
        hYzReco->SetMarkerColor(kBlue);
        hYzReco->SetMarkerSize(1.);
        hYzReco->SetMarkerStyle(19);
    }

    pad = c->cd(2);
    pad->Divide(3, 3);

    for (Int_t iStat = 0; iStat < geoms->GetGemGeometry()->GetNStations(); iStat++) {
        pad->cd(iStat + 1);

        hGemXYProfiles[iStat]->Draw();

        Int_t nHitsPerStat = gems[0].find(iStat)->second.size();
        for (Int_t iHit = 0; iHit < nHitsPerStat; iHit++) {
            TMarker* marker = new TMarker(gems[0].find(iStat)->second[iHit].first, gems[0].find(iStat)->second[iHit].second, 19);
            marker->SetMarkerColor(kRed);
            marker->SetMarkerSize(1.5);
            marker->Draw();
        }

        nHitsPerStat = gems[1].find(iStat)->second.size();
        for (Int_t iHit = 0; iHit < nHitsPerStat; iHit++) {
            TMarker* marker = new TMarker(gems[1].find(iStat)->second[iHit].first, gems[1].find(iStat)->second[iHit].second, 19);
            marker->SetMarkerColor(kBlue);
            marker->SetMarkerSize(.8);
            marker->Draw();
        }

        for (Int_t iMod = 0; iMod < geoms->GetGemGeometry()->GetStation(iStat)->GetNModules(); iMod++) {
            gemModBoxes[iStat][iMod]->Draw("l");

            for (Int_t iLayer = 0; iLayer < geoms->GetGemGeometry()->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLayer++) {
                gemLayBoxes[iStat][iMod][iLayer]->Draw("l");
                gemDeadZones[iStat][iMod][iLayer]->Draw("l");
            }
        }
    }

    for (Int_t iStat = 0; iStat < geoms->GetSiliconGeometry()->GetNStations(); iStat++) {
        pad->cd(iStat + geoms->GetGemGeometry()->GetNStations() + 1);

        hSiliconXYProfiles[iStat]->Draw();

        Int_t nHitsPerStat = silicons[0].find(iStat)->second.size();
        for (Int_t iHit = 0; iHit < nHitsPerStat; iHit++) {
            TMarker* marker = new TMarker(silicons[0].find(iStat)->second[iHit].first, silicons[0].find(iStat)->second[iHit].second, 19);
            marker->SetMarkerColor(kRed);
            marker->SetMarkerSize(1.5);
            marker->Draw();
        }

        nHitsPerStat = silicons[1].find(iStat)->second.size();
        for (Int_t iHit = 0; iHit < nHitsPerStat; iHit++) {
            TMarker* marker = new TMarker(silicons[1].find(iStat)->second[iHit].first, silicons[1].find(iStat)->second[iHit].second, 19);
            marker->SetMarkerColor(kBlue);
            marker->SetMarkerSize(.8);
            marker->Draw();
        }

        for (Int_t iMod = 0; iMod < geoms->GetSiliconGeometry()->GetStation(iStat)->GetNModules(); iMod++) {
            siliconModBoxes[iStat][iMod]->Draw("l");

            for (Int_t iLayer = 0; iLayer < geoms->GetSiliconGeometry()->GetStation(iStat)->GetModule(iMod)->GetNStripLayers(); iLayer++) {
                siliconLayBoxes[iStat][iMod][iLayer]->Draw("l");

                if (iStat == 2 && iMod == 7 && (iLayer == 0 || iLayer == 2))
                    siliconDeadZones[iStat][iMod][iLayer]->Draw("l");
            }
        }
    }
    c->SaveAs("tmp.pdf");

    delete c;

    getchar();
}

void BmnLambdaEmbeddingQa::DrawHistos6(TClonesArray* map1, TClonesArray* map2) {
    // map1 - main part
    // map2 - common ADC

    const Int_t nZones = 2; // main zone and common ADC

    hStripChannel = new TH2F***[nZones];
    TBox**** boundBoxesStrips = new TBox***[nZones];
    TBox**** boundBoxesChannels = new TBox***[nZones];

    // Creating necessary histograms and help boxes on histograms...
    vector <Int_t> naturalGemOrder{11, 10, 5, 6, 8, 9};

    for (Int_t iZone = 0; iZone < nZones; iZone++) {
        const Int_t nStats = geoms->GetGemGeometry()->GetNStations();
        boundBoxesStrips[iZone] = new TBox**[nStats];
        boundBoxesChannels[iZone] = new TBox**[nStats];
        hStripChannel[iZone] = new TH2F**[nStats];

        for (Int_t iStat = 0; iStat < nStats; iStat++) {

            const Int_t nMaps = 2; // Right, Left
            hStripChannel[iZone][iStat] = new TH2F*[nMaps];
            boundBoxesStrips[iZone][iStat] = new TBox*[nMaps];
            boundBoxesChannels[iZone][iStat] = new TBox*[nMaps];

            for (Int_t iMap = 0; iMap < nMaps; iMap++) {

                TString mapping = (iMap == 0) ? "Left Part" : "Right Part";

                hStripChannel[iZone][iStat][iMap] = new TH2F(Form("Zone# %d Stat# %d Map# %d", iZone, iStat, iMap),
                        Form("Stat# %d (GEM %d), %s", iStat, naturalGemOrder[iStat], mapping.Data()), 2060, 0., 2060., 1200, 0., 1200.);
                hStripChannel[iZone][iStat][iMap]->GetXaxis()->SetLabelSize(0.1);
                hStripChannel[iZone][iStat][iMap]->GetYaxis()->SetLabelSize(0.1);
                hStripChannel[iZone][iStat][iMap]->GetXaxis()->SetTitle("Channel#");
                hStripChannel[iZone][iStat][iMap]->GetXaxis()->SetTitleOffset(-.28);
                hStripChannel[iZone][iStat][iMap]->GetXaxis()->SetTitleSize(0.15);
                hStripChannel[iZone][iStat][iMap]->GetXaxis()->CenterTitle();

                hStripChannel[iZone][iStat][iMap]->GetYaxis()->SetTitle("Strip#");
                hStripChannel[iZone][iStat][iMap]->GetYaxis()->SetTitleOffset(-.15);
                hStripChannel[iZone][iStat][iMap]->GetYaxis()->SetTitleSize(0.15);
                hStripChannel[iZone][iStat][iMap]->GetYaxis()->CenterTitle();

                Int_t low = (iMap == 0) ? 985 : 1024;
                Int_t up = (iMap == 0) ? 1080 : 1129;

                boundBoxesStrips[iZone][iStat][iMap] = new TBox(0., low, 2048., up);
                boundBoxesStrips[iZone][iStat][iMap]->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
                boundBoxesStrips[iZone][iStat][iMap]->SetFillStyle(3001);

                boundBoxesChannels[iZone][iStat][iMap] = new TBox(0., 0., 0., 1200.);
                boundBoxesChannels[iZone][iStat][iMap]->SetFillColorAlpha(TColor::GetColor("#cccccc"), 0.3);
                boundBoxesChannels[iZone][iStat][iMap]->SetFillStyle(3001);
            }
        }
    }

    // Filling histograms ...

    // Main part of big zone [0] ...
    for (Int_t iInfo = 0; iInfo < map1->GetEntriesFast(); iInfo++) {
        MappingInfo* info = (MappingInfo*) map1->UncheckedAt(iInfo);

        Int_t stat = info->station;
        TString mapping = info->mapFile;

        Int_t ch = info->channel;
        Int_t str = info->strip;

        Int_t mapIdx = (mapping.Contains("Left")) ? 0 : 1;

        hStripChannel[0][stat][mapIdx]->Fill(ch, str);
    }

    // Common ADC [1] ...
    for (Int_t iInfo = 0; iInfo < map2->GetEntriesFast(); iInfo++) {
        MappingInfo* info = (MappingInfo*) map2->UncheckedAt(iInfo);

        Int_t stat = info->station;
        TString mapping = info->mapFile;

        Int_t mapIdx = (mapping.Contains("Left")) ? 0 : 1;

        map <Int_t, Int_t> stripGlobChan = info->stripChan;

        if ((stat == 0 || stat == 1) && mapIdx == 1)
            continue;

        for (auto it : stripGlobChan)
            hStripChannel[1][stat][mapIdx]->Fill(it.second, it.first);

        boundBoxesChannels[1][stat][mapIdx]->SetX1(info->channels.first);
        boundBoxesChannels[1][stat][mapIdx]->SetX2(info->channels.second);
    }

    TCanvas* c = new TCanvas("c1", "c1", 1200, 800);
    c->Divide(2, 3);

    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.0);

    const Int_t nPads = 6;
    Int_t statsC[nPads] = {0, 0, 3, 3, 5, 5}; // LeftToRight
    Int_t statsD[nPads] = {1, 1, 2, 2, 4, 4}; // RightToLeft

    for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
        TVirtualPad* vPadC = c->cd(iPad);
        vPadC->Divide(1, 2, 0.01, 0.01);

        vPadC->cd(1);
        Int_t mapping = 0;

        Int_t stat = (iPad % 2 == 1) ? statsC[iPad - 1] : statsD[iPad - 1];

        hStripChannel[0][stat][mapping]->Draw("");
        hStripChannel[1][stat][mapping]->Draw("same");
        boundBoxesStrips[1][stat][mapping]->Draw("l");
        boundBoxesChannels[1][stat][mapping]->Draw("l");

        vPadC->cd(2);
        mapping = 1;

        hStripChannel[0][stat][mapping]->Draw("");
        hStripChannel[1][stat][mapping]->Draw("same");
        boundBoxesStrips[1][stat][mapping]->Draw("l");
        boundBoxesChannels[1][stat][mapping]->Draw("l");
    }

    c->SaveAs("strChan.pdf");

    delete c;
}
