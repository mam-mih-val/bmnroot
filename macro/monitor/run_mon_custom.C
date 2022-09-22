
R__ADD_INCLUDE_PATH($VMCWORKDIR)
        
#define L1Tracking // Choose Tracking: L1, VF or CellAuto
        
void run_mon_custom(TString inputFileName = "/ncx/eos/nica/bmn/exp/dst/run7/prerelease/3590-4707_BMN_Argon/bmn_run4649_dst.root",
        TString bmndstFileName = "$VMCWORKDIR/macro/monitor/qa-dst.root",
        TString padConfName = "pad-sample.json",
        Int_t nStartEvent = 0, Int_t nEvents = 0)
{
    gDebug = 0; // Debug option
    // Verbosity level (0 = quiet (progress bar), 1 = event-level, 2 = track-level, 3 = full debug)
    Int_t iVerbose = 0;
    Int_t period = 7;
    Bool_t isPrimary = kFALSE;
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    
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
//    BmnMQSource* source= new BmnMQSource("tcp://localhost:6666", kTRUE);
    BmnFileSource* source = new BmnFileSource(inputFileName);
    fRunAna->SetSource(source);

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(bmndstFileName, 1) < 0) exit(-2);
    fRunAna->SetSink(new FairRootFileSink(bmndstFileName));
//    fRunAna->SetSink(new BmnMQSink());
//    fRunAna->SetGenerateRunInfo(false);
    
    BmnCustomQa* trQaAll = new BmnCustomQa("custom_qa", string(padConfName.Data()));
    trQaAll->SetMonitorMode(kTRUE);
    THttpServer* fServer = new THttpServer("fastcgi:8081;noglobal;cors");
    fServer->SetTimer(50, kTRUE);
    trQaAll->SetObjServer(fServer);
//    source->SetObjServer(fServer);
    fRunAna->AddTask(trQaAll);
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
    cout << "Starting run" << endl;
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inputFileName.Data()) - nStartEvent;
    if 
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
