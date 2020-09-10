#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"


using namespace TMath;

TGeoManager* gGeoMan = NULL;

const Double_t Zshift = -647.5;

//Detector's position
const Double_t X_pos1[] = {-7.5, 0, 0}; // cm
const Double_t X_pos2[] = {7.5, 0, 0}; //cm +15 cm

const Double_t X1_pos1[] = {23.3086, 1.2160, 35.5676 + Zshift}; 
const Double_t Y1_pos[] = {79.9954, -2.8672, 150.3262 + Zshift}; 
const Double_t X2_pos1[] = {-22.0804, 0.6820, 38.4383 + Zshift}; 
const Double_t Y2_pos[] = {-83.5340, -1.9742, 147.0401 + Zshift}; 
const Double_t R_angle[] = {90, 30, 0.0};
const Double_t L_angle[] = {-90, 30, 0.0};

//       X,Y:
//        A______________B
//        |              |
//        |              |
//        |              |
//        |______________|
//        D              C
//
//
//


const Double_t X1_ABSize = 30.00; // cm 20.2422; //cm
const Double_t X1_BCSize = 15.00; //13.7804; //cm
const Double_t X_ZSize = 1.0; //cm

const Double_t X2_ABSize = 30.00; //24.0359; //cm
const Double_t X2_BCSize = 15.00; //12.8968; //cm

const Double_t Y_ABSize = 50.00; //50.1; //cm
const Double_t Y_BCSize = 50.00; //55.3; //cm
const Double_t Y_ZSize = 1.0; //cm

