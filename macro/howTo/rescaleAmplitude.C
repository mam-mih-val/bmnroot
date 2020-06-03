#include <TFile.h>
#include <TChain.h>
#include <TString.h>
#include <TClonesArray.h>
#include <Rtypes.h>
#include <TTimeStamp.h>
#include <TMathBase.h>
#include <TCanvas.h>
#include <TH1D.h>

//#include "../../bmndata/math/BmnMath.h"


//#include "../../bmndata/BmnStripDigit.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/**
 * rescaleAmplitudes
 * Example of 
 * @param fileNameMC
 * @param fileNameEx
 */
void rescaleAmplitude(TString fileNameMC, TString fileNameEx, TString inDSTName) {
    if (fileNameMC == "" || fileNameEx == "") {
        cout << "Files need to be specified!" << endl;
        return;
    }
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
    chainDST->Add(inDSTName.Data());
    Long64_t NEventsDST = chainDST->GetEntries();
    printf("#recorded Track entries = %lld\n", NEventsDST);
    TClonesArray * tracks = nullptr;
    TClonesArray * gemTracks = nullptr;
    TClonesArray * gemHits = nullptr;
    chainDST->SetBranchAddress("BmnGlobalTrack", &tracks);
    chainDST->SetBranchAddress("BmnGemTrack", &gemTracks);
    chainDST->SetBranchAddress("BmnGemStripHit", &gemHits);

    TF1 *mc = BmnRecoTools::GetSignalDistribution(chainMC, gemMC);
    TF1 *ex = BmnRecoTools::GetSignalDistribution(chainEx, gemEx);
    TF1 *funcRescale = BmnRecoTools::GetRescaleFunc(TString("RescaleGEM"), mc, ex);
    printf("rescaling func created\n");

    TString title = "GEM-rescaled";
    TCanvas* can = new TCanvas(title, title, 1600, 900);
    can->SetLogy();
    //    chainEx->Draw("GEM.fStripSignal", "", "norm");
    title = "GEM MC  Rescaled";
    TH1D* hGemMC = new TH1D("mc", title, 400, 0, 0);
    title = "GEM Exp ";
    TH1D* hGemEx = new TH1D("digs", title, 400, 0, 0);
    title = "GEM Exp From Tracks";
    TH1D* hGemExHits = new TH1D("digs-hits", title, 400, 0, 0);
    title = "GEM Exp From Tracks";
    TH1D* hGemExTracks = new TH1D("digs-tracks", title, 400, 0, 0);


    for (Long64_t iEv = 0; iEv < NEventsEx; ++iEv) {
        DrawBar((UInt_t)iEv, (UInt_t)NEventsEx);
        chainEx->GetEntry(iEv);
        chainDST->GetEntry(iEv);
        /** summ strip signals */
        
        /** digs all*/
        for (UInt_t iDig = 0; iDig < gemEx->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(iDig);
            Double_t signal = dig->GetStripSignal();
            hGemEx->Fill(signal);
        }
        
        /** digs from hits*/
                for (Int_t iHit = 0; iHit < gemHits->GetEntriesFast(); iHit++) {
                    BmnHit *hit = (BmnHit *) gemHits->UncheckedAt(iHit);
                    BmnMatch match = hit->GetDigitNumberMatch();
                    const vector<BmnLink> links = match.GetLinks();
                    for (const BmnLink& link : links) {
                        if (link.GetIndex() > (gemEx->GetEntriesFast() - 1)){
//                            printf("Warning in event %lld! Hits Link index out of range! index = %d  entries %d\n",
//                                   iEv, link.GetIndex(), gemEx->GetEntriesFast());
                            continue;
                        }
                        BmnStripDigit * dig = (BmnStripDigit*) gemEx->At(link.GetIndex());
                        hGemExHits->Fill(dig->GetStripSignal());
                    }
                }
        
        /** digs from tracks*/
        for (Int_t iTrack = 0; iTrack < tracks->GetEntriesFast(); iTrack++) {
            BmnGlobalTrack* track = (BmnGlobalTrack*) tracks->UncheckedAt(iTrack);
            if (track->GetGemTrackIndex() != -1) {
                //                for (Int_t iTrack = 0; iTrack < gemTrack->GetEntriesFast(); iTrack++) {
                BmnTrack * gemTr = (BmnTrack*) gemTracks->UncheckedAt(track->GetGemTrackIndex());
                //                        BmnTrack * gemTr = (BmnTrack*) gemTrack->UncheckedAt(track->GetGemTrackIndex());
                //                        if (gemTr->GetNHits() < 4)
                //                            continue;
                for (Int_t iHit = 0; iHit < gemTr->GetNHits(); iHit++) {
                    BmnHit *hit = (BmnHit *) gemHits->UncheckedAt(gemTr->GetHitIndex(iHit));
                    BmnMatch match = hit->GetDigitNumberMatch();
                    const vector<BmnLink> links = match.GetLinks();
                    for (const BmnLink& link : links) {
                        if (link.GetIndex() > (gemEx->GetEntriesFast() - 1)){
//                            printf("Warning in event %lld!Tracks Link index out of range! index = %d  entries %d\n",
//                                   iEv, link.GetIndex(), gemEx->GetEntriesFast());
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
    hGemEx->SetLineColor(kBlack);
    if (hGemEx->Integral() > 0)
        hGemEx->Scale(1/hGemEx->Integral());
    hGemEx->SetMinimum(5e-5);
    hGemEx->SetMaximum(1.2e-1);
    hGemEx->Draw();
    hGemExHits->SetLineColor(kBlue);
    hGemExHits->DrawNormalized("same");
    hGemExTracks->SetLineColor(kTeal);
    hGemExTracks->DrawNormalized("same");

    for (Long64_t iEv = 0; iEv < NEventsMC; ++iEv) {
        DrawBar((UInt_t)iEv, (UInt_t)NEventsMC);
        chainMC->GetEntry(iEv);
        /** summ strip signals */
        for (UInt_t iDig = 0; iDig < gemMC->GetEntriesFast(); iDig++) {
            BmnStripDigit * dig = (BmnStripDigit*) gemMC->At(iDig);
            Double_t signal = funcRescale->Eval(dig->GetStripSignal());
            hGemMC->Fill(signal);
        }
    }
    hGemMC->SetLineColor(kRed);
    hGemMC->DrawNormalized("same");
    
       auto legend = new TLegend(0.6,0.65,0.8,0.9);
   legend->SetHeader("The Legend","C"); // option "C" allows to center the header
   legend->AddEntry(hGemEx,"All digits"); // lpfe
   legend->AddEntry(hGemExHits,"Digits from hits");
   legend->AddEntry(hGemExTracks,"Digits from tracks");
   legend->AddEntry(hGemMC,"MC rescaled by all exp digits");
   legend->Draw();
    
    
    
    can->Print(Form("%s-full.pdf", can->GetName()));
}
