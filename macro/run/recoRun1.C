#include "TH1F.h"
#include "TCanvas.h"
#include "TChain.h"
//#include "../../globaltracking/BmnDchHitFinderRun1.cxx"

void recoRun1() {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add("bmn_run0166_digit.root");

    TClonesArray *dchDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);

    //    TClonesArray *RecoTracks;
    //    dstTree->SetBranchAddress("GlobalTrack", &RecoTracks);
    //    dstTree->SetBranchAddress("BmnGemTracks", &RecoTracks);

    Int_t events = bmnTree->GetEntries();
    cout << "N events = " << events << endl;
    TClonesArray* hits = new TClonesArray("BmnDchHit");

    TH1F* hu = new TH1F("hu", "hu", 256, -InnerRadiusOfOctagon, InnerRadiusOfOctagon);
    TH1F* hv = new TH1F("hv", "hv", 256, -InnerRadiusOfOctagon, InnerRadiusOfOctagon);
    TH1F* hx = new TH1F("hx", "hx", 256, -InnerRadiusOfOctagon, InnerRadiusOfOctagon);
    TH1F* hy = new TH1F("hy", "hy", 256, -InnerRadiusOfOctagon, InnerRadiusOfOctagon);
    for (Int_t iEv = 0; iEv < events; iEv++) {
        bmnTree->GetEntry(iEv);
        ProcessEvent(dchDigits, hits);
        for (Int_t i = 0; i < hits->GetEntriesFast(); ++i) {
            BmnDchHit* hit = (BmnDchHit*) hits->At(i);
            if (hit->GetLayer() == 3) {
                hx->Fill(hit->GetX());
                hy->Fill(hit->GetY());
            }
            if (hit->GetLayer() == 2) {
                hu->Fill(hit->GetX());
                hv->Fill(hit->GetY());
            }
        }
    } // event loop
    TCanvas* c = new TCanvas("c", "c", 1600, 800);
    
    c->Divide(2, 2);
    c->cd(1)->SetLogy();
    hx->Draw();
    c->cd(2)->SetLogy();
    hy->Draw();
    c->cd(3)->SetLogy();
    hu->Draw();
    c->cd(4)->SetLogy();
    hv->Draw();
}
