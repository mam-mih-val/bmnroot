// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------

void bmn_qa_generator(TString recoFile = "$VMCWORKDIR/macro/run/bmndst.root", TString mcFile = "$VMCWORKDIR/macro/run/evetest.root", TString outFile = "qa.root", Int_t nStartEvent = 0, Bool_t isPrimary = kFALSE, Int_t nEvents = 1000000)
{

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs();           // load bmn libraries

  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(recoFile);
  fRun->AddFriend(mcFile);
  fRun->SetOutputFile(outFile);
  fRun->SetGeomFile("$VMCWORKDIR/macro/run/geofile_full.root");
  fRun->SetGenerateRunInfo(false);

  // ============ TASKS ============= //
  
  BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
  fRun->AddTask(mcMatching);
    
  BmnClusteringQa* clQa = new BmnClusteringQa();
  clQa->SetOnlyPrimes(isPrimary);
  fRun->AddTask(clQa);  
    
  BmnTrackingQa* trQa = new BmnTrackingQa();
  trQa->SetOnlyPrimes(isPrimary);
  fRun->AddTask(trQa);
  
  // ============ TASKS ============= //
  
  // -----   Intialise and run   --------------------------------------------
  fRun->Init();
  cout << "Starting run" << endl;
  fRun->Run(nStartEvent, nStartEvent+nEvents);
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
}
