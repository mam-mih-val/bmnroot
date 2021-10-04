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

    FairModule* target = new FairTarget("Target");
    target->SetGeometryFileName("target_CsI.geo");
    fRun->AddModule(target);
    
    // -----   Create detectors        -------------------------

    FairDetector* fd = new BmnFD("FD", kTRUE);
    fd->SetGeometryFileName("Sid_new_v2.root");
    fRun->AddModule(fd);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("geom_BD_det_v2.root");
    fRun->AddModule(bd);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_FutureConfig2020_detailed.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("GEM", kTRUE);
    gems->SetGeometryFileName("GEMS_FutureConfig2020_detailed.root");
    fRun->AddModule(gems);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("CSC_FutureConfig2020.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN7.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_RunSpring2018.root");
    fRun->AddModule(dch);

    FairDetector* tof2 = new BmnTOF("TOF", kTRUE);
    tof2->SetGeometryFileName("tof700_run7_with_support.root");
    fRun->AddModule(tof2);

    FairDetector* ecal = new BmnEcal("ECAL", kTRUE);
    ecal->SetGeometryFileName("ECAL_v3_run8_pos5.root");
    fRun->AddModule(ecal);

    FairDetector* scwall = new BmnScWall("SCWALL", kTRUE);
    scwall->SetGeometryFileName("ScWall_oldnames_no_hole_Zpos_878.0cm_Xshift_0.0cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(scwall);

    FairDetector* hodo = new BmnHodo("HODO", kTRUE);
    hodo->SetGeometryFileName("Hodo_oldnames_Zpos_877.0cm_Xshift_47.50cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(hodo);

    BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    //zdc->SetBirk();
    zdc->SetGeometryFileName("zdc_oldnames_NICA_36mods_no_hole_Zpos_878.0cm_Xshift_-20.0cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(zdc);
}
