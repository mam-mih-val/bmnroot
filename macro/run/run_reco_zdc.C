// --------------------------------------------------------------------------
//
// Macro for reconstruction of simulated events with standard settings
//
// --------------------------------------------------------------------------
#include <TGDMLParse.h>

void run_reco_zdc(TString mcFile = "evetest.root", Int_t nEvents = 1) {

  // ========================================================================
  //          Adjust this part according to your requirements

  // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
  Int_t iVerbose = 1;
  //Int_t iVerbose = 3;

  // Input file (MC events)
//  TString inFile = "data/AuAu_urqmd_3.3p1_4.0_gev__10000_ev__centr__seed_1_fscale88.mc.root";
  //TString inFile = "mc.root";

  // Parameter file
  //TString parFile = "data/AuAu_urqmd_3.3p1_4.0_gev__10000_ev__mbias__seed_0_params.root";
  TString dir = "./"; // Output directory
  TString inFile = dir + mcFile; // MC transport file
  TString scriptFile = TString(gSystem->Getenv("MCFILE"));
  if (scriptFile != "") inFile = dir + scriptFile;
  TString parFile = inFile;

  // Output file
  TString outFile = "reco_zdc.root";
  TString recoFile = TString(gSystem->Getenv("RECOFILE"));
  if (recoFile != "") outFile = dir + recoFile;

  //  Digitisation files.
  // Add TObjectString containing the different file names to
  // a TList which is passed as input to the FairParAsciiFileIo.
  // The FairParAsciiFileIo will take care to create on the fly 
  // a concatenated input parameter file which is then used during
  // the reconstruction.
  TList *parFileList = new TList();

  TString paramDir = gSystem->Getenv("VMCWORKDIR");
  paramDir += "/parameters";

  //TObjString stsDigiFile = paramDir + "/sts/sts_v12b_12344444_std.digi.par";
  TObjString stsDigiFile = paramDir + "/sts_v17a_gem.digi.par";
  //TObjString stsDigiFile = paramDir + "/sts_v15a90_gem.digi.par";
  //TObjString stsDigiFile = paramDir + "/sts_1st_30.digi.par";
  //TObjString stsDigiFile = paramDir + "/sts_var.digi.par";
  TString stsMatBudgetFile = ""; //paramDir + "/sts/sts_matbudget_v12b_12344444.root";
  //TString stsMatBudgetFile = paramDir + "/sts_matbudget_var_fr.root";
  parFileList->Add(&stsDigiFile);

  // In general, the following parts need not be touched
  // ========================================================================


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  // ----  Load libraries   -------------------------------------------------
  //  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  // basiclibs();
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load bmn libraries
  // ------------------------------------------------------------------------


  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run = new FairRunAna();
  run->SetInputFile(inFile);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------




  // =========================================================================
  // ===             Detector Response Simulation (Digitiser)              ===
  // ===                          (where available)                        ===
  // =========================================================================

 // -------------------------------------------------------------------------


  // =========================================================================
  // ===                      ZDC                                          ===
  // =========================================================================


  // -----   ZDC Digi Producer   --------------------------------------------
  FairTask* zdcDigiProducer = new BmnZdcDigiProducer("ZDC Digi Producer");
  run->AddTask(zdcDigiProducer);
  // -------------------------------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(parFile.Data());
  parIo2->open(parFileList, "in");
  rtdb->setFirstInput(parIo1);
  rtdb->setSecondInput(parIo2);
  rtdb->setOutput(parIo1);
  rtdb->saveOutput();
  // ------------------------------------------------------------------------


  // -----   Intialise and run   --------------------------------------------
  run->Init();
  cout << "Starting run" << endl;
  run->Run(0, nEvents);
  // ------------------------------------------------------------------------


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is " << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

//  delete run;

  cout << " Test passed" << endl;
	cout << " All ok " << endl;
}
