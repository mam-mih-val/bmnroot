/*
 * Use this macro to create ROOT geometry for the RunSRCSpring2018 configuration
 * (BM@N RUN-7:SRC)
 *
 * Created: 30.08.2018
 * 02.03.2021 (Updated): radius of hole in active zone (4.0 -> 6.0 cm)
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
//------------------------------------------------------------------------------

//GEM plane sizes (66x41 type) -------------------------------------------------
const Double_t XModuleSize_Station66x41 = 66.0;
const Double_t YModuleSize_Station66x41 = 41.0;
const Double_t ZModuleSize_Station66x41 = 0.9;

const Double_t dXFrame_Station66x41 = 2.0;
const Double_t dYFrame_Station66x41 = 2.0;
const Double_t dZFrame_Station66x41 = 2.1; //was 2.3 cm - thickness
//------------------------------------------------------------------------------

//GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2*0.5;
const Double_t YModuleSize_Station163x45 = 45.0;
const Double_t ZModuleSize_Station163x45 = 0.9;

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

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);
TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz);
TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius);

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
}

void create_rootgeom_GEMS_RunSRCSpring2018() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "RunSRCSpring2018";
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
        TGeoVolume *frame0 = CreateFrameForModule_Station66x41(TString("frame0_")+station->GetName(), dXFrame_Station66x41, dYFrame_Station66x41, dZFrame_Station66x41);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 1 (small 66x41 GEM: left side: lower station)
    if(1) {
        Int_t stationNum = 1; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *frame0 = CreateFrameForModule_Station66x41(TString("frame0_")+station->GetName(), dXFrame_Station66x41, dYFrame_Station66x41, dZFrame_Station66x41);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(+30.5843); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 2 (small 66x41 GEM: right side: upper station)
    if(1) {
        Int_t stationNum = 2; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *frame0 = CreateFrameForModule_Station66x41(TString("frame0_")+station->GetName(), dXFrame_Station66x41, dYFrame_Station66x41, dZFrame_Station66x41);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 3 (small 66x41 GEM: right side: lower station)
    if(1) {
        Int_t stationNum = 3; //station number

        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station66x41(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station66x41, YModuleSize_Station66x41, ZModuleSize_Station66x41);
        TGeoVolume *frame0 = CreateFrameForModule_Station66x41(TString("frame0_")+station->GetName(), dXFrame_Station66x41, dYFrame_Station66x41, dZFrame_Station66x41);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station66x41*0.5);
            module0_transform->RotateY(-30.6845); //deg

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //stations 4-9 (each is a big 163x45 GEM consisting of two modules)
    if(1) {
        for(Int_t istation = 4; istation < 10; ++istation) {

            Int_t stationNum = istation; //station number

            TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));
            TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
            TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
            TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_")+station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0+2.0);
            TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_")+station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0+2.0);

            TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
                module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
                module1_transform->ReflectX(true);
                module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

            TGeoCombiTrans *station_transform = new TGeoCombiTrans();
            station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

            station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
            station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

            station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
            station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

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