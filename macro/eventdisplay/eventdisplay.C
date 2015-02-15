// is_online: true - use Online Mode for EventManager, false - use Offline Mode for EventManager
// data source: 0 - root files with reconstructed and simulation data, 1 - raw files with detector stream data
// input_file - input file name corresponding data source: reconstructed or raw data (begin of file name for raw data files from detector MWPC)
// add_file - second file: if simulation - file with MC data to check reconstruction; if raw - file with geometry to display detectors
// out_file - output file
void eventdisplay (char* input_file = 0, char* add_file = 0, char* out_file = 0, bool is_online = false, int data_source = 0)
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
  TString inputFile = "$VMCWORKDIR/macro/run/bmndst.root";
  if (input_file)
    inputFile = input_file;

  // Define additional file
  TString addFile = "$VMCWORKDIR/macro/run/evetest.root";
  if (add_file)
    addFile = add_file;

  // define parameter file
  TString parFile = addFile;

  // define output file
  TString outFile = "tmp.root";
  if (out_file)
    outFile = out_file;


  // Create FairRunAna
  FairRunAna *fRun = new FairRunAna();

  // set additional file for MC or geometry
  if (CheckFileExist(addFile))
    fRun->SetInputFile(addFile);
  else
    cout<<endl<<"ERROR: Additional file wasn't found!"<<endl;

  if (data_source == 0)
  {
    // add file with reconstruction data as friend
    if (CheckFileExist(inputFile))
        fRun->AddFriend(inputFile);
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
  }

  // set output file
  fRun->SetOutputFile(outFile);


  // Create event manager
  FairEventManager *fMan= new FairEventManager();
  fMan->source_file_name = input_file;

  // set FairTasks for initialization and Run/Draw
  fMan->SetDataSource(is_online, data_source);

  if (gGeoManager)
      gGeoManager->SetVisLevel(3);

  fMan->background_color = 17;

  //FairEventManager::Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
  fMan->Init();
}
