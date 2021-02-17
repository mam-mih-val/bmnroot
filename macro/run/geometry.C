// construct detector geometry
void geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    //FairModule* pipe = new FairPipe("PIPE");
    //pipe->SetGeometryFileName("pipe_Be_kompozit_gap75cm_3.geo");
    //fRun->AddModule(pipe);

    //FairModule* target = new FairTarget("Target");
    //target->SetGeometryFileName("target_au_250mu.geo");
    //fRun->AddModule(target);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    FairModule* magnetSP57 = new FairMagnet("MAGNET_SP57");
    magnet->SetGeometryFileName("magnetSP57_1.root");
    fRun->AddModule(magnetSP57);
    
    // -----   Create detectors        -------------------------
    FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
    mwpc->SetGeometryFileName("MWPC_RunWinter2016.root");
    fRun->AddModule(mwpc);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("bd_v1_run7.geo");
    fRun->AddModule(bd);

    FairDetector* ssd = new BmnSsdMC(kTRUE, "SSDMC");
    ssd->SetGeometryFileName("sts_v18a_bmn.geo.root");
    //fRun->AddModule(ssd);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_RunSpring2018.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("STS", kTRUE);
    gems->SetGeometryFileName("GEMS_RunSpring2018.root");
    fRun->AddModule(gems);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("CSC_RunSpring2018.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN7.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_RunWinter2016.root");
    fRun->AddModule(dch);

    FairDetector* tof2 = new BmnTOF("TOF", kTRUE);
    tof2->SetGeometryFileName("tof700_run7_with_support.root");
    fRun->AddModule(tof2);

    BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    zdc->SetGeometryFileName("rootgeom_bmnzdc_104mods_v1_Zpos_8759mm_Xshift_313mm_Yshift_14mm.root");
    fRun->AddModule(zdc);
}
