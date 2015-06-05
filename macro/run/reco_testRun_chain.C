
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

//MWPC digits
TH1I* h_mwpc0_digit_time = new TH1I("h_mwpc0_digit_time", "Time distribution of MWPC0 digits", 50, -5, 45);
TH1I* h_mwpc1_digit_time = new TH1I("h_mwpc1_digit_time", "Time distribution of MWPC1 digits", 50, -5, 45);
TH1I* h_mwpc2_digit_time = new TH1I("h_mwpc2_digit_time", "Time distribution of MWPC2 digits", 50, -5, 45);

TH1I* h_mwpc_digit_planes = new TH1I("h_mwpc_digit_planes", "Plane distribution of MWPC digits", 21, -1, 20);

TH1I* h_mwpc0_digit_nDigits = new TH1I("h_mwpc0_digit_nDigits", "Number of digits distribution in MWPC0", 21, -1, 20);
TH1I* h_mwpc1_digit_nDigits = new TH1I("h_mwpc1_digit_nDigits", "Number of digits distribution in MWPC1", 21, -1, 20);
TH1I* h_mwpc2_digit_nDigits = new TH1I("h_mwpc2_digit_nDigits", "Number of digits distribution in MWPC2", 21, -1, 20);

//TH1I* h_mwpc0_digit_plane0_nDigits = new TH1I("h_mwpc0_digit_plane0_nDigits", "Number of digits distribution in MWPC0 for plane = 0", 21, -1, 20);
//TH1I* h_mwpc0_digit_plane1_nDigits = new TH1I("h_mwpc0_digit_plane1_nDigits", "Number of digits distribution in MWPC0 for plane = 1", 21, -1, 20);
//TH1I* h_mwpc0_digit_plane2_nDigits = new TH1I("h_mwpc0_digit_plane2_nDigits", "Number of digits distribution in MWPC0 for plane = 2", 21, -1, 20);
//TH1I* h_mwpc0_digit_plane3_nDigits = new TH1I("h_mwpc0_digit_plane3_nDigits", "Number of digits distribution in MWPC0 for plane = 3", 21, -1, 20);
//TH1I* h_mwpc0_digit_plane4_nDigits = new TH1I("h_mwpc0_digit_plane4_nDigits", "Number of digits distribution in MWPC0 for plane = 4", 21, -1, 20);
//TH1I* h_mwpc0_digit_plane5_nDigits = new TH1I("h_mwpc0_digit_plane5_nDigits", "Number of digits distribution in MWPC0 for plane = 5", 21, -1, 20);
//TH1I* h_mwpc1_digit_plane0_nDigits = new TH1I("h_mwpc1_digit_plane0_nDigits", "Number of digits distribution in MWPC1 for plane = 0", 21, -1, 20);
//TH1I* h_mwpc1_digit_plane1_nDigits = new TH1I("h_mwpc1_digit_plane1_nDigits", "Number of digits distribution in MWPC1 for plane = 1", 21, -1, 20);
//TH1I* h_mwpc1_digit_plane2_nDigits = new TH1I("h_mwpc1_digit_plane2_nDigits", "Number of digits distribution in MWPC1 for plane = 2", 21, -1, 20);
//TH1I* h_mwpc1_digit_plane3_nDigits = new TH1I("h_mwpc1_digit_plane3_nDigits", "Number of digits distribution in MWPC1 for plane = 3", 21, -1, 20);
//TH1I* h_mwpc1_digit_plane4_nDigits = new TH1I("h_mwpc1_digit_plane4_nDigits", "Number of digits distribution in MWPC1 for plane = 4", 21, -1, 20);
//TH1I* h_mwpc1_digit_plane5_nDigits = new TH1I("h_mwpc1_digit_plane5_nDigits", "Number of digits distribution in MWPC1 for plane = 5", 21, -1, 20);
//TH1I* h_mwpc2_digit_plane0_nDigits = new TH1I("h_mwpc2_digit_plane0_nDigits", "Number of digits distribution in MWPC2 for plane = 0", 21, -1, 20);
//TH1I* h_mwpc2_digit_plane1_nDigits = new TH1I("h_mwpc2_digit_plane1_nDigits", "Number of digits distribution in MWPC2 for plane = 1", 21, -1, 20);
//TH1I* h_mwpc2_digit_plane2_nDigits = new TH1I("h_mwpc2_digit_plane2_nDigits", "Number of digits distribution in MWPC2 for plane = 2", 21, -1, 20);
//TH1I* h_mwpc2_digit_plane3_nDigits = new TH1I("h_mwpc2_digit_plane3_nDigits", "Number of digits distribution in MWPC2 for plane = 3", 21, -1, 20);
//TH1I* h_mwpc2_digit_plane4_nDigits = new TH1I("h_mwpc2_digit_plane4_nDigits", "Number of digits distribution in MWPC2 for plane = 4", 21, -1, 20);
//TH1I* h_mwpc2_digit_plane5_nDigits = new TH1I("h_mwpc2_digit_plane5_nDigits", "Number of digits distribution in MWPC2 for plane = 5", 21, -1, 20);


