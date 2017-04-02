// -----------------------------------------------------------------------------
// Macro for reconstruction of simulated or experimental events.
//
// inputFileName - input file with data.
//
// To process experimental data, you can use 'runN-NNN:'-like prefix
// and then the geometry will be obtained from the Unified Database.
//
// bmndstFileName - output file with reconstructed data.
//
// nStartEvent - number (starts with zero) of first event to process, default: 0.
//
// nEvents - number of events to process, 0 - all events of given file will be
// processed, default: 10000.
//
// isPrimary - flag needed when working with MC events, default: kTRUE.
//
// alignCorrFileName - input file with the current misalignments,
// i.e. alignment corrections but with the opposite sign.
//
// If alignCorrFileName == '', then no corrections are applied at all.
//
// IMPORTANT: for the time being, the default file name should be kept
// up-to-date by hand here.
//
// NB! As soon as storage of the alignment corrections is arranged in the UniDb,
// we will need to change this, so that the default values are taken from there.
// Candidate for the default then can be:
// alignCorrFileName = "UniDb" (case insensitive!)

void run_reco_bmn(TString inputFileName     = "$VMCWORKDIR/macro/run/evetest.root",
                  TString bmndstFileName    = "$VMCWORKDIR/macro/run/bmndst.root",
                  Int_t   nStartEvent       =  0,
                  Int_t   nEvents           =  10000,
                  Bool_t  isPrimary         =  kTRUE)        
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
    Bool_t isTarget = kFALSE; // flag for tracking (run with target or not)
    Bool_t isExp    = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;
    // for experimental datasource
    Int_t run_period;
    Int_t run_number;
    if (inputFileName.Contains(TPRegexp("^run[0-9]+-[0-9]+:")))
    {
        Ssiz_t indDash = inputFileName.First('-'), indColon = inputFileName.First(':');
        // get run period
        run_period = TString(inputFileName(3, indDash - 3)).Atoi();
        // get run number
        run_number = TString(inputFileName(indDash + 1, indColon - indDash - 1)).Atoi();
        inputFileName.Remove(0, indColon + 1);

        if ( ! CheckFileExist(inputFileName)) {
            cout <<"Error: digi file "+inputFileName+" does not exist!"<< endl;
            exit(-1);
        }
        // set source as raw data file
        fFileSource = new BmnFileSource(inputFileName);

        // get geometry for run
        TString geoFileName = "current_geo_file.root";
        Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, geoFileName.Data());
        if (res_code != 0) {
            cout <<"Geometry file can't be read from the database"<< endl;
            exit(-1);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if ( ! geoFile->IsOpen()) {
            cout <<"Error: could not open ROOT file with geometry: "+geoFileName<< endl;
            exit(-2);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter  next(keyList);
        TKey*  key = (TKey*)next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout <<"Error: TGeoManager isn't top element in geometry file "+geoFileName<< endl;
            exit(-3);
        }
        // set magnet field with factor corresponding to the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0) {
            exit(-2);
        }
        Double_t fieldScale = 0.0;
        Double_t map_current  = 55.87;
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (*field_voltage == 0) {
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
            isTarget = kFALSE; }
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
        if ( ! CheckFileExist(inputFileName)) return;
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

    if (isExp && iVerbose == 0) { // print only progress bar in terminal in quiet mode
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
    if (run_period == 6)
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
    // Set name of file with the alignment corrections (derived from database in future)
    if (isExp) gemHM->SetAlignmentCorrectionsFileName(run_period, run_number);
    // Set name of file with the alignment corrections by hands (for test purposes and iterative alignment also)
    // if (isExp) gemHM->SetAlignmentCorrectionsFileName("$VMCWORKDIR/input/align.root");
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
    CbmTofHitProducer* tof2HP = new CbmTofHitProducer("TOF HitProducer", iVerbose);
    tof2HP->SetZposition(700.);
    tof2HP->SetXshift(32.);
  //fRunAna->AddTask(tof2HP);
    // ====================================================================== //
    // ===                           Tracking (MWPC)                      === //
    // ====================================================================== //
    BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp);
    fRunAna->AddTask(mwpcTF);
    // ====================================================================== //
    // ===                           Tracking (GEM)                       === //
    // ====================================================================== //
    BmnGemSeedFinder* gemSF = new BmnGemSeedFinder();
    gemSF->SetUseLorentz(kTRUE);
    gemSF->SetField(isField);
    gemSF->SetTarget(isTarget);
  //gemSF->SetXRange(-5.0,  20.0);
  //gemSF->SetYRange(-4.8., -3.8);
    if (run_period == 5) // in run 6 that staion is already skipped
        gemSF->AddStationToSkip(0);
  //gemSF->AddStationToSkip(1);
  //gemSF->AddStationToSkip(2);
    fRunAna->AddTask(gemSF);

    BmnGemTrackFinder* gemTF = new BmnGemTrackFinder();
    gemTF->SetField(isField);
    fRunAna->AddTask(gemTF);
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
    FairRuntimeDb*      rtdb   = fRunAna->GetRuntimeDb();
    FairParRootFileIo*  parIo1 = new FairParRootFileIo();
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
    cout <<"Starting run"<< endl;
    fRunAna->Run(nStartEvent, nStartEvent+nEvents);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout <<"Macro finished successfully."<< endl; // marker of successful execution for CDASH
    cout <<"Output file is "+bmndstFileName<< endl;
    cout <<"Parameter file is "+parFileName<< endl;
    cout <<"Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<< endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
