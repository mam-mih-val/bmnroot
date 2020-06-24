#include <vector>

#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>
#include <Rtypes.h>
#include <TTimeStamp.h>
#include <TMathBase.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TLegend.h>

//#include "../../bmndata/math/BmnMath.h"


//#include "../../bmndata/BmnStripDigit.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/**
 * rescaleAmplitudes
 * Example of ADC signal rescaling procedures for GEM strip digits
 * @param fileNameMC
 * @param fileNameEx
 * @param fileNameDST
 */
void rescaleAmplitude(TString fileNameMC, TString fileNameEx, TString fileNameDST) {
    if (fileNameMC == "" || fileNameEx == "") {
        cout << "Files need to be specified!" << endl;
        return;
    }
    gStyle->SetOptStat(0);
    Int_t ClusterSizeThr = 0;
    Double_t lowThr = 0;
    Int_t xBins = 400;
    Double_t xLow = 0.0;
    Double_t xUp = 2300.0;
    Int_t nCSize = 5;
    vector<EColor> cols = {kBlack, kBlue, kTeal, kViolet, kOrange};

    bmnloadlibs(); // load libraries
    //    gSystem->Load("libBmnRecoTools");

    // Load MC digits
    TChain* chainMC = new TChain("bmndata");
    chainMC->Add(fileNameMC.Data());
    Long64_t NEventsMC = chainMC->GetEntries();
    printf("#recorded  MC entries = %lld\n", NEventsMC);
    TClonesArray * gemMC = nullptr;
    chainMC->SetBranchAddress("BmnGemStripDigit", &gemMC);

    // Load Exp digits
    TChain* chainEx = new TChain("bmndata");
    chainEx->Add(fileNameEx.Data());
    Long64_t NEventsEx = chainEx->GetEntries();
    printf("#recorded Exp entries = %lld\n", NEventsEx);
    TClonesArray * gemEx = nullptr;
    chainEx->SetBranchAddress("GEM", &gemEx);

    // Load tracks
    TChain* chainDST = new TChain("bmndata");
    chainDST->Add(fileNameDST.Data());
    Long64_t NEventsDST = chainDST->GetEntries();
    printf("#recorded Track entries = %lld\n", NEventsDST);
    TClonesArray * tracks = nullptr;
    TClonesArray * gemTracks = nullptr;
    TClonesArray * gemHits = nullptr;
    chainDST->SetBranchAddress("BmnGlobalTrack", &tracks);
    chainDST->SetBranchAddress("BmnGemTrack", &gemTracks);
    chainDST->SetBranchAddress("BmnGemStripHit", &gemHits);

    TF1 *mc = BmnRecoTools::GetSignalDistribution(chainMC, gemMC,
            nullptr, nullptr, nullptr, nullptr,
            lowThr, 0, 2e6);
    TF1 *ex = BmnRecoTools::GetSignalDistribution(chainEx, gemEx,
            chainDST, gemHits, gemTracks, tracks,
            0, ClusterSizeThr, 2e6);
    TF1 *funcRescale = BmnRecoTools::GetRescaleFunc(TString("RescaleGEM"), mc, ex);
    printf("rescaling func created\n");

    TString title = "GEM MC  Rescaled";
    TH1D* hGemMC = new TH1D("mc", title, xBins, xLow, xUp);
    title = "GEM Exp ";
    TH1D* hGemEx = new TH1D("digs", title, xBins, xLow, xUp);
    title = "GEM Exp Hits' Digits";
    TH1D* hGemExHits = new TH1D("digs-hits", title, xBins, xLow, xUp);
    title = "GEM Exp Cluster Signal Lower";
    TH1D* hGemExCS = new TH1D("cluster-signal-upper", title, xBins, xLow, xUp);
    title = "GEM Exp Tracks' Digits";
    TH1D* hGemExTracks = new TH1D("digs-tracks", title, xBins, xLow, xUp);
    vector < TH1D* > hGemCluster;
    for (Int_t i = 0; i < nCSize; i++) {
        title = "GEM Cluster Size";
        TH1D* h = new TH1D(
                Form("gem csize %.d", i),
                title,
                xBins, xLow, 1000);
        hGemCluster.push_back(h);
    }



    for (Long64_t iEv = 0; iEv < NEventsEx; ++iEv) {
        DrawBar((UInt_t) iEv, (UInt_t) NEventsEx);
        chainEx->GetEntry(iEv);
        chainDST->GetEntry(iEv);
        /** summ strip signals */

        /** digs all*/
        for (UInt_t iDig = 0; iDig < gemEx->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(iDig);
            Double_t signal = dig->GetStripSignal();
            dig->SetName(""); // clear marks
            hGemEx->Fill(signal);
        }

        /** digs from hits*/
        for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
            BmnGemStripHit *hit = (BmnGemStripHit *) gemHits->UncheckedAt(iHit);
            // fill cluster signal for different cluster sizes
            hGemCluster[0]->Fill(hit->GetStripTotalSignalInLowerLayer());
            Int_t csize = hit->GetClusterSizeInLowerLayer();
            if (csize < hGemCluster.size())
                hGemCluster[csize]->Fill(hit->GetStripTotalSignalInLowerLayer());
            if (hit->GetClusterSizeInLowerLayer() <= ClusterSizeThr)
                continue;
            hGemExCS->Fill(hit->GetStripTotalSignalInLowerLayer());

            BmnMatch match = hit->GetDigitNumberMatch();
            const vector<BmnLink> links = match.GetLinks();
            //                                        printf("\t event %lld  ihit %d: links = %d  digi entries = %d\n",
            //                                               iEv, iHit, links.size(), gemEx->GetEntriesFast());
            for (const BmnLink& link : links) {
                if (link.GetIndex() > (gemEx->GetEntriesFast() - 1)) {
                    printf("Warning in event %lld  hit %d! Hits Link index out of range! index = %d  entries %d\n",
                            iEv, iHit, link.GetIndex(), gemEx->GetEntriesFast());
                    continue;
                }
                BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(link.GetIndex());
                if (strcmp(dig->GetName(), ""))
                    continue;
                dig->SetName("used");
                hGemExHits->Fill(dig->GetStripSignal());
            }
        }
        // clear marks
        for (UInt_t iDig = 0; iDig < gemEx->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(iDig);
            dig->SetName("");
        }

        /** digs from tracks*/
        for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) tracks->UncheckedAt(iTrack);
            if (track->GetGemTrackIndex() != -1) {
                //                for (Int_t iTrack = 0; iTrack < gemTrack->GetEntriesFast(); iTrack++) {
                BmnTrack * gemTr = (BmnTrack*) gemTracks->UncheckedAt(track->GetGemTrackIndex());
                for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                    BmnGemStripHit *hit = (BmnGemStripHit *) gemHits->UncheckedAt(gemTr->GetHitIndex(iHit));

                    BmnMatch match = hit->GetDigitNumberMatch();
                    const vector<BmnLink> links = match.GetLinks();
                    for (const BmnLink& link : links) {
                        if (link.GetIndex() > (gemEx->GetEntriesFast() - 1)) {
                            printf("Warning in event %lld!Tracks Link index out of range! index = %d  entries %d\n",
                                    iEv, link.GetIndex(), gemEx->GetEntriesFast());
                            continue;
                        }
                        BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(link.GetIndex());
                        hGemExTracks->Fill(dig->GetStripSignal());
                    }
                }
                //                }
            }
        }
    }
    printf("Exp histograms filled\n");


    title = "GEM-rescaled";
    TCanvas* can = new TCanvas(title, title, 1600, 900);
    can->SetLogy();
    hGemEx->SetLineColor(kBlack);
    if (hGemEx->Integral() > 0)
        hGemEx->Scale(1 / hGemEx->Integral());
    hGemEx->SetMinimum(8e-5);
    hGemEx->SetMaximum(1.2e-1);
    hGemEx->Draw();
    hGemExHits->SetLineColor(kBlue);
