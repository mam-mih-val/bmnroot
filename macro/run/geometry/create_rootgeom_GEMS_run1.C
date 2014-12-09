/*
  @brief
  Macro for creating geometry root-file describing 1 GEM-module for
  the first BM@N run (<<Shanghai>>)
*/

#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"

using namespace TMath;

//GeoManager
TGeoManager* gGeoMan = NULL;

//Module Parameters
const Double_t XModuleSize = 66.0; //cm
const Double_t YModuleSize = 41.0;
const Double_t ZModuleSize = 2.5;

const Double_t ZModulePosition = 160.0;

const Double_t dXOuterFrame = 5.0;
const Double_t dXInnerFrame = 1.7;
const Double_t dYOuterFrame = 3.75;
const Double_t dYInnerFrame = 1.7;
const Double_t dZSensitiveVolume = 1.0;


const Double_t XSizeSpacer = 10.0;
const Double_t YSizeSpacer = 10.0;
const Double_t SpacerWidth = 0.2;
const Double_t SpacerThickness = dZSensitiveVolume;

//Position of GEM-Module
const Bool_t ModuleCenterPosition = kFALSE;
const Double_t ZDisplacementOfModule = ZModuleSize/2;

//Add spacers or not?
const Bool_t WithSpacers = kTRUE;

