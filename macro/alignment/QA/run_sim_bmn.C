#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
#include "macro/alignment/QA/geometry.C"

void run_sim_bmn(TString inFile = "", TString outFile = "evetest_RUN7_GEM.root", Int_t nStartEvent = 0, Int_t nEvents = 1000,
		 Bool_t flag_store_FairRadLenPoint = kFALSE, Bool_t isFieldMap = kTRUE) {
  TStopwatch timer;
  timer.Start();
  gDebug = 0;

  bmnloadlibs(); // load libraries

  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();

  fRun->SetName("TGeant3");

  geometry(fRun); // load bmn geometry
 
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);

  gRandom->SetSeed(0);
  FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 10); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(1., 1.);  // GeV/c, setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen->SetThetaRange(10, 15); // Polar angle in lab system range [degree]
  boxGen->SetXYZ(0.5, -4.6, -2.3); // Approximate position of target (RunSpring2017)
  primGen->AddGenerator(boxGen);

  fRun->SetOutputFile(outFile.Data());

  // -----   Create magnetic field   ----------------------------------------
  BmnFieldMap* magField = NULL;
  if (isFieldMap) {
    Double_t fieldScale = 0.;
    // magField = new BmnNewFieldMap("field_sp41v2_ascii_noExtrap.dat");
    magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    // Double_t fieldZ = 124.5; // field centre z position
    // magField->SetPosition(0., 0., fieldZ);
    magField->SetScale(fieldScale);
    fRun->SetField(magField);
  } else {
    BmnFieldConst* magField = new BmnFieldConst();
    magField->SetFieldRegion(-300., 300., -300., 300., -300., 300);
    magField->SetField(0., -9. * 0.44, 0.);
    fRun->SetField(magField);
  }

  fRun->SetStoreTraj(kTRUE);
  fRun->SetRadLenRegister(flag_store_FairRadLenPoint); // radiation length manager

  BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSpring2018_misAlign;
  BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
  gemDigit->SetCurrentConfig(gem_config);
  gemDigit->SetOnlyPrimary(kFALSE);
  gemDigit->SetStripMatching(kTRUE);
  fRun->AddTask(gemDigit);

  fRun->Init();
  if (isFieldMap)
    magField->Print();

  // Trajectories Visualization (TGeoManager only)
  FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // Set cuts for storing the trajectories
  trajFilter->SetStepSizeCut(0.01); // 1 cm
  trajFilter->SetVertexCut(-200., -200., -150., 200., 200., 1100.);
  trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  trajFilter->SetEnergyCut(0., 4.); // 0 < Etot < 1.04 GeV //
  trajFilter->SetStorePrimaries(kTRUE);
  trajFilter->SetStoreSecondaries(kTRUE); //kFALSE

  // Fill the Parameter containers for this run
  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();

  BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(), 1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
  //AZ output->open(parFile.Data());
  output->open(gFile);
  rtdb->setOutput(output);

  rtdb->saveOutput();
  rtdb->print();

  // Transport nEvents
  fRun->Run(nEvents);

  fRun->CreateGeometryFile("geofile_full.root");  // save the result setup geometry to the additional file

  timer.Stop();
  Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
  cout << "Macro finished successfully." << endl; // marker of successfully execution for CDASH
}
