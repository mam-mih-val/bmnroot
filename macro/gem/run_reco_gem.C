// --------------------------------------------------------------------------
//
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
#include <TGDMLParse.h>
#include "../run/bmnloadlibs.C"

void run_reco_gem(TString mcFile = "mc.root", Int_t nEvents = 10000) {

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
  TString inFile = /*dir +*/ mcFile; // MC transport file
  
  TString scriptFile = TString(gSystem->Getenv("MCFILE"));
  if (scriptFile != "") inFile = dir + scriptFile;
  TString parFile = inFile;

  // Output file
  TString outFile = "data/reco.root";
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

  // ----  Load libraries   --------------------------------------------------
  #if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
      gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  #endif
      bmnloadlibs(); // load BmnRoot libraries
/*
  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libFairTools");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  //gSystem->Load("libCbmBase");
  //gSystem->Load("libCbmData");
  //gSystem->Load("libField");
  gSystem->Load("libBmnData");
  gSystem->Load("libBmnField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  //gSystem->Load("libEcal");
  gSystem->Load("libKF");
  //gSystem->Load("libMvd");
  gSystem->Load("libSts");
  //gSystem->Load("libLittrack");
  //gSystem->Load("libRich");
  //gSystem->Load("libTrd");
  //gSystem->Load("libTof");
  //gSystem->Load("libGlobal"); 
  //gSystem->Load("libL1");
  gSystem->Load("libCat");
  gSystem->Load("libMinuit2"); // Nedded for rich ellipse fitter
  // ------------------------------------------------------------------------

*/
  //AC added
  FairSource* fFileSource;
  fFileSource = new BmnFileSource(inFile); 

  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *run = new FairRunAna();
  //run->SetInputFile(inFile);
  run->SetInputFile(fFileSource);
  run->SetOutputFile(outFile);
  // ------------------------------------------------------------------------



  // =========================================================================
  // ===             Detector Response Simulation (Digitiser)              ===
  // ===                          (where available)                        ===
  // =========================================================================

  // -----   STS digitizer   -------------------------------------------------
  Double_t threshold  =  4;
  Double_t noiseWidth =  0.01;
  Int_t    nofBits    = 12;
  //Double_t electronsPerAdc    =  10;
  Double_t electronsPerAdc    =  20; //AZ
  Double_t StripDeadTime = 0.1;
  Double_t minStep    =  0.00004; //AZ
  CbmStsDigitize* stsDigitize = new CbmStsDigitize("STS Digitiser", iVerbose);
  stsDigitize->SetRealisticResponse();
  stsDigitize->SetFrontThreshold (threshold);
  stsDigitize->SetBackThreshold  (threshold);
  stsDigitize->SetFrontNoiseWidth(noiseWidth);
  stsDigitize->SetBackNoiseWidth (noiseWidth);
  stsDigitize->SetFrontNofBits   (nofBits);
  stsDigitize->SetBackNofBits    (nofBits);
  stsDigitize->SetFrontNofElPerAdc(electronsPerAdc);
  stsDigitize->SetBackNofElPerAdc(electronsPerAdc);
  stsDigitize->SetStripDeadTime  (StripDeadTime);
  stsDigitize->SetStep   (minStep); //AZ
  run->AddTask(stsDigitize);
  // -------------------------------------------------------------------------


  // =========================================================================
  // ===                      STS local reconstruction                     ===
  // =========================================================================


  // -----   STS Cluster Finder   --------------------------------------------
  FairTask* stsClusterFinder = new CbmStsClusterFinder("STS Cluster Finder",iVerbose);
  run->AddTask(stsClusterFinder);
  // -------------------------------------------------------------------------


  // -----   STS hit finder   ------------------------------------------------
  FairTask* stsFindHits = new CbmStsFindHits("STS Hit Finder", iVerbose);
  run->AddTask(stsFindHits);
  // -------------------------------------------------------------------------


  // -----  STS hit matching   -----------------------------------------------
  FairTask* stsMatchHits = new CbmStsMatchHits("STS Hit Matcher", iVerbose);
  run->AddTask(stsMatchHits);
  // -------------------------------------------------------------------------


  // ---  STS track finding   ------------------------------------------------
  CbmKF* kalman = new CbmKF();
  run->AddTask(kalman);
  CbmL1* l1 = new CbmL1();
  l1->SetMaterialBudgetFileName(stsMatBudgetFile);
  run->AddTask(l1);
  CbmStsTrackFinder* stsTrackFinder = new CbmL1StsTrackFinder();
  FairTask* stsFindTracks = new CbmStsFindTracks(iVerbose, stsTrackFinder);
  run->AddTask(stsFindTracks);
  // -------------------------------------------------------------------------


  // ---   STS track matching   ----------------------------------------------
  FairTask* stsMatchTracks = new CbmStsMatchTracks(iVerbose);
  run->AddTask(stsMatchTracks);
  // -------------------------------------------------------------------------


  // ---   STS track fitting   -----------------------------------------------
///  CbmStsTrackFitter* stsTrackFitter = new CbmStsKFTrackFitter();
///  FairTask* stsFitTracks = new CbmStsFitTracks(stsTrackFitter, iVerbose);
///  run->AddTask(stsFitTracks);
  // -------------------------------------------------------------------------

  // ===                 End of STS local reconstruction                   ===
  // =========================================================================



  // =========================================================================
  // ===                        Global tracking                            ===
  // =========================================================================

///  CbmLitFindGlobalTracks* finder = new CbmLitFindGlobalTracks();
  // Tracking method to be used
  // "branch" - branching tracking
  // "nn" - nearest neighbor tracking
  // "weight" - weighting tracking
///  finder->SetTrackingType("branch");

  // Hit-to-track merger method to be used
  // "nearest_hit" - assigns nearest hit to the track
///  finder->SetMergerType("nearest_hit");

///  run->AddTask(finder);

  // -----   Primary vertex finding   ---------------------------------------
  CbmPrimaryVertexFinder* pvFinder = new CbmPVFinderKF();
  CbmFindPrimaryVertex* findVertex = new CbmFindPrimaryVertex(pvFinder);
  run->AddTask(findVertex);
  // ------------------------------------------------------------------------

  // ===                      End of global tracking                       ===
  // =========================================================================

  // ----------- TRD track Pid Like ----------------------
  // Since in the newest version of this method depends on the global
  // track the task has to move after the global tracking
// FU 08.02.12 Switch the task off since the input file needed for the new geometry has to be generated first.
//  CbmTrdSetTracksPidLike* trdSetTracksPidLikeTask =
//  		new CbmTrdSetTracksPidLike("Likelihood", "Likelihood");
//  run->AddTask(trdSetTracksPidLikeTask);
  // ----------------------------------------------------


  // -----  Parameter database   --------------------------------------------
  //  TString stsDigi = gSystem->Getenv("VMCWORKDIR");
  // stsDigi += "/parameters/sts/";
  //  stsDigi += stsDigiFile;
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
