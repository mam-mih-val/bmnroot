/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   create_rootgeom_TOF400_RUN8.C
 * Author: mikhailr
 *
 * Created on August 25, 2021, 11:06 AM
 */

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include <TMath.h>

#include "macro/run/bmnloadlibs.C"

using namespace std;
using namespace TMath;

void create_rootgeom_TOF400_RUN8_SRC() {

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
    const TString geoDetectorVersion = "RUN8_SRC";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";

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
    const double DetZsize = 3.4; // [cm]			
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
    // The Z=0 of coordinate system is front edge of SP41 pole. 
    const Double_t tofZgap = 0.9; // 9 mm between detectors
    const Double_t tofYoverlap = 3.0; // 30 mm detectors overlap at Y direction
    const Double_t tofZshift = tofZgap + DetZsize;
    const Double_t FromCoverToRpc = 7.95;
    const Double_t FromHVToRpc = 2.3;
    const Double_t ZSP41 = 1333.7575 + 304.812;//the length from 1k200


    /*
     * box1
       |   8.3cm    |
       |            |  
       |   |--|     |
       |   |  |     |
      |--| |__|     |
      |  |          |
      |__| |--|     |    ---->Z
           |  |     |        
      |--| |__|     |
      |  |          |
      |__| |--|     |
           |  |     |
           |__|     |  
             |      |
             |            
            tofZ_Xbox is centr of the PRC  
     */


    // 1 box Det 0-4
    Double_t angleXDegBox1 = 0.399; //0.816
    Double_t angleYDegBox1 = 30.5566;
    Double_t angleZDegBox1 = 0.446; //0.175
    const double box1_x = 148.098; // [cm]
    const double box1_y = -0.473 - FromHVToRpc; // [cm]
    const double box1_z = 1259.38 - ZSP41; // [cm] :

    //    Double_t angleDegBox2 = 0;
    //        const double tofZ_2box = 499.27; // [cm] :
    //        double box2_x = -28.34; // [cm]
    //        double box2_y = 0; // [cm]

    TGeoRotation *geoRot1 = new TGeoRotation;
    geoRot1->RotateX(angleXDegBox1);
    geoRot1->RotateY(angleYDegBox1);
    geoRot1->RotateZ(angleZDegBox1);

    TVector3 *d1 = new TVector3(0, 2. * DetYsize - 2 * tofYoverlap, FromCoverToRpc);
    TVector3 *d2 = new TVector3(0, 1. * DetYsize - 1 * tofYoverlap, FromCoverToRpc - tofZshift);
    TVector3 *d3 = new TVector3(0, 0, FromCoverToRpc);
    TVector3 *d4 = new TVector3(0, -1. * DetYsize + 1 * tofYoverlap, FromCoverToRpc - tofZshift);
    TVector3 *d5 = new TVector3(0, -2. * DetYsize + 2 * tofYoverlap, FromCoverToRpc);

    d1->RotateX(TMath::DegToRad() * angleXDegBox1);
    d2->RotateX(TMath::DegToRad() * angleXDegBox1);
    d3->RotateX(TMath::DegToRad() * angleXDegBox1);
    d4->RotateX(TMath::DegToRad() * angleXDegBox1);
    d5->RotateX(TMath::DegToRad() * angleXDegBox1);

    d1->RotateY(TMath::DegToRad() * angleYDegBox1);
    d2->RotateY(TMath::DegToRad() * angleYDegBox1);
    d3->RotateY(TMath::DegToRad() * angleYDegBox1);
    d4->RotateY(TMath::DegToRad() * angleYDegBox1);
    d5->RotateY(TMath::DegToRad() * angleYDegBox1);

    d1->RotateZ(TMath::DegToRad() * angleZDegBox1);
    d2->RotateZ(TMath::DegToRad() * angleZDegBox1);
    d3->RotateZ(TMath::DegToRad() * angleZDegBox1);
    d4->RotateZ(TMath::DegToRad() * angleZDegBox1);
    d5->RotateZ(TMath::DegToRad() * angleZDegBox1);

    TGeoCombiTrans *cR1 = new TGeoCombiTrans("", d1->X() + box1_x, d1->Y() + box1_y, d1->Z() + box1_z, geoRot1);
    topTof->AddNode(vDetector, 1, cR1); // right side, x > 0

    TGeoCombiTrans *cR2 = new TGeoCombiTrans("", d2->X() + box1_x, d2->Y() + box1_y, d2->Z() + box1_z, geoRot1);
    topTof->AddNode(vDetector, 2, cR2); // right side, x > 0

    TGeoCombiTrans *cR3 = new TGeoCombiTrans("", d3->X() + box1_x, d3->Y() + box1_y, d3->Z() + box1_z, geoRot1);
    topTof->AddNode(vDetector, 3, cR3); // right side, x > 0

    TGeoCombiTrans *cR4 = new TGeoCombiTrans("", d4->X() + box1_x, d4->Y() + box1_y, d4->Z() + box1_z, geoRot1);
    topTof->AddNode(vDetector, 4, cR4); // right side, x > 0

    TGeoCombiTrans *cR5 = new TGeoCombiTrans("", d5->X() + box1_x, d5->Y() + box1_y, d5->Z() + box1_z, geoRot1);
    topTof->AddNode(vDetector, 5, cR5); // right side, x > 0
    //--------------------------------------------------------------------------


    //4 box det 5-9
    Double_t angleXDegBox4 = 0.816; //0.816
    Double_t angleYDegBox4 = 30.487;
    Double_t angleZDegBox4 = 0.175; //0.175
    const double box4_x = 114.728; // [cm]
    const double box4_y = -0.65 - FromHVToRpc; // [cm]
    const double box4_z = 1307.97 - ZSP41; // [cm] :

    TGeoRotation *geoRot4 = new TGeoRotation;
    geoRot4->RotateX(angleXDegBox4);
    geoRot4->RotateY(angleYDegBox4);
    geoRot4->RotateZ(angleZDegBox4);

    TVector3 *d6 = new TVector3(0, 2. * DetYsize - 2 * tofYoverlap, -FromCoverToRpc);
    TVector3 *d7 = new TVector3(0, 1. * DetYsize - 1 * tofYoverlap, -FromCoverToRpc + tofZshift);
    TVector3 *d8 = new TVector3(0, 0, -FromCoverToRpc);
    TVector3 *d9 = new TVector3(0, -1. * DetYsize + 1 * tofYoverlap, -FromCoverToRpc + tofZshift);
    TVector3 *d10 = new TVector3(0, -2. * DetYsize + 2 * tofYoverlap, -FromCoverToRpc);

    d6->RotateX(TMath::DegToRad() * angleXDegBox4);
    d7->RotateX(TMath::DegToRad() * angleXDegBox4);
    d8->RotateX(TMath::DegToRad() * angleXDegBox4);
    d9->RotateX(TMath::DegToRad() * angleXDegBox4);
    d10->RotateX(TMath::DegToRad() * angleXDegBox4);

    d6->RotateY(TMath::DegToRad() * angleYDegBox4);
    d7->RotateY(TMath::DegToRad() * angleYDegBox4);
    d8->RotateY(TMath::DegToRad() * angleYDegBox4);
    d9->RotateY(TMath::DegToRad() * angleYDegBox4);
    d10->RotateY(TMath::DegToRad() * angleYDegBox4);

    d6->RotateZ(TMath::DegToRad() * angleZDegBox4);
    d7->RotateZ(TMath::DegToRad() * angleZDegBox4);
    d8->RotateZ(TMath::DegToRad() * angleZDegBox4);
    d9->RotateZ(TMath::DegToRad() * angleZDegBox4);
    d10->RotateZ(TMath::DegToRad() * angleZDegBox4);

    TGeoCombiTrans *cR6 = new TGeoCombiTrans("", d6->X() + box4_x, d6->Y() + box4_y, d6->Z() + box4_z, geoRot4);
    topTof->AddNode(vDetector, 6, cR6); // right side, x > 0

    TGeoCombiTrans *cR7 = new TGeoCombiTrans("", d7->X() + box4_x, d7->Y() + box4_y, d7->Z() + box4_z, geoRot4);
    topTof->AddNode(vDetector, 7, cR7);

    TGeoCombiTrans *cR8 = new TGeoCombiTrans("", d8->X() + box4_x, d8->Y() + box4_y, d8->Z() + box4_z, geoRot4);
    topTof->AddNode(vDetector, 8, cR8);

    TGeoCombiTrans *cR9 = new TGeoCombiTrans("", d9->X() + box4_x, d9->Y() + box4_y, d9->Z() + box4_z, geoRot4);
    topTof->AddNode(vDetector, 9, cR9);

    TGeoCombiTrans *cR10 = new TGeoCombiTrans("", d10->X() + box4_x, d10->Y() + box4_y, d10->Z() + box4_z, geoRot4);
    topTof->AddNode(vDetector, 10, cR10);

    //--------------------------------------------------------------------------
    //3 box det 10-15
    Double_t angleXDegBox3 = 0.063; //0.063
    Double_t angleYDegBox3 = -31.091; //-31.091
    Double_t angleZDegBox3 = 0.062; //0.062
    const double box3_x = -113.066; // [cm]
    const double box3_y = -0.314 - FromHVToRpc; // [cm]
    const double box3_z = 1306.43 - ZSP41; // [cm] :

    TGeoRotation *geoRot3 = new TGeoRotation;
    geoRot3->RotateX(angleXDegBox3);
    geoRot3->RotateY(angleYDegBox3);
    geoRot3->RotateZ(angleZDegBox3);

    TVector3 *d11 = new TVector3(0, 2. * DetYsize - 2 * tofYoverlap, -FromCoverToRpc + tofZshift);
    TVector3 *d12 = new TVector3(0, 1. * DetYsize - 1 * tofYoverlap, -FromCoverToRpc);
    TVector3 *d13 = new TVector3(0, 0, -FromCoverToRpc + tofZshift);
    TVector3 *d14 = new TVector3(0, -1. * DetYsize + 1 * tofYoverlap, -FromCoverToRpc);
    TVector3 *d15 = new TVector3(0, -2. * DetYsize + 2 * tofYoverlap, -FromCoverToRpc + tofZshift);

    d11->RotateX(TMath::DegToRad() * angleXDegBox3);
    d12->RotateX(TMath::DegToRad() * angleXDegBox3);
    d13->RotateX(TMath::DegToRad() * angleXDegBox3);
    d14->RotateX(TMath::DegToRad() * angleXDegBox3);
    d15->RotateX(TMath::DegToRad() * angleXDegBox3);

    d11->RotateY(TMath::DegToRad() * angleYDegBox3);
    d12->RotateY(TMath::DegToRad() * angleYDegBox3);
    d13->RotateY(TMath::DegToRad() * angleYDegBox3);
    d14->RotateY(TMath::DegToRad() * angleYDegBox3);
    d15->RotateY(TMath::DegToRad() * angleYDegBox3);

    d11->RotateZ(TMath::DegToRad() * angleZDegBox3);
    d12->RotateZ(TMath::DegToRad() * angleZDegBox3);
    d13->RotateZ(TMath::DegToRad() * angleZDegBox3);
    d14->RotateZ(TMath::DegToRad() * angleZDegBox3);
    d15->RotateZ(TMath::DegToRad() * angleZDegBox3);

    TGeoCombiTrans *cR11 = new TGeoCombiTrans("", d11->X() + box3_x, d11->Y() + box3_y, d11->Z() + box3_z, geoRot3);
    topTof->AddNode(vDetector, 11, cR11); // right side, x > 0

    TGeoCombiTrans *cR12 = new TGeoCombiTrans("", d12->X() + box3_x, d12->Y() + box3_y, d12->Z() + box3_z, geoRot3);
    topTof->AddNode(vDetector, 12, cR12);

    TGeoCombiTrans *cR13 = new TGeoCombiTrans("", d13->X() + box3_x, d13->Y() + box3_y, d13->Z() + box3_z, geoRot3);
    topTof->AddNode(vDetector, 13, cR13);

    TGeoCombiTrans *cR14 = new TGeoCombiTrans("", d14->X() + box3_x, d14->Y() + box3_y, d14->Z() + box3_z, geoRot3);
    topTof->AddNode(vDetector, 14, cR14);

    TGeoCombiTrans *cR15 = new TGeoCombiTrans("", d15->X() + box3_x, d15->Y() + box3_y, d15->Z() + box3_z, geoRot3);
    topTof->AddNode(vDetector, 15, cR15);
    //--------------------------------------------------------------------------

    //2 box det 16-20
    Double_t angleXDegBox2 = 0.112; //0.112
    Double_t angleYDegBox2 = -31.511; //-31.511
    Double_t angleZDegBox2 = 0.193; //0.193
    const double box2_x = -145.355; // [cm]
    const double box2_y = -0.473 - FromHVToRpc; // [cm]
    const double box2_z = 1257.61 - ZSP41; // [cm] :

    TGeoRotation *geoRot2 = new TGeoRotation;
    geoRot2->RotateX(angleXDegBox2);
    geoRot2->RotateY(angleYDegBox2);
    geoRot2->RotateZ(angleZDegBox2);

    TVector3 *d16 = new TVector3(0, 2. * DetYsize - 2 * tofYoverlap, FromCoverToRpc - tofZshift);
    TVector3 *d17 = new TVector3(0, 1. * DetYsize - 1 * tofYoverlap, FromCoverToRpc);
    TVector3 *d18 = new TVector3(0, 0, FromCoverToRpc - tofZshift);
    TVector3 *d19 = new TVector3(0, -1. * DetYsize + 1 * tofYoverlap, FromCoverToRpc);
    TVector3 *d20 = new TVector3(0, -2. * DetYsize + 2 * tofYoverlap, FromCoverToRpc - tofZshift);

    d16->RotateX(TMath::DegToRad() * angleXDegBox2);
    d17->RotateX(TMath::DegToRad() * angleXDegBox2);
    d18->RotateX(TMath::DegToRad() * angleXDegBox2);
    d19->RotateX(TMath::DegToRad() * angleXDegBox2);
    d20->RotateX(TMath::DegToRad() * angleXDegBox2);

    d16->RotateY(TMath::DegToRad() * angleYDegBox2);
    d17->RotateY(TMath::DegToRad() * angleYDegBox2);
    d18->RotateY(TMath::DegToRad() * angleYDegBox2);
    d19->RotateY(TMath::DegToRad() * angleYDegBox2);
    d20->RotateY(TMath::DegToRad() * angleYDegBox2);

    d16->RotateZ(TMath::DegToRad() * angleZDegBox2);
    d17->RotateZ(TMath::DegToRad() * angleZDegBox2);
    d18->RotateZ(TMath::DegToRad() * angleZDegBox2);
    d19->RotateZ(TMath::DegToRad() * angleZDegBox2);
    d20->RotateZ(TMath::DegToRad() * angleZDegBox2);

    TGeoCombiTrans *cR16 = new TGeoCombiTrans("", d16->X() + box2_x, d16->Y() + box2_y, d16->Z() + box2_z, geoRot2);
    topTof->AddNode(vDetector, 16, cR16); // right side, x > 0

    TGeoCombiTrans *cR17 = new TGeoCombiTrans("", d17->X() + box2_x, d17->Y() + box2_y, d17->Z() + box2_z, geoRot2);
    topTof->AddNode(vDetector, 17, cR17);

    TGeoCombiTrans *cR18 = new TGeoCombiTrans("", d18->X() + box2_x, d18->Y() + box2_y, d18->Z() + box2_z, geoRot2);
    topTof->AddNode(vDetector, 18, cR18);

    TGeoCombiTrans *cR19 = new TGeoCombiTrans("", d19->X() + box2_x, d19->Y() + box2_y, d19->Z() + box2_z, geoRot2);
    topTof->AddNode(vDetector, 19, cR19);

    TGeoCombiTrans *cR20 = new TGeoCombiTrans("", d20->X() + box2_x, d20->Y() + box2_y, d20->Z() + box2_z, geoRot2);
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