void create_rootgeom_GEMS_run1() {

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
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "run1";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if ( ! mCarbon) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    TGeoMedium* pMedCarbon = gGeoMan->GetMedium("carbon");
    if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    TGeoMedium* pMedFiberGlass = gGeoMan->GetMedium("fiberglass");
    if ( ! pMedFiberGlass ) Fatal("Main", "Medium fiberglass not found");

    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030 ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("arco27030");
    if ( ! pMedArCO27030 ) Fatal("Main", "Medium arco27030 not found");
    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

// Define TOP Geometry
    TGeoVolume* GEMSTop = new TGeoVolumeAssembly(geoDetectorName);
    GEMSTop->SetMedium(pMedAir);

//Shapes and volume pointers
    TGeoBBox *ModuleS;
    TGeoVolume *ModuleV;

    TGeoBBox *VerticalOuterFrameS;
    TGeoBBox *HorizontalOuterFrameS;
    TGeoBBox *HorizontalInnerFrameS;
    TGeoBBox *VerticalInnerFrameS;

    TGeoVolume *VerticalOuterFrameV;
    TGeoVolume *HorizontalOuterFrameV;
    TGeoVolume *HorizontalInnerFrameV;
    TGeoVolume *VerticalInnerFrameV;

    TGeoBBox *SensitiveVolumeS;
    TGeoVolume *SensitiveVolumeV;

    TGeoBBox *HorizontalSpacerS;
    TGeoBBox *VerticalSpacerPartS;
    TGeoBBox *VerticalSpacerSmallPartS;

    TGeoVolume *HorizontalSpacerV;
    TGeoVolume *VerticalSpacerPartV;
    TGeoVolume *VerticalSpacerSmallPartV;

    TGeoVolumeAssembly* SpacerBoxV;
    TGeoVolumeAssembly* FrameBoxV;

    TGeoTranslation *VerticalOuterFrameVPosition;
    TGeoTranslation *HorizontalOuterFrameVPosition;
    TGeoTranslation *HorizontalInnerFrameVPosition;
    TGeoTranslation *VerticalInnerFrameVPosition;
    TGeoRotation *SpacerBoxVRot;
    TGeoTranslation *SensitiveVolumePosition;
//------------------------------------------------------------------------------

//Creating spacers
    Int_t NHorizontalSpacers = (Int_t)((YModuleSize-(dYInnerFrame+dYOuterFrame))/(YSizeSpacer+SpacerWidth));
    Int_t NVerticalSpacers = (Int_t)((XModuleSize-(dXInnerFrame+dXOuterFrame))/(XSizeSpacer+SpacerWidth));
    Double_t YSizeSmallPart = (YModuleSize-(dYInnerFrame+dYOuterFrame))/(YSizeSpacer+SpacerWidth);
    YSizeSmallPart -= (Int_t)YSizeSmallPart;
    YSizeSmallPart *= YSizeSpacer+SpacerWidth;

    HorizontalSpacerS = new TGeoBBox("HorizontalSpacerS", (XModuleSize-(dXInnerFrame+dXOuterFrame))/2, SpacerWidth/2, SpacerThickness/2);
    HorizontalSpacerV = new TGeoVolume("HorizontalSpacerV", HorizontalSpacerS);
    HorizontalSpacerV->SetMedium(pMedCarbon);
    HorizontalSpacerV->SetLineColor(kYellow);
    HorizontalSpacerV->SetTransparency(10);

    VerticalSpacerPartS = new TGeoBBox("VerticalSpacerPartS", SpacerWidth/2, YSizeSpacer/2, SpacerThickness/2);
    VerticalSpacerPartV = new TGeoVolume("VerticalSpacerPartV", VerticalSpacerPartS);
    VerticalSpacerPartV->SetMedium(pMedCarbon);
    VerticalSpacerPartV->SetLineColor(kYellow);
    VerticalSpacerPartV->SetTransparency(10);

    VerticalSpacerSmallPartS = new TGeoBBox("VerticalSpacerSmallPartS", SpacerWidth/2, YSizeSmallPart/2, SpacerThickness/2);
    VerticalSpacerSmallPartV = new TGeoVolume("VerticalSpacerSmallPartV", VerticalSpacerSmallPartS);
    VerticalSpacerSmallPartV->SetMedium(pMedCarbon);
    VerticalSpacerSmallPartV->SetLineColor(kYellow);
    VerticalSpacerSmallPartV->SetTransparency(10);

    SpacerBoxV = new TGeoVolumeAssembly("SpacerBoxV");
    SpacerBoxV->SetMedium(pMedArCO27030);

    Int_t hpart_cnt = 0;
    Int_t vpart_cnt = 0;
    for(Int_t ihspacer = 0; ihspacer < NHorizontalSpacers; ihspacer++) {
        Double_t ydisph = ((YModuleSize -(dYInnerFrame+dYOuterFrame))/2 - SpacerWidth/2) - YSizeSpacer*(ihspacer+1) - SpacerWidth*ihspacer;
        SpacerBoxV->AddNode(HorizontalSpacerV, hpart_cnt, new TGeoTranslation(0, ydisph , 0));
        hpart_cnt++;

        for(Int_t ivspacer = 0; ivspacer < NVerticalSpacers; ivspacer++) {
            Double_t xdispv = (-(XModuleSize -(dXInnerFrame+dXOuterFrame))/2 + SpacerWidth/2) + XSizeSpacer*(ivspacer+1) + SpacerWidth*ivspacer;
            Double_t ydispv = (YModuleSize -(dYInnerFrame+dYOuterFrame))/2 - YSizeSpacer/2 - (YSizeSpacer+SpacerWidth)*ihspacer;
            SpacerBoxV->AddNode(VerticalSpacerPartV, vpart_cnt, new TGeoTranslation(xdispv, ydispv, 0));
            vpart_cnt++;

            if(ihspacer == NHorizontalSpacers-1) {
                Double_t ydispvs = (YModuleSize -(dYInnerFrame+dYOuterFrame))/2 - YSizeSpacer/2 - (YSizeSpacer+SpacerWidth)*(ihspacer+1) + (YSizeSpacer-YSizeSmallPart)/2;
                SpacerBoxV->AddNode(VerticalSpacerSmallPartV, ivspacer, new TGeoTranslation(xdispv, ydispvs, 0));
            }
        }
    }

//Creating sensitive shape and volume + adding spacer box to sensitive volume
    SensitiveVolumeS = new TGeoBBox("SensitiveVolumeS", (XModuleSize -(dXOuterFrame+dXInnerFrame))/2, (YModuleSize-(dYOuterFrame+dYInnerFrame))/2, dZSensitiveVolume/2);

    SensitiveVolumeV = new TGeoVolume("SensitiveVolume_SensorV", SensitiveVolumeS);
    SensitiveVolumeV->SetMedium(pMedArCO27030);
    SensitiveVolumeV->SetLineColor(kRed);
    SensitiveVolumeV->SetTransparency(50);
    //SensitiveVolumeV->SetVisibility(kFALSE);

    SpacerBoxVRot = new TGeoRotation();
    SpacerBoxVRot->SetAngles(180, 0, 0);

    if(WithSpacers)
        SensitiveVolumeV->AddNode(SpacerBoxV, 0, SpacerBoxVRot);

//Creating border frames shapes and volumes
    VerticalOuterFrameS = new TGeoBBox("VerticalOuterFrameS", dXOuterFrame/2, YModuleSize/2, ZModuleSize/2);
    VerticalOuterFrameV = new TGeoVolume("VerticalOuterFrameV", VerticalOuterFrameS);
    VerticalOuterFrameV->SetMedium(pMedFiberGlass);
    VerticalOuterFrameV->SetLineColor(kBlue);
    VerticalOuterFrameV->SetTransparency(40);

    HorizontalOuterFrameS = new TGeoBBox("HorizontalOuterFrameS", (XModuleSize-dXOuterFrame)/2, dYOuterFrame/2, (ZModuleSize-dZSensitiveVolume)/2);
    HorizontalOuterFrameV = new TGeoVolume("HorizontalOuterFrameV", HorizontalOuterFrameS);
    HorizontalOuterFrameV->SetMedium(pMedFiberGlass);
    HorizontalOuterFrameV->SetLineColor(kBlue);
    HorizontalOuterFrameV->SetTransparency(40);

    HorizontalInnerFrameS = new TGeoBBox("HorizontalInnerFrameS", (XModuleSize-dXOuterFrame)/2, dYInnerFrame/2, (ZModuleSize-dZSensitiveVolume)/2);
    HorizontalInnerFrameV = new TGeoVolume("HorizontalInnerFrameV", HorizontalInnerFrameS);
    HorizontalInnerFrameV->SetMedium(pMedFiberGlass);
    HorizontalInnerFrameV->SetLineColor(kBlue);
    HorizontalInnerFrameV->SetTransparency(40);

    VerticalInnerFrameS = new TGeoBBox("VerticalInnerFrameS", dXInnerFrame/2, (YModuleSize-dYInnerFrame-dYOuterFrame)/2, (ZModuleSize-dZSensitiveVolume)/2);
    VerticalInnerFrameV = new TGeoVolume("VerticalInnerFrameV", VerticalInnerFrameS);
    VerticalInnerFrameV->SetMedium(pMedFiberGlass);
    VerticalInnerFrameV->SetLineColor(kBlue);
    VerticalInnerFrameV->SetTransparency(40);

//Creating box frame from parts
    FrameBoxV = new TGeoVolumeAssembly("FrameBoxV");
    FrameBoxV->SetMedium(pMedAir);

    VerticalOuterFrameVPosition = new TGeoTranslation();
    HorizontalOuterFrameVPosition = new TGeoTranslation();
    HorizontalInnerFrameVPosition = new TGeoTranslation();
    VerticalInnerFrameVPosition = new TGeoTranslation();

    VerticalOuterFrameVPosition->SetTranslation(-(XModuleSize/2 - dXOuterFrame/2), 0.0, 0.0);
    HorizontalOuterFrameVPosition->SetTranslation(dXOuterFrame/2, YModuleSize/2-dYOuterFrame/2, -(ZModuleSize/2 - (ZModuleSize-dZSensitiveVolume)/2));
    HorizontalInnerFrameVPosition->SetTranslation(dXOuterFrame/2, -(YModuleSize/2-dYInnerFrame/2), -(ZModuleSize/2 - (ZModuleSize-dZSensitiveVolume)/2));
    VerticalInnerFrameVPosition->SetTranslation(XModuleSize/2 - dXInnerFrame/2, -(dYOuterFrame/2-dYInnerFrame/2), -(ZModuleSize/2 - (ZModuleSize-dZSensitiveVolume)/2));

    FrameBoxV->AddNode(VerticalOuterFrameV, 0, VerticalOuterFrameVPosition);
    FrameBoxV->AddNode(HorizontalOuterFrameV, 0, HorizontalOuterFrameVPosition);
    FrameBoxV->AddNode(HorizontalInnerFrameV, 0, HorizontalInnerFrameVPosition);
    FrameBoxV->AddNode(VerticalInnerFrameV, 0, VerticalInnerFrameVPosition);

//Creating module
    ModuleS = new TGeoBBox("ModuleS", XModuleSize/2, YModuleSize/2, ZModuleSize/2);
    ModuleV = new TGeoVolume("ModuleV", ModuleS);
    ModuleV->SetMedium(pMedAir);
    ModuleV->SetLineColor(kYellow);
    ModuleV->SetTransparency(40);
    //ModuleV->SetVisibility(kFALSE);

    SensitiveVolumePosition = new TGeoTranslation();
    SensitiveVolumePosition->SetTranslation((XModuleSize-(XModuleSize-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame, (YModuleSize -(YModuleSize -(dYOuterFrame+dYInnerFrame)))/2 - dYOuterFrame, (ZModuleSize-dZSensitiveVolume)/2);

    ModuleV->AddNode(FrameBoxV, 0);
    ModuleV->AddNode(SensitiveVolumeV, 0, SensitiveVolumePosition);

    GEMSTop->AddNode(ModuleV, 0, new TGeoTranslation(0.0, 0.0, ZModulePosition));

    if(ModuleCenterPosition == kTRUE) {
        top->AddNode(GEMSTop, 0);
    }
    else {
        top->AddNode(GEMSTop, 0, new TGeoTranslation(0, 0, ZDisplacementOfModule));
    }

    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
