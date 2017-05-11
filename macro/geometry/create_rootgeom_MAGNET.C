//Creating MAGNET geometry. Macro writes geometry to 'geometry/magnet_modified.root'
#include "TGeoManager.h"
#include "TFile.h"
#include "TGeoMedium.h"
#include "TGeoXtru.h"
#include "TMath.h"

using namespace TMath;
//---------------------------

TGeoManager* gGeoMan = NULL;

//--------Magnet position-------------------------------------------------------
    const double XMagnetPos = 0.0;
    const double YMagnetPos = 0.0;
    const double ZMagnetPos = 124.5; //cm

//--------Magnet construct parameters-------------------------------------------

    //Yoke parameters:
    const double XSizeYoke = 700.0; //cm
    const double YSizeYoke = 448.0;
    const double ZSizeYoke = 260.0;

    const double XSizeInnerSpaceInYoke = 460.0; // x-inner size of empty space in yoke
    const double YSizeInnerSpaceInYoke = 230.0; // y-inner size of empty space in yoke
    const double ZSizeInnerSpaceInYoke = ZSizeYoke + 0.01; // z-inner size of empty space in yoke

    double AngleOfBevelYoke = 30.0;
    double AdjCatetOfBevelYoke = 80.0;
    double OppCatetOfBevelYoke = AdjCatetOfBevelYoke*(Tan(AngleOfBevelYoke*Pi()/180));

    //Coil parameters:
    const double XSizeCoil = 429.2;
    const double YSizeCoil = 50.0;
    const double ZSizeCoil = 514.2;

    const double WidthCoil = 110.0;
    const double RMinCoil = 25.0;
    const double RMaxCoil = 135.0;

    const double LengthCoilShortSide = 209.2 - 2*RMinCoil;
    const double LengthCoilLongSide = 294.2 - 2*RMinCoil;

    const double YDistanceBetweenCoils = 120.0;

    //Pole parameters
    const double YDistanceBetweenPoles = 107.0;

    const double XSizePole = 168.0;
    const double YSizePole = (YSizeInnerSpaceInYoke - YDistanceBetweenPoles)/2;
    const double ZSizePole = 249.0;
    double CatetOfBevelPole = 30.0;


