/* Copyright (C) name="CpuLoad" CBM Collaboration, Darmstadt
   SPDX-License-Identifier: GPL-3.0-only
   Authors: Viktor Klochkov, Viktor Klochkov */

void run_analysis_tree_maker(std::string dst_file, std::string geant_file, std::string output_file)
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
  auto* man = new CbmConverterManager();
  man->SetSystem(system);
  man->SetBeamMomentum(beam_mom);
  man->SetOutputName(output_file, "rTree");

  man->AddTask(new CbmSimEventHeaderConverter("SimEventHeader"));
  man->AddTask(new CbmRecEventHeaderConverter("RecEventHeader"));
  man->AddTask(new BmnStsTracksConverter("StsTracks"));
  man->AddTask(new BmnTofHitsConverter("Tof400Hits", BMNTOF::TOF400));
  man->AddTask(new BmnTofHitsConverter("Tof700Hits", BMNTOF::TOF700));

  auto* taskCbmStsTracksConverter = new BmnGlobalTracksConverter("GlobalTracks", "StsTracks", "Tof400", "Tof700");
  man->AddTask(taskCbmStsTracksConverter);
  man->AddTask(new BmnSimParticlesConverter("SimParticles", "GlobalTracks"));

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
