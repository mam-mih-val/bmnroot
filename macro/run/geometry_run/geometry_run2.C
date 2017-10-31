// construct detector geometry
void geometry(FairRunSim *fRun)
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
    FairDetector* mwpc1 = new BmnMWPC1("MWPC1", kTRUE);
    mwpc1->SetGeometryFileName("mwpc1_v1_run2.root");
    fRun->AddModule(mwpc1);

    FairDetector* mwpc2 = new BmnMWPC2("MWPC2", kTRUE);
    mwpc2->SetGeometryFileName("mwpc2_v1_run2.root");
    fRun->AddModule(mwpc2);

    FairDetector* mwpc3 = new BmnMWPC3("MWPC3", kTRUE);
    mwpc3->SetGeometryFileName("mwpc3_v1_run2.root");
    fRun->AddModule(mwpc3);

    FairDetector* dch1 = new BmnDch1("DCH1", kTRUE);
    dch1->SetGeometryFileName("dch1_v1_run2.root");
    fRun->AddModule(dch1);

    FairDetector* dch2 = new BmnDch2("DCH2", kTRUE);
    dch2->SetGeometryFileName("dch2_v1_run2.root");
    fRun->AddModule(dch2);
}
