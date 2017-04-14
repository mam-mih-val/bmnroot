//---------------------------
#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "../../gem/BmnGemStripConfiguration.h"

using namespace TMath;

//GeoManager
TGeoManager* gGeoMan = NULL;

//Set Parameters of GEMS
const Int_t nStations = 6;

//Station Parameters
/*
const Double_t XStationPositions[nStations] = {-2.2, 3.0, -1.8, 1.0, 1.4, 2.1};
const Double_t YStationPositions[nStations] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const Double_t ZStationPositions[nStations] = {30.0, 45.0, 60.0, 80.0, 100.0, 130.0}; // z pos is the first surface of the first sens. volume in module !!!
*/

Double_t XStationPositions[nStations];
    XStationPositions[0] = BmnGemStripPositions_FirstConfig::XStationPositions[0];
    XStationPositions[1] = BmnGemStripPositions_FirstConfig::XStationPositions[1];
    XStationPositions[2] = BmnGemStripPositions_FirstConfig::XStationPositions[2];
    XStationPositions[3] = BmnGemStripPositions_FirstConfig::XStationPositions[3];
    XStationPositions[4] = BmnGemStripPositions_FirstConfig::XStationPositions[4];
    XStationPositions[5] = BmnGemStripPositions_FirstConfig::XStationPositions[5];

Double_t YStationPositions[nStations];
    YStationPositions[0] = BmnGemStripPositions_FirstConfig::YStationPositions[0];
    YStationPositions[1] = BmnGemStripPositions_FirstConfig::YStationPositions[1];
    YStationPositions[2] = BmnGemStripPositions_FirstConfig::YStationPositions[2];
    YStationPositions[3] = BmnGemStripPositions_FirstConfig::YStationPositions[3];
    YStationPositions[4] = BmnGemStripPositions_FirstConfig::YStationPositions[4];
    YStationPositions[5] = BmnGemStripPositions_FirstConfig::YStationPositions[5];

Double_t ZStationPositions[nStations];
    ZStationPositions[0] = BmnGemStripPositions_FirstConfig::ZStationPositions[0];
    ZStationPositions[1] = BmnGemStripPositions_FirstConfig::ZStationPositions[1];
    ZStationPositions[2] = BmnGemStripPositions_FirstConfig::ZStationPositions[2];
    ZStationPositions[3] = BmnGemStripPositions_FirstConfig::ZStationPositions[3];
    ZStationPositions[4] = BmnGemStripPositions_FirstConfig::ZStationPositions[4];
    ZStationPositions[5] = BmnGemStripPositions_FirstConfig::ZStationPositions[5];


//GEM plane sizes (66x41 type)
const Double_t XModuleSize_StationPrototype = 66.0;
const Double_t YModuleSize_StationPrototype = 41.0;

const Double_t XHotZoneSize_StationPrototype = 15.0;
const Double_t YHotZoneSize_StationPrototype = 10.0;

const Double_t XIntersectionSensModule = 5.0;

//GEM plane sizes (163x45 type)
const Double_t OuterZoneSizes_Station163x45[2] = {163.2*0.5, 45.0}; // {xsize, ysize}
const Double_t HotZoneSizes_rectangle_Station163x45[2] = {40.0, 15.0}; // {xsize, ysize}
const Double_t HotZoneSizes_slope_Station163x45[3] = {37.99, 42.01, 15.0}; // {xmin_size, xmax_size, ysize}
const Double_t HoleRadius_Station163x45 = 4.0; //cm (real hole without the circle frame)


//Module Parameters
const Double_t dXOuterFrame = 5.0;
const Double_t dXInnerFrame = 1.7;
const Double_t dYOuterFrame = 3.75;
const Double_t dYInnerFrame = 1.7;
const Double_t dZSensitiveVolume = 0.9;
const Double_t ZModuleSize = 2.5;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedArCO27030 = 0;

class FairGeoMedia;
class FairGeoBuilder;

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

