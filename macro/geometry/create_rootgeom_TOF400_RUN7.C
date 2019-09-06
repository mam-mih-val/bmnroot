//**************************************************************************************
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;

void create_rootgeom_TOF400_RUN7() {
    // Load the necessary FairRoot libraries 
    //    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // -------   Load media from media file   --------------------------------
    TGeoManager *gGeoMan;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;

    // -------   Geometry file name (output)   -------------------------------
    const TString geoDetectorName = "TOF400";
    const TString geoDetectorVersion = "RUN7";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + "BMN_byKolesnicov_Aligned.root";

    // -----------------   Get and create the required media    --------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    // Air
    FairGeoMedium* matAir = geoMedia->getMedium("air");
    if (!matAir) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(matAir);
    TGeoMedium* medAir = gGeoMan->GetMedium("air");
    if (!medAir) Fatal("Main", "Medium air not found");

    // Polypropylene
    FairGeoMedium* matPprop = geoMedia->getMedium("polypropylene");
    if (!matPprop) Fatal("Main", "FairMedium polypropylene not found");
    geoBuild->createMedium(matPprop);
    TGeoMedium* medPprop = gGeoMan->GetMedium("polypropylene");
    if (!medPprop) Fatal("Main", "Medium polypropylene not found");

    // lead
    FairGeoMedium* matPb = geoMedia->getMedium("lead");
    if (!matPb) Fatal("Main", "FairMedium lead not found");
    geoBuild->createMedium(matPb);
    TGeoMedium* medPb = gGeoMan->GetMedium("lead");
    if (!medPb) Fatal("Main", "Medium lead not found");

    // aluminium
    FairGeoMedium* matAl = geoMedia->getMedium("aluminium");
    if (!matAl) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(matAl);
    TGeoMedium* medAl = gGeoMan->GetMedium("aluminium");
    if (!medAl) Fatal("Main", "Medium aluminium not found");

    // RPCglass
    FairGeoMedium* matGlass = geoMedia->getMedium("RPCglass");
    if (!matGlass) Fatal("Main", "FairMedium RPCglass not found");
    geoBuild->createMedium(matGlass);
    TGeoMedium* medGlass = gGeoMan->GetMedium("RPCglass");
    if (!medGlass) Fatal("Main", "Medium RPCglass not found");

    // RPCgas
    FairGeoMedium* matGas = geoMedia->getMedium("RPCgas");
    if (!matGas) Fatal("Main", "FairMedium RPCgas not found");
    geoBuild->createMedium(matGas);
    TGeoMedium* medGas = gGeoMan->GetMedium("RPCgas");
    if (!medGas) Fatal("Main", "Medium RPCgas not found");

    // G10
    FairGeoMedium* matG10 = geoMedia->getMedium("G10");
    if (!matG10) Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(matG10);
    TGeoMedium* medG10 = gGeoMan->GetMedium("G10");
    if (!medG10) Fatal("Main", "Medium G10 not found");

    // -----------------------------------------   Create geometry and top volume  -----------------------------------------------
    gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP"); // TOP VOLUME
    top->SetMedium(medAir);
    gGeoMan->SetTopVolume(top);
    gGeoMan->SetTopVisible(1);
    gGeoMan->SetVisLevel(7);

    // ---------------------------------------  Define TOP Geometry ------------------------------------
    TGeoVolume* topTof = new TGeoVolumeAssembly(geoDetectorName);
    topTof->SetMedium(medAir);

    TGeoVolume* vDetector = new TGeoVolumeAssembly("tof1Detector");
    vDetector->SetMedium(medAir);

    const double DetXsize = 60.; // [cm]		
    const double DetYsize = 30.; // [cm]		
    const double DetZsize = 3.0; // [cm]			
    // --------------------- Create strip ------------------ 	
    const float stripGasX = 1., stripGasY = DetYsize, stripGasZ = 0.3; // [cm]
    TGeoBBox *sStripGas = new TGeoBBox(stripGasX / 2., stripGasY / 2., stripGasZ / 2.);
    sStripGas->SetName("StripGas");
    TGeoVolume *vStripGas = new TGeoVolume("tof1StripActiveGasV", sStripGas, medGas);
    vStripGas->SetLineColor(kRed);


    // --------------------- Create Glass plates ------------------    
    const float detGlassX = DetXsize, detGlassY = DetYsize, detGlassZ = 0.618 / 2.; // [cm]  6.18 mm / 2 plate
    TGeoBBox *sDetGlass = new TGeoBBox(detGlassX / 2., detGlassY / 2., detGlassZ / 2.);
    sDetGlass->SetName("DetectorGlass");
    TGeoVolume *vDetGlass = new TGeoVolume("tof1DetGlassV", sDetGlass, medGlass);
    vDetGlass->SetLineColor(kYellow);

    // --------------------- Create G10 plates ------------------ 
    const float detPlateX = DetXsize, detPlateY = DetYsize, detPlateZ = 0.64 / 2.; // [cm]  6.4 mm / 2 plate	
    TGeoBBox *sDetPlate = new TGeoBBox(detPlateX / 2., detPlateY / 2., detPlateZ / 2.);
    sDetPlate->SetName("DetectorPlate");
    TGeoVolume *vDetPlate = new TGeoVolume("tof1DetPlateV", sDetPlate, medG10);
    vDetPlate->SetLineColor(kBlue);

    // --------------------------------------- Assemble detector

    // Add strips		
    const int NStrips = 48; //	24 strips group	
    const double StripStep = 1.25; // [cm] 
    for (int i = 0; i < NStrips; i++) {
        vDetector->AddNode(vStripGas, i + 1, new TGeoTranslation("", -DetXsize / 2. + StripStep / 2. + i * StripStep, 0., 0.));
    }

    // Add  Glass plates
    vDetector->AddNode(vDetGlass, 1, new TGeoTranslation("", 0., 0., -stripGasZ / 2. - detGlassZ / 2.));
    vDetector->AddNode(vDetGlass, 2, new TGeoTranslation("", 0., 0., +stripGasZ / 2. + detGlassZ / 2.));

    // Add  G10 plates
    vDetector->AddNode(vDetPlate, 1, new TGeoTranslation("", 0., 0., -DetZsize / 2. + detPlateZ / 2.));
    vDetector->AddNode(vDetPlate, 2, new TGeoTranslation("", 0., 0., +DetZsize / 2. - detPlateZ / 2.));

    // -------------------------------------- Install Detectors
    const double tofZgap = 0.86; // 8.6 mm between detectors
    const double tofYoverlap = 3.0; // 30 mm detectors overlap at Y direction

    const double tofXgap = 0.; // [cm] RUN5, part1
    //const double tofXgap = 2. * 24.4; // [cm] RUN5, part2

    double tofX = tofXgap / 2. + DetXsize / 2.;
    double tofZshift = tofZgap + DetZsize;

    // 1 box left side, 0 < X
    const double box1_x = 154.016 - 9. + 2.2; // [cm]
    const double box1_y = -0.51475 - 2.99 - 0.24; // [cm]
    const double box1_z = 448.579 - 4.82; // [cm] :
    TGeoRotation *geoRot1 = new TGeoRotation;
    geoRot1->RotateX(-0.905381);
    geoRot1->RotateY(-0.189805);
    geoRot1->RotateZ(0.352046);

    TGeoRotation *geoRot1_Pl0 = new TGeoRotation;
    geoRot1_Pl0->RotateX(-0.905381);
    geoRot1_Pl0->RotateY(-0.189805);
    geoRot1_Pl0->RotateZ(0.352046 + 7.22 - 0.97);

    TGeoCombiTrans *cR1 = new TGeoCombiTrans("", box1_x + 1.35, 2. * DetYsize - 2 * tofYoverlap + box1_y - 4.14, box1_z + tofZshift - 5.02, geoRot1_Pl0);
    topTof->AddNode(vDetector, 1, cR1);

    TGeoCombiTrans *cR2 = new TGeoCombiTrans("", box1_x, 1. * DetYsize - tofYoverlap + box1_y, box1_z + 2.7, geoRot1);
    topTof->AddNode(vDetector, 2, cR2);

    TGeoCombiTrans *cR3 = new TGeoCombiTrans("", box1_x, 0 + box1_y, box1_z + tofZshift - 5.02, geoRot1);
    topTof->AddNode(vDetector, 3, cR3);

    TGeoCombiTrans *cR4 = new TGeoCombiTrans("", box1_x, -1. * DetYsize + tofYoverlap + box1_y, box1_z + 2.7, geoRot1);
    topTof->AddNode(vDetector, 4, cR4);

    TGeoCombiTrans *cR5 = new TGeoCombiTrans("", box1_x, -2. * DetYsize + 2 * tofYoverlap + box1_y, box1_z + tofZshift - 5.02, geoRot1);
    topTof->AddNode(vDetector, 5, cR5);

    // 2 box left side, 0 < x < box1_x
    const double box2_x = 89.7258 + 2.2; // [cm]
    const double box2_y = -0.51475 - 2.99 - 0.24; // [cm]
    const double box2_z = 434.875; // [cm] :
    TGeoRotation *geoRot2 = new TGeoRotation;
    geoRot2->RotateX(-0.68269);
    geoRot2->RotateY(0.337719);
    geoRot2->RotateZ(0.320785);

    TGeoCombiTrans *cR6 = new TGeoCombiTrans("", box2_x, 2. * DetYsize - 2 * tofYoverlap + box2_y, box2_z + tofZshift - 5.02, geoRot2);
    topTof->AddNode(vDetector, 6, cR6);

    TGeoCombiTrans *cR7 = new TGeoCombiTrans("", box2_x, 1. * DetYsize - tofYoverlap + box2_y, box2_z + 2.7, geoRot2);
    topTof->AddNode(vDetector, 7, cR7);

    TGeoCombiTrans *cR8 = new TGeoCombiTrans("", box2_x, 0 + box2_y, box2_z + tofZshift - 5.02, geoRot2);
    topTof->AddNode(vDetector, 8, cR8);

    TGeoCombiTrans *cR9 = new TGeoCombiTrans("", box2_x, -1. * DetYsize + tofYoverlap + box2_y, box2_z + 2.7, geoRot2);
    topTof->AddNode(vDetector, 9, cR9);

    TGeoCombiTrans *cR10 = new TGeoCombiTrans("", box2_x, -2. * DetYsize + 2 * tofYoverlap + box2_y, box2_z + tofZshift - 5.02, geoRot2);
    topTof->AddNode(vDetector, 10, cR10);

    // 3 box left side, x < 0
    const double box3_x = -72.73 - 5 - 2.6; // [cm]
    const double box3_y = -0.6015 + 0.455; // [cm]
    const double box3_z = 434.129 - 1.32; // [cm] :
    TGeoRotation *geoRot3 = new TGeoRotation;
    geoRot3->RotateX(-0.16831);
    geoRot3->RotateY(0.0435636);
    geoRot3->RotateZ(0.190775);

    TGeoCombiTrans *cR11 = new TGeoCombiTrans("", box3_x, 2. * DetYsize - 2 * tofYoverlap + box3_y, box3_z + tofZshift - 5.02, geoRot3);
    topTof->AddNode(vDetector, 11, cR11);

    TGeoCombiTrans *cR12 = new TGeoCombiTrans("", box3_x, 1. * DetYsize - tofYoverlap + box3_y, box3_z + 2.7, geoRot3);
    topTof->AddNode(vDetector, 12, cR12);

    TGeoCombiTrans *cR13 = new TGeoCombiTrans("", box3_x, 0 + box3_y, box3_z + tofZshift - 5.02, geoRot3);
    topTof->AddNode(vDetector, 13, cR13);

    TGeoCombiTrans *cR14 = new TGeoCombiTrans("", box3_x, -1. * DetYsize + tofYoverlap + box3_y, box3_z + 2.7, geoRot3);
    topTof->AddNode(vDetector, 14, cR14);

    TGeoCombiTrans *cR15 = new TGeoCombiTrans("", box3_x, -2. * DetYsize + 2 * tofYoverlap + box3_y, box3_z + tofZshift - 5.02, geoRot3);
    topTof->AddNode(vDetector, 15, cR15);

    // 4 box left side, box3 < x < 0
    const double box4_x = -138.442 + 10 - 5. - 2.6; // [cm]
    const double box4_y = -0.6015 + 0.455; // [cm]
    const double box4_z = 447.906 - 7.; // [cm] :
    TGeoRotation *geoRot4 = new TGeoRotation;
    geoRot4->RotateX(0);
    geoRot4->RotateY(0);
    geoRot4->RotateZ(0);

    TGeoCombiTrans *cR16 = new TGeoCombiTrans("", box4_x, 2. * DetYsize - 2 * tofYoverlap + box4_y, box4_z + tofZshift - 5.02, geoRot4);
    topTof->AddNode(vDetector, 16, cR16);

    TGeoCombiTrans *cR17 = new TGeoCombiTrans("", box4_x, 1. * DetYsize - tofYoverlap + box4_y, box4_z + 2.7, geoRot4);
    topTof->AddNode(vDetector, 17, cR17);

    TGeoCombiTrans *cR18 = new TGeoCombiTrans("", box4_x, 0 + box4_y, box4_z + tofZshift - 5.02, geoRot4);
    topTof->AddNode(vDetector, 18, cR18);

    TGeoCombiTrans *cR19 = new TGeoCombiTrans("", box4_x, -1. * DetYsize + tofYoverlap + box4_y, box4_z + 2.7, geoRot4);
    topTof->AddNode(vDetector, 19, cR19);

    TGeoCombiTrans *cR20 = new TGeoCombiTrans("", box4_x, -2. * DetYsize + 2 * tofYoverlap + box4_y, box4_z + tofZshift - 5.02, geoRot4);
    topTof->AddNode(vDetector, 20, cR20);

    //2 box
    /*   topTof->AddNode(vDetector, 6, new TGeoTranslation("t1", box2_x, 2. * DetYsize - 2 * tofYoverlap, tofZ_2box + tofZshift)); // right side, x > 0
       topTof->AddNode(vDetector, 7, new TGeoTranslation("t1", box2_x, 1. * DetYsize - tofYoverlap, tofZ_2box));
       topTof->AddNode(vDetector, 8, new TGeoTranslation("t1", box2_x, 0, tofZ_2box + tofZshift));
       topTof->AddNode(vDetector, 9, new TGeoTranslation("t1", box2_x, -1. * DetYsize + tofYoverlap, tofZ_2box));
       topTof->AddNode(vDetector, 10, new TGeoTranslation("t1", box2_x, -2 * DetYsize + 2 * tofYoverlap, tofZ_2box + tofZshift));

       //3 box
       topTof->AddNode(vDetector, 11, new TGeoTranslation("t1", box3_x, 2. * DetYsize - 2 * tofYoverlap, tofZ_2box + tofZshift)); // right side, x > 0
       topTof->AddNode(vDetector, 12, new TGeoTranslation("t1", box3_x, 1. * DetYsize - tofYoverlap, tofZ_2box));
       topTof->AddNode(vDetector, 13, new TGeoTranslation("t1", box3_x, 0, tofZ_2box + tofZshift));
       topTof->AddNode(vDetector, 14, new TGeoTranslation("t1", box3_x, -1. * DetYsize + tofYoverlap, tofZ_2box));
       topTof->AddNode(vDetector, 15, new TGeoTranslation("t1", box3_x, -2 * DetYsize + 2 * tofYoverlap, tofZ_2box + tofZshift));

       // 4 box
       topTof->AddNode(vDetector, 16, new TGeoTranslation("t1", box4_x, 2. * DetYsize - 2 * tofYoverlap, tofZ_1box + tofZshift)); // right side, x > 0
       topTof->AddNode(vDetector, 17, new TGeoTranslation("t1", box4_x, 1. * DetYsize - tofYoverlap, tofZ_1box));
       topTof->AddNode(vDetector, 18, new TGeoTranslation("t1", box4_x, 0, tofZ_1box + tofZshift));
       topTof->AddNode(vDetector, 19, new TGeoTranslation("t1", box4_x, -1. * DetYsize + tofYoverlap, tofZ_1box));
       topTof->AddNode(vDetector, 20, new TGeoTranslation("t1", box4_x, -2. * DetYsize + 2 * tofYoverlap, tofZ_1box + tofZshift));

       //topTof->AddNode(vDetector, 4, new TGeoTranslation("t1",  -tofX,  0., tofZ + tofZshift)); // left side, x < 0
       //topTof->AddNode(vDetector, 5, new TGeoTranslation("t1",  -tofX,  +DetYsize -tofYoverlap, tofZ - tofZshift));  
       //topTof->AddNode(vDetector, 6, new TGeoTranslation("t1",  -tofX,  -DetYsize +tofYoverlap, tofZ - tofZshift));    

   //*/
    // ------------------  Adding  the TOP Volume --------------------------
    top->AddNode(topTof, 1, new TGeoTranslation("tw", 0., 0., 0.));

    // ---------------   Finish   --------------------------------------------
    top->SetVisContainers(kTRUE);

    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    top->Draw("ogl");
    gGeoMan->Browse(new TBrowser);
}

