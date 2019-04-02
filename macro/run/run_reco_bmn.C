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
// nEvents - number of events to process, 0 - all events of given file will be processed, default: 10000.
//
// alignCorrFileName - argument for choosing input file with the alignment corrections.
//
// If alignCorrFileName == 'default', (case insensitive) then corrections are
// retrieved from UniDb according to the running period and run number.
// If alignCorrFileName == '', then no corrections are applied at all.
// If alignCorrFileName == '<path>/<file-name>', then the corrections are taken from that file.

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

#define CellAuto // Choose Tracking: L1 or CellAuto

void run_reco_bmn(TString inputFileName = "$VMCWORKDIR/macro/run/evetest.root",
        TString bmndstFileName = "$VMCWORKDIR/macro/run/bmndst.root",
        Int_t nStartEvent = 0,
        Int_t nEvents = 1000)
{
    // Verbosity level (0=quiet, 1=event-level, 2=track-level, 3=debug)
    Int_t iVerbose = 0;
    gDebug = 0; // Debug option

    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // -----   Reconstruction run   --------------------------------------------
    FairRunAna* fRunAna = new FairRunAna();

    Bool_t isField = (inputFileName.Contains("noField")) ? kFALSE : kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE;//kFALSE; // flag for tracking (run with target or not)
    Bool_t isExp = kFALSE; // flag for hit finder (to create digits or take them from data-file)

    // Declare input source as simulation file or experimental data
    FairSource* fFileSource;
    // for experimental datasource
    Int_t run_period = 7, run_number = -1;
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
    fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
    fRunAna->SetGenerateRunInfo(false);

    // Digitisation files.
    // Add TObjectString file names to a TList which is passed as input to the FairParAsciiFileIo.
    // The FairParAsciiFileIo will create on fly a concatenated input parameter file, which is then used during the reconstruction.
    TList* parFileNameList = new TList();
#ifdef L1
    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v1_BMN_SI_GEM.digi.par";
#else
    TObjString stsDigiFile = "$VMCWORKDIR/parameters/sts_v15a_gem.digi.par";
#endif
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

#ifdef L1
    // ====================================================================== //
    // ===                           STS digitizer                        === //
    // ====================================================================== //
    CbmBmnStsDigitize* stsDigitize = new CbmBmnStsDigitize("STS Digitiser", iVerbose);
    stsDigitize->SetGeoFile("$VMCWORKDIR/geometry/SIGEMS_r7.root");
    fRunAna->AddTask(stsDigitize);

    // ====================================================================== //
    // ===                         STS Cluster Finder                     === //
    // ====================================================================== //
    FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
    fRunAna->AddTask(stsClusterFinder);

    // ====================================================================== //
    // ===                          STS hit finder                        === //
    // ====================================================================== //
    FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
    fRunAna->AddTask(stsFindHits);
#else
    // ====================================================================== //
    // ===                         Silicon hit finder                     === //
    // ====================================================================== //
    BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(run_period, run_number, isExp);
    //siliconHM->SetCurrentConfig(BmnSiliconConfiguration::RunSpring2018); //set explicitly
    fRunAna->AddTask(siliconHM);

    // ====================================================================== //
    // ===                          GEM hit finder                        === //
    // ====================================================================== //
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(run_period, run_number, isExp);
    //gemHM->SetCurrentConfig(BmnGemStripConfiguration::RunSpring2018); //set explicitly
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);
#endif

    // ====================================================================== //
    // ===                          TOF1 hit finder                       === //
    // ====================================================================== //
    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kTRUE);
    tof1HP->SetPeriod(run_period);
    //tof1HP->SetOnlyPrimary(kTRUE);
    fRunAna->AddTask(tof1HP);

    // ====================================================================== //
    // ===                          TOF2 hit finder                       === //
    // ====================================================================== //
    BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run7.txt", !isExp, iVerbose, kTRUE);
    tof2HP->SetTimeResolution(0.115);
    tof2HP->SetMCTimeFile("TOF700_MC_time_run7.txt");
    fRunAna->AddTask(tof2HP);

    // ====================================================================== //
    // ===                          Tracking (MWPC)                       === //
    // ====================================================================== //
    BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp);
    fRunAna->AddTask(mwpcTF);

#ifdef L1
    // ====================================================================== //
    // ===                         STS track finding                      === //
    // ====================================================================== //
    CbmKF* kalman = new CbmKF();
    fRunAna->AddTask(kalman);

    CbmL1* l1 = new CbmL1();
    TString stsMatBudgetFile = ""; // paramDir + "/sts/sts_matbudget_v12b_12344444.root"; // paramDir + "/sts_matbudget_var_fr.root";
    l1->SetMaterialBudgetFileName(stsMatBudgetFile);
    fRunAna->AddTask(l1);

    CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
    FairTask*          stsFindTracks  = new CbmStsFindTracks(iVerbose, stsTrackFinder);
    fRunAna->AddTask(stsFindTracks);
#else
    // ====================================================================== //
    // ===                       Tracking (InnerTracker)                  === //
    // ====================================================================== //
    BmnCellAutoTracking* gemTF = new BmnCellAutoTracking(run_period, run_number, isField, isTarget);
    gemTF->SetDetectorPresence(kSILICON, kTRUE);
    gemTF->SetDetectorPresence(kSSD, kFALSE);
    gemTF->SetDetectorPresence(kGEM, kTRUE);
    //if (!isExp) gemTF->SetRoughVertex(TVector3(0.0, 0.0, 0.0)); //for MC case use correct vertex
    fRunAna->AddTask(gemTF);
#endif

    // ====================================================================== //
    // ===                          Tracking (DCH)                        === //
    // ====================================================================== //
    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    dchTF->SetTransferFunction("pol_coord00813.txt");
    fRunAna->AddTask(dchTF);

#ifdef L1
    // ====================================================================== //
    // ===                      Primary vertex finding                    === //
    // ====================================================================== //
    CbmPrimaryVertexFinder* pvFinder   = new CbmPVFinderKF();
    CbmFindPrimaryVertex *  findVertex = new CbmFindPrimaryVertex(pvFinder);
    fRunAna->AddTask(findVertex);
#else
    // ====================================================================== //
    // ===                      Primary vertex finding                    === //
    // ====================================================================== //
    BmnVertexFinder* gemVF = new BmnVertexFinder(run_period, isField);
    // gemVF->SetVertexApproximation(TVector3(0., 0., 0.));
    fRunAna->AddTask(gemVF);
#endif

    // Residual analysis
    BmnResiduals* res = new BmnResiduals(run_period, run_number, isField);
    fRunAna->AddTask(res);

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
