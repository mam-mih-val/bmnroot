// --------------------------------------------------------------------------
//
// Macro for standard transport simulation using UrQMD input and GEANT3
// Standard CBM setup with MVD, STS, RICH, TRD, TOF and ECAL
//
// V. Friese   22/02/2007
//
// --------------------------------------------------------------------------
#include <TGDMLParse.h>

void run_sim(Int_t nEvents = 2)
{


gRandom->SetSeed(1);
  // ========================================================================
  //          Adjust this part according to your requirements

  // ----- Paths and file names  --------------------------------------------
  //TString inDir   = gSystem->Getenv("VMCWORKDIR");
  TString inDir = "/home/zinchenk/bmon/urqmd/";
  //TString inFile  = inDir + "/AuAu_urqmd_3.3p1_4.0_gev__10000_ev__mbias__seed_0.ftn14";
  TString inFile  = inDir + "auau.ek45gev.0_3fm.10k.0012.f14.gz";
  //TString inFile  = inDir + "CC/cc.ek45gev.0_3fm.10k.0001.f14.gz";
  //TString inFile = "urqmd_4_mb.f14.gz";
  TString outDir  = "./";
  TString outFile = outDir + "/mc.root";
  TString parFile = outDir + "/AuAu_urqmd_3.3p1_4.0_gev__10000_ev__mbias__seed_0_params.root";
  
  
  
  
  // -----  Geometries  -----------------------------------------------------
  TString caveGeom   = "cave.geo";
  TString targetGeom = "target_au_250mu.geo";
  //TString pipeGeom   = "pipe_Be_kompozit_gap75cm_3.geo";
  TString pipeGeom   = "";
  //TString magnetGeom = "";
  TString magnetGeom = "magnet_modified.root";
  //TString magnetGeom = "magnet_SP41mod_v1245.geo";
  //TString stsGeom    = "vv/gem_bmn_40_VV.geo";  
  //TString stsGeom    = "gem_bmn_40_3st.geo";  
  //TString stsGeom    = "gems_1st_80.geo";  
  //TString stsGeom    = "gems_var_fr.geo";  
  //TString stsGeom    = "GEMS_v2_AZ.root";  
  //TString stsGeom    = "GEMS_v2_hf.root";  
  //TString stsGeom    = "GEMS_v15_AZ.root";  
  TString stsGeom    = "GEMS_v17_AZ.root";  
  //TString stsGeom    = "GEMS_v17_SI.root";  
  //TString stsGeom    = "GEMS_v2.root";  
  //TString stsGeom    = "mwpc.geo";  
  //TString stsGeom    = "";  
  //TString tofGeom    = "vv/ecal.geo";
  //TString tofGeom    = "tof_4_7.geo";
  TString tofGeom    = "";

  
  // -----   Magnetic field   -----------------------------------------------
  TString fieldMap    = "field_sp41v1";   // name of field map
  Double_t fieldZ     = 124.5;             // field centre z position
  //Double_t fieldZ     = 165.;             // field centre z position
  //Double_t fieldScale =  0.88;             // field scaling factor
  Double_t fieldScale =  0.44;             // field scaling factor
  
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
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libFairTools");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");  
  //gSystem->Load("libMCStack");
  //gSystem->Load("libCbmBase");
  //gSystem->Load("libCbmData");
  gSystem->Load("libBmnData");
  //gSystem->Load("libCbmGenerators");
  //gSystem->Load("libField");
  gSystem->Load("libBmnField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  //gSystem->Load("libEcal");
  //gSystem->Load("libKF");
  //gSystem->Load("libMvd");
  gSystem->Load("libSts");
  //gSystem->Load("libLittrack");
  //gSystem->Load("libRich");
  //gSystem->Load("libTrd");
  gSystem->Load("libTof");
  // -----------------------------------------------------------------------

 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3");              // Transport engine
  fRun->SetOutputFile(outFile);          // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  // ------------------------------------------------------------------------


  // -----   Create media   -------------------------------------------------
  //fRun->SetMaterials("media_FT_old.geo");       // Materials
  fRun->SetMaterials("media.geo");       // Materials
  // ------------------------------------------------------------------------


  // -----   Create detectors and passive volumes   -------------------------
  if ( caveGeom != "" ) {
    //FairModule* cave = new CbmCave("CAVE");
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
  }

  if ( pipeGeom != "" ) {
    //FairModule* pipe = new CbmPipe("PIPE");
    FairModule* pipe = new FairPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
  }
  
  if ( targetGeom != "" ) {
    //FairModule* target = new CbmTarget("Target");
    FairModule* target = new FairTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
  }

  if ( magnetGeom != "" ) {
    //FairModule* magnet = new CbmMagnet("MAGNET");
    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
  }

  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
    //sts->SetVerboseLevel(2);
  }

  if ( tofGeom != "" ) {
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
  }

