// raw_file_start - begin of file name (full path) with raw data from detector (MWPC), e.g. "2014-08-21_12-10-24_hrb046f"
void online_eventdisplay(char* file_name_begin = "/home/soul/bmnroot/macro/eventdisplay/2014-12-29_18-06-03_hrb046f")
{
  // load main and detectors libraries
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs();

  // load TEve libraries
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");


  // Define simulation file
  TString simFile = "$VMCWORKDIR/macro/run/evetest.root";

  // define output file
  TString outFile = "tmp.root";


  // Create FairRunAna
  FairRunAna *fRun = new FairRunAna();

  // set input file
  if (CheckFileExist(simFile))
    fRun->SetInputFile(simFile);
  else
    cout<<endl<<"ERROR: Input file wasn't found!"<<endl;


  // set output file
  fRun->SetOutputFile(outFile);


  // Create event manager
  OnlineEventManager* fMan= new OnlineEventManager();
  fMan->file_name_begin = file_name_begin;

  if (gGeoManager)
      gGeoManager->SetVisLevel(3);

  fMan->background_color = 17;

  //OnlineEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
  fMan->Init();
}
