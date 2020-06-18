#include "BmnLambdaEmbeddingDrawHistos.h"

BmnLambdaEmbeddingDrawHistos::BmnLambdaEmbeddingDrawHistos() :
geoms(nullptr),
hGemStripInfo(nullptr),
hSiliconStripInfo(nullptr),
pEmbGemHitEff(nullptr),
pEmbSilHitEff(nullptr),
pEffGemStatXY(nullptr),
pEffSilStatXY(nullptr),
hEtaLambda(nullptr),
hNhitsEmbReco(nullptr) {

    const Int_t nActiveAnals = 2;
    for (Int_t iAnal = 0; iAnal < nActiveAnals; iAnal++)
        isActive[iAnal] = kFALSE;

    // Get inn. tracker geometry ...
    geoms = new BmnInnerTrackerGeometryDraw();

    vector <Int_t> naturalGemOrder{11, 10, 5, 6, 8, 9};

    const Int_t nAnalTypes = 2; // mc, afterEmbedding

    enum analType {
        pureMc, afterEmbedding
    };

    // Histograms for BmnLambdaEmbeddingQa::DoDigiAnalysis()

    hGemStripInfo = new TH1F****[nAnalTypes];
    hSiliconStripInfo = new TH1F****[nAnalTypes];

    for (Int_t iType = 0; iType < nAnalTypes; iType++) {
        const Int_t nStatsGem = geoms->GetGemGeometry()->GetNStations();
        const Int_t nStatsSil = geoms->GetSiliconGeometry()->GetNStations();

        hGemStripInfo[iType] = new TH1F***[nStatsGem];
        hSiliconStripInfo[iType] = new TH1F***[nStatsSil];

        TString type = (iType == pureMc) ? "MC-digits" : "Digits after embedding";

        for (Int_t iStat = 0; iStat < nStatsGem; iStat++) {
            Int_t nMods = geoms->GetGemGeometry()->GetStation(iStat)->GetNModules();
            hGemStripInfo[iType][iStat] = new TH1F**[nMods];

            for (Int_t iMod = 0; iMod < nMods; iMod++) {
                Int_t nLays = geoms->GetGemGeometry()->GetStation(iStat)->GetModule(iMod)->GetNStripLayers();
                hGemStripInfo[iType][iStat][iMod] = new TH1F*[nLays];

                for (Int_t iLay = 0; iLay < nLays; iLay++) {
                    hGemStripInfo[iType][iStat][iMod][iLay] = new TH1F(Form("GEM, Type# %d Stat# %d Mod# %d Lay# %d", iType, iStat, iMod, iLay),
                            Form("%s, Stat# %d (GEM %d), Mod# %d", type.Data(), iStat, naturalGemOrder[iStat], iMod), 1200, 0., 1200.);
                    hGemStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetLabelSize(0.1);
                    hGemStripInfo[iType][iStat][iMod][iLay]->GetYaxis()->SetLabelSize(0.1);
                    hGemStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetTitle("Strips#");
                    hGemStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetTitleOffset(-.28);
                    hGemStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetTitleSize(0.15);
                    hGemStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->CenterTitle();

                }
            }
        }

        for (Int_t iStat = 0; iStat < nStatsSil; iStat++) {
            Int_t nMods = geoms->GetSiliconGeometry()->GetStation(iStat)->GetNModules();
            hSiliconStripInfo[iType][iStat] = new TH1F**[nMods];

            for (Int_t iMod = 0; iMod < nMods; iMod++) {
                Int_t nLays = geoms->GetSiliconGeometry()->GetStation(iStat)->GetModule(iMod)->GetNStripLayers();
                hSiliconStripInfo[iType][iStat][iMod] = new TH1F*[nLays];

                for (Int_t iLay = 0; iLay < nLays; iLay++) {
                    hSiliconStripInfo[iType][iStat][iMod][iLay] = new TH1F(Form("SILICON, Type# %d Stat# %d Mod# %d Lay# %d", iType, iStat, iMod, iLay),
                            Form("%s, Stat# %d, Mod# %d", type.Data(), iStat, iMod), 700, 0., 700.);
                    hSiliconStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetLabelSize(0.1);
                    hSiliconStripInfo[iType][iStat][iMod][iLay]->GetYaxis()->SetLabelSize(0.1);
                    hSiliconStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetTitle("Strips#");
                    hSiliconStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetTitleOffset(-.28);
                    hSiliconStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->SetTitleSize(0.15);
                    hSiliconStripInfo[iType][iStat][iMod][iLay]->GetXaxis()->CenterTitle();
                }
            }
        }
    }

    // Histograms for BmnLambdaEmbeddingQa::DoEffAnalysis()

    BmnGemStripStationSet* GEM = geoms->GetGemGeometry();
    BmnSiliconStationSet* SILICON = geoms->GetSiliconGeometry();

    const Int_t nSilModsMax = 8;
    pEmbSilHitEff = new TProfile2D("SILICON eff.", "SILICON eff.", SILICON->GetNStations(), 0., SILICON->GetNStations(), nSilModsMax, 0, nSilModsMax, 0., 1.);

    const Int_t nZones = 2; // 0 --> hot, 1 --> big
    const Int_t nGemModsMax = 2; // mod0 , mod1
    pEmbGemHitEff = new TProfile2D*[nZones];
    TAxis* xAxis = pEmbSilHitEff->GetXaxis();
    TAxis* yAxis = pEmbSilHitEff->GetYaxis();

    for (Int_t iBin = 0; iBin < SILICON->GetNStations(); iBin++)
        xAxis->SetBinLabel(iBin + 1, Form("Stat# %d", iBin));
    for (Int_t iBin = 0; iBin < nSilModsMax; iBin++)
        yAxis->SetBinLabel(iBin + 1, Form("Mod# %d", iBin));
    pEmbSilHitEff->GetXaxis()->SetLabelSize(0.07);
    pEmbSilHitEff->GetYaxis()->SetLabelSize(0.07);

    pEmbSilHitEff->GetXaxis()->SetTitleSize(0.06);
    pEmbSilHitEff->GetYaxis()->SetTitleSize(0.06);

    pEmbSilHitEff->GetXaxis()->SetTitleOffset(.83);
    pEmbSilHitEff->GetYaxis()->SetTitleOffset(.77);

    pEmbSilHitEff->SetMarkerSize(2.3);

    for (Int_t iZone = 0; iZone < nZones; iZone++) {
        TString zone = (iZone == 0) ? "Hot Zone" : "Big Zone";
        pEmbGemHitEff[iZone] = new TProfile2D(Form("GEM eff., %s", zone.Data()),
                Form("GEM, %s", zone.Data()), GEM->GetNStations(), 0., GEM->GetNStations(), nGemModsMax, 0., nGemModsMax, 0., 1.);

        xAxis = pEmbGemHitEff[iZone]->GetXaxis();
        yAxis = pEmbGemHitEff[iZone]->GetYaxis();

        for (Int_t iBin = 0; iBin < GEM->GetNStations(); iBin++)
            xAxis->SetBinLabel(iBin + 1, Form("Stat# %d", iBin));
        for (Int_t iBin = 0; iBin < nGemModsMax; iBin++)
            yAxis->SetBinLabel(iBin + 1, Form("Mod# %d", iBin));

        pEmbGemHitEff[iZone]->GetXaxis()->SetLabelSize(0.07);
        pEmbGemHitEff[iZone]->GetYaxis()->SetLabelSize(0.07);

        pEmbGemHitEff[iZone]->GetXaxis()->SetTitleSize(0.06);
        pEmbGemHitEff[iZone]->GetYaxis()->SetTitleSize(0.06);

        pEmbGemHitEff[iZone]->GetXaxis()->SetTitleOffset(.43);
        pEmbGemHitEff[iZone]->GetYaxis()->SetTitleOffset(.77);

        pEmbGemHitEff[iZone]->SetMarkerSize(2.3);
    }

    pEffGemStatXY = new TProfile2D*[GEM->GetNStations()];
    for (Int_t iStat = 0; iStat < GEM->GetNStations(); iStat++) {
        pEffGemStatXY[iStat] = new TProfile2D(Form("GEM eff. vs. X and Y, stat# %d", iStat),
                Form("Stat# %d (GEM %d); X [cm]; Y [cm]", iStat, naturalGemOrder[iStat]), 500, -100., +100, 500, -10., +50., 0, 1);
        pEffGemStatXY[iStat]->GetXaxis()->SetLabelSize(0.07);
        pEffGemStatXY[iStat]->GetYaxis()->SetLabelSize(0.07);

        pEffGemStatXY[iStat]->GetXaxis()->SetTitleSize(0.06);
        pEffGemStatXY[iStat]->GetYaxis()->SetTitleSize(0.06);

        pEffGemStatXY[iStat]->GetXaxis()->SetTitleOffset(.83);
        pEffGemStatXY[iStat]->GetYaxis()->SetTitleOffset(.77);
    }

    pEffSilStatXY = new TProfile2D*[SILICON->GetNStations()];
    for (Int_t iStat = 0; iStat < SILICON->GetNStations(); iStat++) {
        Double_t xLow = (iStat == 0) ? -8. : (iStat == 1) ? -8. : -13.;
        Double_t xUp = -xLow;

        Double_t yLow = (iStat == 0) ? -12. : (iStat == 1) ? -13. : -20.;
        Double_t yUp = (iStat == 0) ? +3. : (iStat == 1) ? +4. : +10.;

        pEffSilStatXY[iStat] = new TProfile2D(Form("SILICON eff. vs. X and Y, stat# %d", iStat),
                Form("Stat# %d; X [cm]; Y [cm]", iStat), 500, xLow, xUp, 500, yLow, yUp, 0., 1.);
        pEffSilStatXY[iStat]->GetXaxis()->SetLabelSize(0.07);
        pEffSilStatXY[iStat]->GetYaxis()->SetLabelSize(0.07);

        pEffSilStatXY[iStat]->GetXaxis()->SetTitleSize(0.06);
        pEffSilStatXY[iStat]->GetYaxis()->SetTitleSize(0.06);

        pEffSilStatXY[iStat]->GetXaxis()->SetTitleOffset(.83);
        pEffSilStatXY[iStat]->GetYaxis()->SetTitleOffset(.77);
    }

    const Int_t nParts = 2; // proton, pion
    const TString particles[nParts] = {"Protons", "Pions"};
    const Int_t nCuts = 4;

    hEtaLambda = new TH1F**[nParts];
    hNhitsEmbReco = new TH2F*[nParts];
    
    for (Int_t iPart = 0; iPart < nParts; iPart++) {
        hEtaLambda[iPart] = new TH1F*[nCuts];
        
        hNhitsEmbReco[iPart] = new TH2F(Form("%s", particles[iPart].Data()),
                Form("%s; Nhits_{reco}; NHits_{emb}", particles[iPart].Data()), 6, 4., 10., 6, 4., 10.);
        hNhitsEmbReco[iPart]->GetXaxis()->SetLabelSize(0.07);
        hNhitsEmbReco[iPart]->GetYaxis()->SetLabelSize(0.07);

        hNhitsEmbReco[iPart]->GetXaxis()->SetTitleSize(0.07);
        hNhitsEmbReco[iPart]->GetYaxis()->SetTitleSize(0.07);

        hNhitsEmbReco[iPart]->GetXaxis()->SetTitleOffset(.53);
        hNhitsEmbReco[iPart]->GetYaxis()->SetTitleOffset(.77);

        // FIXME!
        const Int_t nBinsX = 16;
        const Double_t xLow = 1.;
        const Double_t xUp = 3.;

        for (Int_t iCut = 0; iCut < nCuts; iCut++) {
            hEtaLambda[iPart][iCut] = new TH1F(Form("%s %d", particles[iPart].Data(), iCut),
                    Form("%s; #eta_{#Lambda^{0}}; N_{reco} / N_{emb}", particles[iPart].Data()), nBinsX, xLow, xUp);
            hEtaLambda[iPart][iCut]->GetXaxis()->SetLabelSize(0.07);
            hEtaLambda[iPart][iCut]->GetYaxis()->SetLabelSize(0.07);

            hEtaLambda[iPart][iCut]->GetXaxis()->SetTitleSize(0.07);
            hEtaLambda[iPart][iCut]->GetYaxis()->SetTitleSize(0.07);

            hEtaLambda[iPart][iCut]->GetXaxis()->SetTitleOffset(-.53);
            hEtaLambda[iPart][iCut]->GetYaxis()->SetTitleOffset(.77);

            hEtaLambda[iPart][iCut]->GetYaxis()->SetRangeUser(0., 1.5);
        }
    }
}

