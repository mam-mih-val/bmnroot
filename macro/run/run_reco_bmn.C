// Macro for reconstruction of simulated or experimental events.
//
// inputFileName - input file with data.
//
// To process experimental data, you must use 'run[#period]-[#run]:'-like prefix,
// and then the geometry will be obtained from the Unified Database
// e.g. "run6-1986:/dataBMN/bmndata1/run6/root/digi/bmn_run1986_digi.root"
//
// bmndstFileName - output file with reconstructed data.
//
// nStartEvent - number of first event to process (starts with zero), default: 0.
//
// nEvents - number of events to process, 0 - all events of given file will be
// processed, default: 10000.
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

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void run_reco_bmn(TString inputFileName = "$VMCWORKDIR/macro/run/evetest.root",
        TString bmndstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
        Int_t nStartEvent = 0,
        Int_t nEvents = 10000,
        TString alignCorrFileName = "default",
        TString steerGemTrackingFile = "gemTrackingSteer.dat")
{
    // Verbosity level (0=quiet, 1=event-level, 2=track-level, 3=debug)
    Int_t iVerbose = 0;

    // ----    Debug option   --------------------------------------------------
    gDebug = 0;

    // ----  Load libraries   --------------------------------------------------
    bmnloadlibs(); // load BmnRoot libraries

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();

    Bool_t isField = kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE;//kFALSE; // flag for tracking (run with target or not)
    Bool_t isExp = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;
    // for experimental datasource
    Int_t run_period=6, run_number;
    Double_t fieldScale = 0.;
    TPRegexp run_prefix("^run[0-9]+-[0-9]+:");
    if (inputFileName.Contains(run_prefix)) {
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
        Int_t res_code = UniDbRun::ReadGeometryFile(run_period, run_number, (char*) geoFileName.Data());
        if (res_code != 0) {
            cout << "Geometry file can't be read from the database" << endl;
            exit(-2);
        }

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
            cout << "Error: TGeoManager isn't top element in geometry file " + geoFileName << endl;
            exit(-4);
        }

        // set magnet field with factor corresponding to the given run
        UniDbRun* pCurrentRun = UniDbRun::GetRun(run_period, run_number);
        if (pCurrentRun == 0)
            exit(-5);
        Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
        if (field_voltage == NULL)
        {
            cout<<"Error: no field voltage was found for run "<<run_period<<":"<<run_number<<endl;
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
        if (!CheckFileExist(inputFileName)) return;
        fFileSource = new FairFileSource(inputFileName);
    }
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(bmndstFileName);
    fRunAna->SetGenerateRunInfo(false);

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the
    // FairParAsciiFileIo.
    // The FairParAsciiFileIo will take care to create on fly a
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
    // ===                           Check Triggers                       === //
    // ====================================================================== //
    //    BmnTriggersCheck* triggs = new BmnTriggersCheck(isExp, run_period, run_number);
    //    fRunAna->AddTask(triggs);
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
    BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp);
    mwpcHM->SetUseDigitsInTimeBin(kFALSE);
    fRunAna->AddTask(mwpcHM);
    // ====================================================================== //
    // ===                         Silicon hit finder                     === //
    // ====================================================================== //
    BmnSiliconConfiguration::SILICON_CONFIG si_config;
    switch(run_period) {
        case 6: //BM@N RUN-6
            si_config = BmnSiliconConfiguration::RunSpring2017;
            break;
        case 7: //BM@N RUN-7
            si_config = BmnSiliconConfiguration::RunSpring2018;
            break;
        default:
            si_config = BmnSiliconConfiguration::RunSpring2017;
            //si_config = BmnSiliconConfiguration::RunSpring2018;
    }
    BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(isExp);
    siliconHM->SetCurrentConfig(si_config);
    fRunAna->AddTask(siliconHM);
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    BmnGemStripConfiguration::GEM_CONFIG gem_config;
    switch(run_period) {
        case 5: //BM@N RUN-5
            gem_config = BmnGemStripConfiguration::RunWinter2016;
            break;
        case 6: //BM@N RUN-6
            gem_config = BmnGemStripConfiguration::RunSpring2017;
            break;
        case 7: //BM@N RUN-7
            gem_config = BmnGemStripConfiguration::RunSpring2018;
            break;
        default:
            gem_config = BmnGemStripConfiguration::RunSpring2017;
            //gem_config = BmnGemStripConfiguration::RunSpring2018;
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(run_period, isExp);
    gemHM->SetCurrentConfig(gem_config);
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);

    // ====================================================================== //
    // ===                           ALIGNMENT (GEM + SI)                 === //
    // ====================================================================== //
    if (isExp) {
        if (alignCorrFileName == "default") {
            gemHM->SetAlignmentCorrectionsFileName(run_number);
            siliconHM->SetAlignmentCorrectionsFileName(run_period, run_number);
        }
        else {
            gemHM->SetAlignmentCorrectionsFileName(alignCorrFileName);
            siliconHM->SetAlignmentCorrectionsFileName(alignCorrFileName);
        }
    }

    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kTRUE);
    tof1HP->SetPeriod(run_period);
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

    BmnGemTracking* gemTF = new BmnGemTracking(run_period, isField, isTarget, steerGemTrackingFile);
    if (!isExp) gemTF->SetRoughVertex(TVector3(0.0, 0.0, 0.0)); //for MC case use correct vertex
    fRunAna->AddTask(gemTF);

    // ====================================================================== //
    // ===                           Tracking (DCH)                       === //
    // ====================================================================== //
    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    dchTF->SetTransferFunction("pol_coord00813.txt");
    fRunAna->AddTask(dchTF);
    // ====================================================================== //
    // ===                          Global Tracking                       === //
    // ====================================================================== //
    BmnGlobalTracking* globalTF = new BmnGlobalTracking(isExp);
    globalTF->SetField(isField);
    fRunAna->AddTask(globalTF);

    // ====================================================================== //
    // ===                     Primary vertex finding                     === //
    // ====================================================================== //
    BmnVertexFinder* gemVF = new BmnVertexFinder(run_period, isField);
    // gemVF->SetVertexApproximation(TVector3(0., 0., 0.));
    fRunAna->AddTask(gemVF);

    // Residual analysis
    if (isExp) {
        BmnGemResiduals* residAnalGem = new BmnGemResiduals(run_period, run_number, fieldScale);
        // residAnal->SetPrintResToFile("file.txt");
        // residAnal->SetUseDistance(kTRUE); // Use distance instead of residuals
        fRunAna->AddTask(residAnalGem);
        // BmnSiResiduals* residAnalSi = new BmnSiResiduals(run_period, run_number, fieldScale);
        // fRunAna->AddTask(residAnalSi);
    }
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
    cout << "Output file is " + bmndstFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
    // ------------------------------------------------------------------------
}
