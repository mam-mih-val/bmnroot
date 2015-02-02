// sim_file - file with simulation data, dst_file - file with DST data, out_file - output file
void eventdisplay (char* sim_file = 0, char* dst_file = 0, char* out_file = 0)
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


  // Define simulation file
  TString simFile = "$VMCWORKDIR/macro/run/evetest.root";
  if (sim_file)
    simFile = sim_file;

  // define parameter file
  TString parFile = simFile;

  // define reconstructed file
  TString dstFile = "$VMCWORKDIR/macro/run/bmndst.root";
  if (dst_file)
    dstFile = dst_file;

  // define output file
  TString outFile = "tmp.root";
  if (out_file)
    outFile = out_file;


  // Create FairRunAna
  FairRunAna *fRun = new FairRunAna();

  // set input file
  if (CheckFileExist(simFile))
    fRun->SetInputFile(simFile);
  else
    cout<<endl<<"ERROR: Input file wasn't found!"<<endl;

  // add friend file with reconstruction data
  if (CheckFileExist(dstFile))
    fRun->AddFriend(dstFile);
  else
    cout<<endl<<"Warning: File with reconstructed data wasn't found!"<<endl;

  // set parameter file
  if (CheckFileExist(parFile))
  {
    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo *parIo1 = new FairParRootFileIo();
    parIo1->open(parFile.Data());
    rtdb->setFirstInput(parIo1);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
  }
  else
    cout<<endl<<"ERROR: Parameter file wasn't found!"<<endl;

  // set output file
  fRun->SetOutputFile(outFile);


  // Create event manager
  FairEventManager *fMan= new FairEventManager();


  Style_t pointMarker = kFullDotSmall;
  Color_t pointColor = kRed;

  // draw MC points
  FairMCPointDraw *TofPoint = new FairMCPointDraw("TofPoint", pointColor, pointMarker);
  fMan->AddTask(TofPoint);
  FairMCPointDraw *PsdPoint = new FairMCPointDraw("PsdPoint", pointColor, pointMarker);
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
  FairHitPointSetDraw *BmnGemHit = new FairHitPointSetDraw("BmnGemHit", kBlack, pointMarker);
  fMan->AddTask(BmnGemHit);
  FairHitPointSetDraw *TOF1Hit = new FairHitPointSetDraw("TOF1Hit", kBlack, pointMarker);
  fMan->AddTask(TOF1Hit);
  FairHitPointSetDraw *BmnDch1Hit = new FairHitPointSetDraw("BmnDch1Hit", kBlack, pointMarker);
  fMan->AddTask(BmnDch1Hit);
  FairHitPointSetDraw *BmnDch2Hit = new FairHitPointSetDraw("BmnDch2Hit", kBlack, pointMarker);
  fMan->AddTask(BmnDch2Hit);
  FairHitPointSetDraw *BmnTof2Hit = new FairHitPointSetDraw("BmnTof2Hit", kBlack, pointMarker);
  fMan->AddTask(BmnTof2Hit);

  // DST hits (box view)
  //FairHitDraw *MpdTpcHit = new FairHitDraw("TpcHit", 1);
  //fMan->AddTask(MpdTpcHit);

  // DST tracks
  //MpdTrackDraw *MpdGlobalTrack = new MpdTrackDraw("GlobalTracks");
  //fMan->AddTask(MpdGlobalTrack);

  if (gGeoManager)
      gGeoManager->SetVisLevel(3);

  fMan->background_color = 17;

  //FairEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
  fMan->Init();
}
