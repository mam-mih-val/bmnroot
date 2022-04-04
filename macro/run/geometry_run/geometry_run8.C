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

    FairDetector* sibt = new BmnSiBT("SiBT", kTRUE);
    sibt->SetGeometryFileName("SiBT_Run8.root");
    fRun->AddModule(sibt);

    FairDetector* simd = new BmnSiMD("SiMD", kTRUE);
    simd->SetGeometryFileName("SiMD_run8_v1.root");
    fRun->AddModule(simd);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("BD_run8_v1.root");
    fRun->AddModule(bd);

    FairDetector* fd = new BmnFD("FD", kTRUE);
    fd->SetGeometryFileName("FD_run8.root");
    fRun->AddModule(fd);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_Run8_3stations_detailed.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("GEM", kTRUE);
    gems->SetGeometryFileName("GEMS_Run8_detailed.root");
    fRun->AddModule(gems);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("CSC_Run8_detailed.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN7.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_Run8.root");
    fRun->AddModule(dch);

    FairDetector* tof2 = new BmnTOF("TOF", kTRUE);
    tof2->SetGeometryFileName("tof700_run8_with_support.root");
    fRun->AddModule(tof2);

    FairDetector* ecal = new BmnEcal("ECAL", kTRUE);
    ecal->SetGeometryFileName("ECAL_v3_run8_pos5.root");
    fRun->AddModule(ecal);

    FairDetector* scwall = new BmnScWall("SCWALL", kTRUE);
    scwall->SetGeometryFileName("ScWall_run8_with_hole_in_box_with_hole_XeCsI_3.9GeV_field_Extrap_scale_1800_900_Zpos_875.0cm_Xshift_78.0cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(scwall);

    FairDetector* hodo = new BmnHodo("HODO", kTRUE);
    hodo->SetGeometryFileName("Hodo_with_box_XeCsI_3.9GeV_field_Extrap_scale_1800_900_Zpos_892.0cm_Xshift_55.50cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(hodo);

    FairDetector* fhcal = new BmnFHCal("FHCAL", kTRUE);
    //zdc->SetBirk();
    fhcal->SetGeometryFileName("FHCal_54mods_hole_XeCsI_3.9GeV_field_Extrap_scale_1800_900_Zpos_900.0cm_Xshift_55.5cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(fhcal);
}
