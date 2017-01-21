// --------------------------------------------------------------------------
// Macro for reconstruction of simulated events
//
// inFile - input file with MC data, default: evetest.root. To process experimental data, you can
// use 'runNp-Nrun:raw_file_path' prefix, e.g. "run5-458:../digits_run5/bmn_run0458_digi.root" then the geometry is obtained from the Unified Database.
// outFile - output file with reconstructed data, default: mpddst.root
// nStartEvent - number (start with zero) of first event to process, default: 0
// nEvents - number of events to process, 0 - all events of given file will be proccessed, default: 1

void run_reco_bmn(TString inFile = "$VMCWORKDIR/macro/run/evetest.root", TString outFile = "$VMCWORKDIR/macro/run/bmndst.root",
        Int_t nStartEvent = 0, Int_t nEvents = 10000, Bool_t isPrimary = kTRUE) {
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

    // -----   Reconstruction run   -------------------------------------------
    FairRunAna *fRun = new FairRunAna();

    Bool_t isField = kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE; // flag for tracking (run with target or not)
    Bool_t isExp = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Set input source as simulation file or experimental data
    FairSource* fFileSource;
    Ssiz_t indColon = inFile.First(':');
    Ssiz_t indDash = inFile.First('-');
    // for experimental datasource
    if ((indColon >= 0) && (indDash < indColon) && (inFile.BeginsWith("run"))) {
        // get run period
        TString number_string(inFile(3, indDash - 3));
        Int_t run_period = number_string.Atoi();
        // get run number
        number_string = inFile(indDash + 1, indColon - indDash - 1);
        Int_t run_number = number_string.Atoi();
        inFile.Remove(0, indColon + 1);

        // set source as raw data file
        if (!CheckFileExist(inFile)) return;
        fFileSource = new BmnFileSource(inFile);

        // get geometry for run
        TString root_file_path = "current_geo_file.root";
        Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, root_file_path.Data());
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

        // set magnet field with factor corresponding the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0) {
            exit(-2);
        }
        Double_t fieldScale = 0;
        double map_current = 900.0;
        int* current_current = pCurrentRun->GetFieldCurrent();
        if (*current_current == 0) {
            fieldScale = 0;
            isField = kFALSE;
        } else
            fieldScale = (*current_current) / map_current;
        BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v3_ascii_Extrap.dat");
        magField->SetScale(fieldScale);
        magField->Init();
        fRun->SetField(magField);
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
    }// for simulated files
    else {
        if (!CheckFileExist(inFile)) return;
        fFileSource = new FairFileSource(inFile);
    }

    fRun->SetSource(fFileSource);
    fRun->SetOutputFile(outFile);
    fRun->SetGenerateRunInfo(false);

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

    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //

    //    BmnMwpcHitProducer* mwpcHP1 = new BmnMwpcHitProducer(1);
    //    fRun->AddTask(mwpcHP1);
    //    BmnMwpcHitProducer* mwpcHP2 = new BmnMwpcHitProducer(2);
    //    fRun->AddTask(mwpcHP2);
    //    BmnMwpcHitProducer* mwpcHP3 = new BmnMwpcHitProducer(3);
    //    fRun->AddTask(mwpcHP3);

    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    
    //BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSummer2016;      // RunSummer2016 config (GEM_RunSummer2016.root))
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunWinter2016; // RunWinter2016 config (GEM_RunWinter2016.root))
    
    if (!isExp) {
        BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
        gemDigit->SetCurrentConfig(gem_config);
        gemDigit->SetOnlyPrimary(isPrimary);
        gemDigit->SetStripMatching(kTRUE);
        fRun->AddTask(gemDigit);
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(isExp);
    gemHM->SetCurrentConfig(gem_config);
    gemHM->SetAlignmentCorrections("alignCorrs_18ene.root");
    gemHM->SetHitMatching(kTRUE);
    fRun->AddTask(gemHM);

    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //

    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", kFALSE, 1, kTRUE);
    //tof1HP->SetOnlyPrimary(kTRUE);
    //    fRun->AddTask(tof1HP);

    // ====================================================================== //
    // ===                           TOF2 hit finder                      === //
    // ====================================================================== //

    CbmTofHitProducer* tof2HP = new CbmTofHitProducer("TOF HitProducer", iVerbose);
    tof2HP->SetZposition(700.);
    tof2HP->SetXshift(32.);
    //    fRun->AddTask(tof2HP);

    // ====================================================================== //
    // ===                           Tracking (GEM)                       === //
    // ====================================================================== //

    BmnGemSeedFinder* gemSF = new BmnGemSeedFinder();
    gemSF->SetUseLorentz(kTRUE);
    gemSF->SetField(isField);
    gemSF->SetTarget(isTarget);
    gemSF->SetXRange(0.0, 100.0);
    gemSF->SetYRange(-3.0, 3.0);
    gemSF->AddStationToSkip(0);
    gemSF->AddStationToSkip(1);
    gemSF->AddStationToSkip(2);
    fRun->AddTask(gemSF);

//    BmnGemTrackFinder* gemTF = new BmnGemTrackFinder();
//    gemTF->SetField(isField);
//    fRun->AddTask(gemTF);

    //    BmnGlobalTracking* glFinder = new BmnGlobalTracking();
    //    glFinder->SetRun1(kRUN1);
    //    fRun->AddTask(glFinder);

    // -----   Primary vertex finding   ---------------------------------------
//    BmnGemVertexFinder* vf = new BmnGemVertexFinder();
//    vf->SetField(isField);
//    fRun->AddTask(vf);
    // ------------------------------------------------------------------------

    // ====================================================================== //
    // ===                           Tracking (DCH)                       === //
    // ====================================================================== //
    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    fRun->AddTask(dchTF);
    
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
