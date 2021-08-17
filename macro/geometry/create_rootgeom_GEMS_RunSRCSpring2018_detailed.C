/*
 * Use this macro to create detailed
 * ROOT geometry for the RunSRCSpring2018 configuration
 * (BM@N RUN-7:SRC) with passive volumes (frames, al. cable supports, ...)
 *
 * Created: 29.09.2020
 * 02.03.2021 (Updated): radius of hole in active zone (4.0 -> 6.0 cm)
 * 22.03.2021 (Updated): material layers and frames for inclined small GEMs added
 * Author: Baranov D.
 */

#include "TGeoVolume.h"


//Set Parameters of GEMS -------------------------------------------------------
const Int_t NStations = 10;      //stations in the detector
const Int_t NMaxModules = 2;    //max. number of modules in a station

//(X-Y-Z)Positions of stations (sensitive volumes)
//const Double_t XStationPositions[NStations] = { 0.0 /*0*/, 0.0 /*1*/, 0.0 /*2*/, 0.0 /*3*/, 0.0 /*4*/, 0.0 /*5*/, 0.0 /*6*/, 0.0 /*7*/, 0.0 /*8*/, 0.0 /*9*/ };
//const Double_t YStationPositions[NStations] = { 0.0 /*0*/, 0.0 /*1*/, 0.0 /*2*/, 0.0 /*3*/, 0.0 /*4*/, 0.0 /*5*/, 0.0 /*6*/, 0.0 /*7*/, 0.0 /*8*/, 0.0 /*9*/ };
//const Double_t ZStationPositions[NStations] = { 0.0 /*0*/, 0.0 /*1*/, 0.0 /*2*/, 0.0 /*3*/, 0.0 /*4*/, 0.0 /*5*/, 0.0 /*6*/, 0.0 /*7*/, 0.0 /*8*/, 0.0 /*9*/ };

//(X-Y-Z)Positions of stations (sensitive volumes) -----------------------------
const Double_t XStationPositions[NStations] = {
    +117.22007, //station 0 (left side: upper GEM M4)
    +117.27807, //station 1 (left side: lower GEM M3)
    -117.51723, //station 2 (right side: upper GEM M1)
    -115.31016, //station 3 (right side: lower GEM M2)
    +1.0055, //station 4 (GEM M10)
    +1.4715, //station 5 (GEM M7)
    +2.1145, //station 6 (GEM M5)
    +2.9225, //station 7 (GEM M6)
    +3.6775, //station 8 (GEM M8)
    +4.5025  //station 9 (GEM M9)
};
const Double_t YStationPositions[NStations] = {
    +16.86775, //station 0 (left side: upper GEM M4)
    -25.90225, //station 1 (left side: lower GEM M3)
    +16.86775, //station 2 (right side: upper GEM M1)
    -25.76025, //station 3 (right side: lower GEM M2)
    +7.97675, //station 4 (GEM M10)
    +7.96375, //station 5 (GEM M7)
    +7.64975, //station 6 (GEM M5)
    +7.74575, //station 7 (GEM M6)
    +8.02675, //station 8 (GEM M8)
    +7.95175  //station 9 (GEM M9)
};
const Double_t ZStationPositions[NStations] = {
    -444.86679, //station 0 (left side: upper GEM M4)
    -447.56632, //station 1 (left side: lower GEM M3)
    -444.91641, //station 2 (right side: upper GEM M1)
    -446.26916, //station 3 (right side: lower GEM M2)
    +64.754, //station 4 (GEM M10)
    +87.854, //station 5 (GEM M7)
    +112.967, //station 6 (GEM M5)
    +135.988, //station 7 (GEM M6)
    +160.845, //station 8 (GEM M8)
    +183.712  //station 9 (GEM M9)
};
//------------------------------------------------------------------------------

//(X-Y-Z)Shifts of modules in each station -------------------------------------
const Double_t XModuleShifts[NStations][NMaxModules] = {
    { 0.0 }, //station 0 (left side: upper GEM M4)
    { 0.0 }, //station 1 (left side: lower GEM M3)
    { 0.0 }, //station 2 (right side: upper GEM M1)
    { 0.0 }, //station 3 (right side: lower GEM M2)
    { 163.2*0.25, -163.2*0.25 }, //station 4 (GEM M10)
    { 163.2*0.25, -163.2*0.25 }, //station 5 (GEM M7)
    { 163.2*0.25, -163.2*0.25 }, //station 6 (GEM M5)
    { 163.2*0.25, -163.2*0.25 }, //station 7 (GEM M6)
    { 163.2*0.25, -163.2*0.25 }, //station 8 (GEM M8)
    { 163.2*0.25, -163.2*0.25 }  //station 9 (GEM M9)
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    { 0.0 }, //station 0 (left side: upper GEM M4)
    { 0.0 }, //station 1 (left side: lower GEM M3)
    { 0.0 }, //station 2 (right side: upper GEM M1)
    { 0.0 }, //station 3 (right side: lower GEM M2)
    { 0.0, 0.0 }, //station 4 (GEM M10)
    { 0.0, 0.0 }, //station 5 (GEM M7)
    { 0.0, 0.0 }, //station 6 (GEM M5)
    { 0.0, 0.0 }, //station 7 (GEM M6)
    { 0.0, 0.0 }, //station 8 (GEM M8)
    { 0.0, 0.0 }  //station 9 (GEM M9)
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    { 0.0 }, //station 0 (left side: upper GEM M4)
    { 0.0 }, //station 1 (left side: lower GEM M3)
    { 0.0 }, //station 2 (right side: upper GEM M1)
    { 0.0 }, //station 3 (right side: lower GEM M2)
    { 0.0, 0.0 }, //station 4 (GEM M10)
    { 0.0, 0.0 }, //station 5 (GEM M7)
    { 0.0, 0.0 }, //station 6 (GEM M5)
    { 0.0, 0.0 }, //station 7 (GEM M6)
    { 0.0, 0.0 }, //station 8 (GEM M8)
    { 0.0, 0.0 }  //station 9 (GEM M9)
};

//rotations of stations around y-axis by 180 deg.
const Bool_t YStationRotations[NStations] = {true/*0*/, true/*1*/, true/*2*/, true/*3*/, true/*4*/, false/*5*/, true/*6*/, false/*7*/, true/*8*/, false/*9*/};

//rotations of stations around x-axis by 180 deg.
const Bool_t XStationRotations[NStations] = {false/*0*/, true/*1*/, false/*2*/, true/*3*/, false/*4*/, false/*5*/, false/*6*/, false/*7*/, false/*8*/, false/*9*/};
//------------------------------------------------------------------------------

//GEM plane sizes (66x41 type) -------------------------------------------------
const Double_t XModuleSize_Station66x41 = 66.0;
const Double_t YModuleSize_Station66x41 = 41.0;
const Double_t ZModuleSize_Station66x41 = 0.9;
const Double_t ZGEMSize_Station66x41 = 2.3; //common thickness of GEM (including sens. vol)

const Double_t dXFrame_Station66x41 = 2.0;
const Double_t dYFrame_Station66x41 = 2.0;
const Double_t dZFrame_Station66x41 = 2.1; //was 2.3 cm - thickness
//------------------------------------------------------------------------------

//GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2*0.5;
const Double_t YModuleSize_Station163x45 = 45.0;
const Double_t ZModuleSize_Station163x45 = 0.9; //sensitive volume
const Double_t ZGEMSize_Station163x45 = 4.1; //common thickness of GEM (including sens. vol)

const Double_t dXFrame_Station163x45 = 2.0;
const Double_t dYFrame_Station163x45 = 2.0;
const Double_t dZFrame_Station163x45 = 3.9;
//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedArCO27030 = 0;
TGeoMedium *pMedAluminium = 0;
TGeoMedium *pMedEpoxideCompound = 0;
TGeoMedium *pMedGlue = 0;
TGeoMedium *pMedBrass = 0;
TGeoMedium *pMedSteel = 0;
TGeoMedium *pMedHoneyComb = 0;
TGeoMedium *pMedPolyamide = 0;

class FairGeoMedia;
class FairGeoBuilder;


TGeoVolume *CreateStation(TString station_name);

TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);
TGeoVolume *CreateLayersForModule_Station66x41(TString layer_name);
TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz);
TGeoVolume *CreateBasicFrameForModule_Station66x41(TString frame_name);
TGeoVolume *CreateDetailedFrameForModule_Station66x41(TString frame_name);

TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
TGeoVolume *CreateLayersForModule_Station163x45(TString layer_name);
TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius);
TGeoVolume *CreateFrameForHalfPlane_Station163x45(TString frame_name);

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

    //copper medium
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if ( ! mCopper  ) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    pMedCopper = gGeoManager->GetMedium("copper");
    if ( ! pMedCopper  ) Fatal("Main", "Medium copper not found");

    //arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030  ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030= gGeoManager->GetMedium("arco27030");
    if ( ! pMedArCO27030  ) Fatal("Main", "Medium arco27030 not found");

    //aluminium medium
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if ( ! mAluminium  ) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    pMedAluminium= gGeoManager->GetMedium("aluminium");
    if ( ! pMedAluminium  ) Fatal("Main", "Medium aluminium not found");

    //epoxide compound (for GEM frames)
    FairGeoMedium* mEpoxideCompound = geoMedia->getMedium("polyurethane_hd");
    if ( ! mEpoxideCompound  ) Fatal("Main", "FairMedium epoxideCompound not found");
    geoBuild->createMedium(mEpoxideCompound);
    pMedEpoxideCompound= gGeoManager->GetMedium("polyurethane_hd");
    if ( ! pMedEpoxideCompound  ) Fatal("Main", "Medium epoxideCompound not found");

    //glue
    FairGeoMedium* mGlue = geoMedia->getMedium("acrylic_glue");
    if ( ! mGlue  ) Fatal("Main", "FairMedium acrylic_glue not found");
    geoBuild->createMedium(mGlue);
    pMedGlue= gGeoManager->GetMedium("acrylic_glue");
    if ( ! pMedGlue  ) Fatal("Main", "Medium acrylic_glue not found");

    //brass medium
    FairGeoMedium* mBrass = geoMedia->getMedium("brass");
    if ( ! mBrass  ) Fatal("Main", "FairMedium brass not found");
    geoBuild->createMedium(mBrass);
    pMedBrass= gGeoManager->GetMedium("brass");
    if ( ! pMedBrass  ) Fatal("Main", "Medium brass not found");

    //steel medium
    FairGeoMedium* mSteel = geoMedia->getMedium("steel");
    if ( ! mSteel  ) Fatal("Main", "FairMedium steel not found");
    geoBuild->createMedium(mSteel);
    pMedSteel= gGeoManager->GetMedium("steel");
    if ( ! pMedSteel  ) Fatal("Main", "Medium steel not found");

    //honeycomb medium
    FairGeoMedium* mHoneyComb = geoMedia->getMedium("AramidHoneyComb");
    if ( ! mHoneyComb  ) Fatal("Main", "FairMedium AramidHoneyComb not found");
    geoBuild->createMedium(mHoneyComb);
    pMedHoneyComb = gGeoManager->GetMedium("AramidHoneyComb");
    if ( ! pMedHoneyComb  ) Fatal("Main", "Medium AramidHoneyComb not found");

    //polyamide medium
    FairGeoMedium* mPolyamide = geoMedia->getMedium("polyamide");
    if ( ! mPolyamide  ) Fatal("Main", "FairMedium polyamide not found");
    geoBuild->createMedium(mPolyamide);
    pMedPolyamide = gGeoManager->GetMedium("polyamide");
    if ( ! pMedPolyamide  ) Fatal("Main", "Medium polyamide not found");
}

