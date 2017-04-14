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
const Int_t nStations = 5;

//Station Parameters
/*
const Double_t XStationPositions[nStations] = {-2.2, 3.0, -1.8, 3.5, -1.1};
const Double_t YStationPositions[nStations] = {0.0, 0.0, 0.0, 0.0, 0.0};
const Double_t ZStationPositions[nStations] = {30.0, 45.0, 60.0, 80.0, 100.0}; // z pos is the first surface of the first sens. volume in module !!!
*/

Double_t XStationPositions[nStations];
    XStationPositions[0] = BmnGemStripPositions_FirstConfigShort::XStationPositions[0];
    XStationPositions[1] = BmnGemStripPositions_FirstConfigShort::XStationPositions[1];
    XStationPositions[2] = BmnGemStripPositions_FirstConfigShort::XStationPositions[2];
    XStationPositions[3] = BmnGemStripPositions_FirstConfigShort::XStationPositions[3];
    XStationPositions[4] = BmnGemStripPositions_FirstConfigShort::XStationPositions[4];

Double_t YStationPositions[nStations];
    YStationPositions[0] = BmnGemStripPositions_FirstConfigShort::YStationPositions[0];
    YStationPositions[1] = BmnGemStripPositions_FirstConfigShort::YStationPositions[1];
    YStationPositions[2] = BmnGemStripPositions_FirstConfigShort::YStationPositions[2];
    YStationPositions[3] = BmnGemStripPositions_FirstConfigShort::YStationPositions[3];
    YStationPositions[4] = BmnGemStripPositions_FirstConfigShort::YStationPositions[4];

Double_t ZStationPositions[nStations];
    ZStationPositions[0] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[0];
    ZStationPositions[1] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[1];
    ZStationPositions[2] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[2];
    ZStationPositions[3] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[3];
    ZStationPositions[4] = BmnGemStripPositions_FirstConfigShort::ZStationPositions[4];


//GEM plane sizes (66x41 type)
const Double_t XModuleSize_StationPrototype = 66.0;
const Double_t YModuleSize_StationPrototype = 41.0;

const Double_t XHotZoneSize_StationPrototype = 15.0;
const Double_t YHotZoneSize_StationPrototype = 10.0;


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

void create_rootgeom_GEMS_1stConfigShort() {

    // Load the necessary FairRoot libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "1stConfigShort";
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
    CreateStation_One66x41Plane(GEMS, "station3", XStationPositions[3], YStationPositions[3], ZStationPositions[3], kTRUE);
    CreateStation_One66x41Plane(GEMS, "station4", XStationPositions[4], YStationPositions[4], ZStationPositions[4], kFALSE);

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

//------------------------------------------------------------------------------