//    hGemExHits->DrawNormalized("same");
    hGemExCS->SetLineColor(kViolet);
    hGemExCS->DrawNormalized("same");
    hGemExTracks->SetLineColor(kTeal);
//    hGemExTracks->DrawNormalized("same");

    for (Long64_t iEv = 0; iEv < NEventsMC; ++iEv) {
        DrawBar((UInt_t) iEv, (UInt_t) NEventsMC);
        chainMC->GetEntry(iEv);
        /** summ strip signals */
        for (UInt_t iDig = 0; iDig < gemMC->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemMC->At(iDig);
            if (dig->GetStripSignal() <= lowThr)
                continue;
            //            if (dig->GetStripSignal() > 2)
            //                continue;
            Double_t signal = funcRescale->Eval(dig->GetStripSignal());
            hGemMC->Fill(signal);
        }
    }
    printf("MC  histogram  filled\n");
    hGemMC->SetLineColor(kRed);
    hGemMC->DrawNormalized("same");

    auto legend = new TLegend(0.6, 0.65, 0.8, 0.9);
    legend->SetHeader("The Legend", "C");
    legend->AddEntry(hGemEx, "All digits"); // lpfe
//    legend->AddEntry(hGemExHits, "Digits from hits");
    legend->AddEntry(hGemExCS, "Cluster signal lower");
//    legend->AddEntry(hGemExTracks, "Digits from tracks");
    legend->AddEntry(hGemMC, "MC rescaled by all exp digits");
    legend->Draw();

    can->Print(Form("%s-th-%.1f-cs-%d-Lower-2m2m.pdf", can->GetName(), lowThr, ClusterSizeThr));
    can->Clear();

    can->SetLogy(kFALSE);
    auto legendCluster = new TLegend(0.6, 0.65, 0.8, 0.9);
    for (Int_t i = 0; i < nCSize; i++) {
        hGemCluster[i]->SetLineColor(cols[i]);
        hGemCluster[i]->Draw(i == 0 ? "" : "same");
        legendCluster->AddEntry(hGemCluster[i], hGemCluster[i]->GetName());
    }
    legendCluster->Draw();
    can->Print(Form("%s-cluster-Lower.pdf", can->GetName()));
    can->Clear();


    delete can;
    delete hGemEx;
    delete hGemExHits;
    delete hGemExTracks;
    delete hGemMC;
    delete legend;
    delete legendCluster;
    delete chainMC;
    delete chainEx;
}
