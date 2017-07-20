// -----------------------------------------------------------------------------
// Macro for reconstruction of simulated or experimental events.
//
// inputFileName - input file with data.
//
// To process experimental data, you must use 'runN-NNN:'-like prefix
// and then the geometry will be obtained from the Unified Database.
//
// bmndstFileName - output file with reconstructed data.
//
// nStartEvent - number of first event to process (starts with zero), default: 0.
//
// nEvents - number of events to process, 0 - all events of given file will be
// processed, default: 10000.
//
// isPrimary - flag needed when working with MC events, default: kTRUE.
//
// alignCorrFileName - argument for choosing input file with the alignment
// corrections.
//
// If alignCorrFileName == 'default', (case insensitive) then corrections are
// retrieved from UniDb according to the running period and run number.
//
// If alignCorrFileName == '', then no corrections are applied at all.
//
// If alignCorrFileName == '<path>/<file-name>', then the corrections are taken
// from that file.

void run_reco_bmn(TString inputFileName     = "run6-1233:vazmin/bmn_run1233_digi.root",
                  TString bmndstFileName    = "bmndst_1233.root",
                  Int_t   nStartEvent       =  0,
                  Int_t   nEvents           =  1000,
                  Bool_t  isPrimary         =  kFALSE,
                  TString alignCorrFileName = "")
{   // Verbosity level (0=quiet, 1=event-level, 2=track-level, 3=debug)
    Int_t iVerbose = 0;
    // ----    Debug option   --------------------------------------------------
    gDebug = 0;
    // -------------------------------------------------------------------------
    // ----  Load libraries   --------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -------------------------------------------------------------------------
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // -------------------------------------------------------------------------
    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();

    Bool_t isField  = kTRUE;  // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE; // flag for tracking (run with target or not)
    Bool_t isExp    = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;
    // for experimental datasource
    Int_t run_period;
    Int_t run_number;
    Double_t fieldScale = 0.;
    if (inputFileName.Contains(TPRegexp("^run[0-9]+-[0-9]+:"))) {
        Ssiz_t indDash = inputFileName.First('-'), indColon = inputFileName.First(':');
        // get run period
        run_period = TString(inputFileName(3, indDash - 3)).Atoi();
        // get run number
        run_number = TString(inputFileName(indDash + 1, indColon - indDash - 1)).Atoi();
        inputFileName.Remove(0, indColon + 1);

        if (!CheckFileExist(inputFileName)) {
            cout << "Error: digi file " + inputFileName + " does not exist!" << endl;
            exit(-1);
        }
        // set source as raw data file
        fFileSource = new BmnFileSource(inputFileName);

        // get geometry for run
        TString geoFileName = "current_geo_file.root";
        Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, geoFileName.Data());
        if (res_code != 0) {
            cout << "Geometry file can't be read from the database" << endl;
            exit(-1);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
            cout << "Error: could not open ROOT file with geometry: " + geoFileName << endl;
            exit(-2);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*) next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout << "Error: TGeoManager isn't top element in geometry file " + geoFileName << endl;
            exit(-3);
        }
        // set magnet field with factor corresponding to the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0) {
            exit(-2);
        }
        Double_t map_current = 55.87;
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (*field_voltage < 10) {
            fieldScale = 0;
            isField = kFALSE;
        } else {
            fieldScale = (*field_voltage) / map_current;
        }
        BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.dat");
        magField->SetScale(fieldScale);
        magField->Init();
        fRunAna->SetField(magField);
        isExp = kTRUE;
        TString targ;
        if (pCurrentRun->GetTargetParticle() == NULL) {
            targ = "-";
            isTarget = kFALSE; 
        }
        else {
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
    } 
    else { // for simulated files
        if (!CheckFileExist(inputFileName)) return;
        fFileSource = new FairFileSource(inputFileName);
    }
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(bmndstFileName);
    fRunAna->SetGenerateRunInfo(false);

    // "Parameter file" in the FairRoot terminology
    TString parFileName = inputFileName;

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the
    // FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on the fly a
    // concatenated input parameter file, which is then used during the
    // reconstruction.
    TList* parFileNameList = new TList();
    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v15a_gem.digi.par";
    parFileNameList->Add(&stsDigiFile);

    TObjString tofDigiFile = "$VMCWORKDIR/parameters/tof_standard.geom.par";
    parFileNameList->Add(&tofDigiFile);

    if (iVerbose == 0) { // print only progress bar in terminal in quiet mode
        BmnCounter* cntr = new BmnCounter(nEvents);
        fRunAna->AddTask(cntr);
    }
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
    BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp);
    //mwpcHM->SetUseDigitsInTimeBin(kFALSE);
    fRunAna->AddTask(mwpcHM);
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    BmnGemStripConfiguration::GEM_CONFIG gem_config;
    if (!isExp || run_period == 6)
        gem_config = BmnGemStripConfiguration::RunSpring2017;
    else if (run_period == 5)
        gem_config = BmnGemStripConfiguration::RunWinter2016;

    if (!isExp) {
        BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
        gemDigit->SetCurrentConfig(gem_config);
        gemDigit->SetOnlyPrimary(isPrimary);
        gemDigit->SetStripMatching(kTRUE);
        fRunAna->AddTask(gemDigit);
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(isExp);
    gemHM->SetCurrentConfig(gem_config);

    if (isExp) {
        if (alignCorrFileName == "")
            gemHM->SetAlignmentCorrectionsFileName(run_period, run_number);
        else
            // set explicitly, for testing purposes and for interactive alignment;
            // in case of determining alignment corrections from scratch,
            // set alignCorrFileName == "" (at first iteration)
            gemHM->SetAlignmentCorrectionsFileName(alignCorrFileName);
    }
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);
    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kTRUE);
    //tof1HP->SetOnlyPrimary(kTRUE);
    fRunAna->AddTask(tof1HP);
    // ====================================================================== //
    // ===                           TOF2 hit finder                      === //
    // ====================================================================== //
    BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run6.txt", !isExp, iVerbose, kTRUE);
    fRunAna->AddTask(tof2HP);
    // ====================================================================== //
    // ===                           Tracking (MWPC)                      === //
    // ====================================================================== //
    BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp);
    fRunAna->AddTask(mwpcTF);
    // ====================================================================== //
    // ===                           Tracking (GEM)                       === //
    // ====================================================================== //

    Bool_t direction = kTRUE;
    BmnGemSeedFinder* gemSF = new BmnGemSeedFinder();
    gemSF->SetUseLorentz(kTRUE);
    gemSF->SetField(isField);
    gemSF->SetDirection(direction);
    gemSF->SetTarget(isTarget);
    if (isExp)
        gemSF->SetRoughVertex(TVector3(0.0, -3.5, -21.7));
    else
        gemSF->SetRoughVertex(TVector3(0.0, 0.0, 0.0));
    gemSF->SetLineFitCut(5.0);
    gemSF->SetYstep(10.0);
    gemSF->SetSigX(0.05);
    gemSF->SetLorentzThresh(1.01);
    gemSF->SetNbins(1000);
   
    if (run_period == 5)
        gemSF->AddStationToSkip(0);
    fRunAna->AddTask(gemSF);

    BmnGemTrackFinder* gemTF = new BmnGemTrackFinder();
    gemTF->SetField(isField);
    gemTF->SetDirection(direction);
    gemTF->SetTarget(isTarget);
    gemTF->SetDistCut(5.0);
    gemTF->SetNHitsCut(4);
    fRunAna->AddTask(gemTF);

    // Resid. analysis
    if (isExp) {
        BmnGemResiduals* residAnal = new BmnGemResiduals(fieldScale);
        // residAnal->SetUseDistance(kTRUE); // Use distance instead of residuals
        fRunAna->AddTask(residAnal); 
    }

    // ====================================================================== //
    // ===                     Primary vertex finding                     === //
    // ====================================================================== //
    BmnGemVertexFinder* vf = new BmnGemVertexFinder();
    vf->SetField(isField);
    fRunAna->AddTask(vf);
    // ====================================================================== //
    // ===                           Tracking (DCH)                       === //
    // ====================================================================== //
    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    dchTF->SetTransferFunction("pol_coord00813.txt");
    fRunAna->AddTask(dchTF);
    // ====================================================================== //
    // ===                          Global Tracking                       === //
    // ====================================================================== //
    BmnGlobalTracking* glFinder = new BmnGlobalTracking();
    fRunAna->AddTask(glFinder);
    // -----   Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFileName.Data());
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
    cout << "Output file is " + bmndstFileName << endl;
    cout << "Parameter file is " + parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
