// --------------------------------------------------------------------------
// Macro for reconstruction of simulated events
//
// inFile - input file with MC data, default: evetest.root. To process experimental data, you can use 'runNNN:' prefix.  The geometry is obtained from the Unified Database.
// outFile - output file with reconstructed data, default: mpddst.root
// nStartEvent - number (start with zero) of first event to process, default: 0
// nEvents - number of events to process, 0 - all events of given file will be proccessed, default: 1
void run_reco_TOF1(TString inFile = "run689:$VMCWORKDIR/macro/run/tof1digit_bmn_run0084_raw.root", TString outFile = "$VMCWORKDIR/macro/run/bmndstDigit.root", Int_t nStartEvent = 0, Int_t nEvents = 1000, Bool_t isPrimary = kTRUE, Bool_t gemCF = kTRUE) 
//void run_reco_TOF1(TString inFile = "$VMCWORKDIR/macro/run/evetest.root", TString outFile = "$VMCWORKDIR/macro/run/bmndst.root", Int_t nStartEvent = 0, Int_t nEvents = 1, Bool_t isPrimary = kTRUE, Bool_t gemCF = kTRUE) 
{
    // ========================================================================
    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;

    // ----    Debug option   -------------------------------------------------
    gDebug = 0;
    // ------------------------------------------------------------------------

    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // ------------------------------------------------------------------------

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // Set input source as simulation file or experimental data (if 'runN:' prefix)
    FairSource* fFileSource;
    Ssiz_t indColon = inFile.First(':');
    // for experimental datasource
    if ((indColon >= 0) && (inFile.BeginsWith("run"))) {
        // get run number
        TString number_string(inFile(3, indColon - 3));
        Int_t run_number = number_string.Atoi();
        inFile.Remove(0, indColon + 1);

        if (!CheckFileExist(inFile)) return;
        fFileSource = new BmnFileSource(inFile);

        // get geometry for run
        TString root_file_path = "current_geo_file.root";
        Int_t res_code = UniRun::ReadGeometryFile(run_number, root_file_path.Data());
        if (res_code != 0) {
            cout << "\nGeometry file can't be read from the database" << endl;
            exit(-1);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(root_file_path, "READ"); 
        if (!geoFile->IsOpen()) {
            cout << "Error: could not open ROOT file with geometry!" << endl;
            exit(-2);
        }

        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*) next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout << "Error: TGeoManager isn't top element in geometry file " << root_file_path << endl;
            exit(-3);
        }
    }// for simulated files
    else {
        if (!CheckFileExist(inFile)) return;
        fFileSource = new FairFileSource(inFile);
    }

    // Parameter file
    TString parFile = inFile;

    //  Digitisation files.
    // Add TObjectString containing the different file names to
    // a TList which is passed as input to the FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on the fly
    // a concatenated input parameter file which is then used during
    // the reconstruction.
    TList *parFileList = new TList();
    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v15a_gem.digi.par";
    parFileList->Add(&stsDigiFile);

    TObjString tofDigiFile = "$VMCWORKDIR/parameters/tof_standard.geom.par";
    parFileList->Add(&tofDigiFile);

    // -----   Reconstruction run   -------------------------------------------
    FairRunAna *fRun = new FairRunAna();
    fRun->SetSource(fFileSource);
    fRun->SetOutputFile(outFile);
    fRun->SetGenerateRunInfo(false);
    // ------------------------------------------------------------------------


    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //
	// BmnTof1HitProducerIdeal(const char *name = "TOF1 Ideal HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false, Bool_t DoMergeHits = false);
	// BmnTof1HitProducer(const char *name = "TOF1 HitProducer", Bool_t useMCdata = true, Int_t verbose = 1, Bool_t DoTest = false);
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("Tof1HitProducer", false, 1, true);
    //tof1HP->SetOnlyPrimary(kTRUE);
    fRun->AddTask(tof1HP);


    // -----  Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFile.Data());
    parIo2->open(parFileList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // ------------------------------------------------------------------------

    // -----   Initialize and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------

    // -----   Finish   -------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Output file is " << outFile << endl;
    cout << "Parameter file is " << parFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
