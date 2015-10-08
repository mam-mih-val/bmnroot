// is_online: true - use Online Mode for EventManager (multithreads), false - use Offline Mode for EventManager (fair tasks)
// data source: 0 - root files with simulation and reconstructed data, 1 - raw file with detector STREAM data (+geometry), 2 - root files with experimental data (+geometry)
// exp_reco_file - file name with data source: experimental (raw or root) or reconstructed data
// sim_geo_file - file with detector geometry and MC data (if simulation)
// out_file - output file
void eventdisplay(char* exp_reco_file = "$VMCWORKDIR/macro/run/bmndst.root", char* geo_sim_file = "$VMCWORKDIR/macro/run/evetest.root", char* out_file = "tmp.root", bool is_online = false, int data_source = 0)
{
  TStopwatch timer;
  timer.Start();
  gDebug = 0;

  // load main and detectors libraries
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs();

  // load TEve libraries
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");


  // define input file
  TString ExpRecoFile = exp_reco_file;
  // define geometry file
  TString GeoSimFile = geo_sim_file;
  // define output file
  TString outFile = out_file;


  // Create FairRunAna
  FairRunAna* fRun = new FairRunAna();

  // Create event manager
  FairEventManager* fMan= new FairEventManager();
  fMan->isOnline = is_online;
  fMan->fDataSource = data_source;


  // simulated and reconstructed data for simulation
  if (data_source == 0)
  {
    if (CheckFileExist(GeoSimFile))
        fRun->SetInputFile(GeoSimFile);
    else
    {
        cout<<endl<<"ERROR: Simulation file with detector geometry wasn't found!"<<endl;
        return;
    }

    // set parameter file with simulation data and detector geometry
    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo *parIo1 = new FairParRootFileIo();
    parIo1->open(GeoSimFile.Data());
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();

    // add file with reconstruction data as friend
    if (CheckFileExist(ExpRecoFile))
        fRun->AddFriend(ExpRecoFile);
    else
        cout<<endl<<"Warning: File with reconstructed data wasn't found!"<<endl;
  }
  // experimental data
  else
  {
    // add file with detector geometry
    if (!CheckFileExist(GeoSimFile))
    {
        cout<<endl<<"ERROR: File with detector geometry wasn't found!"<<endl;
        return;
    }
    else
        fRun->SetInputFile(GeoSimFile);

    fMan->source_file_name = ExpRecoFile;
    fMan->geo_file_name = GeoSimFile;
  }

  // set output file
  fRun->SetOutputFile(outFile);

  // set FairTasks for initialization and Run/Draw
  SetDataSource(fMan, is_online, data_source);


  // visualization parameters
  if (gGeoManager)
      gGeoManager->SetVisLevel(3);

  fMan->background_color = 17;
  fMan->isDarkColor = false;

  //FairEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
  fMan->Init();
}


