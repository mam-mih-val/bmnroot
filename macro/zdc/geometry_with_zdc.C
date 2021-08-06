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
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName("tof_v07a.geo");
    fRun->AddModule(tof);

    BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    zdc->SetGeometryFileName("zdc_52modules_64layers_v1.geo");
    fRun->AddModule( zdc);

    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName("GEMS_v2.root");
    fRun->AddModule(sts);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("geom_BD_det_v2.root");
    fRun->AddModule(bd);
    
    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
//  tof1->SetGeometryFileName("tof1_W400_v1.geo");
    tof1->SetGeometryFileName("TOFB1_v2.root");
    fRun->AddModule(tof1);
    
    FairDetector* dch1 = new BmnDch1("DCH1", kTRUE);
    dch1->SetGeometryFileName("dch1_v1.root");
    fRun->AddModule(dch1);
    
    FairDetector* dch2 = new BmnDch2("DCH2", kTRUE);
    dch2->SetGeometryFileName("dch2_v1.root");
    fRun->AddModule(dch2);
}
