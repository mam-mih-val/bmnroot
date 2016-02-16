geometry(FairRunSim *fRun)
{    
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    FairModule* target = new FairTarget("Target");
    target->SetGeometryFileName("target_au_250mu.geo");
    fRun->AddModule(target);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);
    
    FairDetector* ecal = new BmnEcal("ECAL", kTRUE);
    ecal->SetGeometryFileName("ecal_v1_0.geo");
    fRun->AddModule(ecal);
}
