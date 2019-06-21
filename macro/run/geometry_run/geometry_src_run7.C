// construct detector geometry
void geometry_src_run7(FairRunSim *fRun)
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
    FairDetector* tof = new BmnTOF("TOF", kTRUE);
    tof->SetGeometryFileName("tof700_run7_with_support.root");
    fRun->AddModule(tof);

    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName("GEMS_RunSRCSpring2018.root");
    fRun->AddModule(sts);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN7_SRC.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_RunWinter2016.root");
    fRun->AddModule(dch);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_RunSRCSpring2018.root");
    fRun->AddModule(silicon);
    
    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("CSC_RunSRCSpring2018.root");
    fRun->AddModule(csc);
}
