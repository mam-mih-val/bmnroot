// construct detector geometry
geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    FairModule* pipe = new FairPipe("PIPE");
    pipe->SetGeometryFileName("pipe_Be_kompozit_gap75cm_3.geo");
    fRun->AddModule(pipe);

    FairModule* target = new FairTarget("Target");
    target->SetGeometryFileName("target_au_250mu.geo");
    fRun->AddModule(target);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    // -----   Create detectors        -------------------------
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName("GEMS_RunWinter2016.root");
    fRun->AddModule(sts);
}