void create_rootgeom_MAGNET() {

    // Load the necessary FairRoot libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

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
    const TString geoObjectName = "magnet";
    const TString geoObjectVersion = "modified";
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

    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if ( ! mCopper ) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    TGeoMedium* pMedCopper = gGeoMan->GetMedium("copper");
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

    //Start of yoke shape creating
    const Int_t NYokeProfilePoints = 8;
    const Int_t NYokeZPlanes = 2;
    TGeoXtru *YokeContainerS = new TGeoXtru(NYokeZPlanes);
    YokeContainerS->SetName("YokeContainerS");

    Double_t XYokeProfile[NYokeProfilePoints];
        XYokeProfile[0] = -((XSizeYoke-2*OppCatetOfBevelYoke)/2);
        XYokeProfile[1] = -(XSizeYoke/2);
        XYokeProfile[2] = -(XSizeYoke/2);
        XYokeProfile[3] = -((XSizeYoke-2*OppCatetOfBevelYoke)/2);
        XYokeProfile[4] = (XSizeYoke-2*OppCatetOfBevelYoke)/2;
        XYokeProfile[5] = XSizeYoke/2;
        XYokeProfile[6] = XSizeYoke/2;
        XYokeProfile[7] = (XSizeYoke-2*OppCatetOfBevelYoke)/2;

    Double_t YYokeProfile[NYokeProfilePoints];
        YYokeProfile[0] = -(YSizeYoke/2);
        YYokeProfile[1] = -((YSizeYoke-2*AdjCatetOfBevelYoke)/2);
        YYokeProfile[2] = (YSizeYoke-2*AdjCatetOfBevelYoke)/2;
        YYokeProfile[3] = YSizeYoke/2;
        YYokeProfile[4] = YSizeYoke/2;
        YYokeProfile[5] = (YSizeYoke-2*AdjCatetOfBevelYoke)/2;
        YYokeProfile[6] = -((YSizeYoke-2*AdjCatetOfBevelYoke)/2);
        YYokeProfile[7] = -YSizeYoke/2;

    YokeContainerS->DefinePolygon(NYokeProfilePoints, XYokeProfile, YYokeProfile);
    YokeContainerS->DefineSection(0, -ZSizeYoke/2, 0,0,1.0);
    YokeContainerS->DefineSection(1, ZSizeYoke/2, 0,0,1.0);

    TGeoShape *InnerSpaceInYokeS = new TGeoBBox("InnerSpaceInYokeS", XSizeInnerSpaceInYoke/2, YSizeInnerSpaceInYoke/2, ZSizeInnerSpaceInYoke/2);

    TGeoCompositeShape *YokeS = new TGeoCompositeShape("YokeS", "YokeContainerS-InnerSpaceInYokeS");
    //End of yoke shape creating

    //Start of coil shape creating
    TGeoShape *CoilCornerS = new TGeoTubeSeg("CoilCornerS", RMinCoil, RMaxCoil, YSizeCoil/2, 0, 90);

    TGeoRotation *rot_CoilCorner1 = new TGeoRotation("", 90.0, 0.0, 180.0, 0.0, 90.0, 90.0);
    TGeoTranslation *tran_CoilCorner1 = new TGeoTranslation("", XSizeCoil/2 - (WidthCoil+RMinCoil), 0, -(ZSizeCoil/2 - (WidthCoil+RMinCoil)));
    TGeoCombiTrans *combitran_CoilCorner1 = new TGeoCombiTrans(*tran_CoilCorner1, *rot_CoilCorner1);
    combitran_CoilCorner1->SetName("combitran_CoilCorner1");
    combitran_CoilCorner1->RegisterYourself();

    TGeoRotation *rot_CoilCorner2 = new TGeoRotation("", 90.0, 180.0, 180.0, 0.0, 90.0, 270.0);
    TGeoTranslation *tran_CoilCorner2 = new TGeoTranslation("", -(XSizeCoil/2 - (WidthCoil+RMinCoil)), 0, -(ZSizeCoil/2 - (WidthCoil+RMinCoil)));
    TGeoCombiTrans *combitran_CoilCorner2 = new TGeoCombiTrans(*tran_CoilCorner2, *rot_CoilCorner2);
    combitran_CoilCorner2->SetName("combitran_CoilCorner2");
    combitran_CoilCorner2->RegisterYourself();

    TGeoRotation *rot_CoilCorner3 = new TGeoRotation("", 90.0, 180.0, 0.0, 0.0, 90.0, 90.0);
    TGeoTranslation *tran_CoilCorner3 = new TGeoTranslation("", -(XSizeCoil/2 - (WidthCoil+RMinCoil)), 0, ZSizeCoil/2 - (WidthCoil+RMinCoil));
    TGeoCombiTrans *combitran_CoilCorner3 = new TGeoCombiTrans(*tran_CoilCorner3, *rot_CoilCorner3);
    combitran_CoilCorner3->SetName("combitran_CoilCorner3");
    combitran_CoilCorner3->RegisterYourself();

    TGeoRotation *rot_CoilCorner4 = new TGeoRotation("", 90.0, 0.0, 0.0, 0.0, 90.0, 270.0);
    TGeoTranslation *tran_CoilCorner4 = new TGeoTranslation("", XSizeCoil/2 - (WidthCoil+RMinCoil), 0, ZSizeCoil/2 - (WidthCoil+RMinCoil));
    TGeoCombiTrans *combitran_CoilCorner4 = new TGeoCombiTrans(*tran_CoilCorner4, *rot_CoilCorner4);
    combitran_CoilCorner4->SetName("combitran_CoilCorner4");
    combitran_CoilCorner4->RegisterYourself();

    TGeoShape *CoilShortSideS = new TGeoBBox("CoilShortSideS", LengthCoilShortSide/2, YSizeCoil/2, WidthCoil/2);
    TGeoShape *CoilLongSideS = new TGeoBBox("CoilLongSideS", WidthCoil/2, YSizeCoil/2, LengthCoilLongSide/2);

    TGeoTranslation *tran_CoilShortSide1 = new TGeoTranslation("", 0, 0, -(ZSizeCoil/2 - WidthCoil/2));
    tran_CoilShortSide1->SetName("tran_CoilShortSide1");
    tran_CoilShortSide1->RegisterYourself();

    TGeoTranslation *tran_CoilShortSide2 = new TGeoTranslation("", 0, 0, ZSizeCoil/2 - WidthCoil/2);
    tran_CoilShortSide2->SetName("tran_CoilShortSide2");
    tran_CoilShortSide2->RegisterYourself();

    TGeoTranslation *tran_CoilLongSide1 = new TGeoTranslation("", -(XSizeCoil/2 - WidthCoil/2), 0, 0);
    tran_CoilLongSide1->SetName("tran_CoilLongSide1");
    tran_CoilLongSide1->RegisterYourself();

    TGeoTranslation *tran_CoilLongSide2 = new TGeoTranslation("", XSizeCoil/2 - WidthCoil/2, 0, 0);
    tran_CoilLongSide2->SetName("tran_CoilLongSide2");
    tran_CoilLongSide2->RegisterYourself();

    TGeoCompositeShape *CoilS = new TGeoCompositeShape("CoilTS", "CoilCornerS:combitran_CoilCorner1"
                                                                 "+CoilCornerS:combitran_CoilCorner2"
                                                                 "+CoilCornerS:combitran_CoilCorner3"
                                                                 "+CoilCornerS:combitran_CoilCorner4"
                                                                 "+CoilShortSideS:tran_CoilShortSide1"
                                                                 "+CoilShortSideS:tran_CoilShortSide2"
                                                                 "+CoilLongSideS:tran_CoilLongSide1"
                                                                 "+CoilLongSideS:tran_CoilLongSide2");
    //End of coil shape creating

    //Start of pole shape creating
    const Int_t NPoleProfilePoints = 8;
    const Int_t NPoleZPlanes = 2;
    TGeoXtru *PoleS = new TGeoXtru(NPoleZPlanes);
    PoleS->SetName("PoleS");

    Double_t XPoleProfile[NPoleProfilePoints];
        XPoleProfile[0] = -((XSizePole-2*CatetOfBevelPole)/2);
        XPoleProfile[1] = -(XSizePole/2);
        XPoleProfile[2] = -(XSizePole/2);
        XPoleProfile[3] = -((XSizePole-2*CatetOfBevelPole)/2);
        XPoleProfile[4] = (XSizePole-2*CatetOfBevelPole)/2;
        XPoleProfile[5] = XSizePole/2;
        XPoleProfile[6] = XSizePole/2;
        XPoleProfile[7] = (XSizePole-2*CatetOfBevelPole)/2;

    Double_t ZPoleProfile[NPoleProfilePoints];
        ZPoleProfile[0] = -(ZSizePole/2);
        ZPoleProfile[1] = -((ZSizePole-2*CatetOfBevelPole)/2);
        ZPoleProfile[2] = (ZSizePole-2*CatetOfBevelPole)/2;
        ZPoleProfile[3] = ZSizePole/2;
        ZPoleProfile[4] = ZSizePole/2;
        ZPoleProfile[5] = (ZSizePole-2*CatetOfBevelPole)/2;
        ZPoleProfile[6] = -((ZSizePole-2*CatetOfBevelPole)/2);
        ZPoleProfile[7] = -ZSizePole/2;

    PoleS->DefinePolygon(NPoleProfilePoints, XPoleProfile, ZPoleProfile);
    PoleS->DefineSection(0, -YSizePole/2, 0,0,1.0);
    PoleS->DefineSection(1, YSizePole/2, 0,0,1.0);

    TGeoRotation *rot_Pole = new TGeoRotation("", 90.0, 0.0, 180.0, 0.0, 90.0, 90.0);


//------------Volumes-----------------------------------------------------------
    TGeoVolume *MagnetContainerV = new TGeoVolumeAssembly("Magnet");
    MagnetContainerV->SetMedium(pMedAir);

    TGeoVolume *YokeV = new TGeoVolume("Yoke", YokeS);
    YokeV->SetMedium(pMedSteel);
    YokeV->SetLineColor(kBlue);
    //YokeV->SetVisLeaves(kTRUE);
    //YokeV->SetTransparency(60);

    TGeoVolume *CoilV = new TGeoVolume("Coil", CoilS);
    CoilV->SetMedium(pMedCopper);
    CoilV->SetLineColor(kYellow);
    //CoilV->SetVisLeaves(kTRUE);
    //CoilV->SetTransparency(0);

    TGeoVolume *PoleV = new TGeoVolume("Pole", PoleS);
    PoleV->SetMedium(pMedIron);
    PoleV->SetLineColor(kBlue);
    //PoleV->SetVisLeaves(kTRUE);
    //PoleV->SetTransparency(0);

//--------------Adding volumes to the TOP Volume--------------------------------
    MagnetContainerV->AddNode(CoilV, 1, new TGeoTranslation(0, -(YDistanceBetweenCoils/2 + YSizeCoil/2), 0));
    MagnetContainerV->AddNode(CoilV, 2, new TGeoTranslation(0, YDistanceBetweenCoils/2 + YSizeCoil/2, 0));

    MagnetContainerV->AddNode(PoleV, 1, new TGeoCombiTrans(0, -(YDistanceBetweenPoles/2 + YSizePole/2), 0,rot_Pole));
    MagnetContainerV->AddNode(PoleV, 2, new TGeoCombiTrans(0, YDistanceBetweenPoles/2 + YSizePole/2, 0,rot_Pole));

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
