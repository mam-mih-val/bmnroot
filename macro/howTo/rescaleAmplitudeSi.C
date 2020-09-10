#include <vector>

#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>
#include <Rtypes.h>
#include <TTimeStamp.h>
#include <TMathBase.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TLegend.h>

//#include "../../bmndata/math/BmnMath.h"


//#include "../../bmndata/BmnStripDigit.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/**
 * rescaleAmplitudes
 * Example of ADC signal rescaling procedures for Si strip digits
 * @param fileNameMC
 * @param fileNameEx
 * @param fileNameDST
 * @param aClusterSizeThr cluster size lower threshold
 * @param aLowThr strip signal lower threshold
 */
void rescaleAmplitudeSi(TString fileNameMC, TString fileNameEx, TString fileNameDST,
        Double_t aClusterSizeThr = 1.0,
        Double_t aLowThr = 0.0) {
    if (fileNameMC == "" || fileNameEx == "") {
        cout << "Files need to be specified!" << endl;
        return;
    }
    Int_t periodId = 7;
    Int_t runId = 4649;
    gStyle->SetOptStat(0);
    Int_t ClusterSizeThr = aClusterSizeThr;
    Double_t lowThr = aLowThr;
    Int_t RescalingBins = 4e5;
    Int_t xBins = 400;
    Double_t xLow = 0.0;
    Double_t xUp = 3200.0;
    Int_t nCSize = 5;
    const UInt_t Pad_Width = 1280;
    const UInt_t Pad_Height = 720;
    vector<EColor> cols = {kBlack, kBlue, kTeal, kViolet, kOrange};

    bmnloadlibs(); // load libraries
    //    gSystem->Load("libBmnRecoTools");

    vector<vector<vector<TH1D* > > > histSiliconStrip;
    vector<vector<vector<TH1D* > > > histSiliconStripEx;
    vector<PadInfo*> canStripPads;
    UInt_t sumMods = 0;
    UInt_t maxLayers = 0;
    TString name;
    TString xmlConfFileName = periodId == 7 ? "SiliconRunSpring2018.xml" : "SiliconRunSpring2017.xml";
    xmlConfFileName = TString(getenv("VMCWORKDIR")) + "/parameters/silicon/XMLConfigs/" + xmlConfFileName;
    printf("xmlConfFileName %s\n", xmlConfFileName.Data());
    BmnSiliconStationSet* stationSet = new BmnSiliconStationSet(xmlConfFileName);
    for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
        vector<vector<TH1D*> > rowGEM;
        vector<vector<TH1D*> > rowGEM2;
        BmnSiliconStation* st = stationSet->GetStation(iStation);
        sumMods += st->GetNModules();
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            vector<TH1D*> colGEM;
            vector<TH1D*> colGEM2;
            BmnSiliconModule *mod = st->GetModule(iModule);
            Int_t nStripLayers = (iStation ? (mod->GetNStripLayers() / 2) : mod->GetNStripLayers());
            if (maxLayers < nStripLayers)
                maxLayers = nStripLayers;
            for (Int_t iLayer = 0; iLayer < nStripLayers; iLayer++) {
                BmnSiliconLayer lay = mod->GetStripLayer(iLayer);
                name = Form("Silicon_Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                TH1D *h = new TH1D(name, name, xBins, xLow, xUp);
                h->SetTitleSize(0.06, "XY");
                h->SetLabelSize(0.08, "XY");
                h->GetXaxis()->SetTitle("Signal");
                h->GetXaxis()->SetTitleColor(kOrange + 10);
                h->GetYaxis()->SetTitle("Activation Count");
                h->GetYaxis()->SetTitleColor(kOrange + 10);
                colGEM.push_back(h);
                name = Form("Silicon_Station_%d_module_%d_layer_%d_Ex", iStation, iModule, iLayer);
                TH1D *h2 = new TH1D(name, name, xBins, xLow, xUp);
                h2->SetTitleSize(0.06, "XY");
                h2->SetLabelSize(0.08, "XY");
                h2->GetXaxis()->SetTitle("Signal");
                h2->GetXaxis()->SetTitleOffset(0.5);
                h2->GetXaxis()->SetTitleColor(kOrange + 10);
                h2->GetYaxis()->SetTitle("Activation Count");
                h2->GetYaxis()->SetTitleOffset(0.5);
                h2->GetYaxis()->SetTitleColor(kOrange + 10);
                colGEM2.push_back(h2);
            }
            rowGEM.push_back(colGEM);
            rowGEM2.push_back(colGEM2);

        }
        histSiliconStrip.push_back(rowGEM);
        histSiliconStripEx.push_back(rowGEM2);

    }
    // Create canvas
    name = "Si-full";
    TCanvas *canStrip = new TCanvas(name, name, Pad_Width * maxLayers, Pad_Height * sumMods);
    canStrip->Divide(maxLayers, sumMods);


    BmnRescale* rescale = new BmnRescale(periodId, runId, lowThr, ClusterSizeThr, RescalingBins);
    rescale->CreateRescales(fileNameMC, fileNameDST);
    auto infoVecExp = rescale->GetSilInfoVectorExp();
    auto ResVec = rescale->GetSilRescaleVector();

    vector<vector<vector<TH1D* > > > hMC; // MC
    vector<vector<vector<TH1D* > > > hMCR; // MC Rescaled
    vector<vector<vector<TH1D* > > > hEx; // Exp
    vector<vector<vector<TH1D* > > > hExCS;

    // Load MC digits
    TChain* chainMC = new TChain("bmndata");
    chainMC->Add(fileNameMC.Data());
    Long64_t NEventsMC = chainMC->GetEntries();
    printf("#recorded  MC entries = %lld\n", NEventsMC);
    TClonesArray * gemMC = nullptr;
    chainMC->SetBranchAddress("BmnSiliconDigit", &gemMC);

    // Load Exp digits
    TChain* chainEx = new TChain("bmndata");
    chainEx->Add(fileNameEx.Data());
    Long64_t NEventsEx = chainEx->GetEntries();
    printf("#recorded Exp entries = %lld\n", NEventsEx);
    TClonesArray * gemEx = nullptr;
    chainEx->SetBranchAddress("SILICON", &gemEx);

    // Load tracks
    TChain* chainDST = new TChain("bmndata");
    chainDST->Add(fileNameDST.Data());
    Long64_t NEventsDST = chainDST->GetEntries();
    printf("#recorded Track entries = %lld\n", NEventsDST);
    TClonesArray * tracks = nullptr;
    TClonesArray * gemTracks = nullptr;
    TClonesArray * gemHits = nullptr;
    chainDST->SetBranchAddress("BmnGlobalTrack", &tracks);
    chainDST->SetBranchAddress("BmnSiliconTrack", &gemTracks);
    chainDST->SetBranchAddress("BmnSiliconHit", &gemHits);

    printf("rescaling func created\n");

    TString title = "Si MC  Rescaled";
    TH1D* hGemMC = new TH1D("mc", title, xBins, xLow, xUp);
    title = "Si Exp ";
    TH1D* hGemEx = new TH1D("digs", title, xBins, xLow, xUp);
    title = "Si Exp Hits' Digits";
    TH1D* hGemExHits = new TH1D("digs-hits", title, xBins, xLow, xUp);
    title = "Si Exp Cluster Signal Lower";
    TH1D* hGemExCS = new TH1D("cluster-signal-upper", title, xBins, xLow, xUp);
    title = "Si Exp Tracks' Digits";
    TH1D* hGemExTracks = new TH1D("digs-tracks", title, xBins, xLow, xUp);
    vector < TH1D* > hGemCluster;
    for (Int_t i = 0; i < nCSize; i++) {
        title = "Si Cluster Size";
        TH1D* h = new TH1D(
                Form("si csize %.d", i),
                title,
                xBins, xLow, xUp);
        hGemCluster.push_back(h);
    }



    for (Long64_t iEv = 0; iEv < NEventsEx; ++iEv) {
        DrawBar((UInt_t) iEv, (UInt_t) NEventsEx);
        chainEx->GetEntry(iEv);
        chainDST->GetEntry(iEv);
        //        /** summ strip signals */
        //
        //        /** digs all*/
        //        for (UInt_t iDig = 0; iDig < gemEx->GetEntriesFast(); iDig++) {
        //            BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(iDig);
        //            Double_t signal = dig->GetStripSignal();
        //            dig->SetName(""); // clear marks
        //            hGemEx->Fill(signal);
        //        }
        //
        /** digs from hits*/
        for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) tracks->UncheckedAt(iTrack);
            BmnTrack* subTrack = nullptr;
            if ((track->GetSilTrackIndex() != -1))
                subTrack = static_cast<BmnTrack*> (gemTracks->UncheckedAt(track->GetSilTrackIndex()));
            if (!subTrack)
                continue;
            for (Int_t iHit = 0; iHit < subTrack->GetNHits(); iHit++) {
                //        for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                BmnSiliconHit *hit = (BmnSiliconHit *) gemHits->UncheckedAt(subTrack->GetHitIndex(iHit));
                if (hit->GetFlag() == kFALSE)
                    continue;
                // fill cluster signal for different cluster sizes
                hGemCluster[0]->Fill(hit->GetStripTotalSignalInLowerLayer());
                Int_t s = hit->GetStation();
                Int_t m = hit->GetModule();
                BmnSiliconModule * mod = stationSet->GetStation(s)->GetModule(m);
                for (Int_t l = 0; l < mod->GetStripLayers().size(); l++) {
                    BmnSiliconLayer lay = mod->GetStripLayer(l);
                    Double_t x = hit->GetX();
                    Double_t y = hit->GetY();
                    if (lay.IsPointInsideStripLayer(-x, y)) {
                        hit->SetFlag(kFALSE);
                        Int_t iLayCorr = (s > 0) ? (l / 2) : l;
                        Double_t val = (lay.GetType() == UpperStripLayer) ? hit->GetStripTotalSignalInUpperLayer() : hit->GetStripTotalSignalInLowerLayer();
                        Int_t cs = (lay.GetType() == UpperStripLayer) ? hit->GetClusterSizeInUpperLayer() : hit->GetClusterSizeInLowerLayer();
                        if ((val > lowThr) && (cs > ClusterSizeThr)) {
                            if (histSiliconStripEx[s][m][iLayCorr])
                                histSiliconStripEx[s][m][iLayCorr]->Fill(val);
                            else
                                printf("WTF? histStripEx[%d][%d][%d] == NULL!\n", s, m, l);
                            hGemExCS->Fill(hit->GetStripTotalSignalInLowerLayer());
                        }
                    }
                }
            }
        }
        // clear marks
        for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
            BmnHit *hit = static_cast<BmnHit*> (gemHits->UncheckedAt(iHit));
            hit->SetFlag(kTRUE);
        }
        for (UInt_t iDig = 0; iDig < gemEx->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(iDig);
            dig->SetName("");
        }

    }
    printf("Exp histograms filled\n");


    //    title = "Si-rescaled";
    //    TCanvas* can = new TCanvas(title, title, 1600, 900);
    //    can->SetLogy();
    //    hGemEx->SetLineColor(kBlack);
    //    if (hGemEx->Integral() > 0)
    //        hGemEx->Scale(1 / hGemEx->Integral());
    //    hGemEx->SetMinimum(3e-4);
    //    hGemEx->SetMaximum(5e-2);
    //    hGemEx->Draw();
    //    hGemExHits->SetLineColor(kBlue);
    //    //    hGemExHits->DrawNormalized("same");
    //    hGemExCS->SetLineColor(kViolet);
    //    hGemExCS->DrawNormalized("same");
    //    hGemExTracks->SetLineColor(kTeal);
    //    //    hGemExTracks->DrawNormalized("same");

    for (Long64_t iEv = 0; iEv < NEventsMC; ++iEv) {
        //        printf("iev %lld\n", iEv);
        DrawBar((UInt_t) iEv, (UInt_t) NEventsMC);
        chainMC->GetEntry(iEv);
        /** summ strip signals */
        for (UInt_t iDig = 0; iDig < gemMC->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemMC->At(iDig);
            if (dig->GetStripSignal() <= lowThr)
                continue;
            Int_t s = dig->GetStation();
            Int_t m = dig->GetModule();
            Int_t l = dig->GetStripLayer();
            TF1* func = ResVec[s][m][l];
            if (!func)
                continue;
            Double_t val = dig->GetStripSignal();
            histSiliconStrip[s][m][l]->Fill(func->Eval(val));
        }
    }
    printf("MC  histogram  filled\n");
    ////    hGemMC->SetLineColor(kRed);
    ////    hGemMC->DrawNormalized("same");
    //
    //    auto legend = new TLegend(0.6, 0.65, 0.8, 0.9);
    //    legend->SetHeader("The Legend", "C");
    //    legend->AddEntry(hGemEx, "All digits"); // lpfe
    //    //    legend->AddEntry(hGemExHits, "Digits from hits");
    //    legend->AddEntry(hGemExCS, "Cluster signal lower");
    //    //    legend->AddEntry(hGemExTracks, "Digits from tracks");
    //    legend->AddEntry(hGemMC, "MC rescaled by cluster exp");
    //    legend->Draw();
    //
    //    can->Print(Form("%s-th-%.1f-cs-%d-Lower-up500-2m.pdf", can->GetName(), lowThr, ClusterSizeThr));
    //    can->Clear();
    //
    //    can->SetLogy(kFALSE);
    //    auto legendCluster = new TLegend(0.4, 0.65, 0.6, 0.9);
    //    for (Int_t i = 0; i < nCSize; i++) {
    //        hGemCluster[i]->SetLineColor(cols[i]);
    //        hGemCluster[i]->Draw(i == 0 ? "" : "same");
    //        legendCluster->AddEntry(hGemCluster[i], hGemCluster[i]->GetName());
    //    }
    //    legendCluster->Draw();
    //    can->Print(Form("%s-cluster-Lower.pdf", can->GetName()));
    //    can->Clear();


    Int_t modCtr = 0;
    for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
        BmnSiliconStation* st = stationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiliconModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < (iStation ? (mod->GetNStripLayers() / 2) : mod->GetNStripLayers()); iLayer++) {
                BmnSiliconLayer lay = mod->GetStripLayer(iLayer);
                Int_t iPad = modCtr * maxLayers + iLayer;
                TVirtualPad * pad = canStrip->cd(iPad + 1);
                pad->Clear();
                pad->SetLogy();

                TH1D *hR = histSiliconStrip[iStation][iModule][iLayer];
                if (!hR)
                    continue;
                if (hR->GetIntegral() == 0)
                    continue;
                hR->SetLineColor(kRed);
                hR->DrawNormalized();

                //                TH1D *hExp = infoVecExp[iStation][iModule][iLayer]->hSig;
                TH1D *hExp = histSiliconStripEx[iStation][iModule][iLayer];
                if (!hExp)
                    continue;
                if (hExp->GetIntegral() == 0)
                    continue;
                hExp->SetLineColor(kViolet);
                hExp->DrawNormalized("same");

                auto legend = new TLegend(0.6, 0.65, 0.8, 0.9);
                legend->SetHeader("The Legend", "C");
                legend->AddEntry(hExp, "Cluster signal");
                legend->AddEntry(hR, "MC rescaled by cluster exp");
                legend->Draw();

                pad->Update();
                pad->Modified();
            }
            modCtr++;
        }
    }
    //canStrip->SaveAs("rescale-sil-strip.png");
    //    canStrip->Print(Form("%s-th-%.1f-cs-%d-2m.pdf", canStrip->GetName(), lowThr, ClusterSizeThr));
    canStrip->Print(Form("%s-th-%.1f-cs-%d-tracks.png", canStrip->GetName(), lowThr, ClusterSizeThr));

    //    delete can;
    delete hGemEx;
    delete hGemExHits;
    delete hGemExTracks;
    delete hGemMC;
    //    delete legend;
    //    delete legendCluster;
    delete chainMC;
    delete chainEx;
}
