// --------------------------------------------------------------------------
// Macro for reconstruction of simulated events with standard settings
//
// HitProducers in MVD, RICH, TRD, TOF, ECAL
// Digitizer and HitFinder in STS
// FAST MC for ECAL
// STS track finding and fitting (L1 / KF)
// TRD track finding and fitting (L1 / KF)
// RICH ring finding (ideal) and fitting
// Global track finding (ideal), rich assignment
// Primary vertex finding (ideal)
// Matching of reconstructed and MC tracks in STS, RICH and TRD
//
// V. Friese   24/02/2006
// Version     24/04/2007 (V. Friese)
//
// --------------------------------------------------------------------------


// inFile - input file with MC data, default: evetest.root
// nStartEvent - number (start with zero) of first event to process, default: 0
// nEvents - number of events to process, 0 - all events of given file will be proccessed, default: 1
// outFile - output file with reconstructed data, default: mpddst.root

void run_reco_bmn(TString inFile = "$VMCWORKDIR/macro/run/evetest.root", TString outFile = "$VMCWORKDIR/macro/run/bmndst.root", Int_t nStartEvent = 0, Int_t nEvents = 10000000) {
    // ========================================================================
    // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
    Int_t iVerbose = 0;

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


    // In general, the following parts need not be touched
    // ========================================================================

    // ----    Debug option   -------------------------------------------------
    gDebug = 0;
    // ------------------------------------------------------------------------

    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries

    // ------------------------------------------------------------------------

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ------------------------------------------------------------------------

    // -----   Reconstruction run   -------------------------------------------
    FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(inFile);
    fRun->SetOutputFile(outFile);
    fRun->SetWriteRunInfoFile(false);
    // ------------------------------------------------------------------------

    // =========================================================================
    // ===             Detector Response Simulation (Digitiser)              ===
    // ===                          (where available)                        ===
    // =========================================================================



    // -----   STS digitizer   -------------------------------------------------
    Double_t threshold = 4;
    Double_t noiseWidth = 0.01;
    Int_t nofBits = 12;
    Double_t electronsPerAdc = 10;
    Double_t StripDeadTime = 0.1;
    CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
    stsDigitize->SetRealisticResponse();
    stsDigitize->SetFrontThreshold(threshold);
    stsDigitize->SetBackThreshold(threshold);
    stsDigitize->SetFrontNoiseWidth(noiseWidth);
    stsDigitize->SetBackNoiseWidth(noiseWidth);
    stsDigitize->SetFrontNofBits(nofBits);
    stsDigitize->SetBackNofBits(nofBits);
    stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
    stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
    stsDigitize->SetStripDeadTime(StripDeadTime);
    //   fRun->AddTask(stsDigitize);
    // -------------------------------------------------------------------------

    // =========================================================================
    // ===                      STS local reconstruction                     ===
    // =========================================================================


    // -----   STS Cluster Finder   --------------------------------------------
    FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder", iVerbose);
    //   fRun->AddTask(stsClusterFinder);
    // -------------------------------------------------------------------------


    // -----   STS hit finder   ------------------------------------------------
    FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
    //   fRun->AddTask(stsFindHits);
    // -------------------------------------------------------------------------


    // -----  STS hit matching   -----------------------------------------------
    FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
    //   fRun->AddTask(stsMatchHits);
    // -------------------------------------------------------------------------


    // ---  STS track finding   ------------------------------------------------
    CbmKF* kalman = new CbmKF();
    //   fRun->AddTask(kalman);
    CbmL1* l1 = new CbmL1();
    //   fRun->AddTask(l1);
    CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
    FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
    //   fRun->AddTask(stsFindTracks);
    // -------------------------------------------------------------------------


    // ---   STS track matching   ----------------------------------------------
    FairTask* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
    //   fRun->AddTask(stsMatchTracks);
    // -------------------------------------------------------------------------


    // ---   STS track fitting   -----------------------------------------------
    CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
    FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
    //   fRun->AddTask(stsFitTracks);
    // -------------------------------------------------------------------------

    // ===                 End of STS local reconstruction                   ===
    // =========================================================================


    // ======================================================  ===================
    // ===                     PSD Digitization                      ===
    // =========================================================================

    CbmPsdIdealDigitizer *psddigi = new CbmPsdIdealDigitizer();
    //   fRun->AddTask(psddigi);

    CbmPsdHitProducer *psdhit = new CbmPsdHitProducer();
    //   fRun->AddTask(psdhit);
    CbmPsdReactionPlaneMaker *psdrp = new CbmPsdReactionPlaneMaker();
    //   fRun->AddTask(psdrp);


    //SM --->

    //Temporary flag to change reconstruction chain between standard and RUN-1
    const Bool_t kRUN1 = kFALSE;

    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //

    if (kRUN1) {
        BmnMwpcDigitizer* mwpcDigit1 = new BmnMwpcDigitizer(1);
        fRun->AddTask(mwpcDigit1);
        BmnMwpcDigitizer* mwpcDigit2 = new BmnMwpcDigitizer(2);
        fRun->AddTask(mwpcDigit2);
        BmnMwpcDigitizer* mwpcDigit3 = new BmnMwpcDigitizer(3);
        fRun->AddTask(mwpcDigit3);

        //    BmnMwpcHitFinder* mwpcHF1 = new BmnMwpcHitFinder(1);
        //    fRun->AddTask(mwpcHF1);
        //    BmnMwpcHitFinder* mwpcHF2 = new BmnMwpcHitFinder(2);
        //    fRun->AddTask(mwpcHF2);
        //    BmnMwpcHitFinder* mwpcHF3 = new BmnMwpcHitFinder(3);
        //    fRun->AddTask(mwpcHF3);

        BmnMwpcHitProducer* mwpcHP1 = new BmnMwpcHitProducer(1);
        fRun->AddTask(mwpcHP1);
        BmnMwpcHitProducer* mwpcHP2 = new BmnMwpcHitProducer(2);
        fRun->AddTask(mwpcHP2);
        BmnMwpcHitProducer* mwpcHP3 = new BmnMwpcHitProducer(3);
        fRun->AddTask(mwpcHP3);
    }

    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //

    BmnGemHitProducer* gemHP = new BmnGemHitProducer();
//    gemHP->SetOnlyPrimary(kTRUE);
    fRun->AddTask(gemHP);

      //BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
      //fRun->AddTask(gemDigit);

      //BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker();
      //fRun->AddTask(gemHM);

    // ====================================================================== //
    // ===                           TOF1 hit finder                      === //
    // ====================================================================== //

    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer();
    //tof1HP->SetOnlyPrimary(kTRUE);
    fRun->AddTask(tof1HP);

    // ====================================================================== //
    // ===                           DCH1 hit finder                      === //
    // ====================================================================== //

    BmnDchHitProducer* dch1HP = new BmnDchHitProducer(1);
    //dch1HP->SetOnlyPrimary(kTRUE);
    fRun->AddTask(dch1HP);

    // ====================================================================== //
    // ===                          DCH2 hit finder                       === //
    // ====================================================================== //

    BmnDchHitProducer* dch2HP = new BmnDchHitProducer(2);
    //dch2HP->SetOnlyPrimary(kTRUE);
    fRun->AddTask(dch2HP);

    // ====================================================================== //
    // ===                           TOF2 hit finder                      === //
    // ====================================================================== //

    CbmTofHitProducer* tof2HP = new CbmTofHitProducer("TOF HitProducer", iVerbose);
    tof2HP->SetZposition(700.);
    tof2HP->SetXshift(32.);
    fRun->AddTask(tof2HP);

    // ====================================================================== //
    // ===                           Tracking                             === //
    // ====================================================================== //

    if (kRUN1) {

        //TODO: fix this branch!!!
        BmnSeedFinder* SF = new BmnSeedFinder();
        SF->SetMakeQA(kFALSE);
        SF->SetRun1(kRUN1);
        fRun->AddTask(SF);

    } else {

        BmnGemSeedFinder* gemSF = new BmnGemSeedFinder();
        gemSF->SetMakeQA(kFALSE);
        fRun->AddTask(gemSF);

        BmnGemTrackFinder* gemTF = new BmnGemTrackFinder();
        fRun->AddTask(gemTF);
    }

    BmnGlobalTracking* glFinder = new BmnGlobalTracking();
    glFinder->SetRun1(kRUN1);
    glFinder->SetMakeQA(kFALSE);
    fRun->AddTask(glFinder);

    // <--- SM

    // -----   Primary vertex finding   ---------------------------------------
    CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
    CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
    //  fRun->AddTask(findVertex);
    // ------------------------------------------------------------------------

    // ====================================================================== //
    // ===                         End of tracking                        === //
    // ====================================================================== //


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
    //  delete fRun;

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
