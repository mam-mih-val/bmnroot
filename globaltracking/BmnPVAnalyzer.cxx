#include "BmnPVAnalyzer.h"


using namespace std;
using namespace TMath;

BmnPVAnalyzer::BmnPVAnalyzer(Int_t period, Bool_t isField) {
    fPeriodId = period;
    fGlobalTracksArray = nullptr;
    fDstEventHeader = nullptr;
    fNTracks = 0;
    fRoughVertex3D = (fPeriodId == 7) ? TVector3(0.5, -4.6, -1.0) : (fPeriodId == 6) ? TVector3(0.0, -3.5, -21.9) : TVector3(0.0, 0.0, 0.0);
    fCBMFrameShift3D = TVector3(0.5, -7.06, -1.63);
    fIsField = isField;
#ifdef CBM_TRACKS
    fGlobalTracksBranchName = "StsTrack";
    fDstEHBranchName = "EventHeaderBmn";
    fRoughVertex3D = TVector3(0.22, 2.46, 0.584); // 7 period
#else
    fGlobalTracksBranchName = "BmnGlobalTrack";
    fDstEHBranchName = "DstEventHeader.";
#endif
    fVertexBranchName = "BmnVertex2";

}

BmnPVAnalyzer::~BmnPVAnalyzer() {
}