void create_rootgeom_GEMS_1stConfig() {

    // Load the necessary FairRoot libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "1stConfig";
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

    //station creating

    CreateStation_One66x41Plane(GEMS, "station0", XStationPositions[0], YStationPositions[0], ZStationPositions[0], kFALSE);
    CreateStation_One66x41Plane(GEMS, "station1", XStationPositions[1], YStationPositions[1], ZStationPositions[1], kTRUE);
    CreateStation_One66x41Plane(GEMS, "station2", XStationPositions[2], YStationPositions[2], ZStationPositions[2], kFALSE);

    CreateStation_Two66x41Plane(GEMS, "station3", XStationPositions[3], YStationPositions[3], ZStationPositions[3]);

    CreateStation_One163x45Plane(GEMS, "station4", XStationPositions[4], YStationPositions[4], ZStationPositions[4], HoleRadius_Station163x45+dXInnerFrame, kFALSE);
    CreateStation_One163x45Plane(GEMS, "station5", XStationPositions[5], YStationPositions[5], ZStationPositions[5], HoleRadius_Station163x45+dXInnerFrame, kTRUE);



    top->AddNode(GEMS, 0);
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

//------------------------------------------------------------------------------
void CreateStation_One66x41Plane(TGeoVolume* mother_volume, TString station_name, Double_t x_position, Double_t y_position, Double_t z_position, Bool_t IsInverted) {
    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    //sensitive shapes
    TGeoShape *moduleS = new TGeoBBox("moduleS", XModuleSize_StationPrototype*0.5, YModuleSize_StationPrototype*0.5, dZSensitiveVolume*0.5);
    TGeoShape *hot_zoneS = new TGeoBBox("hot_zoneS", XHotZoneSize_StationPrototype*0.5, YHotZoneSize_StationPrototype*0.5, dZSensitiveVolume*0.5);

    //frame shapes
    TGeoShape *vertical_outer_frameS = new TGeoBBox("vertical_outer_frameS", dXOuterFrame*0.5, (YModuleSize_StationPrototype+dYOuterFrame+dYInnerFrame)*0.5, ZModuleSize*0.5);
    TGeoShape *horizontal_outer_frameS = new TGeoBBox("horizontal_outer_frameS", (XModuleSize_StationPrototype+dXInnerFrame)*0.5, dYOuterFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);
    TGeoShape *vertical_inner_frameS = new TGeoBBox("vertical_inner_frameS", dXInnerFrame*0.5, YModuleSize_StationPrototype*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);
    TGeoShape *horizontal_inner_frameS = new TGeoBBox("horizontal_inner_frameS", (XModuleSize_StationPrototype+dXInnerFrame)*0.5, dYInnerFrame*0.5, ZModuleSize*0.5);

/*
    TGeoXtru *moduleS = new TGeoXtru(2);
        Double_t x_pos[6] = {XModuleSize_StationPrototype, XModuleSize_StationPrototype, 0.0, 0.0, XHotZoneSize_StationPrototype, XHotZoneSize_StationPrototype};
        Double_t y_pos[6] = {0.0, YModuleSize_StationPrototype, YModuleSize_StationPrototype, YHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, 0.0};
    moduleS->DefinePolygon(6, x_pos, y_pos);
    moduleS->DefineSection(0, -dZSensitiveVolume*0.5);
    moduleS->DefineSection(1, dZSensitiveVolume*0.5);
*/
    //sensitive volumes
    TGeoVolume *moduleV = new TGeoVolume("module_Sensor", moduleS);
    TGeoVolume *hot_zoneV = new TGeoVolume("hot_zone_Sensor", hot_zoneS);
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    hot_zoneV->SetLineColor(TColor::GetColor("#e0b0ff"));

    //frame volumes
    TGeoVolume *vertical_outer_frameV = new TGeoVolume("vertical_outer_frame", vertical_outer_frameS);
    TGeoVolume *horizontal_outer_frameV = new TGeoVolume("horizontal_outer_frame", horizontal_outer_frameS);
    TGeoVolume *vertical_inner_frameV = new TGeoVolume("vertical_inner_frame", vertical_inner_frameS);
    TGeoVolume *horizontal_inner_frameV = new TGeoVolume("horizontal_inner_frame", horizontal_inner_frameS);
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly("frame");
    vertical_outer_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_outer_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    vertical_inner_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_inner_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //media for sensitive volumes
    TGeoMedium *sensitive_medium = pMedArCO27030; //set medium
    if(sensitive_medium) {
        moduleV->SetMedium(sensitive_medium);
        hot_zoneV->SetMedium(sensitive_medium);
    }
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //media for frames
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(frame_medium) {
        vertical_outer_frameV->SetMedium(frame_medium);
        horizontal_outer_frameV->SetMedium(frame_medium);
        vertical_inner_frameV->SetMedium(frame_medium);
        horizontal_inner_frameV->SetMedium(frame_medium);
        composite_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");

    moduleV->SetTransparency(20);

    //hot zone position in the module
    TGeoTranslation *hot_zone_trans = new TGeoTranslation(-XModuleSize_StationPrototype*0.5 + XHotZoneSize_StationPrototype*0.5, -YModuleSize_StationPrototype*0.5 + YHotZoneSize_StationPrototype*0.5, 0);

    //module positions in the station
    TGeoCombiTrans *module_center_position = new TGeoCombiTrans();
    module_center_position->ReflectX(true);
    if(IsInverted) {
        module_center_position->ReflectY(true);
    }
    //module_center_position->SetTranslation(0.0, 0.0, 0.0);

    //frame part positions in the frame
    TGeoCombiTrans *left_outer_frame_position = new TGeoCombiTrans();
    left_outer_frame_position->SetTranslation(XModuleSize_StationPrototype*0.5+dXOuterFrame*0.5, (dYOuterFrame-dYInnerFrame)*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);

    TGeoCombiTrans *top_outer_frame_position = new TGeoCombiTrans();
    top_outer_frame_position->SetTranslation(-dXInnerFrame*0.5, YModuleSize_StationPrototype*0.5+dYOuterFrame*0.5, ZModuleSize*0.5);

    TGeoCombiTrans *right_inner_frame_position = new TGeoCombiTrans();
    right_inner_frame_position->SetTranslation(-XModuleSize_StationPrototype*0.5-dXInnerFrame*0.5, 0, ZModuleSize*0.5);

    TGeoCombiTrans *bottom_inner_frame_position = new TGeoCombiTrans();
    bottom_inner_frame_position->SetTranslation(-dXInnerFrame*0.5, -YModuleSize_StationPrototype*0.5-dYInnerFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);

    //frame positions in the station
    TGeoCombiTrans *comp_frame_center_position = new TGeoCombiTrans();
    comp_frame_center_position->ReflectX(true);
    if(IsInverted) {
        comp_frame_center_position->ReflectY(true);
    }
    //comp_frame_center_position->SetTranslation(module_center_position->GetTranslation());

    //station assembling
    moduleV->AddNode(hot_zoneV, 0, hot_zone_trans);

    composite_frameV->AddNode(vertical_outer_frameV, 0, left_outer_frame_position);
    composite_frameV->AddNode(horizontal_outer_frameV, 0, top_outer_frame_position);
    composite_frameV->AddNode(vertical_inner_frameV, 0, right_inner_frame_position);
    composite_frameV->AddNode(horizontal_inner_frameV, 0, bottom_inner_frame_position);

    stationA->AddNode(moduleV, 0, module_center_position);

    stationA->AddNode(composite_frameV, 0, comp_frame_center_position);

    //Station position
    Double_t station_shift = dZSensitiveVolume*0.5;
    TGeoTranslation *station_pos = new TGeoTranslation(x_position, y_position, z_position+station_shift);

    mother_volume->AddNode(stationA, 0, station_pos);
}

void CreateStation_Two66x41Plane(TGeoVolume* mother_volume, TString station_name, Double_t x_position, Double_t y_position, Double_t z_position) {
    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    //sensitive shapes
    TGeoShape *moduleS = new TGeoBBox("moduleS", XModuleSize_StationPrototype*0.5, YModuleSize_StationPrototype*0.5, dZSensitiveVolume*0.5);
    TGeoShape *hot_zoneS = new TGeoBBox("hot_zoneS", XHotZoneSize_StationPrototype*0.5, YHotZoneSize_StationPrototype*0.5, dZSensitiveVolume*0.5);

    //frame shapes
    TGeoShape *vertical_outer_frameS = new TGeoBBox("vertical_outer_frameS", dXOuterFrame*0.5, (YModuleSize_StationPrototype+dYOuterFrame+dYInnerFrame)*0.5, ZModuleSize*0.5);
    TGeoShape *horizontal_outer_frameS = new TGeoBBox("horizontal_outer_frameS", (XModuleSize_StationPrototype+dXInnerFrame)*0.5, dYOuterFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);
    TGeoShape *vertical_inner_frameS = new TGeoBBox("vertical_inner_frameS", dXInnerFrame*0.5, YModuleSize_StationPrototype*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);
    TGeoShape *horizontal_inner_frameS = new TGeoBBox("horizontal_inner_frameS", (XModuleSize_StationPrototype+dXInnerFrame)*0.5, dYInnerFrame*0.5, ZModuleSize*0.5);

/*
    TGeoXtru *moduleS = new TGeoXtru(2);
        Double_t x_pos[6] = {XModuleSize_StationPrototype, XModuleSize_StationPrototype, 0.0, 0.0, XHotZoneSize_StationPrototype, XHotZoneSize_StationPrototype};
        Double_t y_pos[6] = {0.0, YModuleSize_StationPrototype, YModuleSize_StationPrototype, YHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, 0.0};
    moduleS->DefinePolygon(6, x_pos, y_pos);
    moduleS->DefineSection(0, -dZSensitiveVolume*0.5);
    moduleS->DefineSection(1, dZSensitiveVolume*0.5);
*/
    //sensitive volumes
    TGeoVolume *moduleV = new TGeoVolume("module_Sensor", moduleS);
    TGeoVolume *hot_zoneV = new TGeoVolume("hot_zone_Sensor", hot_zoneS);
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    hot_zoneV->SetLineColor(TColor::GetColor("#e0b0ff"));

    //frame volumes
    TGeoVolume *vertical_outer_frameV = new TGeoVolume("vertical_outer_frame", vertical_outer_frameS);
    TGeoVolume *horizontal_outer_frameV = new TGeoVolume("horizontal_outer_frame", horizontal_outer_frameS);
    TGeoVolume *vertical_inner_frameV = new TGeoVolume("vertical_inner_frame", vertical_inner_frameS);
    TGeoVolume *horizontal_inner_frameV = new TGeoVolume("horizontal_inner_frame", horizontal_inner_frameS);
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly("frame");
    vertical_outer_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_outer_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    vertical_inner_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_inner_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //media for sensitive volumes
    TGeoMedium *sensitive_medium = pMedArCO27030; //set medium
    if(sensitive_medium) {
        moduleV->SetMedium(sensitive_medium);
        hot_zoneV->SetMedium(sensitive_medium);
    }
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //media for frames
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(frame_medium) {
        vertical_outer_frameV->SetMedium(frame_medium);
        horizontal_outer_frameV->SetMedium(frame_medium);
        vertical_inner_frameV->SetMedium(frame_medium);
        horizontal_inner_frameV->SetMedium(frame_medium);
        composite_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");

    moduleV->SetTransparency(20);

    //hot zone position in the module
    TGeoTranslation *hot_zone_trans = new TGeoTranslation(-XModuleSize_StationPrototype*0.5 + XHotZoneSize_StationPrototype*0.5, -YModuleSize_StationPrototype*0.5 + YHotZoneSize_StationPrototype*0.5, 0);

    //module positions in the station
    TGeoCombiTrans *module_left_position = new TGeoCombiTrans();
    //module_left_position->ReflectX(true);
    module_left_position->SetTranslation(XModuleSize_StationPrototype*0.5-XIntersectionSensModule*0.5, 0.0, dZSensitiveVolume);

    TGeoCombiTrans *module_right_position = new TGeoCombiTrans();
    module_right_position->ReflectX(true);
    module_right_position->SetTranslation(-XModuleSize_StationPrototype*0.5+XIntersectionSensModule*0.5, 0.0, 0.0);

    //frame part positions in the frame
    TGeoCombiTrans *left_outer_frame_position = new TGeoCombiTrans();
    left_outer_frame_position->SetTranslation(XModuleSize_StationPrototype*0.5+dXOuterFrame*0.5, (dYOuterFrame-dYInnerFrame)*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);

    TGeoCombiTrans *top_outer_frame_position = new TGeoCombiTrans();
    top_outer_frame_position->SetTranslation(-dXInnerFrame*0.5, YModuleSize_StationPrototype*0.5+dYOuterFrame*0.5, ZModuleSize*0.5);

    TGeoCombiTrans *right_inner_frame_position = new TGeoCombiTrans();
    right_inner_frame_position->SetTranslation(-XModuleSize_StationPrototype*0.5-dXInnerFrame*0.5, 0, ZModuleSize*0.5);

    TGeoCombiTrans *bottom_inner_frame_position = new TGeoCombiTrans();
    bottom_inner_frame_position->SetTranslation(-dXInnerFrame*0.5, -YModuleSize_StationPrototype*0.5-dYInnerFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);

    //frame positions in the station
    TGeoCombiTrans *comp_frame_left_position = new TGeoCombiTrans();
    //comp_frame_left_position->ReflectX(true);
    comp_frame_left_position->SetTranslation(module_left_position->GetTranslation());

    TGeoCombiTrans *comp_frame_right_position = new TGeoCombiTrans();
    comp_frame_right_position->ReflectX(true);
    comp_frame_right_position->ReflectZ(true);
    comp_frame_right_position->SetTranslation(module_right_position->GetTranslation());

    //station assembling
    moduleV->AddNode(hot_zoneV, 0, hot_zone_trans);

    composite_frameV->AddNode(vertical_outer_frameV, 0, left_outer_frame_position);
    composite_frameV->AddNode(horizontal_outer_frameV, 0, top_outer_frame_position);
    composite_frameV->AddNode(vertical_inner_frameV, 0, right_inner_frame_position);
    composite_frameV->AddNode(horizontal_inner_frameV, 0, bottom_inner_frame_position);

    stationA->AddNode(moduleV, 0, module_left_position);
    stationA->AddNode(moduleV, 1, module_right_position);

    stationA->AddNode(composite_frameV, 0, comp_frame_left_position);
    stationA->AddNode(composite_frameV, 1, comp_frame_right_position);

    //Station position
    Double_t station_shift = dZSensitiveVolume*0.5;
    TGeoTranslation *station_pos = new TGeoTranslation(x_position, y_position, z_position+station_shift);

    mother_volume->AddNode(stationA, 0, station_pos);
}

void CreateStation_One163x45Plane(TGeoVolume* mother_volume, TString station_name, Double_t x_position, Double_t y_position, Double_t z_position, Double_t hole_radius_with_frame, Bool_t IsInverted) {
    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    Double_t XOuterZoneSize = OuterZoneSizes_Station163x45[0];
    Double_t YOuterZoneSize = OuterZoneSizes_Station163x45[1];

    Double_t XRectangleZoneSize = HotZoneSizes_rectangle_Station163x45[0];
    Double_t YRectangleZoneSize = HotZoneSizes_rectangle_Station163x45[1];

    Double_t XMinSlopeZoneSize = HotZoneSizes_slope_Station163x45[0];
    Double_t XMaxSlopeZoneSize = HotZoneSizes_slope_Station163x45[1];
    Double_t YSlopeZoneSize = HotZoneSizes_slope_Station163x45[2];

    //module shape
    TString part_module_name = "part_moduleS_" + station_name;
    TGeoShape *part_moduleS = new TGeoBBox(part_module_name, XOuterZoneSize*0.5, YOuterZoneSize*0.5, dZSensitiveVolume*0.5);

    //part hot zone shapes (without the hole)
    TString part_rectangle_inner_zone_name = "part_rectangle_inner_zoneS_" + station_name;
    TGeoShape *part_rectangle_inner_zoneS = new TGeoBBox(part_rectangle_inner_zone_name, XRectangleZoneSize*0.5, YRectangleZoneSize*0.5, dZSensitiveVolume*0.5);

    TString part_slope_inner_zone_name = "part_slope_inner_zoneS_" + station_name;
    TGeoXtru *part_slope_inner_zoneS = new TGeoXtru(2); //origin centered
    part_slope_inner_zoneS->SetName(part_slope_inner_zone_name);
        Double_t x_pos[4] = {XMaxSlopeZoneSize*0.5, XMaxSlopeZoneSize*0.5 - (XMaxSlopeZoneSize-XMinSlopeZoneSize), -XMaxSlopeZoneSize*0.5, -XMaxSlopeZoneSize*0.5};
        Double_t y_pos[4] = {-YSlopeZoneSize*0.5, YSlopeZoneSize*0.5, YSlopeZoneSize*0.5, -YSlopeZoneSize*0.5};
    part_slope_inner_zoneS->DefinePolygon(4, x_pos, y_pos);
    part_slope_inner_zoneS->DefineSection(0, -dZSensitiveVolume*0.5);
    part_slope_inner_zoneS->DefineSection(1, dZSensitiveVolume*0.5);

    //hole shape
    TString hole_name = "holeS_" + station_name;
    //TGeoShape *holeS = new TGeoTubeSeg(hole_name, 0.0, hole_radius_with_frame, dZSensitiveVolume*0.5 + 0.1, -1.0, 91.0);
    TGeoShape *holeS = new TGeoTube(hole_name, 0.0, hole_radius_with_frame, dZSensitiveVolume*0.5+0.01);

    //hole positions in the module and zones to extract the hole
    TString hole_module_trans_name = "hole_module_trans_name_" + station_name;
    TGeoTranslation *hole_module_trans = new TGeoTranslation(hole_module_trans_name, -XOuterZoneSize*0.5, -YOuterZoneSize*0.5, 0);
    hole_module_trans->RegisterYourself();

    TString hole_rectangle_inner_zone_trans_name = "hole_rectangle_inner_zone_trans_name_" + station_name;
    TGeoTranslation *hole_rectangle_inner_zone_trans = new TGeoTranslation(hole_rectangle_inner_zone_trans_name, -XRectangleZoneSize*0.5, -YRectangleZoneSize*0.5, 0);
    hole_rectangle_inner_zone_trans->RegisterYourself();

    TString hole_slope_inner_zone_trans_name = "hole_slope_inner_zone_trans_name_" + station_name;
    TGeoTranslation *hole_slope_inner_zone_trans = new TGeoTranslation(hole_slope_inner_zone_trans_name, -XMaxSlopeZoneSize*0.5, -YSlopeZoneSize*0.5, 0);
    hole_slope_inner_zone_trans->RegisterYourself();

    //module and hot zone shapes
    TString bool_expression_module = part_module_name + " - " + hole_name + ":" + hole_module_trans_name;
    TGeoCompositeShape *moduleS = new TGeoCompositeShape(bool_expression_module);

    TString bool_expression_rectangle_inner_zone = part_rectangle_inner_zone_name + " - " + hole_name + ":" + hole_rectangle_inner_zone_trans_name;
    TGeoCompositeShape *rectangle_inner_zoneS = new TGeoCompositeShape(bool_expression_rectangle_inner_zone);

    TString bool_expression_slope_inner_zone = part_slope_inner_zone_name + " - " + hole_name + ":" + hole_slope_inner_zone_trans_name;
    TGeoCompositeShape *slope_inner_zoneS = new TGeoCompositeShape(bool_expression_slope_inner_zone);

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dXOuterFrame*0.5, (YOuterZoneSize+dYInnerFrame+dYOuterFrame)*0.5, ZModuleSize*0.5);
    TGeoShape *horizontal_long_frameS = new TGeoBBox("horizontal_long_frameS", XOuterZoneSize, dYOuterFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);
    TGeoShape *horizontal_short_frameS = new TGeoBBox("horizontal_short_frameS", (XOuterZoneSize-hole_radius_with_frame+dXInnerFrame)*0.5, dYInnerFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5);
    TGeoShape *circle_inner_frameS = new TGeoTubeSeg("circle_inner_frameS", hole_radius_with_frame-dXInnerFrame, hole_radius_with_frame, ZModuleSize*0.5, 0., -180.0);

    //Volumes
    TGeoVolume *module_with_slope_inner_zoneV = new TGeoVolume("module_Sensor", moduleS);
    TGeoVolume *module_with_rect_inner_zoneV = new TGeoVolume("module_Sensor", moduleS);

    TGeoVolume *rectangle_inner_zoneV = new TGeoVolume("inner_zone_Sensor", rectangle_inner_zoneS);
    TGeoVolume *slope_inner_zoneV = new TGeoVolume("inner_zone_Sensor", slope_inner_zoneS);

    TGeoVolume *vertical_frameV = new TGeoVolume("vertical_frame", vertical_frameS);
    TGeoVolume *horizontal_long_frameV = new TGeoVolume("horizontal_long_frame", horizontal_long_frameS);
    TGeoVolume *horizontal_short_frameV = new TGeoVolume("horizontal_short_frame", horizontal_short_frameS);
    TGeoVolume *circle_inner_frameV = new TGeoVolume("circle_inner_frame", circle_inner_frameS);
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly("frame");

    //media for sensitive volumes
    TGeoMedium *sensitive_medium = pMedArCO27030; //set medium
    if(sensitive_medium) {
        module_with_slope_inner_zoneV->SetMedium(sensitive_medium);
        module_with_rect_inner_zoneV->SetMedium(sensitive_medium);
        rectangle_inner_zoneV->SetMedium(sensitive_medium);
        slope_inner_zoneV->SetMedium(sensitive_medium);
    }
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //media for frames
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(frame_medium) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_long_frameV->SetMedium(frame_medium);
        horizontal_short_frameV->SetMedium(frame_medium);
        circle_inner_frameV->SetMedium(frame_medium);
        composite_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //volume visual properties
    module_with_slope_inner_zoneV->SetTransparency(20);
    module_with_rect_inner_zoneV->SetTransparency(20);
    module_with_slope_inner_zoneV->SetLineColor(TColor::GetColor("#ff47ca"));
    module_with_rect_inner_zoneV->SetLineColor(TColor::GetColor("#ff47ca"));
    rectangle_inner_zoneV->SetLineColor(TColor::GetColor("#e0b0ff"));
    slope_inner_zoneV->SetLineColor(TColor::GetColor("#e0b0ff"));
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_long_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_short_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    circle_inner_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //hot zone positions in modules
    TGeoTranslation *slope_inner_zone_position = new TGeoTranslation(-XOuterZoneSize*0.5+XMaxSlopeZoneSize*0.5, -YOuterZoneSize*0.5 + YSlopeZoneSize*0.5, 0);
    TGeoTranslation *rect_inner_zone_position = new TGeoTranslation(-XOuterZoneSize*0.5+XRectangleZoneSize*0.5, -YOuterZoneSize*0.5 + YRectangleZoneSize*0.5, 0);

    //module positions in the station
    TGeoCombiTrans *left_module_position = new TGeoCombiTrans();
    left_module_position->SetTranslation(XOuterZoneSize*0.5, 0.0, 0.0);

    TGeoCombiTrans *right_module_position = new TGeoCombiTrans();
    right_module_position->RotateY(180.0);
    right_module_position->SetTranslation(-XOuterZoneSize*0.5, 0.0, 0.0);


    //frame part positions in the frame
    TGeoTranslation *left_frame_position = new TGeoTranslation(XOuterZoneSize+dXOuterFrame*0.5, (dYOuterFrame-dYInnerFrame)*0.5, ZModuleSize*0.5);
    TGeoTranslation *right_frame_position = new TGeoTranslation(-(XOuterZoneSize+dXOuterFrame*0.5), (dYOuterFrame-dYInnerFrame)*0.5, ZModuleSize*0.5);
    TGeoTranslation *top_long_frame_position = new TGeoTranslation(0, YOuterZoneSize*0.5+dYOuterFrame*0.5, (ZModuleSize-dZSensitiveVolume)*0.5 + dZSensitiveVolume);TGeoTranslation *left_top_short_frame_position = new TGeoTranslation((XOuterZoneSize-hole_radius_with_frame-dXInnerFrame)*0.5 + hole_radius_with_frame, -dYInnerFrame*0.5-YOuterZoneSize*0.5, (ZModuleSize-dZSensitiveVolume)*0.5 + dZSensitiveVolume);
    TGeoTranslation *right_bottom_short_frame_position = new TGeoTranslation(-((XOuterZoneSize-hole_radius_with_frame-dXInnerFrame)*0.5 + hole_radius_with_frame), -dYInnerFrame*0.5-YOuterZoneSize*0.5, (ZModuleSize-dZSensitiveVolume)*0.5 + dZSensitiveVolume);
    TGeoTranslation *left_bottom_short_frame_position = new TGeoTranslation(((XOuterZoneSize-hole_radius_with_frame-dXInnerFrame)*0.5 + hole_radius_with_frame), -dYInnerFrame*0.5-YOuterZoneSize*0.5, (ZModuleSize-dZSensitiveVolume)*0.5 + dZSensitiveVolume);

    //frame positions in the station
    TGeoCombiTrans *upper_frame_trans = new TGeoCombiTrans();
    upper_frame_trans->SetTranslation(0.0, 0.0, -dZSensitiveVolume*0.5);

    //station assembling
    module_with_slope_inner_zoneV->AddNode(slope_inner_zoneV, 0, slope_inner_zone_position);
    module_with_rect_inner_zoneV->AddNode(rectangle_inner_zoneV, 0, rect_inner_zone_position);

    if(IsInverted) {
        stationA->AddNode(module_with_rect_inner_zoneV, 0, left_module_position);
        stationA->AddNode(module_with_slope_inner_zoneV, 1, right_module_position);
    }
    else {
        stationA->AddNode(module_with_slope_inner_zoneV, 0, left_module_position);
        stationA->AddNode(module_with_rect_inner_zoneV, 1, right_module_position);
    }


    //frames
    composite_frameV->AddNode(vertical_frameV, 0, left_frame_position);
    composite_frameV->AddNode(vertical_frameV, 1, right_frame_position);
    composite_frameV->AddNode(horizontal_long_frameV, 0, top_long_frame_position);
    composite_frameV->AddNode(horizontal_short_frameV, 0, left_bottom_short_frame_position);
    composite_frameV->AddNode(horizontal_short_frameV, 1, right_bottom_short_frame_position);
    composite_frameV->AddNode(circle_inner_frameV, 0, new TGeoTranslation(0, -YOuterZoneSize*0.5, ZModuleSize*0.5));

    stationA->AddNode(composite_frameV, 0, upper_frame_trans);

    //Station position
    Double_t station_shift = 0.0;
    station_shift = dZSensitiveVolume*0.5;
    TGeoTranslation *station_pos = new TGeoTranslation(x_position, y_position, z_position+station_shift);

    mother_volume->AddNode(stationA, 0, station_pos);
}

//------------------------------------------------------------------------------
