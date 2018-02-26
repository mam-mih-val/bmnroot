
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

const Float_t TxBound = 2.0;
const Float_t TyBound = 2.0;

TH1F* h_mwpc0_track_Tx = new TH1F("h_mwpc0_track_Tx", "Tx distribution of tracks in MWPC0", 100, -TxBound, TxBound);
TH1F* h_mwpc1_track_Tx = new TH1F("h_mwpc1_track_Tx", "Tx distribution of tracks in MWPC1", 100, -TxBound, TxBound);
TH1F* h_mwpc2_track_Tx = new TH1F("h_mwpc2_track_Tx", "Tx distribution of tracks in MWPC2", 100, -TxBound, TxBound);

TH1F* h_mwpc0_track_Ty = new TH1F("h_mwpc0_track_Ty", "Ty distribution of tracks in MWPC0", 100, -TyBound, TyBound);
TH1F* h_mwpc1_track_Ty = new TH1F("h_mwpc1_track_Ty", "Ty distribution of tracks in MWPC1", 100, -TyBound, TyBound);
TH1F* h_mwpc2_track_Ty = new TH1F("h_mwpc2_track_Ty", "Ty distribution of tracks in MWPC2", 100, -TyBound, TyBound);

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

//MWPC matched tracks
TH1F* h_mwpc_matched_track_Tx = new TH1F("h_mwpc_matched_track_Tx", "Tx distribution of tracks in MWPC", mwpcN, -0.1, 0.1);
TH1F* h_mwpc_matched_track_Ty = new TH1F("h_mwpc_matched_track_Ty", "Ty distribution of tracks in MWPC", mwpcN, -0.1, 0.1);
TH1I* h_mwpc_matched_track_nHits = new TH1I("h_mwpc_matched_track_nHits", "Number of hit distribution of MWPC tracks", 8, 0, 8);
TH1F* h_mwpc_matched_track_chi2 = new TH1F("h_mwpc_matched_track_chi2", "#Chi^2 distribution of tracks in MWPC", mwpcN, 0.0, 10.0);
TH2F* h_mwpc_matched_track_XYstart = new TH2F("h_mwpc_matched_track_XYstart", "Start hits distribution of track in XY plane in MWPC", mwpcN, -mwpcX, mwpcX, mwpcN, -mwpcY, mwpcY);
TH1F* h_mwpc_matched_track_Xstart = new TH1F("h_mwpc_matched_track_Xstart", "Start hits distribution of track in X axis in MWPC", mwpcN, -mwpcX, mwpcX);
TH1F* h_mwpc_matched_track_Ystart = new TH1F("h_mwpc_matched_track_Ystart", "Start hits distribution of track in Y axis in MWPC", mwpcN, -mwpcY, mwpcY);
TH1I* h_mwpc_matched_track_nTracks = new TH1I("h_mwpc_matched_track_nTracks", "Number of tracks distribution in MWPC", 6, -1, 5);

TH1F* h_mwpc_matched_track_dist_3d = new TH1F("h_mwpc_matched_track_dist_3d", "h_mwpc_matched_track_dist_3d", mwpcN, 0.0, res_bound);

void recoRun1() {

    UInt_t nEvents = 10000; // zero corresponds to ALL EVENTS in run

    reco(648, nEvents);

    qa(648, nEvents);

    fOut->Write();
    fOut->Close();
}

