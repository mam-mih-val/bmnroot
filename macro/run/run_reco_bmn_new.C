// -----------------------------------------------------------------------------
// Macro for reconstruction of simulated or experimental events.
//
// digiFileName - input file with data, default: evetest.root.
//
// To process experimental data, you can use 'run[0]*N-[0]*NNN:'-like prefix
// (with arbirtrary number of leading zeros), e.g.
// "run005-000458:../digits_run5/bmn_run458_digi.root", and then the geometry will be
// obtained from the Unified Database.
//
// However, if the file name is like "bmn_run005_Glob_000812_digi.root",
// then nothing to do: the geometry will be
// obtained from the Unified Database automatically without any prefix, although
// if prefix is still used, everything also works correctly.
//
// bmndstFileName - output file with reconstructed data, default: bmndst.root.
//
// nStartEvent - number (starts with zero) of first event to process, default: 0.
//
// nEvents - number of events to process, 0 - all events of given file will be
// proccessed, default: 40000000.
//
// isPrimary - flag needed when working with MC events, default: kTRUE.
//
// alignCorrFileName - input file with the alignment corrections,
// default: for the time being should be kept up-to-date by hand here.

void run_reco_bmn(TString digiFileName      = "$VMCWORKDIR/macro/run/evetest.root"
                 ,TString bmndstFileName    = "$VMCWORKDIR/macro/run/bmndst.root"
                 ,Int_t   nStartEvent       =  0
                 ,Int_t   nEvents           =  40000000
                 ,Bool_t  isPrimary         =  kTRUE
                 ,TString alignCorrFileName = "alignCorrsLocal_GEM.root"
                 )
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
    // check existence of the input digi file
    if ( ! CheckFileExist(digiFileName)) {
        cout <<"Error: digi file "+digiFileName+" does not exist!"<< endl;
        exit(-1);
    }
    // -----   Reconstruction run   --------------------------------------------
    FairRunAna *fRunAna = new FairRunAna();

    Bool_t isField  = kTRUE;  // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE;  // flag for tracking (run with target or not)
    Bool_t isExp    = kFALSE; // flag for hit finder (to create digits or take them from data-file)
    // -------------------------------------------------------------------------
    // Set input source as simulation file or experimental data
    FairSource* fFileSource;

    // for experimental datasource:
    TString digiFileName(digiFileName);
    if (digiFileName.Contains("bmn_run")) {
        // A new, more flexible extraction of run number and run period of the file.
        // Anatoly.Solomin@jinr.ru 2017-02-15 20:42:13
        TString substr; // to store temporary substrings as TString,
                        // not TSubString, because Atoi works only with TString
        TRegexp re = "_[0-9]+_"; // to catch run number in file names
        if (digiFileName.Contains(":")) { // when 'run[0]*N-[0]*NNN:'-like prefix is used
                                          // (with arbitratry number of leading zeros)
            TString  prefix = digiFileName("run[0-9]+-[0-9]+:");
            // get run period from 'runN'
            substr = prefix("run[0-9]+");
            substr = substr(   "[0-9]+");
            Int_t    runPeriod = substr.Atoi();
            // get run number from '-NNN'
            substr = prefix("-[0-9]+");
            substr = substr( "[0-9]+");
            Int_t    runNumber = substr.Atoi();
            // now strip digiFileName from the 'run[0]*N-[0]*NNN:' prefix
            digiFileName.ReplaceAll(prefix, ""); }
        else if (digiFileName.Contains(re)) { // when name is like 'bmn_run005_Glob_000812_digi.root'
            // get run period
            substr = digiFileName("bmn_run[0-9]+");
            substr = substr(             "[0-9]+");
            Int_t    runPeriod = substr.Atoi();
            // get run number
            substr = digiFileName("_[0-9]+_");
            substr = substr(       "[0-9]+");
            Int_t    runNumber = substr.Atoi(); }
        else {
            cout <<"Error: digi file name "+digiFileName+" is not recognized!"<< endl;
            exit(-1);
        }
        // check existence of the input digi file (prefix already stripped from name):
        if (!CheckFileExist(digiFileName)) {
            cout <<"Error: digi file "+digiFileName+" does not exist!"<< endl;
            exit(-1);
        }
        cout <<"runPeriod = "<<runPeriod<<"  runNumber = "<<runNumber<< endl;

        // set source as raw data file
        if (!CheckFileExist(digiFileName)) return;
        fFileSource = new BmnFileSource(digiFileName);

        // get geometry for run
        TString geoFileName = "current_geo_file.root";
        Int_t res_code = UniDbRun::ReadGeometryFile(runPeriod, runNumber, geoFileName.Data());
        if (res_code != 0) {
            cout << "\nGeometry file can't be read from the database" << endl;
            exit(-1);
        }

        // get gGeoManager from ROOT file (if required)
        TFile* geoFile = new TFile(geoFileName, "READ");
        if (!geoFile->IsOpen()) {
            cout <<"Error: could not open ROOT file with geometry: "+geoFileName<< endl;
            exit(-2);
        }
        TList* keyList = geoFile->GetListOfKeys();
        TIter next(keyList);
        TKey* key = (TKey*) next();
        TString className(key->GetClassName());
        if (className.BeginsWith("TGeoManager"))
            key->ReadObj();
        else {
            cout <<"Error: TGeoManager isn't top element in geometry file "+geoFileName<< endl;
            exit(-3);
        }
        // set magnet field with factor corresponding the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(runPeriod, runNumber);
        if (pCurrentRun == 0) {
            exit(-2);
        }
        Double_t fieldScale =   0;
        double map_current  = 900.0;
        int* current_current = pCurrentRun->GetFieldCurrent();
        if (*current_current == 0) {
            fieldScale = 0;
            isField = kFALSE; }
        else {
            fieldScale = (*current_current) / map_current;
        }
        BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v3_ascii_Extrap.dat");
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
        cout << "||\t\tPeriod:\t\t" << runPeriod << "\t\t\t||" << endl;
        cout << "||\t\tNumber:\t\t" << runNumber << "\t\t\t||" << endl;
        cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
        cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
        cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
        cout << "||\t\t\t\t\t\t\t||" << endl;
        cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
    } // for simulated files
    else {
        if (!CheckFileExist(digiFileName)) return;
        fFileSource = new FairFileSource(digiFileName);
    }
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(bmndstFileName);
    fRunAna->SetGenerateRunInfo(false);

    // "Parameter file" in the FairRoot terminology
    TString parFileName = digiFileName;

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the
    // FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on the fly a
    // concatenated input parameter file, which is then used during the
    // reconstruction.
    TList *parFileNameList = new TList();
    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v15a_gem.digi.par";
    parFileNameList->Add(&stsDigiFile);

    TObjString tofDigiFile = "$VMCWORKDIR/parameters/tof_standard.geom.par";
    parFileNameList->Add(&tofDigiFile);
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
  //BmnMwpcHitProducer* mwpcHP1 = new BmnMwpcHitProducer(1);
  //fRunAna->AddTask(mwpcHP1);
  //BmnMwpcHitProducer* mwpcHP2 = new BmnMwpcHitProducer(2);
  //fRunAna->AddTask(mwpcHP2);
  //BmnMwpcHitProducer* mwpcHP3 = new BmnMwpcHitProducer(3);
  //fRunAna->AddTask(mwpcHP3);
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunWinter2016; // config file: (GEM_RunWinter2016.root))

    if (!isExp) {
        BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
        gemDigit->SetCurrentConfig(gem_config);
        gemDigit->SetOnlyPrimary(isPrimary);
        gemDigit->SetStripMatching(kTRUE);
        fRunAna->AddTask(gemDigit);
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(isExp);
    gemHM->SetCurrentConfig(gem_config);
    // Set name of file with the alignment corrections:
    // at iteration 1 will be empty, if we run from scratch,
    // or a default one, which currently is $VMCWORKDIR/input/alignCorrsLocal_GEM.root,
    // (but later on will be taken from the UniDB).
    // Starting from iteration 2, it will contain pattern "_it[1-10]+";
    // in that case [<path>/]alignCorrFileName will be used in BmnGemStripHitMaker as is,
    // instead of the default file.
    // Anatoly.Solomin@jinr.ru 2017-02-15 20:42:13
    gemHM->SetAlignmentCorrectionsFileName(alignCorrFileName);
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);

    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //
  //BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", kFALSE, 1, kTRUE);
  //tof1HP->SetOnlyPrimary(kTRUE);
  //fRunAna->AddTask(tof1HP);
    // ====================================================================== //
    // ===                           TOF2 hit finder                      === //
    // ====================================================================== //
  //CbmTofHitProducer* tof2HP = new CbmTofHitProducer("TOF HitProducer", iVerbose);
  //tof2HP->SetZposition(700.);
  //tof2HP->SetXshift(32.);
  //fRunAna->AddTask(tof2HP);
    // ====================================================================== //
    // ===                           Tracking (GEM)                       === //
    // ====================================================================== //
    BmnGemSeedFinder* gemSF = new BmnGemSeedFinder();
    gemSF->SetUseLorentz(kTRUE);
    gemSF->SetField(isField);
    gemSF->SetTarget(isTarget);
    gemSF->SetXRange( 0.0, 100.0);
    gemSF->SetYRange(-1.0,   1.0);
    gemSF->AddStationToSkip(0);
  //gemSF->AddStationToSkip(1);
  //gemSF->AddStationToSkip(2);
    fRunAna->AddTask(gemSF);

    BmnGemTrackFinder* gemTF = new BmnGemTrackFinder();
    gemTF->SetField(isField);
    fRunAna->AddTask(gemTF);
    // -----   Primary vertex finding   ----------------------------------------
    BmnGemVertexFinder* vf = new BmnGemVertexFinder();
    vf->SetField(isField);
    fRunAna->AddTask(vf);
    // -------------------------------------------------------------------------
    // -----   Global tracking   -----------------------------------------------
  //BmnGlobalTracking* glFinder = new BmnGlobalTracking();
  //fRunAna->AddTask(glFinder);
    // -------------------------------------------------------------------------
    // ====================================================================== //
    // ===                           Tracking (DCH)                       === //
    // ====================================================================== //
    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    fRunAna->AddTask(dchTF);
    // -----  Parameter database   ---------------------------------------------
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
