/*
 * Use this macro to create
 * ROOT geometry for the SRC SRC in the future configuration
 * including passive volumes (frames, al. cable supports, ...)
 *
 * Author: Baranov D.
 * Created: 19.08.2021
 * Updated: ---
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

//Set Parameters of CSC --------------------------------------------------------
const Int_t NStations = 2;      //stations in the detector
const Int_t NMaxModules = 2;    //max. number of modules in a station

//(X-Y-Z)Positions of stations (distances from the origin to the center of the outboard side (not sens.) which is nearest to the target)
const Double_t XStationPositions[NStations] = {
    +109.89 + 0.5055 /* +110.3955 */, //station 0 (left arm)
    -109.89 + 0.5055 /* -109.3845 */  //station 1 (right arm)
};
const Double_t YStationPositions[NStations] = {
    +0.0 - 4.623, //station 0 (left arm)
    +0.0 - 4.623  //station 1 (right arm)
};
const Double_t ZStationPositions[NStations] = {
    +182.89 - 647.48 /* -464.59 */, //station 0 (left arm)
    +182.89 - 647.48 /* -464.59 */ //station 1 (right arm)
};
//------------------------------------------------------------------------------

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {0.0, 0.0}, //station 0
    {0.0, 0.0}, //station 1
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {27.1875, -27.1875}, //station 0
    {27.1875, -27.1875}, //station 1
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {1.25, 1.25}, //station 0
    {1.25, 1.25}, //station 1
};

//Sizes of elements (cm) -------------------------------------------------------

//Gas volume sizes (module: half of station) (AS MODULE SIZES)
const Double_t XGasSize = 113.0;
const Double_t YGasSize = 54.375;
const Double_t ZGasSize = 0.76;

//Sensitive volume sizes (module: half of station)
const Double_t XSensSize = 112.9;
const Double_t YSensSize = 53.25; //106.5*0.5
const Double_t ZSensSize = 0.76;
//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedArCO27030 = 0;
TGeoMedium *pMedArgonIsobutane7525 = 0;
TGeoMedium *pMedRohacell = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule(TString module_name);
TGeoVolume *CreateFrameForModule(TString frame_name);

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    //carbon medium
    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if ( ! mCarbon  ) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    pMedCarbon = gGeoManager->GetMedium("carbon");
    if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

    //fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass  ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if ( ! pMedFiberGlass  ) Fatal("Main", "Medium fiberglass not found");

    //arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030  ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030= gGeoManager->GetMedium("arco27030");
    if ( ! pMedArCO27030  ) Fatal("Main", "Medium arco27030 not found");

    //ArgonIsobutane (75/25): Ar-iC4H10 medium
    FairGeoMedium* mArgonIsobutane7525 = geoMedia->getMedium("ArgonIsobutane7525");
    if ( ! mArgonIsobutane7525  ) Fatal("Main", "FairMedium ArgonIsobutane7525 not found");
    geoBuild->createMedium(mArgonIsobutane7525);
    pMedArgonIsobutane7525= gGeoManager->GetMedium("ArgonIsobutane7525");
    if ( ! pMedArgonIsobutane7525  ) Fatal("Main", "Medium ArgonIsobutane7525 not found");

    //Rohacell medium
    FairGeoMedium* mRohacell = geoMedia->getMedium("rohacellhf71");
    if ( ! mRohacell  ) Fatal("Main", "FairMedium rohacellhf71 not found");
    geoBuild->createMedium(mRohacell);
    pMedRohacell= gGeoManager->GetMedium("rohacellhf71");
    if ( ! pMedRohacell  ) Fatal("Main", "Medium rohacellhf71 not found");
}

