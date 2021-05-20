
void DSTConv(
        TString inFile = "~/filesbmn/4649-cbm-full.root",
//        TString inFile = "~/filesbmn/4649_ren.root",
        TString outFile = "$VMCWORKDIR/macro/run/dst-bmn-4649.root",
        Int_t nStartEvent = 0,
        Int_t nEvents = 00,
        TString inFileToF400 = "") {
    TStopwatch timer;
    timer.Start();
    gDebug = 0;
    Int_t fVerbose = 2;
    Int_t periodId = 7;
    Int_t runId = -1;
    // if nEvents is equal 0 then all events of the given file starting with "nStartEvent" should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumROOTEvents((char*)inFile.Data()) - nStartEvent;

   
    FairRunAna* fRunAna = new FairRunAna();
//    FairSource* src = new BmnFileSource(inFile);
//    fRunAna->SetSource(src);
    fRunAna->SetInputFile(inFile);
    if (inFileToF400.Length() > 0)
        fRunAna->AddFriend(inFileToF400);
    fRunAna->SetOutputFile(outFile.Data());
    fRunAna->SetGenerateRunInfo(kFALSE);
    
    BmnTrackConv * conv = new BmnTrackConv(periodId, runId, kBMNSETUP);
    fRunAna->AddTask(conv);
    
    
    fRunAna->Init();
    fRunAna->Run(nStartEvent, nStartEvent + nEvents);

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
}
