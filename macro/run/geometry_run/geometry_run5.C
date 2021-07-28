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
    FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
    mwpc->SetGeometryFileName("MWPC_RunWinter2016.root");
    fRun->AddModule(mwpc);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("bd_v1_0.geo");
    fRun->AddModule(bd);

    FairDetector* gems = new CbmSts("STS", kTRUE);
    gems->SetGeometryFileName("GEMS_RunWinter2016.root");
    fRun->AddModule(gems);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    //tof1->SetGeometryFileName("TOF400_RUN5_part1.root");
    tof1->SetGeometryFileName("TOF400_RUN5_part2.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_RunSpring2018.root");
    fRun->AddModule(dch);

    FairDetector* tof2 = new CbmTof("TOF", kTRUE);
    tof2->SetGeometryFileName("tof2_v1.root");
    fRun->AddModule(tof2);

    BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    zdc->SetGeometryFileName("zdc_52modules_64layers_v1.geo");
    fRun->AddModule(zdc);
}