void BmnLambdaEmbeddingDrawHistos::ProcessHistos() {
    gStyle->SetOptStat(0);
    gStyle->SetTitleFontSize(0.1);
    gStyle->SetTitleY(1.0);

    // Filling histogram placement class ...
    TClonesArray* hPlace = new TClonesArray("HistoPlacement");

    // HistoPlacement(TString canvName, Int_t pad, Int_t subPad, TString det, Int_t type, Int_t stat, Int_t mod)
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 1, 1, "SILICON", 0, 0, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 1, 2, "SILICON", 0, 0, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 2, 1, "SILICON", 1, 0, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 2, 2, "SILICON", 1, 0, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 3, 1, "SILICON", 0, 0, 2);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 3, 2, "SILICON", 0, 0, 3);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 4, 1, "SILICON", 1, 0, 2);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("a", 4, 2, "SILICON", 1, 0, 3);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("b", 1, 1, "SILICON", 0, 1, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("b", 1, 2, "SILICON", 0, 1, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("b", 2, 1, "SILICON", 1, 1, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("b", 2, 2, "SILICON", 1, 1, 1);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 1, 1, "SILICON", 0, 2, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 1, 2, "SILICON", 0, 2, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 2, 1, "SILICON", 1, 2, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 2, 2, "SILICON", 1, 2, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 3, 1, "SILICON", 0, 2, 2);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 3, 2, "SILICON", 0, 2, 3);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 4, 1, "SILICON", 1, 2, 2);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("c", 4, 2, "SILICON", 1, 2, 3);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 1, 1, "SILICON", 0, 2, 4);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 1, 2, "SILICON", 0, 2, 5);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 2, 1, "SILICON", 1, 2, 4);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 2, 2, "SILICON", 1, 2, 5);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 3, 1, "SILICON", 0, 2, 6);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 3, 2, "SILICON", 0, 2, 7);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 4, 1, "SILICON", 1, 2, 6);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("d", 4, 2, "SILICON", 1, 2, 7);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 1, 1, "GEM", 0, 0, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 1, 2, "GEM", 0, 0, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 2, 1, "GEM", 1, 0, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 2, 2, "GEM", 1, 0, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 3, 1, "GEM", 0, 1, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 3, 2, "GEM", 0, 1, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 4, 1, "GEM", 1, 1, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 4, 2, "GEM", 1, 1, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 5, 1, "GEM", 0, 2, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 5, 2, "GEM", 0, 2, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 6, 1, "GEM", 1, 2, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("e", 6, 2, "GEM", 1, 2, 1);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 1, 1, "GEM", 0, 3, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 1, 2, "GEM", 0, 3, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 2, 1, "GEM", 1, 3, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 2, 2, "GEM", 1, 3, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 3, 1, "GEM", 0, 4, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 3, 2, "GEM", 0, 4, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 4, 1, "GEM", 1, 4, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 4, 2, "GEM", 1, 4, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 5, 1, "GEM", 0, 5, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 5, 2, "GEM", 0, 5, 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 6, 1, "GEM", 1, 5, 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("f", 6, 2, "GEM", 1, 5, 1);

    // HistoPlacement(TString canvName, Int_t pad, TString det, Int_t stat)
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("g", 1, "SILICON", 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("g", 2, "SILICON", 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("g", 3, "SILICON", 2);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("h", 1, "GEM", 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("h", 2, "GEM", 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("h", 3, "GEM", 2);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("h", 4, "GEM", 3);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("h", 5, "GEM", 4);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("h", 6, "GEM", 5);

    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("i", 1, "GEM", 1);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("i", 2, "GEM", 0);
    new ((*hPlace)[hPlace->GetEntriesFast()]) HistoPlacement("i", 3, "SILICON", 0);

    const Int_t nPads = 6;
    const Int_t nSubPads = 2;

    // 1. Draw histos from BmnLambdaEmbeddingQa::DoDigiAnalysis() ...
    if (isActive[0]) {
        const Int_t nCanvs = 6;
        TCanvas* a = new TCanvas("a", "a", 1200, 800);
        TCanvas* b = new TCanvas("b", "b", 1200, 800);
        TCanvas* c = new TCanvas("c", "c", 1200, 800);
        TCanvas* d = new TCanvas("d", "d", 1200, 800);
        TCanvas* e = new TCanvas("e", "e", 1200, 800);
        TCanvas* f = new TCanvas("f", "f", 1200, 800);

        // TCanvas * arrCanv[nCanvs] = {a, b, c, d, e, f};
        vector <TCanvas*> arrCanv{a, b, c, d, e, f};

        for (Int_t iCanv = 0; iCanv < nCanvs; iCanv++) {
            arrCanv[iCanv]->Divide(2, 3);
            for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
                Int_t analType = (iPad % 2 == 1) ? 0 : 1;

                TVirtualPad* vPad = arrCanv[iCanv]->cd(iPad);
                vPad->Divide(1, 2, 0.01, 0.01);

                for (Int_t iSubPad = 1; iSubPad < nSubPads + 1; iSubPad++) {
                    vPad->cd(iSubPad);

                    for (Int_t iPlace = 0; iPlace < hPlace->GetEntriesFast(); iPlace++) {

                        HistoPlacement* place = (HistoPlacement*) hPlace->UncheckedAt(iPlace);

                        TString name = place->GetCanvasName();
                        Int_t pad = place->GetPad();
                        Int_t subpad = place->GetSubpad();
                        Int_t type = place->GetAnalType();
                        TString det = place->GetDetectorName();

                        if ((TString) arrCanv[iCanv]->GetName() != name || iPad != pad || iSubPad != subpad || analType != type)
                            continue;

                        Int_t stat = place->GetStat();
                        Int_t mod = place->GetMod();

                        if (det.Contains("SILICON")) {
                            hSiliconStripInfo[type][stat][mod][0]->Draw();
                            hSiliconStripInfo[type][stat][mod][0]->SetLineColor(kRed);
                            hSiliconStripInfo[type][stat][mod][1]->Draw("same");
                            hSiliconStripInfo[type][stat][mod][1]->SetLineColor(kBlue);
                        } else if (det.Contains("GEM")) {
                            hGemStripInfo[type][stat][mod][0]->Draw();
                            hGemStripInfo[type][stat][mod][0]->SetLineColor(kRed);
                            hGemStripInfo[type][stat][mod][1]->Draw("same");
                            hGemStripInfo[type][stat][mod][1]->SetLineColor(kBlue);
                            // hGemStripInfo[type][stat][mod][2]->Draw("same");
                            // hGemStripInfo[type][stat][mod][3]->Draw("same");
                        }

                        break;
                    }
                }
            }
        }
        a->SaveAs("siliconMcEmbedded_stat0.pdf");
        b->SaveAs("siliconMcEmbedded_stat1.pdf");
        c->SaveAs("siliconMcEmbedded_stat3_1.pdf");
        d->SaveAs("siliconMcEmbedded_stat3_2.pdf");
        e->SaveAs("gemMcEmbedded_stat012.pdf");
        f->SaveAs("gemMcEmbedded_stat345.pdf");
    }

    // 2. Draw histos from BmnLambdaEmbeddingQa::DoEffAnalysis() ...
    if (isActive[1]) {
        TBox*** gemModBoxes = nullptr;
        TBox**** gemLayBoxes = nullptr;
        TPolyLine**** gemDeadZones = nullptr;
        geoms->GetGemBorders(gemModBoxes, gemLayBoxes, gemDeadZones);

        TBox*** silModBoxes = nullptr;
        TBox**** silLayBoxes = nullptr;
        TPolyLine**** silDeadZones = nullptr;
        geoms->GetSiliconBorders(silModBoxes, silLayBoxes, silDeadZones);

        Int_t nCanvs = 2;
        TCanvas* g = new TCanvas("g", "g", 1200, 800);
        TCanvas* h = new TCanvas("h", "h", 1200, 800);

        //TCanvas * arrCanv[nCanvs] = {g, h};
        vector <TCanvas*> arrCanv{g, h};
        for (Int_t iCanv = 0; iCanv < nCanvs; iCanv++) {
            arrCanv[iCanv]->Divide(2, 3);
            for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
                arrCanv[iCanv]->cd(iPad);
                for (Int_t iPlace = 0; iPlace < hPlace->GetEntriesFast(); iPlace++) {
                    HistoPlacement* place = (HistoPlacement*) hPlace->UncheckedAt(iPlace);

                    TString name = place->GetCanvasName();
                    Int_t pad = place->GetPad();
                    TString det = place->GetDetectorName();

                    if ((TString) arrCanv[iCanv]->GetName() != name || iPad != pad)
                        continue;

                    if (det.Contains("SILICON")) {
                        Int_t stat = place->GetStat();
                        pEffSilStatXY[stat]->Draw("colz");

                        for (Int_t iMod = 0; iMod < geoms->GetSiliconGeometry()->GetStation(stat)->GetNModules(); iMod++) {
                            silModBoxes[stat][iMod]->Draw("l");

                            for (Int_t iLayer = 0; iLayer < geoms->GetSiliconGeometry()->GetStation(stat)->GetModule(iMod)->GetNStripLayers(); iLayer++) {
                                silLayBoxes[stat][iMod][iLayer]->Draw("l");
                                if (stat == 2 && iMod == 7 && (iLayer == 0 || iLayer == 2))
                                    silDeadZones[stat][iMod][iLayer]->Draw("l");
                            }
                        }
                    } else if (det.Contains("GEM")) {
                        Int_t stat = place->GetStat();
                        pEffGemStatXY[stat]->Draw("colz");

                        for (Int_t iMod = 0; iMod < geoms->GetGemGeometry()->GetStation(stat)->GetNModules(); iMod++) {
                            gemModBoxes[stat][iMod]->Draw("l");

                            for (Int_t iLayer = 0; iLayer < geoms->GetGemGeometry()->GetStation(stat)->GetModule(iMod)->GetNStripLayers(); iLayer++) {
                                gemLayBoxes[stat][iMod][iLayer]->Draw("l");
                                gemDeadZones[stat][iMod][iLayer]->Draw("l");
                            }
                        }
                    }

                    break;
                }
            }
        }

        g->SaveAs("siliconEff_XY.pdf");
        h->SaveAs("gemEff_XY.pdf");

        nCanvs = 1;
        TCanvas* i = new TCanvas("i", "i", 1200, 800);
        i->Divide(2, 3);

        for (Int_t iPad = 1; iPad < nPads + 1; iPad++) {
            i->cd(iPad)->SetGrid();
            for (Int_t iPlace = 0; iPlace < hPlace->GetEntriesFast(); iPlace++) {
                HistoPlacement* place = (HistoPlacement*) hPlace->UncheckedAt(iPlace);

                TString name = place->GetCanvasName();
                Int_t pad = place->GetPad();
                TString det = place->GetDetectorName();

                if ((TString) i->GetName() != name || iPad != pad)
                    continue;

                if (det.Contains("SILICON")) {
                    pEmbSilHitEff->Draw("TEXT");
                } else if (det.Contains("GEM")) {
                    Int_t zone = place->GetZone();
                    pEmbGemHitEff[zone]->Draw("TEXT");
                }

                break;
            }
        }

        i->SaveAs("gemSiliconEff.pdf");

        TCanvas* j = new TCanvas("j", "j", 1200, 800);
        j->Divide(2, 3);

        for (Int_t iPad = 1; iPad < 3; iPad++) {
            j->cd(iPad)->SetGrid();

            hEtaLambda[iPad - 1][0]->SetMarkerStyle(kFullCircle);
            hEtaLambda[iPad - 1][0]->SetMarkerColor(kRed);
            hEtaLambda[iPad - 1][0]->Draw("P");

            hEtaLambda[iPad - 1][1]->SetMarkerStyle(kFullSquare);
            hEtaLambda[iPad - 1][1]->SetMarkerColor(kBlue);
            hEtaLambda[iPad - 1][1]->Draw("Psame");

            hEtaLambda[iPad - 1][2]->SetMarkerStyle(kFullTriangleUp);
            hEtaLambda[iPad - 1][2]->SetMarkerColor(kMagenta);
            hEtaLambda[iPad - 1][2]->Draw("Psame");

            hEtaLambda[iPad - 1][3]->SetMarkerStyle(kFullTriangleDown);
            hEtaLambda[iPad - 1][3]->Draw("Psame");

            TLegend* legend = new TLegend(0.1, 0.7, 0.58, 0.9);
            legend->AddEntry(hEtaLambda[iPad - 1][0], "nHitsReco = nHitsMc", "p");
            legend->AddEntry(hEtaLambda[iPad - 1][1], "|nHitsReco - nHitsMc| / nHitsMc < 0.2", "p");
            legend->AddEntry(hEtaLambda[iPad - 1][2], "|nHitsReco - nHitsMc| / nHitsMc < 0.4", "p");
            legend->AddEntry(hEtaLambda[iPad - 1][3], "|nHitsReco - nHitsMc| / nHitsMc < 0.5", "p");

            legend->Draw();
        }
        
        for (Int_t iPad = 3; iPad < 5; iPad++) {
            j->cd(iPad)->SetGrid();  
            j->cd(iPad)->SetLogz();
            hNhitsEmbReco[iPad - 3]->Draw("colz");
        }

        j->SaveAs("etaLambda_nReco2Emb.pdf");
    }

    delete hPlace;
}


