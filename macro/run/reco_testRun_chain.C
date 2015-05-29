
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "qaRun1.C"
#include "recoRun1.C"

TFile* fOut = new TFile("bmndst_qa.root", "RECREATE");

UInt_t nBins1 = 200;
Float_t boundX = 120.0 * 2.414 / 2.0; //InnerRadiusOfOctagon;
Float_t boundY = boundX;
Float_t zMin = 500;
Float_t zMax = 800;

TH2F* h_xy1 = new TH2F("hxy_DCH1", "hxy_DCH1", nBins1, -boundX, boundX, nBins1, -boundY, boundY);
TH2F* h_xy2 = new TH2F("hxy_DCH2", "hxy_DCH2", nBins1, -boundX, boundX, nBins1, -boundY, boundY);

UInt_t nBins2 = 200;
Float_t res_bound = 0.0; // cm

TH1I* h_nHits = new TH1I("h_nHits", "h_nHits", 5, 0, 5);

TH1F* h_dch1_dist_3d = new TH1F("h_dch1_dist_3d", "h_dch1_dist_3d", nBins2, 0.0, res_bound);
TH1F* h_dch2_dist_3d = new TH1F("h_dch2_dist_3d", "h_dch2_dist_3d", nBins2, 0.0, res_bound);

TH2F* h_dch1_dist_x = new TH2F("h_dch1_dist_x", "h_dch1_dist_x", nBins2, -res_bound, res_bound, nBins2, -res_bound, res_bound);
TH2F* h_dch1_dist_y = new TH2F("h_dch1_dist_y", "h_dch1_distyx", nBins2, -res_bound, res_bound, nBins2, -res_bound, res_bound);
TH2F* h_dch1_dist_z = new TH2F("h_dch1_dist_z", "h_dch1_dist_z", nBins2, -res_bound, res_bound, nBins2, -res_bound, res_bound);
TH2F* h_dch2_dist_x = new TH2F("h_dch2_dist_x", "h_dch2_dist_x", nBins2, -res_bound, res_bound, nBins2, -res_bound, res_bound);
TH2F* h_dch2_dist_y = new TH2F("h_dch2_dist_y", "h_dch2_distyx", nBins2, -res_bound, res_bound, nBins2, -res_bound, res_bound);
TH2F* h_dch2_dist_z = new TH2F("h_dch2_dist_z", "h_dch2_dist_z", nBins2, -res_bound, res_bound, nBins2, -res_bound, res_bound);



void reco_testRun_chain() {

    recoRun1(250, 0);
    //recoRun1(649);

    //qaRun1(649, 0);
//    qaRun1(650, 0);

    fOut->Write();
    fOut->Close();
}