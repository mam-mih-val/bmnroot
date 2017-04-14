#include "TH1F.h"
#include "TLine.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TMath.h"
#include "TClonesArray.h"

//#include "../../globaltracking/BmnHitFinderRun1.h"

using namespace TMath;

const UInt_t kNPLANES = 16;
const UInt_t kNREALWIRES = 240;
const UInt_t kNWIRES = 256;
const Int_t Number = 4;
const Int_t nb = 100;

void MwpcDigitsAnalysisHist(UInt_t runId = 0) {
//    gStyle->SetOptStat(0);
    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *eveTree = new TChain("cbmsim");
    TString inName = Form("bmn_run%04d_digi.root", runId);
    eveTree->Add(inName);

    TClonesArray *MwpcDigits;
    TClonesArray* MwpcHits = new TClonesArray("BmnMwpcHit");
    eveTree->SetBranchAddress("MWPC", &MwpcDigits);

    Long64_t nEvents = eveTree->GetEntries();
    cout << nEvents << endl;

    TH2F* h_MWPC1 = new TH2F("h_MWPC1", "h_MWPC1", 200, -20, 20, 200, -20, 20);
    TH2F* h_MWPC2 = new TH2F("h_MWPC2", "h_MWPC2", 200, -20, 20, 200, -20, 20);
    
    for (Int_t iEv = 0; iEv < nEvents; iEv++) {
        if (iEv % 100 == 0) cout << "EVENT: " << iEv << endl;
        MwpcHits->Clear();
        eveTree->GetEntry(iEv);
        ProcessMwpcDigits(MwpcDigits, MwpcHits);
//        cout << MwpcDigits->GetEntriesFast() << endl;
        for (Int_t iHit = 0; iHit < MwpcHits->GetEntriesFast(); ++iHit) {
            BmnMwpcHit* hit = (BmnMwpcHit*) MwpcHits->At(iHit);
            //                        cout <<            hit->GetMwpcId() << endl;
            if (hit->GetMwpcId() == 0) h_MWPC1->Fill(hit->GetX(), hit->GetY());
            if (hit->GetMwpcId() == 1) h_MWPC2->Fill(hit->GetX(), hit->GetY());
        }
    }
    
    TCanvas* c_mwpc1 = new TCanvas("mwpc1", "mwpc1", 1000, 1000);
    h_MWPC1->Draw("colz");
    TCanvas* c_mwpc2 = new TCanvas("mwpc2", "mwpc2", 1000, 1000);
    h_MWPC2->Draw("colz");
}
