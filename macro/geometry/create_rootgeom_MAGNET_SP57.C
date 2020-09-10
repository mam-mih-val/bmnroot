//Creating MAGNET SP57 geometry. Macro writes geometry to 'geometry/magnet_sp57.root'
#include "TGeoManager.h"
#include "TFile.h"
#include "TGeoMedium.h"
#include "TGeoXtru.h"
#include "TMath.h"

using namespace TMath;
//---------------------------

TGeoManager* gGeoMan = NULL;

//--------Magnet position-------------------------------------------------------
    const double XMagnetPos = 0.5055 - 0.167 + 7.-3.521; // cm   x=0.5055 cm - Z SRC relative to Z BMN in X direction (D. Baranov),  x=-0.167 - coordinate of the mark in SRC coordinate system (A. Kolesnikov), x = 7 cm - center of the magnet relative to the mark in X direction (M. Patsyuk), x=-3.521 - difference with A. Kolesnikov's measurements (T. Atovullaev)
const double YMagnetPos = -4.62 - 20.53 + 5. + 19.+4.9285; // cm  x=-4.62  cm - Z SRC relative to Z BMN in Y direction (D. Baranov),  x=-20.53 - coordinate of the mark in SRC coordinate system (A. Kolesnikov), x = 5+19 cm - center of the magnet relative to the mark in Y direction (S. Piyadin, technical drawing), x=4.9285 - difference with A. Kolesnikov's measurements (T. Atovullaev)
    const double ZMagnetPos = -576.2; //cm  
//--------Magnet construct parameters-------------------------------------------

    //Yoke parameters:
    const double XSizeYoke = 300.0; //cm
    const double YSizeYoke = 211.0;
    const double ZSizeYoke = 120.0;

    const double XSizeInnerSpaceInYoke = 189.0; // x-inner size of empty space in yoke
    const double YSizeInnerSpaceInYoke = 112.0; // y-inner size of empty space in yoke
    const double ZSizeInnerSpaceInYoke = ZSizeYoke + 0.01; // z-inner size of empty space in yoke

    //Coil parameters:
    const double HCoil = 32.0;
    const double RCoil = 155.0/2.;
    const double YDistanceBetweenCoils = 48.0;

    //Pole parameters
    const double HPole = 5.0;
    const double RPole = 105.0/2.;
    const double YDistanceBetweenPoles = 38.0;

void create_rootgeom_MAGNET_SP57() {

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoObjectName = "magnetSP57";
    const TString geoObjectVersion = "1";
    const TString geoFileName = geoPath + "/geometry/" + geoObjectName + "_"+ geoObjectVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    FairGeoMedium* mIron = geoMedia->getMedium("iron");
    if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
    geoBuild->createMedium(mIron);
    TGeoMedium* pMedIron = gGeoMan->GetMedium("iron");
    if ( ! pMedIron ) Fatal("Main", "Medium iron not found");

    FairGeoMedium* mSteel = geoMedia->getMedium("steel");
    if ( ! mSteel ) Fatal("Main", "FairMedium steel not found");
    geoBuild->createMedium(mSteel);
    TGeoMedium* pMedSteel = gGeoMan->GetMedium("steel");
    if ( ! pMedSteel ) Fatal("Main", "Medium steel not found");

    //the structure of the SP57 coil includes copper and iron with pores for water cooling.
    // To simplify that we assume there is a copper with 0.75 density of real copper, which is called copperSP57.
    FairGeoMedium* mCopper = geoMedia->getMedium("copperSP75");
    if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    TGeoMedium* pMedCopper = gGeoMan->GetMedium("copperSP75");
    if ( ! pMedCopper ) Fatal("Main", "Medium copper not found");

    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoObjectName + "_geom");
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* top = new TGeoVolumeAssembly(geoObjectName);
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

//-----------Solids (shapes)----------------------------------------------------

    TGeoBBox *YokeContainerS = new TGeoBBox("YokeContainerS", XSizeYoke/2., YSizeYoke/2., ZSizeYoke/2.);
    TGeoShape *InnerSpaceInYokeS = new TGeoBBox("InnerSpaceInYokeS", XSizeInnerSpaceInYoke/2, YSizeInnerSpaceInYoke/2, ZSizeInnerSpaceInYoke/2);
    TGeoCompositeShape *YokeS = new TGeoCompositeShape("YokeS", "YokeContainerS-InnerSpaceInYokeS");
    
    TGeoShape* CoilS = new TGeoTube("CoilS", 0., RCoil, HCoil/2.);
    TGeoRotation *rotCoil = new TGeoRotation("rotCoil", 180.0, 90.0, 0.0);

    TGeoShape* PoleS = new TGeoTube("PoleS", 0., RPole, HPole/2.);


//------------Volumes-----------------------------------------------------------
    TGeoVolume *MagnetContainerV = new TGeoVolumeAssembly("Magnet");
    MagnetContainerV->SetMedium(pMedAir);

    TGeoVolume *YokeV = new TGeoVolume("Yoke", YokeS);
    YokeV->SetMedium(pMedSteel);
    YokeV->SetLineColor(kGreen);
    //YokeV->SetVisLeaves(kTRUE);
    //YokeV->SetTransparency(60);
    
    TGeoVolume *CoilV = new TGeoVolume("Coil", CoilS); // copper + steel, the coil has pores for water cooling. density = 0.75!!!!
    CoilV->SetMedium(pMedCopper);
    CoilV->SetLineColor(kRed);
    //CoilV->SetVisLeaves(kTRUE);
    //CoilV->SetTransparency(0);
    
    TGeoVolume *PoleV = new TGeoVolume("Pole", PoleS);
    PoleV->SetMedium(pMedIron);
    PoleV->SetLineColor(kBlue);
    //PoleV->SetVisLeaves(kTRUE);
    //PoleV->SetTransparency(0);
    
//--------------Adding volumes to the TOP Volume--------------------------------
    MagnetContainerV->AddNode(CoilV, 1, new TGeoCombiTrans(0, -(YDistanceBetweenCoils/2 + HCoil/2), 0, rotCoil));
    MagnetContainerV->AddNode(CoilV, 2, new TGeoCombiTrans(0,   YDistanceBetweenCoils/2 + HCoil/2,  0, rotCoil));

    MagnetContainerV->AddNode(PoleV, 1, new TGeoCombiTrans(0, -(YDistanceBetweenPoles/2 + HPole/2), 0, rotCoil));
    MagnetContainerV->AddNode(PoleV, 2, new TGeoCombiTrans(0,   YDistanceBetweenPoles/2 + HPole/2,  0, rotCoil));

    MagnetContainerV->AddNode(YokeV, 0);

    top->AddNode(MagnetContainerV, 0, new TGeoTranslation(XMagnetPos, YMagnetPos, ZMagnetPos));

    top->SetVisContainers(kTRUE);

 //---------------- Finish------------------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