//MWPC hits
Float_t mwpcX = 25.0;
Float_t mwpcY = 25.0;
UInt_t mwpcN = 200;
TH2F* h_mwpc0_hit_XY = new TH2F("h_mwpc0_hit_XY", "Hits distribution in XY plane in MWPC0", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);
TH2F* h_mwpc1_hit_XY = new TH2F("h_mwpc1_hit_XY", "Hits distribution in XY plane in MWPC1", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);
TH2F* h_mwpc2_hit_XY = new TH2F("h_mwpc2_hit_XY", "Hits distribution in XY plane in MWPC2", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);

TH1F* h_mwpc0_hit_X = new TH1F("h_mwpc0_hit_X", "Hits distribution in X axis in MWPC0", mwpcN, -mwpcX, mwpcX);
TH1F* h_mwpc1_hit_X = new TH1F("h_mwpc1_hit_X", "Hits distribution in X axis in MWPC1", mwpcN, -mwpcX, mwpcX);
TH1F* h_mwpc2_hit_X = new TH1F("h_mwpc2_hit_X", "Hits distribution in X axis in MWPC2", mwpcN, -mwpcX, mwpcX);

TH1F* h_mwpc0_hit_Y = new TH1F("h_mwpc0_hit_Y", "Hits distribution in Y axis in MWPC0", mwpcN, -mwpcY, mwpcY);
TH1F* h_mwpc1_hit_Y = new TH1F("h_mwpc1_hit_Y", "Hits distribution in Y axis in MWPC1", mwpcN, -mwpcY, mwpcY);
TH1F* h_mwpc2_hit_Y = new TH1F("h_mwpc2_hit_Y", "Hits distribution in Y axis in MWPC2", mwpcN, -mwpcY, mwpcY);

TH1F* h_mwpc0_hit_Z = new TH1F("h_mwpc0_hit_Z", "Hits distribution in Z axis in MWPC0", mwpcN, 0.0, 0.0);
TH1F* h_mwpc1_hit_Z = new TH1F("h_mwpc1_hit_Z", "Hits distribution in Z axis in MWPC1", mwpcN, 0.0, 0.0);
TH1F* h_mwpc2_hit_Z = new TH1F("h_mwpc2_hit_Z", "Hits distribution in Z axis in MWPC2", mwpcN, 0.0, 0.0);

TH1I* h_mwpc0_hit_nHits = new TH1I("h_mwpc0_hit_nHits", "Number of hits distribution in MWPC0", 21, -1, 20);
TH1I* h_mwpc1_hit_nHits = new TH1I("h_mwpc1_hit_nHits", "Number of hits distribution in MWPC1", 21, -1, 20);
TH1I* h_mwpc2_hit_nHits = new TH1I("h_mwpc2_hit_nHits", "Number of hits distribution in MWPC2", 21, -1, 20);

//MWPC tracks

TH1F* h_mwpc0_track_Tx = new TH1F("h_mwpc0_track_Tx", "Tx distribution of tracks in MWPC0", mwpcN, -15, 15);
TH1F* h_mwpc1_track_Tx = new TH1F("h_mwpc1_track_Tx", "Tx distribution of tracks in MWPC1", mwpcN, -15, 15);
TH1F* h_mwpc2_track_Tx = new TH1F("h_mwpc2_track_Tx", "Tx distribution of tracks in MWPC2", mwpcN, -15, 15);

