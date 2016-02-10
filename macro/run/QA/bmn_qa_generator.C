// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------

void bmn_qa_generator(TString inFile = "$VMCWORKDIR/macro/run/bmndst.root", TString outFile = "qa.root", Int_t nStartEvent = 0, Bool_t isPrimary = kTRUE, Int_t nEvents = 1000000)
{

  // Parameter file
  TString parFile = inFile;

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs();           // load bmn libraries

  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile(inFile);
  fRun->AddFriend("$VMCWORKDIR/macro/run/evetest.root");
//  fRun->AddFriend("$VMCWORKDIR/macro/run/evetest100urqmd.root");
  fRun->SetOutputFile(outFile);
  fRun->SetGeomFile("$VMCWORKDIR/macro/run/geofile_full.root");
  fRun->SetWriteRunInfoFile(false);

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
  
  // -----  Parameter database   --------------------------------------------
//  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
//  FairParRootFileIo* parIo1 = new FairParRootFileIo();
//  parIo1->open(parFile.Data());
//  rtdb->setFirstInput(parIo1);
//  rtdb->setOutput(parIo1);
//  rtdb->saveOutput();
  // ------------------------------------------------------------------------
  
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