void reco(Int_t runId = 648, Int_t nEvents = 10000) {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TString geoName = (runId >= 12 && runId <= 188) ? "geometry_run1" :
            (runId >= 220 && runId <= 395) ? "geometry_run2" :
            (runId >= 403 && runId <= 688) ? "geometry_run3" :
            "WRONG runId = " + runId + "! NO GEOMETRY FOUND!!!";

    cout << "INFO: Geometry type: " << geoName << endl;
    if (geoName.Contains("NO GEOMETRY FOUND!!!")) return;

    TGeoManager::Import("geometry_run/" + geoName + ".root");

    TChain *bmnTree = new TChain("BMN_DIGIT");
    bmnTree->Add(TString::Format("bmn_run0%d_digit.root", runId));

    TChain *bmnRawTree = new TChain("BMN_RAW");
    bmnRawTree->Add(TString::Format("bmn_run0%d_eb+hrb.root", runId));

    TClonesArray *dchDigits;
    TClonesArray *mwpcDigits;
    //    TClonesArray *tof400Digits;
    //    TClonesArray *tof700Digits;
    //    TClonesArray *zdcDigits;
    bmnTree->SetBranchAddress("bmn_dch_digit", &dchDigits);
    //    bmnTree->SetBranchAddress("bmn_tof1_digit", &tof400Digits);
    //    bmnTree->SetBranchAddress("bmn_tof2_digit", &tof700Digits);
    //    bmnTree->SetBranchAddress("bmn_zdc_digit", &zdcDigits);
    bmnRawTree->SetBranchAddress("bmn_mwpc", &mwpcDigits);

    Int_t startEvent = 0;
    Int_t events = (nEvents == 0) ? bmnTree->GetEntries() : nEvents;

    TFile* fReco = new TFile(TString::Format("bmndst_run%d.root", runId), "RECREATE");
    TTree* tReco = new TTree("cbmsim", TString::Format("bmndst_run%d", runId));

    TClonesArray* dchHits = new TClonesArray("BmnDchHit");
    TClonesArray* mwpcHits = new TClonesArray("BmnMwpcHit");
    TClonesArray* dchTracks = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks0 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks1 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcTracks2 = new TClonesArray("CbmTrack");
    TClonesArray* mwpcMatchedTracks = new TClonesArray("CbmTrack");

    tReco->Branch("BmnDchHit", &dchHits);
    tReco->Branch("BmnMwpcHit", &mwpcHits);
    tReco->Branch("DchTracks", &dchTracks);
    tReco->Branch("Mwpc0Seeds", &mwpcTracks0);
    tReco->Branch("Mwpc1Seeds", &mwpcTracks1);
    tReco->Branch("Mwpc2Seeds", &mwpcTracks2);
    tReco->Branch("MwpcMatchedTracks", &mwpcMatchedTracks);


//    cout << "DCH tracks reading: START" << endl;
//    //File is prepared by Nikolay Voytishin
//    Int_t nEv = 0;
//    Float_t x1, y1, z1;
//    Float_t x2, y2, z2;
//
//    ifstream ifs;
//    ifs.open("dc_segments_run0648.txt", ifstream::in);
//    //    while (!ifs.eof()) {
//    for (Int_t iTr = 0; iTr < 15; ++iTr) {
//        dchHits->Clear();
//        dchTracks->Clear();
//        ifs >> nEv >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
//        cout << nEv << " " << x1 << " " << y1 << " " << z1 << " " << x2 << " " << y2 << " " << z2 << endl;
//        TVector3 pos1(x1, y1, z1);
//        new((*dchHits)[dchHits->GetEntriesFast()]) BmnDchHit(0, pos1, TVector3(0, 0, 0), 0);
//        TVector3 pos2(x2, y2, z2);
//        new((*dchHits)[dchHits->GetEntriesFast()]) BmnDchHit(0, pos2, TVector3(0, 0, 0), 0);
//        new((*dchTracks)[dchTracks->GetEntriesFast()]) CbmTrack();
//        CbmTrack* tr = (CbmTrack*) dchTracks->At(dchTracks->GetEntriesFast() - 1);
//        tr->AddHit(dchHits->GetEntriesFast() - 2, (HitType) 0);
//        tr->AddHit(dchHits->GetEntriesFast() - 1, (HitType) 0);
//        tReco->Fill();
//    }
//
//    ifs.close();
//    cout << "DCH tracks reading: FINISH" << endl;

    cout << "Time Selection: START" << endl;

    TH1F* h_times0 = new TH1F("h_times0", "mwpc0_times", 40, 0.0, 40.0);
    TH1F* h_times1 = new TH1F("h_times1", "mwpc1_times", 40, 0.0, 40.0);
    TH1F* h_times2 = new TH1F("h_times2", "mwpc2_times", 40, 0.0, 40.0);
    for (Int_t iEv = startEvent; iEv < startEvent + events; iEv++) {
        if (iEv % 1000 == 0) cout << "TIME SEECTION: \tRUN#" << runId << "\tEvent: " << iEv + 1 << "/" << startEvent + events << endl;
        bmnRawTree->GetEntry(iEv);
        for (Int_t iDig = 0; iDig < mwpcDigits->GetEntriesFast(); ++iDig) {
            BmnMwpcDigit* digi = (BmnMwpcDigit*) mwpcDigits->At(iDig);
            Short_t plane = digi->GetPlane();
            if (plane < 6)
                h_times0->Fill(digi->GetTime());
            else if (plane < 12)
                h_times1->Fill(digi->GetTime());
            else if (plane < 18)
                h_times2->Fill(digi->GetTime());
        }
    }

    DigitsTimeSelection(h_times0, mwpc0_leftTime, mwpc0_rightTime);
    DigitsTimeSelection(h_times1, mwpc1_leftTime, mwpc1_rightTime);
    DigitsTimeSelection(h_times2, mwpc2_leftTime, mwpc2_rightTime);

    delete h_times0;
    delete h_times1;
    delete h_times2;

    cout << "Time Selection: FINISH" << endl;

    for (Int_t iEv = startEvent; iEv < startEvent + events; iEv++) {
        bmnTree->GetEntry(iEv);
        bmnRawTree->GetEntry(iEv);

        dchHits->Clear();
        mwpcHits->Clear();
        dchTracks->Clear();
        mwpcTracks0->Clear();
        mwpcTracks1->Clear();
        mwpcTracks2->Clear();
        mwpcMatchedTracks->Clear();

        if (iEv % 1000 == 0) cout << "RECO: \tRUN#" << runId << "\tEvent: " << iEv + 1 << "/" << startEvent + events << endl;
        cout << "Event: " << iEv << endl;

        /* ======= Functions for hits reconstruction ======= */
        ProcessDchDigits(dchDigits, dchHits);
        ProcessMwpcDigits(mwpcDigits, mwpcHits);

        /* ======= Functions for "seeding" ======= */
        DchTrackFinder(dchHits, dchTracks);
        MwpcTrackFinder(mwpcHits, mwpcTracks0, 0);
        MwpcTrackFinder(mwpcHits, mwpcTracks1, 1);
        MwpcTrackFinder(mwpcHits, mwpcTracks2, 2);

        /* ======= Functions for tracks matching ======= */

        //        MwpcTrackMatching(mwpcHits, mwpcMatchedTracks, mwpcTracks0, mwpcTracks1);
        MwpcTrackMatchingByAllHits(mwpcHits, mwpcMatchedTracks);

        tReco->Fill();
    } // event loop

    tReco->Write();
    fReco->Close();

    delete bmnTree;
    delete bmnRawTree;

}

