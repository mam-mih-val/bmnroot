// construct detector geometry
void geometry(FairRunSim *fRun)
{
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

//    FairModule* pipe = new FairPipe("PIPE");
//    pipe->SetGeometryFileName("pipe_Be_kompozit_gap75cm_3.geo");
//    fRun->AddModule(pipe);

    //    FairModule* target = new FairTarget("Target");
    //    target->SetGeometryFileName("target_au_250mu.geo");
    //    fRun->AddModule(target);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    // -----   Create detectors        -------------------------
    FairDetector* tof = new BmnTOF("TOF", kTRUE);
    tof->SetGeometryFileName("tof700_run6.root");
    fRun->AddModule(tof);

    CbmPsdv1* psd = new CbmPsdv1("PSD", kTRUE);
    psd->SetXshift(45.);
    psd->SetZposition(1000.);
    psd->SetHole(1); // 0 for no hole
    fRun->AddModule(psd);

    FairDetector* sts = new CbmSts("STS", kTRUE);
    //sts->SetGeometryFileName("GEMS_RunWinter2016.root");
    sts->SetGeometryFileName("GEMS_RunSpring2017.root");
    //sts->SetGeometryFileName("GEMS_RunSpring2018.root");
    fRun->AddModule(sts);

    //    FairDetector* recoil = new BmnRecoil("RECOIL", kTRUE);
    //    recoil->SetGeometryFileName("recoil_modules_70x12_v1.geo");
    //    fRun->AddModule(recoil);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN5_part2.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_RunWinter2016.root");
    fRun->AddModule(dch);

    FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
    mwpc->SetGeometryFileName("MWPC_RunWinter2016.root");
    fRun->AddModule(mwpc);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("bd_v1_0.geo");
    //fRun->AddModule(bd);

    FairDetector* emc = new BmnEcal("EMC", kTRUE);
    emc->SetGeometryFileName("ecal_v1_0.geo");
    fRun->AddModule(emc);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    //silicon->SetGeometryFileName("Silicon_v1.root");
    silicon->SetGeometryFileName("Silicon_RunSpring2017.root");
    //silicon->SetGeometryFileName("Silicon_RunSpring2018.root");
    fRun->AddModule(silicon);

    FairDetector* ssd = new BmnSSD("SSD", kTRUE);
    ssd->SetGeometryFileName("SSD_v17a.root");
    //fRun->AddModule(ssd);
}
