// construct detector geometry
void geometry(FairRunSim *fRun) {
  // Set Material file Name
  fRun->SetMaterials("media.geo");
  
  // -----   Create passive volumes   -------------------------
  FairModule* cave = new FairCave("CAVE");
  cave->SetGeometryFileName("cave.geo");
  fRun->AddModule(cave);
  
  // FairModule* magnet = new FairMagnet("MAGNET");
  // magnet->SetGeometryFileName("magnet_modified.root");
  // fRun->AddModule(magnet);
  
  // -----   Create detectors        -------------------------
  FairDetector* tof = new BmnTOF("TOF", kTRUE);  
  
  FairDetector* sts = new CbmSts("STS", kTRUE);
  sts->SetGeometryFileName("GEMS_RunSpring2018_misAlign.root");
  fRun->AddModule(sts);
}