void qa(Int_t runId = 650, Int_t nEvents = 0) {

    /* Load basic libraries */
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    TChain *dstTree = new TChain("cbmsim");
    dstTree->Add(TString::Format("bmndst_run%d.root", runId));

    TChain *bmnRawTree = new TChain("BMN_RAW");
    bmnRawTree->Add(TString::Format("bmn_run0%d_eb+hrb.root", runId));

    TClonesArray *dchTracks;
    dstTree->SetBranchAddress("DchTracks", &dchTracks);
    TClonesArray *dchHits;
    dstTree->SetBranchAddress("BmnDchHit", &dchHits);
    TClonesArray *mwpcTracks0;
    dstTree->SetBranchAddress("Mwpc0Seeds", &mwpcTracks0);
    TClonesArray *mwpcTracks1;
    dstTree->SetBranchAddress("Mwpc1Seeds", &mwpcTracks1);
    TClonesArray *mwpcTracks2;
    dstTree->SetBranchAddress("Mwpc2Seeds", &mwpcTracks2);
    TClonesArray *mwpcHits;
    dstTree->SetBranchAddress("BmnMwpcHit", &mwpcHits);
    TClonesArray *mwpcDigits;
    bmnRawTree->SetBranchAddress("bmn_mwpc", &mwpcDigits);
    TClonesArray *mwpcMatchedTracks;
    dstTree->SetBranchAddress("MwpcMatchedTracks", &mwpcMatchedTracks);

    Int_t events = (nEvents == 0) ? dstTree->GetEntries() : nEvents;

    for (Int_t iEv = 0; iEv < events; iEv++) {
        dstTree->GetEntry(iEv);
        bmnRawTree->GetEntry(iEv);

        if (iEv % 1000 == 0) cout << "QA: \tRUN#" << runId << "\tEvent: " << iEv + 1 << "/" << events << endl;
        //        for (Int_t glIdx = 0; glIdx < /*dchTracks->GetEntriesFast()*/ 0; ++glIdx) {
        //            CbmTrack* track = (CbmTrack*) dchTracks->At(glIdx);
        //            FairTrackParam* par = track->GetParamFirst();
        //            Float_t Tx = par->GetTx();
        //            Float_t Ty = par->GetTy();
        //            Float_t Az = par->GetQp() / Sqrt(Tx * Tx + Ty * Ty + 1.0);
        //            Float_t Ax = Az * Tx;
        //            Float_t Ay = Az * Ty;
        //            Float_t Vx = par->GetX();
        //            Float_t Vy = par->GetY();
        //            Float_t Vz = par->GetZ();
        //            h_nHits->Fill(track->GetNofHits());
        //            for (Int_t i = 0; i < track->GetNofHits(); ++i) {
        //                BmnDchHit* hit = (BmnDchHit*) dchHits->At(track->GetHitIndex(i));
        //                Short_t dchId = hit->GetDchId();
        //                Float_t Hx = hit->GetX();
        //                Float_t Hy = hit->GetY();
        //                Float_t Hz = hit->GetZ();
        //
        //                //3D
        //                Float_t dist = DistFromPointToLine3D(TVector3(Ax, Ay, Az), TVector3(Vx, Vy, Vz), TVector3(Hx, Hy, Hz));
        //                if (dchId == 1) {
        //                    h_dch1_dist_3d->Fill(dist);
        //                    h_dch1_dist_x->Fill(Hx, dist);
        //                    h_dch1_dist_y->Fill(Hy, dist);
        //                    h_dch1_dist_z->Fill(Hz, dist);
        //                    h_xy1->Fill(Hx, Hy);
        //
        //                } else if (dchId == 2) {
        //                    h_dch2_dist_3d->Fill(dist);
        //                    h_dch2_dist_x->Fill(Hx, dist);
        //                    h_dch2_dist_y->Fill(Hy, dist);
        //                    h_dch2_dist_z->Fill(Hz, dist);
        //                    h_xy2->Fill(Hx, Hy);
        //                }
        //            }
        //        }

        //MWPC DIGI
        Int_t digiCounter0 = 0;
        Int_t digiCounter1 = 0;
        Int_t digiCounter2 = 0;

        for (Int_t digId = 0; digId < mwpcDigits->GetEntriesFast(); ++digId) {
            BmnMwpcDigit* digi = (BmnMwpcDigit*) mwpcDigits->At(digId);
            Short_t plane = digi->GetPlane();
            Short_t time = digi->GetTime();

            if (plane / 6 == 0) {
                if (time < mwpc0_leftTime || time > mwpc0_rightTime) continue;
                h_mwpc1_digit_time->Fill(time);
                digiCounter1++;
            } else if (plane / 6 == 1) {
                if (time < mwpc1_leftTime || time > mwpc1_rightTime) continue;
                h_mwpc0_digit_time->Fill(time);
                digiCounter0++;
            } else if (plane / 6 == 2) {
                if (time < mwpc2_leftTime || time > mwpc2_rightTime) continue;
                h_mwpc2_digit_time->Fill(time);
                digiCounter2++;
            }
            h_mwpc_digit_planes->Fill(plane);
        }
        h_mwpc0_digit_nDigits->Fill(digiCounter0);
        h_mwpc1_digit_nDigits->Fill(digiCounter1);
        h_mwpc2_digit_nDigits->Fill(digiCounter2);

        //MWPC HIT
        Int_t hitCounter0 = 0;
        Int_t hitCounter1 = 0;
        Int_t hitCounter2 = 0;
                       
        for (Int_t hitId = 0; hitId < mwpcHits->GetEntriesFast(); ++hitId) {
            BmnMwpcHit* mwpcHit = (BmnMwpcHit*) mwpcHits->At(hitId);
            Short_t mwpcId = mwpcHit->GetMwpcId();
            Float_t X = mwpcHit->GetX();
            Float_t Y = mwpcHit->GetY();
            Float_t Z = mwpcHit->GetZ();
            if (mwpcId == 0) {
                h_mwpc0_hit_XY->Fill(X, Y);
                h_mwpc0_hit_X->Fill(X);
                h_mwpc0_hit_Y->Fill(Y);
                h_mwpc0_hit_Z->Fill(Z);
                hitCounter0++;
            } else if (mwpcId == 1) {
                h_mwpc1_hit_XY->Fill(X, Y);
                h_mwpc1_hit_X->Fill(X);
                h_mwpc1_hit_Y->Fill(Y);
                h_mwpc1_hit_Z->Fill(Z);
                hitCounter1++;
            } else if (mwpcId == 2) {
                h_mwpc2_hit_XY->Fill(X, Y);
                h_mwpc2_hit_X->Fill(X);
                h_mwpc2_hit_Y->Fill(Y);
                h_mwpc2_hit_Z->Fill(Z);
                hitCounter2++;
            }
        }
        h_mwpc0_hit_nHits->Fill(hitCounter0);
        h_mwpc1_hit_nHits->Fill(hitCounter1);
        h_mwpc2_hit_nHits->Fill(hitCounter2);

        //MWPC TRACK
        for (Int_t trId = 0; trId < mwpcTracks0->GetEntriesFast(); ++trId) {
            CbmTrack* tr = (CbmTrack*) mwpcTracks0->At(trId);
            FairTrackParam* par = tr->GetParamFirst();
            Float_t X = par->GetX();
            Float_t Y = par->GetY();
            h_mwpc0_track_Tx->Fill(par->GetTx());
            h_mwpc0_track_Ty->Fill(par->GetTy());
            h_mwpc0_track_nHits->Fill(tr->GetNDF());
            h_mwpc0_track_chi2->Fill(tr->GetChiSq());
            h_mwpc0_track_XYstart->Fill(X, Y);
            h_mwpc0_track_Xstart->Fill(X);
            h_mwpc0_track_Ystart->Fill(Y);
        }
        for (Int_t trId = 0; trId < mwpcTracks1->GetEntriesFast(); ++trId) {
            CbmTrack* tr = (CbmTrack*) mwpcTracks1->At(trId);
            FairTrackParam* par = tr->GetParamFirst();
            Float_t X = par->GetX();
            Float_t Y = par->GetY();
            h_mwpc1_track_Tx->Fill(par->GetTx());
            h_mwpc1_track_Ty->Fill(par->GetTy());
            h_mwpc1_track_nHits->Fill(tr->GetNDF());
            h_mwpc1_track_chi2->Fill(tr->GetChiSq());
            h_mwpc1_track_XYstart->Fill(X, Y);
            h_mwpc1_track_Xstart->Fill(X);
            h_mwpc1_track_Ystart->Fill(Y);
        }
        //        for (Int_t trId = 0; trId < mwpcTracks2->GetEntriesFast(); ++trId) {
        //            CbmTrack* tr = (CbmTrack*) mwpcTracks2->At(trId);
        //            FairTrackParam* par = tr->GetParamFirst();
        //            Float_t X = par->GetX();
        //            Float_t Y = par->GetY();
        //            h_mwpc2_track_Tx->Fill(par->GetTx());
        //            h_mwpc2_track_Ty->Fill(par->GetTy());
        //            h_mwpc2_track_nHits->Fill(tr->GetNDF());
        //            h_mwpc2_track_chi2->Fill(tr->GetChiSq());
        //            h_mwpc2_track_XYstart->Fill(X, Y);
        //            h_mwpc2_track_Xstart->Fill(X);
        //            h_mwpc2_track_Ystart->Fill(Y);
        //        }

        h_mwpc0_track_nTracks->Fill(mwpcTracks0->GetEntriesFast());
        h_mwpc1_track_nTracks->Fill(mwpcTracks1->GetEntriesFast());
        //        h_mwpc2_track_nTracks->Fill(mwpcTracks2->GetEntriesFast());

        //MWPC matched tracks  
        for (Int_t trId = 0; trId < mwpcMatchedTracks->GetEntriesFast(); ++trId) {
            CbmTrack* tr = (CbmTrack*) mwpcMatchedTracks->At(trId);
            FairTrackParam* par = tr->GetParamFirst();
            Float_t X = par->GetX();
            Float_t Y = par->GetY();
            Float_t Z = par->GetZ();
            Float_t Tx = par->GetTx();
            Float_t Ty = par->GetTy();
            Float_t Az = par->GetQp() / Sqrt(Tx * Tx + Ty * Ty + 1.0);
            Float_t Ax = Az * Tx;
            Float_t Ay = Az * Ty;
            //            for (Int_t i = 0; i < tr->GetNofHits(); ++i) {
            //                BmnMwpcHit* mwpcHit = (BmnMwpcHit*) mwpcHits->At(tr->GetHitIndex(i));
            //                Float_t Hx = mwpcHit->GetX();
            //                Float_t Hy = mwpcHit->GetY();
            //                Float_t Hz = mwpcHit->GetZ();
            //
            //                Float_t dist = DistFromPointToLine3D(TVector3(Ax, Ay, Az), TVector3(X, Y, Z), TVector3(Hx, Hy, Hz));
            //                h_mwpc_matched_track_dist_3d->Fill(dist);
            //            }

            h_mwpc_matched_track_Tx->Fill(Tx);
            h_mwpc_matched_track_Ty->Fill(Ty);
            h_mwpc_matched_track_nHits->Fill(tr->GetNDF());
            h_mwpc_matched_track_chi2->Fill(tr->GetChiSq());
            h_mwpc_matched_track_XYstart->Fill(X, Y);
            h_mwpc_matched_track_Xstart->Fill(X);
            h_mwpc_matched_track_Ystart->Fill(Y);
        }

        h_mwpc_matched_track_nTracks->Fill(mwpcMatchedTracks->GetEntriesFast());

    } // event loop    
}

Float_t DistFromPointToLine2D(Float_t A, Float_t B, Float_t C, Float_t Mx, Float_t My) {
    // Distance from point (Mx, My) to line Ax + By + C = 0 is equal:
    // d = |A * Mx + B * My + C| / Sqrt(A^2 + B^2)
    return Abs(A * Mx + B * My + C) / Sqrt(A * A + B * B);
}

Float_t DistFromPointToLine3D(TVector3 direction, TVector3 vertex, TVector3 hit) {
    TVector3 VertHit = vertex - hit;
    TVector3 VectProd = VertHit.Cross(direction);
    Float_t d = VectProd.Mag() / direction.Mag();
    return d;
}