void create_rootgeom_GEMS_RunSRCSpring2018_detailed() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "RunSRCSpring2018_detailed";
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

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* GEMS = new TGeoVolumeAssembly(geoDetectorName);
    GEMS->SetMedium(pMedAir);

    //TGeoVolume *target = gGeoMan->MakeSphere("target", 0, 0, 5.0);
    //GEMS->AddNode(target, 0, new TGeoTranslation(0.0, 0.0, -647.476));

    //station 0 (small 66x41 GEM: left side: upper station)
    if(1) {
        Int_t stationNum = 0; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *layers0 = CreateLayersForModule_Station66x41(TString("layer0_")+station->GetName());
        //TGeoVolume *frame0 = CreateBasicFrameForModule_Station66x41(TString("frame0_")+station->GetName());
        TGeoVolume *frame0 = CreateDetailedFrameForModule_Station66x41(TString("frame0_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *layers0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) layers0_transform->ReflectZ(true);
            if(XStationRotations[stationNum] == true) layers0_transform->RotateX(180.0);
            layers0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            layers0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *frame0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) frame0_transform->RotateY(180.0);
            if(XStationRotations[stationNum] == true) frame0_transform->RotateX(180.0);
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            frame0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        //station->AddNode(layers0, 0, new TGeoCombiTrans(*layers0_transform)); //layers
        station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 1 (small 66x41 GEM: left side: lower station)
    if(1) {
        Int_t stationNum = 1; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *layers0 = CreateLayersForModule_Station66x41(TString("layer0_")+station->GetName());
        //TGeoVolume *frame0 = CreateBasicFrameForModule_Station66x41(TString("frame0_")+station->GetName());
        TGeoVolume *frame0 = CreateDetailedFrameForModule_Station66x41(TString("frame0_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *layers0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) layers0_transform->ReflectZ(true);
            if(XStationRotations[stationNum] == true) layers0_transform->RotateX(180.0);
            layers0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            layers0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *frame0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) frame0_transform->RotateY(180.0);
            if(XStationRotations[stationNum] == true) frame0_transform->RotateX(180.0);
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            frame0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        //station->AddNode(layers0, 0, new TGeoCombiTrans(*layers0_transform)); //layers
        station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 2 (small 66x41 GEM: right side: upper station)
    if(1) {
        Int_t stationNum = 2; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *layers0 = CreateLayersForModule_Station66x41(TString("layer0_")+station->GetName());
        //TGeoVolume *frame0 = CreateBasicFrameForModule_Station66x41(TString("frame0_")+station->GetName());
        TGeoVolume *frame0 = CreateDetailedFrameForModule_Station66x41(TString("frame0_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *layers0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) layers0_transform->ReflectZ(true);
            if(XStationRotations[stationNum] == true) layers0_transform->RotateX(180.0);
            layers0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            layers0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *frame0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) frame0_transform->RotateY(180.0);
            if(XStationRotations[stationNum] == true) frame0_transform->RotateX(180.0);
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            frame0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        //station->AddNode(layers0, 0, new TGeoCombiTrans(*layers0_transform)); //layers
        station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 3 (small 66x41 GEM: right side: lower station)
    if(1) {
        Int_t stationNum = 3; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *layers0 = CreateLayersForModule_Station66x41(TString("layer0_")+station->GetName());
        //TGeoVolume *frame0 = CreateBasicFrameForModule_Station66x41(TString("frame0_")+station->GetName());
        TGeoVolume *frame0 = CreateDetailedFrameForModule_Station66x41(TString("frame0_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *layers0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) layers0_transform->ReflectZ(true);
            if(XStationRotations[stationNum] == true) layers0_transform->RotateX(180.0);
            layers0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            layers0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *frame0_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) frame0_transform->RotateY(180.0);
            if(XStationRotations[stationNum] == true) frame0_transform->RotateX(180.0);
            frame0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5) ;
            frame0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        //station->AddNode(layers0, 0, new TGeoCombiTrans(*layers0_transform)); //layers
        station->AddNode(frame0, 0, new TGeoCombiTrans(*frame0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //stations 4-9 (each is a big 163x45 GEM consisting of two modules)
    if(1) {
        for(Int_t istation = 4; istation < 10; ++istation) {

            Int_t stationNum = istation;
            TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));

            TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
            TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);

            TGeoVolume *layers0 = CreateLayersForModule_Station163x45(TString("layer0_")+station->GetName());
            TGeoVolume *layers1 = CreateLayersForModule_Station163x45(TString("layer1_")+station->GetName());

            TGeoVolume *frame = CreateFrameForHalfPlane_Station163x45(TString("frame_")+station->GetName());

            if(istation == 6) {
                station->SetName(TString("disabled_station") + TString::Itoa(stationNum, 10));
                module0->SetName(TString("module0_")+station->GetName());
                module1->SetName(TString("module1_")+station->GetName());
            }

            TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
                if(XStationRotations[stationNum] == true) module0_transform->RotateX(180.0);
                module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            if(XStationRotations[stationNum] == true) module1_transform->RotateX(180.0);
                module1_transform->ReflectX(true);
                module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

           TGeoCombiTrans *layers0_transform = new TGeoCombiTrans();
                if(YStationRotations[stationNum] == true) layers0_transform->ReflectZ(true);
                if(XStationRotations[stationNum] == true) layers0_transform->RotateX(180.0);
                layers0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *layers1_transform = new TGeoCombiTrans();
                if(YStationRotations[stationNum] == true) layers1_transform->ReflectZ(true);
                if(XStationRotations[stationNum] == true) layers1_transform->RotateX(180.0);
                layers1_transform->ReflectX(true);
                layers1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *frame_transform = new TGeoCombiTrans();
            if(YStationRotations[stationNum] == true) frame_transform->RotateY(180.0);
            if(XStationRotations[stationNum] == true) frame_transform->RotateX(180.0);
            frame_transform->SetTranslation((XModuleShifts[stationNum][0] + XModuleShifts[stationNum][1]), YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *station_transform = new TGeoCombiTrans();
            station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
            station->AddNode(module1, 1, new TGeoCombiTrans(*module1_transform)); //module

            station->AddNode(layers0, 0, new TGeoCombiTrans(*layers0_transform)); //layers
            station->AddNode(layers1, 1, new TGeoCombiTrans(*layers1_transform)); //layers

            station->AddNode(frame, 0, frame_transform);

            GEMS->AddNode(station, 0, station_transform);
        }
    }

    top->AddNode(GEMS, 0);
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
//------------------------------------------------------------------------------

TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize) {

    //module shape
    TGeoShape *moduleS = new TGeoBBox("moduleS", xsize*0.5, ysize*0.5, zsize*0.5);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if(pMedArCO27030) moduleV->SetMedium(pMedArCO27030);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}

TGeoVolume *CreateLayersForModule_Station66x41(TString layer_name) {

    //frame volumes
    TGeoVolume *layers = new TGeoVolumeAssembly(layer_name);
    layers->SetMedium(pMedAir);

    //Common parameters for all layers
    Double_t layerFront_XSize = 66.0; //cm
    Double_t layerFront_YSize = 41.0; //cm
    Double_t layerFront_XShift = 0.0; //cm
    Double_t layerFront_YShift = 0.0; //cm

    Double_t layerBack_XSize = 66.0; //cm
    Double_t layerBack_YSize = 41.0; //cm
    Double_t layerBack_XShift = 0.0; //cm
    Double_t layerBack_YShift = 0.0; //cm
    Double_t layerBack_ZShift = 0.0; //cm

    if (layer_name == "layer0_station3") layerBack_ZShift = +0.2; //z-shift for an extended module

    //copper layer -------------------------------------------------------------
    Double_t copperLayer_ZSize = 0.00655; //cm;
    Double_t copperLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *copperLayerFrontS = new TGeoBBox(TString("copperLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, copperLayer_ZSize*0.5);
    TGeoShape *copperLayerBackS = new TGeoBBox(TString("copperLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, copperLayer_ZSize*0.5);

    TGeoVolume *copperLayerFrontV = new TGeoVolume(TString("copperLayerFrontV")+=TString("_") + layers->GetName(), copperLayerFrontS);
    TGeoVolume *copperLayerBackV = new TGeoVolume(TString("copperLayerBackV")+=TString("_") + layers->GetName(), copperLayerBackS);

    //volume medium
    TGeoMedium *copperLayerV_medium = pMedCopper;
    if(copperLayerV_medium) {
        copperLayerFrontV->SetMedium(copperLayerV_medium);
        copperLayerBackV->SetMedium(copperLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for copperLayerV!");

    //volume visual property (transparency)
    copperLayerFrontV->SetLineColor(TColor::GetColor("#ff7538"));
    copperLayerFrontV->SetTransparency(0);
    copperLayerBackV->SetLineColor(TColor::GetColor("#ff7538"));
    copperLayerBackV->SetTransparency(0);

    TGeoCombiTrans *copperLayerFront_transf[2];

    copperLayerFront_transf[0] = new TGeoCombiTrans();
    copperLayerFront_transf[0]->SetDx(+layerFront_XShift);
    copperLayerFront_transf[0]->SetDy(+layerFront_YShift);
    copperLayerFront_transf[0]->SetDz(-ZGEMSize_Station66x41*0.5 + copperLayer_ZSize*0.5);

    TGeoCombiTrans *copperLayerBack_transf[1];

    copperLayerBack_transf[0] = new TGeoCombiTrans();
    copperLayerBack_transf[0]->SetDx(+layerBack_XShift);
    copperLayerBack_transf[0]->SetDy(+layerBack_YShift);
    copperLayerBack_transf[0]->SetDz(+ZGEMSize_Station66x41*0.5 - copperLayer_ZSize*0.5 + layerBack_ZShift) ;

    layers->AddNode(copperLayerFrontV, 0, copperLayerFront_transf[0]);
    layers->AddNode(copperLayerBackV, 0, copperLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //glue layer ---------------------------------------------------------------
    Double_t glueLayer_ZSize = 0.01; //cm; half-thickness of all glue layers
    Double_t glueLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *glueLayerFrontS = new TGeoBBox(TString("glueLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, glueLayer_ZSize*0.5);
    TGeoShape *glueLayerBackS = new TGeoBBox(TString("glueLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, glueLayer_ZSize*0.5);

    TGeoVolume *glueLayerFrontV = new TGeoVolume(TString("glueLayerFrontV")+=TString("_") + layers->GetName(), glueLayerFrontS);
    TGeoVolume *glueLayerBackV = new TGeoVolume(TString("glueLayerBackV")+=TString("_") + layers->GetName(), glueLayerBackS);

    //volume medium
    TGeoMedium *glueLayerV_medium = pMedGlue;
    if(glueLayerV_medium) {
        glueLayerFrontV->SetMedium(glueLayerV_medium);
        glueLayerBackV->SetMedium(glueLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for glueLayerV!");

    //volume visual property (transparency)
    glueLayerFrontV->SetLineColor(TColor::GetColor("#1919ff"));
    glueLayerFrontV->SetTransparency(0);
    glueLayerBackV->SetLineColor(TColor::GetColor("#1919ff"));
    glueLayerBackV->SetTransparency(0);

    TGeoCombiTrans *glueLayerFront_transf[1];

    glueLayerFront_transf[0] = new TGeoCombiTrans();
    glueLayerFront_transf[0]->SetDx(+layerFront_XShift);
    glueLayerFront_transf[0]->SetDy(+layerFront_YShift);
    glueLayerFront_transf[0]->SetDz(-ZGEMSize_Station66x41*0.5 + copperLayer_ZSize + glueLayer_ZSize*0.5);

    TGeoCombiTrans *glueLayerBack_transf[1];

    glueLayerBack_transf[0] = new TGeoCombiTrans();
    glueLayerBack_transf[0]->SetDx(+layerBack_XShift);
    glueLayerBack_transf[0]->SetDy(+layerBack_YShift);
    glueLayerBack_transf[0]->SetDz(+ZGEMSize_Station66x41*0.5 - copperLayer_ZSize - glueLayer_ZSize*0.5 + layerBack_ZShift) ;

    layers->AddNode(glueLayerFrontV, 0, glueLayerFront_transf[0]);
    layers->AddNode(glueLayerBackV, 0, glueLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //epoxide layer ------------------------------------------------------------
    Double_t epoxideLayer_ZSize = 0.105; //cm; half-thickness of all epoxide layers
    Double_t epoxideLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *epoxideLayerFrontS = new TGeoBBox(TString("epoxideLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, epoxideLayer_ZSize*0.5);
    TGeoShape *epoxideLayerBackS = new TGeoBBox(TString("epoxideLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, epoxideLayer_ZSize*0.5);

    TGeoVolume *epoxideLayerFrontV = new TGeoVolume(TString("epoxideLayerFrontV")+=TString("_") + layers->GetName(), epoxideLayerFrontS);
    TGeoVolume *epoxideLayerBackV = new TGeoVolume(TString("epoxideLayerBackV")+=TString("_") + layers->GetName(), epoxideLayerBackS);

    //volume medium
    TGeoMedium *epoxideLayerV_medium = pMedEpoxideCompound;
    if(epoxideLayerV_medium) {
        epoxideLayerFrontV->SetMedium(epoxideLayerV_medium);
        epoxideLayerBackV->SetMedium(epoxideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for epoxideLayerV!");

    //volume visual property (transparency)
    epoxideLayerFrontV->SetLineColor(TColor::GetColor("#aa33ff"));
    epoxideLayerFrontV->SetTransparency(0);
    epoxideLayerBackV->SetLineColor(TColor::GetColor("#aa33ff"));
    epoxideLayerBackV->SetTransparency(0);

    TGeoCombiTrans *epoxideLayerFront_transf[1];

    epoxideLayerFront_transf[0] = new TGeoCombiTrans();
    epoxideLayerFront_transf[0]->SetDx(+layerFront_XShift);
    epoxideLayerFront_transf[0]->SetDy(+layerFront_YShift);
    epoxideLayerFront_transf[0]->SetDz(-ZGEMSize_Station66x41*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize*0.5);

    TGeoCombiTrans *epoxideLayerBack_transf[1];

    epoxideLayerBack_transf[0] = new TGeoCombiTrans();
    epoxideLayerBack_transf[0]->SetDx(+layerBack_XShift);
    epoxideLayerBack_transf[0]->SetDy(+layerBack_YShift);
    epoxideLayerBack_transf[0]->SetDz(+ZGEMSize_Station66x41*0.5 - copperLayer_ZSize - glueLayer_ZSize - epoxideLayer_ZSize*0.5 + layerBack_ZShift) ;

    layers->AddNode(epoxideLayerFrontV, 0, epoxideLayerFront_transf[0]);
    layers->AddNode(epoxideLayerBackV, 0, epoxideLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //honeycomb layer ----------------------------------------------------------
    //Double_t honeycombLayer_ZSize = 1.5-(0.05+0.00055+0.005+0.0125); //cm; half-thickness of all honeycomb layers
    //Double_t honeycombLayer_ZSize = 1.5-(0.06805); //cm; half-thickness of all honeycomb layers
    Double_t honeycombLayer_ZSize = 0.56; //cm; half-thickness of all honeycomb layers
    Double_t honeycombLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *honeycombLayerFrontS = new TGeoBBox(TString("honeycombLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, honeycombLayer_ZSize*0.5);
    TGeoShape *honeycombLayerBackS = new TGeoBBox(TString("honeycombLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, honeycombLayer_ZSize*0.5);

    TGeoVolume *honeycombLayerFrontV = new TGeoVolume(TString("honeycombLayerFrontV")+=TString("_") + layers->GetName(), honeycombLayerFrontS);
    TGeoVolume *honeycombLayerBackV = new TGeoVolume(TString("honeycombLayerBackV")+=TString("_") + layers->GetName(), honeycombLayerBackS);

    //volume medium
    TGeoMedium *honeycombLayerV_medium = pMedHoneyComb;
    if(honeycombLayerV_medium) {
        honeycombLayerFrontV->SetMedium(honeycombLayerV_medium);
        honeycombLayerBackV->SetMedium(honeycombLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for honeycombLayerV!");

    //volume visual property (transparency)
    honeycombLayerFrontV->SetLineColor(TColor::GetColor("#ffdf84"));
    honeycombLayerFrontV->SetTransparency(0);
    honeycombLayerBackV->SetLineColor(TColor::GetColor("#ffdf84"));
    honeycombLayerBackV->SetTransparency(0);

    TGeoCombiTrans *honeycombLayerFront_transf[1];

    honeycombLayerFront_transf[0] = new TGeoCombiTrans();
    honeycombLayerFront_transf[0]->SetDx(+layerFront_XShift);
    honeycombLayerFront_transf[0]->SetDy(+layerFront_YShift);
    honeycombLayerFront_transf[0]->SetDz(-ZGEMSize_Station66x41*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize + honeycombLayer_ZSize*0.5);

    TGeoCombiTrans *honeycombLayerBack_transf[1];

    honeycombLayerBack_transf[0] = new TGeoCombiTrans();
    honeycombLayerBack_transf[0]->SetDx(+layerBack_XShift);
    honeycombLayerBack_transf[0]->SetDy(+layerBack_YShift);
    honeycombLayerBack_transf[0]->SetDz(+ZGEMSize_Station66x41*0.5 - copperLayer_ZSize - glueLayer_ZSize - epoxideLayer_ZSize - honeycombLayer_ZSize*0.5 + layerBack_ZShift) ;

    layers->AddNode(honeycombLayerFrontV, 0, honeycombLayerFront_transf[0]);
    layers->AddNode(honeycombLayerBackV, 0, honeycombLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //polyamide layer ----------------------------------------------------------
    Double_t polyamideLayer_ZSize = 0.0125; //cm; half-thickness of all polyamide layers
    Double_t polyamideLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *polyamideLayerFrontS = new TGeoBBox(TString("polyamideLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, polyamideLayer_ZSize*0.5);
    TGeoShape *polyamideLayerBackS = new TGeoBBox(TString("polyamideLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, polyamideLayer_ZSize*0.5);

    TGeoVolume *polyamideLayerFrontV = new TGeoVolume(TString("polyamideLayerFrontV")+=TString("_") + layers->GetName(), polyamideLayerFrontS);
    TGeoVolume *polyamideLayerBackV = new TGeoVolume(TString("polyamideLayerBackV")+=TString("_") + layers->GetName(), polyamideLayerBackS);

    //volume medium
    TGeoMedium *polyamideLayerV_medium = pMedPolyamide;
    if(polyamideLayerV_medium) {
        polyamideLayerFrontV->SetMedium(polyamideLayerV_medium);
        polyamideLayerBackV->SetMedium(polyamideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for polyamideLayerV!");

    //volume visual property (transparency)
    polyamideLayerFrontV->SetLineColor(TColor::GetColor("#fefe22"));
    polyamideLayerFrontV->SetTransparency(0);
    polyamideLayerBackV->SetLineColor(TColor::GetColor("#fefe22"));
    polyamideLayerBackV->SetTransparency(0);

    TGeoCombiTrans *polyamideLayerFront_transf[1];

    polyamideLayerFront_transf[0] = new TGeoCombiTrans();
    polyamideLayerFront_transf[0]->SetDx(+layerFront_XShift);
    polyamideLayerFront_transf[0]->SetDy(+layerFront_YShift);
    polyamideLayerFront_transf[0]->SetDz(-ZGEMSize_Station66x41*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize + honeycombLayer_ZSize + polyamideLayer_ZSize*0.5);

    TGeoCombiTrans *polyamideLayerBack_transf[1];

    polyamideLayerBack_transf[0] = new TGeoCombiTrans();
    polyamideLayerBack_transf[0]->SetDx(+layerBack_XShift);
    polyamideLayerBack_transf[0]->SetDy(+layerBack_YShift);
    polyamideLayerBack_transf[0]->SetDz(+ZGEMSize_Station66x41*0.5 - copperLayer_ZSize - glueLayer_ZSize - epoxideLayer_ZSize - honeycombLayer_ZSize - polyamideLayer_ZSize*0.5 + layerBack_ZShift) ;

    layers->AddNode(polyamideLayerFrontV, 0, polyamideLayerFront_transf[0]);
    layers->AddNode(polyamideLayerBackV, 0, polyamideLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    return layers;
}

TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dx*0.5, YModuleSize_Station66x41*0.5+dy, dz*0.5);
    TGeoShape *horizontal_frameS = new TGeoBBox("horizontal_frameS", XModuleSize_Station66x41*0.5, dy*0.5, dz*0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name+"_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_frameV = new TGeoVolume(frame_name+"_horizontal_frameV", horizontal_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");
    if(pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station66x41*0.5+dXFrame_Station66x41*0.5, 0.0, 0.0);

    TGeoCombiTrans *right_vertical_frame_position = new TGeoCombiTrans();
    right_vertical_frame_position->SetTranslation(-XModuleSize_Station66x41*0.5-dXFrame_Station66x41*0.5, 0.0, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station66x41*0.5+dYFrame_Station66x41*0.5, 0.0);

    TGeoCombiTrans *lower_horizontal_frame_position = new TGeoCombiTrans();
    lower_horizontal_frame_position->SetTranslation(0.0, -YModuleSize_Station66x41*0.5-dYFrame_Station66x41*0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(vertical_frameV, 1, right_vertical_frame_position);
    composite_frameV->AddNode(horizontal_frameV, 0, bottom_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_frameV, 1, lower_horizontal_frame_position);

    return composite_frameV;
}

TGeoVolume *CreateBasicFrameForModule_Station66x41(TString frame_name) {

    //frame volumes
    TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);
    frames->SetMedium(pMedAir);

    //back epoxide frame -------------------------------------------------------
    Double_t backEpoxideFramePart_XSize = 75.1; //cm
    Double_t backEpoxideFramePart_YSize = 48.5; //cm
    Double_t backEpoxideFramePart_ZSize = 0.7; //cm

    //layer shape
    TGeoShape *backEpoxideFramePartS = new TGeoBBox(TString("backEpoxideFramePartS")+=TString("_") + frames->GetName(), backEpoxideFramePart_XSize*0.5, backEpoxideFramePart_YSize*0.5, backEpoxideFramePart_ZSize*0.5);
    TGeoShape *backEpoxideActiveAreaS = new TGeoBBox(TString("backEpoxideActiveAreaS")+=TString("_") + frames->GetName(), XModuleSize_Station66x41*0.5, YModuleSize_Station66x41*0.5, ZModuleSize_Station66x41*0.5);

    TGeoTranslation *backEpoxideFramePart_pos = new TGeoTranslation();
        backEpoxideFramePart_pos->SetName("backEpoxideFramePart_pos");
        backEpoxideFramePart_pos->SetDx(-2.75/*shift*/);
        backEpoxideFramePart_pos->SetDy(+2.05/*shift*/);
        backEpoxideFramePart_pos->SetDz(0.0);
        backEpoxideFramePart_pos->RegisterYourself();

    TGeoCompositeShape *backEpoxideFrameS = new TGeoCompositeShape();
    backEpoxideFrameS->SetName(TString("backEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "backEpoxideFramePartS"; expression += TString("_") + frames->GetName();
            expression += ":backEpoxideFramePart_pos";
            expression += "-backEpoxideActiveAreaS"; expression += TString("_") + frames->GetName();

        backEpoxideFrameS->MakeNode(expression);
        backEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *backEpoxideFrameV = new TGeoVolume(TString("backEpoxideFrameV")+=TString("_") + frames->GetName(), backEpoxideFrameS);

    //volume medium
    TGeoMedium *backEpoxideFrameV_medium = pMedEpoxideCompound;
    if(backEpoxideFrameV_medium) {
        backEpoxideFrameV->SetMedium(backEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for backEpoxideFrameV!");

    //volume visual property (transparency)
    backEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    backEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *backEpoxideFrame_transf[1];

    backEpoxideFrame_transf[0] = new TGeoCombiTrans();
    backEpoxideFrame_transf[0]->SetDx(0.0);
    backEpoxideFrame_transf[0]->SetDy(0.0);
    backEpoxideFrame_transf[0]->SetDz(+(ZModuleSize_Station66x41-backEpoxideFramePart_ZSize)*0.5 + backEpoxideFramePart_ZSize);

    frames->AddNode(backEpoxideFrameV, 0, backEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //middle epoxide frame -----------------------------------------------------
    Double_t middleEpoxideFrameBigPart_XSize = 69.3; //cm
    Double_t middleEpoxideFrameBigPart_YSize = 44.7; //cm
    Double_t middleEpoxideFrameBigPart_ZSize = 0.9; //cm

    Double_t middleEpoxideFrameSmallPart_XSize = 3.4; //cm
    Double_t middleEpoxideFrameSmallPart_YSize = 16.8; //cm
    Double_t middleEpoxideFrameSmallPart_ZSize = 0.9; //cm

    Double_t middleEpoxideFrameHoleArea_XSize = 66.5; //cm
    Double_t middleEpoxideFrameHoleArea_YSize = 41.9; //cm
    Double_t middleEpoxideFrameHoleArea_ZSize = 0.9+0.1; //cm

    //layer shape
    TGeoShape *middleEpoxideFrameBigPartS = new TGeoBBox(TString("middleEpoxideFrameBigPartS")+=TString("_") + frames->GetName(), middleEpoxideFrameBigPart_XSize*0.5, middleEpoxideFrameBigPart_YSize*0.5, middleEpoxideFrameBigPart_ZSize*0.5);
    TGeoShape *middleEpoxideFrameSmallPartS = new TGeoBBox(TString("middleEpoxideFrameSmallPartS")+=TString("_") + frames->GetName(), middleEpoxideFrameSmallPart_XSize*0.5, middleEpoxideFrameSmallPart_YSize*0.5, middleEpoxideFrameSmallPart_ZSize*0.5);
    TGeoShape *middleEpoxideHoleAreaS = new TGeoBBox(TString("middleEpoxideHoleAreaS")+=TString("_") + frames->GetName(), middleEpoxideFrameHoleArea_XSize*0.5, middleEpoxideFrameHoleArea_YSize*0.5, middleEpoxideFrameHoleArea_ZSize*0.5);

    TGeoTranslation *middleEpoxideFrameBigPart_pos = new TGeoTranslation();
        middleEpoxideFrameBigPart_pos->SetName("middleEpoxideFrameBigPart_pos");
        middleEpoxideFrameBigPart_pos->SetDx(0.0/*shift*/);
        middleEpoxideFrameBigPart_pos->SetDy(+0.15/*shift*/);
        middleEpoxideFrameBigPart_pos->SetDz(0.0);
        middleEpoxideFrameBigPart_pos->RegisterYourself();

    TGeoTranslation *middleEpoxideFrameSmallPart_pos = new TGeoTranslation();
        middleEpoxideFrameSmallPart_pos->SetName("middleEpoxideFrameSmallPart_pos");
        middleEpoxideFrameSmallPart_pos->SetDx(-(middleEpoxideFrameBigPart_XSize*0.5 + middleEpoxideFrameSmallPart_XSize*0.5));
        middleEpoxideFrameSmallPart_pos->SetDy(middleEpoxideFrameBigPart_YSize*0.5 - middleEpoxideFrameSmallPart_YSize*0.5 + 0.15/*shift*/);
        middleEpoxideFrameSmallPart_pos->SetDz(0.0);
        middleEpoxideFrameSmallPart_pos->RegisterYourself();

    TGeoCompositeShape *middleEpoxideFrameS = new TGeoCompositeShape();
    middleEpoxideFrameS->SetName(TString("middleEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "middleEpoxideFrameBigPartS"; expression += TString("_") + frames->GetName();
            expression += ":middleEpoxideFrameBigPart_pos";
            expression += "-middleEpoxideHoleAreaS"; expression += TString("_") + frames->GetName();
            expression += "+middleEpoxideFrameSmallPartS"; expression += TString("_") + frames->GetName();
            expression += ":middleEpoxideFrameSmallPart_pos";

        middleEpoxideFrameS->MakeNode(expression);
        middleEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *middleEpoxideFrameV = new TGeoVolume(TString("middleEpoxideFrameV")+=TString("_") + frames->GetName(), middleEpoxideFrameS);

    //volume medium
    TGeoMedium *middleEpoxideFrameV_medium = pMedEpoxideCompound;
    if(middleEpoxideFrameV_medium) {
        middleEpoxideFrameV->SetMedium(middleEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for middleEpoxideFrameV!");

    //volume visual property (transparency)
    middleEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    middleEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *middleEpoxideFrame_transf[1];

    middleEpoxideFrame_transf[0] = new TGeoCombiTrans();
    middleEpoxideFrame_transf[0]->SetDx(0.0);
    middleEpoxideFrame_transf[0]->SetDy(0.0);
    middleEpoxideFrame_transf[0]->SetDz(0.0);

    frames->AddNode(middleEpoxideFrameV, 0, middleEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //front epoxide frame -----------------------------------------------------
    Double_t frontEpoxideFrameBigPart_XSize = 69.5; //cm
    Double_t frontEpoxideFrameBigPart_YSize = 44.7; //cm
    Double_t frontEpoxideFrameBigPart_ZSize = 0.7; //cm

    Double_t frontEpoxideFrameSmallPart_XSize = 3.2; //cm
    Double_t frontEpoxideFrameSmallPart_YSize = 16.8; //cm
    Double_t frontEpoxideFrameSmallPart_ZSize = 0.7; //cm

    Double_t frontEpoxideFrameHoleArea_XSize = 66.0; //cm
    Double_t frontEpoxideFrameHoleArea_YSize = 41.1; //cm
    Double_t frontEpoxideFrameHoleArea_ZSize = 0.9+0.1; //cm

    //layer shape
    TGeoShape *frontEpoxideFrameBigPartS = new TGeoBBox(TString("frontEpoxideFrameBigPartS")+=TString("_") + frames->GetName(), frontEpoxideFrameBigPart_XSize*0.5, frontEpoxideFrameBigPart_YSize*0.5, frontEpoxideFrameBigPart_ZSize*0.5);
    TGeoShape *frontEpoxideFrameSmallPartS = new TGeoBBox(TString("frontEpoxideFrameSmallPartS")+=TString("_") + frames->GetName(), frontEpoxideFrameSmallPart_XSize*0.5, frontEpoxideFrameSmallPart_YSize*0.5, frontEpoxideFrameSmallPart_ZSize*0.5);
    TGeoShape *frontEpoxideHoleAreaS = new TGeoBBox(TString("frontEpoxideHoleAreaS")+=TString("_") + frames->GetName(), frontEpoxideFrameHoleArea_XSize*0.5, frontEpoxideFrameHoleArea_YSize*0.5, frontEpoxideFrameHoleArea_ZSize*0.5);

    TGeoTranslation *frontEpoxideFrameBigPart_pos = new TGeoTranslation();
        frontEpoxideFrameBigPart_pos->SetName("frontEpoxideFrameBigPart_pos");
        frontEpoxideFrameBigPart_pos->SetDx(0.0/*shift*/);
        frontEpoxideFrameBigPart_pos->SetDy(+0.15/*shift*/);
        frontEpoxideFrameBigPart_pos->SetDz(0.0);
        frontEpoxideFrameBigPart_pos->RegisterYourself();

    TGeoTranslation *frontEpoxideFrameSmallPart_pos = new TGeoTranslation();
        frontEpoxideFrameSmallPart_pos->SetName("frontEpoxideFrameSmallPart_pos");
        frontEpoxideFrameSmallPart_pos->SetDx(-(frontEpoxideFrameBigPart_XSize*0.5 + frontEpoxideFrameSmallPart_XSize*0.5));
        frontEpoxideFrameSmallPart_pos->SetDy(frontEpoxideFrameBigPart_YSize*0.5 - frontEpoxideFrameSmallPart_YSize*0.5 + 0.15/*shift*/);
        frontEpoxideFrameSmallPart_pos->SetDz(0.0);
        frontEpoxideFrameSmallPart_pos->RegisterYourself();

    TGeoCompositeShape *frontEpoxideFrameS = new TGeoCompositeShape();
    frontEpoxideFrameS->SetName(TString("frontEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "frontEpoxideFrameBigPartS"; expression += TString("_") + frames->GetName();
            expression += ":frontEpoxideFrameBigPart_pos";
            expression += "-frontEpoxideHoleAreaS"; expression += TString("_") + frames->GetName();
            expression += "+frontEpoxideFrameSmallPartS"; expression += TString("_") + frames->GetName();
            expression += ":frontEpoxideFrameSmallPart_pos";

        frontEpoxideFrameS->MakeNode(expression);
        frontEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *frontEpoxideFrameV = new TGeoVolume(TString("frontEpoxideFrameV")+=TString("_") + frames->GetName(), frontEpoxideFrameS);

    //volume medium
    TGeoMedium *frontEpoxideFrameV_medium = pMedEpoxideCompound;
    if(frontEpoxideFrameV_medium) {
        frontEpoxideFrameV->SetMedium(frontEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for frontEpoxideFrameV!");

    //volume visual property (transparency)
    frontEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    frontEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *frontEpoxideFrame_transf[1];

    frontEpoxideFrame_transf[0] = new TGeoCombiTrans();
    frontEpoxideFrame_transf[0]->SetDx(0.0);
    frontEpoxideFrame_transf[0]->SetDy(0.0);
    frontEpoxideFrame_transf[0]->SetDz(-(ZModuleSize_Station66x41-frontEpoxideFrameBigPart_ZSize)*0.5 - frontEpoxideFrameBigPart_ZSize);

    frames->AddNode(frontEpoxideFrameV, 0, frontEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    return frames;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateDetailedFrameForModule_Station66x41(TString frame_name) {

    //frame volumes
    TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);
    frames->SetMedium(pMedAir);

    //center fiberglass frame  -------------------------------------------------
    Double_t centerFiberGlassFrameSidePart_XSize = 1.45; //cm
    Double_t centerFiberGlassFrameSidePart_YSize = 44.6; //cm
    Double_t centerFiberGlassFrameSidePart_ZSize = 0.89; //cm

    Double_t centerFiberGlassFrameLongPart_XSize = 66.5; //cm
    Double_t centerFiberGlassFrameLongPart_YSize = 1.45; //cm
    Double_t centerFiberGlassFrameLongPart_ZSize = 0.89; //cm

    Double_t distanceBetweenFiberGlassFrameSideParts = 66.5; //cm
    Double_t distanceBetweenFiberGlassFrameLongParts = 41.7; //cm

    TGeoShape *centerFiberGlassFrameSidePartS = new TGeoBBox(TString("centerFiberGlassFrameSidePartS")+=TString("_") + frames->GetName(), centerFiberGlassFrameSidePart_XSize*0.5, centerFiberGlassFrameSidePart_YSize*0.5, centerFiberGlassFrameSidePart_ZSize*0.5);
    TGeoShape *centerFiberGlassFrameLongPartS = new TGeoBBox(TString("centerFiberGlassFrameLongPartS")+=TString("_") + frames->GetName(), centerFiberGlassFrameLongPart_XSize*0.5, centerFiberGlassFrameLongPart_YSize*0.5, centerFiberGlassFrameLongPart_ZSize*0.5);

    TGeoTranslation *centerFiberGlassFrameSidePart_pos1 = new TGeoTranslation();
        centerFiberGlassFrameSidePart_pos1->SetName(TString("centerFiberGlassFrameSidePart_pos1")+=TString("_") + frames->GetName());
        centerFiberGlassFrameSidePart_pos1->SetDx(+(distanceBetweenFiberGlassFrameSideParts + centerFiberGlassFrameSidePart_XSize)*0.5);
        centerFiberGlassFrameSidePart_pos1->SetDy(0.0);
        centerFiberGlassFrameSidePart_pos1->SetDz(0.0);
        centerFiberGlassFrameSidePart_pos1->RegisterYourself();

    TGeoTranslation *centerFiberGlassFrameSidePart_pos2 = new TGeoTranslation();
        centerFiberGlassFrameSidePart_pos2->SetName(TString("centerFiberGlassFrameSidePart_pos2")+=TString("_") + frames->GetName());
        centerFiberGlassFrameSidePart_pos2->SetDx(-(distanceBetweenFiberGlassFrameSideParts + centerFiberGlassFrameSidePart_XSize)*0.5);
        centerFiberGlassFrameSidePart_pos2->SetDy(0.0);
        centerFiberGlassFrameSidePart_pos2->SetDz(0.0);
        centerFiberGlassFrameSidePart_pos2->RegisterYourself();

    TGeoTranslation *centerFiberGlassFrameLongPart_top = new TGeoTranslation();
        centerFiberGlassFrameLongPart_top->SetName(TString("centerFiberGlassFrameLongPart_top")+=TString("_") + frames->GetName());
        centerFiberGlassFrameLongPart_top->SetDx(0.0);
        centerFiberGlassFrameLongPart_top->SetDy(+(distanceBetweenFiberGlassFrameLongParts + centerFiberGlassFrameLongPart_YSize)*0.5);
        centerFiberGlassFrameLongPart_top->SetDz(0.0);
        centerFiberGlassFrameLongPart_top->RegisterYourself();

    TGeoTranslation *centerFiberGlassFrameLongPart_bottom = new TGeoTranslation();
        centerFiberGlassFrameLongPart_bottom->SetName(TString("centerFiberGlassFrameLongPart_bottom")+=TString("_") + frames->GetName());
        centerFiberGlassFrameLongPart_bottom->SetDx(0.0);
        centerFiberGlassFrameLongPart_bottom->SetDy(-(distanceBetweenFiberGlassFrameLongParts + centerFiberGlassFrameLongPart_YSize)*0.5);
        centerFiberGlassFrameLongPart_bottom->SetDz(0.0);
        centerFiberGlassFrameLongPart_bottom->RegisterYourself();

    TGeoCompositeShape *centerFiberGlassFrameS = new TGeoCompositeShape();
    centerFiberGlassFrameS->SetName(TString("centerFiberGlassFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "centerFiberGlassFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":centerFiberGlassFrameSidePart_pos1"; expression += TString("_") + frames->GetName();
            expression += "+centerFiberGlassFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":centerFiberGlassFrameSidePart_pos2"; expression += TString("_") + frames->GetName();
            expression += "+centerFiberGlassFrameLongPartS"; expression += TString("_") + frames->GetName();
            expression += ":centerFiberGlassFrameLongPart_top"; expression += TString("_") + frames->GetName();
            expression += "+centerFiberGlassFrameLongPartS"; expression += TString("_") + frames->GetName();
            expression += ":centerFiberGlassFrameLongPart_bottom"; expression += TString("_") + frames->GetName();

        centerFiberGlassFrameS->MakeNode(expression);
        centerFiberGlassFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *centerFiberGlassFrameV = new TGeoVolume(TString("centerFiberGlassFrameV")+=TString("_") + frames->GetName(), centerFiberGlassFrameS);

    //volume medium
    TGeoMedium *centerFiberGlassFrameV_medium = pMedFiberGlass;
    if(centerFiberGlassFrameV_medium) {
        centerFiberGlassFrameV->SetMedium(centerFiberGlassFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for centerFiberGlassFrameV!");

    //volume visual property (transparency)
    centerFiberGlassFrameV->SetLineColor(TColor::GetColor("#ccffcc"));
    centerFiberGlassFrameV->SetTransparency(0);

    TGeoCombiTrans *centerFiberGlassFrame_transf[1];

    centerFiberGlassFrame_transf[0] = new TGeoCombiTrans();
    centerFiberGlassFrame_transf[0]->SetDx(0.0);
    centerFiberGlassFrame_transf[0]->SetDy(0.0);
    centerFiberGlassFrame_transf[0]->SetDz(0.0);

    frames->AddNode(centerFiberGlassFrameV, 0, centerFiberGlassFrame_transf[0]);
    //--------------------------------------------------------------------------

    //center brass frame  ------------------------------------------------------
    Double_t centerBrassFrameSidePart_XSize = 1.45; //cm
    Double_t centerBrassFrameSidePart_YSize = 44.6; //cm
    Double_t centerBrassFrameSidePart_ZSize = 0.005; //cm

    Double_t centerBrassFrameLongPart_XSize = 66.5; //cm
    Double_t centerBrassFrameLongPart_YSize = 1.45; //cm
    Double_t centerBrassFrameLongPart_ZSize = 0.005; //cm

    Double_t distanceBetweenBrassFrameSideParts = 66.5; //cm
    Double_t distanceBetweenBrassFrameLongParts = 41.7; //cm

    TGeoShape *centerBrassFrameSidePartS = new TGeoBBox(TString("centerBrassFrameSidePartS")+=TString("_") + frames->GetName(), centerBrassFrameSidePart_XSize*0.5, centerBrassFrameSidePart_YSize*0.5, centerBrassFrameSidePart_ZSize*0.5);
    TGeoShape *centerBrassFrameLongPartS = new TGeoBBox(TString("centerBrassFrameLongPartS")+=TString("_") + frames->GetName(), centerBrassFrameLongPart_XSize*0.5, centerBrassFrameLongPart_YSize*0.5, centerBrassFrameLongPart_ZSize*0.5);

    TGeoTranslation *centerBrassFrameSidePart_pos1 = new TGeoTranslation();
        centerBrassFrameSidePart_pos1->SetName(TString("centerBrassFrameSidePart_pos1")+=TString("_") + frames->GetName());
        centerBrassFrameSidePart_pos1->SetDx(+(distanceBetweenBrassFrameSideParts + centerBrassFrameSidePart_XSize)*0.5);
        centerBrassFrameSidePart_pos1->SetDy(0.0);
        centerBrassFrameSidePart_pos1->SetDz(0.0);
        centerBrassFrameSidePart_pos1->RegisterYourself();

    TGeoTranslation *centerBrassFrameSidePart_pos2 = new TGeoTranslation();
        centerBrassFrameSidePart_pos2->SetName(TString("centerBrassFrameSidePart_pos2")+=TString("_") + frames->GetName());
        centerBrassFrameSidePart_pos2->SetDx(-(distanceBetweenBrassFrameSideParts + centerBrassFrameSidePart_XSize)*0.5);
        centerBrassFrameSidePart_pos2->SetDy(0.0);
        centerBrassFrameSidePart_pos2->SetDz(0.0);
        centerBrassFrameSidePart_pos2->RegisterYourself();

    TGeoTranslation *centerBrassFrameLongPart_top = new TGeoTranslation();
        centerBrassFrameLongPart_top->SetName(TString("centerBrassFrameLongPart_top")+=TString("_") + frames->GetName());
        centerBrassFrameLongPart_top->SetDx(0.0);
        centerBrassFrameLongPart_top->SetDy(+(distanceBetweenBrassFrameLongParts + centerBrassFrameLongPart_YSize)*0.5);
        centerBrassFrameLongPart_top->SetDz(0.0);
        centerBrassFrameLongPart_top->RegisterYourself();

    TGeoTranslation *centerBrassFrameLongPart_bottom = new TGeoTranslation();
        centerBrassFrameLongPart_bottom->SetName(TString("centerBrassFrameLongPart_bottom")+=TString("_") + frames->GetName());
        centerBrassFrameLongPart_bottom->SetDx(0.0);
        centerBrassFrameLongPart_bottom->SetDy(-(distanceBetweenBrassFrameLongParts + centerBrassFrameLongPart_YSize)*0.5);
        centerBrassFrameLongPart_bottom->SetDz(0.0);
        centerBrassFrameLongPart_bottom->RegisterYourself();

    TGeoCompositeShape *centerBrassFrameS = new TGeoCompositeShape();
    centerBrassFrameS->SetName(TString("centerBrassFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "centerBrassFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":centerBrassFrameSidePart_pos1"; expression += TString("_") + frames->GetName();
            expression += "+centerBrassFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":centerBrassFrameSidePart_pos2"; expression += TString("_") + frames->GetName();
            expression += "+centerBrassFrameLongPartS"; expression += TString("_") + frames->GetName();
            expression += ":centerBrassFrameLongPart_top"; expression += TString("_") + frames->GetName();
            expression += "+centerBrassFrameLongPartS"; expression += TString("_") + frames->GetName();
            expression += ":centerBrassFrameLongPart_bottom"; expression += TString("_") + frames->GetName();

        centerBrassFrameS->MakeNode(expression);
        centerBrassFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *centerBrassFrameV = new TGeoVolume(TString("centerBrassFrameV")+=TString("_") + frames->GetName(), centerBrassFrameS);

    //volume medium
    TGeoMedium *centerBrassFrameV_medium = pMedBrass;
    if(centerBrassFrameV_medium) {
        centerBrassFrameV->SetMedium(centerBrassFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for centerBrassFrameV!");

    //volume visual property (transparency)
    centerBrassFrameV->SetLineColor(TColor::GetColor("#f07422"));
    centerBrassFrameV->SetTransparency(0);

    TGeoCombiTrans *centerBrassFrame_transf[1];

    centerBrassFrame_transf[0] = new TGeoCombiTrans();
    centerBrassFrame_transf[0]->SetDx(0.0);
    centerBrassFrame_transf[0]->SetDy(0.0);
    centerBrassFrame_transf[0]->SetDz(-(centerFiberGlassFrameSidePart_ZSize*0.5 + centerBrassFrameSidePart_ZSize*0.5));

    frames->AddNode(centerBrassFrameV, 0, centerBrassFrame_transf[0]);
    //--------------------------------------------------------------------------

    //center steel frame  ------------------------------------------------------
    Double_t centerSteelFrameSidePart_XSize = 1.45; //cm
    Double_t centerSteelFrameSidePart_YSize = 44.6; //cm
    Double_t centerSteelFrameSidePart_ZSize = 0.005; //cm

    Double_t centerSteelFrameLongPart_XSize = 66.5; //cm
    Double_t centerSteelFrameLongPart_YSize = 1.45; //cm
    Double_t centerSteelFrameLongPart_ZSize = 0.005; //cm

    Double_t distanceBetweenSteelFrameSideParts = 66.5; //cm
    Double_t distanceBetweenSteelFrameLongParts = 41.7; //cm

    TGeoShape *centerSteelFrameSidePartS = new TGeoBBox(TString("centerSteelFrameSidePartS")+=TString("_") + frames->GetName(), centerSteelFrameSidePart_XSize*0.5, centerSteelFrameSidePart_YSize*0.5, centerSteelFrameSidePart_ZSize*0.5);
    TGeoShape *centerSteelFrameLongPartS = new TGeoBBox(TString("centerSteelFrameLongPartS")+=TString("_") + frames->GetName(), centerSteelFrameLongPart_XSize*0.5, centerSteelFrameLongPart_YSize*0.5, centerSteelFrameLongPart_ZSize*0.5);

    TGeoTranslation *centerSteelFrameSidePart_pos1 = new TGeoTranslation();
        centerSteelFrameSidePart_pos1->SetName(TString("centerSteelFrameSidePart_pos1")+=TString("_") + frames->GetName());
        centerSteelFrameSidePart_pos1->SetDx(+(distanceBetweenSteelFrameSideParts + centerSteelFrameSidePart_XSize)*0.5);
        centerSteelFrameSidePart_pos1->SetDy(0.0);
        centerSteelFrameSidePart_pos1->SetDz(0.0);
        centerSteelFrameSidePart_pos1->RegisterYourself();

    TGeoTranslation *centerSteelFrameSidePart_pos2 = new TGeoTranslation();
        centerSteelFrameSidePart_pos2->SetName(TString("centerSteelFrameSidePart_pos2")+=TString("_") + frames->GetName());
        centerSteelFrameSidePart_pos2->SetDx(-(distanceBetweenSteelFrameSideParts + centerSteelFrameSidePart_XSize)*0.5);
        centerSteelFrameSidePart_pos2->SetDy(0.0);
        centerSteelFrameSidePart_pos2->SetDz(0.0);
        centerSteelFrameSidePart_pos2->RegisterYourself();

    TGeoTranslation *centerSteelFrameLongPart_top = new TGeoTranslation();
        centerSteelFrameLongPart_top->SetName(TString("centerSteelFrameLongPart_top")+=TString("_") + frames->GetName());
        centerSteelFrameLongPart_top->SetDx(0.0);
        centerSteelFrameLongPart_top->SetDy(+(distanceBetweenSteelFrameLongParts + centerSteelFrameLongPart_YSize)*0.5);
        centerSteelFrameLongPart_top->SetDz(0.0);
        centerSteelFrameLongPart_top->RegisterYourself();

    TGeoTranslation *centerSteelFrameLongPart_bottom = new TGeoTranslation();
        centerSteelFrameLongPart_bottom->SetName(TString("centerSteelFrameLongPart_bottom")+=TString("_") + frames->GetName());
        centerSteelFrameLongPart_bottom->SetDx(0.0);
        centerSteelFrameLongPart_bottom->SetDy(-(distanceBetweenSteelFrameLongParts + centerSteelFrameLongPart_YSize)*0.5);
        centerSteelFrameLongPart_bottom->SetDz(0.0);
        centerSteelFrameLongPart_bottom->RegisterYourself();

    TGeoCompositeShape *centerSteelFrameS = new TGeoCompositeShape();
    centerSteelFrameS->SetName(TString("centerSteelFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "centerSteelFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":centerSteelFrameSidePart_pos1"; expression += TString("_") + frames->GetName();
            expression += "+centerSteelFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":centerSteelFrameSidePart_pos2"; expression += TString("_") + frames->GetName();
            expression += "+centerSteelFrameLongPartS"; expression += TString("_") + frames->GetName();
            expression += ":centerSteelFrameLongPart_top"; expression += TString("_") + frames->GetName();
            expression += "+centerSteelFrameLongPartS"; expression += TString("_") + frames->GetName();
            expression += ":centerSteelFrameLongPart_bottom"; expression += TString("_") + frames->GetName();

        centerSteelFrameS->MakeNode(expression);
        centerSteelFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *centerSteelFrameV = new TGeoVolume(TString("centerSteelFrameV")+=TString("_") + frames->GetName(), centerSteelFrameS);

    //volume medium
    TGeoMedium *centerSteelFrameV_medium = pMedSteel;
    if(centerSteelFrameV_medium) {
        centerSteelFrameV->SetMedium(centerSteelFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for centerSteelFrameV!");

    //volume visual property (transparency)
    centerSteelFrameV->SetLineColor(TColor::GetColor("#42aaff"));
    centerSteelFrameV->SetTransparency(0);

    TGeoCombiTrans *centerSteelFrame_transf[1];

    centerSteelFrame_transf[0] = new TGeoCombiTrans();
    centerSteelFrame_transf[0]->SetDx(0.0);
    centerSteelFrame_transf[0]->SetDy(0.0);
    centerSteelFrame_transf[0]->SetDz(-(centerFiberGlassFrameSidePart_ZSize*0.5 + centerSteelFrameSidePart_ZSize + centerSteelFrameSidePart_ZSize*0.5));

    frames->AddNode(centerSteelFrameV, 0, centerSteelFrame_transf[0]);
    //--------------------------------------------------------------------------

    //upper fiberglass side block ----------------------------------------------
    Double_t upperFiberGlassSideBlock_XSize = 3.3; //cm
    Double_t upperFiberGlassSideBlock_YSize = 16.8; //cm
    Double_t upperFiberGlassSideBlock_ZSize = 1.6; //cm

    TGeoShape *upperFiberGlassSideBlockS = new TGeoBBox(TString("upperFiberGlassSideBlockS")+=TString("_") + frames->GetName(), upperFiberGlassSideBlock_XSize*0.5, upperFiberGlassSideBlock_YSize*0.5, upperFiberGlassSideBlock_ZSize*0.5);

    TGeoVolume *upperFiberGlassSideBlockV = new TGeoVolume(TString("upperFiberGlassSideBlockV")+=TString("_") + frames->GetName(), upperFiberGlassSideBlockS);

    //volume medium
    TGeoMedium *upperFiberGlassSideBlockV_medium = pMedFiberGlass;
    if(upperFiberGlassSideBlockV_medium) {
        upperFiberGlassSideBlockV->SetMedium(upperFiberGlassSideBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for upperFiberGlassSideBlockV!");

    //volume visual property (transparency)
    upperFiberGlassSideBlockV->SetLineColor(TColor::GetColor("#ccffcc"));
    upperFiberGlassSideBlockV->SetTransparency(0);

    TGeoCombiTrans *upperFiberGlassSideBlock_transf[1];

    upperFiberGlassSideBlock_transf[0] = new TGeoCombiTrans();
    upperFiberGlassSideBlock_transf[0]->SetDx(-(distanceBetweenFiberGlassFrameSideParts*0.5 + centerFiberGlassFrameSidePart_XSize + upperFiberGlassSideBlock_XSize*0.5));
    upperFiberGlassSideBlock_transf[0]->SetDy(+(centerFiberGlassFrameSidePart_YSize*0.5 - upperFiberGlassSideBlock_YSize*0.5));
    upperFiberGlassSideBlock_transf[0]->SetDz(-(upperFiberGlassSideBlock_ZSize*0.5 - centerFiberGlassFrameSidePart_ZSize*0.5));

    frames->AddNode(upperFiberGlassSideBlockV, 0, upperFiberGlassSideBlock_transf[0]);
    //--------------------------------------------------------------------------

    //FRONT COVER  =============================================================
    Double_t frontFiberGlassCover_XSize = 69.4; //cm
    Double_t frontFiberGlassCover_YSize = 44.6; //cm
    Double_t frontFiberGlassCover_ZSize = 0.692; //cm

    Double_t frontCover_dXshift = 0.0; //cm
    Double_t frontCover_dYshift = +0.1*0.5/*shift*/; //cm
    Double_t frontCover_dZshift = -centerFiberGlassFrameSidePart_ZSize*0.5 - centerSteelFrameSidePart_ZSize - centerSteelFrameSidePart_ZSize; //cm

    //small epoxide inner layer ------------------------------------------------
    Double_t smallEpoxideInnerLayer_XSize = 65.6; //cm
    Double_t smallEpoxideInnerLayer_YSize = 41.6; //cm
    Double_t smallEpoxideInnerLayer_ZSize = 0.05 + 0.01*0.5; //cm

    TGeoShape *smallEpoxideInnerLayerS = new TGeoBBox(TString("smallEpoxideInnerLayerS")+=TString("_") + frames->GetName(), smallEpoxideInnerLayer_XSize*0.5, smallEpoxideInnerLayer_YSize*0.5, smallEpoxideInnerLayer_ZSize*0.5);

    TGeoVolume *smallEpoxideInnerLayerV = new TGeoVolume(TString("smallEpoxideInnerLayerV")+=TString("_") + frames->GetName(), smallEpoxideInnerLayerS);

    //volume medium
    TGeoMedium *smallEpoxideInnerLayerV_medium = pMedEpoxideCompound;
    if(smallEpoxideInnerLayerV_medium) {
        smallEpoxideInnerLayerV->SetMedium(smallEpoxideInnerLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for smallEpoxideInnerLayerV!");

    //volume visual property (transparency)
    smallEpoxideInnerLayerV->SetLineColor(TColor::GetColor("#aa33ff"));
    smallEpoxideInnerLayerV->SetTransparency(0);

    TGeoCombiTrans *smallEpoxideInnerLayer_transf[1];

    smallEpoxideInnerLayer_transf[0] = new TGeoCombiTrans();
    smallEpoxideInnerLayer_transf[0]->SetDx(frontCover_dXshift);
    smallEpoxideInnerLayer_transf[0]->SetDy(frontCover_dYshift);
    smallEpoxideInnerLayer_transf[0]->SetDz(-smallEpoxideInnerLayer_ZSize*0.5 + frontCover_dZshift);

    frames->AddNode(smallEpoxideInnerLayerV, 0, smallEpoxideInnerLayer_transf[0]);
    //--------------------------------------------------------------------------

    //small honeycomb layer ----------------------------------------------------
    Double_t smallHoneyCombLayer_XSize = 65.6; //cm
    Double_t smallHoneyCombLayer_YSize = 41.6; //cm
    Double_t smallHoneyCombLayer_ZSize = 0.592; //cm

    TGeoShape *smallHoneyCombLayerS = new TGeoBBox(TString("smallHoneyCombLayerS")+=TString("_") + frames->GetName(), smallHoneyCombLayer_XSize*0.5, smallHoneyCombLayer_YSize*0.5, smallHoneyCombLayer_ZSize*0.5);

    TGeoVolume *smallHoneyCombLayerV = new TGeoVolume(TString("smallHoneyCombLayerV")+=TString("_") + frames->GetName(), smallHoneyCombLayerS);

    //volume medium
    TGeoMedium *smallHoneyCombLayerV_medium = pMedHoneyComb;
    if(smallHoneyCombLayerV_medium) {
        smallHoneyCombLayerV->SetMedium(smallHoneyCombLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for smallHoneyCombLayerV!");

    //volume visual property (transparency)
    smallHoneyCombLayerV->SetLineColor(TColor::GetColor("#ffdf84"));
    smallHoneyCombLayerV->SetTransparency(0);

    TGeoCombiTrans *smallHoneyCombLayer_transf[1];

    smallHoneyCombLayer_transf[0] = new TGeoCombiTrans();
    smallHoneyCombLayer_transf[0]->SetDx(frontCover_dXshift);
    smallHoneyCombLayer_transf[0]->SetDy(frontCover_dYshift);
    smallHoneyCombLayer_transf[0]->SetDz(-smallHoneyCombLayer_ZSize*0.5 - smallEpoxideInnerLayer_ZSize + frontCover_dZshift);

    frames->AddNode(smallHoneyCombLayerV, 0, smallHoneyCombLayer_transf[0]);
    //--------------------------------------------------------------------------

    //small epoxide outer layer ------------------------------------------------
    Double_t smallEpoxideOuterLayer_XSize = 65.6; //cm
    Double_t smallEpoxideOuterLayer_YSize = 41.6; //cm
    Double_t smallEpoxideOuterLayer_ZSize = 0.05; //cm

    TGeoShape *smallEpoxideOuterLayerS = new TGeoBBox(TString("smallEpoxideOuterLayerS")+=TString("_") + frames->GetName(), smallEpoxideOuterLayer_XSize*0.5, smallEpoxideOuterLayer_YSize*0.5, smallEpoxideOuterLayer_ZSize*0.5);

    TGeoVolume *smallEpoxideOuterLayerV = new TGeoVolume(TString("smallEpoxideOuterLayerV")+=TString("_") + frames->GetName(), smallEpoxideOuterLayerS);

    //volume medium
    TGeoMedium *smallEpoxideOuterLayerV_medium = pMedEpoxideCompound;
    if(smallEpoxideOuterLayerV_medium) {
        smallEpoxideOuterLayerV->SetMedium(smallEpoxideOuterLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for smallEpoxideOuterLayerV!");

    //volume visual property (transparency)
    smallEpoxideOuterLayerV->SetLineColor(TColor::GetColor("#aa33ff"));
    smallEpoxideOuterLayerV->SetTransparency(0);

    TGeoCombiTrans *smallEpoxideOuterLayer_transf[1];

    smallEpoxideOuterLayer_transf[0] = new TGeoCombiTrans();
    smallEpoxideOuterLayer_transf[0]->SetDx(frontCover_dXshift);
    smallEpoxideOuterLayer_transf[0]->SetDy(frontCover_dYshift);
    smallEpoxideOuterLayer_transf[0]->SetDz(-smallEpoxideOuterLayer_ZSize*0.5 - smallHoneyCombLayer_ZSize - smallEpoxideInnerLayer_ZSize + frontCover_dZshift);

    frames->AddNode(smallEpoxideOuterLayerV, 0, smallEpoxideOuterLayer_transf[0]);
    //--------------------------------------------------------------------------

    //small fiberglass frame ---------------------------------------------------
    Double_t smallFiberGlassFrameLowerPart_XSize = 65.6; //cm
    Double_t smallFiberGlassFrameLowerPart_YSize = 1.55; //cm
    Double_t smallFiberGlassFrameLowerPart_ZSize = 0.692; //cm

    Double_t smallFiberGlassFrameUpperPart_XSize = 65.6; //cm
    Double_t smallFiberGlassFrameUpperPart_YSize = 1.45; //cm
    Double_t smallFiberGlassFrameUpperPart_ZSize = 0.692; //cm

    Double_t smallFiberGlassFrameSidePart_XSize = 1.9; //cm
    Double_t smallFiberGlassFrameSidePart_YSize = 44.6; //cm
    Double_t smallFiberGlassFrameSidePart_ZSize = 0.692; //cm

    TGeoShape *smallFiberGlassFrameLowerPartS = new TGeoBBox(TString("smallFiberGlassFrameLowerPartS")+=TString("_") + frames->GetName(), smallFiberGlassFrameLowerPart_XSize*0.5, smallFiberGlassFrameLowerPart_YSize*0.5, smallFiberGlassFrameLowerPart_ZSize*0.5);
    TGeoShape *smallFiberGlassFrameUpperPartS = new TGeoBBox(TString("smallFiberGlassFrameUpperPartS")+=TString("_") + frames->GetName(), smallFiberGlassFrameUpperPart_XSize*0.5, smallFiberGlassFrameUpperPart_YSize*0.5, smallFiberGlassFrameUpperPart_ZSize*0.5);
    TGeoShape *smallFiberGlassFrameSidePartS = new TGeoBBox(TString("smallFiberGlassFrameSidePartS")+=TString("_") + frames->GetName(), smallFiberGlassFrameSidePart_XSize*0.5, smallFiberGlassFrameSidePart_YSize*0.5, smallFiberGlassFrameSidePart_ZSize*0.5);

    TGeoTranslation *smallFiberGlassFrameLowerPart_pos = new TGeoTranslation();
        smallFiberGlassFrameLowerPart_pos->SetName(TString("smallFiberGlassFrameLowerPart_pos")+=TString("_") + frames->GetName());
        smallFiberGlassFrameLowerPart_pos->SetDx(0.0);
        smallFiberGlassFrameLowerPart_pos->SetDy(-smallEpoxideInnerLayer_YSize*0.5 - smallFiberGlassFrameLowerPart_YSize*0.5);
        smallFiberGlassFrameLowerPart_pos->SetDz(0.0);
        smallFiberGlassFrameLowerPart_pos->RegisterYourself();

    TGeoTranslation *smallFiberGlassFrameUpperPart_pos = new TGeoTranslation();
        smallFiberGlassFrameUpperPart_pos->SetName(TString("smallFiberGlassFrameUpperPart_pos")+=TString("_") + frames->GetName());
        smallFiberGlassFrameUpperPart_pos->SetDx(0.0);
        smallFiberGlassFrameUpperPart_pos->SetDy(smallEpoxideInnerLayer_YSize*0.5 + smallFiberGlassFrameUpperPart_YSize*0.5);
        smallFiberGlassFrameUpperPart_pos->SetDz(0.0);
        smallFiberGlassFrameUpperPart_pos->RegisterYourself();

    TGeoTranslation *smallFiberGlassFrameSidePart_pos1 = new TGeoTranslation();
        smallFiberGlassFrameSidePart_pos1->SetName(TString("smallFiberGlassFrameSidePart_pos1")+=TString("_") + frames->GetName());
        smallFiberGlassFrameSidePart_pos1->SetDx(smallEpoxideInnerLayer_XSize*0.5 + smallFiberGlassFrameSidePart_XSize*0.5);
        smallFiberGlassFrameSidePart_pos1->SetDy((smallFiberGlassFrameUpperPart_YSize-smallFiberGlassFrameLowerPart_YSize)*0.5);
        smallFiberGlassFrameSidePart_pos1->SetDz(0.0);
        smallFiberGlassFrameSidePart_pos1->RegisterYourself();

    TGeoTranslation *smallFiberGlassFrameSidePart_pos2 = new TGeoTranslation();
        smallFiberGlassFrameSidePart_pos2->SetName(TString("smallFiberGlassFrameSidePart_pos2")+=TString("_") + frames->GetName());
        smallFiberGlassFrameSidePart_pos2->SetDx(-(smallEpoxideInnerLayer_XSize*0.5 + smallFiberGlassFrameSidePart_XSize*0.5));
        smallFiberGlassFrameSidePart_pos2->SetDy((smallFiberGlassFrameUpperPart_YSize-smallFiberGlassFrameLowerPart_YSize)*0.5);
        smallFiberGlassFrameSidePart_pos2->SetDz(0.0);
        smallFiberGlassFrameSidePart_pos2->RegisterYourself();

    TGeoCompositeShape *smallFiberGlassFrameS = new TGeoCompositeShape();
    smallFiberGlassFrameS->SetName(TString("smallFiberGlassFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "smallFiberGlassFrameLowerPartS"; expression += TString("_") + frames->GetName();
            expression += ":smallFiberGlassFrameLowerPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+smallFiberGlassFrameUpperPartS"; expression += TString("_") + frames->GetName();
            expression += ":smallFiberGlassFrameUpperPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+smallFiberGlassFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":smallFiberGlassFrameSidePart_pos1"; expression += TString("_") + frames->GetName();
            expression += "+smallFiberGlassFrameSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":smallFiberGlassFrameSidePart_pos2"; expression += TString("_") + frames->GetName();

        smallFiberGlassFrameS->MakeNode(expression);
        smallFiberGlassFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *smallFiberGlassFrameV = new TGeoVolume(TString("smallFiberGlassFrameV")+=TString("_") + frames->GetName(), smallFiberGlassFrameS);

    //volume medium
    TGeoMedium *smallFiberGlassFrameV_medium = pMedFiberGlass;
    if(smallFiberGlassFrameV_medium) {
        smallFiberGlassFrameV->SetMedium(smallFiberGlassFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for smallFiberGlassFrameV!");

    //volume visual property (transparency)
    smallFiberGlassFrameV->SetLineColor(TColor::GetColor("#ccffcc"));
    smallFiberGlassFrameV->SetTransparency(0);

    TGeoCombiTrans *smallFiberGlassFrame_transf[1];

    smallFiberGlassFrame_transf[0] = new TGeoCombiTrans();
    smallFiberGlassFrame_transf[0]->SetDx(frontCover_dXshift);
    smallFiberGlassFrame_transf[0]->SetDy(frontCover_dYshift);
    smallFiberGlassFrame_transf[0]->SetDz(-smallFiberGlassFrameLowerPart_ZSize*0.5 + frontCover_dZshift);

    frames->AddNode(smallFiberGlassFrameV, 0, smallFiberGlassFrame_transf[0]);
    //--------------------------------------------------------------------------

    //small glue layer ---------------------------------------------------------
    Double_t smallGlueLayer_XSize = 69.4; //cm
    Double_t smallGlueLayer_YSize = 44.6; //cm
    Double_t smallGlueLayer_ZSize = 0.005+0.005; //cm (total one side thickness)

    TGeoShape *smallGlueLayerS = new TGeoBBox(TString("smallGlueLayerS")+=TString("_") + frames->GetName(), smallGlueLayer_XSize*0.5, smallGlueLayer_YSize*0.5, smallGlueLayer_ZSize*0.5);

    TGeoVolume *smallGlueLayerV = new TGeoVolume(TString("smallGlueLayerV")+=TString("_") + frames->GetName(), smallGlueLayerS);

    //volume medium
    TGeoMedium *smallGlueLayerV_medium = pMedGlue;
    if(smallGlueLayerV_medium) {
        smallGlueLayerV->SetMedium(smallGlueLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for smallGlueLayerV!");

    //volume visual property (transparency)
    smallGlueLayerV->SetLineColor(TColor::GetColor("#eeeeee"));
    smallGlueLayerV->SetTransparency(0);

    TGeoCombiTrans *smallGlueLayer_transf[1];

    smallGlueLayer_transf[0] = new TGeoCombiTrans();
    smallGlueLayer_transf[0]->SetDx(0.0);
    smallGlueLayer_transf[0]->SetDy(0.0);
    smallGlueLayer_transf[0]->SetDz(-smallGlueLayer_ZSize*0.5 - smallEpoxideOuterLayer_ZSize - smallHoneyCombLayer_ZSize - smallEpoxideInnerLayer_ZSize + frontCover_dZshift);

    frames->AddNode(smallGlueLayerV, 0, smallGlueLayer_transf[0]);
    //--------------------------------------------------------------------------

    //small polyamide layer ----------------------------------------------------
    Double_t smallPolyamideLayer_XSize = 69.4; //cm
    Double_t smallPolyamideLayer_YSize = 44.6; //cm
    Double_t smallPolyamideLayer_ZSize = 0.02*0.5; //cm

    TGeoShape *smallPolyamideLayerS = new TGeoBBox(TString("smallPolyamideLayerS")+=TString("_") + frames->GetName(), smallPolyamideLayer_XSize*0.5, smallPolyamideLayer_YSize*0.5, smallPolyamideLayer_ZSize*0.5);

    TGeoVolume *smallPolyamideLayerV = new TGeoVolume(TString("smallPolyamideLayerV")+=TString("_") + frames->GetName(), smallPolyamideLayerS);

    //volume medium
    TGeoMedium *smallPolyamideLayerV_medium = pMedPolyamide;
    if(smallPolyamideLayerV_medium) {
        smallPolyamideLayerV->SetMedium(smallPolyamideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for smallPolyamideLayerV!");

    //volume visual property (transparency)
    smallPolyamideLayerV->SetLineColor(TColor::GetColor("#fefe22"));
    smallPolyamideLayerV->SetTransparency(0);

    TGeoCombiTrans *smallPolyamideLayer_transf[1];

    smallPolyamideLayer_transf[0] = new TGeoCombiTrans();
    smallPolyamideLayer_transf[0]->SetDx(0.0);
    smallPolyamideLayer_transf[0]->SetDy(0.0);
    smallPolyamideLayer_transf[0]->SetDz(-smallPolyamideLayer_ZSize*0.5 - smallGlueLayer_ZSize - smallEpoxideOuterLayer_ZSize - smallHoneyCombLayer_ZSize - smallEpoxideInnerLayer_ZSize + frontCover_dZshift);

    frames->AddNode(smallPolyamideLayerV, 0, smallPolyamideLayer_transf[0]);
    //--------------------------------------------------------------------------

    //small copper layer ------------------------------------------------
    Double_t smallCopperLayer_XSize = 69.4; //cm
    Double_t smallCopperLayer_YSize = 44.6; //cm
    Double_t smallCopperLayer_ZSize = 0.0035 + 0.0061*0.5/*inside gem*/; //cm

    TGeoShape *smallCopperLayerS = new TGeoBBox(TString("smallCopperLayerS")+=TString("_") + frames->GetName(), smallCopperLayer_XSize*0.5, smallCopperLayer_YSize*0.5, smallCopperLayer_ZSize*0.5);

    TGeoVolume *smallCopperLayerV = new TGeoVolume(TString("smallCopperLayerV")+=TString("_") + frames->GetName(), smallCopperLayerS);

    //volume medium
    TGeoMedium *smallCopperLayerV_medium = pMedCopper;
    if(smallCopperLayerV_medium) {
        smallCopperLayerV->SetMedium(smallCopperLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for smallCopperLayerV!");

    //volume visual property (transparency)
    smallCopperLayerV->SetLineColor(TColor::GetColor("#ff7538"));
    smallCopperLayerV->SetTransparency(0);

    TGeoCombiTrans *smallCopperLayer_transf[1];

    smallCopperLayer_transf[0] = new TGeoCombiTrans();
    smallCopperLayer_transf[0]->SetDx(0.0);
    smallCopperLayer_transf[0]->SetDy(0.0);
    smallCopperLayer_transf[0]->SetDz(-smallCopperLayer_ZSize*0.5 - smallPolyamideLayer_ZSize - smallGlueLayer_ZSize - smallEpoxideOuterLayer_ZSize - smallHoneyCombLayer_ZSize - smallEpoxideInnerLayer_ZSize + frontCover_dZshift);

    frames->AddNode(smallCopperLayerV, 0, smallCopperLayer_transf[0]);
    //--------------------------------------------------------------------------
    //==========================================================================

    //BACK COVER ===============================================================
    Double_t backFiberGlassCover_XSize = 75.3; //cm
    Double_t backFiberGlassCover_YSize = 48.5; //cm
    Double_t backFiberGlassCover_ZSize = 0.662; //cm

    Double_t backCover_dXshift = -0.075/*shift*/; //cm
    Double_t backCover_dYshift = -0.125/*shift*/; //cm
    Double_t backCover_dZshift = centerFiberGlassFrameSidePart_ZSize*0.5 + 0.005; //cm

    //big epoxide inner layer ------------------------------------------------
    Double_t bigEpoxideInnerLayer_XSize = 65.95; //cm
    Double_t bigEpoxideInnerLayer_YSize = 40.85; //cm
    Double_t bigEpoxideInnerLayer_ZSize = 0.05 + 0.01*0.5; //cm

    TGeoShape *bigEpoxideInnerLayerS = new TGeoBBox(TString("bigEpoxideInnerLayerS")+=TString("_") + frames->GetName(), bigEpoxideInnerLayer_XSize*0.5, bigEpoxideInnerLayer_YSize*0.5, bigEpoxideInnerLayer_ZSize*0.5);

    TGeoVolume *bigEpoxideInnerLayerV = new TGeoVolume(TString("bigEpoxideInnerLayerV")+=TString("_") + frames->GetName(), bigEpoxideInnerLayerS);

    //volume medium
    TGeoMedium *bigEpoxideInnerLayerV_medium = pMedEpoxideCompound;
    if(bigEpoxideInnerLayerV_medium) {
        bigEpoxideInnerLayerV->SetMedium(bigEpoxideInnerLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for bigEpoxideInnerLayerV!");

    //volume visual property (transparency)
    bigEpoxideInnerLayerV->SetLineColor(TColor::GetColor("#aa33ff"));
    bigEpoxideInnerLayerV->SetTransparency(0);

    TGeoCombiTrans *bigEpoxideInnerLayer_transf[1];

    bigEpoxideInnerLayer_transf[0] = new TGeoCombiTrans();
    bigEpoxideInnerLayer_transf[0]->SetDx(backCover_dXshift);
    bigEpoxideInnerLayer_transf[0]->SetDy(backCover_dYshift);
    bigEpoxideInnerLayer_transf[0]->SetDz(bigEpoxideInnerLayer_ZSize*0.5 + backCover_dZshift);

    frames->AddNode(bigEpoxideInnerLayerV, 0, bigEpoxideInnerLayer_transf[0]);
    //--------------------------------------------------------------------------

    //big honeycomb layer ----------------------------------------------------
    Double_t bigHoneyCombLayer_XSize = 65.95; //cm
    Double_t bigHoneyCombLayer_YSize = 40.85; //cm
    Double_t bigHoneyCombLayer_ZSize = 0.562; //cm

    TGeoShape *bigHoneyCombLayerS = new TGeoBBox(TString("bigHoneyCombLayerS")+=TString("_") + frames->GetName(), bigHoneyCombLayer_XSize*0.5, bigHoneyCombLayer_YSize*0.5, bigHoneyCombLayer_ZSize*0.5);

    TGeoVolume *bigHoneyCombLayerV = new TGeoVolume(TString("bigHoneyCombLayerV")+=TString("_") + frames->GetName(), bigHoneyCombLayerS);

    //volume medium
    TGeoMedium *bigHoneyCombLayerV_medium = pMedHoneyComb;
    if(bigHoneyCombLayerV_medium) {
        bigHoneyCombLayerV->SetMedium(bigHoneyCombLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for bigHoneyCombLayerV!");

    //volume visual property (transparency)
    bigHoneyCombLayerV->SetLineColor(TColor::GetColor("#ffdf84"));
    bigHoneyCombLayerV->SetTransparency(0);

    TGeoCombiTrans *bigHoneyCombLayer_transf[1];

    bigHoneyCombLayer_transf[0] = new TGeoCombiTrans();
    bigHoneyCombLayer_transf[0]->SetDx(backCover_dXshift);
    bigHoneyCombLayer_transf[0]->SetDy(backCover_dYshift);
    bigHoneyCombLayer_transf[0]->SetDz(bigHoneyCombLayer_ZSize*0.5 + bigEpoxideInnerLayer_ZSize + backCover_dZshift);

    frames->AddNode(bigHoneyCombLayerV, 0, bigHoneyCombLayer_transf[0]);
    //--------------------------------------------------------------------------

    //big epoxide outer layer --------------------------------------------------
    Double_t bigEpoxideOuterLayer_XSize = 65.95; //cm
    Double_t bigEpoxideOuterLayer_YSize = 40.85; //cm
    Double_t bigEpoxideOuterLayer_ZSize = 0.05; //cm

    TGeoShape *bigEpoxideOuterLayerS = new TGeoBBox(TString("bigEpoxideOuterLayerS")+=TString("_") + frames->GetName(), bigEpoxideOuterLayer_XSize*0.5, bigEpoxideOuterLayer_YSize*0.5, bigEpoxideOuterLayer_ZSize*0.5);

    TGeoVolume *bigEpoxideOuterLayerV = new TGeoVolume(TString("bigEpoxideOuterLayerV")+=TString("_") + frames->GetName(), bigEpoxideOuterLayerS);

    //volume medium
    TGeoMedium *bigEpoxideOuterLayerV_medium = pMedEpoxideCompound;
    if(bigEpoxideOuterLayerV_medium) {
        bigEpoxideOuterLayerV->SetMedium(bigEpoxideOuterLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for bigEpoxideOuterLayerV!");

    //volume visual property (transparency)
    bigEpoxideOuterLayerV->SetLineColor(TColor::GetColor("#aa33ff"));
    bigEpoxideOuterLayerV->SetTransparency(0);

    TGeoCombiTrans *bigEpoxideOuterLayer_transf[1];

    bigEpoxideOuterLayer_transf[0] = new TGeoCombiTrans();
    bigEpoxideOuterLayer_transf[0]->SetDx(backCover_dXshift);
    bigEpoxideOuterLayer_transf[0]->SetDy(backCover_dYshift);
    bigEpoxideOuterLayer_transf[0]->SetDz(bigEpoxideOuterLayer_ZSize*0.5 + bigHoneyCombLayer_ZSize + bigEpoxideInnerLayer_ZSize + backCover_dZshift);

    frames->AddNode(bigEpoxideOuterLayerV, 0, bigEpoxideOuterLayer_transf[0]);
    //--------------------------------------------------------------------------

    //big fiberglass frame -----------------------------------------------------
    Double_t bigFiberGlassFrameLowerPart_XSize = 65.95; //cm
    Double_t bigFiberGlassFrameLowerPart_YSize = 1.75; //cm
    Double_t bigFiberGlassFrameLowerPart_ZSize = 0.662; //cm

    Double_t bigFiberGlassFrameUpperPart_XSize = 65.95; //cm
    Double_t bigFiberGlassFrameUpperPart_YSize = 5.9; //cm
    Double_t bigFiberGlassFrameUpperPart_ZSize = 0.662; //cm

    Double_t bigFiberGlassFrameSmallSidePart_XSize = 1.8; //cm
    Double_t bigFiberGlassFrameSmallSidePart_YSize = 48.5; //cm
    Double_t bigFiberGlassFrameSmallSidePart_ZSize = 0.662; //cm

    Double_t bigFiberGlassFrameBigSidePart_XSize = 7.55; //cm
    Double_t bigFiberGlassFrameBigSidePart_YSize = 48.5; //cm
    Double_t bigFiberGlassFrameBigSidePart_ZSize = 0.662; //cm

    TGeoShape *bigFiberGlassFrameLowerPartS = new TGeoBBox(TString("bigFiberGlassFrameLowerPartS")+=TString("_") + frames->GetName(), bigFiberGlassFrameLowerPart_XSize*0.5, bigFiberGlassFrameLowerPart_YSize*0.5, bigFiberGlassFrameLowerPart_ZSize*0.5);
    TGeoShape *bigFiberGlassFrameUpperPartS = new TGeoBBox(TString("bigFiberGlassFrameUpperPartS")+=TString("_") + frames->GetName(), bigFiberGlassFrameUpperPart_XSize*0.5, bigFiberGlassFrameUpperPart_YSize*0.5, bigFiberGlassFrameUpperPart_ZSize*0.5);
    TGeoShape *bigFiberGlassFrameSmallSidePartS = new TGeoBBox(TString("bigFiberGlassFrameSmallSidePartS")+=TString("_") + frames->GetName(), bigFiberGlassFrameSmallSidePart_XSize*0.5, bigFiberGlassFrameSmallSidePart_YSize*0.5, bigFiberGlassFrameSmallSidePart_ZSize*0.5);
    TGeoShape *bigFiberGlassFrameBigSidePartS = new TGeoBBox(TString("bigFiberGlassFrameBigSidePartS")+=TString("_") + frames->GetName(), bigFiberGlassFrameBigSidePart_XSize*0.5, bigFiberGlassFrameBigSidePart_YSize*0.5, bigFiberGlassFrameBigSidePart_ZSize*0.5);

    TGeoTranslation *bigFiberGlassFrameLowerPart_pos = new TGeoTranslation();
        bigFiberGlassFrameLowerPart_pos->SetName(TString("bigFiberGlassFrameLowerPart_pos")+=TString("_") + frames->GetName());
        bigFiberGlassFrameLowerPart_pos->SetDx(0.0);
        bigFiberGlassFrameLowerPart_pos->SetDy(-bigEpoxideInnerLayer_YSize*0.5 - bigFiberGlassFrameLowerPart_YSize*0.5);
        bigFiberGlassFrameLowerPart_pos->SetDz(0.0);
        bigFiberGlassFrameLowerPart_pos->RegisterYourself();

    TGeoTranslation *bigFiberGlassFrameUpperPart_pos = new TGeoTranslation();
        bigFiberGlassFrameUpperPart_pos->SetName(TString("bigFiberGlassFrameUpperPart_pos")+=TString("_") + frames->GetName());
        bigFiberGlassFrameUpperPart_pos->SetDx(0.0);
        bigFiberGlassFrameUpperPart_pos->SetDy(bigEpoxideInnerLayer_YSize*0.5 + bigFiberGlassFrameUpperPart_YSize*0.5);
        bigFiberGlassFrameUpperPart_pos->SetDz(0.0);
        bigFiberGlassFrameUpperPart_pos->RegisterYourself();

    TGeoTranslation *bigFiberGlassFrameSmallSidePart_pos = new TGeoTranslation();
        bigFiberGlassFrameSmallSidePart_pos->SetName(TString("bigFiberGlassFrameSmallSidePart_pos")+=TString("_") + frames->GetName());
        bigFiberGlassFrameSmallSidePart_pos->SetDx(bigEpoxideInnerLayer_XSize*0.5 + bigFiberGlassFrameSmallSidePart_XSize*0.5);
        bigFiberGlassFrameSmallSidePart_pos->SetDy((bigFiberGlassFrameUpperPart_YSize-bigFiberGlassFrameLowerPart_YSize)*0.5);
        bigFiberGlassFrameSmallSidePart_pos->SetDz(0.0);
        bigFiberGlassFrameSmallSidePart_pos->RegisterYourself();

    TGeoTranslation *bigFiberGlassFrameBigSidePart_pos = new TGeoTranslation();
        bigFiberGlassFrameBigSidePart_pos->SetName(TString("bigFiberGlassFrameBigSidePart_pos")+=TString("_") + frames->GetName());
        bigFiberGlassFrameBigSidePart_pos->SetDx(-(bigEpoxideInnerLayer_XSize*0.5 + bigFiberGlassFrameBigSidePart_XSize*0.5));
        bigFiberGlassFrameBigSidePart_pos->SetDy((bigFiberGlassFrameUpperPart_YSize-bigFiberGlassFrameLowerPart_YSize)*0.5);
        bigFiberGlassFrameBigSidePart_pos->SetDz(0.0);
        bigFiberGlassFrameBigSidePart_pos->RegisterYourself();

    TGeoCompositeShape *bigFiberGlassFrameS = new TGeoCompositeShape();
    bigFiberGlassFrameS->SetName(TString("bigFiberGlassFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "bigFiberGlassFrameLowerPartS"; expression += TString("_") + frames->GetName();
            expression += ":bigFiberGlassFrameLowerPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+bigFiberGlassFrameUpperPartS"; expression += TString("_") + frames->GetName();
            expression += ":bigFiberGlassFrameUpperPart_pos"; expression += TString("_") + frames->GetName();
            expression += "+bigFiberGlassFrameSmallSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":bigFiberGlassFrameSmallSidePart_pos"; expression += TString("_") + frames->GetName();
            expression += "+bigFiberGlassFrameBigSidePartS"; expression += TString("_") + frames->GetName();
            expression += ":bigFiberGlassFrameBigSidePart_pos"; expression += TString("_") + frames->GetName();

        bigFiberGlassFrameS->MakeNode(expression);
        bigFiberGlassFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *bigFiberGlassFrameV = new TGeoVolume(TString("bigFiberGlassFrameV")+=TString("_") + frames->GetName(), bigFiberGlassFrameS);

    //volume medium
    TGeoMedium *bigFiberGlassFrameV_medium = pMedFiberGlass;
    if(bigFiberGlassFrameV_medium) {
        bigFiberGlassFrameV->SetMedium(bigFiberGlassFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for bigFiberGlassFrameV!");

    //volume visual property (transparency)
    bigFiberGlassFrameV->SetLineColor(TColor::GetColor("#ccffcc"));
    bigFiberGlassFrameV->SetTransparency(0);

    TGeoCombiTrans *bigFiberGlassFrame_transf[1];

    bigFiberGlassFrame_transf[0] = new TGeoCombiTrans();
    bigFiberGlassFrame_transf[0]->SetDx(backCover_dXshift);
    bigFiberGlassFrame_transf[0]->SetDy(backCover_dYshift);
    bigFiberGlassFrame_transf[0]->SetDz(bigFiberGlassFrameLowerPart_ZSize*0.5 + backCover_dZshift);

    frames->AddNode(bigFiberGlassFrameV, 0, bigFiberGlassFrame_transf[0]);
    //--------------------------------------------------------------------------

    //big glue layer -----------------------------------------------------------
    Double_t bigGlueLayer_XSize = 75.3; //cm
    Double_t bigGlueLayer_YSize = 48.5; //cm
    Double_t bigGlueLayer_ZSize = 0.005+0.005; //cm (total one side thickness)

    TGeoShape *bigGlueLayerS = new TGeoBBox(TString("bigGlueLayerS")+=TString("_") + frames->GetName(), bigGlueLayer_XSize*0.5, bigGlueLayer_YSize*0.5, bigGlueLayer_ZSize*0.5);

    TGeoVolume *bigGlueLayerV = new TGeoVolume(TString("bigGlueLayerV")+=TString("_") + frames->GetName(), bigGlueLayerS);

    //volume medium
    TGeoMedium *bigGlueLayerV_medium = pMedGlue;
    if(bigGlueLayerV_medium) {
        bigGlueLayerV->SetMedium(bigGlueLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for bigGlueLayerV!");

    //volume visual property (transparency)
    bigGlueLayerV->SetLineColor(TColor::GetColor("#eeeeee"));
    bigGlueLayerV->SetTransparency(0);

    TGeoCombiTrans *bigGlueLayer_transf[1];

    bigGlueLayer_transf[0] = new TGeoCombiTrans();
    bigGlueLayer_transf[0]->SetDx(-2.95/*shift*/);
    bigGlueLayer_transf[0]->SetDy(+1.95/*shift*/);
    bigGlueLayer_transf[0]->SetDz(bigGlueLayer_ZSize*0.5 + bigEpoxideOuterLayer_ZSize + bigHoneyCombLayer_ZSize + bigEpoxideInnerLayer_ZSize + backCover_dZshift);

    frames->AddNode(bigGlueLayerV, 0, bigGlueLayer_transf[0]);
    //--------------------------------------------------------------------------

    //big polyamide layer ------------------------------------------------------
    Double_t bigPolyamideLayer_XSize = 75.3; //cm
    Double_t bigPolyamideLayer_YSize = 48.5; //cm
    Double_t bigPolyamideLayer_ZSize = 0.02*0.5; //cm (total one side thickness)

    TGeoShape *bigPolyamideLayerS = new TGeoBBox(TString("bigPolyamideLayerS")+=TString("_") + frames->GetName(), bigPolyamideLayer_XSize*0.5, bigPolyamideLayer_YSize*0.5, bigPolyamideLayer_ZSize*0.5);

    TGeoVolume *bigPolyamideLayerV = new TGeoVolume(TString("bigPolyamideLayerV")+=TString("_") + frames->GetName(), bigPolyamideLayerS);

    //volume medium
    TGeoMedium *bigPolyamideLayerV_medium = pMedPolyamide;
    if(bigPolyamideLayerV_medium) {
        bigPolyamideLayerV->SetMedium(bigPolyamideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for bigPolyamideLayerV!");

    //volume visual property (transparency)
    bigPolyamideLayerV->SetLineColor(TColor::GetColor("#fefe22"));
    bigPolyamideLayerV->SetTransparency(0);

    TGeoCombiTrans *bigPolyamideLayer_transf[1];

    bigPolyamideLayer_transf[0] = new TGeoCombiTrans();
    bigPolyamideLayer_transf[0]->SetDx(-2.95/*shift*/);
    bigPolyamideLayer_transf[0]->SetDy(+1.95/*shift*/);
    bigPolyamideLayer_transf[0]->SetDz(bigPolyamideLayer_ZSize*0.5 + bigGlueLayer_ZSize + bigEpoxideOuterLayer_ZSize + bigHoneyCombLayer_ZSize + bigEpoxideInnerLayer_ZSize + backCover_dZshift);

    frames->AddNode(bigPolyamideLayerV, 0, bigPolyamideLayer_transf[0]);
    //--------------------------------------------------------------------------

    //big copper layer ---------------------------------------------------------
    Double_t bigCopperLayer_XSize = 75.3; //cm
    Double_t bigCopperLayer_YSize = 48.5; //cm
    Double_t bigCopperLayer_ZSize = 0.0035 + 0.0061*0.5/*inside gem*/; //cm

    TGeoShape *bigCopperLayerS = new TGeoBBox(TString("bigCopperLayerS")+=TString("_") + frames->GetName(), bigCopperLayer_XSize*0.5, bigCopperLayer_YSize*0.5, bigCopperLayer_ZSize*0.5);

    TGeoVolume *bigCopperLayerV = new TGeoVolume(TString("bigCopperLayerV")+=TString("_") + frames->GetName(), bigCopperLayerS);

    //volume medium
    TGeoMedium *bigCopperLayerV_medium = pMedCopper;
    if(bigCopperLayerV_medium) {
        bigCopperLayerV->SetMedium(bigCopperLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for bigCopperLayerV!");

    //volume visual property (transparency)
    bigCopperLayerV->SetLineColor(TColor::GetColor("#ff7538"));
    bigCopperLayerV->SetTransparency(0);

    TGeoCombiTrans *bigCopperLayer_transf[1];

    bigCopperLayer_transf[0] = new TGeoCombiTrans();
    bigCopperLayer_transf[0]->SetDx(-2.95/*shift*/);
    bigCopperLayer_transf[0]->SetDy(+1.95/*shift*/);
    bigCopperLayer_transf[0]->SetDz(bigCopperLayer_ZSize*0.5 + bigPolyamideLayer_ZSize + bigGlueLayer_ZSize + bigEpoxideOuterLayer_ZSize + bigHoneyCombLayer_ZSize + bigEpoxideInnerLayer_ZSize + backCover_dZshift);

    frames->AddNode(bigCopperLayerV, 0, bigCopperLayer_transf[0]);
    //--------------------------------------------------------------------------
    //==========================================================================

    //big side plate -----------------------------------------------------------
    Double_t bigSidePlate_XSize = 5.2; //cm
    Double_t bigSidePlate_YSize = 27.6; //cm
    Double_t bigSidePlate_ZSize = 0.3; //cm

    TGeoShape *bigSidePlateS = new TGeoBBox(TString("bigSidePlateS")+=TString("_") + frames->GetName(), bigSidePlate_XSize*0.5, bigSidePlate_YSize*0.5, bigSidePlate_ZSize*0.5);

    TGeoVolume *bigSidePlateV = new TGeoVolume(TString("bigSidePlateV")+=TString("_") + frames->GetName(), bigSidePlateS);

    //volume medium
    TGeoMedium *bigSidePlateV_medium = pMedAluminium;
    if(bigSidePlateV_medium) {
        bigSidePlateV->SetMedium(bigSidePlateV_medium);
    }
    else Fatal("Main", "Invalid medium for bigSidePlateV!");

    //volume visual property (transparency)
    bigSidePlateV->SetLineColor(TColor::GetColor("#ffcccc"));
    bigSidePlateV->SetTransparency(0);

    TGeoCombiTrans *bigSidePlate_transf[1];

    bigSidePlate_transf[0] = new TGeoCombiTrans();
    bigSidePlate_transf[0]->SetDx(-(backFiberGlassCover_XSize - frontFiberGlassCover_XSize*0.5) - bigSidePlate_XSize*0.5 + 3.0/*shift*/);
    bigSidePlate_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + bigSidePlate_YSize*0.5);
    bigSidePlate_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - bigSidePlate_ZSize*0.5);

    frames->AddNode(bigSidePlateV, 0, bigSidePlate_transf[0]);
    //--------------------------------------------------------------------------

    //big side support ---------------------------------------------------------
    Double_t bigSideSupport_XSize = 3.2; //cm
    Double_t bigSideSupport_YSize = 26; //cm
    Double_t bigSideSupport_ZSize = 0.6; //cm

    TGeoShape *bigSideSupportS = new TGeoBBox(TString("bigSideSupportS")+=TString("_") + frames->GetName(), bigSideSupport_XSize*0.5, bigSideSupport_YSize*0.5, bigSideSupport_ZSize*0.5);

    TGeoVolume *bigSideSupportV = new TGeoVolume(TString("bigSideSupportV")+=TString("_") + frames->GetName(), bigSideSupportS);

    //volume medium
    TGeoMedium *bigSideSupportV_medium = pMedAluminium;
    if(bigSideSupportV_medium) {
        bigSideSupportV->SetMedium(bigSideSupportV_medium);
    }
    else Fatal("Main", "Invalid medium for bigSideSupportV!");

    //volume visual property (transparency)
    bigSideSupportV->SetLineColor(TColor::GetColor("#ffcccc"));
    bigSideSupportV->SetTransparency(0);

    TGeoCombiTrans *bigSideSupport_transf[1];

    bigSideSupport_transf[0] = new TGeoCombiTrans();
    bigSideSupport_transf[0]->SetDx(-(backFiberGlassCover_XSize - frontFiberGlassCover_XSize*0.5) - bigSideSupport_XSize*0.5);
    bigSideSupport_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + bigSideSupport_YSize*0.5 + 1.6/*shift*/);
    bigSideSupport_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - bigSidePlate_ZSize - bigSideSupport_ZSize*0.5);

    frames->AddNode(bigSideSupportV, 0, bigSideSupport_transf[0]);
    //--------------------------------------------------------------------------

    //small side plate -----------------------------------------------------------
    Double_t smallSidePlate_XSize = 4.2; //cm
    Double_t smallSidePlate_YSize = 3.7; //cm
    Double_t smallSidePlate_ZSize = 0.3; //cm

    TGeoShape *smallSidePlateS = new TGeoBBox(TString("smallSidePlateS")+=TString("_") + frames->GetName(), smallSidePlate_XSize*0.5, smallSidePlate_YSize*0.5, smallSidePlate_ZSize*0.5);

    TGeoVolume *smallSidePlateV = new TGeoVolume(TString("smallSidePlateV")+=TString("_") + frames->GetName(), smallSidePlateS);

    //volume medium
    TGeoMedium *smallSidePlateV_medium = pMedAluminium;
    if(smallSidePlateV_medium) {
        smallSidePlateV->SetMedium(smallSidePlateV_medium);
    }
    else Fatal("Main", "Invalid medium for smallSidePlateV!");

    //volume visual property (transparency)
    smallSidePlateV->SetLineColor(TColor::GetColor("#ffcccc"));
    smallSidePlateV->SetTransparency(0);

    TGeoCombiTrans *smallSidePlate_transf[1];

    smallSidePlate_transf[0] = new TGeoCombiTrans();
    smallSidePlate_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - smallSidePlate_XSize*0.5 + 2.2/*shift*/);
    smallSidePlate_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + smallSidePlate_YSize*0.5 + 0.2/*shift*/);
    smallSidePlate_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - smallSidePlate_ZSize*0.5);

    frames->AddNode(smallSidePlateV, 0, smallSidePlate_transf[0]);
    //--------------------------------------------------------------------------

    //small side support ---------------------------------------------------------
    Double_t smallSideSupport_XSize = 4.0; //cm
    Double_t smallSideSupport_YSize = 2.4; //cm
    Double_t smallSideSupport_ZSize = 0.6; //cm

    TGeoShape *smallSideSupportS = new TGeoBBox(TString("smallSideSupportS")+=TString("_") + frames->GetName(), smallSideSupport_XSize*0.5, smallSideSupport_YSize*0.5, smallSideSupport_ZSize*0.5);

    TGeoVolume *smallSideSupportV = new TGeoVolume(TString("smallSideSupportV")+=TString("_") + frames->GetName(), smallSideSupportS);

    //volume medium
    TGeoMedium *smallSideSupportV_medium = pMedAluminium;
    if(smallSideSupportV_medium) {
        smallSideSupportV->SetMedium(smallSideSupportV_medium);
    }
    else Fatal("Main", "Invalid medium for smallSideSupportV!");

    //volume visual property (transparency)
    smallSideSupportV->SetLineColor(TColor::GetColor("#ffcccc"));
    smallSideSupportV->SetTransparency(0);

    TGeoCombiTrans *smallSideSupport_transf[1];

    smallSideSupport_transf[0] = new TGeoCombiTrans();
    smallSideSupport_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - smallSideSupport_XSize*0.5 + 3.3/*shift*/);
    smallSideSupport_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + smallSideSupport_YSize*0.5);
    smallSideSupport_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - bigSidePlate_ZSize - bigSideSupport_ZSize*0.5);

    frames->AddNode(smallSideSupportV, 0, smallSideSupport_transf[0]);
    //--------------------------------------------------------------------------

    //side amplifier block -----------------------------------------------------
    Double_t sideAmplifierBlock_XSize = 0.3; //cm
    Double_t sideAmplifierBlock_YSize = 24.825; //cm
    Double_t sideAmplifierBlock_ZSize = 8.41; //cm

    TGeoShape *sideAmplifierBlockS = new TGeoBBox(TString("sideAmplifierBlockS")+=TString("_") + frames->GetName(), sideAmplifierBlock_XSize*0.5, sideAmplifierBlock_YSize*0.5, sideAmplifierBlock_ZSize*0.5);

    TGeoVolume *sideAmplifierBlockV = new TGeoVolume(TString("sideAmplifierBlockV")+=TString("_") + frames->GetName(), sideAmplifierBlockS);

    //volume medium
    TGeoMedium *sideAmplifierBlockV_medium = pMedFiberGlass;
    if(sideAmplifierBlockV_medium) {
        sideAmplifierBlockV->SetMedium(sideAmplifierBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for sideAmplifierBlockV!");

    //volume visual property (transparency)
    sideAmplifierBlockV->SetLineColor(TColor::GetColor("#99ff99"));
    sideAmplifierBlockV->SetTransparency(0);

    TGeoCombiTrans *sideAmplifierBlock_transf[1];

    sideAmplifierBlock_transf[0] = new TGeoCombiTrans();
    sideAmplifierBlock_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - sideAmplifierBlock_XSize*0.5 - 1.6/*shift*/);
    sideAmplifierBlock_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideAmplifierBlock_YSize*0.5 + 1.575/*shift*/);
    sideAmplifierBlock_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideAmplifierBlock_ZSize*0.5);

    frames->AddNode(sideAmplifierBlockV, 0, sideAmplifierBlock_transf[0]);
    //--------------------------------------------------------------------------

    //upper amplifier block ----------------------------------------------------
    Double_t upperAmplifierBlockOutPart_XSize = 65.6; //cm
    Double_t upperAmplifierBlockOutPart_YSize = 0.3; //cm
    Double_t upperAmplifierBlockOutPart_ZSize = 8.41; //cm

    Double_t upperAmplifierBlockInLeftPart_XSize = 29.3; //cm
    Double_t upperAmplifierBlockInLeftPart_YSize = 0.3; //cm
    Double_t upperAmplifierBlockInLeftPart_ZSize = 8.41; //cm

    Double_t upperAmplifierBlockInRightPart_XSize = 26.0; //cm
    Double_t upperAmplifierBlockInRightPart_YSize = 0.3; //cm
    Double_t upperAmplifierBlockInRightPart_ZSize = 8.41; //cm

    TGeoShape *upperAmplifierBlockOutPartS = new TGeoBBox(TString("upperAmplifierBlockOutPartS")+=TString("_") + frames->GetName(), upperAmplifierBlockOutPart_XSize*0.5, upperAmplifierBlockOutPart_YSize*0.5, upperAmplifierBlockOutPart_ZSize*0.5);
    TGeoShape *upperAmplifierBlockInLeftPartS = new TGeoBBox(TString("upperAmplifierBlockInLeftPartS")+=TString("_") + frames->GetName(), upperAmplifierBlockInLeftPart_XSize*0.5, upperAmplifierBlockInLeftPart_YSize*0.5, upperAmplifierBlockInLeftPart_ZSize*0.5);
    TGeoShape *upperAmplifierBlockInRightPartS = new TGeoBBox(TString("upperAmplifierBlockInRightPartS")+=TString("_") + frames->GetName(), upperAmplifierBlockInRightPart_XSize*0.5, upperAmplifierBlockInRightPart_YSize*0.5, upperAmplifierBlockInRightPart_ZSize*0.5);

    TGeoVolume *upperAmplifierBlockOutPartV = new TGeoVolume(TString("upperAmplifierBlockOutPartV")+=TString("_") + frames->GetName(), upperAmplifierBlockOutPartS);
    TGeoVolume *upperAmplifierBlockInLeftPartV = new TGeoVolume(TString("upperAmplifierBlockInLeftPartV")+=TString("_") + frames->GetName(), upperAmplifierBlockInLeftPartS);
    TGeoVolume *upperAmplifierBlockInRightPartV = new TGeoVolume(TString("upperAmplifierBlockInRightPartV")+=TString("_") + frames->GetName(), upperAmplifierBlockInRightPartS);

    //volume medium
    TGeoMedium *upperAmplifierBlockOutPartV_medium = pMedFiberGlass;
    if(upperAmplifierBlockOutPartV_medium) {
        upperAmplifierBlockOutPartV->SetMedium(upperAmplifierBlockOutPartV_medium);
        upperAmplifierBlockInLeftPartV->SetMedium(upperAmplifierBlockOutPartV_medium);
        upperAmplifierBlockInRightPartV->SetMedium(upperAmplifierBlockOutPartV_medium);
    }
    else Fatal("Main", "Invalid medium for upperAmplifierBlock!");

    //volume visual property (transparency)
    upperAmplifierBlockOutPartV->SetLineColor(TColor::GetColor("#99ff99"));
    upperAmplifierBlockOutPartV->SetTransparency(0);
    upperAmplifierBlockInLeftPartV->SetLineColor(TColor::GetColor("#99ff99"));
    upperAmplifierBlockInLeftPartV->SetTransparency(0);
    upperAmplifierBlockInRightPartV->SetLineColor(TColor::GetColor("#99ff99"));
    upperAmplifierBlockInRightPartV->SetTransparency(0);

    TGeoCombiTrans *upperAmplifierBlock_transf[3];

    upperAmplifierBlock_transf[0] = new TGeoCombiTrans();
    upperAmplifierBlock_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - upperAmplifierBlockOutPart_XSize*0.5 - 2.0/*shift*/);
    upperAmplifierBlock_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + upperAmplifierBlockOutPart_YSize*0.5 + 2.9/*shift*/);
    upperAmplifierBlock_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperAmplifierBlockOutPart_ZSize*0.5);

    upperAmplifierBlock_transf[1] = new TGeoCombiTrans();
    upperAmplifierBlock_transf[1]->SetDx(frontFiberGlassCover_XSize*0.5 - upperAmplifierBlockInLeftPart_XSize*0.5 - 6.8/*shift*/);
    upperAmplifierBlock_transf[1]->SetDy(frontFiberGlassCover_YSize*0.5 + upperAmplifierBlockInLeftPart_YSize*0.5 + 1.4/*shift*/);
    upperAmplifierBlock_transf[1]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperAmplifierBlockInLeftPart_ZSize*0.5);

    upperAmplifierBlock_transf[2] = new TGeoCombiTrans();
    upperAmplifierBlock_transf[2]->SetDx(frontFiberGlassCover_XSize*0.5 - upperAmplifierBlockInLeftPart_XSize - upperAmplifierBlockInRightPart_XSize*0.5 - 7.6/*shift*/);
    upperAmplifierBlock_transf[2]->SetDy(frontFiberGlassCover_YSize*0.5 + upperAmplifierBlockInRightPart_YSize*0.5 + 1.4/*shift*/);
    upperAmplifierBlock_transf[2]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperAmplifierBlockInRightPart_ZSize*0.5);

    frames->AddNode(upperAmplifierBlockOutPartV, 0, upperAmplifierBlock_transf[0]);
    frames->AddNode(upperAmplifierBlockInLeftPartV, 0, upperAmplifierBlock_transf[1]);
    frames->AddNode(upperAmplifierBlockInRightPartV, 0, upperAmplifierBlock_transf[2]);
    //--------------------------------------------------------------------------

    //side PCB -----------------------------------------------------------------
    Double_t sidePCB_XSize = 0.15; //cm
    Double_t sidePCB_YSize = 35.05; //cm
    Double_t sidePCB_ZSize = 4.0; //cm

    TGeoShape *sidePCBS = new TGeoBBox(TString("sidePCBS")+=TString("_") + frames->GetName(), sidePCB_XSize*0.5, sidePCB_YSize*0.5, sidePCB_ZSize*0.5);

    TGeoVolume *sidePCBV = new TGeoVolume(TString("sidePCBV")+=TString("_") + frames->GetName(), sidePCBS);

    //volume medium
    TGeoMedium *sidePCBV_medium = pMedFiberGlass;
    if(sidePCBV_medium) {
        sidePCBV->SetMedium(sidePCBV_medium);
    }
    else Fatal("Main", "Invalid medium for sidePCBV!");

    //volume visual property (transparency)
    sidePCBV->SetLineColor(TColor::GetColor("#55ee55"));
    sidePCBV->SetTransparency(0);

    TGeoCombiTrans *sidePCB_transf[1];

    sidePCB_transf[0] = new TGeoCombiTrans();
    sidePCB_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - sidePCB_XSize*0.5 - 0.45/*shift*/);
    sidePCB_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sidePCB_YSize*0.5 + 2.5/*shift*/);
    sidePCB_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sidePCB_ZSize*0.5 - 4.4/*shift*/);

    frames->AddNode(sidePCBV, 0, sidePCB_transf[0]);
    //--------------------------------------------------------------------------

    //upper PCB ----------------------------------------------------------------
    Double_t upperPCB_XSize = 70.1; //cm
    Double_t upperPCB_YSize = 0.15; //cm
    Double_t upperPCB_ZSize = 4.0; //cm

    TGeoShape *upperPCBS = new TGeoBBox(TString("upperPCBS")+=TString("_") + frames->GetName(), upperPCB_XSize*0.5, upperPCB_YSize*0.5, upperPCB_ZSize*0.5);

    TGeoVolume *upperPCBV = new TGeoVolume(TString("upperPCBV")+=TString("_") + frames->GetName(), upperPCBS);

    //volume medium
    TGeoMedium *upperPCBV_medium = pMedFiberGlass;
    if(upperPCBV_medium) {
        upperPCBV->SetMedium(upperPCBV_medium);
    }
    else Fatal("Main", "Invalid medium for upperPCBV!");

    //volume visual property (transparency)
    upperPCBV->SetLineColor(TColor::GetColor("#55ee55"));
    upperPCBV->SetTransparency(0);

    TGeoCombiTrans *upperPCB_transf[1];

    upperPCB_transf[0] = new TGeoCombiTrans();
    upperPCB_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - upperPCB_XSize*0.5 - 1.7/*shift*/);
    upperPCB_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + upperPCB_YSize*0.5 + 0.1/*shift*/);
    upperPCB_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperPCB_ZSize*0.5 - 4.4/*shift*/);

    frames->AddNode(upperPCBV, 0, upperPCB_transf[0]);
    //--------------------------------------------------------------------------

    //side bottom leg ----------------------------------------------------------
    Double_t sideBottomSmallLeg_XSize = 2.0; //cm
    Double_t sideBottomSmallLeg_YSize = 0.46; //cm
    Double_t sideBottomSmallLeg_ZSize = 1.9; //cm

    Double_t sideBottomBigLeg_XSize = 5.4; //cm
    Double_t sideBottomBigLeg_YSize = 0.46; //cm
    Double_t sideBottomBigLeg_ZSize = 3.2; //cm

    TGeoShape *sideBottomSmallLegS = new TGeoBBox(TString("sideBottomSmallLegS")+=TString("_") + frames->GetName(), sideBottomSmallLeg_XSize*0.5, sideBottomSmallLeg_YSize*0.5, sideBottomSmallLeg_ZSize*0.5);
    TGeoShape *sideBottomBigLegS = new TGeoBBox(TString("sideBottomBigLegS")+=TString("_") + frames->GetName(), sideBottomBigLeg_XSize*0.5, sideBottomBigLeg_YSize*0.5, sideBottomBigLeg_ZSize*0.5);

    TGeoVolume *sideBottomSmallLegV = new TGeoVolume(TString("sideBottomSmallLegV")+=TString("_") + frames->GetName(), sideBottomSmallLegS);
    TGeoVolume *sideBottomBigLegV = new TGeoVolume(TString("sideBottomBigLegV")+=TString("_") + frames->GetName(), sideBottomBigLegS);

    //volume medium
    TGeoMedium *sideBottomLeg_medium = pMedAluminium;
    if(sideBottomLeg_medium) {
        sideBottomSmallLegV->SetMedium(sideBottomLeg_medium);
        sideBottomBigLegV->SetMedium(sideBottomLeg_medium);
    }
    else Fatal("Main", "Invalid medium for sideBottomLegV!");

    //volume visual property (transparency)
    sideBottomSmallLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideBottomSmallLegV->SetTransparency(0);
    sideBottomBigLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideBottomBigLegV->SetTransparency(0);

    TGeoCombiTrans *sideBottomSmallLeg_transf[1];

    sideBottomSmallLeg_transf[0] = new TGeoCombiTrans();
    sideBottomSmallLeg_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - sideBottomSmallLeg_XSize*0.5 - 2.9/*shift*/);
    sideBottomSmallLeg_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideBottomSmallLeg_YSize*0.5 + 1.05/*shift*/);
    sideBottomSmallLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideBottomSmallLeg_ZSize*0.5 - bigSidePlate_ZSize);

    TGeoCombiTrans *sideBottomBigLeg_transf[1];

    sideBottomBigLeg_transf[0] = new TGeoCombiTrans();
    sideBottomBigLeg_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 + sideBottomBigLeg_XSize*0.5 - sideBottomSmallLeg_XSize - 2.9/*shift*/);
    sideBottomBigLeg_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideBottomBigLeg_YSize*0.5 + 1.05/*shift*/);
    sideBottomBigLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideBottomBigLeg_ZSize*0.5 - bigSidePlate_ZSize - sideBottomSmallLeg_ZSize);

    frames->AddNode(sideBottomSmallLegV, 0, sideBottomSmallLeg_transf[0]);
    frames->AddNode(sideBottomBigLegV, 0, sideBottomBigLeg_transf[0]);
    //--------------------------------------------------------------------------

    //side top leg -------------------------------------------------------------
    Double_t sideTopSmallLeg_XSize = 2.0; //cm
    Double_t sideTopSmallLeg_YSize = 0.56; //cm
    Double_t sideTopSmallLeg_ZSize = 2.24; //cm

    Double_t sideTopBigLeg_XSize = 4.3; //cm
    Double_t sideTopBigLeg_YSize = 0.56; //cm
    Double_t sideTopBigLeg_ZSize = 2.8; //cm

    TGeoShape *sideTopSmallLegS = new TGeoBBox(TString("sideTopSmallLegS")+=TString("_") + frames->GetName(), sideTopSmallLeg_XSize*0.5, sideTopSmallLeg_YSize*0.5, sideTopSmallLeg_ZSize*0.5);
    TGeoShape *sideTopBigLegS = new TGeoBBox(TString("sideTopBigLegS")+=TString("_") + frames->GetName(), sideTopBigLeg_XSize*0.5, sideTopBigLeg_YSize*0.5, sideTopBigLeg_ZSize*0.5);

    TGeoVolume *sideTopSmallLegV = new TGeoVolume(TString("sideTopSmallLegV")+=TString("_") + frames->GetName(), sideTopSmallLegS);
    TGeoVolume *sideTopBigLegV = new TGeoVolume(TString("sideTopBigLegV")+=TString("_") + frames->GetName(), sideTopBigLegS);

    //volume medium
    TGeoMedium *sideTopLeg_medium = pMedAluminium;
    if(sideTopLeg_medium) {
        sideTopSmallLegV->SetMedium(sideTopLeg_medium);
        sideTopBigLegV->SetMedium(sideTopLeg_medium);
    }
    else Fatal("Main", "Invalid medium for sideTopLegV!");

    //volume visual property (transparency)
    sideTopSmallLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideTopSmallLegV->SetTransparency(0);
    sideTopBigLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideTopBigLegV->SetTransparency(0);

    TGeoCombiTrans *sideTopSmallLeg_transf[1];

    sideTopSmallLeg_transf[0] = new TGeoCombiTrans();
    sideTopSmallLeg_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - sideTopSmallLeg_XSize*0.5 - 2.9/*shift*/);
    sideTopSmallLeg_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideTopSmallLeg_YSize*0.5 + 27.23/*shift*/);
    sideTopSmallLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideTopSmallLeg_ZSize*0.5 - bigSidePlate_ZSize);

    TGeoCombiTrans *sideTopBigLeg_transf[1];

    sideTopBigLeg_transf[0] = new TGeoCombiTrans();
    sideTopBigLeg_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 + sideTopBigLeg_XSize*0.5 - sideTopSmallLeg_XSize - 2.9/*shift*/);
    sideTopBigLeg_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideTopBigLeg_YSize*0.5 + 27.23/*shift*/);
    sideTopBigLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideTopBigLeg_ZSize*0.5 - bigSidePlate_ZSize - sideTopSmallLeg_ZSize);

    frames->AddNode(sideTopSmallLegV, 0, sideTopSmallLeg_transf[0]);
    frames->AddNode(sideTopBigLegV, 0, sideTopBigLeg_transf[0]);
    //--------------------------------------------------------------------------

    //side stick ---------------------------------------------------------------
    Double_t sideLongStick_XSize = 0.6; //cm
    Double_t sideLongStick_YSize = 25.72; //cm
    Double_t sideLongStick_ZSize = 1.2; //cm

    Double_t sideShortStick_XSize = 0.6; //cm
    Double_t sideShortStick_YSize = 9.6; //cm
    Double_t sideShortStick_ZSize = 1.2; //cm

    TGeoShape *sideLongStickS = new TGeoBBox(TString("sideLongStickS")+=TString("_") + frames->GetName(), sideLongStick_XSize*0.5, sideLongStick_YSize*0.5, sideLongStick_ZSize*0.5);
    TGeoShape *sideShortStickS = new TGeoBBox(TString("sideShortStickS")+=TString("_") + frames->GetName(), sideShortStick_XSize*0.5, sideShortStick_YSize*0.5, sideShortStick_ZSize*0.5);

    TGeoVolume *sideLongStickV = new TGeoVolume(TString("sideLongStickV")+=TString("_") + frames->GetName(), sideLongStickS);
    TGeoVolume *sideShortStickV = new TGeoVolume(TString("sideShortStickV")+=TString("_") + frames->GetName(), sideShortStickS);

    //volume medium
    TGeoMedium *sideStick_medium = pMedAluminium;
    if(sideStick_medium) {
        sideLongStickV->SetMedium(sideStick_medium);
        sideShortStickV->SetMedium(sideStick_medium);
    }
    else Fatal("Main", "Invalid medium for sideTopLegV!");

    //volume visual property (transparency)
    sideLongStickV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideLongStickV->SetTransparency(0);
    sideShortStickV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideShortStickV->SetTransparency(0);

    TGeoCombiTrans *sideLongStick_transf[1];

    sideLongStick_transf[0] = new TGeoCombiTrans();
    sideLongStick_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - 0.9/*shift*/);
    sideLongStick_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideLongStick_YSize*0.5 + 1.51/*shift*/);
    sideLongStick_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideLongStick_ZSize*0.5 - bigSidePlate_ZSize - sideTopSmallLeg_ZSize - sideTopBigLeg_ZSize + 0.04/*shift*/);

    TGeoCombiTrans *sideShortStick_transf[1];

    sideShortStick_transf[0] = new TGeoCombiTrans();
    sideShortStick_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - 0.9/*shift*/);
    sideShortStick_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideShortStick_YSize*0.5 + sideLongStick_YSize + 2.07/*shift*/);
    sideShortStick_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideShortStick_ZSize*0.5 - bigSidePlate_ZSize - sideTopSmallLeg_ZSize - sideTopBigLeg_ZSize + 0.04/*shift*/);

    frames->AddNode(sideLongStickV, 0, sideLongStick_transf[0]);
    frames->AddNode(sideShortStickV, 0, sideShortStick_transf[0]);
    //--------------------------------------------------------------------------

    //side funny box -----------------------------------------------------------
    Double_t sideFunnyBox_XSize = 0.8; //cm
    Double_t sideFunnyBox_YSize = 1.0; //cm
    Double_t sideFunnyBox_ZSize = 1.5; //cm

    TGeoShape *sideFunnyBoxS = new TGeoBBox(TString("sideFunnyBoxS")+=TString("_") + frames->GetName(), sideFunnyBox_XSize*0.5, sideFunnyBox_YSize*0.5, sideFunnyBox_ZSize*0.5);

    TGeoVolume *sideFunnyBoxV = new TGeoVolume(TString("sideFunnyBoxV")+=TString("_") + frames->GetName(), sideFunnyBoxS);

    //volume medium
    TGeoMedium *sideFunnyBoxV_medium = pMedAluminium;
    if(sideFunnyBoxV_medium) {
        sideFunnyBoxV->SetMedium(sideFunnyBoxV_medium);
    }
    else Fatal("Main", "Invalid medium for sideFunnyBoxV!");

    //volume visual property (transparency)
    sideFunnyBoxV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideFunnyBoxV->SetTransparency(0);

    TGeoCombiTrans *sideFunnyBox_transf[1];

    sideFunnyBox_transf[0] = new TGeoCombiTrans();
    sideFunnyBox_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5);
    sideFunnyBox_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideFunnyBox_YSize*0.5 + sideBottomBigLeg_YSize + 1.05/*shift*/);
    sideFunnyBox_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideShortStick_ZSize*0.5 - bigSidePlate_ZSize - sideBottomSmallLeg_ZSize - sideBottomBigLeg_ZSize);

    frames->AddNode(sideFunnyBoxV, 0, sideFunnyBox_transf[0]);
    //--------------------------------------------------------------------------

    //upper platform -----------------------------------------------------------
    Double_t upperPlatform_XSize = 2.2; //cm
    Double_t upperPlatform_YSize = 3.7; //cm
    Double_t upperPlatform_ZSize = 0.2; //cm

    TGeoShape *upperPlatformS = new TGeoBBox(TString("upperPlatformS")+=TString("_") + frames->GetName(), upperPlatform_XSize*0.5, upperPlatform_YSize*0.5, upperPlatform_ZSize*0.5);

    TGeoVolume *upperPlatformV = new TGeoVolume(TString("upperPlatformV")+=TString("_") + frames->GetName(), upperPlatformS);

    //volume medium
    TGeoMedium *upperPlatformV_medium = pMedAluminium;
    if(upperPlatformV_medium) {
        upperPlatformV->SetMedium(upperPlatformV_medium);
    }
    else Fatal("Main", "Invalid medium for upperPlatformV!");

    //volume visual property (transparency)
    upperPlatformV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperPlatformV->SetTransparency(0);

    TGeoCombiTrans *upperPlatform_transf[1];

    upperPlatform_transf[0] = new TGeoCombiTrans();
    upperPlatform_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - upperPlatform_XSize*0.5 - 0.8/*shift*/);
    upperPlatform_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + upperPlatform_YSize*0.5 + 0.2/*shift*/);
    upperPlatform_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperPlatform_ZSize*0.5);

    frames->AddNode(upperPlatformV, 0, upperPlatform_transf[0]);
    //--------------------------------------------------------------------------

    //front left leg -----------------------------------------------------------
    Double_t frontLeftLeg_XSize = 0.53; //cm
    Double_t frontLeftLeg_YSize = 2.5; //cm
    Double_t frontLeftLeg_ZSize = 7.4; //cm

    TGeoShape *frontLeftLegS = new TGeoBBox(TString("frontLeftLegS")+=TString("_") + frames->GetName(), frontLeftLeg_XSize*0.5, frontLeftLeg_YSize*0.5, frontLeftLeg_ZSize*0.5);

    TGeoVolume *frontLeftLegV = new TGeoVolume(TString("frontLeftLegV")+=TString("_") + frames->GetName(), frontLeftLegS);

    //volume medium
    TGeoMedium *frontLeftLegV_medium = pMedAluminium;
    if(frontLeftLegV_medium) {
        frontLeftLegV->SetMedium(frontLeftLegV_medium);
    }
    else Fatal("Main", "Invalid medium for frontLeftLegV!");

    //volume visual property (transparency)
    frontLeftLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontLeftLegV->SetTransparency(0);

    TGeoCombiTrans *frontLeftLeg_transf[1];

    frontLeftLeg_transf[0] = new TGeoCombiTrans();
    frontLeftLeg_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - frontLeftLeg_XSize*0.5 - 1.97/*shift*/);
    frontLeftLeg_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + frontLeftLeg_YSize*0.5 + 0.4/*shift*/);
    frontLeftLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperPlatform_ZSize - frontLeftLeg_ZSize*0.5);

    frames->AddNode(frontLeftLegV, 0, frontLeftLeg_transf[0]);
    //--------------------------------------------------------------------------

    //front right leg ----------------------------------------------------------
    Double_t frontRightLeg_XSize = 0.55; //cm
    Double_t frontRightLeg_YSize = 2.5; //cm
    Double_t frontRightLeg_ZSize = 6.4; //cm

    TGeoShape *frontRightLegS = new TGeoBBox(TString("frontRightLegS")+=TString("_") + frames->GetName(), frontRightLeg_XSize*0.5, frontRightLeg_YSize*0.5, frontRightLeg_ZSize*0.5);

    TGeoVolume *frontRightLegV = new TGeoVolume(TString("frontRightLegV")+=TString("_") + frames->GetName(), frontRightLegS);

    //volume medium
    TGeoMedium *frontRightLegV_medium = pMedAluminium;
    if(frontRightLegV_medium) {
        frontRightLegV->SetMedium(frontRightLegV_medium);
    }
    else Fatal("Main", "Invalid medium for frontRightLegV!");

    //volume visual property (transparency)
    frontRightLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontRightLegV->SetTransparency(0);

    TGeoCombiTrans *frontRightLeg_transf[1];

    frontRightLeg_transf[0] = new TGeoCombiTrans();
    frontRightLeg_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - frontRightLeg_XSize*0.5 - 1.82/*shift*/);
    frontRightLeg_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + frontRightLeg_YSize*0.5 + 0.4/*shift*/);
    frontRightLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - smallSidePlate_ZSize - frontRightLeg_ZSize*0.5);

    frames->AddNode(frontRightLegV, 0, frontRightLeg_transf[0]);
    //--------------------------------------------------------------------------

    //front center leg ---------------------------------------------------------
    Double_t frontCenterLowLeg_XSize = 1.3; //cm
    Double_t frontCenterLowLeg_YSize = 0.54; //cm
    Double_t frontCenterLowLeg_ZSize = 6.875; //cm

    Double_t frontCenterUpLeg_XSize = 0.75; //cm
    Double_t frontCenterUpLeg_YSize = 1.9; //cm
    Double_t frontCenterUpLeg_ZSize = 6.875; //cm

    TGeoShape *frontCenterLowLegS = new TGeoBBox(TString("frontCenterLowLegS")+=TString("_") + frames->GetName(), frontCenterLowLeg_XSize*0.5, frontCenterLowLeg_YSize*0.5, frontCenterLowLeg_ZSize*0.5);
    TGeoShape *frontCenterUpLegS = new TGeoBBox(TString("frontCenterUpLegS")+=TString("_") + frames->GetName(), frontCenterUpLeg_XSize*0.5, frontCenterUpLeg_YSize*0.5, frontCenterUpLeg_ZSize*0.5);

    TGeoVolume *frontCenterLowLegV = new TGeoVolume(TString("frontCenterLowLegV")+=TString("_") + frames->GetName(), frontCenterLowLegS);
    TGeoVolume *frontCenterUpLegV = new TGeoVolume(TString("frontCenterUpLegV")+=TString("_") + frames->GetName(), frontCenterUpLegS);

    //volume medium
    TGeoMedium *frontCenterLeg_medium = pMedAluminium;
    if(frontCenterLeg_medium) {
        frontCenterLowLegV->SetMedium(frontCenterLeg_medium);
        frontCenterUpLegV->SetMedium(frontCenterLeg_medium);
    }
    else Fatal("Main", "Invalid medium for frontCenterLeg!");

    //volume visual property (transparency)
    frontCenterLowLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontCenterLowLegV->SetTransparency(0);
    frontCenterUpLegV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontCenterUpLegV->SetTransparency(0);

    TGeoCombiTrans *frontCenterLowLeg_transf[1];

    frontCenterLowLeg_transf[0] = new TGeoCombiTrans();
    frontCenterLowLeg_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - frontCenterLowLeg_XSize*0.5 - 35.84/*shift*/);
    frontCenterLowLeg_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + frontCenterLowLeg_YSize*0.5 + 0.46/*shift*/);
    frontCenterLowLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - frontCenterLowLeg_ZSize*0.5);

    TGeoCombiTrans *frontCenterUpLeg_transf[1];

    frontCenterUpLeg_transf[0] = new TGeoCombiTrans();
    frontCenterUpLeg_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - frontCenterUpLeg_XSize*0.5 - 36.1/*shift*/);
    frontCenterUpLeg_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + frontCenterUpLeg_YSize*0.5 + frontCenterLowLeg_YSize + 0.46/*shift*/);
    frontCenterUpLeg_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - frontCenterUpLeg_ZSize*0.5);

    frames->AddNode(frontCenterLowLegV, 0, frontCenterLowLeg_transf[0]);
    frames->AddNode(frontCenterUpLegV, 0, frontCenterUpLeg_transf[0]);
    //--------------------------------------------------------------------------

    //upper plank --------------------------------------------------------------
    Double_t upperRightPlank_XSize = 34.49; //cm
    Double_t upperRightPlank_YSize = 0.6; //cm
    Double_t upperRightPlank_ZSize = 1.2; //cm

    Double_t upperLeftPlank_XSize = 33.73; //cm
    Double_t upperLeftPlank_YSize = 0.6; //cm
    Double_t upperLeftPlank_ZSize = 1.2; //cm

    TGeoShape *upperRightPlankS = new TGeoBBox(TString("upperRightPlankS")+=TString("_") + frames->GetName(), upperRightPlank_XSize*0.5, upperRightPlank_YSize*0.5, upperRightPlank_ZSize*0.5);
    TGeoShape *upperLeftPlankS = new TGeoBBox(TString("upperLeftPlankS")+=TString("_") + frames->GetName(), upperLeftPlank_XSize*0.5, upperLeftPlank_YSize*0.5, upperLeftPlank_ZSize*0.5);

    TGeoVolume *upperRightPlankV = new TGeoVolume(TString("upperRightPlankV")+=TString("_") + frames->GetName(), upperRightPlankS);
    TGeoVolume *upperLeftPlankV = new TGeoVolume(TString("upperLeftPlankV")+=TString("_") + frames->GetName(), upperLeftPlankS);

    //volume medium
    TGeoMedium *upperPlank_medium = pMedAluminium;
    if(upperPlank_medium) {
        upperRightPlankV->SetMedium(upperPlank_medium);
        upperLeftPlankV->SetMedium(upperPlank_medium);
    }
    else Fatal("Main", "Invalid medium for upperPlankV!");

    //volume visual property (transparency)
    upperRightPlankV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperRightPlankV->SetTransparency(0);
    upperLeftPlankV->SetLineColor(TColor::GetColor("#ffcccc"));
    upperLeftPlankV->SetTransparency(0);

    TGeoCombiTrans *upperRightPlank_transf[1];

    upperRightPlank_transf[0] = new TGeoCombiTrans();
    upperRightPlank_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - upperRightPlank_XSize*0.5 - 36.88/*shift*/);
    upperRightPlank_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + upperRightPlank_YSize*0.5 + 2.291/*shift*/);
    upperRightPlank_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperRightPlank_ZSize*0.5 - 5.23/*shift*/);

    TGeoCombiTrans *upperLeftPlank_transf[1];

    upperLeftPlank_transf[0] = new TGeoCombiTrans();
    upperLeftPlank_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - upperLeftPlank_XSize*0.5 - 2.37/*shift*/);
    upperLeftPlank_transf[0]->SetDy(frontFiberGlassCover_YSize*0.5 + upperLeftPlank_YSize*0.5 + 2.291/*shift*/);
    upperLeftPlank_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - upperLeftPlank_ZSize*0.5 - 5.23/*shift*/);

    frames->AddNode(upperRightPlankV, 0, upperRightPlank_transf[0]);
    frames->AddNode(upperLeftPlankV, 0, upperLeftPlank_transf[0]);
    //--------------------------------------------------------------------------

    //side channel bar ---------------------------------------------------------
    Double_t sideChannelBar_XSize = 1.5; //cm
    Double_t sideChannelBar_YSize = 40.0; //cm
    Double_t sideChannelBar_ZSize = 0.38; //cm

    TGeoShape *sideChannelBarS = new TGeoBBox(TString("sideChannelBarS")+=TString("_") + frames->GetName(), sideChannelBar_XSize*0.5, sideChannelBar_YSize*0.5, sideChannelBar_ZSize*0.5);

    TGeoVolume *sideChannelBarV = new TGeoVolume(TString("sideChannelBarV")+=TString("_") + frames->GetName(), sideChannelBarS);

    //volume medium
    TGeoMedium *sideChannelBarV_medium = pMedAluminium;
    if(sideChannelBarV_medium) {
        sideChannelBarV->SetMedium(sideChannelBarV_medium);
    }
    else Fatal("Main", "Invalid medium for sideChannelBarV!");

    //volume visual property (transparency)
    sideChannelBarV->SetLineColor(TColor::GetColor("#ffcccc"));
    sideChannelBarV->SetTransparency(0);

    TGeoCombiTrans *sideChannelBar_transf[1];

    sideChannelBar_transf[0] = new TGeoCombiTrans();
    sideChannelBar_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - sideChannelBar_XSize*0.5 + 2.0/*shift*/);
    sideChannelBar_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + sideChannelBar_YSize*0.5 + 1.05/*shift*/);
    sideChannelBar_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - sideChannelBar_ZSize*0.5 - 2.14/*shift*/);

    frames->AddNode(sideChannelBarV, 0, sideChannelBar_transf[0]);
    //--------------------------------------------------------------------------

    //front funny channel bar --------------------------------------------------
    Double_t frontFunnyChannelBar_XSize = 2.82; //cm
    Double_t frontFunnyChannelBar_YSize = 2.5; //cm
    Double_t frontFunnyChannelBar_ZSize = 0.47; //cm

    TGeoShape *frontFunnyChannelBarS = new TGeoBBox(TString("frontFunnyChannelBarS")+=TString("_") + frames->GetName(), frontFunnyChannelBar_XSize*0.5, frontFunnyChannelBar_YSize*0.5, frontFunnyChannelBar_ZSize*0.5);

    TGeoVolume *frontFunnyChannelBarV = new TGeoVolume(TString("frontFunnyChannelBarV")+=TString("_") + frames->GetName(), frontFunnyChannelBarS);

    //volume medium
    TGeoMedium *frontFunnyChannelBarV_medium = pMedAluminium;
    if(frontFunnyChannelBarV_medium) {
        frontFunnyChannelBarV->SetMedium(frontFunnyChannelBarV_medium);
    }
    else Fatal("Main", "Invalid medium for frontFunnyChannelBarV!");

    //volume visual property (transparency)
    frontFunnyChannelBarV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontFunnyChannelBarV->SetTransparency(0);

    TGeoCombiTrans *frontFunnyChannelBar_transf[1];

    frontFunnyChannelBar_transf[0] = new TGeoCombiTrans();
    frontFunnyChannelBar_transf[0]->SetDx(-frontFiberGlassCover_XSize*0.5 - frontFunnyChannelBar_XSize*0.5 - 2.5/*shift*/);
    frontFunnyChannelBar_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + frontFunnyChannelBar_YSize*0.5 + 43.1/*shift*/);
    frontFunnyChannelBar_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - frontFunnyChannelBar_ZSize*0.5 - 2.1/*shift*/);

    frames->AddNode(frontFunnyChannelBarV, 0, frontFunnyChannelBar_transf[0]);
    //--------------------------------------------------------------------------

    //front channel bar --------------------------------------------------------
    Double_t frontRightChannelBar_XSize = 34.23; //cm
    Double_t frontRightChannelBar_YSize = 2.5; //cm
    Double_t frontRightChannelBar_ZSize = 0.47; //cm

    Double_t frontLeftChannelBar_XSize = 33.47; //cm
    Double_t frontLeftChannelBar_YSize = 2.5; //cm
    Double_t frontLeftChannelBar_ZSize = 0.47; //cm

    TGeoShape *frontRightChannelBarS = new TGeoBBox(TString("frontRightChannelBarS")+=TString("_") + frames->GetName(), frontRightChannelBar_XSize*0.5, frontRightChannelBar_YSize*0.5, frontRightChannelBar_ZSize*0.5);
    TGeoShape *frontLeftChannelBarS = new TGeoBBox(TString("frontLeftChannelBarS")+=TString("_") + frames->GetName(), frontLeftChannelBar_XSize*0.5, frontLeftChannelBar_YSize*0.5, frontLeftChannelBar_ZSize*0.5);

    TGeoVolume *frontRightChannelBarV = new TGeoVolume(TString("frontRightChannelBarV")+=TString("_") + frames->GetName(), frontRightChannelBarS);
    TGeoVolume *frontLeftChannelBarV = new TGeoVolume(TString("frontLeftChannelBarV")+=TString("_") + frames->GetName(), frontLeftChannelBarS);

    //volume medium
    TGeoMedium *frontChannel_medium = pMedAluminium;
    if(frontChannel_medium) {
        frontRightChannelBarV->SetMedium(frontChannel_medium);
        frontLeftChannelBarV->SetMedium(frontChannel_medium);
    }
    else Fatal("Main", "Invalid medium for frontChannelBarV!");

    //volume visual property (transparency)
    frontRightChannelBarV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontRightChannelBarV->SetTransparency(0);
    frontLeftChannelBarV->SetLineColor(TColor::GetColor("#ffcccc"));
    frontLeftChannelBarV->SetTransparency(0);

    TGeoCombiTrans *frontRightChannelBar_transf[1];

    frontRightChannelBar_transf[0] = new TGeoCombiTrans();
    frontRightChannelBar_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - frontRightChannelBar_XSize*0.5 - 37.14/*shift*/);
    frontRightChannelBar_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + frontRightChannelBar_YSize*0.5 + 43.1/*shift*/);
    frontRightChannelBar_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - frontRightChannelBar_ZSize*0.5 - 2.1/*shift*/);

    TGeoCombiTrans *frontLeftChannelBar_transf[1];

    frontLeftChannelBar_transf[0] = new TGeoCombiTrans();
    frontLeftChannelBar_transf[0]->SetDx(frontFiberGlassCover_XSize*0.5 - frontLeftChannelBar_XSize*0.5 - 2.37/*shift*/);
    frontLeftChannelBar_transf[0]->SetDy(-frontFiberGlassCover_YSize*0.5 + frontLeftChannelBar_YSize*0.5 + 43.1/*shift*/);
    frontLeftChannelBar_transf[0]->SetDz(centerFiberGlassFrameSidePart_ZSize*0.5 - frontLeftChannelBar_ZSize*0.5 - 2.1/*shift*/);

    frames->AddNode(frontRightChannelBarV, 0, frontRightChannelBar_transf[0]);
    frames->AddNode(frontLeftChannelBarV, 0, frontLeftChannelBar_transf[0]);
    //--------------------------------------------------------------------------

    return frames;
}
//------------------------------------------------------------------------------

TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius) {

    //module shapes
    TGeoShape *module_partS = new TGeoBBox("module_partS", xsize*0.5, ysize*0.5, zsize*0.5);
    TGeoShape *holeS = new TGeoTube("holeS", 0.0, hole_radius, zsize*0.5+0.01);

    gRandom->SetSeed(0);
    Int_t uniqueId = gRandom->Integer(10000);
    TGeoTranslation *hole_module_trans = new TGeoTranslation(TString("hole_module_trans_")+TString::Itoa(uniqueId,10), -xsize*0.5, -ysize*0.5, 0.0);

    hole_module_trans->RegisterYourself();

    TString bool_expression_module = TString(module_partS->GetName()) + TString(" - ") + TString(holeS->GetName()) + TString(":") + TString(hole_module_trans->GetName());
    TGeoCompositeShape *moduleS = new TGeoCompositeShape(bool_expression_module);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if(pMedArCO27030) moduleV->SetMedium(pMedArCO27030);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}

TGeoVolume *CreateLayersForModule_Station163x45(TString layer_name) {

    //frame volumes
    TGeoVolume *layers = new TGeoVolumeAssembly(layer_name);
    layers->SetMedium(pMedAir);

    //Common parameters for all layers
    Double_t layer_XSize = XModuleSize_Station163x45; //cm
    Double_t layer_YSize = YModuleSize_Station163x45; //cm

    Double_t layerFront_XSize = 80.85; //cm
    Double_t layerFront_YSize = 44.2; //cm
    Double_t layerFront_XShift = -0.375; //cm
    Double_t layerFront_YShift = -0.2; //cm

    Double_t layerBack_XSize = 80.71; //cm
    Double_t layerBack_YSize = 44.2; //cm
    Double_t layerBack_XShift = -0.445; //cm
    Double_t layerBack_YShift = -0.2; //cm

    //copper layer -------------------------------------------------------------
    Double_t copperLayer_ZSize = 0.00655; //cm;
    Double_t copperLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *copperLayerFrontBlankS = new TGeoBBox(TString("copperLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, copperLayer_ZSize*0.5);
    TGeoShape *copperLayerBackBlankS = new TGeoBBox(TString("copperLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, copperLayer_ZSize*0.5);

    TGeoShape *copperLayerFrontHoleS = new TGeoTube(TString("copperLayerFrontHoleS")+=TString("_") + layers->GetName(), 0.0, copperLayerHole_Radius, copperLayer_ZSize*0.5 + 0.01);
    TGeoShape *copperLayerBackHoleS = new TGeoTube(TString("copperLayerBackHoleS")+=TString("_") + layers->GetName(), 0.0, copperLayerHole_Radius, copperLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *copperLayerFrontHole_pos = new TGeoTranslation();
        copperLayerFrontHole_pos->SetName("copperLayerFrontHole_pos");
        copperLayerFrontHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerFront_XShift);
        copperLayerFrontHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerFront_YShift);
        copperLayerFrontHole_pos->SetDz(0.0);
        copperLayerFrontHole_pos->RegisterYourself();

    TGeoTranslation *copperLayerBackHole_pos = new TGeoTranslation();
        copperLayerBackHole_pos->SetName("copperLayerBackHole_pos");
        copperLayerBackHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerBack_XShift);
        copperLayerBackHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerBack_YShift);
        copperLayerBackHole_pos->SetDz(0.0);
        copperLayerBackHole_pos->RegisterYourself();

    TGeoCompositeShape *copperLayerFrontS = new TGeoCompositeShape();
    copperLayerFrontS->SetName(TString("copperLayerFrontS")+=TString("_") + layers->GetName());
    {
        TString expression = "copperLayerFrontBlankS"; expression += TString("_") + layers->GetName();
            expression += "-copperLayerFrontHoleS"; expression += TString("_") + layers->GetName();
            expression += ":copperLayerFrontHole_pos";

        copperLayerFrontS->MakeNode(expression);
        copperLayerFrontS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *copperLayerBackS = new TGeoCompositeShape();
    copperLayerBackS->SetName(TString("copperLayerBackS")+=TString("_") + layers->GetName());
    {
        TString expression = "copperLayerBackBlankS"; expression += TString("_") + layers->GetName();
            expression += "-copperLayerBackHoleS"; expression += TString("_") + layers->GetName();
            expression += ":copperLayerBackHole_pos";

        copperLayerBackS->MakeNode(expression);
        copperLayerBackS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *copperLayerFrontV = new TGeoVolume(TString("copperLayerFrontV")+=TString("_") + layers->GetName(), copperLayerFrontS);
    TGeoVolume *copperLayerBackV = new TGeoVolume(TString("copperLayerBackV")+=TString("_") + layers->GetName(), copperLayerBackS);

    //volume medium
    TGeoMedium *copperLayerV_medium = pMedCopper;
    if(copperLayerV_medium) {
        copperLayerFrontV->SetMedium(copperLayerV_medium);
        copperLayerBackV->SetMedium(copperLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for copperLayerV!");

    //volume visual property (transparency)
    copperLayerFrontV->SetLineColor(TColor::GetColor("#ff7538"));
    copperLayerFrontV->SetTransparency(0);
    copperLayerBackV->SetLineColor(TColor::GetColor("#ff7538"));
    copperLayerBackV->SetTransparency(0);

    TGeoCombiTrans *copperLayerFront_transf[2];

    copperLayerFront_transf[0] = new TGeoCombiTrans();
    copperLayerFront_transf[0]->SetDx(+layerFront_XShift);
    copperLayerFront_transf[0]->SetDy(+layerFront_YShift);
    copperLayerFront_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize*0.5);

    TGeoCombiTrans *copperLayerBack_transf[1];

    copperLayerBack_transf[0] = new TGeoCombiTrans();
    copperLayerBack_transf[0]->SetDx(+layerBack_XShift);
    copperLayerBack_transf[0]->SetDy(+layerBack_YShift);
    copperLayerBack_transf[0]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5) ;

    layers->AddNode(copperLayerFrontV, 0, copperLayerFront_transf[0]);
    layers->AddNode(copperLayerBackV, 0, copperLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //glue layer ---------------------------------------------------------------
    Double_t glueLayer_ZSize = 0.01; //cm; half-thickness of all glue layers
    Double_t glueLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *glueLayerFrontBlankS = new TGeoBBox(TString("glueLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, glueLayer_ZSize*0.5);
    TGeoShape *glueLayerBackBlankS = new TGeoBBox(TString("glueLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, glueLayer_ZSize*0.5);

    TGeoShape *glueLayerFrontHoleS = new TGeoTube(TString("glueLayerFrontHoleS")+=TString("_") + layers->GetName(), 0.0, glueLayerHole_Radius, glueLayer_ZSize*0.5 + 0.01);
    TGeoShape *glueLayerBackHoleS = new TGeoTube(TString("glueLayerBackHoleS")+=TString("_") + layers->GetName(), 0.0, glueLayerHole_Radius, glueLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *glueLayerFrontHole_pos = new TGeoTranslation();
        glueLayerFrontHole_pos->SetName("glueLayerFrontHole_pos");
        glueLayerFrontHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerFront_XShift);
        glueLayerFrontHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerFront_YShift);
        glueLayerFrontHole_pos->SetDz(0.0);
        glueLayerFrontHole_pos->RegisterYourself();

    TGeoTranslation *glueLayerBackHole_pos = new TGeoTranslation();
        glueLayerBackHole_pos->SetName("glueLayerBackHole_pos");
        glueLayerBackHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerBack_XShift);
        glueLayerBackHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerBack_YShift);
        glueLayerBackHole_pos->SetDz(0.0);
        glueLayerBackHole_pos->RegisterYourself();

    TGeoCompositeShape *glueLayerFrontS = new TGeoCompositeShape();
    glueLayerFrontS->SetName(TString("glueLayerFrontS")+=TString("_") + layers->GetName());
    {
        TString expression = "glueLayerFrontBlankS"; expression += TString("_") + layers->GetName();
            expression += "-glueLayerFrontHoleS"; expression += TString("_") + layers->GetName();
            expression += ":glueLayerFrontHole_pos";

        glueLayerFrontS->MakeNode(expression);
        glueLayerFrontS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *glueLayerBackS = new TGeoCompositeShape();
    glueLayerBackS->SetName(TString("glueLayerBackS")+=TString("_") + layers->GetName());
    {
        TString expression = "glueLayerBackBlankS"; expression += TString("_") + layers->GetName();
            expression += "-glueLayerBackHoleS"; expression += TString("_") + layers->GetName();
            expression += ":glueLayerBackHole_pos";

        glueLayerBackS->MakeNode(expression);
        glueLayerBackS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *glueLayerFrontV = new TGeoVolume(TString("glueLayerFrontV")+=TString("_") + layers->GetName(), glueLayerFrontS);
    TGeoVolume *glueLayerBackV = new TGeoVolume(TString("glueLayerBackV")+=TString("_") + layers->GetName(), glueLayerBackS);

    //volume medium
    TGeoMedium *glueLayerV_medium = pMedGlue;
    if(glueLayerV_medium) {
        glueLayerFrontV->SetMedium(glueLayerV_medium);
        glueLayerBackV->SetMedium(glueLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for glueLayerV!");

    //volume visual property (transparency)
    glueLayerFrontV->SetLineColor(TColor::GetColor("#1919ff"));
    glueLayerFrontV->SetTransparency(0);
    glueLayerBackV->SetLineColor(TColor::GetColor("#1919ff"));
    glueLayerBackV->SetTransparency(0);

    TGeoCombiTrans *glueLayerFront_transf[1];

    glueLayerFront_transf[0] = new TGeoCombiTrans();
    glueLayerFront_transf[0]->SetDx(+layerFront_XShift);
    glueLayerFront_transf[0]->SetDy(+layerFront_YShift);
    glueLayerFront_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize*0.5);

    TGeoCombiTrans *glueLayerBack_transf[1];

    glueLayerBack_transf[0] = new TGeoCombiTrans();
    glueLayerBack_transf[0]->SetDx(+layerBack_XShift);
    glueLayerBack_transf[0]->SetDy(+layerBack_YShift);
    glueLayerBack_transf[0]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize*0.5) ;

    layers->AddNode(glueLayerFrontV, 0, glueLayerFront_transf[0]);
    layers->AddNode(glueLayerBackV, 0, glueLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //epoxide layer ------------------------------------------------------------
    Double_t epoxideLayer_ZSize = 0.105; //cm; half-thickness of all epoxide layers
    Double_t epoxideLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *epoxideLayerFrontBlankS = new TGeoBBox(TString("epoxideLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, epoxideLayer_ZSize*0.5);
    TGeoShape *epoxideLayerBackBlankS = new TGeoBBox(TString("epoxideLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, epoxideLayer_ZSize*0.5);

    TGeoShape *epoxideLayerFrontHoleS = new TGeoTube(TString("epoxideLayerFrontHoleS")+=TString("_") + layers->GetName(), 0.0, epoxideLayerHole_Radius, epoxideLayer_ZSize*0.5 + 0.01);
    TGeoShape *epoxideLayerBackHoleS = new TGeoTube(TString("epoxideLayerBackHoleS")+=TString("_") + layers->GetName(), 0.0, epoxideLayerHole_Radius, epoxideLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *epoxideLayerFrontHole_pos = new TGeoTranslation();
        epoxideLayerFrontHole_pos->SetName("epoxideLayerFrontHole_pos");
        epoxideLayerFrontHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerFront_XShift);
        epoxideLayerFrontHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerFront_YShift);
        epoxideLayerFrontHole_pos->SetDz(0.0);
        epoxideLayerFrontHole_pos->RegisterYourself();

    TGeoTranslation *epoxideLayerBackHole_pos = new TGeoTranslation();
        epoxideLayerBackHole_pos->SetName("epoxideLayerBackHole_pos");
        epoxideLayerBackHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerBack_XShift);
        epoxideLayerBackHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerBack_YShift);
        epoxideLayerBackHole_pos->SetDz(0.0);
        epoxideLayerBackHole_pos->RegisterYourself();

    TGeoCompositeShape *epoxideLayerFrontS = new TGeoCompositeShape();
    epoxideLayerFrontS->SetName(TString("epoxideLayerFrontS")+=TString("_") + layers->GetName());
    {
        TString expression = "epoxideLayerFrontBlankS"; expression += TString("_") + layers->GetName();
            expression += "-epoxideLayerFrontHoleS"; expression += TString("_") + layers->GetName();
            expression += ":epoxideLayerFrontHole_pos";

        epoxideLayerFrontS->MakeNode(expression);
        epoxideLayerFrontS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *epoxideLayerBackS = new TGeoCompositeShape();
    epoxideLayerBackS->SetName(TString("epoxideLayerBackS")+=TString("_") + layers->GetName());
    {
        TString expression = "epoxideLayerBackBlankS"; expression += TString("_") + layers->GetName();
            expression += "-epoxideLayerBackHoleS"; expression += TString("_") + layers->GetName();
            expression += ":epoxideLayerBackHole_pos";

        epoxideLayerBackS->MakeNode(expression);
        epoxideLayerBackS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *epoxideLayerFrontV = new TGeoVolume(TString("epoxideLayerFrontV")+=TString("_") + layers->GetName(), epoxideLayerFrontS);
    TGeoVolume *epoxideLayerBackV = new TGeoVolume(TString("epoxideLayerBackV")+=TString("_") + layers->GetName(), epoxideLayerBackS);

    //volume medium
    TGeoMedium *epoxideLayerV_medium = pMedEpoxideCompound;
    if(epoxideLayerV_medium) {
        epoxideLayerFrontV->SetMedium(epoxideLayerV_medium);
        epoxideLayerBackV->SetMedium(epoxideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for epoxideLayerV!");

    //volume visual property (transparency)
    epoxideLayerFrontV->SetLineColor(TColor::GetColor("#aa33ff"));
    epoxideLayerFrontV->SetTransparency(0);
    epoxideLayerBackV->SetLineColor(TColor::GetColor("#aa33ff"));
    epoxideLayerBackV->SetTransparency(0);

    TGeoCombiTrans *epoxideLayerFront_transf[1];

    epoxideLayerFront_transf[0] = new TGeoCombiTrans();
    epoxideLayerFront_transf[0]->SetDx(+layerFront_XShift);
    epoxideLayerFront_transf[0]->SetDy(+layerFront_YShift);
    epoxideLayerFront_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize*0.5);

    TGeoCombiTrans *epoxideLayerBack_transf[1];

    epoxideLayerBack_transf[0] = new TGeoCombiTrans();
    epoxideLayerBack_transf[0]->SetDx(+layerBack_XShift);
    epoxideLayerBack_transf[0]->SetDy(+layerBack_YShift);
    epoxideLayerBack_transf[0]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize - epoxideLayer_ZSize*0.5) ;

    layers->AddNode(epoxideLayerFrontV, 0, epoxideLayerFront_transf[0]);
    layers->AddNode(epoxideLayerBackV, 0, epoxideLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    //honeycomb layer ----------------------------------------------------------
    Double_t honeycombLayer_ZSize = 1.5-(0.06805); //cm; half-thickness of all honeycomb layers
    Double_t honeycombLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *honeycombLayerFrontBlankS = new TGeoBBox(TString("honeycombLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, honeycombLayer_ZSize*0.5);
    TGeoShape *honeycombLayerBackBlankS = new TGeoBBox(TString("honeycombLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, honeycombLayer_ZSize*0.5);

    TGeoShape *honeycombLayerFrontHoleS = new TGeoTube(TString("honeycombLayerFrontHoleS")+=TString("_") + layers->GetName(), 0.0, honeycombLayerHole_Radius, honeycombLayer_ZSize*0.5 + 0.01);
    TGeoShape *honeycombLayerBackHoleS = new TGeoTube(TString("honeycombLayerBackHoleS")+=TString("_") + layers->GetName(), 0.0, honeycombLayerHole_Radius, honeycombLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *honeycombLayerFrontHole_pos = new TGeoTranslation();
        honeycombLayerFrontHole_pos->SetName("honeycombLayerFrontHole_pos");
        honeycombLayerFrontHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerFront_XShift);
        honeycombLayerFrontHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerFront_YShift);
        honeycombLayerFrontHole_pos->SetDz(0.0);
        honeycombLayerFrontHole_pos->RegisterYourself();

    TGeoTranslation *honeycombLayerBackHole_pos = new TGeoTranslation();
        honeycombLayerBackHole_pos->SetName("honeycombLayerBackHole_pos");
        honeycombLayerBackHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerBack_XShift);
        honeycombLayerBackHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerBack_YShift);
        honeycombLayerBackHole_pos->SetDz(0.0);
        honeycombLayerBackHole_pos->RegisterYourself();

    TGeoCompositeShape *honeycombLayerFrontS = new TGeoCompositeShape();
    honeycombLayerFrontS->SetName(TString("honeycombLayerFrontS")+=TString("_") + layers->GetName());
    {
        TString expression = "honeycombLayerFrontBlankS"; expression += TString("_") + layers->GetName();
            expression += "-honeycombLayerFrontHoleS"; expression += TString("_") + layers->GetName();
            expression += ":honeycombLayerFrontHole_pos";

        honeycombLayerFrontS->MakeNode(expression);
        honeycombLayerFrontS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *honeycombLayerBackS = new TGeoCompositeShape();
    honeycombLayerBackS->SetName(TString("honeycombLayerBackS")+=TString("_") + layers->GetName());
    {
        TString expression = "honeycombLayerBackBlankS"; expression += TString("_") + layers->GetName();
            expression += "-honeycombLayerBackHoleS"; expression += TString("_") + layers->GetName();
            expression += ":honeycombLayerBackHole_pos";

        honeycombLayerBackS->MakeNode(expression);
        honeycombLayerBackS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *honeycombLayerFrontV = new TGeoVolume(TString("honeycombLayerFrontV")+=TString("_") + layers->GetName(), honeycombLayerFrontS);
    TGeoVolume *honeycombLayerBackV = new TGeoVolume(TString("honeycombLayerBackV")+=TString("_") + layers->GetName(), honeycombLayerBackS);

    //volume medium
    TGeoMedium *honeycombLayerV_medium = pMedHoneyComb;
    if(honeycombLayerV_medium) {
        honeycombLayerFrontV->SetMedium(honeycombLayerV_medium);
        honeycombLayerBackV->SetMedium(honeycombLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for honeycombLayerV!");

    //volume visual property (transparency)
    honeycombLayerFrontV->SetLineColor(TColor::GetColor("#ffdf84"));
    honeycombLayerFrontV->SetTransparency(0);
    honeycombLayerBackV->SetLineColor(TColor::GetColor("#ffdf84"));
    honeycombLayerBackV->SetTransparency(0);

    TGeoCombiTrans *honeycombLayerFront_transf[1];

    honeycombLayerFront_transf[0] = new TGeoCombiTrans();
    honeycombLayerFront_transf[0]->SetDx(+layerFront_XShift);
    honeycombLayerFront_transf[0]->SetDy(+layerFront_YShift);
    honeycombLayerFront_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize + honeycombLayer_ZSize*0.5);

    TGeoCombiTrans *honeycombLayerBack_transf[1];

    honeycombLayerBack_transf[0] = new TGeoCombiTrans();
    honeycombLayerBack_transf[0]->SetDx(+layerBack_XShift);
    honeycombLayerBack_transf[0]->SetDy(+layerBack_YShift);
    honeycombLayerBack_transf[0]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize - epoxideLayer_ZSize - honeycombLayer_ZSize*0.5) ;

    layers->AddNode(honeycombLayerFrontV, 0, honeycombLayerFront_transf[0]);
    layers->AddNode(honeycombLayerBackV, 0, honeycombLayerBack_transf[0]);
    //--------------------------------------------------------------------------

     //polyamide layer ----------------------------------------------------------
    Double_t polyamideLayer_ZSize = 0.0125; //cm; half-thickness of all polyamide layers
    Double_t polyamideLayerHole_Radius = 6.0; //cm

    //layer shape
    TGeoShape *polyamideLayerFrontBlankS = new TGeoBBox(TString("polyamideLayerFrontBlankS")+=TString("_") + layers->GetName(), layerFront_XSize*0.5, layerFront_YSize*0.5, polyamideLayer_ZSize*0.5);
    TGeoShape *polyamideLayerBackBlankS = new TGeoBBox(TString("polyamideLayerBackBlankS")+=TString("_") + layers->GetName(), layerBack_XSize*0.5, layerBack_YSize*0.5, polyamideLayer_ZSize*0.5);

    TGeoShape *polyamideLayerFrontHoleS = new TGeoTube(TString("polyamideLayerFrontHoleS")+=TString("_") + layers->GetName(), 0.0, polyamideLayerHole_Radius, polyamideLayer_ZSize*0.5 + 0.01);
    TGeoShape *polyamideLayerBackHoleS = new TGeoTube(TString("polyamideLayerBackHoleS")+=TString("_") + layers->GetName(), 0.0, polyamideLayerHole_Radius, polyamideLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *polyamideLayerFrontHole_pos = new TGeoTranslation();
        polyamideLayerFrontHole_pos->SetName("polyamideLayerFrontHole_pos");
        polyamideLayerFrontHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerFront_XShift);
        polyamideLayerFrontHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerFront_YShift);
        polyamideLayerFrontHole_pos->SetDz(0.0);
        polyamideLayerFrontHole_pos->RegisterYourself();

    TGeoTranslation *polyamideLayerBackHole_pos = new TGeoTranslation();
        polyamideLayerBackHole_pos->SetName("polyamideLayerBackHole_pos");
        polyamideLayerBackHole_pos->SetDx(-XModuleSize_Station163x45*0.5 - layerBack_XShift);
        polyamideLayerBackHole_pos->SetDy(-YModuleSize_Station163x45*0.5 - layerBack_YShift);
        polyamideLayerBackHole_pos->SetDz(0.0);
        polyamideLayerBackHole_pos->RegisterYourself();

    TGeoCompositeShape *polyamideLayerFrontS = new TGeoCompositeShape();
    polyamideLayerFrontS->SetName(TString("polyamideLayerFrontS")+=TString("_") + layers->GetName());
    {
        TString expression = "polyamideLayerFrontBlankS"; expression += TString("_") + layers->GetName();
            expression += "-polyamideLayerFrontHoleS"; expression += TString("_") + layers->GetName();
            expression += ":polyamideLayerFrontHole_pos";

        polyamideLayerFrontS->MakeNode(expression);
        polyamideLayerFrontS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoCompositeShape *polyamideLayerBackS = new TGeoCompositeShape();
    polyamideLayerBackS->SetName(TString("polyamideLayerBackS")+=TString("_") + layers->GetName());
    {
        TString expression = "polyamideLayerBackBlankS"; expression += TString("_") + layers->GetName();
            expression += "-polyamideLayerBackHoleS"; expression += TString("_") + layers->GetName();
            expression += ":polyamideLayerBackHole_pos";

        polyamideLayerBackS->MakeNode(expression);
        polyamideLayerBackS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *polyamideLayerFrontV = new TGeoVolume(TString("polyamideLayerFrontV")+=TString("_") + layers->GetName(), polyamideLayerFrontS);
    TGeoVolume *polyamideLayerBackV = new TGeoVolume(TString("polyamideLayerBackV")+=TString("_") + layers->GetName(), polyamideLayerBackS);

    //volume medium
    TGeoMedium *polyamideLayerV_medium = pMedPolyamide;
    if(polyamideLayerV_medium) {
        polyamideLayerFrontV->SetMedium(polyamideLayerV_medium);
        polyamideLayerBackV->SetMedium(polyamideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for polyamideLayerV!");

    //volume visual property (transparency)
    polyamideLayerFrontV->SetLineColor(TColor::GetColor("#fefe22"));
    polyamideLayerFrontV->SetTransparency(0);
    polyamideLayerBackV->SetLineColor(TColor::GetColor("#fefe22"));
    polyamideLayerBackV->SetTransparency(0);

    TGeoCombiTrans *polyamideLayerFront_transf[1];

    polyamideLayerFront_transf[0] = new TGeoCombiTrans();
    polyamideLayerFront_transf[0]->SetDx(+layerFront_XShift);
    polyamideLayerFront_transf[0]->SetDy(+layerFront_YShift);
    polyamideLayerFront_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize + honeycombLayer_ZSize + polyamideLayer_ZSize*0.5);

    TGeoCombiTrans *polyamideLayerBack_transf[1];

    polyamideLayerBack_transf[0] = new TGeoCombiTrans();
    polyamideLayerBack_transf[0]->SetDx(+layerBack_XShift);
    polyamideLayerBack_transf[0]->SetDy(+layerBack_YShift);
    polyamideLayerBack_transf[0]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize - epoxideLayer_ZSize - honeycombLayer_ZSize - polyamideLayer_ZSize*0.5) ;

    layers->AddNode(polyamideLayerFrontV, 0, polyamideLayerFront_transf[0]);
    layers->AddNode(polyamideLayerBackV, 0, polyamideLayerBack_transf[0]);
    //--------------------------------------------------------------------------

    return layers;
}

TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dx*0.5, YModuleSize_Station163x45*0.5+dy, dz*0.5);
    TGeoShape *horizontal_upper_frameS = new TGeoBBox("horizontal_upper_frameS", XModuleSize_Station163x45*0.5, dy*0.5, dz*0.5);
    TGeoShape *horizontal_bottom_frameS = new TGeoBBox("horizontal_upper_frameS", XModuleSize_Station163x45*0.5-hole_radius*0.5, dy*0.5, dz*0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name+"_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_upper_frameV = new TGeoVolume(frame_name+"_horizontal_upper_frameV", horizontal_upper_frameS);
    TGeoVolume *horizontal_bottom_frameV = new TGeoVolume(frame_name+"_horizontal_bottom_frameV", horizontal_bottom_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_upper_frameV->SetMedium(frame_medium);
        horizontal_bottom_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");
    if(pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_upper_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_bottom_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station163x45*0.5+dXFrame_Station163x45*0.5, 0.0, 0.0);

    TGeoCombiTrans *upper_horizontal_frame_position = new TGeoCombiTrans();
    upper_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station163x45*0.5+dYFrame_Station163x45*0.5, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(hole_radius*0.5, -YModuleSize_Station163x45*0.5-dYFrame_Station163x45*0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(horizontal_upper_frameV, 0, upper_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_bottom_frameV, 0, bottom_horizontal_frame_position);

    return composite_frameV;
}

//------------------------------------------------------------------------------
TGeoVolume *CreateFrameForHalfPlane_Station163x45(TString frame_name) {

    //frame volumes
    TGeoVolume *frames = new TGeoVolumeAssembly(frame_name);
    frames->SetMedium(pMedAir);

    //back lower epoxide frame -------------------------------------------------
    Double_t backLowerEpoxideFrame_XSize = 161.42; //cm
    Double_t backLowerEpoxideFrame_YSize = 2.2; //cm
    Double_t backLowerEpoxideFrame_ZSize = 1.6; //cm

    Double_t backLowerEpoxideFrameArch_Rmin = 4.0;
    Double_t backLowerEpoxideFrameArch_Rmax = 6.0;
    Double_t backLowerEpoxideFrameArch_ZSize = backLowerEpoxideFrame_ZSize;

    TGeoShape *backLowerEpoxideFrameHalfS = new TGeoBBox(TString("backLowerEpoxideFrameHalfS")+=TString("_") + frames->GetName(), (backLowerEpoxideFrame_XSize*0.5 - backLowerEpoxideFrameArch_Rmin)*0.5, backLowerEpoxideFrame_YSize*0.5, backLowerEpoxideFrame_ZSize*0.5);
    TGeoShape *backLowerEpoxideFrameArchS = new TGeoTubeSeg(TString("backLowerEpoxideFrameArchS")+=TString("_") + frames->GetName(), backLowerEpoxideFrameArch_Rmin, backLowerEpoxideFrameArch_Rmax, backLowerEpoxideFrameArch_ZSize*0.5, 0.0, 180.0);

    TGeoTranslation *backLowerEpoxideFrameHalf_pos1 = new TGeoTranslation();
        backLowerEpoxideFrameHalf_pos1->SetName("backLowerEpoxideFrameHalf1_pos1");
        backLowerEpoxideFrameHalf_pos1->SetDx(+((backLowerEpoxideFrame_XSize*0.5 - backLowerEpoxideFrameArch_Rmin)*0.5 + backLowerEpoxideFrameArch_Rmin));
        backLowerEpoxideFrameHalf_pos1->SetDy(-(backLowerEpoxideFrame_YSize*0.5) + 0.2/*shift*/);
        backLowerEpoxideFrameHalf_pos1->SetDz(0.0);
        backLowerEpoxideFrameHalf_pos1->RegisterYourself();

    TGeoTranslation *backLowerEpoxideFrameHalf_pos2 = new TGeoTranslation();
        backLowerEpoxideFrameHalf_pos2->SetName("backLowerEpoxideFrameHalf1_pos2");
        backLowerEpoxideFrameHalf_pos2->SetDx(-((backLowerEpoxideFrame_XSize*0.5 - backLowerEpoxideFrameArch_Rmin)*0.5 + backLowerEpoxideFrameArch_Rmin));
        backLowerEpoxideFrameHalf_pos2->SetDy(-(backLowerEpoxideFrame_YSize*0.5) + 0.2/*shift*/);
        backLowerEpoxideFrameHalf_pos2->SetDz(0.0);
        backLowerEpoxideFrameHalf_pos2->RegisterYourself();

    TGeoTranslation *backLowerEpoxideFrameArch_pos = new TGeoTranslation();
        backLowerEpoxideFrameArch_pos->SetName("backLowerEpoxideFrameArch_pos");
        backLowerEpoxideFrameArch_pos->SetDx(0.0);
        backLowerEpoxideFrameArch_pos->SetDy(0.0);
        backLowerEpoxideFrameArch_pos->SetDz(0.0);
        backLowerEpoxideFrameArch_pos->RegisterYourself();

    TGeoCompositeShape *backLowerEpoxideFrameS = new TGeoCompositeShape();
    backLowerEpoxideFrameS->SetName(TString("backLowerEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "backLowerEpoxideFrameHalfS"; expression += TString("_") + frames->GetName();
            expression += ":backLowerEpoxideFrameHalf1_pos1";
            expression += "+backLowerEpoxideFrameHalfS"; expression += TString("_") + frames->GetName();
            expression += ":backLowerEpoxideFrameHalf1_pos2";
            expression += "+backLowerEpoxideFrameArchS"; expression += TString("_") + frames->GetName();
            expression += ":backLowerEpoxideFrameArch_pos";

        backLowerEpoxideFrameS->MakeNode(expression);
        backLowerEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *backLowerEpoxideFrameV = new TGeoVolume(TString("backLowerEpoxideFrameV")+=TString("_") + frames->GetName(), backLowerEpoxideFrameS);

    //volume medium
    TGeoMedium *backLowerEpoxideFrameV_medium = pMedEpoxideCompound;
    if(backLowerEpoxideFrameV_medium) {
        backLowerEpoxideFrameV->SetMedium(backLowerEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for backLowerEpoxideFrameV!");

    //volume visual property (transparency)
    backLowerEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    backLowerEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *backLowerEpoxideFrame_transf[1];

    backLowerEpoxideFrame_transf[0] = new TGeoCombiTrans();
    backLowerEpoxideFrame_transf[0]->SetDx(0.0);
    backLowerEpoxideFrame_transf[0]->SetDy(-(YModuleSize_Station163x45*0.5));
    backLowerEpoxideFrame_transf[0]->SetDz(+(backLowerEpoxideFrame_ZSize*0.5 + ZModuleSize_Station163x45*0.5));

    frames->AddNode(backLowerEpoxideFrameV, 0, backLowerEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //front lower epoxide frame -------------------------------------------------
    Double_t frontLowerEpoxideFrame_XSize = 161.7; //cm
    Double_t frontLowerEpoxideFrame_YSize = 2.2; //cm
    Double_t frontLowerEpoxideFrame_ZSize = 1.6; //cm

    Double_t frontLowerEpoxideFrameArch_Rmin = 4.0;
    Double_t frontLowerEpoxideFrameArch_Rmax = 6.0;
    Double_t frontLowerEpoxideFrameArch_ZSize = frontLowerEpoxideFrame_ZSize;

    TGeoShape *frontLowerEpoxideFrameHalfS = new TGeoBBox(TString("frontLowerEpoxideFrameHalfS")+=TString("_") + frames->GetName(), (frontLowerEpoxideFrame_XSize*0.5 - frontLowerEpoxideFrameArch_Rmin)*0.5, frontLowerEpoxideFrame_YSize*0.5, frontLowerEpoxideFrame_ZSize*0.5);
    TGeoShape *frontLowerEpoxideFrameArchS = new TGeoTubeSeg(TString("frontLowerEpoxideFrameArchS")+=TString("_") + frames->GetName(), frontLowerEpoxideFrameArch_Rmin, frontLowerEpoxideFrameArch_Rmax, frontLowerEpoxideFrameArch_ZSize*0.5, 0.0, 180.0);

    TGeoTranslation *frontLowerEpoxideFrameHalf_pos1 = new TGeoTranslation();
        frontLowerEpoxideFrameHalf_pos1->SetName("frontLowerEpoxideFrameHalf1_pos1");
        frontLowerEpoxideFrameHalf_pos1->SetDx(+((frontLowerEpoxideFrame_XSize*0.5 - frontLowerEpoxideFrameArch_Rmin)*0.5 + frontLowerEpoxideFrameArch_Rmin));
        frontLowerEpoxideFrameHalf_pos1->SetDy(-(frontLowerEpoxideFrame_YSize*0.5) + 0.2/*shift*/);
        frontLowerEpoxideFrameHalf_pos1->SetDz(0.0);
        frontLowerEpoxideFrameHalf_pos1->RegisterYourself();

    TGeoTranslation *frontLowerEpoxideFrameHalf_pos2 = new TGeoTranslation();
        frontLowerEpoxideFrameHalf_pos2->SetName("frontLowerEpoxideFrameHalf1_pos2");
        frontLowerEpoxideFrameHalf_pos2->SetDx(-((frontLowerEpoxideFrame_XSize*0.5 - frontLowerEpoxideFrameArch_Rmin)*0.5 + frontLowerEpoxideFrameArch_Rmin));
        frontLowerEpoxideFrameHalf_pos2->SetDy(-(frontLowerEpoxideFrame_YSize*0.5) + 0.2/*shift*/);
        frontLowerEpoxideFrameHalf_pos2->SetDz(0.0);
        frontLowerEpoxideFrameHalf_pos2->RegisterYourself();

    TGeoTranslation *frontLowerEpoxideFrameArch_pos = new TGeoTranslation();
        frontLowerEpoxideFrameArch_pos->SetName("frontLowerEpoxideFrameArch_pos");
        frontLowerEpoxideFrameArch_pos->SetDx(0.0);
        frontLowerEpoxideFrameArch_pos->SetDy(0.0);
        frontLowerEpoxideFrameArch_pos->SetDz(0.0);
        frontLowerEpoxideFrameArch_pos->RegisterYourself();

    TGeoCompositeShape *frontLowerEpoxideFrameS = new TGeoCompositeShape();
    frontLowerEpoxideFrameS->SetName(TString("frontLowerEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "frontLowerEpoxideFrameHalfS"; expression += TString("_") + frames->GetName();
            expression += ":frontLowerEpoxideFrameHalf1_pos1";
            expression += "+frontLowerEpoxideFrameHalfS"; expression += TString("_") + frames->GetName();
            expression += ":frontLowerEpoxideFrameHalf1_pos2";
            expression += "+frontLowerEpoxideFrameArchS"; expression += TString("_") + frames->GetName();
            expression += ":frontLowerEpoxideFrameArch_pos";

        frontLowerEpoxideFrameS->MakeNode(expression);
        frontLowerEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *frontLowerEpoxideFrameV = new TGeoVolume(TString("frontLowerEpoxideFrameV")+=TString("_") + frames->GetName(), frontLowerEpoxideFrameS);

    //volume medium
    TGeoMedium *frontLowerEpoxideFrameV_medium = pMedEpoxideCompound;
    if(frontLowerEpoxideFrameV_medium) {
        frontLowerEpoxideFrameV->SetMedium(frontLowerEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for frontLowerEpoxideFrameV!");

    //volume visual property (transparency)
    frontLowerEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    frontLowerEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *frontLowerEpoxideFrame_transf[1];

    frontLowerEpoxideFrame_transf[0] = new TGeoCombiTrans();
    frontLowerEpoxideFrame_transf[0]->SetDx(0.0);
    frontLowerEpoxideFrame_transf[0]->SetDy(-(YModuleSize_Station163x45*0.5));
    frontLowerEpoxideFrame_transf[0]->SetDz(-(frontLowerEpoxideFrame_ZSize*0.5 + ZModuleSize_Station163x45*0.5));

    frames->AddNode(frontLowerEpoxideFrameV, 0, frontLowerEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //back side epoxide frame --------------------------------------------------
    Double_t backSideEpoxideFramePart_XSize = 9.2; //cm
    Double_t backSideEpoxideFramePart_YSize = 52.6; //cm
    Double_t backSideEpoxideFramePart_ZSize = 1.6; //cm

    Double_t backSideEpoxideFrame_XDistBetweenParts = 161.42; //cm

    TGeoShape *backSideEpoxideFramePartS = new TGeoBBox(TString("backSideEpoxideFramePartS")+=TString("_") + frames->GetName(), backSideEpoxideFramePart_XSize*0.5, backSideEpoxideFramePart_YSize*0.5, backSideEpoxideFramePart_ZSize*0.5);

    TGeoTranslation *backSideEpoxideFramePart_pos1 = new TGeoTranslation();
        backSideEpoxideFramePart_pos1->SetName("backSideEpoxideFramePart_pos1");
        backSideEpoxideFramePart_pos1->SetDx(+(backSideEpoxideFrame_XDistBetweenParts*0.5+backSideEpoxideFramePart_XSize*0.5));
        backSideEpoxideFramePart_pos1->SetDy(0.0);
        backSideEpoxideFramePart_pos1->SetDz(0.0);
        backSideEpoxideFramePart_pos1->RegisterYourself();

    TGeoTranslation *backSideEpoxideFramePart_pos2 = new TGeoTranslation();
        backSideEpoxideFramePart_pos2->SetName("backSideEpoxideFramePart_pos2");
        backSideEpoxideFramePart_pos2->SetDx(-(backSideEpoxideFrame_XDistBetweenParts*0.5+backSideEpoxideFramePart_XSize*0.5));
        backSideEpoxideFramePart_pos2->SetDy(0.0);
        backSideEpoxideFramePart_pos2->SetDz(0.0);
        backSideEpoxideFramePart_pos2->RegisterYourself();

    TGeoCompositeShape *backSideEpoxideFrameS = new TGeoCompositeShape();
    backSideEpoxideFrameS->SetName(TString("backSideEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "backSideEpoxideFramePartS"; expression += TString("_") + frames->GetName();
            expression += ":backSideEpoxideFramePart_pos1";
            expression += "+backSideEpoxideFramePartS"; expression += TString("_") + frames->GetName();
            expression += ":backSideEpoxideFramePart_pos2";

        backSideEpoxideFrameS->MakeNode(expression);
        backSideEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *backSideEpoxideFrameV = new TGeoVolume(TString("backSideEpoxideFrameV")+=TString("_") + frames->GetName(), backSideEpoxideFrameS);

    //volume medium
    TGeoMedium *backSideEpoxideFrameV_medium = pMedEpoxideCompound;
    if(backSideEpoxideFrameV_medium) {
        backSideEpoxideFrameV->SetMedium(backSideEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for backSideEpoxideFrameV!");

    //volume visual property (transparency)
    backSideEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    backSideEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *backSideEpoxideFrame_transf[1];

    backSideEpoxideFrame_transf[0] = new TGeoCombiTrans();
    backSideEpoxideFrame_transf[0]->SetDx(0.0);
    backSideEpoxideFrame_transf[0]->SetDy(+((backSideEpoxideFramePart_YSize - YModuleSize_Station163x45)*0.5) - frontLowerEpoxideFrame_YSize + 0.2/*shift*/);
    backSideEpoxideFrame_transf[0]->SetDz(+(backSideEpoxideFramePart_ZSize*0.5 + ZModuleSize_Station163x45*0.5));

    frames->AddNode(backSideEpoxideFrameV, 0, backSideEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //back upper epoxide frame -------------------------------------------------
    Double_t backUpperEpoxideFrame_XSize = 161.42; //cm
    Double_t backUpperEpoxideFrame_YSize = 6.2; //cm
    Double_t backUpperEpoxideFrame_ZSize = 1.6; //cm

    TGeoShape *backUpperEpoxideFrameS = new TGeoBBox("backUpperEpoxideFrameS", backUpperEpoxideFrame_XSize*0.5, backUpperEpoxideFrame_YSize*0.5, backUpperEpoxideFrame_ZSize*0.5);

    TGeoVolume *backUpperEpoxideFrameV = new TGeoVolume(TString("backUpperEpoxideFrameV")+=TString("_") + frames->GetName(), backUpperEpoxideFrameS);

    //volume medium
    TGeoMedium *backUpperEpoxideFrameV_medium = pMedEpoxideCompound;
    if(backUpperEpoxideFrameV_medium) {
        backUpperEpoxideFrameV->SetMedium(backUpperEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for backUpperEpoxideFrameV!");

    //volume visual property (transparency)
    backUpperEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    backUpperEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *backUpperEpoxideFrame_transf[1];

    backUpperEpoxideFrame_transf[0] = new TGeoCombiTrans();
    backUpperEpoxideFrame_transf[0]->SetDx(0.0);
    backUpperEpoxideFrame_transf[0]->SetDy(+(backUpperEpoxideFrame_YSize*0.5 + YModuleSize_Station163x45*0.5) - 0.6/*shift*/);
    backUpperEpoxideFrame_transf[0]->SetDz(+(backUpperEpoxideFrame_ZSize*0.5 + ZModuleSize_Station163x45*0.5));

    frames->AddNode(backUpperEpoxideFrameV, 0, backUpperEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //front side epoxide frame -------------------------------------------------
    Double_t frontSideEpoxideFramePart_XSize = 3.5; //cm
    Double_t frontSideEpoxideFramePart_YSize = 49.9; //cm
    Double_t frontSideEpoxideFramePart_ZSize = 1.6; //cm

    Double_t frontSideEpoxideFrame_XDistBetweenParts = 161.7; //cm

    TGeoShape *frontSideEpoxideFramePartS = new TGeoBBox(TString("frontSideEpoxideFramePartS")+=TString("_") + frames->GetName(), frontSideEpoxideFramePart_XSize*0.5, frontSideEpoxideFramePart_YSize*0.5, frontSideEpoxideFramePart_ZSize*0.5);

    TGeoTranslation *frontSideEpoxideFramePart_pos1 = new TGeoTranslation();
        frontSideEpoxideFramePart_pos1->SetName("frontSideEpoxideFramePart_pos1");
        frontSideEpoxideFramePart_pos1->SetDx(+(frontSideEpoxideFrame_XDistBetweenParts*0.5+frontSideEpoxideFramePart_XSize*0.5));
        frontSideEpoxideFramePart_pos1->SetDy(0.0);
        frontSideEpoxideFramePart_pos1->SetDz(0.0);
        frontSideEpoxideFramePart_pos1->RegisterYourself();

    TGeoTranslation *frontSideEpoxideFramePart_pos2 = new TGeoTranslation();
        frontSideEpoxideFramePart_pos2->SetName("frontSideEpoxideFramePart_pos2");
        frontSideEpoxideFramePart_pos2->SetDx(-(frontSideEpoxideFrame_XDistBetweenParts*0.5+frontSideEpoxideFramePart_XSize*0.5));
        frontSideEpoxideFramePart_pos2->SetDy(0.0);
        frontSideEpoxideFramePart_pos2->SetDz(0.0);
        frontSideEpoxideFramePart_pos2->RegisterYourself();

    TGeoCompositeShape *frontSideEpoxideFrameS = new TGeoCompositeShape();
    frontSideEpoxideFrameS->SetName(TString("frontSideEpoxideFrameS")+=TString("_") + frames->GetName());
    {
        TString expression = "frontSideEpoxideFramePartS"; expression += TString("_") + frames->GetName();
            expression += ":frontSideEpoxideFramePart_pos1";
            expression += "+frontSideEpoxideFramePartS"; expression += TString("_") + frames->GetName();
            expression += ":frontSideEpoxideFramePart_pos2";

        frontSideEpoxideFrameS->MakeNode(expression);
        frontSideEpoxideFrameS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *frontSideEpoxideFrameV = new TGeoVolume(TString("frontSideEpoxideFrameV")+=TString("_") + frames->GetName(), frontSideEpoxideFrameS);

    //volume medium
    TGeoMedium *frontSideEpoxideFrameV_medium = pMedEpoxideCompound;
    if(frontSideEpoxideFrameV_medium) {
        frontSideEpoxideFrameV->SetMedium(frontSideEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for frontSideEpoxideFrameV!");

    //volume visual property (transparency)
    frontSideEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    frontSideEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *frontSideEpoxideFrame_transf[1];

    frontSideEpoxideFrame_transf[0] = new TGeoCombiTrans();
    frontSideEpoxideFrame_transf[0]->SetDx(0.0);
    frontSideEpoxideFrame_transf[0]->SetDy(+((frontSideEpoxideFramePart_YSize - YModuleSize_Station163x45)*0.5) - frontLowerEpoxideFrame_YSize + 0.2/*shift*/);
    frontSideEpoxideFrame_transf[0]->SetDz(-(frontSideEpoxideFramePart_ZSize*0.5 + ZModuleSize_Station163x45*0.5));

    frames->AddNode(frontSideEpoxideFrameV, 0, frontSideEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //front upper epoxide frame -------------------------------------------------
    Double_t frontUpperEpoxideFrame_XSize = 161.7; //cm
    Double_t frontUpperEpoxideFrame_YSize = 3.5; //cm
    Double_t frontUpperEpoxideFrame_ZSize = 1.6; //cm

    TGeoShape *frontUpperEpoxideFrameS = new TGeoBBox("frontUpperEpoxideFrameS", frontUpperEpoxideFrame_XSize*0.5, frontUpperEpoxideFrame_YSize*0.5, frontUpperEpoxideFrame_ZSize*0.5);

    TGeoVolume *frontUpperEpoxideFrameV = new TGeoVolume(TString("frontUpperEpoxideFrameV")+=TString("_") + frames->GetName(), frontUpperEpoxideFrameS);

    //volume medium
    TGeoMedium *frontUpperEpoxideFrameV_medium = pMedEpoxideCompound;
    if(frontUpperEpoxideFrameV_medium) {
        frontUpperEpoxideFrameV->SetMedium(frontUpperEpoxideFrameV_medium);
    }
    else Fatal("Main", "Invalid medium for frontUpperEpoxideFrameV!");

    //volume visual property (transparency)
    frontUpperEpoxideFrameV->SetLineColor(TColor::GetColor("#ccccff"));
    frontUpperEpoxideFrameV->SetTransparency(0);

    TGeoCombiTrans *frontUpperEpoxideFrame_transf[1];

    frontUpperEpoxideFrame_transf[0] = new TGeoCombiTrans();
    frontUpperEpoxideFrame_transf[0]->SetDx(0.0);
    frontUpperEpoxideFrame_transf[0]->SetDy(+(frontUpperEpoxideFrame_YSize*0.5 + YModuleSize_Station163x45*0.5) - 0.6/*shift*/);
    frontUpperEpoxideFrame_transf[0]->SetDz(-(frontUpperEpoxideFrame_ZSize*0.5 + ZModuleSize_Station163x45*0.5));

    frames->AddNode(frontUpperEpoxideFrameV, 0, frontUpperEpoxideFrame_transf[0]);
    //--------------------------------------------------------------------------

    //alum. cable support ------------------------------------------------------
    Double_t alumCableSupportPart_XSize = 3.0; //cm
    Double_t alumCableSupportPart_YSize = 41.3; //cm
    Double_t alumCableSupportPart_ZSize = 0.2; //cm

    Double_t alumCableSupport_XDistBetweenParts = 164.82; //cm

    TGeoShape *alumCableSupportPartS = new TGeoBBox(TString("alumCableSupportPartS")+=TString("_") + frames->GetName(), alumCableSupportPart_XSize*0.5, alumCableSupportPart_YSize*0.5, alumCableSupportPart_ZSize*0.5);

    TGeoTranslation *alumCableSupportPart_pos1 = new TGeoTranslation();
        alumCableSupportPart_pos1->SetName("alumCableSupportPart_pos1");
        alumCableSupportPart_pos1->SetDx(+(alumCableSupport_XDistBetweenParts*0.5+alumCableSupportPart_XSize*0.5));
        alumCableSupportPart_pos1->SetDy(0.0);
        alumCableSupportPart_pos1->SetDz(0.0);
        alumCableSupportPart_pos1->RegisterYourself();

    TGeoTranslation *alumCableSupportPart_pos2 = new TGeoTranslation();
        alumCableSupportPart_pos2->SetName("alumCableSupportPart_pos2");
        alumCableSupportPart_pos2->SetDx(-(alumCableSupport_XDistBetweenParts*0.5+alumCableSupportPart_XSize*0.5));
        alumCableSupportPart_pos2->SetDy(0.0);
        alumCableSupportPart_pos2->SetDz(0.0);
        alumCableSupportPart_pos2->RegisterYourself();

    TGeoCompositeShape *alumCableSupportS = new TGeoCompositeShape();
    alumCableSupportS->SetName(TString("alumCableSupportS")+=TString("_") + frames->GetName());
    {
        TString expression = "alumCableSupportPartS"; expression += TString("_") + frames->GetName();
            expression += ":alumCableSupportPart_pos1";
            expression += "+alumCableSupportPartS"; expression += TString("_") + frames->GetName();
            expression += ":alumCableSupportPart_pos2";

        alumCableSupportS->MakeNode(expression);
        alumCableSupportS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *alumCableSupportV = new TGeoVolume(TString("alumCableSupportV")+=TString("_") + frames->GetName(), alumCableSupportS);

    //volume medium
    TGeoMedium *alumCableSupportV_medium = pMedAluminium;
    if(alumCableSupportV_medium) {
        alumCableSupportV->SetMedium(alumCableSupportV_medium);
    }
    else Fatal("Main", "Invalid medium for alumCableSupportV!");

    //volume visual property (transparency)
    alumCableSupportV->SetLineColor(TColor::GetColor("#ffcccc"));
    alumCableSupportV->SetTransparency(0);

    TGeoCombiTrans *alumCableSupport_transf[1];

    alumCableSupport_transf[0] = new TGeoCombiTrans();
    alumCableSupport_transf[0]->SetDx(0.0);
    alumCableSupport_transf[0]->SetDy(+((alumCableSupportPart_YSize - YModuleSize_Station163x45)*0.5) - frontLowerEpoxideFrame_YSize + 0.2/*shift*/);
    alumCableSupport_transf[0]->SetDz(-(alumCableSupportPart_ZSize*0.5 + ZModuleSize_Station163x45*0.5 + frontSideEpoxideFramePart_ZSize + 1.0/*shift*/));

    frames->AddNode(alumCableSupportV, 0, alumCableSupport_transf[0]);
    //--------------------------------------------------------------------------

    //amplifier upper block ----------------------------------------------------
    Double_t amplifierUpperBlockPart1_XSize = 161.44; //cm
    Double_t amplifierUpperBlockPart1_YSize = 0.6; //cm
    Double_t amplifierUpperBlockPart1_ZSize = 8.41; //cm

    Double_t amplifierUpperBlockPart2_XSize = 161.16; //cm
    Double_t amplifierUpperBlockPart2_YSize = 0.6; //cm
    Double_t amplifierUpperBlockPart2_ZSize = 8.41; //cm

    TGeoShape *amplifierUpperBlockPart1S = new TGeoBBox(TString("amplifierUpperBlockPart1S")+=TString("_") + frames->GetName(), amplifierUpperBlockPart1_XSize*0.5, amplifierUpperBlockPart1_YSize*0.5, amplifierUpperBlockPart1_ZSize*0.5);
    TGeoShape *amplifierUpperBlockPart2S = new TGeoBBox(TString("amplifierUpperBlockPart2S")+=TString("_") + frames->GetName(), amplifierUpperBlockPart2_XSize*0.5, amplifierUpperBlockPart2_YSize*0.5, amplifierUpperBlockPart2_ZSize*0.5);

    TGeoTranslation *amplifierUpperBlockPart1_pos = new TGeoTranslation();
        amplifierUpperBlockPart1_pos->SetName("amplifierUpperBlockPart1_pos");
        amplifierUpperBlockPart1_pos->SetDx(0.0);
        amplifierUpperBlockPart1_pos->SetDy(0.0);
        amplifierUpperBlockPart1_pos->SetDz(0.0);
        amplifierUpperBlockPart1_pos->RegisterYourself();

    TGeoTranslation *amplifierUpperBlockPart2_pos = new TGeoTranslation();
        amplifierUpperBlockPart2_pos->SetName("amplifierUpperBlockPart2_pos");
        amplifierUpperBlockPart2_pos->SetDx(-0.76);
        amplifierUpperBlockPart2_pos->SetDy(-(amplifierUpperBlockPart1_YSize*0.5 + amplifierUpperBlockPart2_YSize*0.5 + 0.33));
        amplifierUpperBlockPart2_pos->SetDz(0.0);
        amplifierUpperBlockPart2_pos->RegisterYourself();

    TGeoCompositeShape *amplifierUpperBlockS = new TGeoCompositeShape();
    amplifierUpperBlockS->SetName(TString("amplifierUpperBlockS")+=TString("_") + frames->GetName());
    {
        TString expression = "amplifierUpperBlockPart1S"; expression += TString("_") + frames->GetName();
            expression += ":amplifierUpperBlockPart1_pos";
            expression += "+amplifierUpperBlockPart1S"; expression += TString("_") + frames->GetName();
            expression += ":amplifierUpperBlockPart2_pos";

        amplifierUpperBlockS->MakeNode(expression);
        amplifierUpperBlockS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *amplifierUpperBlockV = new TGeoVolume(TString("amplifierUpperBlockV")+=TString("_") + frames->GetName(), amplifierUpperBlockS);

    //volume medium
    TGeoMedium *amplifierUpperBlockV_medium = pMedFiberGlass;
    if(amplifierUpperBlockV_medium) {
        amplifierUpperBlockV->SetMedium(amplifierUpperBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for amplifierUpperBlockV!");

    //volume visual property (transparency)
    amplifierUpperBlockV->SetLineColor(TColor::GetColor("#ccffcc"));
    amplifierUpperBlockV->SetTransparency(0);

    TGeoCombiTrans *amplifierUpperBlock_transf[1];

    amplifierUpperBlock_transf[0] = new TGeoCombiTrans();
    amplifierUpperBlock_transf[0]->SetDx(0.0);
    amplifierUpperBlock_transf[0]->SetDy(+(YModuleSize_Station163x45*0.5 + backUpperEpoxideFrame_YSize) - 1.0/*shift*/);
    amplifierUpperBlock_transf[0]->SetDz(-(amplifierUpperBlockPart1_ZSize*0.5) + ZModuleSize_Station163x45*0.5);

    frames->AddNode(amplifierUpperBlockV, 0, amplifierUpperBlock_transf[0]);
    //--------------------------------------------------------------------------

    //amplifier side block -----------------------------------------------------
    Double_t amplifierSideBlockPartBig_XSize = 0.6; //cm
    Double_t amplifierSideBlockPartBig_YSize = 33.9; //cm
    Double_t amplifierSideBlockPartBig_ZSize = 8.41; //cm

    Double_t amplifierSideBlockPartSmall_XSize = 0.6; //cm
    Double_t amplifierSideBlockPartSmall_YSize = 27.05; //cm
    Double_t amplifierSideBlockPartSmall_ZSize = 8.41; //cm

    TGeoShape *amplifierSideBlockPartBigS = new TGeoBBox(TString("amplifierSideBlockPartBigS")+=TString("_") + frames->GetName(), amplifierSideBlockPartBig_XSize*0.5, amplifierSideBlockPartBig_YSize*0.5, amplifierSideBlockPartBig_ZSize*0.5);
    TGeoShape *amplifierSideBlockPartSmallS = new TGeoBBox(TString("amplifierSideBlockPartSmallS")+=TString("_") + frames->GetName(), amplifierSideBlockPartSmall_XSize*0.5, amplifierSideBlockPartSmall_YSize*0.5, amplifierSideBlockPartSmall_ZSize*0.5);

    TGeoTranslation *amplifierSideBlockPartBig_pos = new TGeoTranslation();
        amplifierSideBlockPartBig_pos->SetName("amplifierSideBlockPartBig_pos");
        amplifierSideBlockPartBig_pos->SetDx(0.0);
        amplifierSideBlockPartBig_pos->SetDy(0.0);
        amplifierSideBlockPartBig_pos->SetDz(0.0);
        amplifierSideBlockPartBig_pos->RegisterYourself();

    TGeoTranslation *amplifierSideBlockPartSmall_pos = new TGeoTranslation();
        amplifierSideBlockPartSmall_pos->SetName("amplifierSideBlockPartSmall_pos");
        amplifierSideBlockPartSmall_pos->SetDx(+(amplifierSideBlockPartBig_XSize*0.5 + amplifierSideBlockPartSmall_XSize*0.5 + 1.72));
        amplifierSideBlockPartSmall_pos->SetDy(+0.05);
        amplifierSideBlockPartSmall_pos->SetDz(0.0);
        amplifierSideBlockPartSmall_pos->RegisterYourself();

    TGeoCompositeShape *amplifierSideBlockS = new TGeoCompositeShape();
    amplifierSideBlockS->SetName(TString("amplifierSideBlockS")+=TString("_") + frames->GetName());
    {
        TString expression = "amplifierSideBlockPartBigS"; expression += TString("_") + frames->GetName();
            expression += ":amplifierSideBlockPartBig_pos";
            expression += "+amplifierSideBlockPartSmallS"; expression += TString("_") + frames->GetName();
            expression += ":amplifierSideBlockPartSmall_pos";

        amplifierSideBlockS->MakeNode(expression);
        amplifierSideBlockS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *amplifierSideBlockV = new TGeoVolume(TString("amplifierSideBlockV")+=TString("_") + frames->GetName(), amplifierSideBlockS);

    //volume medium
    TGeoMedium *amplifierSideBlockV_medium = pMedFiberGlass;
    if(amplifierSideBlockV_medium) {
        amplifierSideBlockV->SetMedium(amplifierSideBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for amplifierSideBlockV!");

    //volume visual property (transparency)
    amplifierSideBlockV->SetLineColor(TColor::GetColor("#ccffcc"));
    amplifierSideBlockV->SetTransparency(0);

    TGeoCombiTrans *amplifierSideBlock_transf[2];

    amplifierSideBlock_transf[0] = new TGeoCombiTrans();
    amplifierSideBlock_transf[0]->SetDx(+86.47/*shift*/);
    amplifierSideBlock_transf[0]->SetDy(-5.5);
    amplifierSideBlock_transf[0]->SetDz(-(amplifierSideBlockPartBig_ZSize*0.5) + ZModuleSize_Station163x45*0.5);

    amplifierSideBlock_transf[1] = new TGeoCombiTrans();
    amplifierSideBlock_transf[1]->RotateY(180.0);
    amplifierSideBlock_transf[1]->SetDx(-86.47/*shift*/);
    amplifierSideBlock_transf[1]->SetDy(-5.5);
    amplifierSideBlock_transf[1]->SetDz(-(amplifierSideBlockPartBig_ZSize*0.5) + ZModuleSize_Station163x45*0.5);

    frames->AddNode(amplifierSideBlockV, 0, amplifierSideBlock_transf[0]);
    frames->AddNode(amplifierSideBlockV, 1, amplifierSideBlock_transf[1]);
    //--------------------------------------------------------------------------

    //upper PCB (Printed Circuit Board) ----------------------------------------
    Double_t upperPcb_XSize = 177.22; //cm
    Double_t upperPcb_YSize = 0.3; //cm
    Double_t upperPcb_ZSize = 4.0; //cm

    TGeoShape *upperPcbS = new TGeoBBox("upperPcbS", upperPcb_XSize*0.5, upperPcb_YSize*0.5, upperPcb_ZSize*0.5);

    TGeoVolume *upperPcbV = new TGeoVolume(TString("upperPcbV")+=TString("_") + frames->GetName(), upperPcbS);

    //volume medium
    TGeoMedium *upperPcbV_medium = pMedFiberGlass;
    if(upperPcbV_medium) {
        upperPcbV->SetMedium(upperPcbV_medium);
    }
    else Fatal("Main", "Invalid medium for upperPcbV!");

    //volume visual property (transparency)
    upperPcbV->SetLineColor(TColor::GetColor("#88ff88"));
    upperPcbV->SetTransparency(0);

    TGeoCombiTrans *upperPcb_transf[1];

    upperPcb_transf[0] = new TGeoCombiTrans();
    upperPcb_transf[0]->SetDx(+0.1/*shift*/);
    upperPcb_transf[0]->SetDy(+(upperPcb_YSize*0.5 + 23.5/*shift*/));
    upperPcb_transf[0]->SetDz(-(upperPcb_ZSize*0.5) + ZModuleSize_Station163x45*0.5 - ZModuleSize_Station163x45 - frontSideEpoxideFramePart_ZSize);

    frames->AddNode(upperPcbV, 0, upperPcb_transf[0]);
    //--------------------------------------------------------------------------

    //side PCB -----------------------------------------------------------------
    Double_t sidePcbPart_XSize = 0.3; //cm
    Double_t sidePcbPart_YSize = 35.05; //cm
    Double_t sidePcbPart_ZSize = 4.0; //cm

    TGeoShape *sidePcbPartS = new TGeoBBox(TString("sidePcbPartS")+=TString("_") + frames->GetName(), sidePcbPart_XSize*0.5, sidePcbPart_YSize*0.5, sidePcbPart_ZSize*0.5);

    TGeoTranslation *sidePcbPart_pos1 = new TGeoTranslation();
        sidePcbPart_pos1->SetName("sidePcbPart_pos1");
        sidePcbPart_pos1->SetDx(0.0);
        sidePcbPart_pos1->SetDy(0.0);
        sidePcbPart_pos1->SetDz(0.0);
        sidePcbPart_pos1->RegisterYourself();

    TGeoTranslation *sidePcbPart_pos2 = new TGeoTranslation();
        sidePcbPart_pos2->SetName("sidePcbPart_pos2");
        sidePcbPart_pos2->SetDx(+(sidePcbPart_XSize + 6.1/*shift*/));
        sidePcbPart_pos2->SetDy(0.0);
        sidePcbPart_pos2->SetDz(0.0);
        sidePcbPart_pos2->RegisterYourself();


    TGeoCompositeShape *sidePcbS = new TGeoCompositeShape();
    sidePcbS->SetName(TString("sidePcbS")+=TString("_") + frames->GetName());
    {
        TString expression = "sidePcbPartS"; expression += TString("_") + frames->GetName();
            expression += ":sidePcbPart_pos1";
            expression += "+sidePcbPartS"; expression += TString("_") + frames->GetName();
            expression += ":sidePcbPart_pos2";

        sidePcbS->MakeNode(expression);
        sidePcbS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *sidePcbV = new TGeoVolume(TString("sidePcbV")+=TString("_") + frames->GetName(), sidePcbS);

    //volume medium
    TGeoMedium *sidePcbV_medium = pMedFiberGlass;
    if(sidePcbV_medium) {
        sidePcbV->SetMedium(sidePcbV_medium);
    }
    else Fatal("Main", "Invalid medium for sidePcbV!");

    //volume visual property (transparency)
    sidePcbV->SetLineColor(TColor::GetColor("#88ff88"));
    sidePcbV->SetTransparency(0);

    TGeoCombiTrans *sidePcb_transf[2];

    sidePcb_transf[0] = new TGeoCombiTrans();
    sidePcb_transf[0]->SetDx(+(sidePcbPart_XSize*0.5 + 84.1/*shift*/));
    sidePcb_transf[0]->SetDy(-6.8/*shift*/);
    sidePcb_transf[0]->SetDz(-(sidePcbPart_ZSize*0.5 + alumCableSupportPart_ZSize + ZModuleSize_Station163x45*0.5 + frontSideEpoxideFramePart_ZSize + 1.0/*shift*/));

    sidePcb_transf[1] = new TGeoCombiTrans();
    sidePcb_transf[1]->RotateY(180.0);
    sidePcb_transf[1]->SetDx(-(sidePcbPart_XSize*0.5 + 84.1/*shift*/));
    sidePcb_transf[1]->SetDy(-6.8/*shift*/);
    sidePcb_transf[1]->SetDz(-(sidePcbPart_ZSize*0.5 + alumCableSupportPart_ZSize + ZModuleSize_Station163x45*0.5 + frontSideEpoxideFramePart_ZSize + 1.0/*shift*/));

    frames->AddNode(sidePcbV, 0, sidePcb_transf[0]);
    frames->AddNode(sidePcbV, 1, sidePcb_transf[1]);
    //--------------------------------------------------------------------------

    //alum. side support -------------------------------------------------------
    Double_t alumSideSupport_XSize = 4.8; //cm
    Double_t alumSideSupport_YSize = 52.6; //cm
    Double_t alumSideSupport_ZSize = 0.4; //cm

    TGeoShape *alumSideSupportS = new TGeoBBox("alumSideSupportS", alumSideSupport_XSize*0.5, alumSideSupport_YSize*0.5, alumSideSupport_ZSize*0.5);

    TGeoVolume *alumSideSupportV = new TGeoVolume(TString("alumSideSupportV")+=TString("_") + frames->GetName(), alumSideSupportS);

    //volume medium
    TGeoMedium *alumSideSupportV_medium = pMedAluminium;
    if(alumSideSupportV_medium) {
        alumSideSupportV->SetMedium(alumSideSupportV_medium);
    }
    else Fatal("Main", "Invalid medium for alumSideSupportV!");

    //volume visual property (transparency)
    alumSideSupportV->SetLineColor(TColor::GetColor("#ffcccc"));
    alumSideSupportV->SetTransparency(0);

    TGeoCombiTrans *alumSideSupport_transf[2];

    alumSideSupport_transf[0] = new TGeoCombiTrans();
    alumSideSupport_transf[0]->SetDx(+(alumSideSupport_XSize*0.5 + 88.31/*shift*/));
    alumSideSupport_transf[0]->SetDy(+1.8);
    alumSideSupport_transf[0]->SetDz(+(ZModuleSize_Station163x45*0.5 - alumSideSupport_ZSize*0.5));

    alumSideSupport_transf[1] = new TGeoCombiTrans();
    alumSideSupport_transf[1]->SetDx(-(alumSideSupport_XSize*0.5 + 88.31/*shift*/));
    alumSideSupport_transf[1]->SetDy(+1.8/*shift*/);
    alumSideSupport_transf[1]->SetDz(+(ZModuleSize_Station163x45*0.5 - alumSideSupport_ZSize*0.5));

    frames->AddNode(alumSideSupportV, 0, alumSideSupport_transf[0]);
    frames->AddNode(alumSideSupportV, 1, alumSideSupport_transf[1]);
    //--------------------------------------------------------------------------

    //bosch shape --------------------------------------------------------------
    Double_t boschShape_XSize = 2.25; //cm
    Double_t boschShape_YSize = 52.6; //cm
    Double_t boschShape_ZSize = 2.25; //cm

    TGeoShape *boschShapeS = new TGeoBBox("boschShapeS", boschShape_XSize*0.5, boschShape_YSize*0.5, boschShape_ZSize*0.5);

    TGeoVolume *boschShapeV = new TGeoVolume(TString("boschShapeV")+=TString("_") + frames->GetName(), boschShapeS);

    //volume medium
    TGeoMedium *boschShapeV_medium = pMedAluminium;
    if(boschShapeV_medium) {
        boschShapeV->SetMedium(boschShapeV_medium);
    }
    else Fatal("Main", "Invalid medium for boschShapeV!");

    //volume visual property (transparency)
    boschShapeV->SetLineColor(TColor::GetColor("#ffcccc"));
    boschShapeV->SetTransparency(0);

    TGeoCombiTrans *boschShape_transf[2];

    boschShape_transf[0] = new TGeoCombiTrans();
    boschShape_transf[0]->SetDx(+(boschShape_XSize*0.5 + 90.86/*shift*/));
    boschShape_transf[0]->SetDy(+1.8/*shift*/);
    boschShape_transf[0]->SetDz(+(ZModuleSize_Station163x45*0.5 + boschShape_ZSize*0.5));

    boschShape_transf[1] = new TGeoCombiTrans();
    boschShape_transf[1]->SetDx(-(boschShape_XSize*0.5 + 90.86/*shift*/));
    boschShape_transf[1]->SetDy(+1.8/*shift*/);
    boschShape_transf[1]->SetDz(+(ZModuleSize_Station163x45*0.5 + boschShape_ZSize*0.5));

    frames->AddNode(boschShapeV, 0, boschShape_transf[0]);
    frames->AddNode(boschShapeV, 1, boschShape_transf[1]);
    //--------------------------------------------------------------------------

    //lower fiberglass block ---------------------------------------------------------
    Double_t lowerFiberGlassBlock_XSize = 165.1; //cm
    Double_t lowerFiberGlassBlock_YSize = 1.75; //cm
    Double_t lowerFiberGlassBlock_ZSize = 0.9; //cm

    Double_t lowerFiberGlassBlockArch_Rmin = 4.0;
    Double_t lowerFiberGlassBlockArch_Rmax = 5.75;
    Double_t lowerFiberGlassBlockArch_ZSize = lowerFiberGlassBlock_ZSize;

    TGeoShape *lowerFiberGlassBlockHalfS = new TGeoBBox(TString("lowerFiberGlassBlockHalfS")+=TString("_") + frames->GetName(), (lowerFiberGlassBlock_XSize*0.5 - lowerFiberGlassBlockArch_Rmin)*0.5, lowerFiberGlassBlock_YSize*0.5, lowerFiberGlassBlock_ZSize*0.5);
    TGeoShape *lowerFiberGlassBlockArchS = new TGeoTubeSeg(TString("lowerFiberGlassBlockArchS")+=TString("_") + frames->GetName(), lowerFiberGlassBlockArch_Rmin, lowerFiberGlassBlockArch_Rmax, lowerFiberGlassBlockArch_ZSize*0.5, 0.0, 180.0);

    TGeoTranslation *lowerFiberGlassBlockHalf_pos1 = new TGeoTranslation();
        lowerFiberGlassBlockHalf_pos1->SetName("lowerFiberGlassBlockHalf_pos1");
        lowerFiberGlassBlockHalf_pos1->SetDx(+((lowerFiberGlassBlock_XSize*0.5 - lowerFiberGlassBlockArch_Rmin)*0.5 + lowerFiberGlassBlockArch_Rmin));
        lowerFiberGlassBlockHalf_pos1->SetDy(-(lowerFiberGlassBlock_YSize*0.5));
        lowerFiberGlassBlockHalf_pos1->SetDz(0.0);
        lowerFiberGlassBlockHalf_pos1->RegisterYourself();

    TGeoTranslation *lowerFiberGlassBlockHalf_pos2 = new TGeoTranslation();
        lowerFiberGlassBlockHalf_pos2->SetName("lowerFiberGlassBlockHalf_pos2");
        lowerFiberGlassBlockHalf_pos2->SetDx(-((lowerFiberGlassBlock_XSize*0.5 - lowerFiberGlassBlockArch_Rmin)*0.5 + lowerFiberGlassBlockArch_Rmin));
        lowerFiberGlassBlockHalf_pos2->SetDy(-(lowerFiberGlassBlock_YSize*0.5));
        lowerFiberGlassBlockHalf_pos2->SetDz(0.0);
        lowerFiberGlassBlockHalf_pos2->RegisterYourself();

    TGeoTranslation *lowerFiberGlassBlockArch_pos = new TGeoTranslation();
        lowerFiberGlassBlockArch_pos->SetName("lowerFiberGlassBlockArch_pos");
        lowerFiberGlassBlockArch_pos->SetDx(0.0);
        lowerFiberGlassBlockArch_pos->SetDy(0.0);
        lowerFiberGlassBlockArch_pos->SetDz(0.0);
        lowerFiberGlassBlockArch_pos->RegisterYourself();

    TGeoCompositeShape *lowerFiberGlassBlockS = new TGeoCompositeShape();
    lowerFiberGlassBlockS->SetName(TString("lowerFiberGlassBlockS")+=TString("_") + frames->GetName());
    {
        TString expression = "lowerFiberGlassBlockHalfS"; expression += TString("_") + frames->GetName();
            expression += ":lowerFiberGlassBlockHalf_pos1";
            expression += "+lowerFiberGlassBlockHalfS"; expression += TString("_") + frames->GetName();
            expression += ":lowerFiberGlassBlockHalf_pos2";
            expression += "+lowerFiberGlassBlockArchS"; expression += TString("_") + frames->GetName();
            expression += ":lowerFiberGlassBlockArch_pos";

        lowerFiberGlassBlockS->MakeNode(expression);
        lowerFiberGlassBlockS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *lowerFiberGlassBlockV = new TGeoVolume(TString("lowerFiberGlassBlockV")+=TString("_") + frames->GetName(), lowerFiberGlassBlockS);

    //volume medium
    TGeoMedium *lowerFiberGlassBlockV_medium = pMedFiberGlass;
    if(lowerFiberGlassBlockV_medium) {
        lowerFiberGlassBlockV->SetMedium(lowerFiberGlassBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for lowerFiberGlassBlockV!");

    //volume visual property (transparency)
    lowerFiberGlassBlockV->SetLineColor(TColor::GetColor("#88ff88"));
    lowerFiberGlassBlockV->SetTransparency(0);

    TGeoCombiTrans *lowerFiberGlassBlock_transf[1];

    lowerFiberGlassBlock_transf[0] = new TGeoCombiTrans();
    lowerFiberGlassBlock_transf[0]->SetDx(0.0);
    lowerFiberGlassBlock_transf[0]->SetDy(-(YModuleSize_Station163x45*0.5));
    lowerFiberGlassBlock_transf[0]->SetDz(0.0);

    frames->AddNode(lowerFiberGlassBlockV, 0, lowerFiberGlassBlock_transf[0]);
    //--------------------------------------------------------------------------

    //side fiberglass block ----------------------------------------------------
    Double_t sideFiberGlassBlockPart_XSize = 1.45; //cm
    Double_t sideFiberGlassBlockPart_YSize = 49.4; //cm
    Double_t sideFiberGlassBlockPart_ZSize = 0.89; //cm

    TGeoShape *sideFiberGlassBlockS = new TGeoBBox("sideFiberGlassBlockS", sideFiberGlassBlockPart_XSize*0.5, sideFiberGlassBlockPart_YSize*0.5, sideFiberGlassBlockPart_ZSize*0.5);

    TGeoVolume *sideFiberGlassBlockV = new TGeoVolume(TString("sideFiberGlassBlockV")+=TString("_") + frames->GetName(), sideFiberGlassBlockS);

    //volume medium
    TGeoMedium *sideFiberGlassBlockV_medium = pMedFiberGlass;
    if(sideFiberGlassBlockV_medium) {
        sideFiberGlassBlockV->SetMedium(sideFiberGlassBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for sideFiberGlassBlockV!");

    //volume visual property (transparency)
    sideFiberGlassBlockV->SetLineColor(TColor::GetColor("#88ff88"));
    sideFiberGlassBlockV->SetTransparency(0);

    TGeoCombiTrans *sideFiberGlassBlock_transf[2];

    sideFiberGlassBlock_transf[0] = new TGeoCombiTrans();
    sideFiberGlassBlock_transf[0]->SetDx(+(lowerFiberGlassBlock_XSize*0.5 + sideFiberGlassBlockPart_XSize*0.5));
    sideFiberGlassBlock_transf[0]->SetDy(+0.45/*shift*/);
    sideFiberGlassBlock_transf[0]->SetDz(+0.005/*shift for fitting-elements as a plane*/);

    sideFiberGlassBlock_transf[1] = new TGeoCombiTrans();
    sideFiberGlassBlock_transf[1]->SetDx(-(lowerFiberGlassBlock_XSize*0.5 + sideFiberGlassBlockPart_XSize*0.5));
    sideFiberGlassBlock_transf[1]->SetDy(+0.45/*shift*/);
    sideFiberGlassBlock_transf[1]->SetDz(+0.005/*shift for fitting-elements as a plane*/);

    frames->AddNode(sideFiberGlassBlockV, 0, sideFiberGlassBlock_transf[0]);
    frames->AddNode(sideFiberGlassBlockV, 1, sideFiberGlassBlock_transf[1]);
    //--------------------------------------------------------------------------

    //upper fiberglass block ---------------------------------------------------
    Double_t upperFiberGlassBlock_XSize = 165.1; //cm
    Double_t upperFiberGlassBlock_YSize = 1.45; //cm
    Double_t upperFiberGlassBlock_ZSize = 0.89; //cm

    TGeoShape *upperFiberGlassBlockS = new TGeoBBox("upperFiberGlassBlockS", upperFiberGlassBlock_XSize*0.5, upperFiberGlassBlock_YSize*0.5, upperFiberGlassBlock_ZSize*0.5);

    TGeoVolume *upperFiberGlassBlockV = new TGeoVolume(TString("upperFiberGlassBlockV")+=TString("_") + frames->GetName(), upperFiberGlassBlockS);

    //volume medium
    TGeoMedium *upperFiberGlassBlockV_medium = pMedFiberGlass;
    if(upperFiberGlassBlockV_medium) {
        upperFiberGlassBlockV->SetMedium(upperFiberGlassBlockV_medium);
    }
    else Fatal("Main", "Invalid medium for upperFiberGlassBlockV!");

    //volume visual property (transparency)
    upperFiberGlassBlockV->SetLineColor(TColor::GetColor("#88ff88"));
    upperFiberGlassBlockV->SetTransparency(0);

    TGeoCombiTrans *upperFiberGlassBlock_transf[1];

    upperFiberGlassBlock_transf[0] = new TGeoCombiTrans();
    upperFiberGlassBlock_transf[0]->SetDx(0.0);
    upperFiberGlassBlock_transf[0]->SetDy(+(sideFiberGlassBlockPart_YSize*0.5 - upperFiberGlassBlock_YSize*0.5 + 0.45/*shift*/));
    upperFiberGlassBlock_transf[0]->SetDz(+0.005/*shift for fitting-elements as a plane*/);

    frames->AddNode(upperFiberGlassBlockV, 0, upperFiberGlassBlock_transf[0]);
    //--------------------------------------------------------------------------

    //brass layer --------------------------------------------------------------
    Double_t upperBrasslayer_XSize = 165.1; //cm
    Double_t upperBrasslayer_YSize = 1.45; //cm
    Double_t upperBrasslayer_ZSize = 0.005; //cm

    Double_t sideBrasslayer_XSize = 1.45; //cm
    Double_t sideBrasslayer_YSize = 49.4; //cm
    Double_t sideBrasslayer_ZSize = 0.005; //cm

    TGeoShape *upperBrasslayerS = new TGeoBBox("upperBrasslayerS", upperBrasslayer_XSize*0.5, upperBrasslayer_YSize*0.5, upperBrasslayer_ZSize*0.5);
    TGeoShape *sideBrasslayerS = new TGeoBBox("sideBrasslayerS", sideBrasslayer_XSize*0.5, sideBrasslayer_YSize*0.5, sideBrasslayer_ZSize*0.5);

    TGeoVolume *upperBrasslayerV = new TGeoVolume(TString("upperBrasslayerV")+=TString("_") + frames->GetName(), upperBrasslayerS);
    TGeoVolume *sideBrasslayerV = new TGeoVolume(TString("sideBrasslayerV")+=TString("_") + frames->GetName(), sideBrasslayerS);

    //volume medium
    TGeoMedium *upperBrasslayerV_medium = pMedBrass;
    if(upperBrasslayerV_medium) {
        upperBrasslayerV->SetMedium(upperBrasslayerV_medium);
        sideBrasslayerV->SetMedium(upperBrasslayerV_medium);
    }
    else Fatal("Main", "Invalid medium for upperBrasslayerV_medium!");

    //volume visual property (transparency)
    upperBrasslayerV->SetLineColor(TColor::GetColor("#f07422"));
    upperBrasslayerV->SetTransparency(0);
    sideBrasslayerV->SetLineColor(TColor::GetColor("#f07422"));
    sideBrasslayerV->SetTransparency(0);

    TGeoCombiTrans *upperBrasslayer_transf[1];

    upperBrasslayer_transf[0] = new TGeoCombiTrans();
    upperBrasslayer_transf[0]->SetDx(0.0);
    upperBrasslayer_transf[0]->SetDy(+(sideFiberGlassBlockPart_YSize*0.5 - upperBrasslayer_YSize*0.5 + 0.45/*shift*/));
    upperBrasslayer_transf[0]->SetDz(-ZModuleSize_Station163x45*0.5 + upperBrasslayer_ZSize*0.5 + 0.005/*shift*/);

    TGeoCombiTrans *sideBrasslayer_transf[2];

    sideBrasslayer_transf[0] = new TGeoCombiTrans();
    sideBrasslayer_transf[0]->SetDx(+(lowerFiberGlassBlock_XSize*0.5 + sideBrasslayer_XSize*0.5));
    sideBrasslayer_transf[0]->SetDy(+0.45/*shift*/);
    sideBrasslayer_transf[0]->SetDz(-ZModuleSize_Station163x45*0.5 + sideBrasslayer_ZSize*0.5 + 0.005/*shift*/);

    sideBrasslayer_transf[1] = new TGeoCombiTrans();
    sideBrasslayer_transf[1]->SetDx(-(lowerFiberGlassBlock_XSize*0.5 + sideBrasslayer_XSize*0.5));
    sideBrasslayer_transf[1]->SetDy(+0.45/*shift*/);
    sideBrasslayer_transf[1]->SetDz(-ZModuleSize_Station163x45*0.5 + sideBrasslayer_ZSize*0.5 + 0.005/*shift*/);

    frames->AddNode(upperBrasslayerV, 0, upperBrasslayer_transf[0]);

    frames->AddNode(sideBrasslayerV, 0, sideBrasslayer_transf[0]);
    frames->AddNode(sideBrasslayerV, 1, sideBrasslayer_transf[1]);
    //--------------------------------------------------------------------------

    //steel layer --------------------------------------------------------------
    Double_t upperSteellayer_XSize = 165.1; //cm
    Double_t upperSteellayer_YSize = 1.45; //cm
    Double_t upperSteellayer_ZSize = 0.005; //cm

    Double_t sideSteellayer_XSize = 1.45; //cm
    Double_t sideSteellayer_YSize = 49.4; //cm
    Double_t sideSteellayer_ZSize = 0.005; //cm

    TGeoShape *upperSteellayerS = new TGeoBBox("upperSteellayerS", upperSteellayer_XSize*0.5, upperSteellayer_YSize*0.5, upperSteellayer_ZSize*0.5);
    TGeoShape *sideSteellayerS = new TGeoBBox("sideSteellayerS", sideSteellayer_XSize*0.5, sideSteellayer_YSize*0.5, sideSteellayer_ZSize*0.5);

    TGeoVolume *upperSteellayerV = new TGeoVolume(TString("upperSteellayerV")+=TString("_") + frames->GetName(), upperSteellayerS);
    TGeoVolume *sideSteellayerV = new TGeoVolume(TString("sideSteellayerV")+=TString("_") + frames->GetName(), sideSteellayerS);

    //volume medium
    TGeoMedium *upperSteellayerV_medium = pMedSteel;
    if(upperSteellayerV_medium) {
        upperSteellayerV->SetMedium(upperSteellayerV_medium);
        sideSteellayerV->SetMedium(upperSteellayerV_medium);
    }
    else Fatal("Main", "Invalid medium for upperSteellayerV_medium!");

    //volume visual property (transparency)
    upperSteellayerV->SetLineColor(TColor::GetColor("#42aaff"));
    upperSteellayerV->SetTransparency(0);
    sideSteellayerV->SetLineColor(TColor::GetColor("#42aaff"));
    sideSteellayerV->SetTransparency(0);

    TGeoCombiTrans *upperSteellayer_transf[1];

    upperSteellayer_transf[0] = new TGeoCombiTrans();
    upperSteellayer_transf[0]->SetDx(0.0);
    upperSteellayer_transf[0]->SetDy(+(sideFiberGlassBlockPart_YSize*0.5 - upperSteellayer_YSize*0.5 + 0.45/*shift*/));
    upperSteellayer_transf[0]->SetDz(-ZModuleSize_Station163x45*0.5 + upperSteellayer_ZSize*0.5 + 0.005/*shift*/);

    TGeoCombiTrans *sideSteellayer_transf[2];

    sideSteellayer_transf[0] = new TGeoCombiTrans();
    sideSteellayer_transf[0]->SetDx(+(lowerFiberGlassBlock_XSize*0.5 + sideSteellayer_XSize*0.5));
    sideSteellayer_transf[0]->SetDy(+0.45/*shift*/);
    sideSteellayer_transf[0]->SetDz(-ZModuleSize_Station163x45*0.5 + sideSteellayer_ZSize*0.5);

    sideSteellayer_transf[1] = new TGeoCombiTrans();
    sideSteellayer_transf[1]->SetDx(-(lowerFiberGlassBlock_XSize*0.5 + sideSteellayer_XSize*0.5));
    sideSteellayer_transf[1]->SetDy(+0.45/*shift*/);
    sideSteellayer_transf[1]->SetDz(-ZModuleSize_Station163x45*0.5 + sideSteellayer_ZSize*0.5);

    frames->AddNode(upperSteellayerV, 0, upperSteellayer_transf[0]);

    frames->AddNode(sideSteellayerV, 0, sideSteellayer_transf[0]);
    frames->AddNode(sideSteellayerV, 1, sideSteellayer_transf[1]);
    //--------------------------------------------------------------------------

    return frames;
}