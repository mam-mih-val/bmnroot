// Macro for reconstruction of simulated or experimental events for SRC
//
// inputFileName - input file with data (MC or exp. data).
//
// To process experimental data, you must use 'run[#period]-[#run]:'-like prefix,
// and then the geometry will be obtained from the Unified Database
// e.g. "run[#period]-[#run]:PATH_TO_DIGI_DATA/digiFile.root"
//
// bmndstFileName - output file with reconstructed data
// nStartEvent - number of first event to process (starts with zero), default: 0
// nEvents - number of events to process, 0 - all events of given file will be processed
R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__LOAD_LIBRARY(libBmnAlignment.so)
R__LOAD_LIBRARY(libSilicon.so)
void run_reco_src(TString inputFileName = "$VMCWORKDIR/macro/run/evetest.root", TString srcdstFileName = "$VMCWORKDIR/macro/run/srcdst.root", Int_t nStartEvent = 0, Int_t nEvents = 10) {
    // Verbosity level (0=quiet, 1=event-level, 2=track-level, 3=debug)
    Int_t iVerbose = 0;

    // ----    Debug option   --------------------------------------------------
    gDebug = 0;

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();
    fRunAna->SetEventHeader(new DstEventHeader());

    Bool_t isField = (inputFileName.Contains("noField")) ? kFALSE : kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE; //kTRUE; // flag for tracking (run with target or not)
    Bool_t isExp = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;

    // -2 means use of the SRC-setup when processing MC-input
    // DO NOT change it manually!
    Int_t run_period = 7, run_number = -2;
    Double_t fieldScale = 0.;
    TPRegexp run_prefix("^run[0-9]+-[0-9]+:");
    if (inputFileName.Contains(run_prefix)) {
        Ssiz_t indDash = inputFileName.First('-'), indColon = inputFileName.First(':');
        // get run period
        run_period = TString(inputFileName(3, indDash - 3)).Atoi();
        // get run number
        run_number = TString(inputFileName(indDash + 1, indColon - indDash - 1)).Atoi();
        inputFileName.Remove(0, indColon + 1);

        if (!BmnFunctionSet::CheckFileExist(inputFileName)) {
            cout << "Error: digi file " + inputFileName + " does not exist!" << endl;
            exit(-1);
        }
        // set source as raw root data file (without additional directories)
        fFileSource = new BmnFileSource(inputFileName);

        // get geometry for run
        TString geoFileName = "full_geometry.root";
        // Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*) geoFileName.Data());
        // if (res_code != 0) {
        //     cout << "Error: could not read geometry file from the database" << endl;
        //     exit(-2);
        // }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
            cout << "Error: could not open ROOT file with geometry: " + geoFileName << endl;
            exit(-3);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*) next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout << "Error: TGeoManager is not top element in geometry file " + geoFileName << endl;
            exit(-4);
        }

        // set magnet field with factor corresponding to the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0)
            exit(-5);
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (field_voltage == NULL) {
            cout << "Error: no field voltage was found for run " << run_period << ":" << run_number << endl;
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
        cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
        cout << "||\t\t\t\t\t\t\t||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
    } else { // for simulated files
        if (!BmnFunctionSet::CheckFileExist(inputFileName)) return;
        fFileSource = new FairFileSource(inputFileName);
    }
    fRunAna->SetSource(fFileSource);
    fRunAna->SetSink(new FairRootFileSink(srcdstFileName));
    fRunAna->SetGenerateRunInfo(false);

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the
    // FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on fly a
    // concatenated input parameter file, which is then used during the
    // reconstruction.
    TList* parFileNameList = new TList();

    TObjString tofDigiFile = "$VMCWORKDIR/parameters/tof_standard.geom.par";
    parFileNameList->Add(&tofDigiFile);

    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
     BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp, run_period, run_number);
     fRunAna->AddTask(mwpcHM);

    // ====================================================================== //
    // ===                         Silicon hit finder                     === //
    // ====================================================================== //
    BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(run_period, run_number, isExp);
    if (!isExp)
        siliconHM->SetCurrentConfig(BmnSiliconConfiguration::RunSRCSpring2018); //set explicitly
    fRunAna->AddTask(siliconHM);
    
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(run_period, run_number, isExp);
    if (!isExp)
        gemHM->SetCurrentConfig(BmnGemStripConfiguration::RunSRCSpring2018); //set explicitly
    gemHM->SetHitMatching(kTRUE);
    gemHM->SetSrcSetup(kTRUE);
    fRunAna->AddTask(gemHM);

    // ====================================================================== //
    // ===                          CSC hit finder                        === //
    // ====================================================================== //
    BmnCSCHitMaker* cscHM = new BmnCSCHitMaker(run_period, run_number, isExp);
    if (!isExp)
        cscHM->SetCurrentConfig(BmnCSCConfiguration::RunSRCSpring2018); //set explicitly
    cscHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(cscHM);

    // ====================================================================== //
    // ===                         TOF400 hit finder                      === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kTRUE);
    tof1HP->SetPeriodRun(run_period, run_number);
    fRunAna->AddTask(tof1HP);

    // ====================================================================== //
    // ===                         TOF700 hit finder                      === //
    // ====================================================================== //
    BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run7.txt", !isExp, iVerbose, kTRUE);
    tof2HP->SetTimeResolution(0.115);
    tof2HP->SetMCTimeFile("TOF700_MC_src_qgsm_time_run7.txt");
    tof2HP->SetMainStripSelection(0); // 0 - minimal time, 1 - maximal amplitude
    tof2HP->SetSelectXYCalibration(2); // 2 - Petukhov, 1 - Panin
    tof2HP->SetTimeMin(-2.f); // minimal digit time
    tof2HP->SetTimeMax(+15.f); // Maximal digit time
    tof2HP->SetDiffTimeMaxSmall(1.3f); // Abs maximal difference for small chambers
    tof2HP->SetDiffTimeMaxBig(3.5f); // Abs maximal difference for big chambers
    fRunAna->AddTask(tof2HP);

    // ====================================================================== //
    // ===                           LAND hit finder                      === //
    // ====================================================================== //
    // BmnLANDHitProducer* land = new BmnLANDHitProducer("LAND", !isExp, iVerbose, kTRUE);
    // fRunAna->AddTask(land);

    // ====================================================================== //
    // ===                          Tracking (MWPC)                       === //
    // ====================================================================== //
     BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp, run_period, run_number);
     fRunAna->AddTask(mwpcTF);

    // ====================================================================== //
    // ===                          Tracking (Silicon)                    === //
    // ====================================================================== //
     BmnSiliconTrackFinder* siTF = new BmnSiliconTrackFinder(isTarget, run_number);
     fRunAna->AddTask(siTF);

     // ====================================================================== //
     // ===                        Tracking (Upstream magnet)              === //
     // ====================================================================== //
     BmnUpstreamTracking* upTF = new BmnUpstreamTracking(run_number);
     fRunAna->AddTask(upTF);

    // ====================================================================== //
    // ===                   Tracking (GEM in magnet)                     === //
    // ====================================================================== //

    BmnCellAutoTracking* tf = new BmnCellAutoTracking(run_period, run_number, isField, isTarget);
    tf->SetDetectorPresence(kSILICON, kFALSE); //we have separated task for silicon tracking in SRC
    fRunAna->AddTask(tf);

    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    dchTF->SetTransferFunction("transfer_func2932.txt");
    fRunAna->AddTask(dchTF);

    // Residual analysis
    BmnResiduals* res = new BmnResiduals(run_period, run_number, isField);
    fRunAna->AddTask(res);

    BmnGlobalTracking* glTF = new BmnGlobalTracking(isField, isExp, kFALSE);
    glTF->SetSrcSetup(kTRUE);
    fRunAna->AddTask(glTF);

    // Fill DST Event Header (if iVerbose = 0, then print progress bar)
    BmnFillDstTask* dst_task = new BmnFillDstTask(nEvents);
    dst_task->SetRunNumber(run_period, run_number);
    fRunAna->AddTask(dst_task);

    BmnPidSRC* pid = new BmnPidSRC();
    fRunAna->AddTask(pid);

    // -----   Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(inputFileName.Data());
    parIo2->open(parFileNameList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
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
    cout << "Input  file is " + inputFileName << endl;
    cout << "Output file is " + srcdstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
