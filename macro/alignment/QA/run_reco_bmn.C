#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void run_reco_bmn(TString inputFileName = "evetest_RUN7_GEM.root",
		  TString bmndstFileName = "bmndst_RUN7_GEM.root",
		  Int_t nStartEvent = 0,
		  Int_t nEvents = 1000,
		  TString alignCorrFileName = "align_start_null.root",
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

  Bool_t isField = kFALSE; // flag for tracking (to use mag.field or not)
  Bool_t isTarget = kTRUE; //kFALSE; // flag for tracking (run with target or not)
 
  // Declare input source as simulation file or experimental data
  FairSource* fFileSource;

  Int_t run_period = 7;
  Double_t fieldScale = 0.;
  Bool_t isExp = kTRUE;
  TVector3 vertex = TVector3(0.5, -4.6, -2.3);
    
  if (!CheckFileExist(inputFileName))
    return;
  fFileSource = new FairFileSource(inputFileName);
    
  fRunAna->SetSource(fFileSource);
  fRunAna->SetOutputFile(bmndstFileName);
  fRunAna->SetGenerateRunInfo(false);

  if (iVerbose == 0) { // print only progress bar in terminal in quiet mode
    BmnCounter* cntr = new BmnCounter(nEvents);
    fRunAna->AddTask(cntr);
  }
    
  // ====================================================================== //
  // ===                         GEM hit finder                         === //
  // ====================================================================== //
  BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSpring2018;
  BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(run_period, isExp);
  gemHM->SetCurrentConfig(gem_config);
  gemHM->SetHitMatching(kTRUE);
  fRunAna->AddTask(gemHM);

  // ====================================================================== //
  // ===                           ALIGNMENT (GEM)                      === //
  // ====================================================================== //
  // if (iterNum > 0)
    gemHM->SetAlignmentCorrectionsFileName(alignCorrFileName);
  
  // ====================================================================== //
  // ===                           Tracking (GEM)                       === //
  // ====================================================================== //
  BmnGemTracking* gemTF = new BmnGemTracking(run_period, isField, isTarget, steerGemTrackingFile);
  gemTF->SetRoughVertex(vertex);
  fRunAna->AddTask(gemTF);

  // ====================================================================== //
  // ===                          Global Tracking                       === //
  // ====================================================================== //
  BmnGlobalTracking* globalTF = new BmnGlobalTracking(isExp);
  globalTF->SetField(isField);
  fRunAna->AddTask(globalTF);

  // ====================================================================== //
  // ===                     Primary vertex finding                     === //
  // ====================================================================== //
  BmnVertexFinder* gemVF = new BmnVertexFinder(run_period);
  gemVF->SetVertexApproximation(vertex);
  fRunAna->AddTask(gemVF);

  // Residual analysis
  BmnGemResiduals* residAnalGem = new BmnGemResiduals(run_period);
  fRunAna->AddTask(residAnalGem);
   
  // -----   Parameter database   --------------------------------------------
  FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
  FairParRootFileIo* parIo1 = new FairParRootFileIo();
  FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
  parIo1->open(inputFileName.Data());
  //parIo2->open(parFileNameList, "in");
  rtdb->setFirstInput(parIo1);
  //rtdb->setSecondInput(parIo2);
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