void create_rootgeom_SRCArmTriggers() {

    // Load the necessary FairRott libraries
    //gROOT->LoadMacro("$MCWORKDIR/macro/run/bmnloadlibs.C");
    //bmnloadlibs(); // load libraries

    // -------   Load media from media file----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/";
    TString medFile = geoPath + "media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;

    // ------   Geometry file name (output)----------------------------------
    const TString geoDetectorName = "SRCArmTriggers";
    const TString geoDetectorVersion = "Spring2018";
    TString geoFileName = geoPath + geoDetectorName + "_" + geoDetectorVersion + ".root";
    //-----------------------------------------------------------------------

    // ---------- Get and Create the required media
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if (!mAir) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if (!pMedAir) Fatal("Main", "Medium air not found");

    FairGeoMedium* mBC408 = geoMedia->getMedium("BC408");
    if (!mBC408) Fatal("Main", "FairMedium BC400 not found");
    geoBuild->createMedium(mBC408);
    TGeoMedium* pMedBC408 = gGeoMan->GetMedium("BC408");
    if (!pMedBC408) Fatal("Main", " Medium BC408 not found");


    // ----------   Create geometry and top volume --------------------------
    gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //

    //Define TOP Geometry
    TGeoVolume* ARM = new TGeoVolumeAssembly(geoDetectorName);
    ARM->SetMedium(pMedAir);
    ARM->SetTransparency(50);

    //Translotations and rotations
    TGeoTranslation* DetPos_transX_1 = new TGeoTranslation("DetPos_transX1_1", X_pos1[0], X_pos1[1], X_pos1[2]);
    TGeoTranslation* DetPos_transX_2 = new TGeoTranslation("DetPos_transX1_2", X_pos2[0], X_pos2[1], X_pos2[2]);

    TGeoTranslation* DetPos_transX1_1 = new TGeoTranslation("DetPos_transX1_1", X1_pos1[0], X1_pos1[1], X1_pos1[2]);
    TGeoTranslation* DetPos_transX2_1 = new TGeoTranslation("DetPos_transX2_1", X2_pos1[0], X2_pos1[1], X2_pos1[2]);
    TGeoTranslation* DetPos_transY1 = new TGeoTranslation("DetPos_transY1", Y1_pos[0], Y1_pos[1], Y1_pos[2]);
    TGeoTranslation* DetPos_transY2 = new TGeoTranslation("DetPos_transY2", Y2_pos[0], Y2_pos[1], Y2_pos[2]);

    TGeoRotation* DetPos_rot1 = new TGeoRotation("DetPos_rot1", R_angle[0], R_angle[1], R_angle[2]);
    TGeoRotation* DetPos_rot2 = new TGeoRotation("DetPos_rot2", L_angle[0], L_angle[1], L_angle[2]);


    // Solids (shapes)
    // Container X1
    TGeoBBox* SRCArmsContainerX1S = new TGeoBBox("SRCArmsContainerX1S", X1_BCSize, X1_ABSize / 2, X_ZSize / 2);
    TGeoVolume* SRCArmsContainerX1V = new TGeoVolume("SRCArmsContainerX1V", SRCArmsContainerX1S);
    SRCArmsContainerX1V->SetMedium(pMedAir);
    SRCArmsContainerX1V->SetVisibility(kTRUE);
    SRCArmsContainerX1V->SetTransparency(75);

    //Container X2     
    TGeoBBox* SRCArmsContainerX2S = new TGeoBBox("SRCArmsContainerX2S", X1_BCSize, X1_ABSize / 2, X_ZSize / 2);
    TGeoVolume* SRCArmsContainerX2V = new TGeoVolume("SRCArmsContainerX2V", SRCArmsContainerX2S);
    SRCArmsContainerX2V->SetMedium(pMedAir);
    SRCArmsContainerX2V->SetVisibility(kTRUE);
    SRCArmsContainerX2V->SetTransparency(75);

    // Container Y1
    TGeoBBox* SRCArmsContainerY1S = new TGeoBBox("SRCArmsContainerY1S", Y_BCSize / 2, Y_ABSize / 2, Y_ZSize / 2);
    TGeoVolume* SRCArmsContainerY1V = new TGeoVolume("SRCArmsContainerY1V", SRCArmsContainerY1S);
    SRCArmsContainerY1V->SetMedium(pMedAir);
    SRCArmsContainerY1V->SetVisibility(kTRUE);
    SRCArmsContainerY1V->SetTransparency(75);

    // Container Y2
    TGeoBBox* SRCArmsContainerY2S = new TGeoBBox("SRCArmsContainerY2S", Y_BCSize / 2, Y_ABSize / 2, Y_ZSize / 2);
    TGeoVolume* SRCArmsContainerY2V = new TGeoVolume("SRCArmsContainerY2V", SRCArmsContainerY2S);
    SRCArmsContainerY2V->SetMedium(pMedAir);
    SRCArmsContainerY2V->SetVisibility(kTRUE);
    SRCArmsContainerY2V->SetTransparency(75);


    // Active planes X1_1 & X1_2
    TGeoBBox *SRCArmsActivePlane_X1S1 = new TGeoBBox("SRCArmsActivePlane_X1S1", X1_BCSize / 2, X1_ABSize / 2, X_ZSize / 2);
    TGeoVolume *SRCArmsActivePlane_X1V1 = new TGeoVolume("SRCArmsActivePlane_X1V1", SRCArmsActivePlane_X1S1);
    SRCArmsActivePlane_X1V1->SetMedium(pMedBC408);
    SRCArmsActivePlane_X1V1->SetVisibility(kTRUE);
    SRCArmsActivePlane_X1V1->SetTransparency(40);

    TGeoBBox *SRCArmsActivePlane_X1S2 = new TGeoBBox("SRCArmsActivePlane_X1S2", X1_BCSize / 2, X1_ABSize / 2, X_ZSize / 2);
    TGeoVolume *SRCArmsActivePlane_X1V2 = new TGeoVolume("SRCArmsActivePlane_X1V2", SRCArmsActivePlane_X1S2);
    SRCArmsActivePlane_X1V2->SetMedium(pMedBC408);
    SRCArmsActivePlane_X1V2->SetVisibility(kTRUE);
    SRCArmsActivePlane_X1V2->SetTransparency(40);

    // Active planes X2_1 & X2_2
    TGeoBBox *SRCArmsActivePlane_X2S1 = new TGeoBBox("SRCArmsActivePlane_X2S1", X1_BCSize / 2, X1_ABSize / 2, X_ZSize / 2);
    TGeoVolume *SRCArmsActivePlane_X2V1 = new TGeoVolume("SRCArmsActivePlane_X2V1", SRCArmsActivePlane_X2S1);
    SRCArmsActivePlane_X2V1->SetMedium(pMedBC408);
    SRCArmsActivePlane_X2V1->SetVisibility(kTRUE);
    SRCArmsActivePlane_X2V1->SetTransparency(40);

    TGeoBBox *SRCArmsActivePlane_X2S2 = new TGeoBBox("SRCArmsActivePlane_X2S2", X1_BCSize / 2, X1_ABSize / 2, X_ZSize / 2);
    TGeoVolume *SRCArmsActivePlane_X2V2 = new TGeoVolume("SRCArmsActivePlane_X2V2", SRCArmsActivePlane_X2S2);
    SRCArmsActivePlane_X2V2->SetMedium(pMedBC408);
    SRCArmsActivePlane_X2V2->SetVisibility(kTRUE);
    SRCArmsActivePlane_X2V2->SetTransparency(40);

    // Active plane Y1
    TGeoBBox *SRCArmsActivePlane_Y1S = new TGeoBBox("SRCArmsActivePlane_Y1S", Y_BCSize / 2, Y_ABSize / 2, Y_ZSize / 2);
    TGeoVolume *SRCArmsActivePlane_Y1V = new TGeoVolume("SRCArmsActivePlane_Y1V", SRCArmsActivePlane_Y1S);
    SRCArmsActivePlane_Y1V->SetMedium(pMedBC408);
    SRCArmsActivePlane_Y1V->SetVisibility(kTRUE);
    SRCArmsActivePlane_Y1V->SetTransparency(40);

    // Active plane Y2
    TGeoBBox *SRCArmsActivePlane_Y2S = new TGeoBBox("SRCArmsActivePlane_Y2S", Y_BCSize / 2, Y_ABSize / 2, Y_ZSize / 2);
    TGeoVolume *SRCArmsActivePlane_Y2V = new TGeoVolume("SRCArmsActivePlane_Y2V", SRCArmsActivePlane_Y2S);
    SRCArmsActivePlane_Y2V->SetMedium(pMedBC408);
    SRCArmsActivePlane_Y2V->SetVisibility(kTRUE);
    SRCArmsActivePlane_Y2V->SetTransparency(40);


    //Adding volumes to the Top Volume
    SRCArmsContainerX1V->AddNode(SRCArmsActivePlane_X1V1, 0, DetPos_transX_1); //, new TGeoCombiTrans(*DetPos_transX1_1, *DetPos_rot1));
    SRCArmsContainerX1V->AddNode(SRCArmsActivePlane_X1V2, 0, DetPos_transX_2); //, new TGeoCombiTrans(*DetPos_transX1_2, *DetPos_rot1));
    SRCArmsContainerX2V->AddNode(SRCArmsActivePlane_X2V1, 0, DetPos_transX_1); //, new TGeoCombiTrans(*DetPos_transX2_1, *DetPos_rot2));
    SRCArmsContainerX2V->AddNode(SRCArmsActivePlane_X2V2, 0, DetPos_transX_2); //, new TGeoCombiTrans(*DetPos_transX2_2, *DetPos_rot2));

    SRCArmsContainerY1V->AddNode(SRCArmsActivePlane_Y1V, 0); //, new TGeoCombiTrans(*DetPos_transY1, *DetPos_rot1));
    SRCArmsContainerY2V->AddNode(SRCArmsActivePlane_Y2V, 0); //, new TGeoCombiTrans(*DetPos_transY2, *DetPos_rot2));


    ARM->AddNode(SRCArmsContainerX1V, 0, new TGeoCombiTrans(*DetPos_transX1_1, *DetPos_rot1));
    ARM->AddNode(SRCArmsContainerX2V, 0, new TGeoCombiTrans(*DetPos_transX2_1, *DetPos_rot2));
    ARM->AddNode(SRCArmsContainerY1V, 0, new TGeoCombiTrans(*DetPos_transY1, *DetPos_rot1));
    ARM->AddNode(SRCArmsContainerY2V, 0, new TGeoCombiTrans(*DetPos_transY2, *DetPos_rot2));
    top->AddNode(ARM, 0);

    top->SetVisContainers(kTRUE);

    //-----------------Finish----------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");


}
