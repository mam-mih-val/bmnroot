/* Copyright (C) name="CpuLoad" CBM Collaboration, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Viktor Klochkov */

void run_analysis_tree_maker(std::string dst_file, std::string geant_file, std::string geometry_file, std::string output_file, double sqrt_snn)
{
  const std::string system = "Xe+Cs";  // TODO can we read it automatically?
//  const float sqrt_snn     = 3.0;
  const float beam_mom     = 4.85;
  const bool is_event_base = false;

  // --- Logger settings ----------------------------------------------------
  const TString logLevel     = "INFO";
  const TString logVerbosity = "LOW";
  // ------------------------------------------------------------------------

  // -----   Environment   --------------------------------------------------
  const TString myName = "run_analysis_tree_maker";
  const TString srcDir = gSystem->Getenv("VMCWORKDIR");  // top source directory
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  std::cout << "-I- " << myName << ": Using reco file " << dst_file << std::endl;
  // -----   Reconstruction run   -------------------------------------------
  auto* run         = new FairRunAna();
  auto* inputSource = new FairFileSource(dst_file);
  inputSource->AddFriend(geant_file);
  run->SetSource(inputSource);
  run->SetOutputFile(output_file.c_str());
  // ------------------------------------------------------------------------
  // AnalysisTree converter
  auto* man = new BmnConverterManager();
  man->SetSystem(system);
  man->SetSqrtSnn(sqrt_snn);
  man->SetOutputName(output_file, "rTree");
  man->SetGeometryFile(geometry_file);

  man->AddTask(new BmnSimEventHeaderConverter("SimEventHeader"));
  man->AddTask(new BmnRecEventHeaderConverter("RecEventHeader"));
  man->AddTask(new BmnFHCalModulesConverter("FHCalModules"));
  man->AddTask(new BmnStsTracksConverter("StsTracks"));

  man->AddTask(new BmnTofHitsConverter("Tof400Hits", BMNTOF::TOF400));
  man->AddTask(new BmnTofHitsConverter("Tof700Hits", BMNTOF::TOF700));

  auto* taskBmnGlobalTracksConverter = new BmnGlobalTracksConverter("GlobalTracks", "StsTracks", "Tof400Hits", "Tof700Hits");
  man->AddTask(taskBmnGlobalTracksConverter);
  man->AddTask(new BmnSimParticlesConverter("SimParticles", "GlobalTracks", "StsTracks"));

  run->AddTask(man);

  run->Init();

  std::cout << "Starting run" << std::endl;
  run->Run(0);
  // ------------------------------------------------------------------------

  timer.Stop();
  const Double_t rtime = timer.RealTime();
  const Double_t ctime = timer.CpuTime();
  std::cout << "Macro finished succesfully." << std::endl;
  std::cout << "Output file is " << output_file << std::endl;

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