InitStatus BmnPVAnalyzer::Init() {
    if (fVerbose > 1) cout << "=========================== BmnPVAnalyzer init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) Fatal("Init", "FairRootManager is not instantiated");

    fGlobalTracksArray = (TClonesArray*) ioman->GetObject(fGlobalTracksBranchName); //in
    if (!fGlobalTracksArray) {
        cout << "BmnPVAnalyzer::Init(): branch " << fGlobalTracksBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
#ifdef CBM_TRACKS
    fDstEventHeader = (TClonesArray*) ioman->GetObject(fDstEHBranchName); //in
#else
    fDstEventHeader = (DstEventHeader*) ioman->GetObject(fDstEHBranchName); //in
#endif

    if (!fDstEventHeader) {
        cout << "BmnPVAnalyzer::Init(): branch " << fDstEHBranchName << " not found! Task will be deactivated" << endl;
        SetActive(kFALSE);
        return kERROR;
    }
    fVertexArray = new TClonesArray(CbmVertex::Class()); //out
    ioman->Register(fVertexBranchName, "GEM", fVertexArray, kTRUE);
    fVertexArrayAll = new TClonesArray(CbmVertex::Class()); //out
    ioman->Register("BmnVertex2All", "GEM", fVertexArrayAll, kTRUE);


    //    txyz = new TTree("data", "");
    //    txyz->Branch("evNo", &evNo, "evNo/I");
    //    txyz->Branch("pv[4]", &pv, "pv[4]/F");
    //    txyz->Branch("FirstPar[6]", &FirstPar, "FirstPar[6]/F");
    //    txyz->Branch("LastPar[6]", &LastPar, "LastPar[6]/F");
    //    parFileList = new TList();
    //    TObjString stsDigiFile = "/data1/pokat/soft_run6/sts_v18BMN_gem.digi.par";
    //    parFileList->Add(&stsDigiFile);
    //    GetBmnGeom("/data1/pokat/soft_run6/SIGEMS_v1.root"); //"GEMS_RunWinter2016_old.root"); //SIGEMS_v1
    ApplyAlignment();
    //    FairTask *pKF = new CbmKF();
    //    fRun->AddTask(pKF);
    //    fRun->Init();

    TChain *rec = FairRootManager::Instance()->GetInChain();
    //    TClonesArray *stsTrack = (TClonesArray *) FairRootManager::Instance()->GetObject("StsTrack");
    //    CbmVertex *primVtx = (CbmVertex *) FairRootManager::Instance()->GetObject("PrimaryVertex.");
    CbmStsKFTrackFitter fitter;
    fitter.Init();

    events = rec->GetEntries();

    InitHists();
    //        InitHistsVP();
    t_min = 1000000000;
    t_max = -1000000000;




    if (fVerbose > 1) cout << "=========================== BmnPVAnalyzer init finished ===================" << endl;

    return kSUCCESS;
}

void BmnPVAnalyzer::Exec(Option_t* opt) {
    if (!IsActive())
        return;
    if (fVerbose > 1) cout << "======================== BmnPVAnalyzer exec started  ======================" << endl;

    TStopwatch sw;
    sw.Start();

    ProcessEvent();
    //        ProcessEventVP();

    sw.Stop();
    fTime += sw.RealTime();
    if (fVerbose > 1) cout << "\n======================== BmnPVAnalyzer exec finished ======================" << endl;

}

void BmnPVAnalyzer::Finish() {
    if (fVerbose > 0) printf("hist file name %s\n", hhist->GetName());
    hhist->Write();
    hhist->Close();
    delete hhist;
    //    ofstream outFile;
    //    outFile.open("QA/timing.txt", ofstream::app);
    //    outFile << "Vertex Finder Time: " << fTime;
    if (fVerbose == 0) cout << "Work time of the BmnPVAnalyzer: " << fTime << endl;
}

void BmnPVAnalyzer::InitHists() {
    nh_bins = 200;
    //======================================================================
    //  Open histograms file
    //
#ifdef CBM_TRACKS
    hhist = new TFile("my_hist_file_mz_cbm.root", "RECREATE");
#else
    hhist = new TFile("my_hist_file_mz.root", "RECREATE");
#endif
    hhist->cd();
    cdTracks = hhist->mkdir("Tracks");
    cdTracks->cd();

    hh1[1] = new TH1D("hh1_1", "Time per event", 1000, 0., 9000000.);
    hh1[2] = new TH1D("hh1_2", "Time per event low mult", 1000, 0., 9000000.);
    hh1[3] = new TH1D("hh1_3", "Time per event high mult", 1000, 0., 9000000.);

    hh1[10] = new TH1D("hh1_10", "TRK Mult", 200, 0., 200.);
    hh1[11] = new TH1D("hh1_11", "TRK Mult", 200, 0., 200.);

    hh1[16] = new TH1D("hh1_16", "Z of Track", 700, 0.0, 140.);

    hh1[21] = new TH1D("hh1_21", "PVX", 200, -20., 20.);
    hh1[22] = new TH1D("hh1_22", "PVY", 200, -20., 20.);
    hh1[23] = new TH1D("hh1_23", "PVZ", 1200, -450., 150.);
    hh1[24] = new TH1D("hh1_24", "Z Prim", 400, -33., -13.);

    hh1[25] = new TH1D("hh1_25", "PVX", 200, -20., 20.);
    hh1[26] = new TH1D("hh1_26", "PVY", 200, -20., 20.);
    hh1[27] = new TH1D("hh1_27", "PVZ", 1200, -450., 150.);
    hh1[28] = new TH1D("hh1_28", "Z Prim", 400, -33., -13.);

    hh1[30] = new TH1D("hh1_30", "qP", 150, -15., 15.);

    hh1[41] = new TH1D("hh1_41", "X vtx", 200, -20., 20.);
    hh1[42] = new TH1D("hh1_42", "Y vtx", 200, -20., 20.);
    hh1[43] = new TH1D("hh1_43", "Z vtx", 1200, -450., 150.);
    hh1[44] = new TH1D("hh1_44", "CDA vtx", 200, 0., 20.);

    hh2[131] = new TH2D("hh2_131", "x1/x2 0-2GeV", 100, -50., 50., 100, -50., 50.);
    hh2[132] = new TH2D("hh2_132", "y1/y2 0-2GeV", 100, -50., 50., 100, -50., 50.);

    hh2[133] = new TH2D("hh2_133", "x1/x2 2-4GeV", 100, -50., 50., 100, -50., 50.);
    hh2[134] = new TH2D("hh2_134", "y1/y2 2-4GeV", 100, -50., 50., 100, -50., 50.);

    hh2[135] = new TH2D("hh2_135", "x1/x2 4-6GeV", 100, -50., 50., 100, -50., 50.);
    hh2[136] = new TH2D("hh2_136", "y1/y2 4-6GeV", 100, -50., 50., 100, -50., 50.);

    hh2[141] = new TH2D("hh2_141", "x1/x2 0.0-0.5GeV", 100, -50., 50., 100, -50., 50.);
    hh2[142] = new TH2D("hh2_142", "x1/x2 0.5-1.0GeV", 100, -50., 50., 100, -50., 50.);
    hh2[143] = new TH2D("hh2_143", "x1/x2 1.0-1.5GeV", 100, -50., 50., 100, -50., 50.);
    hh2[144] = new TH2D("hh2_144", "x1/x2 1.5-2.0GeV", 100, -50., 50., 100, -50., 50.);
    hh2[145] = new TH2D("hh2_145", "x1/x2 2.0-2.5GeV", 100, -50., 50., 100, -50., 50.);
    hh2[146] = new TH2D("hh2_146", "x1/x2 2.5-3.0GeV", 100, -50., 50., 100, -50., 50.);
    hh2[147] = new TH2D("hh2_147", "x1/x2 3.0-3.5GeV", 100, -50., 50., 100, -50., 50.);
    hh2[148] = new TH2D("hh2_148", "x1/x2 3.5-4.0GeV", 100, -50., 50., 100, -50., 50.);
    hh2[149] = new TH2D("hh2_149", "x1/x2 4.0-4.5GeV", 100, -50., 50., 100, -50., 50.);
    hh2[150] = new TH2D("hh2_150", "x1/x2 4.5-5.0GeV", 100, -50., 50., 100, -50., 50.);
    hh2[151] = new TH2D("hh2_151", "x1/x2 5.0-5.5GeV", 100, -50., 50., 100, -50., 50.);

    hh2[161] = new TH2D("hh2_161", " pZ/pT GeV, Pos", 60, 0., 15., 30, 0., 3.);
    hh2[162] = new TH2D("hh2_162", " pZ/pT GeV, Neg", 60, 0., 15., 30, 0., 3.);

    //--------------------------------------------------------------------------------------
    hh1[201] = new TH1D("hh1_201", "TRK Mult", 50, 0., 50.);
    hh1[202] = new TH1D("hh1_202", "TRK Mult", 50, 0., 50.);
    hh1[203] = new TH1D("hh1_203", "TRK Mult", 50, 0., 50.);
    hh1[204] = new TH1D("hh1_204", "TRK Mult", 50, 0., 50.);
    hh1[205] = new TH1D("hh1_205", "TRK Mult", 50, 0., 50.);
    hh1[206] = new TH1D("hh1_206", "TRK Mult", 50, 0., 50.);

    //--------------------------------------------------------------------------------------
    hh1[301] = new TH1D("hh1_301", "N Hits Pos", 100, 0., 100.);
    hh1[302] = new TH1D("hh1_302", "N Hits Neg", 100, 0., 100.);
    //--------------------------------------------------------------------------------------
    hh1[310] = new TH1D("hh1_310", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[310]->SetMinimum(0.);

    hh1[311] = new TH1D("hh1_311", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[311]->SetMinimum(0.);

    hh1[350] = new TH1D("hh1_350", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[350]->SetMinimum(0.);

    hh1[351] = new TH1D("hh1_351", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[351]->SetMinimum(0.);

    hh1[352] = new TH1D("hh1_352", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[352]->SetMinimum(0.);

    hh1[353] = new TH1D("hh1_353", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[353]->SetMinimum(0.);

    hh1[354] = new TH1D("hh1_354", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[354]->SetMinimum(0.);

    hh1[355] = new TH1D("hh1_355", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[355]->SetMinimum(0.);

    hh1[356] = new TH1D("hh1_356", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[356]->SetMinimum(0.);

    hh1[357] = new TH1D("hh1_357", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[357]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------

    hh1[410] = new TH1D("hh1_410", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[410]->SetMinimum(0.);

    hh1[411] = new TH1D("hh1_411", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[411]->SetMinimum(0.);

    hh1[450] = new TH1D("hh1_450", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[450]->SetMinimum(0.);

    hh1[451] = new TH1D("hh1_451", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[451]->SetMinimum(0.);

    hh1[452] = new TH1D("hh1_452", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[452]->SetMinimum(0.);

    hh1[453] = new TH1D("hh1_453", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[453]->SetMinimum(0.);

    hh1[454] = new TH1D("hh1_454", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[454]->SetMinimum(0.);

    hh1[455] = new TH1D("hh1_455", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[455]->SetMinimum(0.);

    hh1[456] = new TH1D("hh1_456", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[456]->SetMinimum(0.);

    hh1[457] = new TH1D("hh1_457", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[457]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------

    hh1[510] = new TH1D("hh1_510", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[510]->SetMinimum(0.);

    hh1[511] = new TH1D("hh1_511", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[511]->SetMinimum(0.);

    hh1[550] = new TH1D("hh1_550", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[550]->SetMinimum(0.);

    hh1[551] = new TH1D("hh1_551", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[551]->SetMinimum(0.);

    hh1[552] = new TH1D("hh1_552", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[552]->SetMinimum(0.);

    hh1[553] = new TH1D("hh1_553", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[553]->SetMinimum(0.);

    hh1[554] = new TH1D("hh1_554", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[554]->SetMinimum(0.);

    hh1[555] = new TH1D("hh1_555", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[555]->SetMinimum(0.);

    hh1[556] = new TH1D("hh1_556", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[556]->SetMinimum(0.);

    hh1[557] = new TH1D("hh1_557", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[557]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    hh1[2310] = new TH1D("hh1_2310", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2310]->SetMinimum(0.);

    hh1[2311] = new TH1D("hh1_2311", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2311]->SetMinimum(0.);

    hh1[2350] = new TH1D("hh1_2350", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2350]->SetMinimum(0.);

    hh1[2351] = new TH1D("hh1_2351", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2351]->SetMinimum(0.);

    hh1[2352] = new TH1D("hh1_2352", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2352]->SetMinimum(0.);

    hh1[2353] = new TH1D("hh1_2353", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2353]->SetMinimum(0.);

    hh1[2354] = new TH1D("hh1_2354", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2354]->SetMinimum(0.);

    hh1[2355] = new TH1D("hh1_2355", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2355]->SetMinimum(0.);

    hh1[2356] = new TH1D("hh1_2356", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2356]->SetMinimum(0.);

    hh1[2357] = new TH1D("hh1_2357", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2357]->SetMinimum(0.);

    hh1[2358] = new TH1D("hh1_2358", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2358]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[2410] = new TH1D("hh1_2410", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2410]->SetMinimum(0.);

    hh1[2411] = new TH1D("hh1_2411", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2411]->SetMinimum(0.);

    hh1[2450] = new TH1D("hh1_2450", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2450]->SetMinimum(0.);

    hh1[2451] = new TH1D("hh1_2451", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2451]->SetMinimum(0.);

    hh1[2452] = new TH1D("hh1_2452", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2452]->SetMinimum(0.);

    hh1[2453] = new TH1D("hh1_2453", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2453]->SetMinimum(0.);

    hh1[2454] = new TH1D("hh1_2454", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2454]->SetMinimum(0.);

    hh1[2455] = new TH1D("hh1_2455", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2455]->SetMinimum(0.);

    hh1[2456] = new TH1D("hh1_2456", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2456]->SetMinimum(0.);

    hh1[2457] = new TH1D("hh1_2457", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2457]->SetMinimum(0.);

    hh1[2458] = new TH1D("hh1_2458", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2458]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[2510] = new TH1D("hh1_2510", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2510]->SetMinimum(0.);

    hh1[2511] = new TH1D("hh1_2511", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2511]->SetMinimum(0.);

    hh1[2550] = new TH1D("hh1_2550", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2550]->SetMinimum(0.);

    hh1[2551] = new TH1D("hh1_2551", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2551]->SetMinimum(0.);

    hh1[2552] = new TH1D("hh1_2552", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2552]->SetMinimum(0.);

    hh1[2553] = new TH1D("hh1_2553", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2553]->SetMinimum(0.);

    hh1[2554] = new TH1D("hh1_2554", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2554]->SetMinimum(0.);

    hh1[2555] = new TH1D("hh1_2555", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2555]->SetMinimum(0.);

    hh1[2556] = new TH1D("hh1_2556", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2556]->SetMinimum(0.);

    hh1[2557] = new TH1D("hh1_2557", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2557]->SetMinimum(0.);

    hh1[2558] = new TH1D("hh1_2558", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2558]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    hh1[360] = new TH1D("hh1_360", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[360]->SetMinimum(0.);

    hh1[361] = new TH1D("hh1_361", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[361]->SetMinimum(0.);

    hh1[362] = new TH1D("hh1_362", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[362]->SetMinimum(0.);

    hh1[363] = new TH1D("hh1_363", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[363]->SetMinimum(0.);

    hh1[364] = new TH1D("hh1_364", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[364]->SetMinimum(0.);

    hh1[365] = new TH1D("hh1_365", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[365]->SetMinimum(0.);

    hh1[366] = new TH1D("hh1_366", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[366]->SetMinimum(0.);

    hh1[367] = new TH1D("hh1_367", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[367]->SetMinimum(0.);

    hh1[368] = new TH1D("hh1_368", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[368]->SetMinimum(0.);

    hh1[369] = new TH1D("hh1_369", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[369]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------

    hh1[460] = new TH1D("hh1_460", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[460]->SetMinimum(0.);

    hh1[461] = new TH1D("hh1_461", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[461]->SetMinimum(0.);

    hh1[462] = new TH1D("hh1_462", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[462]->SetMinimum(0.);

    hh1[463] = new TH1D("hh1_463", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[463]->SetMinimum(0.);

    hh1[464] = new TH1D("hh1_464", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[464]->SetMinimum(0.);

    hh1[465] = new TH1D("hh1_465", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[465]->SetMinimum(0.);

    hh1[466] = new TH1D("hh1_466", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[466]->SetMinimum(0.);

    hh1[467] = new TH1D("hh1_467", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[467]->SetMinimum(0.);

    hh1[468] = new TH1D("hh1_468", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[468]->SetMinimum(0.);

    hh1[469] = new TH1D("hh1_469", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[469]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------

    hh1[560] = new TH1D("hh1_560", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[560]->SetMinimum(0.);

    hh1[561] = new TH1D("hh1_561", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[561]->SetMinimum(0.);

    hh1[562] = new TH1D("hh1_562", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[562]->SetMinimum(0.);

    hh1[563] = new TH1D("hh1_563", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[563]->SetMinimum(0.);

    hh1[564] = new TH1D("hh1_564", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[564]->SetMinimum(0.);

    hh1[565] = new TH1D("hh1_565", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[565]->SetMinimum(0.);

    hh1[566] = new TH1D("hh1_566", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[566]->SetMinimum(0.);

    hh1[567] = new TH1D("hh1_567", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[567]->SetMinimum(0.);

    hh1[568] = new TH1D("hh1_568", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[568]->SetMinimum(0.);

    hh1[569] = new TH1D("hh1_569", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[569]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    hh1[2360] = new TH1D("hh1_2360", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2360]->SetMinimum(0.);

    hh1[2361] = new TH1D("hh1_2361", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2361]->SetMinimum(0.);

    hh1[2362] = new TH1D("hh1_2362", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2362]->SetMinimum(0.);

    hh1[2363] = new TH1D("hh1_2363", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2363]->SetMinimum(0.);

    hh1[2364] = new TH1D("hh1_2364", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2364]->SetMinimum(0.);

    hh1[2365] = new TH1D("hh1_2365", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2365]->SetMinimum(0.);

    hh1[2366] = new TH1D("hh1_2366", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2366]->SetMinimum(0.);

    hh1[2367] = new TH1D("hh1_2367", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2367]->SetMinimum(0.);

    hh1[2368] = new TH1D("hh1_2368", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2368]->SetMinimum(0.);

    hh1[2369] = new TH1D("hh1_2369", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[2369]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------

    hh1[2460] = new TH1D("hh1_2460", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2460]->SetMinimum(0.);

    hh1[2461] = new TH1D("hh1_2461", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2461]->SetMinimum(0.);

    hh1[2462] = new TH1D("hh1_2462", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2462]->SetMinimum(0.);

    hh1[2463] = new TH1D("hh1_2463", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2463]->SetMinimum(0.);

    hh1[2464] = new TH1D("hh1_2464", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2464]->SetMinimum(0.);

    hh1[2465] = new TH1D("hh1_2465", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2465]->SetMinimum(0.);

    hh1[2466] = new TH1D("hh1_2466", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2466]->SetMinimum(0.);

    hh1[2467] = new TH1D("hh1_2467", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2467]->SetMinimum(0.);

    hh1[2468] = new TH1D("hh1_2468", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2468]->SetMinimum(0.);

    hh1[2469] = new TH1D("hh1_2469", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2469]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------

    hh1[2560] = new TH1D("hh1_2560", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2560]->SetMinimum(0.);

    hh1[2561] = new TH1D("hh1_2561", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2561]->SetMinimum(0.);

    hh1[2562] = new TH1D("hh1_2562", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2562]->SetMinimum(0.);

    hh1[2563] = new TH1D("hh1_2563", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2563]->SetMinimum(0.);

    hh1[2564] = new TH1D("hh1_2564", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2564]->SetMinimum(0.);

    hh1[2565] = new TH1D("hh1_2565", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2565]->SetMinimum(0.);

    hh1[2566] = new TH1D("hh1_2566", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2566]->SetMinimum(0.);

    hh1[2567] = new TH1D("hh1_2567", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2567]->SetMinimum(0.);

    hh1[2568] = new TH1D("hh1_2568", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2568]->SetMinimum(0.);

    hh1[2569] = new TH1D("hh1_2569", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[2569]->SetMinimum(0.);

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    hh1[3310] = new TH1D("hh1_3310", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[3310]->SetMinimum(0.);

    hh1[3311] = new TH1D("hh1_3311", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[3311]->SetMinimum(0.);

    hh1[3350] = new TH1D("hh1_3350", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[3350]->SetMinimum(0.);

    hh1[3351] = new TH1D("hh1_3351", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[3351]->SetMinimum(0.);

    hh1[3352] = new TH1D("hh1_3352", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[3352]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[3410] = new TH1D("hh1_3410", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3410]->SetMinimum(0.);

    hh1[3411] = new TH1D("hh1_3411", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3411]->SetMinimum(0.);

    hh1[3450] = new TH1D("hh1_3450", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3450]->SetMinimum(0.);

    hh1[3451] = new TH1D("hh1_3451", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3451]->SetMinimum(0.);

    hh1[3452] = new TH1D("hh1_3452", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3452]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[3510] = new TH1D("hh1_3510", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3510]->SetMinimum(0.);

    hh1[3511] = new TH1D("hh1_3511", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3511]->SetMinimum(0.);

    hh1[3550] = new TH1D("hh1_3550", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3550]->SetMinimum(0.);

    hh1[3551] = new TH1D("hh1_3551", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3551]->SetMinimum(0.);

    hh1[3552] = new TH1D("hh1_3552", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[3552]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------


    //--------------------------------------------------------------------------------------
    hh1[4310] = new TH1D("hh1_4310", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[4310]->SetMinimum(0.);

    hh1[4311] = new TH1D("hh1_4311", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[4311]->SetMinimum(0.);

    hh1[4350] = new TH1D("hh1_4350", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[4350]->SetMinimum(0.);

    hh1[4351] = new TH1D("hh1_4351", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[4351]->SetMinimum(0.);

    hh1[4352] = new TH1D("hh1_4352", "M(#pi+#pi-) [Gev]. All V0s", nh_bins, HM_K0 - M_K0_Cut, HM_K0 + M_K0_Cut);
    hh1[4352]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[4410] = new TH1D("hh1_4410", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4410]->SetMinimum(0.);

    hh1[4411] = new TH1D("hh1_4411", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4411]->SetMinimum(0.);

    hh1[4450] = new TH1D("hh1_4450", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4450]->SetMinimum(0.);

    hh1[4451] = new TH1D("hh1_4451", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4451]->SetMinimum(0.);

    hh1[4452] = new TH1D("hh1_4452", "M(p#pi-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4452]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[4510] = new TH1D("hh1_4510", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4510]->SetMinimum(0.);

    hh1[4511] = new TH1D("hh1_4511", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4511]->SetMinimum(0.);

    hh1[4550] = new TH1D("hh1_4550", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4550]->SetMinimum(0.);

    hh1[4551] = new TH1D("hh1_4551", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4551]->SetMinimum(0.);

    hh1[4552] = new TH1D("hh1_4552", "M(#pi+p-) [GeV]. All V0s", nh_bins, HM_L0 - M_L0_Cut, HM_L0 + M_L0_Cut);
    hh1[4552]->SetMinimum(0.);
    //--------------------------------------------------------------------------------------

    hh1[599] = new TH1D("hh1_599", "CosT", 100, -1., 1.);

    hh2[600] = new TH2D("hh2_600", "Armentero", 200, -1.0, 1.0, 100, 0.0, 0.3);
    hh2[601] = new TH2D("hh2_601", "Armentero", 200, -1.0, 1.0, 100, 0.0, 0.3);

    //---------------------------------------------------------------------------------------------

    hh1[1020] = new TH1D("hh1_1020", "N Prim", 20, 0., 20.);
    hh1[1021] = new TH1D("hh1_1021", "X Prim", 200, -20., 20.);
    hh1[1022] = new TH1D("hh1_1022", "Y Prim", 200, -20., 20.);
    hh1[1023] = new TH1D("hh1_1023", "Z Prim", 1200, -450., 150.);
    hh1[1024] = new TH1D("hh1_1024", "Z Prim", 400, -33., -13.);

    hh1[1025] = new TH1D("hh1_1025", "Chi2 Prim", 500, 0., 0.5);
    hh1[1026] = new TH1D("hh1_1026", "CDA Prim", 500, 0., 0.5);
    hh1[1027] = new TH1D("hh1_1027", "Track Mult. in  Prim", 20, 0., 20.);

    hh1[1031] = new TH1D("hh1_1031", "X Prim", 200, -20., 20.);
    hh1[1032] = new TH1D("hh1_1032", "Y Prim", 200, -20., 20.);
    hh1[1033] = new TH1D("hh1_1033", "Z Prim", 1200, -450., 150.);
    hh1[1034] = new TH1D("hh1_1034", "Z Prim", 400, -33., -13.);
    hh2[1034] = new TH2D("hh2_1034", "X/Y Prim", 100, -4., 4., 100, -4., 4.);

    hh1[1035] = new TH1D("hh1_1035", "Chi2 Prim", 500, 0., 0.5);
    hh1[1036] = new TH1D("hh1_1036", "CDA Prim", 500, 0., 0.5);
    hh1[1037] = new TH1D("hh1_1037", "Track Mult. in  Prim", 20, 0., 20.);

    hh1[1041] = new TH1D("hh1_1041", "Z Prim", 1000, -10., 15.);
    hh2[1041] = new TH2D("hh2_1041", "X/Y Prim", 100, -10., 10., 100, -10., 10.);

    hh1[1042] = new TH1D("hh1_1042", "Z Prim at -120", 700, -150., -80.);
    hh2[1042] = new TH2D("hh2_1042", "X/Y Prim at -120", 100, -10., 10., 100, -10., 10.);

    hh1[1043] = new TH1D("hh1_1043", "Z Prim at -120", 1000, -10., 15.);
    hh2[1043] = new TH2D("hh2_1043", "X/Y Prim at -120", 100, -10., 10., 100, -10., 10.);

    hh1[1044] = new TH1D("hh1_1044", "Z Prim at -120", 700, -150., -80.);
    hh2[1044] = new TH2D("hh2_1044", "X/Y Prim at -120", 100, -10., 10., 100, -10., 10.);

    hh2[1045] = new TH2D("hh2_1045", "X/Z Prim", 600, -450., 150., 100, -10., 10.);
    hh2[1046] = new TH2D("hh2_1046", "Y/Z Prim", 600, -450., 150., 100, -10., 10.);

    hh1[1052] = new TH1D("hh1_1052", "Z Prim", 400, -33., -13.);
    hh1[1053] = new TH1D("hh1_1053", "Z Prim", 400, -33., -13.);
    hh1[1054] = new TH1D("hh1_1054", "Z Prim", 400, -33., -13.);
    hh1[1055] = new TH1D("hh1_1055", "Z Prim", 400, -33., -13.);
    hh1[1056] = new TH1D("hh1_1056", "Z Prim", 400, -33., -13.);
    hh1[1057] = new TH1D("hh1_1057", "Z Prim", 400, -33., -13.);
    hh1[1058] = new TH1D("hh1_1058", "Z Prim", 400, -33., -13.);
    hh1[1059] = new TH1D("hh1_1059", "Z Prim", 400, -33., -13.);
    //---------------------------------------------------------------------------------------------

    hh1[1118] = new TH1D("hh1_1118", "N Prim", 20, 0., 20.);
    hh1[1119] = new TH1D("hh1_1119", "N Prim", 20, 0., 20.);

    hh1[1120] = new TH1D("hh1_1120", "N Prim", 20, 0., 20.);
    hh1[1121] = new TH1D("hh1_1121", "X Prim", 200, -20., 20.);
    hh1[1122] = new TH1D("hh1_1122", "Y Prim", 200, -20., 20.);
    hh1[1123] = new TH1D("hh1_1123", "Z Prim", 1200, -450., 150.);
    hh1[1124] = new TH1D("hh1_1124", "Z Prim", 400, -33., -13.);

    hh1[1125] = new TH1D("hh1_1125", "Chi2 Prim", 500, 0., 0.5);
    hh1[1126] = new TH1D("hh1_1126", "CDA Prim", 500, 0., 0.5);
    hh1[1127] = new TH1D("hh1_1127", "Track Mult. in  Prim", 20, 0., 20.);

    hh1[1131] = new TH1D("hh1_1131", "X Prim", 200, -20., 20.);
    hh1[1132] = new TH1D("hh1_1132", "Y Prim", 200, -20., 20.);
    hh1[1133] = new TH1D("hh1_1133", "Z Prim", 1200, -450., 150.);
    hh1[1134] = new TH1D("hh1_1134", "Z Prim", 400, -33., -13.);

    hh1[1151] = new TH1D("hh1_1151", "X Prim", 200, -20., 20.);
    hh1[1152] = new TH1D("hh1_1152", "Y Prim", 200, -20., 20.);
    hh1[1153] = new TH1D("hh1_1153", "Z Prim", 1200, -450., 150.);
    hh1[1154] = new TH1D("hh1_1154", "Z Prim", 400, -33., -13.);

    hh2[1134] = new TH2D("hh2_1134", "X/Y Prim", 100, -4., 4., 100, -4., 4.);

    hh1[1135] = new TH1D("hh1_1135", "Chi2 Prim", 500, 0., 0.5);
    hh1[1136] = new TH1D("hh1_1136", "CDA Prim", 500, 0., 0.5);
    hh1[1137] = new TH1D("hh1_1137", "Track Mult. in  Prim", 20, 0., 20.);

    hh1[1141] = new TH1D("hh1_1141", "Z Prim", 1000, -10., 15.);
    hh2[1141] = new TH2D("hh2_1141", "X/Y Prim", 100, -10., 10., 100, -10., 10.);

    hh1[1142] = new TH1D("hh1_1142", "Z Prim at -120", 700, -150., -80.);
    hh2[1142] = new TH2D("hh2_1142", "X/Y Prim at -120", 100, -10., 10., 100, -10., 10.);

    hh1[1143] = new TH1D("hh1_1143", "Z Prim at -120", 1000, -10., 15.);
    hh2[1143] = new TH2D("hh2_1143", "X/Y Prim at -120", 100, -10., 10., 100, -10., 10.);

    hh1[1144] = new TH1D("hh1_1144", "Z Prim at -120", 700, -150., -80.);
    hh2[1144] = new TH2D("hh2_1144", "X/Y Prim at -120", 100, -10., 10., 100, -10., 10.);

    hh1[1160] = new TH1D("hh1_1160", "Z Prim", 400, -33., -13.);
    hh1[1161] = new TH1D("hh1_1161", "Z Prim", 400, -33., -13.);
    hh1[1162] = new TH1D("hh1_1162", "Z Prim", 400, -33., -13.);
    hh1[1163] = new TH1D("hh1_1163", "Z Prim", 400, -33., -13.);
    hh1[1164] = new TH1D("hh1_1164", "Z Prim", 400, -33., -13.);
    hh1[1165] = new TH1D("hh1_1165", "Z Prim", 400, -33., -13.);
    hh1[1166] = new TH1D("hh1_1166", "Z Prim", 400, -33., -13.);
    hh1[1167] = new TH1D("hh1_1167", "Z Prim", 400, -33., -13.);
    hh1[1168] = new TH1D("hh1_1168", "Z Prim", 400, -33., -13.);
    hh1[1169] = new TH1D("hh1_1169", "Z Prim", 400, -33., -13.);
    hh1[1170] = new TH1D("hh1_1170", "Z Prim", 400, -33., -13.);
    //---------------------------------------------------------------------------------------------

    hh1[1250] = new TH1D("hh1_1250", "V0  imp. vs. Prim", 500, 0., 2.);
    hh1[1251] = new TH1D("hh1_1251", "Pos imp. vs. Prim", 500, 0., 2.);
    hh1[1252] = new TH1D("hh1_1252", "Neg imp. vs. Prim", 500, 0., 2.);

    hh1[1260] = new TH1D("hh1_1260", "Omega", 500, 0., 50.);
    hh1[1261] = new TH1D("hh1_1261", "Omega Log", 500, 0., 5.);
    hh1[1262] = new TH1D("hh1_1262", "Omega Log", 500, 0., 5.);

    hh1[5021] = new TH1D("hh1_5021", "Delta X Prim ", 400, -20., 20.);
    hh1[5022] = new TH1D("hh1_5022", "Delta Y Prim ", 400, -20., 20.);
    hh1[5023] = new TH1D("hh1_5023", "Delta Z Prim ", 1500, -150., 150.);

    hh1[5031] = new TH1D("hh1_5031", "Delta X Prim ", 400, -20., 20.);
    hh1[5032] = new TH1D("hh1_5032", "Delta Y Prim ", 400, -20., 20.);
    hh1[5033] = new TH1D("hh1_5033", "Delta Z Prim ", 1500, -150., 150.);

    hh1[5041] = new TH1D("hh1_5041", "Delta X Prim ", 400, -20., 20.);
    hh1[5042] = new TH1D("hh1_5042", "Delta Y Prim ", 400, -20., 20.);
    hh1[5043] = new TH1D("hh1_5043", "Delta Z Prim ", 1500, -150., 150.);

    hh1[5051] = new TH1D("hh1_5051", "Delta X Prim ", 400, -20., 20.);
    hh1[5052] = new TH1D("hh1_5052", "Delta Y Prim ", 400, -20., 20.);
    hh1[5053] = new TH1D("hh1_5053", "Delta Z Prim ", 1500, -150., 150.);

    cdTracks = hhist->mkdir("v5_topol");
    cdTracks->cd();

    hh1[6010] = new TH1D("hh1_6010", "Z vertex", 1200, -450., 150.);
    hh2[6010] = new TH2D("hh2_6010", "X/Y vertex", 50, -20., 20., 50, -20., 20.);

    hh1[6021] = new TH1D("hh1_6021", "X vertex", 400, -20., 20.);
    hh1[6022] = new TH1D("hh1_6022", "Y vertex", 400, -20., 20.);
    hh1[6023] = new TH1D("hh1_6023", "Z vertex", 1200, -450., 150.);

    hh1[6025] = new TH1D("hh1_6025", "X vertex Ave", 400, -20., 20.);
    hh1[6026] = new TH1D("hh1_6026", "Y vertex Ave", 400, -20., 20.);
    hh1[6027] = new TH1D("hh1_6027", "Z vertex Ave", 1200, -450., 150.);

    hh1[6030] = new TH1D("hh1_6030", "Chi2 vertex", 100, 0., 20.);
    hh1[6031] = new TH1D("hh1_6031", "Chi2 vertex best", 100, 0., 20.);
    hh1[6032] = new TH1D("hh1_6032", "Average V-V Distance", 500, 0., 50.);
    hh1[6033] = new TH1D("hh1_6033", "Average V-V X-Y Distance", 500, 0., 50.);

    hh1[6035] = new TH1D("hh1_6035", "CDA All", 500, 0., 20.);
    hh1[6036] = new TH1D("hh1_6036", "CDA Best", 500, 0., 20.);

    hh1[6040] = new TH1D("hh1_6040", "Cda All Tracks", 500, 0., 20.);
    hh1[6041] = new TH1D("hh1_6041", "Mult Tracks All", 40, 0., 40.);
    hh1[6042] = new TH1D("hh1_6042", "Mult Tracks Acc", 40, 0., 40.);
    hh2[6042] = new TH2D("hh2_6042", "Mult Tracks All/Acc", 40, 0., 40., 40, 0., 40.);

}

void BmnPVAnalyzer::ProcessEvent() {

    CbmStsTrack * tr_list[1000] = {nullptr};
    CbmStsTrack *tr_pos = nullptr, *tr_neg = nullptr;

    iev++;
    fVertexArray->Delete();
    fVertexArrayAll->Delete();

    fNTracks = fGlobalTracksArray->GetEntriesFast();


    // =========================================================================
    gettimeofday(&tv1, NULL);

    tsumm_0 = tsec_old * 1000000 + tmu_old;

    tmu = tv1.tv_usec;
    tsec = tv1.tv_sec;

    tsumm_1 = tsec * 1000000 + tmu;

    dtsumm = tsumm_1 - tsumm_0;

    hh1[1]->Fill(dtsumm);

    tmu_old = tmu;
    tsec_old = tsec;

    if (dtsumm < t_min) t_min = dtsumm;
    if (dtsumm > t_max) t_max = dtsumm;

    //     cout<<" dtsumm       "<< dtsumm << endl;
    // =========================================================================

    //     if (iev != 37) continue;
    //     if (iev != 37) continue;
    //     if (iev != 142) continue;
    //     printf("%s%5i \n"," +++++++++++++++++++++++++++ New Event  ",iev);

    if ((iev % 5000 == 0) && (fVerbose > 0)) {
        curtime = time(NULL);
        loctime = localtime(&curtime);

        cout << "--------------------------------" << endl;
        cout << " ==> " << asctime(loctime) << endl;

        cout << "Processed " << iev << "  events out of " << events << endl;

        cout << " Min time/event       " << t_min << endl;
        cout << " Max time/event       " << t_max << endl;
    }


    //    if (iev % 5000 == 0) //  Save TMP histogramm file
    //    {
    //        cout << " Saving Histograms after " << iev << flush << endl;
    //
    //        //  Open histograms file
    //
    //        TFile *htmp = new TFile("my_hist_file_tmp.root", "RECREATE");
    //
    //        //          CopyDir(hhist,htmp);
    //        htmp->cd();
    //        CopyDir(hhist);
    //        htmp->cd();
    //
    //        htmp->Close();
    //        delete htmp;
    //    }
    evNo = iev;

    Int_t nTracks = fGlobalTracksArray->GetEntriesFast();
#ifdef CBM_TRACKS
    Int_t eventId = fDstEventHeader->GetEntriesFast() > 0 ? ((BmnEventHeader*) fDstEventHeader->At(0))->GetEventId() : 0;
#else
    Int_t eventId = fDstEventHeader->GetEventId();
#endif
    hh1[10]->Fill(nTracks);
    if (fVerbose > 0) cout << iev << "  evId " << eventId << "  ntracks:  " << nTracks << flush << endl;

    //Primary Vertex position

    //    pv[0] = primVtx->GetX();
    //    pv[1] = primVtx->GetY();
    //    pv[2] = primVtx->GetZ();
    //    pv[3] = primVtx->GetNTracks();
    //
    //    int it_peak;
    //    it_peak = -1;
    //
    //    if ((fabs(pv[0]) < 0.3)&&(fabs(pv[1]) < 0.3)&& ((fabs(pv[2]) - 21.9) < 0.6))
    //        it_peak = 1;
    //
    //    hh1[21]->Fill(pv[0]);
    //    hh1[22]->Fill(pv[1]);
    //    hh1[23]->Fill(pv[2]);
    //    hh1[24]->Fill(pv[2]);

    //    if ( iev < 1000 ) hh1[24]->Fill(iev,nTracks);

    ZStation[0][0] = 0.0;
    ZStation[1][0] = 4.0;
    ZStation[0][1] = 32.0;
    ZStation[1][1] = 34.0;
    ZStation[0][2] = 64.0;
    ZStation[1][2] = 66.0;
    ZStation[0][3] = 128.0;
    ZStation[1][3] = 130.0;
    ZStation[0][4] = 131.5;
    ZStation[1][4] = 132.5;

    int n_summ;
    n_summ = 0;

    //    printf("%s%10i \n"," nTracks = ",nTracks);
    //    printf("%s \n","  Track selection");
    for (Int_t i = 0; i < nTracks; ++i) {
        iNaN[i] = -1;

#ifdef CBM_TRACKS
        CbmStsTrack *tr = (CbmStsTrack *) fGlobalTracksArray->UncheckedAt(i);
        if (tr->GetNStsHits() < NHITS_THR)
            continue;
        FairTrackParam * param = tr->GetParamFirst();
        param->SetX(param->GetX() + fCBMFrameShift3D.X());
        param->SetY(param->GetY() + fCBMFrameShift3D.Y());
        param->SetZ(param->GetZ() + fCBMFrameShift3D.Z());
#else
        CbmStsTrack *tr = new CbmStsTrack();
        BmnGlobalTrack *bgt = (BmnGlobalTrack *) fGlobalTracksArray->UncheckedAt(i);
        tr->SetPidHypo(bgt->GetPDG());
        tr->SetParamFirst(*(bgt->GetParamFirst()));
        tr->SetChi2(bgt->GetChi2());
        tr->SetNDF(bgt->GetNDF());
#endif

        float dd[6];
        bool it_skip;

        dd[0] = tr->GetParamFirst()->GetX();
        dd[1] = tr->GetParamFirst()->GetY();
        dd[2] = tr->GetParamFirst()->GetZ();
        dd[3] = tr->GetParamFirst()->GetTx();
        dd[4] = tr->GetParamFirst()->GetTy();
        dd[5] = tr->GetParamFirst()->GetQp();

        //       printf("%s%10.3f%10.3f%10.3f%10.3f%10.3f \n"," dd[0 - 5] ",dd[0],dd[1],dd[2],dd[3],dd[4],dd[5]);

        it_skip = true;
        for (Int_t ii = 0; ii < 6; ++ii) {
            if (isnan(dd[ii])) it_skip = false;
        }

        if (it_skip) iNaN[i] = 0;

        if (iNaN[i] == 0) {
            tr_list[n_summ] = tr;
            //            printf("n_summ %d  tr = %08X\n", n_summ, tr);
            ++n_summ;
        }
    }

    if (n_summ < 2) {
        //       printf("%s \n", " Number of traks less then 2   ");
        return;
    }

    //    printf("%s%10i \n"," n_summ =  ",n_summ);

    //===================================================================
    //===================================================================

    int Niter;
    Niter = 5;

    nTracks = n_summ;
    Nsize = n_summ;


    //--------------------------------------
    if ((dtsumm > 2500) && (dtsumm < 5000)) hh1[206]->Fill(nTracks);
    if ((dtsumm > 1000) && (dtsumm < 2500)) hh1[205]->Fill(nTracks);
    if ((dtsumm > 500) && (dtsumm < 1000)) hh1[204]->Fill(nTracks);
    if ((dtsumm > 190) && (dtsumm < 500)) hh1[203]->Fill(nTracks);
    if ((dtsumm > 70) && (dtsumm < 190)) hh1[202]->Fill(nTracks);
    if ((dtsumm < 70)) hh1[201]->Fill(nTracks);
    //--------------------------------------
    //--------------------------------------
    //    if ((nTracks < 7) && (nTracks > 40)) continue;
    //    if ((nTracks < 5) && (nTracks > 10)) continue;
    if (nTracks > 12) return;
    //--------------------------------------
    hh1[11]->Fill(nTracks);
    //--------------------------------------

    hh1[25]->Fill(pv[0]);
    hh1[26]->Fill(pv[1]);
    hh1[27]->Fill(pv[2]);
    hh1[28]->Fill(pv[2]);

    for (Int_t i = 0; i < nTracks; ++i) {

        CbmStsTrack *tr = tr_list[i];

        //First Parameters
        FirstPar[0] = tr->GetParamFirst()->GetX();
        FirstPar[1] = tr->GetParamFirst()->GetY();
        FirstPar[2] = tr->GetParamFirst()->GetZ();

        FirstPar[3] = tr->GetParamFirst()->GetTx();
        FirstPar[4] = tr->GetParamFirst()->GetTy();
        FirstPar[5] = tr->GetParamFirst()->GetQp();

        if (fabs(FirstPar[5]) > 0.) FirstPar[5] = 1. / FirstPar[5];

        for (Int_t m = 0; m < 5; ++m) {
            if (FirstPar[2] > ZStation[0][m] && FirstPar[2] < ZStation[1][m]) {
                Ttype[m] = m;
                break;
            }
        }

        icharge[i] = 0;
        if (FirstPar[5] > 0.) icharge[i] = 1;
        if (FirstPar[5] < 0.) icharge[i] = -1;

        dpartra_F[0][i] = (double) FirstPar[0];
        dpartra_F[1][i] = (double) FirstPar[1];
        dpartra_F[2][i] = (double) FirstPar[2];
        dpartra_F[3][i] = (double) FirstPar[3];
        dpartra_F[4][i] = (double) FirstPar[4];
        dpartra_F[5][i] = (double) fabs(FirstPar[5]);

        //---------------------------------------------------------------------------------------------
        /*
        idpri = 1;
        if (idpri > 0)
        {
              if (i == 0 )printf("\n %s \n"," List of Tracks");
              if( iNaN[i] == 0 )   
              printf("%s%5i%10.3f%10.4f%10.3f%10.3f%10.4f%10.3f \n","dpartra_F (X/Y/Z/Tx/Ty/qP) : ",i,
              dpartra_F[0][i],dpartra_F[1][i],dpartra_F[2][i],dpartra_F[3][i],dpartra_F[4][i],FirstPar[5]);
        }
        idpri = 0;
         */
        //---------------------------------------------------------------------------------------------
        Tx = dpartra_F[3][i];
        Ty = dpartra_F[4][i];

        zzT = dpartra_F[2][i];
        pttot_F = dpartra_F[5][i];

        pttot = dpartra_F[5][i];
        ptz = pttot / sqrt(1. + Tx * Tx + Ty * Ty);
        ptx = ptz*Tx;
        pty = ptz*Ty;

        dptrack_F[0][i] = ptx;
        dptrack_F[1][i] = pty;
        dptrack_F[2][i] = ptz;
        dptrack_F[3][i] = sqrt(pttot * pttot + M_pi * M_pi);
        dptrack_F[4][i] = pttot;

        hh1[16]->Fill(FirstPar[2]);
        hh1[30]->Fill(FirstPar[5]);

        double ptt;
        ptt = sqrt(ptx * ptx + pty * pty);

        if (icharge[i] > 0.) hh2[161]->Fill(ptz, ptt);
        if (icharge[i] < 0.) hh2[162]->Fill(ptz, ptt);

        Int_t nhits = tr->GetNStsHits();
        // if(nhits<4) continue;

        //SetTrack & extrapolate to Z
        CbmKFTrack kftr = CbmKFTrack(*tr);
        FairTrackParam param;
        kftr.Extrapolate(300.0); //primVtx->GetZ());
        kftr.GetTrackParam(param);

        //Last Parameters    -  track paremeters after the magnet
        LastPar[0] = param.GetX();
        LastPar[1] = param.GetY();
        LastPar[2] = param.GetZ();

        LastPar[3] = param.GetTx();
        LastPar[4] = param.GetTy();
        LastPar[5] = param.GetQp();

        if (fabs(LastPar[5]) > 0.) LastPar[5] = 1. / LastPar[5];

        dpartra_L[0][i] = (double) LastPar[0];
        dpartra_L[1][i] = (double) LastPar[1];
        dpartra_L[2][i] = (double) LastPar[2];
        dpartra_L[3][i] = (double) LastPar[3];
        dpartra_L[4][i] = (double) LastPar[4];
        dpartra_L[5][i] = (double) fabs(LastPar[5]);

        Tx = dpartra_L[3][i];
        Ty = dpartra_L[4][i];

        pttot = dpartra_L[5][i];
        ptz = pttot / sqrt(1. + Tx * Tx + Ty * Ty);
        ptx = ptz*Tx;
        pty = ptz*Ty;

        dptrack_L[0][i] = ptx;
        dptrack_L[1][i] = pty;
        dptrack_L[2][i] = ptz;
        dptrack_L[3][i] = sqrt(pttot * pttot + M_pi * M_pi);
        dptrack_L[4][i] = pttot;

        //  extrapolation by stright lines and by Kalman for the comparison .

        double x1 = 0., y1 = 0., z1 = 0.;
        double x2 = 0., y2 = 0., z2 = 0.;
        double zl = 0., zh = 0., zz = 0., zstep = 0.;
        zl = 50.;
        zh = -50.;
        zstep = (zh - zl) / (double) 25;

        //      printf("%s \n"," ==================================================");
        for (Int_t l = 0; l < 25; ++l) {
            zz = zl + zstep * (double) l;
            x1 = FirstPar[0] + zz * FirstPar[3];
            y1 = FirstPar[1] + zz * FirstPar[4];

            kftr.Extrapolate(zz);
            kftr.GetTrackParam(param);

            x2 = param.GetX();
            y2 = param.GetY();
            z2 = param.GetZ();

            if (pttot_F > 0 && pttot_F <= 2.) {
                hh2[131]->Fill(x1, x2);
                hh2[132]->Fill(y1, y2);
            }

            if (pttot_F > 2 && pttot_F <= 4.) {
                hh2[133]->Fill(x1, x2);
                hh2[134]->Fill(y1, y2);
            }

            if (pttot_F > 4 && pttot_F <= 6.) {
                hh2[135]->Fill(x1, x2);
                hh2[136]->Fill(y1, y2);
            }

            //         if ( (fabs(y1-y2) < 4.) && ((zzT > 0.) && (zzT < 4.)) )
            if ((fabs(y1 - y2) < 4.) && ((zzT > 32.) && (zzT < 34.))) {
                if (pttot_F > 0.0 && pttot_F <= 0.5) {
                    hh2[141]->Fill(x1, x2);
                }
                if (pttot_F > 0.5 && pttot_F <= 1.0) {
                    hh2[142]->Fill(x1, x2);
                }

                if (pttot_F > 1.0 && pttot_F <= 1.5) {
                    hh2[143]->Fill(x1, x2);
                }
                if (pttot_F > 1.5 && pttot_F <= 2.0) {
                    hh2[144]->Fill(x1, x2);
                }

                if (pttot_F > 2.0 && pttot_F <= 2.5) {
                    hh2[145]->Fill(x1, x2);
                }
                if (pttot_F > 2.5 && pttot_F <= 3.0) {
                    hh2[146]->Fill(x1, x2);
                }

                if (pttot_F > 3.0 && pttot_F <= 3.5) {
                    hh2[147]->Fill(x1, x2);
                }
                if (pttot_F > 3.5 && pttot_F <= 4.0) {
                    hh2[148]->Fill(x1, x2);
                }

                if (pttot_F > 4.0 && pttot_F <= 4.5) {
                    hh2[149]->Fill(x1, x2);
                }
                if (pttot_F > 4.5 && pttot_F <= 5.0) {
                    hh2[150]->Fill(x1, x2);
                }

                if (pttot_F > 5.0 && pttot_F <= 5.5) {
                    hh2[151]->Fill(x1, x2);
                }

            }
            /*
                     printf("%s%8.3f%8.3f%8.3f \n","x1,y1,zz   ",x1,y1,zz);
                     printf("%s%8.3f%8.3f%8.3f \n","x2,y2,z2   ",x2,y2,z2);
                     printf("%s \n","  ");
             */
        }

        //      txyz->Fill();
    } // for (Int_t i = 0; i < nTracks;
    //
    //---------------------------------------------
    //---------------------------------------------

    TLorentzVector lpi1, lp1;
    TLorentzVector lpi2, lp2;

    TVector3 tpi1, tpi2;
    TVector3 tpi1n, tpi2n;

    double E1 = 0., E11 = 0.;
    double E2 = 0., E22 = 0.;

    double K0mass = 0., Lmass = 0., ALmass = 0.;
    double gamma_k0 = 0., gamma_l0 = 0., gamma_a0 = 0.;

    TLorentzVector llpi1, llp1;
    TLorentzVector llpi2, llp2;

    double EE1 = 0., EE11 = 0.;
    double EE2 = 0., EE22 = 0.;

    double KK0mass = 0., LLmass = 0., AALmass = 0.;
    double ggamma_k0 = 0., ggamma_l0 = 0., ggamma_a0 = 0.;
    double ctau_k0 = 0., ctau_l0 = 0., ctau_a0 = 0.;

    TVector3 ttpi1, ttpi2;

    double firstP[6] = {0.}, lastP[6] = {0.};
    double firstN[6] = {0.}, lastN[6] = {0.};
    double CosT = 0.;

    double xp = 0., yp = 0., xn = 0., yn = 0.;

    // definition for VERCDA
    int nv_trk = 0, nvhi = 0;
    double vec[5][300] = {0.}, ww[300] = {0.}, vert[3] = {0.}, cda[4][300] = {0.}, ssq;

    int Npl = 0, List[100] = {0};
    double zvert[3] = {0.}, dcda[4][300] = {0.}, Z0;
    double zvtx[3] = {0.};

    double upx = 0., upy = 0., upz = 0., uptot = 0.;
    double unx = 0., uny = 0., unz = 0., untot = 0.;
    double V0Px, V0Py = 0., V0Pz = 0., V0Ptot = 0.;
    double imp_pos[4] = {0.}, imp_neg[4] = {0.}, imp_v0[4] = {0.};
    double omega = 0., omegalog = 0.;
    //
    //===================================================================
    //===================================================================
    //  Primary Vertex search
    //===================================================================
    //===================================================================
    //
    int jfirst = 0;
    int itpl = 0;
    int ntopol[VTX_LEN];
    int inxtpl[VTX_LEN][VTX_LEN];
    int Mult_Type = 0;

    double V_XYZ[3][VTX_LEN] = {0.};
    double V_cda[4][VTX_LEN][VTX_LEN] = {0.};
    double V_chi2[VTX_LEN] = {0.};

    double X_Fit = 0., Y_Fit = 0., Z_Fit = 0.;
    double X_Sec = 0., Y_Sec = 0., Z_Sec = 0.;

    //--------------------------------------------------------------------------------------
    /*
    if (idpri > 0)
    {
        printf("%s \n"," ++++++++++++++++++++++++++++++++++++++++++++  ");
        printf("%s%10i \n","                                   Call to vtx_topol, Event #", iev);

        printf("%s%10i%10i%14.3f \n"," Tracks, Planes", nTracks, Npl, Z0);
        printf("%s \n","   ");
    }
     */
    //--------------------------------------------------------------------------------------
    Mult_Type = -1;

    Npl = 10;
    Niter = 3;
    Z0 = 150.;

    for (Int_t i = 0; i < nTracks; ++i) {
        List[i] = i;
    }

    jfirst = -1;

    for (int i = 0; i < Min(nTracks, VTX_LEN); ++i) {
        V_XYZ[0][i] = fRoughVertex3D.X();
        V_XYZ[1][i] = fRoughVertex3D.Y();
        V_XYZ[2][i] = fRoughVertex3D.Z();
    }
    //-----------------------------------------------------------------------------------------------
    double xprim1, yprim1, zprim1;
    double xprim2, yprim2, zprim2;
    double dxprim = 0., dyprim = 0., dzprim = 0.;

    xprim1 = 1000.;
    yprim1 = 1000.;
    zprim1 = 1000.;

    xprim2 = 1000.;
    yprim2 = 1000.;
    zprim2 = 1000.;

    //-----------------------------------------------------------------------------------------------
    //   Low Track Multiplicity Event
    Int_t NTThreshold = 10;

    if ((nTracks > 1) && (nTracks < NTThreshold)) {
        Mult_Type = 1;

        int nt = 0;

        //      printf("%s \n"," ++++++++++++++++++++++++++++++++++++++++++++  ");
        //      printf("%s%10i \n","                                   Call to vtx_topol, Event #", iev);

        hh1[2]->Fill(dtsumm);
        if (fVerbose > 1) printf("%s \n", "  ----------------------- ");
        if (fVerbose > 1) printf("%s \n", "  First Call to vtx_topol");
        //        for (int i = 0; i < nTracks; ++i) {
        //            printf("vtx_topol tr_list[%2i] = %08X\n", i, tr_list[i]);
        //        }
        vtx_topol(nTracks, Niter, Npl, Z0, List, tr_list,
                jfirst, itpl, ntopol, inxtpl,
                V_XYZ, V_cda, V_chi2);

        if (jfirst >= 0) {
            //            new ((*fVertexArray)[fVertexArray->GetEntriesFast()])
            //                    CbmVertex("vertex", "vertex",
            //                    V_XYZ[0][jfirst],
            //                    V_XYZ[1][jfirst],
            //                    V_XYZ[2][jfirst],
            //                    V_chi2[jfirst], 0, ntopol[jfirst], TMatrixFSym(3), fRoughVertex3D);
            hh1[1020]->Fill(itpl);

            for (Int_t i = 0; i < itpl; ++i) {
                hh1[1021]-> Fill(V_XYZ[0][i]);
                hh1[1022]-> Fill(V_XYZ[1][i]);
                hh1[1023]-> Fill(V_XYZ[2][i]);
                hh1[1024]-> Fill(V_XYZ[2][i]);

                hh1[1025]-> Fill(V_chi2[i]);

                nt = ntopol[i];
                hh1[1027]-> Fill(nt);
                for (Int_t k = 0; k < nt; ++k) {
                    hh1[1026]-> Fill(V_cda[3][k][i]);
                }

                if ((V_XYZ[2][i] > -10.) && (V_XYZ[2][i] < 15.) && nt > 2) {
                    hh1[1043]-> Fill(V_XYZ[2][i]);
                    hh2[1043]-> Fill(V_XYZ[0][i], V_XYZ[1][i]);
                }

                if ((V_XYZ[2][i] > -150.) && (V_XYZ[2][i] <-80.) && nt > 2) {
                    hh1[1044]-> Fill(V_XYZ[2][i]);
                    hh2[1044]-> Fill(V_XYZ[0][i], V_XYZ[1][i]);
                }
            }

            hh1[1031]-> Fill(V_XYZ[0][jfirst]);
            hh1[1032]-> Fill(V_XYZ[1][jfirst]);
            hh1[1033]-> Fill(V_XYZ[2][jfirst]);
            hh1[1034]-> Fill(V_XYZ[2][jfirst]);
            xprim1 = V_XYZ[0][jfirst];
            yprim1 = V_XYZ[1][jfirst];
            zprim1 = V_XYZ[2][jfirst];

            hh1[1035]-> Fill(V_chi2[jfirst]);

            nt = ntopol[jfirst];
            hh1[1037]-> Fill(nt);
            for (Int_t k = 0; k < nt; ++k) {
                hh1[1036]-> Fill(V_cda[3][k][jfirst]);
            }

            if ((V_XYZ[2][jfirst] > -26) && (V_XYZ[2][jfirst] <-21) && nt > 2) {
                hh2[1034]-> Fill(V_XYZ[0][jfirst], V_XYZ[1][jfirst]);
            }

            if ((V_XYZ[2][jfirst] > -10.) && (V_XYZ[2][jfirst] < 15.) && nt > 2) {
                hh1[1041]-> Fill(V_XYZ[2][jfirst]);
                hh2[1041]-> Fill(V_XYZ[0][jfirst], V_XYZ[1][jfirst]);
            }

            if ((V_XYZ[2][jfirst] > -150.) && (V_XYZ[2][jfirst] <-80.) && nt > 2) {
                hh1[1042]-> Fill(V_XYZ[2][jfirst]);
                hh2[1042]-> Fill(V_XYZ[0][jfirst], V_XYZ[1][jfirst]);
            }

            hh2[1045]-> Fill(V_XYZ[0][jfirst], V_XYZ[2][jfirst]);
            hh2[1046]-> Fill(V_XYZ[1][jfirst], V_XYZ[2][jfirst]);

            if ((nt > 1) && (nt < 10))
                hh1[1050 + nTracks]-> Fill(V_XYZ[2][jfirst]);
        }
    }
    //-----------------------------------------------------------------------------------------------
    //  High track multiplicity event

    double Xav = 0., Yav = 0., Zav = 0.;

    int jNtracks = 0, jList[VTX_LEN] = {0};

    int nt = 0;
    //-----------------------------------------------------------------------------------------------
    if ((nTracks >= NTThreshold) && (nTracks < 100)) {

        Mult_Type = 2;

        double XPrel = 0., YPrel = 0., ZPrel = 0.;
        XPrel = -10000.;
        YPrel = -10000.;
        ZPrel = -10000.;

        hh1[3]->Fill(dtsumm);

        if (fVerbose > 1) printf("%s \n", "  ----------------------- ");
        if (fVerbose > 1) printf("%s \n", "  Second Call to v5_topol");

        //        for (int i = 0; i < nTracks; ++i) {
        //            printf("v5_topol tr_list[%2i] = %08X\n", i, tr_list[i]);
        //        }
        v5_topol(nTracks, Niter, Npl, Z0, List, tr_list,
                jfirst, itpl, ntopol, inxtpl,
                V_XYZ, V_cda, V_chi2, Xav, Yav, Zav, jNtracks, jList);

        hh1[1118]->Fill(itpl);

        if (jfirst >= 0) {
            XPrel = V_XYZ[0][jfirst];
            YPrel = V_XYZ[1][jfirst];
            ZPrel = V_XYZ[2][jfirst];

            hh1[1151]-> Fill(XPrel);
            hh1[1152]-> Fill(YPrel);
            hh1[1153]-> Fill(ZPrel);
            hh1[1154]-> Fill(ZPrel);
        }

        jfirst = -1;
        itpl = 0;

        //------------------------------------------------
        if (fVerbose > 1) printf("%s \n", "  ----------------------- ");
        if (fVerbose > 1) printf("%s \n", "  Second Call to vtx_topol");
        //------------------------------------------------
        vtx_topol(jNtracks, Niter, Npl, Z0, jList, tr_list,
                jfirst, itpl, ntopol, inxtpl,
                V_XYZ, V_cda, V_chi2);
        //-----------------------------------------------------------------------------------------------
        if (jfirst >= 0) {

            new ((*fVertexArray)[fVertexArray->GetEntriesFast()])
                    CbmVertex("vertex", "vertex",
                    V_XYZ[0][jfirst],
                    V_XYZ[1][jfirst],
                    V_XYZ[2][jfirst],
                    V_chi2[jfirst], 0, ntopol[jfirst], TMatrixFSym(3), fRoughVertex3D);

            hh1[1119]->Fill(itpl);

            for (Int_t i = 0; i < itpl; ++i) {
                new ((*fVertexArrayAll)[fVertexArrayAll->GetEntriesFast()])
                        CbmVertex("vertex", "vertex",
                        V_XYZ[0][i],
                        V_XYZ[1][i],
                        V_XYZ[2][i],
                        V_chi2[i], 0, ntopol[i], TMatrixFSym(3), fRoughVertex3D);
                hh1[1121]-> Fill(V_XYZ[0][i]);
                hh1[1122]-> Fill(V_XYZ[1][i]);
                hh1[1123]-> Fill(V_XYZ[2][i]);
                hh1[1124]-> Fill(V_XYZ[2][i]);

                hh1[1125]-> Fill(V_chi2[i]);

                nt = ntopol[i];
                hh1[1127]-> Fill(nt);
                for (Int_t k = 0; k < nt; ++k) {
                    hh1[1126]-> Fill(V_cda[3][k][i]);
                }

                if ((V_XYZ[2][i] > -10.) && (V_XYZ[2][i] < 15.) && nt > 2) {
                    hh1[1143]-> Fill(V_XYZ[2][i]);
                    hh2[1143]-> Fill(V_XYZ[0][i], V_XYZ[1][i]);
                }

                if ((V_XYZ[2][i] > -150.) && (V_XYZ[2][i] <-80.) && nt > 2) {
                    hh1[1144]-> Fill(V_XYZ[2][i]);
                    hh2[1144]-> Fill(V_XYZ[0][i], V_XYZ[1][i]);
                }


            } // End of :  for (Int_t i = 0; i < itpl; ++i)

            hh1[1131]-> Fill(V_XYZ[0][jfirst]);
            hh1[1132]-> Fill(V_XYZ[1][jfirst]);
            hh1[1133]-> Fill(V_XYZ[2][jfirst]);
            hh1[1134]-> Fill(V_XYZ[2][jfirst]);

            hh1[1135]-> Fill(V_chi2[jfirst]);

            nt = ntopol[jfirst];
            hh1[1137]-> Fill(nt);
            for (Int_t k = 0; k < nt; ++k) {
                hh1[1136]-> Fill(V_cda[3][k][jfirst]);
            }

            if ((V_XYZ[2][jfirst] > -26) && (V_XYZ[2][jfirst] <-21) && nt > 2) {
                hh2[1134]-> Fill(V_XYZ[0][jfirst], V_XYZ[1][jfirst]);
            }

            if ((V_XYZ[2][jfirst] > -10.) && (V_XYZ[2][jfirst] < 15.) && nt > 2) {
                hh1[1141]-> Fill(V_XYZ[2][jfirst]);
                hh2[1141]-> Fill(V_XYZ[0][jfirst], V_XYZ[1][jfirst]);
            }

            if ((V_XYZ[2][jfirst] > -150.) && (V_XYZ[2][jfirst] <-80.) && nt > 2) {
                hh1[1142]-> Fill(V_XYZ[2][jfirst]);
                hh2[1142]-> Fill(V_XYZ[0][jfirst], V_XYZ[1][jfirst]);
            }
            if ((nt > 9) && (nt < 19))
                hh1[1150 + nt + 1]-> Fill(V_XYZ[2][jfirst]);


            hh1[5021]-> Fill(V_XYZ[0][jfirst] - Xav);
            hh1[5022]-> Fill(V_XYZ[1][jfirst] - Yav);
            hh1[5023]-> Fill(V_XYZ[2][jfirst] - Zav);

            hh1[5031]-> Fill(V_XYZ[0][jfirst] - XPrel);
            hh1[5032]-> Fill(V_XYZ[1][jfirst] - YPrel);
            hh1[5033]-> Fill(V_XYZ[2][jfirst] - ZPrel);
        } // End of:  if (jfirst >= 0 )
    } // End of:  if ((nTracks > 9) && (nTracks < 100)) 
    //-----------------------------------------------------------------------------------------------

    if (jfirst < 0) return;

    dxprim = xprim1 - xprim2;
    dyprim = yprim1 - yprim2;
    dzprim = zprim1 - zprim2;

    hh1[5041]-> Fill(dxprim);
    hh1[5042]-> Fill(dyprim);
    hh1[5043]-> Fill(dzprim);

    if ((zprim1 > -28.) && (zprim1 < -20.) && (zprim2 > -28.) && (zprim2 < -20.)) {
        hh1[5051]-> Fill(dxprim);
        hh1[5052]-> Fill(dyprim);
        hh1[5053]-> Fill(dzprim);
    }

    //===================================================================
    //===================================================================
    //         printf("%s \n"," Start V0 search ");

    Npl = 10;
    Z0 = 150.;

    X_Fit = fRoughVertex3D.X();
    Y_Fit = fRoughVertex3D.Y();
    Z_Fit = fRoughVertex3D.Z();

    if (jfirst >= 0) {
        X_Fit = V_XYZ[0][jfirst];
        Y_Fit = V_XYZ[1][jfirst];
        Z_Fit = V_XYZ[2][jfirst];
    }

    for (Int_t i = 0; i < nTracks; ++i) {

        if (icharge[i] < 0) continue;

        double ppx1 = 0., ppy1 = 0., ppz1 = 0., pptot1 = 0.;
        double tx1 = 0., ty1 = 0.;

        tr_pos = tr_list[i];

        Int_t nhits_pos = tr_pos->GetNStsHits();

        hh1[301]->Fill(nhits_pos);

        CbmKFTrack kftr_p = CbmKFTrack(*tr_pos);
        FairTrackParam param_p;
        kftr_p.Extrapolate(0.); //primVtx->GetZ());
        kftr_p.GetTrackParam(param_p);

        xp = param_p.GetX();
        yp = param_p.GetY();

        ppx1 = dptrack_F[0][i];
        ppy1 = dptrack_F[1][i];
        ppz1 = dptrack_F[2][i];
        pptot1 = dptrack_F[4][i];

        tpi1.SetXYZ(ppx1, ppy1, ppz1);

        tx1 = ppx1 / ppz1;
        ty1 = ppy1 / ppz1;

        E1 = sqrt(pptot1 * pptot1 + M_pi * M_pi);
        lpi1.SetPxPyPzE(ppx1, ppy1, ppz1, E1);

        E11 = sqrt(pptot1 * pptot1 + M_p * M_p);
        lp1.SetPxPyPzE(ppx1, ppy1, ppz1, E11);

        for (int k = 0; k < 6; ++k) {
            firstP[k] = dpartra_F[k][i];
            lastP[k] = dpartra_L[k][i];
        }

        List[0] = i;

        for (Int_t j = 0; j < nTracks; ++j) {

            if (icharge[j] > 0) continue;

            double ppx2 = 0., ppy2 = 0., ppz2 = 0., pptot2 = 0.;
            double tx2 = 0., ty2 = 0.;

            tr_neg = tr_list[j];

            Int_t nhits_neg = tr_neg->GetNStsHits();

            if (i == 0) hh1[302]->Fill(nhits_neg);

            CbmKFTrack kftr_n = CbmKFTrack(*tr_neg);
            FairTrackParam param_n;
            kftr_n.Extrapolate(0.); //primVtx->GetZ());
            kftr_n.GetTrackParam(param_n);

            xn = param_n.GetX();
            yn = param_n.GetY();

            ppx2 = dptrack_F[0][j];
            ppy2 = dptrack_F[1][j];
            ppz2 = dptrack_F[2][j];
            pptot2 = dptrack_F[4][j];

            tpi2.SetXYZ(ppx2, ppy2, ppz2);

            tx2 = ppx2 / ppz2;
            ty2 = ppy2 / ppz2;

            E2 = sqrt(pptot2 * pptot2 + M_pi * M_pi);
            lpi2.SetPxPyPzE(ppx2, ppy2, ppz2, E2);

            E22 = sqrt(pptot2 * pptot2 + M_p * M_p);
            lp2.SetPxPyPzE(ppx2, ppy2, ppz2, E22);

            for (int k = 0; k < 6; ++k) {
                firstN[k] = dpartra_F[k][j];
                lastN[k] = dpartra_L[k][j];
            }

            List[1] = j;

            Decay_Plane(firstP, lastP, firstN, lastN, CosT);
            hh1[599]->Fill(CosT);

            int inv_trk = 0;

            //--------------------------------------------------------------------------
            Niter = 3;
            //--------------------------------------------------------------------------

            TLorentzVector lvKs = lpi1 + lpi2; // Lorentz vector of K0S system
            K0mass = lvKs.M();
            gamma_k0 = K0mass / lvKs(2);


            TLorentzVector lvL0 = lp1 + lpi2; // Lorentz vector of L0 system
            Lmass = lvL0.M();
            gamma_l0 = Lmass / lvL0(2);


            TLorentzVector lvAL = lpi1 + lp2; // Lorentz vector of AL0 system
            ALmass = lvAL.M();
            gamma_a0 = ALmass / lvAL(2);
            //
            //----------------------------------------------------
            //
            inv_trk = 2;
            VirtualPlanes(inv_trk, Niter, Npl, Z0, List, tr_list, zvert, zvtx, dcda, ssq);

            X_Sec = zvtx[0];
            Y_Sec = zvtx[1];
            Z_Sec = zvtx[2];

            //------------------------------------------------------
            // Positive track parameters at Secondary vertex

            CbmKFTrack kftr_pos1 = CbmKFTrack(*tr_pos);
            FairTrackParam param_pos1;
            kftr_pos1.Extrapolate(Z_Sec);
            kftr_pos1.GetTrackParam(param_pos1);

            Tx = (double) param_pos1.GetTx();
            Ty = (double) param_pos1.GetTy();
            uptot = (double) fabs(param_pos1.GetQp());

            if (uptot > 0) uptot = 1. / uptot;

            upz = uptot / sqrt(1. + Tx * Tx + Ty * Ty);
            upx = upz*Tx;
            upy = upz*Ty;

            ttpi1.SetXYZ(upx, upy, upz);

            EE1 = sqrt(uptot * uptot + M_pi * M_pi);
            llpi1.SetPxPyPzE(upx, upy, upz, EE1);

            EE11 = sqrt(uptot * uptot + M_p * M_p);
            llp1.SetPxPyPzE(upx, upy, upz, EE11);

            //------------------------------------------------------
            // Negative track parameters at Secondary vertex

            CbmKFTrack kftr_neg1 = CbmKFTrack(*tr_neg);
            FairTrackParam param_neg1;
            kftr_neg1.Extrapolate(Z_Sec);
            kftr_neg1.GetTrackParam(param_neg1);

            Tx = (double) param_neg1.GetTx();
            Ty = (double) param_neg1.GetTy();
            untot = (double) fabs(param_neg1.GetQp());

            if (untot > 0) untot = 1. / untot;

            unz = untot / sqrt(1. + Tx * Tx + Ty * Ty);
            unx = unz*Tx;
            uny = unz*Ty;

            ttpi2.SetXYZ(unx, uny, unz);

            EE2 = sqrt(untot * untot + M_pi * M_pi);
            llpi2.SetPxPyPzE(unx, uny, unz, EE2);

            EE22 = sqrt(untot * untot + M_p * M_p);
            llp2.SetPxPyPzE(unx, uny, unz, EE22);

            //------------------------------------------------------
            double D_vtx = 0.;

            D_vtx = Z_Sec - pv[2];

            TLorentzVector llvKs = llpi1 + llpi2; // Lorentz vector of K0S system
            KK0mass = llvKs.M();

            ggamma_k0 = KK0mass / llvKs(2);
            ctau_k0 = D_vtx / ggamma_k0;

            TLorentzVector llvL0 = llp1 + llpi2; // Lorentz vector of L0 system
            LLmass = llvL0.M();

            ggamma_l0 = LLmass / llvL0(2);
            ctau_l0 = D_vtx / ggamma_l0;


            TLorentzVector llvAL = llpi1 + llp2; // Lorentz vector of AL0 system
            AALmass = llvAL.M();

            ggamma_a0 = AALmass / llvAL(2);
            ctau_a0 = D_vtx / ggamma_a0;

            //-----------------------------------------------------
            //-----------------------------------------------------
            //-----------------------------------------------------
            V0Px = llvKs(0);
            V0Py = llvKs(1);
            V0Pz = llvKs(2);
            V0Ptot = sqrt(V0Px * V0Px + V0Py * V0Py + V0Pz * V0Pz);

            ddpoint(X_Fit, Y_Fit, Z_Fit, X_Sec, Y_Sec, Z_Sec,
                    V0Px, V0Py, V0Pz, V0Ptot, imp_v0);

            hh1[1250]->Fill(imp_v0[3]);

            //-----------------------------------------------------
            // Positive track parameters at Primary vertex
            //        CbmKFTrack kftr_pos1 = CbmKFTrack(*tr_pos);
            //        FairTrackParam param_pos1;
            kftr_pos1.Extrapolate(Z_Fit);
            kftr_pos1.GetTrackParam(param_pos1);

            Tx = (double) param_pos1.GetTx();
            Ty = (double) param_pos1.GetTy();
            uptot = (double) fabs(param_pos1.GetQp());

            if (uptot > 0) uptot = 1. / uptot;

            upz = uptot / sqrt(1. + Tx * Tx + Ty * Ty);
            upx = upz*Tx;
            upy = upz*Ty;

            ddpoint(X_Fit, Y_Fit, Z_Fit, X_Sec, Y_Sec, Z_Sec,
                    upx, upy, upz, uptot, imp_pos);
            hh1[1251]->Fill(imp_pos[3]);

            //-----------------------------------------------------
            // Negative track parameters at Primary vertex
            //        CbmKFTrack kftr_neg1 = CbmKFTrack(*tr_neg);
            //        FairTrackParam param_neg1;
            kftr_neg1.Extrapolate(Z_Fit);
            kftr_neg1.GetTrackParam(param_neg1);

            Tx = (double) param_neg1.GetTx();
            Ty = (double) param_neg1.GetTy();
            untot = (double) fabs(param_neg1.GetQp());

            if (untot > 0) untot = 1. / untot;

            unz = untot / sqrt(1. + Tx * Tx + Ty * Ty);
            unx = unz*Tx;
            uny = unz*Ty;

            ddpoint(X_Fit, Y_Fit, Z_Fit, X_Sec, Y_Sec, Z_Sec,
                    unx, uny, unz, untot, imp_neg);
            hh1[1252]->Fill(imp_neg[3]);

            //-----------------------------------------------------

            omega = imp_pos[3] * imp_neg[3] /
                    (imp_v0[3] * imp_v0[3] + 4. * dcda[3][0] * dcda[3][0]);
            omegalog = log(omega);

            hh1[1260]->Fill(omega);
            hh1[1261]->Fill(omegalog);

            if ((imp_v0[3] < 0.3) && (imp_pos[3] > 0.2) && imp_neg[3] > 0.2) {
                hh1[1262]->Fill(omegalog);
            }
            //-----------------------------------------------------
            //-----------------------------------------------------

            // Armenteros plot variables

            Double_t apt = 0.;
            double pl1 = 0.;
            double pl2 = 0.;
            double alpha = 0.;

            TVector3 vvsum = ttpi1 + ttpi2;
            apt = ttpi1.Perp(vvsum);
            pl1 = sqrt(ttpi1.Mag2() - ttpi1.Perp2(vvsum));
            pl2 = sqrt(ttpi2.Mag2() - ttpi2.Perp2(vvsum));
            alpha = (pl1 - pl2) / (pl1 + pl2);


            // Armenteros plot variables
            if (dcda[3][0] < 1.0) {
                if (omegalog > 3.0) {
                    hh2[600]->Fill(alpha, apt);
                    if ((uptot < 4.0) && (untot > 0.3)) {
                        hh2[601]->Fill(alpha, apt);
                    }
                }
            }

            //-----------------------------------------------------

            //        if ((CosT < -0.95) && (fabs(yp - yn) < 4.)) 
            if ((CosT < -0.90) && Mult_Type == 1) {

                hh1[41]->Fill(vert[0]);
                hh1[42]->Fill(vert[1]);
                hh1[43]->Fill(vert[2]);
                hh1[44]->Fill(cda[3][0]);

                if (omegalog > 0.5) {
                    hh1[310]->Fill(KK0mass);
                    hh1[410]->Fill(LLmass);
                    hh1[510]->Fill(AALmass);

                    if (apt > 0.15) {
                        hh1[353]->Fill(KK0mass);
                    }
                    if (apt < 0.15) {
                        hh1[453]->Fill(LLmass);
                        hh1[553]->Fill(AALmass);
                    }

                    if (omegalog > 1.0) {
                        hh1[311]->Fill(KK0mass);
                        hh1[411]->Fill(LLmass);
                        hh1[511]->Fill(AALmass);

                        if (apt > 0.15) {
                            hh1[354]->Fill(KK0mass);
                        }
                        if (apt < 0.15) {
                            hh1[454]->Fill(LLmass);
                            hh1[554]->Fill(AALmass);
                        }

                        if (omegalog > 1.5) {
                            hh1[350]->Fill(KK0mass);
                            hh1[450]->Fill(LLmass);
                            hh1[550]->Fill(AALmass);

                            if (apt > 0.15) {
                                hh1[355]->Fill(KK0mass);
                            }
                            if (apt < 0.15) {
                                hh1[455]->Fill(LLmass);
                                hh1[555]->Fill(AALmass);
                            }

                            if (omegalog > 2.5) {
                                hh1[351]->Fill(KK0mass);
                                hh1[451]->Fill(LLmass);
                                hh1[551]->Fill(AALmass);

                                if (apt > 0.15) {
                                    hh1[356]->Fill(KK0mass);
                                }
                                if (apt < 0.15) {
                                    hh1[456]->Fill(LLmass);
                                    hh1[556]->Fill(AALmass);
                                }

                                if (omegalog > 3.0) {
                                    hh1[352]->Fill(KK0mass);
                                    hh1[452]->Fill(LLmass);
                                    hh1[552]->Fill(AALmass);

                                    if (apt > 0.15) {
                                        hh1[357]->Fill(KK0mass);
                                    }
                                    if (apt < 0.15) {
                                        hh1[457]->Fill(LLmass);
                                        hh1[557]->Fill(AALmass);
                                    }
                                }
                            }
                        }
                    }
                }

                if ((uptot < 3.0) && (untot > 0.3)) {
                    if (omegalog > 0.5) {
                        hh1[2310]->Fill(KK0mass);
                        hh1[2410]->Fill(LLmass);
                        hh1[2510]->Fill(AALmass);

                        if (apt > 0.15) {
                            hh1[2353]->Fill(KK0mass);
                        }
                        if (apt < 0.15) {
                            hh1[2453]->Fill(LLmass);
                            hh1[2553]->Fill(AALmass);
                        }
                        if ((nhits_pos > 3) && (nhits_neg > 3)) {
                            hh1[2358]->Fill(KK0mass);
                            hh1[2458]->Fill(LLmass);
                            hh1[2558]->Fill(AALmass);
                        }

                        if (omegalog > 1.0) {
                            hh1[2311]->Fill(KK0mass);
                            hh1[2411]->Fill(LLmass);
                            hh1[2511]->Fill(AALmass);

                            if (apt > 0.15) {
                                hh1[2354]->Fill(KK0mass);
                            }
                            if (apt < 0.15) {
                                hh1[2454]->Fill(LLmass);
                                hh1[2554]->Fill(AALmass);
                            }

                            if (omegalog > 1.5) {
                                hh1[2350]->Fill(KK0mass);
                                hh1[2450]->Fill(LLmass);
                                hh1[2550]->Fill(AALmass);

                                if (apt > 0.15) {
                                    hh1[2355]->Fill(KK0mass);
                                }
                                if (apt < 0.15) {
                                    hh1[2455]->Fill(LLmass);
                                    hh1[2555]->Fill(AALmass);
                                }

                                if (omegalog > 2.5) {
                                    hh1[2351]->Fill(KK0mass);
                                    hh1[2451]->Fill(LLmass);
                                    hh1[2551]->Fill(AALmass);

                                    if (apt > 0.15) {
                                        hh1[2356]->Fill(KK0mass);
                                    }
                                    if (apt < 0.15) {
                                        hh1[2456]->Fill(LLmass);
                                        hh1[2556]->Fill(AALmass);
                                    }

                                    if (omegalog > 3.0) {
                                        hh1[2352]->Fill(KK0mass);
                                        hh1[2452]->Fill(LLmass);
                                        hh1[2552]->Fill(AALmass);

                                        if (apt > 0.15) {
                                            hh1[2357]->Fill(KK0mass);
                                        }
                                        if (apt < 0.15) {
                                            hh1[2457]->Fill(LLmass);
                                            hh1[2557]->Fill(AALmass);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } // End of if ( (CosT < -0.90) )
            //
            //  ---------------  High Multilicity
            //
            if ((CosT < -0.90) && Mult_Type == 2) {

                if (omegalog > 0.5) {
                    hh1[360]->Fill(KK0mass);
                    hh1[460]->Fill(LLmass);
                    hh1[560]->Fill(AALmass);

                    if (apt > 0.15) {
                        hh1[361]->Fill(KK0mass);
                    }
                    if (apt < 0.15) {
                        hh1[461]->Fill(LLmass);
                        hh1[561]->Fill(AALmass);
                    }

                    if (omegalog > 1.0) {
                        hh1[362]->Fill(KK0mass);
                        hh1[462]->Fill(LLmass);
                        hh1[562]->Fill(AALmass);

                        if (apt > 0.15) {
                            hh1[363]->Fill(KK0mass);
                        }
                        if (apt < 0.15) {
                            hh1[463]->Fill(LLmass);
                            hh1[563]->Fill(AALmass);
                        }

                        if (omegalog > 1.5) {
                            hh1[364]->Fill(KK0mass);
                            hh1[464]->Fill(LLmass);
                            hh1[564]->Fill(AALmass);

                            if (apt > 0.15) {
                                hh1[365]->Fill(KK0mass);
                            }
                            if (apt < 0.15) {
                                hh1[465]->Fill(LLmass);
                                hh1[565]->Fill(AALmass);
                            }

                            if (omegalog > 2.5) {
                                hh1[366]->Fill(KK0mass);
                                hh1[466]->Fill(LLmass);
                                hh1[566]->Fill(AALmass);

                                if (apt > 0.15) {
                                    hh1[367]->Fill(KK0mass);
                                }
                                if (apt < 0.15) {
                                    hh1[467]->Fill(LLmass);
                                    hh1[567]->Fill(AALmass);
                                }

                                if (omegalog > 3.0) {
                                    hh1[368]->Fill(KK0mass);
                                    hh1[468]->Fill(LLmass);
                                    hh1[568]->Fill(AALmass);

                                    if (apt > 0.15) {
                                        hh1[369]->Fill(KK0mass);
                                    }
                                    if (apt < 0.15) {
                                        hh1[469]->Fill(LLmass);
                                        hh1[569]->Fill(AALmass);
                                    }
                                }
                            }
                        }
                    }
                }

                if ((uptot < 3.0) && (untot > 0.3)) {
                    if (omegalog > 0.5) {
                        hh1[2360]->Fill(KK0mass);
                        hh1[2460]->Fill(LLmass);
                        hh1[2560]->Fill(AALmass);

                        if (apt > 0.15) {
                            hh1[2361]->Fill(KK0mass);
                        }
                        if (apt < 0.15) {
                            hh1[2461]->Fill(LLmass);
                            hh1[2561]->Fill(AALmass);
                        }

                        if (omegalog > 1.0) {
                            hh1[2362]->Fill(KK0mass);
                            hh1[2462]->Fill(LLmass);
                            hh1[2562]->Fill(AALmass);

                            if (apt > 0.15) {
                                hh1[2363]->Fill(KK0mass);
                            }
                            if (apt < 0.15) {
                                hh1[2463]->Fill(LLmass);
                                hh1[2563]->Fill(AALmass);
                            }

                            if (omegalog > 1.5) {
                                hh1[2364]->Fill(KK0mass);
                                hh1[2464]->Fill(LLmass);
                                hh1[2564]->Fill(AALmass);

                                if (apt > 0.15) {
                                    hh1[2365]->Fill(KK0mass);
                                }
                                if (apt < 0.15) {
                                    hh1[2465]->Fill(LLmass);
                                    hh1[2565]->Fill(AALmass);
                                }

                                if (omegalog > 2.5) {
                                    hh1[2366]->Fill(KK0mass);
                                    hh1[2466]->Fill(LLmass);
                                    hh1[2566]->Fill(AALmass);

                                    if (apt > 0.15) {
                                        hh1[2367]->Fill(KK0mass);
                                    }
                                    if (apt < 0.15) {
                                        hh1[2467]->Fill(LLmass);
                                        hh1[2567]->Fill(AALmass);
                                    }

                                    if (omegalog > 3.0) {
                                        hh1[2368]->Fill(KK0mass);
                                        hh1[2468]->Fill(LLmass);
                                        hh1[2568]->Fill(AALmass);

                                        if (apt > 0.15) {
                                            hh1[2369]->Fill(KK0mass);
                                        }
                                        if (apt < 0.15) {
                                            hh1[2469]->Fill(LLmass);
                                            hh1[2569]->Fill(AALmass);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                //--------------------------------------

            } // End of if ( (CosT < -0.90) )

            //====================================================================
            if ((uptot < 3.0) && (untot > 0.3)) {
                if ((Z_Sec - pv[2]) > 2.) {
                    hh1[3310]->Fill(KK0mass);
                    hh1[3410]->Fill(LLmass);
                    hh1[3510]->Fill(AALmass);
                }

                if ((Z_Sec - pv[2]) > 3.) {
                    hh1[3311]->Fill(KK0mass);
                    hh1[3411]->Fill(LLmass);
                    hh1[3511]->Fill(AALmass);
                }

                if (ctau_k0 > 0.5) hh1[3350]->Fill(KK0mass);
                if (ctau_k0 > 1.5) hh1[3351]->Fill(KK0mass);
                if (ctau_k0 > 2.5) hh1[3352]->Fill(KK0mass);

                if (ctau_l0 > 2.5) hh1[3450]->Fill(LLmass);
                if (ctau_l0 > 5.0) hh1[3451]->Fill(LLmass);
                if (ctau_l0 > 7.5) hh1[3452]->Fill(LLmass);

                if (ctau_a0 > 2.5) hh1[3550]->Fill(AALmass);
                if (ctau_a0 > 5.0) hh1[3551]->Fill(AALmass);
                if (ctau_a0 > 7.5) hh1[3552]->Fill(AALmass);

                //                if (it_peak > 0) {
                //                    if (ctau_k0 > 0.5) hh1[4350]->Fill(KK0mass);
                //                    if (ctau_k0 > 1.5) hh1[4351]->Fill(KK0mass);
                //                    if (ctau_k0 > 2.5) hh1[4352]->Fill(KK0mass);
                //
                //                    if (ctau_l0 > 2.5) hh1[4450]->Fill(LLmass);
                //                    if (ctau_l0 > 5.0) hh1[4451]->Fill(LLmass);
                //                    if (ctau_l0 > 7.5) hh1[4452]->Fill(LLmass);
                //
                //                    if (ctau_a0 > 2.5) hh1[4550]->Fill(AALmass);
                //                    if (ctau_a0 > 5.0) hh1[4551]->Fill(AALmass);
                //                    if (ctau_a0 > 7.5) hh1[4552]->Fill(AALmass);
                //                }
            }

            //------------------------------------------------
            //------------------------------------------------
            //------------------------------------------------

        } // for (Int_t i = 0; i < nTracks;

    } // for (Int_t i = 0; i < nTracks;
}

void BmnPVAnalyzer::CopyDir(TDirectory *source) {
    //    TDirectory *savdir = gDirectory;

    //   cout<<" -------------------- Start CopyDir  --------------------------"<<endl;
    //   cout<<"J_ev_counts  =   "<<J_ev_counts<<endl;
    source->cd();
    //   source->ls("-m");
    //=============================================

    TList* obj_list = (TList*) source->GetList();
    TIter next_object((TList*) obj_list);
    TObject* obj;

    //  cout << "-- Booking objects:" << endl;
    while ((obj = next_object())) {
        TString objname = obj->GetName();

        //        cout << " " << objname << endl;

        //        TDirectory *adir = source->mkdir(objname);

        //        source->cd(objname);

        TList* obj_list1 = (TList*) ((TDirectory*) obj)->GetList();
        TIter next_object1((TList*) obj_list1);
        TObject* obj1;

        //    cout << "-- Booking objects:" << endl;
        while ((obj1 = next_object1())) {
            TString objname1 = obj1->GetName();
            //            cout << "1 " << objname1 << endl;

            //            adir->cd();
            ((TDirectory*) obj)->cd();
            //            ((TDirectory*)obj)->WriteObject(obj1, objname1.Data());
            obj1->Write();
        }
    }
}

//----------------------------------------------------------------------
//
//     ******************************************************************
//     *                                                                *
//     * Calculate invariant mass of NPAR particles with masses         *
//     *        AMS and momenta PPA                                     *
//     *      in OMEGA frame, E - the gamma energy                      *
//     * Output: EFMS - inv. mass (Gev), POUT - momentum of combin.     *
//     *                                                                *
//     ******************************************************************

void BmnPVAnalyzer::efmass(int &npar, double ams[], double ppa[][300],
        double &efms, double pout[]) {
    int i = 0, np = 0;
    double dpx = 0., dpy = 0., dpz = 0.;
    double dpxt = 0., dpyt = 0., dpzt = 0., detot = 0., dm = 0.;

    np = npar;
    if (npar > 50) {
        np = 50;
        //        cout<<"\n*** EFFMASS error: sorry, can not handle
        //         more than 50 particles.\n";
    }

    detot = (double) 0.;
    dpxt = (double) 0.;
    dpyt = (double) 0.;
    dpzt = (double) 0.;

    for (i = 0; i < np; i++) {
        dpx = (double) ppa[0][i];
        dpy = (double) ppa[1][i];
        dpz = (double) ppa[2][i];
        dm = (double) ams[i];

        dpxt += dpx;
        dpyt += dpy;
        dpzt += dpz;
        detot += sqrt(dpx * dpx + dpy * dpy +
                dpz * dpz + dm * dm);
    }

    efms = (double) sqrt(detot * detot - dpxt * dpxt -
            dpyt * dpyt - dpzt * dpzt);
    pout[0] = (double) dpxt;
    pout[1] = (double) dpyt;
    pout[2] = (double) dpzt;
    pout[3] = (double) detot;
    pout[4] = (double) sqrt(dpxt * dpxt + dpyt * dpyt
            + dpzt * dpzt);
    return;
}
//
//----------------------------------------------------------------------
//

void BmnPVAnalyzer::armentero(double& ppx, double& ppy, double& ppz,
        double& pnx, double& pny, double& pnz,
        double& ep, double& en,
        double& alfa, double& ptt,
        double& eppos, double& epmin) {
    /*- Local variables -*/
    double spx = 0., spy = 0., spz = 0., sp = 0.;
    double pp = 0., pn = 0., pl1 = 0., pl2 = 0., ptm = 0.;
    /*-------------------*/

    pp = sqrt(ppx * ppx + ppy * ppy + ppz * ppz);
    pn = sqrt(pnx * pnx + pny * pny + pnz * pnz);
    eppos = ep / pp;
    epmin = en / pn;

    spx = ppx + pnx;
    spy = ppy + pny;
    spz = ppz + pnz;
    sp = sqrt(spx * spx + spy * spy + spz * spz);

    // Armentero - Podolansky plot

    pl1 = (ppx * spx + ppy * spy + ppz * spz) / sp;
    pl2 = (pnx * spx + pny * spy + pnz * spz) / sp;
    alfa = (pl1 - pl2) / (pl1 + pl2);
    ptm = (1. - ((pl2 / pn)*(pl2 / pn)));

    ptm < 0. ? ptt = 0. : ptt = pn * sqrt(ptm);

    return;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//  Impact parameter of the track vs Vertex
//

void BmnPVAnalyzer::ddpoint(double& dxm, double& dym, double& dzm,
        double& dxs, double& dys, double& dzs,
        double& dpx, double& dpy, double& dpz,
        double& dptot, double dist[]) {
    /* Local variables */
    static double dd = 0., xmin = 0., ymin = 0., zmin = 0., dtx = 0., dty = 0.;

    /* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC */
    /*       XM,YM,ZM      - POSITION OF PRIMARY VERTEX               */
    /*       XS,YS,ZS      - POSITION OF SECONDARY VERTEX             */
    /*       PX,PY,PZ,PTOT - MOMENTUM OF TRACK                        */
    /*       DIST(1-4)     - POSITION OF NEAREST POINT ON THE TRACK   */
    /*                       AND DISTANSE                             */
    /* CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC   */

    /* Function Body */

    dtx = dpx / dpz;
    dty = dpy / dpz;

    dd = (dtx * (dxm - dxs) + dty * (dym - dys) +(dzm - dzs)) /
            (dtx * dtx + dty * dty + 1.);

    xmin = dxs + dd * dtx - dxm;
    ymin = dys + dd * dty - dym;
    zmin = dzs + dd - dzm;

    dd = sqrt(xmin * xmin + ymin * ymin + zmin * zmin);

    dist[0] = xmin;
    dist[1] = ymin;
    dist[2] = zmin;
    dist[3] = dd;

    return;
}

void BmnPVAnalyzer::vercda(double dvec[][300], double dww[],
        double dcda[][300], double dvert[],
        int& nv, double& dssq, int& nvhi) {
    //---------------------------------------------------------------
    int i = 0, j = 0, n = 0;
    double dd[3] = {0.}, dam[3][3] = {0.};
    double dcosv[3] = {0.};
    double dt11 = 0., dt12 = 0., dt13 = 0., dt22 = 0., dt23 = 0., dt33 = 0., det = 0., dsq = 0., dist = 0.;
    double dhicda = 0., dpdotn = 0.;
    //---------------------------------------------------------------
    dsq = -1000.0;
    dssq = 0.0;
    //---------------------------------------------------------------
    /*
         printf("%s \n", " ------------------------------------ ");
         printf("%s \n", " VERCDA VERCDA VERCDA ");

        i =0; j= 0;

         printf("%s%4i%10.4f%10.4f%10.4f %10.4f%10.4f \n","dvec 1 : ",i,
         dvec[0][0],dvec[1][0],dvec[2][0],dvec[3][0],dvec[4][0]);

         printf("%s%4i%10.4f%10.4f%10.4f %10.4f%10.4f \n","dvec 2 : ",j,
         dvec[0][1],dvec[1][1],dvec[2][1],dvec[3][1],dvec[4][1]);
         printf("%s \n","                ");

         if ( nv > 2)
         {
         printf("%s%4i%10.4f%10.4f%10.4f %10.4f%10.4f \n","dvec 3 : ",j,
         dvec[0][2],dvec[1][2],dvec[2][2],dvec[3][2],dvec[4][2]);
         printf("%s \n","                ");
        
         }
     */
    //---------------------------------------------------------------
    for (j = 0; j < 3; j++) {
        dd[j] = 0.;
        for (i = 0; i < 3; i++) {
            dam[i][j] = 0.;
        }
    }

    /*   ***   LOOP ON TRACKS */

    for (n = 0; n < nv; n++) {
        dcosv[2] = sqrt(1. / (dvec[3][n] * dvec[3][n] +
                dvec[4][n] * dvec[4][n] + 1.));
        dcosv[0] = dcosv[2] * dvec[3][n];
        dcosv[1] = dcosv[2] * dvec[4][n];
        //     printf("%s%12.7f%12.4f%12.7f%12.7f \n", "n dcosv: ",n,dcosv[0],dcosv[1],dcosv[2],dww[n]);
        dam[0][0] = dam[0][0]+(dcosv[0] * dcosv[0] - 1.) * dww[n];
        dam[1][1] = dam[1][1]+(dcosv[1] * dcosv[1] - 1.) * dww[n];
        dam[2][2] = dam[2][2]+(dcosv[2] * dcosv[2] - 1.) * dww[n];
        dam[0][1] = dam[0][1] + dcosv[0] * dcosv[1] * dww[n];
        dam[0][2] = dam[0][2] + dcosv[0] * dcosv[2] * dww[n];
        dam[1][2] = dam[1][2] + dcosv[1] * dcosv[2] * dww[n];

        dpdotn = dvec[0][n] * dcosv[0] + dvec[1][n] * dcosv[1] +
                dvec[2][n] * dcosv[2];
        //     printf("%s%4i%12.7f%12.7f%12.7f%12.7f \n", "n dcosv: ",n,dcosv[0],dcosv[1],dcosv[2],dpdotn);

        dd[0] += (dpdotn * dcosv[0] - dvec[0][n]) * dww[n];
        dd[1] += (dpdotn * dcosv[1] - dvec[1][n]) * dww[n];
        dd[2] += (dpdotn * dcosv[2] - dvec[2][n]) * dww[n];
    }
    //     printf("%s \n","                ");

    dt11 = dam[1][1] * dam[2][2] - dam[1][2] * dam[1][2];
    dt12 = dam[1][2] * dam[0][2] - dam[0][1] * dam[2][2];
    dt13 = dam[0][1] * dam[1][2] - dam[1][1] * dam[0][2];
    dt22 = dam[0][0] * dam[2][2] - dam[0][2] * dam[0][2];
    dt23 = dam[0][1] * dam[0][2] - dam[0][0] * dam[1][2];
    dt33 = dam[0][0] * dam[1][1] - dam[0][1] * dam[0][1];

    det = dam[0][0] * dt11 + dam[0][1] * dt12 + dam[0][2] * dt13;

    if (det == 0.) {
        goto L70;
    }

    det = 1. / det;
    //    printf("%s%12.7f \n", "dvert  : ",det);

    dvert[0] = det * (dd[0] * dt11 + dd[1] * dt12 + dd[2] * dt13);
    dvert[1] = det * (dd[0] * dt12 + dd[1] * dt22 + dd[2] * dt23);
    dvert[2] = det * (dd[0] * dt13 + dd[1] * dt23 + dd[2] * dt33);
    /*
         printf("%s%12.7f%12.7f%12.7f \n", "dt     : ",dt11,dt12,dt13);
         printf("%s%12.7f%12.7f%12.7f \n", "dt     : ",dt22,dt23,dt33);
         printf("%s%12.7f%12.7f%12.7f \n", "dd     : ",dd[0],dd[1],dd[2]);
         printf("%s%12.7f%12.7f%12.7f \n", "dvert  : ",dvert[0],dvert[1],dvert[2]);
         printf("%s \n","                ");
     */
    /*   ***   COMPUTATION OF C.D.A AND CHISQ */

    dssq = 0.;
    nvhi = 1;
    dhicda = 0.;

    for (n = 0; n < nv; n++) {

        dcosv[2] = sqrt(1. / (dvec[3][n] * dvec[3][n] +
                dvec[4][n] * dvec[4][n] + 1.));
        dcosv[0] = dcosv[2] * dvec[3][n];
        dcosv[1] = dcosv[2] * dvec[4][n];

        if (dww[n] <= 0.) continue;
        //	{
        //	    goto L50;
        //	}

        dist = 0.;

        for (i = 0; i < 3; ++i) {
            dd[i] = dvert[i] - dvec[i][n];
            dist += dd[i] * dcosv[i];
        }

        for (i = 0; i < 3; ++i) {
            dcda[i][n] = dvert[i] - dvec[i][n]
                    - dist * dcosv[i];
        }

        dsq = dcda[0][n] * dcda[0][n] + dcda[1][n] * dcda[1][n] + dcda[2][n] * dcda[2][n];
        dcda[3][n] = sqrt(dsq);

        //     printf("%s%7i%12.3f%12.3f%12.3f%12.3f \n", "n, dcda[0][n],dsq     : "
        //             ,n,dcda[0][n],dcda[1][n],dcda[2][n],dsq);

        if (dsq < dhicda) {
            goto L40;
        }

        dhicda = dsq;
        nvhi = n;
L40:
        dssq += dsq * dww[n];

        //L50:
        //	;
    }

    goto L999;

L70:
    dssq = 1e8;
    dvert[0] = 1.e5;
    dvert[1] = 1.e5;
    dvert[2] = 1.e5;

    for (i = 0; i < nv; ++i) {
        for (j = 0; j < 4; ++j) {
            dcda[j][i] = 1.e5;
        }
    }

L999:

    //     double dvvq;
    //     dvvq = dssq / (double) nv;
    //     printf("%s%12.3f%12.3f \n", " VERCDA 1 : dssq, dssq/nv -  ",dssq, dvvq);

    return;
}

void BmnPVAnalyzer::Decay_Plane(double track_Fp[], double track_Lp[], double track_Fm[], double track_Lm[], double& CosT) {
    double Tx, Ty, ptx, pty, ptz, pttot;
    TVector3 v1p, v1pn, v2p, v2pn;
    TVector3 v1m, v1mn, v2m, v2mn;
    TVector3 vp_Prod, vp_Prodn;
    TVector3 vm_Prod, vm_Prodn;

    //------------------------------------------------
    Tx = track_Fp[3];
    Ty = track_Fp[4];

    pttot = fabs(1. / (double) track_Fp[5]);
    ptz = pttot / sqrt(1. + Tx * Tx + Ty * Ty);
    ptx = ptz*Tx;
    pty = ptz*Ty;

    v1p.SetXYZ(ptx, pty, ptz);
    v1pn = v1p.Unit();

    Tx = track_Lp[3];
    Ty = track_Lp[4];

    pttot = fabs(1. / (double) track_Lp[5]);
    ptz = pttot / sqrt(1. + Tx * Tx + Ty * Ty);
    ptx = ptz*Tx;
    pty = ptz*Ty;

    v2p.SetXYZ(ptx, pty, ptz);
    v2pn = v2p.Unit();

    vp_Prod = v1pn.Cross(v2pn);
    vp_Prodn = vp_Prod.Unit();

    //--------------------------------------------------
    Tx = track_Fm[3];
    Ty = track_Fm[4];

    pttot = fabs(1. / (double) track_Fm[5]);
    ptz = pttot / sqrt(1. + Tx * Tx + Ty * Ty);
    ptx = ptz*Tx;
    pty = ptz*Ty;

    v1m.SetXYZ(ptx, pty, ptz);
    v1mn = v1m.Unit();

    Tx = track_Lm[3];
    Ty = track_Lm[4];

    pttot = fabs(1. / (double) track_Lm[5]);
    ptz = pttot / sqrt(1. + Tx * Tx + Ty * Ty);
    ptx = ptz*Tx;
    pty = ptz*Ty;

    v2m.SetXYZ(ptx, pty, ptz);
    v2mn = v2m.Unit();

    vm_Prod = v1mn.Cross(v2mn);
    vm_Prodn = vm_Prod.Unit();


    CosT = vp_Prodn.Dot(vm_Prodn);

    return;
}

void BmnPVAnalyzer::VirtualPlanes(int& N, int Niter, int& NZ, double& Z0, int List[], CbmStsTrack *CList[],
        double vert[], double vtx[], double dcda[][300], double& ssq) {
    int idpri = {0};
    int it = {0};
    CbmStsTrack * trk[300] = {nullptr};

    double qq[300] = {0.};
    double xf[300] = {0.}, yf[300] = {0.}, zf[300] = {0.};

    double ZH = 0., Zstep = {0.}, ZZ[100] = {0.};
    double ZL = 0.;

    double xp = 0., yp = 0., zp = 0.;
    double sx = 0., sy = 0., sz = 0.;
    double ssq2 = 0.;
    double Zt = 0.;

    double ssq_min = 0.;

    idpri = 1;

    ssq_min = 10000.;

    for (int i = 0; i < 3; ++i) {
        vert[i] = 100000.;
    }

    for (int i = 0; i < N; ++i) {
        it = List[i];
        trk[i] = CList[it];
        //        printf(" CList[%2i of %d] = %08X\n", i, N, trk[i]);
    }

    ZL = -450.;
    ZH = Z0;

    //-----------------------------------------------------------
    //  Loops on Virtual planes
    //
    //-----------------------------------------------------------
    //    printf("%s \n", "    ");
    //    printf("%s \n", "-------------------------------------");
    //    printf("%s \n", "        From VirtualPlanes");
    //    printf("%s \n", "-------------------------------------");
    //    printf("%s \n", "    ");
    //
    //    printf("%s \n", " Inside VirtualPlanes search ");
    //    printf("%s%6i%6i \n", " N, NZ ", N, NZ);
    //
    //    printf("%s ", " Track combination : ");
    //
    //    for (int l = 0; l < N; ++l) {
    //        printf("%10i ", List[l]);
    //    }
    //    printf("%s \n", " ");

    for (int l = 0; l < Niter; ++l) //  >>>>>>>>>>>>  Start of Iteration loop
    {

        ssq_min = 10000.;

        Zstep = (ZH - ZL) / (double) (NZ - 1);


        //        printf("%s%5i \n", " Iteration on Virtual plane : ", l);
        //        printf("%s%12.3f \n", " Step on Z : ", Zstep);

        for (int i = 0; i < NZ; ++i) {
            ZZ[i] = ZH - Zstep * (double) i;
            //            printf("%s%6i%10.3f \n", " i, ZZ ", i, ZZ[i]);
        }

        for (int i = 0; i < NZ; ++i) {
            qq[i] = 0.;
        }

        for (int i = 0; i < NZ; ++i) {

            //                 printf("%s \n","   ");
            //                 printf("%s%4i / %4i \n","Plane - ",i,NZ);

            sx = 0.;
            sy = 0.;
            sz = 0.;
            ssq2 = 0.;

            Zt = ZZ[i];

            for (int k = 0; k < N; ++k) {
                //                                 printf(" trk[%2i] = %08X\n",k, trk[k]);
                CbmKFTrack kftr = CbmKFTrack(*trk[k]);
                FairTrackParam param;
                kftr.Extrapolate(Zt);
                kftr.GetTrackParam(param);

                xp = param.GetX();
                yp = param.GetY();
                zp = param.GetZ();

                //                printf("%s%4i%s%10.3f%10.3f%10.3f \n", "Track - ", k, "  X/Y/Z - ", xp, yp, zp);

                xf[k] = xp;
                yf[k] = yp;
                zf[k] = zp;

                sx = sx + xp;
                sy = sy + yp;
                sz = sz + zp;

            } // End of for (int k=0; k<N; ++k) - Loop on Track

            sx = sx / (double) N;
            sy = sy / (double) N;
            sz = sz / (double) N;

            //            printf("%s%5i%10.3f%10.3f%10.3f \n", "Average X/Y/Z - ", i, sx, sy, sz);

            double summ;
            summ = 0.;
            for (int k = 0; k < N; ++k) {
                summ = summ + (sx - xf[k])*(sx - xf[k]) + (sy - yf[k])*(sy - yf[k]);
            }
            qq[i] = sqrt(summ) / (double) N;

            if (qq[i] < ssq_min) // Minimum value of Chi^2 at Virtual pane i
            {
                ssq_min = qq[i];
                vert[0] = sx;
                vert[1] = sy;
                vert[2] = sz;
                ssq = ssq_min;
                //        printf("%s%5i%10.3f%10.3f%10.3f%10.3f \n","Plane, - ",i,ssq_min,vert[0],vert[1],vert[2]);
            }

        } // End of for (int i=0; i<NZ; ++i) - Loop on Virtual Planes


        //        printf("%s \n", "    ");
        //        for (int i = 0; i < NZ; ++i) {
        //            printf("%s%5i%10.3f%10.3f \n", "i,ZZ[i],qq[i] - ", i, ZZ[i], qq[i]);
        //        }
        //        printf("%s \n", "    ");
        //        printf("%s%10.3f%10.3f%10.3f \n", "Vertex position vert - ", vert[0], vert[1], vert[2]);
        //        printf("%s \n", "    "); //-----------------------------------------------------------

        ZL = vert[2] - 1.5 * Zstep; //  Set up a new region for the
        ZH = vert[2] + 1.5 * Zstep; //  minimum search

        //-----------------------------------------------------------
        //     printf("%s%10.3f%10.3f \n","New Z limits ZL, ZH ", ZL, ZH);
        //     printf("%s \n","    ");
        //-----------------------------------------------------------

    } //<<<<<<<<<<<  End of Iteration loop:  for (int l=0; l<Niter; ++l)
    //-----------------------------------------------------------
    //
    //  Fit to improuve the resolution. Stright line fit/
    //
    int ntrk = 0, nvhi = 0;
    double vec[5][300] = {0.}, ww[300] = {0.}, vvv[3] = {0.}, ccd[4][300] = {0.};
    double ssu = 0.;

    for (int k = 0; k < N; ++k) {
        CbmKFTrack kftr = CbmKFTrack(*trk[k]);
        FairTrackParam param;
        kftr.Extrapolate(vert[2]);
        kftr.GetTrackParam(param);

        vec[0][k] = param.GetX();
        vec[1][k] = param.GetY();
        vec[2][k] = param.GetZ();
        vec[3][k] = param.GetTx();
        vec[4][k] = param.GetTy();
        ww[k] = 1.;
    }

    ntrk = N;
    nvhi = -1;
    ssu = 1.e10;

    vercda(vec, ww, ccd, vvv, ntrk, ssu, nvhi);

    //    printf("%s%12.3f \n", " VERCDA 2 : dssq -  ", ssu);

    ssq = ssu; //  Vertex chi^2 

    vtx[0] = vvv[0]; //  Vertex position                          
    vtx[1] = vvv[1];
    vtx[2] = vvv[2];

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < 4; ++j) {
            dcda[j][i] = ccd[j][i]; //  Distance from PV to the each track
        }
    }

    return;
}

bool BmnPVAnalyzer::combi(int ic[], int& nc, int& jc) {
    int i, i1, k;

    if (nc <= 0 || jc <= 0) return false;

    if (jc > nc)
        cout << "\n Error : j > n, must be : j< n  ";
    else {
        if (ic[0] == 0) {
            for (i = 0; i < jc; i++) ic[i] = i + 1;
            ic[jc] = 0;
        } else {
            for (i1 = 0; i1 < nc; i1++) {
                k = i1;
                if (ic[k + 1] != ic[k] + 1)
                    break;
                ic[k] = k + 1;
            }
            ic[k] = ic[k] + 1;
            if (ic[jc - 1] == nc + 1) ic[0] = 0;
        }
    }

    //printf("%s%10i \n","From combi : ic[0] = ",ic[0]);

    if (ic[0] == 0) {
        return false;
    } else return true;
}

void BmnPVAnalyzer::vtx_topol(int& N, int Niter, int& NZ, double& Z0, int iList[], CbmStsTrack *CList[],
        int& jfirst, int& itpl, int ntopol[], int inxtpl[][VTX_LEN],
        double V_XYZ[][VTX_LEN], double V_cda[][VTX_LEN][VTX_LEN], double vxhi2[]) {
    //=======================================================================
    //  N  - Number of tracks
    //  NZ - Number of virtual planes to cearch the primary vertec
    //  Z0 - Limit on Z downstream the beam for the fiducial volume search
    //  iList[] - list of the selected tracks for for the vestex search
    //  CList[] - Tracks parameters for all tracks
    //
    //  jfirst    - The Primary vertex among all topologies
    //  itpl      - Number of reconstructed topologies
    //  ntopol    - Number of tracks in each topologies
    //  inxtpl    - List of tracks in each topology.
    //  Prim_XYZ  - X/Y/Z position of the topology
    //  vxhi2     - Chi^2 of the topology  
    //=======================================================================
    //
    TStopwatch timer;
    timer.Start();

    int idpri;
    int ltr, ltrnew;
    int limtar, itplmx;
    int itag_orig[50];
    int nt_save;
    int n0save;
    int icmb_save;

    int List[50];
    int ListSave[50];
    int iset[50], jbank[50], index[50];
    int icmb, newcmb;

    double ssold;

    double dcda[4][300];
    double vert[3];
    double zvtx[3];
    double ssq;

    double xyzta1[0][10], xyzta2[0][10];
    double cmvert[20][10];
    double dvercut[20];
    double zt1, zt2;

    /*        To restore Verteces in space. */

    idpri = 0;

    if (N < 2) return;

    itplmx = 50;

    //-------------------------------------------------------------
    //  Information to save

    itpl = 0; //  Number of Primaries
    jfirst = -1; //  Primary vertex pointer

    for (int ii = 0; ii < itplmx; ii++) {
        ntopol[ii] = 0; //  Number of Tracks in topology
        itag_orig[ii] = 0; //  The interaction Target

        for (int jj = 0; jj < itplmx; jj++) {
            inxtpl[ii][jj] = 0;
        }
    }

    for (int ii = 0; ii < 4; ii++) //  Minimum distance : track - vertex
    {
        for (int jj = 0; jj < N; jj++) {
            dcda[ii][jj] = 10000.;
        }
    }


    ltr = N;
    ltrnew = N;

    for (int ii = 0; ii < ltr; ii++) {
        jbank[ii] = -1;
    }

    for (int k = 0; k < ltr; ++k) {
        ListSave[k] = iList[k];
    }
    //----------------------------------------------------
    /*
    idpri = 1;
    if (idpri > 0)
    {
       cout<<"  "<<endl; 
       cout<<" ------------ Start list of the tracks ---------------"<<endl; 

       cout<<"iList "; 
       for (int ii = 0; ii < ltr; ii++)
       {
         cout<<iList[ii]<<" ";
       }
       cout<<endl<<endl;
    }
    idpri = 0;
     */
    //===========================================

    //===========================================
    //     L5:L5:L5:L5:L5:L5:L5:L5:
    // Return point for the vertex search
    // when one of the verteces already found                     
L5:
    ;

    //---------------------------------------------------------------
    /*
    idpri = 0;
    if (idpri > 0)
    {
    printf("%s \n\n","  ======   Print out at L5  =====");
    }
    idpri = 0;
     */
    //if (il > 0) return;
    //++il;
    //---------------------------------------------------------------

    ltr = ltrnew;
    newcmb = 0;

    //  111111111111111111111111111111111111111111111111111111111
    /*
    idpri = 1;
    if (idpri > 0)
    {
    printf("%s \n\n","  ======   Start combination Loops :  for (icmb = ltr; icmb >1; --icmb) =====");
    }
    idpri = 0;
     */
    //printf("%s \n","  ");

    for (icmb = ltr; icmb > 1; --icmb) // Track combinations $C_{ltr}^{icmb}$
        //    for (icmb = ltr; icmb >= (N-2); --icmb) 
    {

        //  111111111111111111111111111111111111111111111111111111111

        ssold = 200000.;
        for (int ii = 0; ii < ltr; ii++) {
            iset[ii] = 0;
        }

        //===========================================
        //    L10:L10:L10:L10:L10:L10:L10:L10:

L10:
        ;

        int iWhile;
        iWhile = 0;
        nt_save = 0;
        n0save = 0;

        /*
        idpri = 0;
        if (idpri > 0)
        {
        printf("%s \n\n","  ======   L10:  Start combi      :  while ( combi(iset, ltr, icmb) ) =====");
        }
        idpri = 0;
         */

        while (combi(iset, ltr, icmb)) // Generating track combinations
        {
            iWhile = 1;

            //===========================================

            ssq = 1.e5;
            for (int k = 0; k < icmb; ++k) {
                List[k] = ListSave[iset[k] - 1];
            }
            //----------------------------------------------------

            /*
            idpri = 1;
            if (idpri > 0)
            {
               cout<<" ------------ Start a new combination ---------------"<<endl; 
               cout<<" ------------ With number of tracks  =  "<<icmb<<endl; 
               cout<<" ------------ iset "; 
               for (int ii = 0; ii < icmb; ii++)
               {
                 cout<<iset[ii]<<" ";
               }
               cout<<endl<<endl;
            }
            idpri = 0;
             */

            //----------------------------------------------------

            /*
            idpri = 0;
            if (idpri > 1)
            {
               cout<<" ------------ Formed list  ---------------"<<endl; 
               cout<<" ------------ iset[k]-1  ---------------"<<endl; 
               cout<<"List[iset[k]-1]  "; 
               for (int ii = 0; ii < icmb; ii++)
               {
                 cout<<List[ii]<<" ";
               }
               cout<<endl<<endl;
            }
            idpri = 0;
             */

            //===========================================

            // VirtualPlanes - Vertex search for a given track combination

            VirtualPlanes(icmb, Niter, NZ, Z0, List, CList, vert, zvtx, dcda, ssq);

            double uchi;
            uchi = 0.;
            for (int mm = 0; mm < icmb; mm++) {
                uchi = uchi + dcda[0][mm] * dcda[0][mm] + dcda[1][mm] * dcda[1][mm] +
                        dcda[2][mm] * dcda[2][mm];
            }

            uchi = sqrt(uchi) / (double) icmb;

            ssq = uchi;

            if (ssq > ssold) //  Search the best vertex candidate by chi^2
            {

                /*
                idpri = 1;
                if (idpri > 0)
                {
                printf("%s%15.3f%15.3f \n","   Combination rejected :   (ssq > ssold) : ",ssq,ssold);
                cout<<"   Go To L15"<<endl;

                }
                idpri = 0;

                 */
                goto L15; // Ask for a new tracks combination
            }
            /*
            idpri = 1;
            if (idpri > 0)
            {
            printf("%s%15.3f%15.3f \n","   Combination accepted :   (ssq < ssold) : ",ssq,ssold);
            }
            idpri = 0;
             */

            //===========================================
            //          Check all possible tagets
            //cout<<"Check for quality"<<endl;
            //===========================================

            limtar = 1;

            for (int nt = 0; nt < limtar; nt++) //  Loop on targets set
            {
                for (int j = 0; j < 20; j++) {
                    dvercut[j] = (double) cmvert[j][nt];
                }

                zt1 = (double) xyzta1[2][nt]; //  Check Z of the vertex
                zt2 = (double) xyzta2[2][nt];

                zt1 = -450.;
                zt2 = 150.;

                if (zvtx[2] < zt1) goto L20;
                if (zvtx[2] > zt2) goto L20;

                dvercut[7] = 0.4;
                /*
                idpri = 0;
                if (idpri > 0)
                {
                printf("%s\n","   ");
                printf("%s%10.2f\n","           dvercut[7] = ",dvercut[7]);
                for (int j = 0; j < icmb; j++) 
                {
                  printf("%s%10i%10i%15.4f \n"," j,dcda[3][j]  ", j, List[j], dcda[3][j]);
                }
                printf("%s\n","   ");
                }
                idpri = 0;
                 */
                for (int j = 0; j < icmb; j++) //  Check the track - vertex impackt
                {
                    if (dcda[3][j] > dvercut[7]) {
                        /*
                        idpri = 0;
                        if (idpri > 0)
                        {
                        printf("%s "    ,"    Combination rejected by dcda[][] cuts");
                        printf("\n%s \n\n"  ,"    go to L20, end of target loop ");
                        }
                        idpri = 0;
                         */
                        goto L20;
                    }
                }

                nt_save = nt; // Save target signature

                /*
                idpri = 0;
                if (idpri > 0)
                {
                printf("%s \n","  Save target signature go to L30");
                }
                idpri = 0;
                 */

                goto L30; // save combination

L20:
                ;

            } // End Loop on targets set

            /*
            idpri = 0;
            if (idpri > 0)
            {
            printf("%s \n","  After target end of loop go to L15");
            }
            idpri = 0;
             */

            goto L15; // Ask for a new tracks combination
            //===========================================
            /*      Save the combination */
            //===========================================

L30:
            ;

            /*
            idpri = 0;
            if (idpri > 0)
            {
            printf("%s \n","  L30: - Saving the Vertex");
            }
            idpri = 0;
             */

            ssold = ssq;

            vxhi2[itpl] = ssq;

            for (int j = 0; j < 3; j++) {
                V_XYZ[j][itpl] = zvtx[j];
            }

            icmb_save = 0;
            for (int j = 0; j < icmb; j++) {
                inxtpl[j][itpl] = List[j];

                //               cout<<"inxtpl[j][itpl],j,itpl  " << List[j] << "   " << j << "   " << itpl<<endl;

                for (int l = 0; l < 4; l++) {
                    V_cda[l][icmb_save][itpl] = dcda[l][List[j]];
                }
                ++icmb_save;
            }
            //            cout<<endl;


            ntopol[itpl] = icmb_save; // Number of tracks in the vertex
            itag_orig[itpl] = nt_save; // Target index if multitarget

            //            if (n0save == 0) ++itpl;

            n0save = 1;

            if (itpl > itplmx) //  Too many verteces ( max = 50)
            {
                goto L1000;
            }

            //--------------------------------------------------------------
L15:
            ;

            /*
            idpri = 0;
            if (idpri > 0)
            {
            printf("\n%s \n","  L15 : - End of:   while ( combi(iset, ltr, icmb) )");
            }
            idpri = 0;
             */

        } // End of:   while ( combi(iset, ltr, icmb) )

        //--------------------------------------------------------------

        if (n0save == 0) {

            /*
            idpri = 0;
            if (idpri > 0)
            {
            printf("%s \n","  ");
            printf("%s \n"," Nothing is found:  n0save == 0");
            printf("%s \n","  ");
            }
            idpri = 0;
             */

            continue;
        }

        if (n0save > 0) {

            /*
            idpri = 0;
            if (idpri > 0)
            {
                        printf("%s \n","  ");
                        printf("%s \n"," Vertex is found: n0save > 0");
                        printf("%s \n","  ");

                        cout<<"inxtpl[j]  :::::    " ;
            }
            idpri = 0;
             */

            ++itpl;

            /*
            idpri = 0;
            if (idpri > 0)
            {
                        for (int j = 0; j < ntopol[itpl-1]; j++) 
                        {
                            cout<<inxtpl[j][itpl-1]<<"   " ;
                        }
                        cout<<endl;
            }
            idpri = 0;
             */

            for (int j = 0; j < ntopol[itpl - 1]; j++) {
                for (int k = 0; k < ltr; k++) {
                    if (iList[k] == inxtpl[j][itpl - 1]) jbank[k] = inxtpl[j][itpl - 1];
                }
            }

            /*
            idpri = 0;
            if (idpri > 0)
            {
                        cout<<"jbank[j]   :::::    " ;
                        for (int j = 0; j < ltr; j++) 
                        {
                            cout<<jbank[j]<<"   " ;
                        }
                        cout<<endl;
            }
            idpri = 0;
             */

            int L = 0;
            for (int j = 0; j < ltr; j++) {
                if (jbank[j] < 0) {
                    ListSave[L] = iList[j];
                    ++L;
                }
            }

            ltrnew = L;

            /*
            idpri = 0;
            if (idpri > 0)
            {
                        cout<<"ltrnew  =   "<<ltrnew <<endl;
                        cout<<"ListSave[L]   :::::    " ;

                        for (int j = 0; j < L; j++) 
                        {
                            cout<<ListSave[j]<<"   " ;
                        }
                        cout<<endl;
            }
            idpri = 0;
             */

            if (ltrnew < 2) {
                goto L1000;
            }

            //	    cout<<"Going back to combi :  to L5    "<<endl ;

            goto L5; // Repeat the procedure with the rest part of the tracks
        }

        //  111111111111111111111111111111111111111111111111111111111

    }; // End of:   for (icmb = ltr; icmb >1; --icmb) 

    //  111111111111111111111111111111111111111111111111111111111

L1000:

    if (itpl == 0) goto L9999;

    //===========================================
    /*   Select Primary Vertex */
    //===========================================

    jfirst = -1;
    if (itpl == 1) {
        jfirst = 0;
    }

    if (itpl > 1) {
        int ntmin;
        ntmin = -1;

        jfirst = -1;

        for (int j = 0; j < itpl; j++) // Vertex with the maximum tracks is the primary.
        {
            if (ntopol[j] >= ntmin) {
                ntmin = ntopol[j];
                jfirst = j;
            }
        }
    }

L9999:
    ;

    //---------------------------------------------
    //---------------------------------------------
    /*
    idpri = 1;
    if (idpri > 0 )

    {
      cout <<endl<<" VTX_TOPOL statistic (Pure) "<<endl;
      cout<<"The number of verteces: "<<itpl<<endl;
      cout<<"The primary vertex    : "<<jfirst<<endl;
      cout<<endl<<"The vertex list"<<endl<<endl;

      for (int it = 0; it < itpl; it++) 
      {
        cout<<"The vertex # "<<(it+1)<<endl;
        cout<<"The total number tracks in it : "<<ntopol[it]<<endl;
        cout<<"The list of tracks            : ";
        for (int i = 0; i < ntopol[it]; i++) 
        {
           cout<<inxtpl[i][it]<<" ";
        }
        cout<<endl;
        cout<<"The vertex position X Y Z     : ";
        cout<<V_XYZ[0][it]<<"   "<<V_XYZ[1][it]<<"   "<<V_XYZ[2][it]<<"   "<<vxhi2[it]<<endl<<endl;
      }
    }
    idpri = 0;
     */
    //---------------------------------------------
    //---------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    if (fVerbose > 1) {
        cout << endl << endl;
        cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    }

    return;

} /* vtx_topol__ */

void BmnPVAnalyzer::v5_topol(int& N, int Niter, int& NZ, double& Z0, int iList[], CbmStsTrack *CList[],
        int& jfirst, int& itpl, int ntopol[], int inxtpl[][VTX_LEN],
        double V_XYZ[][VTX_LEN], double V_cda[][VTX_LEN][VTX_LEN], double vxhi2[],
        double& x_avr, double& y_avr, double& z_avr, int& jNtracks, int jList[]) {
    //=======================================================================
    //  N  - Number of tracks
    //  NZ - Number of virtual planes to cearch the primary vertec
    //  Z0 - Limit on Z downstream the beam for the fiducial volume search
    //
    //  jfirst    - The Primary vertex among all topologies
    //  itpl      - Number of reconstructed topologies
    //  ntopol    - Number of tracks in each topologies
    //  inxtpl    - List of tracks in each topology.
    //  Prim_XYZ  - X/Y/Z position of the topology
    //  vxhi2     - Chi^2 of the topology  
    //=======================================================================
    //

    int idpri_local = 0;
    int ltr = 0, ltrnew = 0;
    int limtar = 0, itplmx = 0;
    int itag_orig[VTX_LEN] = {0};
    int nt_save = 0;
    int n0save = 0;
    int icmb_save = 0;

    int ListTpl[VTX_LEN][VTX_LEN] = {0};
    int List[VTX_LEN] = {0};
    int ListSave[VTX_LEN] = {0};
    int iset[VTX_LEN] = {0}, jbank[VTX_LEN] = {0}, index[VTX_LEN] = {0};
    int icmb = 0, newcmb = 0;

    double ssold = 0.;

    double dcda[4][300] = {0.};
    double vert[3] = {0.};
    double zvtx[3] = {0.};
    double ssq = 0.;

    double xyzta1[3][10] = {0.}, xyzta2[3][10] = {0.};
    double cmvert[20][10] = {0.};
    double dvercut[20] = {0.};
    double zt1 = 0., zt2 = 0.;

    int lav = 0;
    double v_avr[3][5000] = {0.};
    double chi2_avr[5000] = {0.};

    //    static bool first = true;
    //
    //    if (first) {
    //        first = false;
    //    }

    //    cdTracks->cd();

    /*        To restore Verteces in space. */

    idpri_local = 0;

    if (N < 2) return;

    itplmx = 50;

    //-------------------------------------------------------------
    //  Information to save

    itpl = 0; //  Number of Primaries
    jfirst = -1; //  Primary vertex pointer

    for (int ii = 0; ii < itplmx; ii++) {
        ntopol[ii] = 0; //  Number of Tracks in topology
        itag_orig[ii] = 0; //  The interaction Target

        for (int jj = 0; jj < itplmx; jj++) {
            inxtpl[ii][jj] = 0;
        }
    }

    for (int ii = 0; ii < 4; ii++) {
        for (int jj = 0; jj < N; jj++) {
            dcda[ii][jj] = 10000.;
        }
    }

    lav = 0;
    for (int ii = 0; ii < 3; ii++) {
        for (int jj = 0; jj < 5000; jj++) {
            v_avr[ii][jj] = 0.;
        }
    }


    ltr = N;
    ltrnew = N;

    icmb = ICMB;
    if (N < icmb) icmb = N;

    //int il;
    //il= 0;

    for (int ii = 0; ii < ltr; ii++) {
        jbank[ii] = -1;
    }

    for (int k = 0; k < ltr; ++k) {
        ListSave[k] = iList[k];
    }

    //===========================================
    //     L5:L5:L5:L5:L5:L5:L5:L5:

L5:
    ;

    //if (il > 0) return;
    //++il;

    ltr = ltrnew;
    newcmb = 0;

    //  111111111111111111111111111111111111111111111111111111111
    //printf("%s \n","  ======   Start combination Loops :  for (int ii = 0; ii < ltr; ii++) =====");
    //printf("%s \n","  ");

    //    for (icmb = ltr; icmb >1; --icmb) 
    //    for (icmb = ltr; icmb >= (N-2); --icmb) 
    {

        //  111111111111111111111111111111111111111111111111111111111
        ssold = 200000.;
        for (int ii = 0; ii < ltr; ii++) {
            iset[ii] = 0;
        }

        //===========================================
        //    L10:L10:L10:L10:L10:L10:L10:L10:

L10:
        ;

        int iWhile;
        iWhile = 0;
        nt_save = 0;
        n0save = 0;

        //        printf("%s \n", "  ======   Start combi             :  while ( combi(iset, ltr, icmb) ) =====");
        //        printf("%s \n", "  ");
        //        //        ltr = 3;
        //        Int_t iiter = 0;
        while (combi(iset, ltr, icmb)) {
            //            printf("iter %d\n", iiter++);
            iWhile = 1;

            //===========================================

            ssq = 1.e5;
            for (int k = 0; k < icmb; ++k) {
                List[k] = ListSave[iset[k] - 1];
            }
            //----------------------------------------------------
            /*
            idpri = 1;
            if (idpri > 0)
            {
               cout<<" ------------ Start a new combination ---------------"<<endl; 
               cout<<"With number of tracks  =  "<<icmb<<endl; 
               cout<<"iset "; 
               for (int ii = 0; ii < icmb; ii++)
               {
                 cout<<iset[ii]<<" ";
               }
               cout<<endl<<endl;
            }
            idpri = 0;
             */
            //----------------------------------------------------
            /*
            idpri = 1;
               cout<<" ------------ Formed list  ---------------"<<endl; 
            if (idpri > 0)
            {
               cout<<"List "; 
               for (int ii = 0; ii < icmb; ii++)
               {
                 cout<<List[ii]<<" ";
               }
               cout<<endl<<endl;
            }
            idpri = 0;
             */
            //===========================================

            VirtualPlanes(icmb, Niter, NZ, Z0, List, CList, vert, zvtx, dcda, ssq);

            double uchi = 0.;
            for (int mm = 0; mm < icmb; mm++) {
                uchi = uchi + dcda[0][mm] * dcda[0][mm] + dcda[1][mm] * dcda[1][mm] + dcda[2][mm] * dcda[2][mm];
            }

            uchi = sqrt(uchi) / (double) icmb;

            ssq = uchi;

            v_avr[0][lav] = zvtx[0];
            v_avr[1][lav] = zvtx[1];
            v_avr[2][lav] = zvtx[2];
            chi2_avr[lav] = ssq;

            hh1[6021]->Fill(zvtx[0]);
            hh1[6022]->Fill(zvtx[1]);
            hh1[6023]->Fill(zvtx[2]);
            hh1[6030]->Fill(ssq);

            for (int j = 0; j < icmb; j++) {
                hh1[6035]->Fill(dcda[3][j]);
            }

            if (lav < 5000) ++lav;
            /*
               cout<<"List "; 
               for (int ii = 0; ii < icmb; ii++)
               {
                 cout<<List[ii]<<" ";
               }
               cout<<endl;

                printf("%s%10i%10i\n"," ltr,  icmb ",ltr, icmb);
                cout<<"The vertex position X Y Z     : ";
                cout<<zvtx[0]<<"      "<<zvtx[1]<<"       "<<zvtx[2]<<endl;
             */
            //printf("%s%15.4f\n"," zvtx[2] = ",zvtx[2]);
            //            hh1[5010]->Fill(zvtx[2]);

            if (ssq > ssold) {
                //printf("%s%15.3f%15.3f \n","   Combination rejected :  if(ssq > ssold) : ",ssq,ssold);

                goto L15;
            }

            //===========================================
            //          Check all possible tagets
            //cout<<"Check for quality"<<endl;
            //===========================================

            limtar = 1;

            for (int nt = 0; nt < limtar; nt++) //  Loop on targets set
            {
                for (int j = 0; j < 20; j++) {
                    dvercut[j] = (double) cmvert[j][nt];
                }

                zt1 = (double) xyzta1[2][nt];
                zt2 = (double) xyzta2[2][nt];

                zt1 = -450.;
                zt2 = 150.;

                if (zvtx[2] < zt1) goto L20;
                if (zvtx[2] > zt2) goto L20;

                dvercut[7] = 10000.4;
                /*
                printf("%s\n","   ");
                printf("%s%15.4f\n"," dvercut[7] = ",dvercut[7]);
                for (int j = 0; j < icmb; j++) 
                {
                  printf("%s%10i%10i%15.4f \n"," j,dcda[3][j]  ", j, List[j], dcda[3][j]);
                }
                printf("%s\n","   ");
                 */
                for (int j = 0; j < icmb; j++) {
                    if (dcda[3][j] > dvercut[7]) {
                        //printf("%s \n","  Combination rejected by dcda[][] cuts");
                        goto L20;
                    }
                }

                nt_save = nt; // Save target signature

                //printf("%s \n","  After target loop go to L15");

                goto L30; // save combination

L20:
                ;

            } // End Loop on targets set

            //printf("%s \n","  After target end of loop go to L15");
            goto L15;
            //===========================================
            /*      Save the combination */
            //===========================================

L30:
            ;

            //            printf("%s \n", "  L30: - Saving the Vertex");

            n0save = 1;
            ssold = ssq;

            vxhi2[itpl] = ssq;

            hh1[6031]->Fill(ssq);

            for (int j = 0; j < icmb; j++) {
                ListTpl[j][itpl] = List[j];
            }

            for (int j = 0; j < 3; j++) {
                V_XYZ[j][itpl] = zvtx[j];
            }

            hh1[6010]->Fill(zvtx[2]);
            hh2[6010]->Fill(zvtx[0], zvtx[1]);

            icmb_save = 0;
            for (int j = 0; j < icmb; j++) {
                inxtpl[j][itpl] = List[j];

                for (int l = 0; l < 4; l++) {
                    V_cda[l][icmb_save][itpl] = dcda[l][List[j]];
                }
                ++icmb_save;
            }


            ntopol[itpl] = icmb_save;
            itag_orig[itpl] = nt_save;

            ++itpl;

            if (itpl > itplmx) //  Too many verteces
            {
                goto L1000;
            }

            //--------------------------------------------------------------
L15:
            ;

        } // End of:   while ( combi(iset, ltr, icmb) )

        //--------------------------------------------------------------

        //	if (n0save == 0 ) 
        //	{
        //        printf("%s \n", " n0save == 0");
        //        printf("jfirst = %d itpl = %d\n", jfirst, itpl);
        //           continue;
        //        }

        if (n0save > 0) {

            /*
            printf("%s \n"," n0save > 0");
                        cout<<"inxtpl[j]  :::::    " ;
                        for (int j = 0; j < ntopol[itpl-1]; j++) 
                        {
                            cout<<inxtpl[j][itpl-1]<<"   " ;
                        }
                        cout<<endl;
             */
            for (int j = 0; j < ntopol[itpl - 1]; j++) {
                for (int k = 0; k < ltr; k++) {
                    if (iList[k] == inxtpl[j][itpl - 1]) jbank[k] = inxtpl[j][itpl - 1];
                }
            }
            /*
                        cout<<"jbank[j]   :::::    " ;
                        for (int j = 0; j < ltr; j++) 
                        {
                            cout<<jbank[j]<<"   " ;
                        }
                        cout<<endl;
             */
            int L = 0;
            for (int j = 0; j < ltr; j++) {
                if (jbank[j] < 0) {
                    ListSave[L] = iList[j];
                    ++L;
                }
            }

            //	    ltrnew = L;
            /*
                        cout<<"ltrnew  =   "<<ltrnew <<endl;
                        cout<<"ListSave[L]   :::::    " ;
                        for (int j = 0; j < L; j++) 
                        {
                            cout<<ListSave[j]<<"   " ;
                        }
                        cout<<endl;
             */
            //	    if (ltrnew < 2)
            {
                goto L1000;
            }

            //	    cout<<"Going back to combi :  to L5    "<<endl ;

            //            goto L5;
        }

        //  111111111111111111111111111111111111111111111111111111111

    }; // End of:   for (icmb = ltr; icmb >1; --icmb) 

    //  111111111111111111111111111111111111111111111111111111111

L1000:
    if (itpl == 0) goto L9999;

    //===========================================
    /*   Select Primary Vertex */
    //===========================================
    int ntmin;

    ntmin = 0;
    jfirst = -1;
    if (itpl == 1) {
        jfirst = 0;
        ntmin = ntopol[jfirst];
    }

    if (itpl > 1) {
        ntmin = -1;

        jfirst = -1;
        /*
                for (int j = 0; j < itpl; j++) 
                {
                    if (ntopol[j] >= ntmin) 
                    {
                        ntmin = ntopol[j];
                        jfirst = j;
                    }
                }
         */
        double ch2min;
        ch2min = 10.e7;
        for (int j = 0; j < itpl; j++) {
            if (vxhi2[j] < ch2min) {
                ch2min = vxhi2[j];
                jfirst = j;
            }
        }
    }

L9999:
    ;

    if (jfirst < 0)
        return;
    ntmin = ntopol[jfirst];
    for (int j = 0; j < ntmin; j++) {
        hh1[6036]->Fill(V_cda[3][j][jfirst]);
    }

    x_avr = 0.;
    y_avr = 0.;
    z_avr = 0.;

    for (int jj = 0; jj < lav; jj++) {
        x_avr = x_avr + v_avr[0][jj];
        y_avr = y_avr + v_avr[1][jj];
        z_avr = z_avr + v_avr[2][jj];
    }

    if (lav > 0) {
        x_avr = x_avr / (double) lav;
        y_avr = y_avr / (double) lav;
        z_avr = z_avr / (double) lav;

        hh1[6025]->Fill(x_avr);
        hh1[6026]->Fill(y_avr);
        hh1[6027]->Fill(z_avr);
    }

    if (lav > 1) {
        for (int jj = 0; jj < lav - 1; jj++) {
            double x1, y1, z1;
            x1 = v_avr[0][jj];
            y1 = v_avr[1][jj];
            z1 = v_avr[2][jj];

            for (int kk = jj + 1; kk < lav; kk++) {
                double x2, y2, z2;
                x2 = v_avr[0][kk];
                y2 = v_avr[1][kk];
                z2 = v_avr[2][kk];

                double delta;
                delta = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2));
                hh1[6032]->Fill(delta);

                delta = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
                hh1[6033]->Fill(delta);
            }
        }
    }
    //    printf("%s%10.4f%10.4f%10.4f \n", " Average X/Y/Z  ", x_avr, y_avr, z_avr);
    //---------------------------------------------
    int ntsize = 0;
    int nacc = 0;
    double xp = 0., yp = 0., zp = 0.;
    double xs = 0., ys = 0., zs = 0.;
    double Tx_local = 0., Ty_local = 0.;
    double PX = 0., PY = 0., PZ = 0., PP = 0.;
    double Cda[4] = {0.}, TrCda[4][VTX_LEN] = {0.};

    nacc = 0;

    xp = V_XYZ[0][jfirst];
    yp = V_XYZ[1][jfirst];
    zp = V_XYZ[2][jfirst];

    for (int i = 0; i < Nsize; i++) {
        CbmKFTrack kftr = CbmKFTrack(*CList[i]);
        FairTrackParam param;
        kftr.Extrapolate(zp);
        kftr.GetTrackParam(param);

        xs = param.GetX();
        ys = param.GetY();
        zs = param.GetZ();

        Tx_local = CList[i]->GetParamFirst()->GetTx();
        Ty_local = CList[i]->GetParamFirst()->GetTy();
        PP = CList[i]->GetParamFirst()->GetQp();
        if (fabs(PP) > 0.) PP = 1. / PP;
        PZ = PP / sqrt(1. + Tx_local * Tx_local + Ty_local * Ty_local);
        PX = PZ*Tx_local;
        PY = PZ*Ty_local;

        ddpoint(xp, yp, zp, xs, ys, zs, PX, PY, PZ, PP, Cda);

        hh1[6040]->Fill(Cda[3]);

        if (Cda[3] < 3.) {
            jList[nacc] = i;
            if (nacc < 50) ++nacc;
        }
    }

    jNtracks = nacc;

    hh1[6041]->Fill(Nsize);
    hh1[6042]->Fill(nacc);
    hh2[6042]->Fill(Nsize, nacc);

    return;
} /* v5_topol__ */

Int_t BmnPVAnalyzer::GetNofModules(TGeoNode* station) {
    Int_t nModules = 0;
    for (Int_t iModule = 0; iModule < station->GetNdaughters(); iModule++) {
        TGeoNode* module = station->GetDaughter(iModule);
        if (TString(module->GetName()).Contains("module")) nModules++;
    }
    return nModules;
}

void BmnPVAnalyzer::ApplyAlignment() {

    // Apply alignment

    TGeoNode *st_node = NULL;
    // Get STS node
    TGeoNode *sts = NULL;
    gGeoManager->CdTop();
    TGeoNode *cave = gGeoManager->GetCurrentNode();
    for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
        TGeoNode *node = cave->GetDaughter(iNode);
        TString name = node->GetName();
        if (name.Contains("GEMS_0")) {
            sts = node;
            gGeoManager->CdDown(iNode);
            break;
        }
    }
    if (!sts) {
        cout << "-E- CbmBmnStsDigitize::ApplyAlignment: Cannot find top GEM node"
                << endl;
        //return kFALSE;
    }

    // Loop over stations in STS
    Int_t statNr = 0;
    Int_t nStat0 = sts->GetNdaughters();
    Int_t nMod0sts = 0;

    // cout<<"STATIONS : "<<nStat0<<endl;
    Double_t xAlign[] = {0, 0, -0.05, 0, 27.7107, -34.6137, 40.9328, -40.763, 40.8164, -40.8704,
        //9.476,3.483,-2.5296, -8.5457,               9.476,3.519879,-2.468392,-8.549611};
        //         9.476, 3.469,-2.5926,-8.596,                     9.476,3.4695,-2.591,-8.6056}; //for 4 GeV
        9.476, 3.269, -2.7926, -8.796, 9.476, 3.2695, -2.791, -8.7056}; //for 3.5 GeV
    //{9.476,  -8.556,   -8.5656,  9.476,    -2.5526,  3.4695,    -2.551,   3.469};
    //{0, 0, -0.2, 0,       27.94,-34.38,      41.26,-40.44,      41.59,-40.12}; //run5
    //0.24, 0.32,     0.32, 0.50,      0.83, 0.64};//-1.5, -1.70, -1.5,   27.94,-34.38,      -40.44,41.26,      41.59,-40.12};
    Double_t yAlign[] = {0, 0, 0.931983, 0, -0.18801, -0.245647, -0.01268, -0.03827, -0.32845, 0.01928,
        //6.67738,6.70216,6.69713,6.75107,       -6.67738,-6.24577,-6.26909,-6.29003 }; //-6.67738,-6.20394,-6.27195,-6.24655
        6.67738, 6.74426, 6.6588, 6.69745, -6.67738, -6.36139, -6.34705, -6.48174};
    //{0,  0, 0.20, 0 ,     0.19,-0.625,      0.31,0.13,      0.276,0.369};//run5
    //0.19, -0.625,     0.13, 0.31,    0.267, 0.369 };//0,    0.20,   0,    0.19,-0.425,      -2.27,-2.09,      -2.133,-2.031};
    // Double_t zAlign2[] = {0, 0.9, 1.6, 0.9,      1.9, 3.9,      2.2,1.7 ,     2.7, 1.7};
    /*
   // Double_t zAlign[] = {0, 31.8, 63.8, 95.8, 127.8,159.8,191.8};
    Double_t zAlign3[] = {2.47, 32.85,65.35,96.67,130.635,161.47,194.35};// 2.47};
    Double_t zAlign23[] = {0, 0.0, 0.0, 0.0,-1.03,1.3,0.10,-0.10,0.10,-0.25,
  //  Double_t zAlign3[] = {2.47, 32.85,65.25,96.65,130.95,161.45,194.25};// 2.47};
  //  Double_t zAlign23[] = {0, 0.0, 0.0, 0.0,-1.2,1.2,0.10,-0.10,0.10,-0.10,
    //3.1, 1.35, 3.59, 1.84, 2.88, 3.37, 2.06, 1.57};   
    0.63,-0.9,0.41,-1.12,   -0.63,0.9,-0.41,   1.12 };//1.12, -0.41, 0.9, -0.63, -1.12, 0.41,-0.9,0.63};         // 0.63,-0.9, 0.41,-1.12,    -0.63,0.9,-0.41,1.12 }; // 
  //arZ0[MQP]={ 3.1, 1.35, 3.59, 1.84, 2.88, 3.37, 2.06, 1.57};
   // old
 
     */
    //  Double_t zAlign3[] = {2.47, 32.85,65.25,96.65,130.95,161.45,194.25};// 2.47}; // for 4 GeV
    // Double_t zAlign3[] = {2.47, 32.87,65.23,96.67,130.5,161.27,193.83};// 2.47}; // for MC D. Bar
    //Double_t zAlign3[] = {2.47, 32.15,64.65,96.65,130.35,161.25,193.45};// 2.47}; // for MC D. Bar exp value
    //Double_t zAlign3[] = {2.47, 31.87,64.75,96.65,130.5,161.3,193.6};// 2.47}; // for MC D. Bar exp value 2
    //128.65; 131.15
    Double_t zAlign3[] = {2.47, 32.85, 65.35, 96.65, 130.95, 161.45, 194.25}; // 2.47}; //for 3.5 GeV
    //  Double_t zAlign3[] = {2.47, 32.85,65.25,96.65,130.95,161.45,194.25};// 2.47};
    Double_t zAlign23[] = {0, 0.0, 0.0, 0.0, -1.2, 1.2, 0.10, -0.10, 0.10, -0.1, //-1.45,1.05
        0.63, -0.9, 0.41, -1.12, -0.63, 0.9, -0.41, 1.12}; //for 4 GeV

    // Double_t zAlign23[] = {0, 0.0, 0.0, 0.0,    -1.25,1.25,    0.,0.,0.,0., //-1.45,1.05
    //   0.,0.,0.,0,   0,0.,0,   0 }; // for MC D. Bar

    Bool_t MC = false;
    Double_t driftcorr = 0; //-0.2;

    //for (Int_t iNode = 1; iNode < sts->GetNdaughters(); iNode++) {
    for (Int_t iNode = 0; iNode < nStat0; iNode++) {
        // Go to station node
        gGeoManager->CdDown(iNode);
        TGeoNode *stationNode = gGeoManager->GetCurrentNode();
        TString statName = stationNode->GetName();
        if (!statName.Contains("station")) {
            gGeoManager->CdUp();
            continue;
        }
        //AZ Int_t statNr = stationNode->GetNumber();
        ++statNr; //AZ

        TGeoHMatrix *matr = gGeoManager->GetCurrentMatrix();
        Double_t *statTrans = matr->GetTranslation();
        //  statTrans[0] = xAlign[statNr];
        //  statTrans[1] = yAlign[statNr];

        //  cout<<"ST TRANS Z: "<<statTrans[2]<<endl;
        //if(statNr-1 >1)  statTrans[2] = zAlign3[statNr-1]+driftcorr;//zAlign[statNr];
        statTrans[2] = zAlign3[statNr - 1]; //zAlign[statNr];
        if (statNr - 1 > 1)
            statTrans[2] = statTrans[2] + driftcorr;
        //cout<<"ST TRANS Z: "<<statTrans[2]<<endl;
        if (iNode > 0)
            statTrans[2] = statTrans[2] - 2.47;
        matr->SetTranslation(statTrans);
        TGeoHMatrix *matr0 = new TGeoHMatrix(*matr);
        matr0->RegisterYourself();

        //  Int_t nModules = stationNode->GetNdaughters();
        Int_t nModules = GetNofModules(stationNode);
        //  cout<<"nModules: "<<nModules<<endl;

        //sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
        //sts->GetVolume()->RemoveNode(stationNode); //AZ
        sts->GetVolume()->AddNode((TGeoVolumeAssembly *) stationNode->GetVolume(), 0, matr0); //AZ

        //AZ- hard-coded st_node=sts->GetVolume()->GetNode(iNode+6);
        st_node = (TGeoNode *) sts->GetVolume()->GetNodes()->Last(); //AZ

        Double_t statZ = statTrans[2];
        //cout <<"sta: " << statNr << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << " " << sts->GetNdaughters() << endl;

        //gGeoManager->CdUp();               // to sts

        //-----------------------module translate-------------
        Int_t moduleNr = 0, copy_no = 0;
        //cout<<"nMODULES: "<< nModules<<endl;
        if (iNode == 0) {
            nMod0sts = nModules;
            nModules = nModules * 2;
        }
        // ---> Large sensors
        for (Int_t iStatD = 0; iStatD < nModules; iStatD++) {
            gGeoManager->CdDown(iStatD);

            TGeoNode *module = gGeoManager->GetCurrentNode();
            if (!TString(module->GetName()).Contains("module")) {
                gGeoManager->CdUp();
                continue;
            }
            cout << iStatD << " " << module->GetName() << endl;
            if (iNode == 0) {

                Int_t nOfModSi = 10 + (moduleNr++);
                //cout<<"modSI: "<<nOfModSi<<" "<<zAlign23[nOfModSi]<<endl;
                // fprintf(parFile, "%4d %4d\n", moduleNr, 1);
                // ---> Sensor number
                // geoMan->CdUp(); // back to module sensor
                // SaveSensor(geoMan, parFile, phiStat, sensor);
                TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
                Double_t *modTrans = matrMod->GetTranslation();
                //cout<<"trans: "<<modTrans[2]<<endl;
                if (!MC)
                    modTrans[0] = xAlign[nOfModSi];
                else if (nModules > 1)
                    modTrans[0] = modTrans[0];
                else
                    modTrans[0] = 0;
                /*if(!MC) modTrans[1] = yAlign[nOfModSi];
         else modTrans[1] = 0; */
                modTrans[2] = zAlign23[nOfModSi]; //zAlign2[ipNr];
                matrMod->SetTranslation(modTrans);
                TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
                matr0Mod->RegisterYourself();

                //sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
                //sts->GetVolume()->RemoveNode(stationNode); //AZ
                //  stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),0,matr0Mod); //AZ

                //cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName()<< " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
                //Double_t* sensTrans = matrMod->GetTranslation();
                //cout<<"trans: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< " Nr mod:  "<<moduleNr<<endl;
                //stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
                st_node->GetVolume()->AddNode((TGeoVolumeAssembly *) module->GetVolume(), copy_no, matr0Mod);
                //cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName() << " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
                //Double_t  modZ = modTrans[2];
                Double_t *modTrans1 = gGeoManager->GetCurrentMatrix()->GetTranslation();
                //	cout <<"mod: " << nOfModSi << " VEC: " << modTrans1[0] << " "<<modTrans1[1] << " "<<modTrans1[2] << endl;
                copy_no++;
                //delete matr0Mod;
                gGeoManager->CdUp(); // back to module
            }
            if (iNode > 0) {
                if (TString(module->GetName()).Contains("Senso")) {
                    if (iNode > 0 && iNode <= 3)
                        moduleNr = 0;
                    else {
                        if (iStatD == 0)
                            moduleNr = 0;
                        if (iStatD > 0)
                            moduleNr = 1;
                    }
                    //moduleNr=iStatD;
                    //fprintf(parFile, "%4d %4d\n", moduleNr, 1);
                    //SaveSensor(geoMan, parFile, phiStat, module);

                    Int_t ipNr = 0;
                    //cout<<" iNode: "<< iNode<<endl;
                    if (iNode > 0 && iNode <= 3)
                        ipNr = iNode;
                    else
                        ipNr = iNode * 2 - 4 + moduleNr;

                    TGeoHMatrix *matrMod = gGeoManager->GetCurrentMatrix();
                    Double_t *modTrans = matrMod->GetTranslation();
                    if (!MC)
                        modTrans[0] = xAlign[ipNr];
                    else if (nModules > 1)
                        modTrans[0] = modTrans[0];
                    else
                        modTrans[0] = 0;
                    if (!MC)
                        modTrans[1] = yAlign[ipNr];
                    else
                        modTrans[1] = 0;
                    modTrans[2] = zAlign23[ipNr]; //zAlign2[ipNr];
                    //cout<<"ST TRANS Z: "<<zAlign23[ipNr]<<endl;
                    matrMod->SetTranslation(modTrans);
                    TGeoHMatrix *matr0Mod = new TGeoHMatrix(*matrMod);
                    matr0Mod->RegisterYourself();
                    //cout<<" ip N: "<< ipNr<<endl;
                    //sts->GetVolume()->ReplaceNode(stationNode,0,gGeoManager->GetCurrentMatrix()); //AZ
                    //sts->GetVolume()->RemoveNode(stationNode); //AZ
                    //  stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),0,matr0Mod); //AZ

                    //cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName()<< " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
                    //Double_t* sensTrans = matrMod->GetTranslation();
                    //cout<<"trans: "<<sensTrans[0]<<" "<<sensTrans[1]<<" "<<sensTrans[2]<< " Nr mod:  "<<moduleNr<<endl;
                    //stationNode->GetVolume()->AddNode((TGeoVolumeAssembly*)module->GetVolume(),copy_no,matr0Mod);
                    st_node->GetVolume()->AddNode((TGeoVolumeAssembly *) module->GetVolume(), copy_no, matr0Mod);
                    //cout<<" 1 st name add: "<< stationNode->GetName()<<" mod name add: "<<module->GetName() << " "<< module<< " i: "<<iStatD<<" cols: "<<st_node->GetVolume()->GetNdaughters()<<endl;
                    //Double_t  modZ = modTrans[2];
                    //cout <<"mod: " << ipNr << " VEC: " << modTrans[0] << " "<<modTrans[1] << " "<<modTrans[2] << endl;
                    copy_no++;
                    //delete matr0Mod;
                }

                gGeoManager->CdUp(); // back to module
            }
        } // for (Int_t iStatD = 0; iStatD < nModules;
        //----------------------end module translate----------
        //delete matr0;
        gGeoManager->CdUp(); // to sts
    } // station loop

    // Remove extra nodes
    for (Int_t iNode = 0; iNode < nStat0; iNode++) {
        // Go to station node
        //gGeoManager->CdDown(iNode);
        gGeoManager->CdDown(1);
        TGeoNode *stationNode = gGeoManager->GetCurrentNode();
        sts->GetVolume()->RemoveNode(stationNode); //AZ
        gGeoManager->CdUp(); // to sts
    }

    for (Int_t iNode = 0; iNode < nStat0; iNode++) {
        // Go to station node
        //gGeoManager->CdDown(iNode);
        gGeoManager->CdDown(iNode);
        TGeoNode *stationNode = gGeoManager->GetCurrentNode();

        TString statName = stationNode->GetName();
        if (!statName.Contains("station")) {
            gGeoManager->CdUp();
            continue;
        }
        if (iNode > 0) {
            Int_t nMod = 1;
            if (iNode > 3)
                nMod = 2;
            for (Int_t iStatD = 0; iStatD < nMod; iStatD++) {
                gGeoManager->CdDown(0); //stationNode->GetNdaughters()-1);

                TGeoNode *module = gGeoManager->GetCurrentNode();
                //  cout<<" 2 st name del: "<< stationNode->GetName()<<" mod name del: "<<module->GetName() << " i: "<<iStatD<<endl;
                stationNode->GetVolume()->RemoveNode(module); //AZ

                gGeoManager->CdUp(); // to sts
            }
        } else {
            for (Int_t iStatD = 0; iStatD < nMod0sts; iStatD++) {
                gGeoManager->CdDown(iStatD); //stationNode->GetNdaughters()-1);
                TString mName = gGeoManager->GetCurrentNode()->GetName();
                if (!mName.Contains("module")) {
                    gGeoManager->CdUp();
                    continue;
                }
                TGeoNode *moduleS = gGeoManager->GetCurrentNode();
                //   cout<<" DELETE VALUME: "<< stationNode->GetName()<<" mod name del: "<<moduleS->GetName() << " i: "<<iStatD<<endl;
                stationNode->GetVolume()->RemoveNode(moduleS); //AZ

                gGeoManager->CdUp(); // to sts
            }
        }

        gGeoManager->CdUp();
    }

    for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {

        // Go to station node
        //gGeoManager->CdDown(iNode);
        gGeoManager->CdDown(iNode);
        TGeoNode *stationNode = gGeoManager->GetCurrentNode();
        cout << " Check-in after remove STATION: " << stationNode->GetName() << " zpos : " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << endl;
        for (Int_t iModule = 0; iModule < stationNode->GetNdaughters(); iModule++) {
            gGeoManager->CdDown(iModule);
            TGeoNode *moduleNode = gGeoManager->GetCurrentNode();
            cout << " Check-in after remove st name : " << stationNode->GetName() << " mod name : " << moduleNode->GetName() << endl;
            Double_t *sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
            cout << "trans mod: " << sensTrans[0] << " " << sensTrans[1] << " " << sensTrans[2] << endl;
            //  stationNode->GetVolume()->RemoveNode(moduleNode); //AZ
            gGeoManager->CdUp();
        }

        gGeoManager->CdUp(); // to sts
    }

    //exit(0);
}
