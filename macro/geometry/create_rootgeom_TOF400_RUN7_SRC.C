
/* 
 * File:   create_rootgeom_TOF400_RUN7_SRC.C
 * Author: mikhailr
 *
 * Created on January 18, 2018, 12:19 PM
 */
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
using namespace std;

void create_rootgeom_TOF400_RUN7_SRC() {
    // Load the necessary FairRoot libraries 
//    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // -------   Load media from media file   --------------------------------
    TGeoManager *gGeoMan;
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;

    // -------   Geometry file name (output)   -------------------------------
    const TString geoDetectorName = "TOF400";
    const TString geoDetectorVersion = "RUN7_SRC_AllignmentZY";
    const TString geoFileName = Form("%s_%s%s",geoDetectorName.Data(),geoDetectorVersion.Data(),".root");

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
    const double tofYoverlap = 3.0; // 30 mm detectors overlap at X direction

    const double tofXgap = 0.; // [cm] RUN5, part1
    //const double tofXgap = 2. * 24.4; // [cm] RUN5, part2
    const double tofZ_1box = -234.705; // [cm] :
    const double tofZ_2box = -217.27; // [cm] :
    const double tofZ_3box = -217.27; // [cm] :
    const double tofZ_4box = -234.705; // [cm] :

    double tofX = tofXgap / 2. + DetXsize / 2.;
    double tofZshift = -1. * (tofZgap + DetZsize);
    double tofYshift = -2.9; //[cm] base on BM@N alignment
    double box1_x = 277.299; // [cm]
    double box2_x = 222.701; // [cm]
    double box3_x = -222.701; // [cm]
    double box4_x = -277.299; // [cm]

    Double_t angleDegBox1_2 = 30., angleDegBox3_4 = -30.;

    TGeoRotation *geoRot = new TGeoRotation;
    geoRot->RotateY(angleDegBox1_2);

    // 1 box
    //        topTof->AddNode(vDetector, 1, new TGeoTranslation("t1", box1_x, 2. * DetYsize - 2 * tofYoverlap, tofZ_1box + tofZshift)); // right side, x > 0
    TGeoCombiTrans *cR1 = new TGeoCombiTrans("", box1_x, 2. * DetYsize - 2 * tofYoverlap + tofYshift, tofZ_1box + tofZshift, geoRot);
    topTof->AddNode(vDetector, 1, cR1); // right side, x > 0

    TGeoCombiTrans *cR2 = new TGeoCombiTrans("", box1_x, 1. * DetYsize - tofYoverlap + tofYshift, tofZ_1box, geoRot);
    topTof->AddNode(vDetector, 2, cR2);

    TGeoCombiTrans *cR3 = new TGeoCombiTrans("", box1_x, 0 + tofYshift, tofZ_1box + tofZshift, geoRot);
    topTof->AddNode(vDetector, 3, cR3);

    TGeoCombiTrans *cR4 = new TGeoCombiTrans("", box1_x, -1. * DetYsize + tofYoverlap + tofYshift, tofZ_1box, geoRot);
    topTof->AddNode(vDetector, 4, cR4);

    TGeoCombiTrans *cR5 = new TGeoCombiTrans("", box1_x, -2. * DetYsize + 2 * tofYoverlap + tofYshift, tofZ_1box + tofZshift, geoRot);
    topTof->AddNode(vDetector, 5, cR5);

    //2 box
    TGeoCombiTrans *cR6 = new TGeoCombiTrans("", box2_x, 2. * DetYsize - 2 * tofYoverlap + tofYshift, tofZ_2box + tofZshift, geoRot);
    topTof->AddNode(vDetector, 6, cR6); // right side, x > 0

    TGeoCombiTrans *cR7 = new TGeoCombiTrans("", box2_x, 1. * DetYsize - tofYoverlap + tofYshift, tofZ_2box, geoRot);
    topTof->AddNode(vDetector, 7, cR7);

    TGeoCombiTrans *cR8 = new TGeoCombiTrans("", box2_x, 0 + tofYshift, tofZ_2box + tofZshift, geoRot);
    topTof->AddNode(vDetector, 8, cR8);

    TGeoCombiTrans *cR9 = new TGeoCombiTrans("", box2_x, -1. * DetYsize + tofYoverlap + tofYshift, tofZ_2box, geoRot);
    topTof->AddNode(vDetector, 9, cR9);

    TGeoCombiTrans *cR10 = new TGeoCombiTrans("", box2_x, -2 * DetYsize + 2 * tofYoverlap + tofYshift, tofZ_2box + tofZshift, geoRot);
    topTof->AddNode(vDetector, 10, cR10);

    TGeoRotation *geoRot2 = new TGeoRotation;
    geoRot2->RotateY(angleDegBox3_4);
    //3 box
    TGeoCombiTrans *cR11 = new TGeoCombiTrans("", box3_x, 2. * DetYsize - 2 * tofYoverlap + tofYshift, tofZ_2box + tofZshift, geoRot2);
    topTof->AddNode(vDetector, 11, cR11); // right side, x > 0

    TGeoCombiTrans *cR12 = new TGeoCombiTrans("", box3_x, 1. * DetYsize - tofYoverlap + tofYshift, tofZ_2box, geoRot2);
    topTof->AddNode(vDetector, 12, cR12);

    TGeoCombiTrans *cR13 = new TGeoCombiTrans("", box3_x, 0 + tofYshift, tofZ_2box + tofZshift, geoRot2);
    topTof->AddNode(vDetector, 13, cR13);

    TGeoCombiTrans *cR14 = new TGeoCombiTrans("", box3_x, -1. * DetYsize + tofYoverlap + tofYshift, tofZ_2box, geoRot2);
    topTof->AddNode(vDetector, 14, cR14);

    TGeoCombiTrans *cR15 = new TGeoCombiTrans("", box3_x, -2 * DetYsize + 2 * tofYoverlap + tofYshift, tofZ_2box + tofZshift, geoRot2);
    topTof->AddNode(vDetector, 15, cR15);

    // 4 box
    TGeoCombiTrans *cR16 = new TGeoCombiTrans("", box4_x, 2. * DetYsize - 2 * tofYoverlap + tofYshift, tofZ_1box + tofZshift, geoRot2);
    topTof->AddNode(vDetector, 16, cR16); // right side, x > 0

    TGeoCombiTrans *cR17 = new TGeoCombiTrans("", box4_x, 1. * DetYsize - tofYoverlap + tofYshift, tofZ_1box, geoRot2);
    topTof->AddNode(vDetector, 17, cR17);

    TGeoCombiTrans *cR18 = new TGeoCombiTrans("", box4_x, 0 + tofYshift, tofZ_1box + tofZshift, geoRot2);
    topTof->AddNode(vDetector, 18, cR18);

    TGeoCombiTrans *cR19 = new TGeoCombiTrans("", box4_x, -1. * DetYsize + tofYoverlap + tofYshift, tofZ_1box, geoRot2);
    topTof->AddNode(vDetector, 19, cR19);

    TGeoCombiTrans *cR20 = new TGeoCombiTrans("", box4_x, -2. * DetYsize + 2 * tofYoverlap + tofYshift, tofZ_1box + tofZshift, geoRot2);
    topTof->AddNode(vDetector, 20, cR20);


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

