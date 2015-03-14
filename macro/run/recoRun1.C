
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
//#include "../../globaltracking/BmnDchHitFinderRun1.cxx"

void recoRun1() {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add("bmn_run0258_digit.root");

    TClonesArray *dchDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);

    Int_t events = bmnTree->GetEntries();
    cout << "N events = " << events << endl;
    TClonesArray* hits = new TClonesArray("BmnDchHit");
    
    UInt_t nBins = 1000;
    Float_t bound = InnerRadiusOfOctagon;

    TH1F* hu = new TH1F("hu", "hu", nBins, -bound, bound);
    TH1F* hv = new TH1F("hv", "hv", nBins, -bound, bound);
    TH1F* hx = new TH1F("hx", "hx", nBins, -bound, bound);
    TH1F* hy = new TH1F("hy", "hy", nBins, -bound, bound);
    
    TH2F* hxy = new TH2F("hxy", "hxy", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* hvu = new TH2F("hvu", "hvu", nBins, -bound, bound, nBins, -bound, bound);
    
    TH2F* hxv = new TH2F("hxv", "hxv", nBins, -bound, bound, nBins, -bound, bound);
    TH2F* hyu = new TH2F("hyu", "hyu", nBins, -bound, bound, nBins, -bound, bound);
    
    for (Int_t iEv = 0; iEv < events; iEv++) {
        bmnTree->GetEntry(iEv);
        hits->Clear();
        ProcessEvent(dchDigits, hits);
        for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
            BmnDchHit* hit = (BmnDchHit*) hits->At(i);
            Float_t x = hit->GetX();
            Float_t y = hit->GetY();
            if (hit->GetLayer() == 1) {
                hx->Fill(x);
                hy->Fill(y);
                hxy->Fill(x, y);
            }
            if (hit->GetLayer() == 0) {
                hu->Fill(y);
                hv->Fill(x);
                hvu->Fill(x, y);
            }
        }
    } // event loop
    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 800);
    
    c1->Divide(2, 2);
    c1->cd(1)->SetLogy();
    hx->Draw();
    c1->cd(2)->SetLogy();
    hy->Draw();
    c1->cd(3)->SetLogy();
    hu->Draw();
    c1->cd(4)->SetLogy();
    hv->Draw();
    
    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 800);
    
    c2->Divide(2, 2);
    c2->cd(1);
    hxy->Draw("colz");
    c2->cd(2);
    hvu->Draw("colz");
    c2->cd(3);
    hxv->Draw("colz");
    c2->cd(4);
    hyu->Draw("colz");
}