// set FairRunAna tasks depending from data source and on/offline mode
class FairEventManager;
void SetDataSource(FairEventManager* fMan, bool is_online, int data_source)
{
    // root files with simulation and reconstructed data
    if (data_source == 0)
    {
        Style_t pointMarker = kFullDotSmall;
        Color_t pointColor = kRed;

        // draw MC points
        FairMCPointDraw *TofPoint = new FairMCPointDraw("TofPoint", pointColor, pointMarker);
        fMan->AddTask(TofPoint);
        FairMCModuleDraw *PsdPoint = new FairMCModuleDraw("PsdPoint", pointColor, pointMarker);
        fMan->AddTask(PsdPoint);
        FairMCPointDraw *StsPoint = new FairMCPointDraw("StsPoint", pointColor, pointMarker);
        fMan->AddTask(StsPoint);
        FairMCPointDraw *RecoilPoint = new FairMCPointDraw("RecoilPoint", pointColor, pointMarker);
        fMan->AddTask(RecoilPoint);
        FairMCPointDraw *TOF1Point = new FairMCPointDraw("TOF1Point", pointColor, pointMarker);
        fMan->AddTask(TOF1Point);
        FairMCPointDraw *DCH1Point = new FairMCPointDraw("DCH1Point", pointColor, pointMarker);
        fMan->AddTask(DCH1Point);
        FairMCPointDraw *DCH2Point = new FairMCPointDraw("DCH2Point", pointColor, pointMarker);
        fMan->AddTask(DCH2Point);
        FairMCPointDraw *MWPC1Point = new FairMCPointDraw("MWPC1Point", pointColor, pointMarker);
        fMan->AddTask(MWPC1Point);
        FairMCPointDraw *MWPC2Point = new FairMCPointDraw("MWPC2Point", pointColor, pointMarker);
        fMan->AddTask(MWPC2Point);
        FairMCPointDraw *MWPC3Point = new FairMCPointDraw("MWPC3Point", pointColor, pointMarker);
        fMan->AddTask(MWPC3Point);

        // draw MC geometry tracks
        FairMCTracks* GeoTrack = new FairMCTracks("GeoTracks");
        fMan->AddTask(GeoTrack);

        // draw MC tracks
        //FairMCStack* MCTrack = new FairMCStack("MCTrack");
        //fMan->AddTask(MCTrack);

        // DST hits
        FairHitPointSetDraw *BmnGemHit = new FairHitPointSetDraw("BmnGemStripHit", kBlack, pointMarker);
        fMan->AddTask(BmnGemHit);
        FairHitPointSetDraw *TOF1Hit = new FairHitPointSetDraw("TOF1Hit", kBlack, pointMarker);
        fMan->AddTask(TOF1Hit);
        FairHitPointSetDraw *BmnDch1Hit = new FairHitPointSetDraw("BmnDch1Hit0", kBlack, pointMarker);
        fMan->AddTask(BmnDch1Hit);
        FairHitPointSetDraw *BmnDch2Hit = new FairHitPointSetDraw("BmnDch2Hit0", kBlack, pointMarker);
        fMan->AddTask(BmnDch2Hit);
        FairHitPointSetDraw *BmnTof2Hit = new FairHitPointSetDraw("BmnTof2Hit", kBlack, pointMarker);
        fMan->AddTask(BmnTof2Hit);

        // DST hits (box view)
        //FairHitDraw *MpdTpcHit = new FairHitDraw("TpcHit", 1);
        //fMan->AddTask(MpdTpcHit);

        // DST tracks from simulation data
        BmnTrackDraw* BmnGlobalTrack = new BmnTrackDraw("GlobalTrack");
        fMan->AddTask(BmnGlobalTrack);

        return;
    }

    // raw files with detector stream data
    if ((data_source == 1) && (!is_online))
    {
        Style_t pointMarker = kFullDotSmall;
        Color_t pointColor = kRed;

        // draw MWPC Digits
        RawMWPCDigitDraw* MWPCDigit = new RawMWPCDigitDraw("MWPCDigit", pointColor, pointMarker);
        MWPCDigit->source_file_name = fMan->source_file_name;
        fMan->AddTask(MWPCDigit);
    }

    // root files with digits from raw format
    if (data_source == 2)
    {
        Style_t pointMarker = kFullDotSmall;
        Color_t pointColor = kRed;

        // draw MWPC digits
        //BmnDigitDraw* MwpcDigit = new BmnDigitDraw("bmn_mwpc_digit", 1, pointColor, pointMarker);
        //fMan->AddTask(MwpcDigit);

        // draw DCH digits
        //BmnDigitDraw* DchDigit = new BmnDigitDraw("bmn_dch_digit", 2, pointColor, pointMarker);
        //fMan->AddTask(DchDigit);

        // draw MWPC hits
        BmnHitDraw* MwpcHit = new BmnHitDraw("BmnMwpcHit", pointColor, pointMarker);
        fMan->AddTask(MwpcHit);

        // draw DCH hits
        BmnHitDraw* DchHit = new BmnHitDraw("BmnDchHit", pointColor, pointMarker);
        fMan->AddTask(DchHit);

        // draw MWPC tracks
        BmnExpTrackDraw* MwpcTrack = new BmnExpTrackDraw("MwpcMatchedTracks", "BmnMwpcHit");
        fMan->AddTask(MwpcTrack);
        
        // draw DCH tracks
        BmnExpTrackDraw* DchTrack = new BmnExpTrackDraw("DchTracks", "BmnDchHit");
        fMan->AddTask(DchTrack);
    }
}
