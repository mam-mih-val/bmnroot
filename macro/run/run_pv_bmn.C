// Macro for reconstruction of simulated or experimental events for BM@N
//
// inputFileName - input file with data (MC or exp. data)
// bmndstInFileName - output file with reconstructed data
// nStartEvent - number of first event to process (starts with zero), default: 0
// nEvents - number of events to process, 0 - all events of given file will be processed, default: 1 000 events

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#define CellAuto // Choose Tracking: L1 or CellAuto

void run_pv_bmn(TString inputFileName = "$VMCWORKDIR/macro/run/bmnsim.root",
        TString bmndstInFileName = "$VMCWORKDIR/macro/run/bmndst.root",
        TString bmndstOutFileName = "$VMCWORKDIR/macro/run/bmn-pv.root",
        Int_t nStartEvent = 0,
        Int_t nEvents = 1000) {
    gDebug = 0; // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 2;

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // check input file exists
    if (!BmnFunctionSet::CheckFileExist(inputFileName)) {
        cout << "ERROR: input file " + inputFileName + " does not exist!" << endl;
        exit(-1);
    }
    if (!BmnFunctionSet::CheckFileExist(bmndstInFileName)) {
        cout << "ERROR: input file " + bmndstInFileName + " does not exist!" << endl;
        exit(-1);
    }

    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();

    Bool_t isField = (inputFileName.Contains("noField")) ? kFALSE : kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE; //kTRUE; // flag for tracking (run with target or not)
    Bool_t isExp = !BmnFunctionSet::isSimulationFile(inputFileName); // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;

    // -1 means use of the BM@N-setup when processing MC-input
    // DO NOT change it manually!
    Int_t run_period = 7, run_number = -1;
    Double_t fieldScale = 0.;
    //    if (!isExp) // for simulation files
    //        fFileSource = new FairFileSource(inputFileName);
    //    else        // for experimental files
    //    {
    //        // set source as raw root data file
    fFileSource = new BmnFileSource(inputFileName, run_period, run_number);

    // get geometry for run
    gRandom->SetSeed(0);
    TString geoFileName = Form("current_geo_file_%d.root", UInt_t(gRandom->Integer(UINT32_MAX)));
    Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*) geoFileName.Data());
    if (res_code != 0) {
        cout << "ERROR: could not read geometry file from the database" << endl;
        exit(-2);
    }

    // get gGeoManager from ROOT file (if required)
    TFile* geoFile = new TFile(geoFileName, "READ");
    if (!geoFile->IsOpen()) {
        cout << "ERROR: could not open ROOT file with geometry: " + geoFileName << endl;
        exit(-3);
    }
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*) next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        cout << "ERROR: TGeoManager is not top element in geometry file " + geoFileName << endl;
        exit(-4);
    }

    // set magnet field with factor corresponding to the given run
    UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
    if (pCurrentRun == 0)
        exit(-5);
    Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
    if (field_voltage == NULL) {
        cout << "ERROR: no field voltage was found for run " << run_period << ":" << run_number << endl;
        exit(-6);
    }
    Double_t map_current = 55.87;
    if (*field_voltage < 10) {
        fieldScale = 0;
        isField = kFALSE;
    } else
        fieldScale = (*field_voltage) / map_current;

    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    magField->SetScale(fieldScale);
    magField->Init();
    fRunAna->SetField(magField);
    isExp = kTRUE;
    TString targ;
    if (pCurrentRun->GetTargetParticle() == NULL) {
        targ = "-";
        isTarget = kFALSE;
    } else {
        targ = (pCurrentRun->GetTargetParticle())[0];
        isTarget = kTRUE;
    }
    TString beam = pCurrentRun->GetBeamParticle();
    cout << "\n\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << endl;
    cout << "||\t\t\t\t\t\t\t||" << endl;
    cout << "||\t\tPeriod:\t\t" << run_period << "\t\t\t||" << endl;
    cout << "||\t\tNumber:\t\t" << run_number << "\t\t\t||" << endl;
    cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
    cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
    cout << "||\t\tField scale:\t" << /*setprecision(4) <<*/ fieldScale << "\t\t\t||" << endl;
    cout << "||\t\t\t\t\t\t\t||" << endl;
    cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
    remove(geoFileName.Data());
    //    }
    //    fRunAna->SetSource(fFileSource);
    fRunAna->SetInputFile(bmndstInFileName);
    fRunAna->SetSink(new FairRootFileSink(bmndstOutFileName));
    //    fRunAna->SetGenerateRunInfo(true);

    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*) bmndstInFileName.Data()) - nStartEvent;

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the FairParAsciiFileIo.
    // The FairParAsciiFileIo will create on fly a concatenated input parameter file, which is then used during the reconstruction.
    //    TList* parFileNameList = new TList();
    //    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v1_BMN_SI_GEM.digi.par";
    //    parFileNameList->Add(&stsDigiFile);

    // ====================================================================== //
    // ===          Alternative Primary vertex finding                    === //
    // ====================================================================== //
    CbmKF* kalman = new CbmKF("q", iVerbose);
    fRunAna->AddTask(kalman);
    BmnPVAnalyzer* pv = new BmnPVAnalyzer(run_period, run_number, isField);
    fRunAna->AddTask(pv);


    //    // Fill DST Event Header (if iVerbose = 0, then print progress bar)
    //    BmnFillDstTask* dst_task = new BmnFillDstTask(nEvents);
    //    dst_task->SetRunNumber(run_period, run_number);
    //    fRunAna->AddTask(dst_task);

    // -----   Parameter database   --------------------------------------------
    //    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    //    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    //    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    //    parIo1->open(inputFileName.Data());
    //    parIo2->open(parFileNameList, "in");
    //    rtdb->setFirstInput(parIo1);
    //    rtdb->setSecondInput(parIo2);
    //    rtdb->setOutput(parIo1);
    //    rtdb->saveOutput();
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl; // marker of successful execution for CDASH
    cout << "Input prop file is " + inputFileName << endl;
    cout << "Input  dst file is " + bmndstInFileName << endl;
    cout << "Output dst file is " + bmndstOutFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
