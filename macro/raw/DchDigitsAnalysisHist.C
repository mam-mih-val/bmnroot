#include "TH1F.h"
#include "TLine.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TMath.h"

//#include "../../globaltracking/BmnHitFinderRun1.h"

using namespace TMath;

const UInt_t kNPLANES = 16;
const UInt_t kNREALWIRES = 240;
const UInt_t kNWIRES = 256;
const Int_t Number = 4;
const Int_t nb = 100;
const Float_t angles[kNPLANES] = {
    135, 135, 45, 45, 90, 90, 180, 180,
    135, 135, 45, 45, 90, 90, 180, 180
};
const TString names[kNPLANES] = {
    "VA_1", "VB_1", "UA_1", "UB_1", "YA_1", "YB_1", "XA_1", "XB_1",
    "VA_2", "VB_2", "UA_2", "UB_2", "YA_2", "YB_2", "XA_2", "XB_2"
};

void DchDigitsAnalysisHist(UInt_t runId = 0) {
//    gStyle->SetOptStat(0);
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", runId);
    eveTree->Add(inName);

    TClonesArray *DchDigits;
    TClonesArray* DchHits = new TClonesArray("BmnDchHit");
    eveTree->SetBranchAddress("DCH", &DchDigits);

    Long64_t nEvents = eveTree->GetEntries();
    cout << nEvents << endl;

    TH2F* h_DCH1 = new TH2F("h_DCH1", "h_DCH1", 500, 0, 0, 500, 0, 0);
    TH2F* h_DCH2 = new TH2F("h_DCH2", "h_DCH2", 500, 0, 0, 500, 0, 0);
    
    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        if (iEv % 100 == 0) cout << "EVENT: " << iEv << endl;
        DchHits->Clear();
        eveTree->GetEntry(iEv);
        ProcessDchDigits(DchDigits, DchHits);
//        cout << DchHits->GetEntriesFast() << endl;
        for (Int_t iHit = 0; iHit < DchHits->GetEntriesFast(); ++iHit) {
            BmnDchHit* hit = (BmnDchHit*) DchHits->At(iHit);
            if (hit->GetDchId() == 1) h_DCH1->Fill(hit->GetX(), hit->GetY());
            if (hit->GetDchId() == 2) h_DCH2->Fill(hit->GetX(), hit->GetY());
        }
    }
    
    TCanvas* c_dch1 = new TCanvas("dch1", "dch1", 1000, 1000);
    h_DCH1->Draw("colz");
    TCanvas* c_dch2 = new TCanvas("dch2", "dch2", 1000, 1000);
    h_DCH2->Draw("colz");
}
