// construct detector geometry
void geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    // -----   Create detectors        -------------------------

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_FutureConfig2020_detailed.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("STS", kTRUE);
    gems->SetGeometryFileName("GEMS_FutureConfig2020_detailed.root");
    fRun->AddModule(gems);

    FairDetector* scwall = new BmnScWall("SCWALL", kTRUE);
    scwall->SetGeometryFileName("ScWall_oldnames_no_hole_Zpos_900.0cm_Xshift_0.0cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(scwall);

    FairDetector* hodo = new BmnHodo("HODO", kTRUE);
    hodo->SetGeometryFileName("Hodo_oldnames_Zpos_900.0cm_Xshift_0.0cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(hodo);
}