TH1F* h_mwpc0_track_Ty = new TH1F("h_mwpc0_track_Ty", "Ty distribution of tracks in MWPC0", mwpcN, -15, 15);
TH1F* h_mwpc1_track_Ty = new TH1F("h_mwpc1_track_Ty", "Ty distribution of tracks in MWPC1", mwpcN, -15, 15);
TH1F* h_mwpc2_track_Ty = new TH1F("h_mwpc2_track_Ty", "Ty distribution of tracks in MWPC2", mwpcN, -15, 15);

TH1I* h_mwpc0_track_nHits = new TH1I("h_mwpc0_track_nHits", "Number of hit distribution of MWPC0 tracks", 5, -1, 4);
TH1I* h_mwpc1_track_nHits = new TH1I("h_mwpc1_track_nHits", "Number of hit distribution of MWPC1 tracks", 5, -1, 4);
TH1I* h_mwpc2_track_nHits = new TH1I("h_mwpc2_track_nHits", "Number of hit distribution of MWPC2 tracks", 5, -1, 4);

TH1F* h_mwpc0_track_chi2 = new TH1F("h_mwpc0_track_chi2", "#Chi^2 distribution of tracks in MWPC0", mwpcN, -1, 20);
TH1F* h_mwpc1_track_chi2 = new TH1F("h_mwpc1_track_chi2", "#Chi^2 distribution of tracks in MWPC1", mwpcN, -1, 20);
TH1F* h_mwpc2_track_chi2 = new TH1F("h_mwpc2_track_chi2", "#Chi^2 distribution of tracks in MWPC2", mwpcN, -1, 20);

TH2F* h_mwpc0_track_XYstart = new TH2F("h_mwpc0_track_XYstart", "Start hits distribution of track in XY plane in MWPC0", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);
TH2F* h_mwpc1_track_XYstart = new TH2F("h_mwpc1_track_XYstart", "Start hits distribution of track in XY plane in MWPC1", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);
TH2F* h_mwpc2_track_XYstart = new TH2F("h_mwpc2_track_XYstart", "Start hits distribution of track in XY plane in MWPC2", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);

TH1F* h_mwpc0_track_Xstart = new TH1F("h_mwpc0_track_Xstart", "Start hits distribution of track in X axis in MWPC0", mwpcN, -mwpcX, mwpcX);
TH1F* h_mwpc1_track_Xstart = new TH1F("h_mwpc1_track_Xstart", "Start hits distribution of track in X axis in MWPC1", mwpcN, -mwpcX, mwpcX);
TH1F* h_mwpc2_track_Xstart = new TH1F("h_mwpc2_track_Xstart", "Start hits distribution of track in X axis in MWPC2", mwpcN, -mwpcX, mwpcX);

TH1F* h_mwpc0_track_Ystart = new TH1F("h_mwpc0_track_Ystart", "Start hits distribution of track in Y axis in MWPC0", mwpcN, -mwpcY, mwpcY);
TH1F* h_mwpc1_track_Ystart = new TH1F("h_mwpc1_track_Ystart", "Start hits distribution of track in Y axis in MWPC1", mwpcN, -mwpcY, mwpcY);
TH1F* h_mwpc2_track_Ystart = new TH1F("h_mwpc2_track_Ystart", "Start hits distribution of track in Y axis in MWPC2", mwpcN, -mwpcY, mwpcY);

TH1I* h_mwpc0_track_nTracks = new TH1I("h_mwpc0_track_nTracks", "Number of tracks distribution in MWPC0", 21, -1, 20);
TH1I* h_mwpc1_track_nTracks = new TH1I("h_mwpc1_track_nTracks", "Number of tracks distribution in MWPC1", 21, -1, 20);
TH1I* h_mwpc2_track_nTracks = new TH1I("h_mwpc2_track_nTracks", "Number of tracks distribution in MWPC2", 21, -1, 20);

void reco_testRun_chain() {

    UInt_t nEvents = 10000; // zero corresponds to ALL EVENTS in run

    recoRun1(250, nEvents);
    
    qaRun1(250, nEvents);

    fOut->Write();
    fOut->Close();
}