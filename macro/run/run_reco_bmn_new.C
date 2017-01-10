// -----------------------------------------------------------------------------
// Macro for reconstruction of simulated or experimental events.
//
// digiFileName - input file with data, default: evetest.root.
//
// To process experimental data, you can use 'runN-NNN:'-like prefix with
// arbirtrary leading zeros, e.g.
// "run5-458:../digits_run5/bmn_run458_digi.root" then the geometry will be
// obtained from the Unified Database.
//
// However, if the file name is like
// "bmn_run005_Glob_000812_digi.root", then nothing to do: the geometry will be
// obtained from the Unified Database automatically without any prefix, although
// if prefix is still used, it also works correctly.
//
// bmndstFileName - output file with reconstructed data, default: bmndst.root.
//
// nStartEvent - number (start with zero) of first event to process, default: 0.
//
// nEvents - number of events to process, 0 - all events of given file will be
// proccessed, default: 1.
//
// isPrimary - flag needed when working with MC events, default: kTRUE.
//
// alignCorrFileName - input file with the alignment corrections, default: "".
//
void run_reco_bmn_new(TString digiFileName      = "$VMCWORKDIR/macro/run/evetest.root",
                      TString bmndstFileName    = "$VMCWORKDIR/macro/run/bmndst.root",
                      Int_t   nStartEvent       = 0,
                      Int_t   nEvents           = 40000000,
                      Bool_t  isPrimary         = kTRUE,
                      TString alignCorrFileName = "")
{
    if (!CheckFileExist(digiFileName)) return;
    // when name of the file with alignment corrections is not empty,
    // check if it exists 
    if (alignCorrFileName != ""  &&  !CheckFileExist(alignCorrFileName)) return;
    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;
    // -----   Debug option   --------------------------------------------------
    gDebug = 0;
    // -----   Load libraries   ------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRun = new FairRunAna();

    Bool_t isField  = kTRUE;  // flag for tracking:   use magnetic field or not
    Bool_t isTarget = kTRUE;  // flag for tracking:   run with target or not
    Bool_t isExp    = kFALSE; // flag for hit finder: create digits or take them from data-file

    // Set input source as simulation file or experimental data
  //FairSource* fFileSource;
    // for experimental datasource
    if (digiFileName.Contains("bmn_run[0-9]+")) {
        if (digiFileName.Contains(":")) { // when 'runN-NNN:' prefix is used
            // get run period from 'runN'
            Int_t   runPeriod = ((TString)digiFileName("[0-9]+")).Atoi();
            // get run number from '-NNN'
            TString runNumStr = digiFileName("-[0-9]+");
            Int_t   runNumber = ((TString)runNumStr("[0-9]+")).Atoi();
            // now strip digiFileName from the 'runN-NNN:' prefix
            digiFileName("run[0-9]+-[0-9]+:") = "";
        }
        else if (digiFileName.Contains("_[0-9]+_")) { // when name is like 'bmn_run005_Glob_000812_digi.root'
            // get run period
            TString runPerStr = digiFileName("bmn_run[0-9]+");
            Int_t   runPeriod = ((TString)runPerStr("[0-9]+")).Atoi();
            // get run number
            TString runNumStr = digiFileName(      "_[0-9]+_");
            Int_t   runNumber = ((TString)runNumStr("[0-9]+")).Atoi();
        }
        else {
            cout << "Error: digi file name is not recognized: "+geoFileName << endl;
            exit(-1);
        }
        // set source as raw data file
        FairSource* fFileSource = new BmnFileSource(digiFileName);
        fRun->SetSource(fFileSource);
        // get geometry for run
        TString geoFileName = "current_geo_file.root";
        Int_t res_code = UniDbRun::ReadGeometryFile(runPeriod, runNumber, geoFileName.Data());
        if (res_code != 0) {
            cout << "Error: geometry file can't be read from the database" << endl;
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
        Double_t fieldScale = 0;
        double map_current = 900.0;
        int* current_current = pCurrentRun->GetFieldCurrent();
        if (*current_current == 0) {
            fieldScale = 0;
            isField = kFALSE;
        }
        else
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
        }
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
    }
    else { // for simulated files
        FairSource* fFileSource = new FairFileSource(digiFileName);
        fRun->SetSource(fFileSource);
    }
  //fRun->SetSource(fFileSource);
    fRun->SetOutputFile(bmndstFileName);
    fRun->SetGenerateRunInfo(false);
    // "Parameter file" in the FairRoot terminology
    TString parFileName = digiFileName;
    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the
    // FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on the fly a
    // concatenated input parameter file, which is then used during the
    // reconstruction.
    TList* parFileList = new TList();
    TObjString stsdigiFileName = "$VMCWORKDIR/parameters/sts_v15a_gem.digi.par";
    parFileList->Add(&stsdigiFileName);

    TObjString tofdigiFileName = "$VMCWORKDIR/parameters/tof_standard.geom.par";
    parFileList->Add(&tofdigiFileName);
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
  //BmnMwpcHitProducer* mwpcHP1 = new BmnMwpcHitProducer(1);
  //fRun->AddTask(mwpcHP1);
  //BmnMwpcHitProducer* mwpcHP2 = new BmnMwpcHitProducer(2);
  //fRun->AddTask(mwpcHP2);
  //BmnMwpcHitProducer* mwpcHP3 = new BmnMwpcHitProducer(3);
  //fRun->AddTask(mwpcHP3);
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
  //BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSummer2016; // config file: GEM_RunSummer2016.root
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunWinter2016; // config file: GEM_RunWinter2016.root
    if (!isExp) {
        BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
        gemDigit->SetCurrentConfig(gem_config);
        gemDigit->SetOnlyPrimary(isPrimary);
        gemDigit->SetStripMatching(kTRUE);
        fRun->AddTask(gemDigit);
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(isExp);
    gemHM->SetCurrentConfig(gem_config);
    gemHM->SetAlignmentCorrections(alignCorrFileName);
    gemHM->SetHitMatching(kTRUE);
    fRun->AddTask(gemHM);
    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", kFALSE, 1, kTRUE);
  //tof1HP->SetOnlyPrimary(kTRUE);
  //fRun->AddTask(tof1HP);
    // ====================================================================== //
    // ===                           DCH1 hit finder                      === //
    // ====================================================================== //
  //BmnDchHitProducer*    dch1HP = new BmnDchHitProducer(1,0,false);
    BmnDchHitProducerTmp* dch1HP = new BmnDchHitProducerTmp(1);
  //dch1HP->SetOnlyPrimary(kTRUE);
  //fRun->AddTask(dch1HP);
    // ====================================================================== //
    // ===                          DCH2 hit finder                       === //
    // ====================================================================== //
  //BmnDchHitProducer*    dch2HP = new BmnDchHitProducer(2,0,false);
    BmnDchHitProducerTmp* dch2HP = new BmnDchHitProducerTmp(2);
  //dch2HP->SetOnlyPrimary(kTRUE);
  //fRun->AddTask(dch2HP);
    // ====================================================================== //
    // ===                           TOF2 hit finder                      === //
    // ====================================================================== //
    CbmTofHitProducer* tof2HP = new CbmTofHitProducer("TOF HitProducer", iVerbose);
    tof2HP->SetZposition(700.);
    tof2HP->SetXshift(32.);
  //fRun->AddTask(tof2HP);
    // ====================================================================== //
    // ===                           Tracking                             === //
    // ====================================================================== //
    BmnGemSeedFinder* gemSF = new BmnGemSeedFinder();
    gemSF->SetUseLorentz(kTRUE);
    gemSF->SetField(isField);
    gemSF->SetTarget(isTarget);
    fRun->AddTask(gemSF);

    BmnGemTrackFinder* gemTF = new BmnGemTrackFinder();
    gemTF->SetField(isField);
    fRun->AddTask(gemTF);

  //BmnGlobalTracking* glFinder = new BmnGlobalTracking();
  //glFinder->SetRun1(kRUN1);
  //fRun->AddTask(glFinder);
    // -----   Primary vertex finding   ----------------------------------------
    BmnGemVertexFinder* vf = new BmnGemVertexFinder();
    vf->SetField(isField);
    fRun->AddTask(vf);
    // -------------------------------------------------------------------------
    // ====================================================================== //
    // ===                         End of tracking                        === //
    // ====================================================================== //
    // -----  Parameter database   ---------------------------------------------
    FairRuntimeDb*      rtdb   = fRun->GetRuntimeDb();
    FairParRootFileIo*  parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(parFileName.Data());
    parIo2->open(parFileList, "in");
    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // -------------------------------------------------------------------------
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Output file is "+bmndstFileName << endl;
    cout << "Parameter file is "+parFileName << endl;
    cout << "Real time "<<rtime<<" s, CPU time "<<ctime<<" s" << endl;
    cout << endl;
    // -------------------------------------------------------------------------
}
