
R__ADD_INCLUDE_PATH($VMCWORKDIR)
        
#define L1Tracking // Choose Tracking: L1, VF or CellAuto
        
void run_mon_reco(TString inputFileName = "/ncx/eos/nica/bmn/exp/dst/run7/prerelease/3590-4707_BMN_Argon/bmn_run4649_dst.root",
        TString bmndstFileName = "$VMCWORKDIR/macro/monitor/bmndst.root",
        Int_t nStartEvent = -1, Int_t nEvents = -1)
{
    gDebug = 0; // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    Int_t period = 7;
    Bool_t isPrimary = kFALSE;
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    TString confGem = gPathGemConfig + ((period == 8) ? "GemRun8.xml" : (period == 7) ? "GemRunSpring2018.xml" : (period == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml");
    TString confSil = gPathSilConfig + ((period == 8) ? "SiliconRun8_3stations.xml" : (period == 7) ? "SiliconRunSpring2018.xml" : (period == 6) ? "SiliconRunSpring2017.xml" : "SiliconRunSpring2017.xml");
    TString confCSC = gPathCscConfig + ((period == 8) ? "CSCRun8.xml" : "");
    
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    // check input file exists
    if (!BmnFunctionSet::CheckFileExist(inputFileName, 1)) exit(-1);

    // -----   Reconstruction run   --------------------------------------------
//    FairRunAna* fRunAna = new FairRunAna();
    FairRunOnline* fRunAna = new FairRunOnline();
//    fRunAna->SetEventHeader(new DstEventHeader());

    // Declare input source as simulation file or experimental data
    BmnMQSource* source= new BmnMQSource("tcp://localhost:6666", kTRUE);
//    BmnFileSource* source = new BmnFileSource(inputFileName);
    fRunAna->SetSource(source);
    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inputFileName.Data()) - nStartEvent;

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(bmndstFileName, 1) < 0) exit(-2);
    fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
//    fRunAna->SetSink(new BmnMQSink());
//    fRunAna->SetGenerateRunInfo(false);
    
//    BmnQaOffline* qaSystem = new BmnQaOffline("");
//    fRunAna->AddTask(qaSystem);
    
    BmnTrackingQaExp* trQaAll = new BmnTrackingQaExp(0, "tracking_qa", confGem, confSil, confCSC);
    trQaAll->SetDetectorPresence(kSILICON, kTRUE);
    trQaAll->SetDetectorPresence(kSSD, kFALSE);
    trQaAll->SetDetectorPresence(kGEM, kTRUE);
    trQaAll->SetOnlyPrimes(isPrimary);
    trQaAll->SetMonitorMode(kTRUE);
    THttpServer* fServer = new THttpServer("fastcgi:8081;noglobal;cors");
    fServer->SetTimer(50, kTRUE);
    trQaAll->SetObjServer(fServer);
    fMqSource->SetObjServer(fServer);
    TString innerTrackBranchName; //use different track container
#ifdef L1Tracking
    innerTrackBranchName = "StsTrack";
#else
    innerTrackBranchName = "StsVector";
#endif
    trQaAll->SetInnerTracksBranchName(innerTrackBranchName);
    fRunAna->AddTask(trQaAll);
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
