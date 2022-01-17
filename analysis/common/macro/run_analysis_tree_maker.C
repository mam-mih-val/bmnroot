/* Copyright (C) name="CpuLoad" CBM Collaboration, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Viktor Klochkov */

void run_analysis_tree_maker(TString dataSet = "data_test/", TString setupName = "sis100_electron",
                             TString unigenFile = "")
{
  const std::string system = "Au+Au";  // TODO can we read it automatically?
  const float beam_mom     = 12.;
  const bool is_event_base = false;

  // --- Logger settings ----------------------------------------------------
  const TString logLevel     = "INFO";
  const TString logVerbosity = "LOW";
  // ------------------------------------------------------------------------

  // -----   Environment   --------------------------------------------------
  const TString myName = "run_analysis_tree_maker";
  const TString srcDir = gSystem->Getenv("VMCWORKDIR");  // top source directory
  // ------------------------------------------------------------------------

  // -----   In- and output file names   ------------------------------------
  TString traFile           = dataSet + "sim/bmnsim_3731428_2.root";
//   TString rawFile           = dataSet + ".raw.root";
  TString recFile           = dataSet + "reco/bmndst_3731428_2.root";
//   TString geoFile           = dataSet + ".geo.root";
//   TString parFile           = dataSet + ".par.root";
  const std::string outFile = dataSet.Data() + std::string("analysistree.root");
//   if (unigenFile.Length() == 0) { unigenFile = srcDir + "/input/urqmd.auau.10gev.centr.root"; }
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // -----   Remove old CTest runtime dependency file  ----------------------
  const TString dataDir  = gSystem->DirName(dataSet);
  const TString dataName = gSystem->BaseName(dataSet);
  const TString testName = ("run_treemaker");
  // ------------------------------------------------------------------------

//  // -----   Load the geometry setup   -------------------------------------
//  std::cout << std::endl;
//  const TString setupFile  = srcDir + "/geometry/setup/setup_" + setupName + ".C";
//  const TString setupFunct = "setup_" + setupName + "()";
//
//  std::cout << "-I- " << myName << ": Loading macro " << setupFile << std::endl;


//  gROOT->LoadMacro(setupFile);
//  gROOT->ProcessLine(setupFunct);
//  CbmSetup* setup = CbmSetup::Instance();

//  TString geoTag;
//  auto* parFileList = new TList();

//   std::cout << "-I- " << myName << ": Using raw file " << rawFile << std::endl;
//   std::cout << "-I- " << myName << ": Using parameter file " << parFile << std::endl;
  std::cout << "-I- " << myName << ": Using reco file " << recFile << std::endl;
//   if (unigenFile.Length() > 0) std::cout << "-I- " << myName << ": Using unigen file " << unigenFile << std::endl;

  // -----   Reconstruction run   -------------------------------------------
  auto* run         = new FairRunAna();
  auto* inputSource = new FairFileSource(recFile);
  inputSource->AddFriend(traFile);
//   inputSource->AddFriend(rawFile);
  run->SetSource(inputSource);
//   run->SetOutputFile(outFile.c_str());
//   run->SetGenerateRunInfo(kTRUE);
  // ------------------------------------------------------------------------

  // AnalysisTree converter
  auto* man = new CbmConverterManager();
  man->SetSystem(system);
  man->SetBeamMomentum(beam_mom);
  man->SetOutputName(outFile, "rTree");

  man->AddTask(new CbmSimEventHeaderConverter("SimEventHeader"));
  man->AddTask(new CbmRecEventHeaderConverter("RecEventHeader"));
  man->AddTask(new CbmSimTracksConverter("SimParticles"));

  CbmStsTracksConverter* taskCbmStsTracksConverter = new CbmStsTracksConverter("VtxTracks", "SimParticles");
  taskCbmStsTracksConverter->SetIsWriteKFInfo();
  taskCbmStsTracksConverter->SetIsReproduceCbmKFPF();
  man->AddTask(taskCbmStsTracksConverter);
//
//  man->AddTask(new CbmRichRingsConverter("RichRings", "VtxTracks"));
//  man->AddTask(new CbmTofHitsConverter("TofHits", "VtxTracks"));
//  man->AddTask(new CbmTrdTracksConverter("TrdTracks", "VtxTracks"));
//  if(is_event_base){
//    man->AddTask(new CbmPsdModulesConverter("PsdModules"));
//  }
  run->AddTask(man);

// -----   Intialise and run   --------------------------------------------
  run->Init();

  std::cout << "Starting run" << std::endl;
  run->Run(0);
  // ------------------------------------------------------------------------

  timer.Stop();
  const Double_t rtime = timer.RealTime();
  const Double_t ctime = timer.CpuTime();
  std::cout << "Macro finished succesfully." << std::endl;
  std::cout << "Output file is " << outFile << std::endl;

  printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);

  // -----   CTest resource monitoring   ------------------------------------
  FairSystemInfo sysInfo;
  const Float_t maxMemory = sysInfo.GetMaxMemory();
  std::cout << R"(<DartMeasurement name="MaxMemory" type="numeric/double">)";
  std::cout << maxMemory;
  std::cout << "</DartMeasurement>" << std::endl;
  std::cout << R"(<DartMeasurement name="WallTime" type="numeric/double">)";
  std::cout << rtime;
  std::cout << "</DartMeasurement>" << std::endl;
  const Float_t cpuUsage = ctime / rtime;
  std::cout << R"(<DartMeasurement name="CpuLoad" type="numeric/double">)";
  std::cout << cpuUsage;
  std::cout << "</DartMeasurement>" << std::endl;
  // ------------------------------------------------------------------------

  // -----   Finish   -------------------------------------------------------
  std::cout << " Test passed" << std::endl;
  std::cout << " All ok " << std::endl;
  //   Generate_CTest_Dependency_File(depFile);
  // ------------------------------------------------------------------------

//  RemoveGeoManager();
}