void create_rootgeom_CSC_SRCFutureConfig2021() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "CSC";
    const TString geoDetectorVersion = "SRCFutureConfig2021";
    const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    DefineRequiredMedia(geoMedia, geoBuild);
    // -------------------------------------------------------------------------

    // --------------   Create geometry and top volume  ------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // -------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* CSC = new TGeoVolumeAssembly(geoDetectorName);
    CSC->SetMedium(pMedAir);

    //station 0 (consisting of two modules)
    if(1) {
        Int_t stationNum = 0; //station number

        TGeoVolume *station = CreateStation(TString("station")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station->GetName());

        TGeoVolume *frame0 = CreateFrameForModule(TString("frame0_")+station->GetName());
        TGeoVolume *frame1 = CreateFrameForModule(TString("frame1_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+0.5*ZGasSize);
            module0_transform->RotateY(+31.0); //deg

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->RotateZ(180);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+0.5*ZGasSize);
            module1_transform->RotateY(+31.0); //deg

        TGeoCombiTrans *frame0_transform = new TGeoCombiTrans();
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+0.5*ZGasSize);
            frame0_transform->RotateY(+31.0); //deg

        TGeoCombiTrans *frame1_transform = new TGeoCombiTrans();
            frame1_transform->RotateZ(180);
            frame1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+0.5*ZGasSize);
            frame1_transform->RotateY(+31.0); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform));

        station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform));
        station->AddNode(frame1, 0, new TGeoCombiTrans(*frame1_transform));

        CSC->AddNode(station, 0, station_transform);
    }

    //station 1 (consisting of two modules)
    if(1) {
        Int_t stationNum = 1; //station number

        TGeoVolume *station = CreateStation(TString("station")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station->GetName());

        TGeoVolume *frame0 = CreateFrameForModule(TString("frame0_")+station->GetName());
        TGeoVolume *frame1 = CreateFrameForModule(TString("frame1_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+0.5*ZGasSize);
            module0_transform->RotateY(-31.0); //deg

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->RotateZ(180);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+0.5*ZGasSize);
            module1_transform->RotateY(-31.0); //deg

        TGeoCombiTrans *frame0_transform = new TGeoCombiTrans();
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+0.5*ZGasSize);
            frame0_transform->RotateY(-31.0); //deg

        TGeoCombiTrans *frame1_transform = new TGeoCombiTrans();
            frame1_transform->RotateZ(180);
            frame1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+0.5*ZGasSize);
            frame1_transform->RotateY(-31.0); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform));

        station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform));
        station->AddNode(frame1, 0, new TGeoCombiTrans(*frame1_transform));

        CSC->AddNode(station, 0, station_transform);
    }


    top->AddNode(CSC, 0);
    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    //gGeoMan->RestoreMasterVolume();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}

TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule(TString module_name) {

    //shapes
    TGeoShape *moduleS = new TGeoBBox(TString("moduleS")+=TString("_") + module_name, XGasSize*0.5, YGasSize*0.5, ZGasSize*0.5);
    TGeoShape *sensS = new TGeoBBox(TString("sensS")+=TString("_") + module_name, XSensSize*0.5, YSensSize*0.5, ZSensSize*0.5);

    //volumes
    TGeoVolume *moduleV = new TGeoVolume(TString("moduleV")+=TString("_") + module_name, moduleS);
    TGeoVolume *sensV = new TGeoVolume(TString("SensorV")+=TString("_") + module_name, sensS); 

    //medium
    if(pMedArgonIsobutane7525) {
        moduleV->SetMedium(pMedArgonIsobutane7525);
        sensV->SetMedium(pMedArgonIsobutane7525);
    }
    else Fatal("Main", "Invalid medium for moduleV and sensV volumes!");

    //volume visual property (transparency)
    moduleV->SetLineColor(TColor::GetColor("#47ffca"));
    moduleV->SetTransparency(30);
    sensV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensV->SetTransparency(30);

    TGeoCombiTrans *sensV_transf[1];
    sensV_transf[0] = new TGeoCombiTrans();
    sensV_transf[0]->SetDx(0.0);
    sensV_transf[0]->SetDy(-(YGasSize - YSensSize)*0.5);
    sensV_transf[0]->SetDz(0.0);

    moduleV->AddNode(sensV, 0, sensV_transf[0]);

    return moduleV;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateFrameForModule(TString frame_name) {

    //frame
    TGeoVolume *frameA = new TGeoVolumeAssembly(frame_name);
    frameA->SetMedium(pMedAir);

    //vertical frames ----------------------------------------------------------
    Double_t verticalFrame_XSize = 4.5; //cm
    Double_t verticalFrame_YSize = YGasSize; //cm
    Double_t verticalFrame_ZSize = 3.25; //cm

    TGeoShape *verticalFrameS = new TGeoBBox(TString("verticalFrameS")+=TString("_") + frameA->GetName(), verticalFrame_XSize*0.5, verticalFrame_YSize*0.5, verticalFrame_ZSize*0.5);

    TGeoVolume *verticalFrameV = new TGeoVolume(TString("verticalFrameV")+=TString("_") + frameA->GetName(), verticalFrameS);

    //volume medium
    TGeoMedium *verticalFrameV_medium = pMedFiberGlass;
    if(verticalFrameV_medium) {
        verticalFrameV->SetMedium(verticalFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for verticalFrameV!");

    //volume visual property (transparency)
    verticalFrameV->SetLineColor(TColor::GetColor("#9999ff"));
    verticalFrameV->SetTransparency(0);

    TGeoCombiTrans *verticalFrameV_transf[2];

    verticalFrameV_transf[0] = new TGeoCombiTrans();
    verticalFrameV_transf[0]->SetDx(+(XGasSize*0.5 + verticalFrame_XSize*0.5));
    verticalFrameV_transf[0]->SetDy(0.0);
    verticalFrameV_transf[0]->SetDz(0.0);

    verticalFrameV_transf[1] = new TGeoCombiTrans();
    verticalFrameV_transf[1]->SetDx(-(XGasSize*0.5 + verticalFrame_XSize*0.5));
    verticalFrameV_transf[1]->SetDy(0.0);
    verticalFrameV_transf[1]->SetDz(0.0);

    frameA->AddNode(verticalFrameV, 0, verticalFrameV_transf[0]);
    frameA->AddNode(verticalFrameV, 1, verticalFrameV_transf[1]);
    //--------------------------------------------------------------------------

    //horizontal frame ---------------------------------------------------------
    Double_t horizontalFrame_XSize = XGasSize + verticalFrame_XSize*2; //cm
    Double_t horizontalFrame_YSize = 4.5; //cm
    Double_t horizontalFrame_ZSize = 3.25; //cm

    TGeoShape *horizontalFrameS = new TGeoBBox(TString("horizontalFrameS")+=TString("_") + frameA->GetName(), horizontalFrame_XSize*0.5, horizontalFrame_YSize*0.5, horizontalFrame_ZSize*0.5);

    TGeoVolume *horizontalFrameV = new TGeoVolume(TString("horizontalFrameV")+=TString("_") + frameA->GetName(), horizontalFrameS);

    //volume medium
    TGeoMedium *horizontalFrameV_medium = pMedFiberGlass;
    if(horizontalFrameV_medium) {
        horizontalFrameV->SetMedium(horizontalFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for horizontalFrameV!");

    //volume visual property (transparency)
    horizontalFrameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontalFrameV->SetTransparency(0);

    TGeoCombiTrans *horizontalFrameV_transf[1];

    horizontalFrameV_transf[0] = new TGeoCombiTrans();
    horizontalFrameV_transf[0]->SetDx(0.0);
    horizontalFrameV_transf[0]->SetDy(+(YGasSize*0.5 + horizontalFrame_YSize*0.5));
    horizontalFrameV_transf[0]->SetDz(0.0);

    frameA->AddNode(horizontalFrameV, 0, horizontalFrameV_transf[0]);
    //--------------------------------------------------------------------------

    //panels -------------------------------------------------------------------
    Double_t fiberGlassPanel_XSize = XGasSize; //cm
    Double_t fiberGlassPanel_YSize = YGasSize; //cm
    Double_t fiberGlassPanel_ZSize = 0.1; //cm

    Double_t honeyCombPanel_XSize = XGasSize; //cm
    Double_t honeyCombPanel_YSize = YGasSize; //cm
    Double_t honeyCombPanel_ZSize = 1.05; //cm

    TGeoShape *fiberGlassPanelS = new TGeoBBox(TString("fiberGlassPanelS")+=TString("_") + frameA->GetName(), fiberGlassPanel_XSize*0.5, fiberGlassPanel_YSize*0.5, fiberGlassPanel_ZSize*0.5);
    TGeoShape *honeyCombPanelS = new TGeoBBox(TString("honeyCombPanelS")+=TString("_") + frameA->GetName(), honeyCombPanel_XSize*0.5, honeyCombPanel_YSize*0.5, honeyCombPanel_ZSize*0.5);

    TGeoVolume *fiberGlassPanelV = new TGeoVolume(TString("fiberGlassPanelV")+=TString("_") + frameA->GetName(), fiberGlassPanelS);
    TGeoVolume *honeyCombPanelV = new TGeoVolume(TString("honeyCombPanelV")+=TString("_") + frameA->GetName(), honeyCombPanelS);

    //volume medium
    TGeoMedium *fiberGlassPanelV_medium = pMedFiberGlass;
    if(fiberGlassPanelV_medium) {
        fiberGlassPanelV->SetMedium(fiberGlassPanelV_medium);
    }
    else Fatal("Main", "Invalid medium for fiberGlassPanelV!");

    TGeoMedium *honeyCombPanelV_medium = pMedRohacell;
    if(honeyCombPanelV_medium) {
        honeyCombPanelV->SetMedium(honeyCombPanelV_medium);
    }
    else Fatal("Main", "Invalid medium for honeyCombPanelV!");

    //volume visual property (transparency)
    fiberGlassPanelV->SetLineColor(TColor::GetColor("#ff9933"));
    fiberGlassPanelV->SetTransparency(0);
    honeyCombPanelV->SetLineColor(TColor::GetColor("#ffff00"));
    honeyCombPanelV->SetTransparency(0);

    TGeoCombiTrans *fiberGlassPanelV_transf[4];

    fiberGlassPanelV_transf[0] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[0]->SetDx(0.0);
    fiberGlassPanelV_transf[0]->SetDy(0.0);
    fiberGlassPanelV_transf[0]->SetDz(-(ZGasSize*0.5 + fiberGlassPanel_ZSize*0.5));

    fiberGlassPanelV_transf[1] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[1]->SetDx(0.0);
    fiberGlassPanelV_transf[1]->SetDy(0.0);
    fiberGlassPanelV_transf[1]->SetDz(+(ZGasSize*0.5 + fiberGlassPanel_ZSize*0.5));

    fiberGlassPanelV_transf[2] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[2]->SetDx(0.0);
    fiberGlassPanelV_transf[2]->SetDy(0.0);
    fiberGlassPanelV_transf[2]->SetDz(-(ZGasSize*0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize + fiberGlassPanel_ZSize*0.5));

    fiberGlassPanelV_transf[3] = new TGeoCombiTrans();
    fiberGlassPanelV_transf[3]->SetDx(0.0);
    fiberGlassPanelV_transf[3]->SetDy(0.0);
    fiberGlassPanelV_transf[3]->SetDz(+(ZGasSize*0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize + fiberGlassPanel_ZSize*0.5));

    TGeoCombiTrans *honeyCombPanelV_transf[4];

    honeyCombPanelV_transf[0] = new TGeoCombiTrans();
    honeyCombPanelV_transf[0]->SetDx(0.0);
    honeyCombPanelV_transf[0]->SetDy(0.0);
    honeyCombPanelV_transf[0]->SetDz(-(ZGasSize*0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize*0.5));

    honeyCombPanelV_transf[1] = new TGeoCombiTrans();
    honeyCombPanelV_transf[1]->SetDx(0.0);
    honeyCombPanelV_transf[1]->SetDy(0.0);
    honeyCombPanelV_transf[1]->SetDz(+(ZGasSize*0.5 + fiberGlassPanel_ZSize + honeyCombPanel_ZSize*0.5));

    frameA->AddNode(fiberGlassPanelV, 0, fiberGlassPanelV_transf[0]);
    frameA->AddNode(fiberGlassPanelV, 1, fiberGlassPanelV_transf[1]);
    frameA->AddNode(fiberGlassPanelV, 2, fiberGlassPanelV_transf[2]);
    frameA->AddNode(fiberGlassPanelV, 3, fiberGlassPanelV_transf[3]);

    frameA->AddNode(honeyCombPanelV, 0, honeyCombPanelV_transf[0]);
    frameA->AddNode(honeyCombPanelV, 1, honeyCombPanelV_transf[1]);
    //--------------------------------------------------------------------------

    return frameA;
}