/*
  if ( ecalGeom != "" ) {
    FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data()); 
    fRun->AddModule(ecal);
  }
*/  
  // ------------------------------------------------------------------------



  // -----   Create magnetic field   ----------------------------------------
  //CbmFieldMap* magField = new CbmFieldMapSym3(fieldMap);
  //BmnFieldMap* magField = new BmnFieldMapSym3("field_sp41v1_ascii", "A");
  //BmnFieldMap* magField = new BmnFieldMapSym3("field_sp41v1_ascii_noExtrap.dat");
  BmnFieldMap* magField = new BmnFieldMapSym3("field_sp41v1_ascii_Extrap.dat");
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------

  // Use theexperiment specific MC Event header instead of the default one
  // This one stores additional information about the reaction plane
  //CbmMCEventHeader* mcHeader = new CbmMCEventHeader();
  //fRun->SetMCEventHeader(mcHeader);

  // -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  fRun->SetGenerator(primGen);       
  /*
  // Use the CbmUrqmdGenrator which calculates a reaction plane and
  // rotate all particles accordingly
  //CbmUrqmdGenerator*  urqmdGen = new CbmUrqmdGenerator(inFile);
  FairUrqmdGenerator*  urqmdGen = new FairUrqmdGenerator(inFile);
  urqmdGen->SetEventPlane(0. , 360.);
  */
  MpdUrqmdGenerator*  urqmdGen = new MpdUrqmdGenerator(inFile);
  primGen->AddGenerator(urqmdGen);

  //*
  //TString laqGSM = "/home/zinchenk/laqgsm/LAQGSM_AuAu_4_mb_100K_mer.r12";
  TString laqGSM = "/home/zinchenk/laqgsm/AuAu4_5mb_1.r12";
  //MpdLAQGSMGenerator* laq = new MpdLAQGSMGenerator(laqGSM, kFALSE, 3);
  //laq->SkipEvents(20000);
  //primGen->AddGenerator(laq);
  //*/

  /*
  CbmAnaHypYPtGenerator *hypGen = new CbmAnaHypYPtGenerator(3122); // lambda
  hypGen->SetDistributionPt(0.155); // 4 GeV
  hypGen->SetDistributionY(1.186,0.645); // BMN
  //primGen->AddGenerator(hypGen);
  */

  MpdHypYPtGenerator *hypGen = new MpdHypYPtGenerator(333); // phi0
  hypGen->SetDistributionPt(0.153); //
  hypGen->SetDistributionY(1.24,0.46); // BMN
  //primGen->AddGenerator(hypGen);

  MpdHypYPtGenerator *hypGen = new MpdHypYPtGenerator(3122); // lambda
  hypGen->SetDistributionPt(0.180); // 4 GeV
  hypGen->SetDistributionY(1.22,0.50); // BMN
  //primGen->AddGenerator(hypGen); 

  MpdHypYPtGenerator *hypGen = new MpdHypYPtGenerator(3312); // Xi-
  hypGen->SetDistributionPt(0.160); // 4 GeV
  hypGen->SetDistributionY(1.22,0.43); // BMN
  //primGen->AddGenerator(hypGen);

  MpdHypYPtGenerator *hypGen = new MpdHypYPtGenerator(321,10); // K+
  hypGen->SetDistributionPt(0.158); // exper.
  hypGen->SetDistributionY(1.17,0.7); // BMN
  //primGen->AddGenerator(hypGen);

  //FairBoxGenerator* boxGen = new FairBoxGenerator(-13, 1); // 13 = muon; 1 = multipl.
  FairBoxGenerator* boxGen = new FairBoxGenerator(-211, 5); // 13 = muon; 1 = multipl.
  boxGen->SetPRange(0.1,7.0); // GeV/c //setPRange vs setPtRange
  boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
  //boxGen->SetThetaRange(10.0, 30); // Polar angle in lab system range [degree]
  boxGen->SetThetaRange(0.1, 60); // Polar angle in lab system range [degree]
  //boxGen->SetEtaRange(-1.3, 1.3); // Eta
  boxGen->SetXYZ(0., 0., 0.); // mm o cm ??
  //primGen->AddGenerator(boxGen);

  //FairBoxGenerator* boxGen1 = new FairBoxGenerator(13, 1); // 13 = muon; 1 = multipl.
  FairBoxGenerator* boxGen1 = new FairBoxGenerator(211, 1); // 13 = muon; 1 = multipl.
  boxGen1->SetPRange(0.1,7.0); // GeV/c //setPRange vs setPtRange
  boxGen1->SetPhiRange(0, 360); // Azimuth angle range [degree]
  boxGen1->SetThetaRange(0.1, 60); // Polar angle in lab system range [degree]
  //boxGen1->SetThetaRange(0., 0); // Polar angle in lab system range [degree]
  //boxGen1->SetEtaRange(-1.3, 1.3); // Eta
  boxGen1->SetXYZ(0., 0., 0.); // mm o cm ??
  //primGen->AddGenerator(boxGen1);

  // ------------------------------------------------------------------------

 
  // -Trajectories Visualization (TGeoManager Only )
  // Switch this on if you want to visualize tracks in the
  // eventdisplay.
  // This is normally switch off, because of the huge files created
  // when it is switched on. 
  // fRun->SetStoreTraj(kTRUE);

  // -----   Run initialisation   -------------------------------------------
  fRun->Init();

  // Add user decays 
  //gROOT->LoadMacro("./UserDecay.C");
  //UserDecay();

  // ------------------------------------------------------------------------
  
  // Set cuts for storing the trajectories.
  // Switch this on only if trajectories are stored.
  // Choose this cuts according to your needs, but be aware
  // that the file size of the output file depends on these cuts

  // FairTrajFilter* trajFilter = FairTrajFilter::Instance();
  // trajFilter->SetStepSizeCut(0.01); // 1 cm
  // trajFilter->SetVertexCut(-2000., -2000., 4., 2000., 2000., 100.);
  // trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
  // trajFilter->SetEnergyCut(0., 1.02); // 0 < Etot < 1.04 GeV
  // trajFilter->SetStorePrimaries(kTRUE);
  // trajFilter->SetStoreSecondaries(kTRUE);

  // -----   Runtime database   ---------------------------------------------
  //CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  //parOut->open(parFile.Data());
  parOut->open(gFile);
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------

 
  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
  // ------------------------------------------------------------------------
  fRun->CreateGeometryFile("data/geofile_full.root");


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << outFile << endl;
  cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------

  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}

