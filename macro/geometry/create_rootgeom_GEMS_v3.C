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
const Int_t nStations = 12;

//Station Parameters
/*
const Double_t XStationPositions[nStations] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
const Double_t YStationPositions[nStations] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
const Double_t ZStationPositions[nStations] = {30., 45., 60., 80., 100., 130., 160., 190., 230., 270., 315., 360.};
*/

Double_t XStationPositions[nStations];
    XStationPositions[0] = BmnGemStripPositions_FullConfig::XStationPositions[0];
    XStationPositions[1] = BmnGemStripPositions_FullConfig::XStationPositions[1];
    XStationPositions[2] = BmnGemStripPositions_FullConfig::XStationPositions[2];
    XStationPositions[3] = BmnGemStripPositions_FullConfig::XStationPositions[3];
    XStationPositions[4] = BmnGemStripPositions_FullConfig::XStationPositions[4];
    XStationPositions[5] = BmnGemStripPositions_FullConfig::XStationPositions[5];
    XStationPositions[6] = BmnGemStripPositions_FullConfig::XStationPositions[6];
    XStationPositions[7] = BmnGemStripPositions_FullConfig::XStationPositions[7];
    XStationPositions[8] = BmnGemStripPositions_FullConfig::XStationPositions[8];
    XStationPositions[9] = BmnGemStripPositions_FullConfig::XStationPositions[9];
    XStationPositions[10] = BmnGemStripPositions_FullConfig::XStationPositions[10];
    XStationPositions[11] = BmnGemStripPositions_FullConfig::XStationPositions[11];

Double_t YStationPositions[nStations];
    YStationPositions[0] = BmnGemStripPositions_FullConfig::YStationPositions[0];
    YStationPositions[1] = BmnGemStripPositions_FullConfig::YStationPositions[1];
    YStationPositions[2] = BmnGemStripPositions_FullConfig::YStationPositions[2];
    YStationPositions[3] = BmnGemStripPositions_FullConfig::YStationPositions[3];
    YStationPositions[4] = BmnGemStripPositions_FullConfig::YStationPositions[4];
    YStationPositions[5] = BmnGemStripPositions_FullConfig::YStationPositions[5];
    YStationPositions[6] = BmnGemStripPositions_FullConfig::YStationPositions[6];
    YStationPositions[7] = BmnGemStripPositions_FullConfig::YStationPositions[7];
    YStationPositions[8] = BmnGemStripPositions_FullConfig::YStationPositions[8];
    YStationPositions[9] = BmnGemStripPositions_FullConfig::YStationPositions[9];
    YStationPositions[10] = BmnGemStripPositions_FullConfig::YStationPositions[10];
    YStationPositions[11] = BmnGemStripPositions_FullConfig::YStationPositions[11];

Double_t ZStationPositions[nStations];
    ZStationPositions[0] = BmnGemStripPositions_FullConfig::ZStationPositions[0];
    ZStationPositions[1] = BmnGemStripPositions_FullConfig::ZStationPositions[1];
    ZStationPositions[2] = BmnGemStripPositions_FullConfig::ZStationPositions[2];
    ZStationPositions[3] = BmnGemStripPositions_FullConfig::ZStationPositions[3];
    ZStationPositions[4] = BmnGemStripPositions_FullConfig::ZStationPositions[4];
    ZStationPositions[5] = BmnGemStripPositions_FullConfig::ZStationPositions[5];
    ZStationPositions[6] = BmnGemStripPositions_FullConfig::ZStationPositions[6];
    ZStationPositions[7] = BmnGemStripPositions_FullConfig::ZStationPositions[7];
    ZStationPositions[8] = BmnGemStripPositions_FullConfig::ZStationPositions[8];
    ZStationPositions[9] = BmnGemStripPositions_FullConfig::ZStationPositions[9];
    ZStationPositions[10] = BmnGemStripPositions_FullConfig::ZStationPositions[10];
    ZStationPositions[11] = BmnGemStripPositions_FullConfig::ZStationPositions[11];

//Beam Pipe Hole Parameters
const Double_t BeamPipeMinRadius = 2.5; // radius of the hole in first GEM-station
const Double_t BeamPipeMaxRadius = 5.0; // radius of the hole in last GEM-station
      Double_t BeamPipeRadius[nStations];

//Zone sizes (station 0 and 1) - half  (in the one quandrant)
const Double_t  XZoneSizes_Station0[4] = {30.0, 16.0, 12.0, 8.0}; // x sizes for 1, 2, 3, 4 zones
const Double_t  YZoneSizes_Station0[4] = {30.0, 12.0, 7.0, 3.0}; // y sizes for 1, 2, 3, 4 zones

//Zone sizes (station 2 and 3) - half  (in the one quandrant)
const Double_t  XZoneSizes_Station2[4] = {40.0, 20.0, 15.0, 10.0}; // x sizes for 1, 2, 3, 4 zones
const Double_t  YZoneSizes_Station2[4] = {40.0, 15.0, 9.0, 4.0}; // y sizes for 1, 2, 3, 4 zones

//Prototype station sizes (station 4)
const Double_t XModuleSize_StationPrototype = 66.0;
const Double_t YModuleSize_StationPrototype = 41.0;

const Double_t XHotZoneSize_StationPrototype = 15.0;
const Double_t YHotZoneSize_StationPrototype = 10.0;

//Two modules station sizes (station 5, 6, 7)
const Double_t OuterZoneSizes_Station5[2] = {163.2/2, 45.0}; // {xsize, ysize}
const Double_t HotZoneSizes_rectangle_Station5[2] = {40.0, 15.0}; // {xsize, ysize}
const Double_t HotZoneSizes_slope_Station5[3] = {37.99, 42.01, 15.0}; // {xmin_size, xmax_size, ysize}

//Two modules station sizes (station 8, 9, 10, 11)
const Double_t OuterZoneSizes_Station8[2] = {200.0/2, 45.0}; // {xsize, ysize}
const Double_t HotZoneSizes_rectangle_Station8[2] = {50.0, 20.0}; // {xsize, ysize}
const Double_t HotZoneSizes_slope_Station8[3] = {47.14, 52.5, 20.0}; // {xmin_size, xmax_size, ysize} //ЗДЕСЬ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


//Module Parameters
const Int_t    nModulesInStation = 4;
const Double_t dXOuterFrame = 5.0;
const Double_t dXInnerFrame = 1.7;
const Double_t dYOuterFrame = 3.75;
const Double_t dYInnerFrame = 1.7;
const Double_t dZSensitiveVolume = 0.9;
const Double_t ZModuleSize = 2.5;

      Double_t XModuleSize[nStations];
      Double_t YModuleSize[nStations];

const Double_t XSizeSpacer = 10.0;
const Double_t YSizeSpacer = 10.0;
const Double_t SpacerWidth = 0.2;
const Double_t SpacerThickness = dZSensitiveVolume;

//Position of GEM-Detector
const Bool_t StationCenterPosition = kFALSE;
const Double_t ZDisplacementOfDetector = (ZModuleSize*2)/2;

//Add spacers or not?
const Bool_t WithSpacers = kTRUE;

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

void CalculateParameters() {
    //Calculate the Beam Pipe Radius for each GEM-stations
    Double_t BeamPipeAngle = (ATan((BeamPipeMaxRadius-BeamPipeMinRadius) / (ZStationPositions[nStations-1] - ZStationPositions[0])))*180/Pi();
    BeamPipeRadius[0] = BeamPipeMinRadius;
    BeamPipeRadius[nStations-1] = BeamPipeMaxRadius;
    Double_t delta_const = BeamPipeMaxRadius - BeamPipeMinRadius;
    for(UInt_t iStation = 1; iStation < nStations-1; iStation++) {
        BeamPipeRadius[iStation] = delta_const + (ZStationPositions[iStation] - ZStationPositions[0])*Tan((BeamPipeAngle*Pi()/180));
    }
    cout << "Beam Pipe Angle = " <<  BeamPipeAngle << " deg" << "\n";
    cout << "Beam Pipe Radiuses:" << "\n";
    for(UInt_t iStation = 0; iStation < nStations; iStation++) {
        cout << "     Station["<<iStation<<"] = " << BeamPipeRadius[iStation] << " cm" << "\n";
    }
    cout << "----------------------------" <<"\n";

}

void create_rootgeom_GEMS_v3() {

    CalculateParameters();

    // Load the necessary FairRoot libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "v3";
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
    CreateStation_Type4Zones(GEMS, "station0", XStationPositions[0], YStationPositions[0], ZStationPositions[0], XZoneSizes_Station0, YZoneSizes_Station0, BeamPipeRadius[0]+dXInnerFrame);
    CreateStation_Type4Zones(GEMS, "station1", XStationPositions[1], YStationPositions[1], ZStationPositions[1], XZoneSizes_Station0, YZoneSizes_Station0, BeamPipeRadius[1]+dXInnerFrame);
    CreateStation_Type4Zones(GEMS, "station2", XStationPositions[2], YStationPositions[2], ZStationPositions[2], XZoneSizes_Station2, YZoneSizes_Station2, BeamPipeRadius[2]+dXInnerFrame);
    CreateStation_Type4Zones(GEMS, "station3", XStationPositions[3], YStationPositions[3], ZStationPositions[3], XZoneSizes_Station2, YZoneSizes_Station2, BeamPipeRadius[3]+dXInnerFrame);

    CreateStationTypePrototype(GEMS, "station4", XStationPositions[4], YStationPositions[4], ZStationPositions[4], BeamPipeRadius[4]);

    CreateStation_TypeTwoModules(GEMS, "station5", XStationPositions[5], YStationPositions[5], ZStationPositions[5], BeamPipeRadius[5]+dXInnerFrame, kFALSE, OuterZoneSizes_Station5, HotZoneSizes_rectangle_Station5, HotZoneSizes_slope_Station5);
    CreateStation_TypeTwoModules(GEMS, "station6", XStationPositions[6], YStationPositions[6], ZStationPositions[6], BeamPipeRadius[6]+dXInnerFrame, kTRUE, OuterZoneSizes_Station5, HotZoneSizes_rectangle_Station5, HotZoneSizes_slope_Station5);
    CreateStation_TypeTwoModules(GEMS, "station7", XStationPositions[7], YStationPositions[7], ZStationPositions[7], BeamPipeRadius[7]+dXInnerFrame, kFALSE, OuterZoneSizes_Station5, HotZoneSizes_rectangle_Station5, HotZoneSizes_slope_Station5);

    CreateStation_TypeTwoModules(GEMS, "station8", XStationPositions[8], YStationPositions[8], ZStationPositions[8], BeamPipeRadius[8]+dXInnerFrame, kTRUE, OuterZoneSizes_Station8, HotZoneSizes_rectangle_Station8, HotZoneSizes_slope_Station8);
    CreateStation_TypeTwoModules(GEMS, "station9", XStationPositions[9], YStationPositions[9], ZStationPositions[9], BeamPipeRadius[9]+dXInnerFrame, kFALSE, OuterZoneSizes_Station8, HotZoneSizes_rectangle_Station8, HotZoneSizes_slope_Station8);
    CreateStation_TypeTwoModules(GEMS, "station10", XStationPositions[10], YStationPositions[10], ZStationPositions[10], BeamPipeRadius[10]+dXInnerFrame, kTRUE, OuterZoneSizes_Station8, HotZoneSizes_rectangle_Station8, HotZoneSizes_slope_Station8);
    CreateStation_TypeTwoModules(GEMS, "station11", XStationPositions[11], YStationPositions[11], ZStationPositions[11], BeamPipeRadius[11]+dXInnerFrame, kFALSE, OuterZoneSizes_Station8, HotZoneSizes_rectangle_Station8, HotZoneSizes_slope_Station8);



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
void CreateStation_Type4Zones(TGeoVolume* mother_volume, TString station_name, Double_t x_position, Double_t y_position, Double_t z_position, Double_t *x_zone_sizes, Double_t *y_zone_sizes, Double_t hole_radius) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    //zone sizes (in the one quadrant)
    Double_t xHalfSizeZone1 = x_zone_sizes[0];
    Double_t yHalfSizeZone1 = y_zone_sizes[0];

    Double_t xHalfSizeZone2 = x_zone_sizes[1];
    Double_t yHalfSizeZone2 = y_zone_sizes[1];

    Double_t xHalfSizeZone3 = x_zone_sizes[2];
    Double_t yHalfSizeZone3 = y_zone_sizes[2];

    Double_t xHalfSizeZone4 = x_zone_sizes[3];
    Double_t yHalfSizeZone4 = y_zone_sizes[3];

    //zone part sizes
    Double_t vpart_xsize_zone1 = xHalfSizeZone1 - xHalfSizeZone2;
    Double_t vpart_ysize_zone1 = yHalfSizeZone1;
    Double_t hpart_xsize_zone1 = xHalfSizeZone2;
    Double_t hpart_ysize_zone1 = yHalfSizeZone1 - yHalfSizeZone2;

    Double_t vpart_xsize_zone2 = xHalfSizeZone2 - xHalfSizeZone3;
    Double_t vpart_ysize_zone2 = yHalfSizeZone2;
    Double_t hpart_xsize_zone2 = xHalfSizeZone3;
    Double_t hpart_ysize_zone2 = yHalfSizeZone2 - yHalfSizeZone3;

    Double_t vpart_xsize_zone3 = xHalfSizeZone3 - xHalfSizeZone4;
    Double_t vpart_ysize_zone3 = yHalfSizeZone3;
    Double_t hpart_xsize_zone3 = xHalfSizeZone4;
    Double_t hpart_ysize_zone3 = yHalfSizeZone3 - yHalfSizeZone4;

    Double_t part_xsize_zone4 = xHalfSizeZone4;
    Double_t part_ysize_zone4 = yHalfSizeZone4;

    //zone1 shape
    TString vpart_zone1S_name = Form("vpart_zone1_%s", station_name.Data());
    TString hpart_zone1S_name = Form("hpart_zone1_%s", station_name.Data());
    TString hole_zone1S_name = Form("hole_zone1_%s", station_name.Data());
    TGeoShape *vpart_zone1S = new TGeoBBox(vpart_zone1S_name, vpart_xsize_zone1/2, vpart_ysize_zone1/2, dZSensitiveVolume/2);
    TGeoShape *hpart_zone1S = new TGeoBBox(hpart_zone1S_name, hpart_xsize_zone1/2, hpart_ysize_zone1/2, dZSensitiveVolume/2);
    //TGeoShape *hole_zone1S = new TGeoTubeSeg(hole_zone1S_name, 0.0, hole_radius, dZSensitiveVolume/2 + 0.1, -1, 91);
    TGeoShape *hole_zone1S = new TGeoTube(hole_zone1S_name, 0.0, hole_radius, dZSensitiveVolume/2+0.01);
    TGeoTranslation *vpart_zone1_trans = new TGeoTranslation(vpart_xsize_zone1/2 + xHalfSizeZone2, yHalfSizeZone1/2, 0);
    TGeoTranslation *hpart_zone1_trans = new TGeoTranslation(hpart_xsize_zone1/2, hpart_ysize_zone1/2 + yHalfSizeZone2, 0);
    TString vpart_zone1S_trans_name = Form("vpart_zone1_trans_%s", station_name.Data());
    TString hpart_zone1S_trans_name = Form("hpart_zone1_trans_%s", station_name.Data());
    vpart_zone1_trans->SetName(vpart_zone1S_trans_name);
    hpart_zone1_trans->SetName(hpart_zone1S_trans_name);
    vpart_zone1_trans->RegisterYourself();
    hpart_zone1_trans->RegisterYourself();
    TString bool_expression_zone1 = vpart_zone1S_name + ":" + vpart_zone1S_trans_name + " + " + hpart_zone1S_name + ":" + hpart_zone1S_trans_name;
    if(xHalfSizeZone2 < hole_radius || yHalfSizeZone2 < hole_radius)
        bool_expression_zone1 += " - " + hole_zone1S_name;
    TGeoCompositeShape *zone1S = new TGeoCompositeShape(bool_expression_zone1);

    //zone2 shape
    TString vpart_zone2S_name = Form("vpart_zone2_%s", station_name.Data());
    TString hpart_zone2S_name = Form("hpart_zone2_%s", station_name.Data());
    TString hole_zone2S_name = Form("hole_zone2_%s", station_name.Data());
    TGeoShape *vpart_zone2S = new TGeoBBox(vpart_zone2S_name, vpart_xsize_zone2/2, vpart_ysize_zone2/2, dZSensitiveVolume/2);
    TGeoShape *hpart_zone2S = new TGeoBBox(hpart_zone2S_name, hpart_xsize_zone2/2, hpart_ysize_zone2/2, dZSensitiveVolume/2);
    //TGeoShape *hole_zone2S = new TGeoTubeSeg(hole_zone2S_name, 0.0, hole_radius, dZSensitiveVolume/2 + 0.1, -1, 91);
    TGeoShape *hole_zone2S = new TGeoTube(hole_zone2S_name, 0.0, hole_radius, dZSensitiveVolume/2+0.01);
    TGeoTranslation *vpart_zone2_trans = new TGeoTranslation(vpart_xsize_zone2/2 + xHalfSizeZone3, yHalfSizeZone2/2, 0);
    TGeoTranslation *hpart_zone2_trans = new TGeoTranslation(hpart_xsize_zone2/2, hpart_ysize_zone2/2 + yHalfSizeZone3, 0);
    TString vpart_zone2S_trans_name = Form("vpart_zone2_trans_%s", station_name.Data());
    TString hpart_zone2S_trans_name = Form("hpart_zone2_trans_%s", station_name.Data());
    vpart_zone2_trans->SetName(vpart_zone2S_trans_name);
    hpart_zone2_trans->SetName(hpart_zone2S_trans_name);
    vpart_zone2_trans->RegisterYourself();
    hpart_zone2_trans->RegisterYourself();
    TString bool_expression_zone2 = vpart_zone2S_name + ":" + vpart_zone2S_trans_name + " + " + hpart_zone2S_name + ":" + hpart_zone2S_trans_name;
    if(xHalfSizeZone3 < hole_radius || yHalfSizeZone3 < hole_radius)
        bool_expression_zone2 += " - " + hole_zone2S_name;
    TGeoCompositeShape *zone2S = new TGeoCompositeShape(bool_expression_zone2);

    //zone3 shape
    TString vpart_zone3S_name = Form("vpart_zone3_%s", station_name.Data());
    TString hpart_zone3S_name = Form("hpart_zone3_%s", station_name.Data());
    TString hole_zone3S_name = Form("hole_zone3_%s", station_name.Data());
    TGeoShape *vpart_zone3S = new TGeoBBox(vpart_zone3S_name, vpart_xsize_zone3/2, vpart_ysize_zone3/2, dZSensitiveVolume/2);
    TGeoShape *hpart_zone3S = new TGeoBBox(hpart_zone3S_name, hpart_xsize_zone3/2, hpart_ysize_zone3/2, dZSensitiveVolume/2);
    //TGeoShape *hole_zone3S = new TGeoTubeSeg(hole_zone3S_name, 0.0, hole_radius, dZSensitiveVolume/2 + 0.1, -1, 91);
    TGeoShape *hole_zone3S = new TGeoTube(hole_zone3S_name, 0.0, hole_radius, dZSensitiveVolume/2+0.01);
    TGeoTranslation *vpart_zone3_trans = new TGeoTranslation(vpart_xsize_zone3/2 + xHalfSizeZone4, yHalfSizeZone3/2, 0);
    TGeoTranslation *hpart_zone3_trans = new TGeoTranslation(hpart_xsize_zone3/2, hpart_ysize_zone3/2 + yHalfSizeZone4, 0);
    TString vpart_zone3S_trans_name = Form("vpart_zone3_trans_%s", station_name.Data());
    TString hpart_zone3S_trans_name = Form("hpart_zone3_trans_%s", station_name.Data());
    vpart_zone3_trans->SetName(vpart_zone3S_trans_name);
    hpart_zone3_trans->SetName(hpart_zone3S_trans_name);
    vpart_zone3_trans->RegisterYourself();
    hpart_zone3_trans->RegisterYourself();
    TString bool_expression_zone3 = vpart_zone3S_name + ":" + vpart_zone3S_trans_name + " + " + hpart_zone3S_name + ":" + hpart_zone3S_trans_name;
    if(xHalfSizeZone4 < hole_radius || yHalfSizeZone4 < hole_radius)
        bool_expression_zone3 += " - " + hole_zone3S_name;
    TGeoCompositeShape *zone3S = new TGeoCompositeShape(bool_expression_zone3);

    //zone4 shape
    TString part_zone4S_name = Form("part_zone4_%s", station_name.Data());
    TString hole_zone4S_name = Form("hole_zone4_%s", station_name.Data());
    TGeoShape *part_zone4S = new TGeoBBox(part_zone4S_name, part_xsize_zone4/2, part_ysize_zone4/2, dZSensitiveVolume/2);
    //TGeoShape *hole_zone4S = new TGeoTubeSeg(hole_zone4S_name, 0.0, hole_radius, dZSensitiveVolume/2 + 0.1, -1, 91);
    TGeoShape *hole_zone4S = new TGeoTube(hole_zone4S_name, 0.0, hole_radius, dZSensitiveVolume/2+0.01);
    TGeoTranslation *part_zone4_trans = new TGeoTranslation(part_xsize_zone4/2, part_ysize_zone4/2, 0);
    TString part_zone4S_trans_name = Form("part_zone4_trans_%s", station_name.Data());
    part_zone4_trans->SetName(part_zone4S_trans_name);
    part_zone4_trans->RegisterYourself();
    TString bool_expression_zone4 = part_zone4S_name + ":" + part_zone4S_trans_name + " - " + hole_zone4S_name;
    TGeoCompositeShape *zone4S = new TGeoCompositeShape(bool_expression_zone4);

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dXOuterFrame/2, (2*yHalfSizeZone1+2*dYOuterFrame)/2, ZModuleSize/2);
    TGeoShape *horizontal_frameS = new TGeoBBox("vertical_frameS", 2*xHalfSizeZone1/2, dYOuterFrame/2, (ZModuleSize-dZSensitiveVolume)/2);
    TGeoShape *circle_inner_frameS = new TGeoTube("circle_inner_frameS", hole_radius-dXInnerFrame, hole_radius, (ZModuleSize-dZSensitiveVolume)/2);

    //zone volumes
    TGeoVolume *zone1V = new TGeoVolume("zone1_Sensor", zone1S);
    zone1V->SetLineColor(TColor::GetColor("#ff47ca"));

    TGeoVolume *zone2V = new TGeoVolume("zone2_Sensor", zone2S);
    zone2V->SetLineColor(TColor::GetColor("#e0b0ff"));

    TGeoVolume *zone3V = new TGeoVolume("zone3_Sensor", zone3S);
    zone3V->SetLineColor(TColor::GetColor("#2af7ed"));

    TGeoVolume *zone4V = new TGeoVolume("zone4_Sensor", zone4S);
    zone4V->SetLineColor(TColor::GetColor("#ffff66"));

    //frame volumes
    TGeoVolume *vertical_frameV = new TGeoVolume("vertical_frame", vertical_frameS);
    TGeoVolume *horizontal_frameV = new TGeoVolume("horizontal_frame", horizontal_frameS);
    TGeoVolume *circle_inner_frameV = new TGeoVolume("circle_inner_frame", circle_inner_frameS);
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    circle_inner_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //zone media
    TGeoMedium *zone_medium = pMedArCO27030; //set medium
    if(zone_medium) {
        zone1V->SetMedium(zone_medium);
        zone2V->SetMedium(zone_medium);
        zone3V->SetMedium(zone_medium);
        zone4V->SetMedium(zone_medium);
    }
    else Fatal("Main", "Invalid medium for zones!");

    //frame media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if(frame_medium) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_frameV->SetMedium(frame_medium);
        circle_inner_frameV->SetMedium(frame_medium);
    }
    else Fatal("Main", "Invalid medium for frames!");

    //left-top quadrant
    stationA->AddNode(zone1V, 0);
    stationA->AddNode(zone2V, 0);
    stationA->AddNode(zone3V, 0);
    stationA->AddNode(zone4V, 0);

    //right-top quadrant
    TGeoCombiTrans *quadrant_right_top_position = new TGeoCombiTrans();
    quadrant_right_top_position->ReflectX(true);
    stationA->AddNode(zone1V, 1, quadrant_right_top_position);
    stationA->AddNode(zone2V, 1, quadrant_right_top_position);
    stationA->AddNode(zone3V, 1, quadrant_right_top_position);
    stationA->AddNode(zone4V, 1, quadrant_right_top_position);

    //left-bottom quadrant
    TGeoCombiTrans *quadrant_left_bottom_position = new TGeoCombiTrans();
    quadrant_left_bottom_position->ReflectY(true);
    stationA->AddNode(zone1V, 2, quadrant_left_bottom_position);
    stationA->AddNode(zone2V, 2, quadrant_left_bottom_position);
    stationA->AddNode(zone3V, 2, quadrant_left_bottom_position);
    stationA->AddNode(zone4V, 2, quadrant_left_bottom_position);

    //right-bottom quadrant
    TGeoCombiTrans *quadrant_right_bottom_position = new TGeoCombiTrans();
    quadrant_right_bottom_position->ReflectY(true);
    quadrant_right_bottom_position->ReflectX(true);
    stationA->AddNode(zone1V, 3, quadrant_right_bottom_position);
    stationA->AddNode(zone2V, 3, quadrant_right_bottom_position);
    stationA->AddNode(zone3V, 3, quadrant_right_bottom_position);
    stationA->AddNode(zone4V, 3, quadrant_right_bottom_position);

    //frame
    TGeoTranslation *left_vertical_frame_pos = new TGeoTranslation(dXOuterFrame/2+xHalfSizeZone1, 0, -(ZModuleSize-dZSensitiveVolume)/2);
    TGeoTranslation *right_vertical_frame_pos = new TGeoTranslation(-(dXOuterFrame/2+xHalfSizeZone1), 0, -(ZModuleSize-dZSensitiveVolume)/2);
    TGeoTranslation *top_horizontal_frame_pos = new TGeoTranslation(0, dYOuterFrame/2+yHalfSizeZone1, -ZModuleSize/2);
    TGeoTranslation *bottom_horizontal_frame_pos = new TGeoTranslation(0, -(dYOuterFrame/2+yHalfSizeZone1), -ZModuleSize/2);
    stationA->AddNode(vertical_frameV, 0, left_vertical_frame_pos);
    stationA->AddNode(vertical_frameV, 1, right_vertical_frame_pos);
    stationA->AddNode(horizontal_frameV, 0, top_horizontal_frame_pos);
    stationA->AddNode(horizontal_frameV, 1, bottom_horizontal_frame_pos);
    stationA->AddNode(circle_inner_frameV, 0);

    //Station position
    Double_t station_shift = 0.0;
    if(!StationCenterPosition) station_shift = dZSensitiveVolume/2 + (ZModuleSize-dZSensitiveVolume);
    TGeoTranslation *station_pos = new TGeoTranslation(x_position, y_position, z_position+station_shift);

    mother_volume->AddNode(stationA, 0, station_pos);
}

void CreateStationTypePrototype(TGeoVolume* mother_volume, TString station_name, Double_t x_position, Double_t y_position, Double_t z_position, Double_t hole_radius) {
    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    //sensitive shapes
    TGeoShape *moduleS = new TGeoBBox("moduleS", XModuleSize_StationPrototype/2, YModuleSize_StationPrototype/2, dZSensitiveVolume/2);
    TGeoShape *hot_zoneS = new TGeoBBox("hot_zoneS", XHotZoneSize_StationPrototype/2, YHotZoneSize_StationPrototype/2, dZSensitiveVolume/2);

    //frame shapes
    TGeoShape *vertical_outer_frameS = new TGeoBBox("vertical_outer_frameS", dXOuterFrame/2, (YModuleSize_StationPrototype+dYOuterFrame+dYInnerFrame)/2, ZModuleSize/2);
    TGeoShape *horizontal_outer_frameS = new TGeoBBox("horizontal_outer_frameS", (XModuleSize_StationPrototype+dXInnerFrame)/2, dYOuterFrame/2, (ZModuleSize-dZSensitiveVolume)/2);
    TGeoShape *vertical_inner_frameS = new TGeoBBox("vertical_inner_frameS", dXInnerFrame/2, YModuleSize_StationPrototype/2, (ZModuleSize-dZSensitiveVolume)/2);
    TGeoShape *horizontal_inner_frameS = new TGeoBBox("horizontal_inner_frameS", (XModuleSize_StationPrototype+dXInnerFrame)/2, dYInnerFrame/2, ZModuleSize/2);

/*
    TGeoXtru *moduleS = new TGeoXtru(2);
        Double_t x_pos[6] = {XModuleSize_StationPrototype, XModuleSize_StationPrototype, 0.0, 0.0, XHotZoneSize_StationPrototype, XHotZoneSize_StationPrototype};
        Double_t y_pos[6] = {0.0, YModuleSize_StationPrototype, YModuleSize_StationPrototype, YHotZoneSize_StationPrototype, YHotZoneSize_StationPrototype, 0.0};
    moduleS->DefinePolygon(6, x_pos, y_pos);
    moduleS->DefineSection(0, -dZSensitiveVolume/2);
    moduleS->DefineSection(1, dZSensitiveVolume/2);
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
    TGeoTranslation *hot_zone_trans = new TGeoTranslation(-XModuleSize_StationPrototype/2 + XHotZoneSize_StationPrototype/2, -YModuleSize_StationPrototype/2 + YHotZoneSize_StationPrototype/2, 0);

    //module positions in the station
    TGeoCombiTrans *module_left_bottom_position = new TGeoCombiTrans();
    module_left_bottom_position->ReflectY(true);
    module_left_bottom_position->SetTranslation(XModuleSize_StationPrototype/2+dXInnerFrame+hole_radius, -YModuleSize_StationPrototype/2+dYInnerFrame+hole_radius, -dZSensitiveVolume/2);

    TGeoCombiTrans *module_right_bottom_position = new TGeoCombiTrans();
    module_right_bottom_position->ReflectX(true);
    module_right_bottom_position->ReflectY(true);
    module_right_bottom_position->SetTranslation(-XModuleSize_StationPrototype/2+hole_radius+dXInnerFrame, -YModuleSize_StationPrototype/2-dYInnerFrame-hole_radius, dZSensitiveVolume/2);

    TGeoCombiTrans *module_right_top_position = new TGeoCombiTrans();
    module_right_top_position->ReflectX(true);
    module_right_top_position->SetTranslation(-XModuleSize_StationPrototype/2-dXInnerFrame-hole_radius, YModuleSize_StationPrototype/2-dYInnerFrame-hole_radius, -dZSensitiveVolume/2);

    TGeoCombiTrans *module_left_top_position = new TGeoCombiTrans();
    module_left_top_position->SetTranslation(XModuleSize_StationPrototype/2-dXInnerFrame-hole_radius, YModuleSize_StationPrototype/2+dYInnerFrame+hole_radius, dZSensitiveVolume/2);

    //frame part positions in the frame
    TGeoCombiTrans *left_outer_frame_position = new TGeoCombiTrans();
    left_outer_frame_position->SetTranslation(XModuleSize_StationPrototype/2+dXOuterFrame/2, (dYOuterFrame-dYInnerFrame)/2, (ZModuleSize-dZSensitiveVolume)/2);

    TGeoCombiTrans *top_outer_frame_position = new TGeoCombiTrans();
    top_outer_frame_position->SetTranslation(-dXInnerFrame/2, YModuleSize_StationPrototype/2+dYOuterFrame/2, ZModuleSize/2);

    TGeoCombiTrans *right_inner_frame_position = new TGeoCombiTrans();
    right_inner_frame_position->SetTranslation(-XModuleSize_StationPrototype/2-dXInnerFrame/2, 0, ZModuleSize/2);

    TGeoCombiTrans *bottom_inner_frame_position = new TGeoCombiTrans();
    bottom_inner_frame_position->SetTranslation(-dXInnerFrame/2, -YModuleSize_StationPrototype/2-dYInnerFrame/2, (ZModuleSize-dZSensitiveVolume)/2);

    //frame positions in the station
    TGeoCombiTrans *comp_frame_left_bottom_position = new TGeoCombiTrans();
    comp_frame_left_bottom_position->ReflectZ(true);
    comp_frame_left_bottom_position->ReflectY(true);
    comp_frame_left_bottom_position->SetTranslation(module_left_bottom_position->GetTranslation());

    TGeoCombiTrans *comp_frame_right_bottom_position = new TGeoCombiTrans();
    comp_frame_right_bottom_position->RotateX(180.0);
    comp_frame_right_bottom_position->RotateY(180.0);
    comp_frame_right_bottom_position->SetTranslation(module_right_bottom_position->GetTranslation());

    TGeoCombiTrans *comp_frame_right_top_position = new TGeoCombiTrans();
    comp_frame_right_top_position->RotateY(180.0);
    comp_frame_right_top_position->SetTranslation(module_right_top_position->GetTranslation());

    TGeoCombiTrans *comp_frame_left_top_position = new TGeoCombiTrans();
    comp_frame_left_top_position->SetTranslation(module_left_top_position->GetTranslation());

    //station assembling
    moduleV->AddNode(hot_zoneV, 0, hot_zone_trans);

    composite_frameV->AddNode(vertical_outer_frameV, 0, left_outer_frame_position);
    composite_frameV->AddNode(horizontal_outer_frameV, 0, top_outer_frame_position);
    composite_frameV->AddNode(vertical_inner_frameV, 0, right_inner_frame_position);
    composite_frameV->AddNode(horizontal_inner_frameV, 0, bottom_inner_frame_position);

    stationA->AddNode(moduleV, 0, module_left_bottom_position);
    stationA->AddNode(moduleV, 1, module_right_bottom_position);
    stationA->AddNode(moduleV, 2, module_right_top_position);
    stationA->AddNode(moduleV, 3, module_left_top_position);

    stationA->AddNode(composite_frameV, 0, comp_frame_left_bottom_position);
    stationA->AddNode(composite_frameV, 1, comp_frame_right_bottom_position);
    stationA->AddNode(composite_frameV, 2, comp_frame_right_top_position);
    stationA->AddNode(composite_frameV, 3, comp_frame_left_top_position);

    //Station position
    Double_t station_shift = 0.0;
    if(!StationCenterPosition) station_shift = (ZModuleSize*2)/2;
    TGeoTranslation *station_pos = new TGeoTranslation(x_position, y_position, z_position+station_shift);

    mother_volume->AddNode(stationA, 0, station_pos);

}

void CreateStation_TypeTwoModules(TGeoVolume* mother_volume, TString station_name, Double_t x_position, Double_t y_position, Double_t z_position, Double_t hole_radius, Bool_t start_positive_angle, Double_t *outer_zone_sizes, Double_t *hot_zone_sizes_rect, Double_t *hot_zone_sizes_slope) {
    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    Double_t XOuterZoneSize = outer_zone_sizes[0];
    Double_t YOuterZoneSize = outer_zone_sizes[1];

    Double_t XRectangleZoneSize = hot_zone_sizes_rect[0];
    Double_t YRectangleZoneSize = hot_zone_sizes_rect[1];

    Double_t XMinSlopeZoneSize = hot_zone_sizes_slope[0];
    Double_t XMaxSlopeZoneSize = hot_zone_sizes_slope[1];
    Double_t YSlopeZoneSize = hot_zone_sizes_slope[2];

    //module shape
    TString part_module_name = "part_moduleS_" + station_name;
    TGeoShape *part_moduleS = new TGeoBBox(part_module_name, XOuterZoneSize/2, YOuterZoneSize/2, dZSensitiveVolume/2);

    //part hot zone shapes (without the hole)
    TString part_rectangle_inner_zone_name = "part_rectangle_inner_zoneS_" + station_name;
    TGeoShape *part_rectangle_inner_zoneS = new TGeoBBox(part_rectangle_inner_zone_name, XRectangleZoneSize/2, YRectangleZoneSize/2, dZSensitiveVolume/2);

    TString part_slope_inner_zone_name = "part_slope_inner_zoneS_" + station_name;
    TGeoXtru *part_slope_inner_zoneS = new TGeoXtru(2); //origin centered
    part_slope_inner_zoneS->SetName(part_slope_inner_zone_name);
        Double_t x_pos[4] = {XMaxSlopeZoneSize/2, XMaxSlopeZoneSize/2 - (XMaxSlopeZoneSize-XMinSlopeZoneSize), -XMaxSlopeZoneSize/2, -XMaxSlopeZoneSize/2};
        Double_t y_pos[4] = {-YSlopeZoneSize/2, YSlopeZoneSize/2, YSlopeZoneSize/2, -YSlopeZoneSize/2};
    part_slope_inner_zoneS->DefinePolygon(4, x_pos, y_pos);
    part_slope_inner_zoneS->DefineSection(0, -dZSensitiveVolume/2);
    part_slope_inner_zoneS->DefineSection(1, dZSensitiveVolume/2);

    //hole shape
    TString hole_name = "holeS_" + station_name;
    //TGeoShape *holeS = new TGeoTubeSeg(hole_name, 0.0, hole_radius, dZSensitiveVolume/2 + 0.1, -1.0, 91.0);
    TGeoShape *holeS = new TGeoTube(hole_name, 0.0, hole_radius, dZSensitiveVolume/2+0.01);

    //hole positions in the module and zones to extract the hole
    TString hole_module_trans_name = "hole_module_trans_name_" + station_name;
    TGeoTranslation *hole_module_trans = new TGeoTranslation(hole_module_trans_name, -XOuterZoneSize/2, -YOuterZoneSize/2, 0);
    hole_module_trans->RegisterYourself();

    TString hole_rectangle_inner_zone_trans_name = "hole_rectangle_inner_zone_trans_name_" + station_name;
    TGeoTranslation *hole_rectangle_inner_zone_trans = new TGeoTranslation(hole_rectangle_inner_zone_trans_name, -XRectangleZoneSize/2, -YRectangleZoneSize/2, 0);
    hole_rectangle_inner_zone_trans->RegisterYourself();

    TString hole_slope_inner_zone_trans_name = "hole_slope_inner_zone_trans_name_" + station_name;
    TGeoTranslation *hole_slope_inner_zone_trans = new TGeoTranslation(hole_slope_inner_zone_trans_name, -XMaxSlopeZoneSize/2, -YSlopeZoneSize/2, 0);
    hole_slope_inner_zone_trans->RegisterYourself();

    //module and hot zone shapes
    TString bool_expression_module = part_module_name + " - " + hole_name + ":" + hole_module_trans_name;
    TGeoCompositeShape *moduleS = new TGeoCompositeShape(bool_expression_module);

    TString bool_expression_rectangle_inner_zone = part_rectangle_inner_zone_name + " - " + hole_name + ":" + hole_rectangle_inner_zone_trans_name;
    TGeoCompositeShape *rectangle_inner_zoneS = new TGeoCompositeShape(bool_expression_rectangle_inner_zone);

    TString bool_expression_slope_inner_zone = part_slope_inner_zone_name + " - " + hole_name + ":" + hole_slope_inner_zone_trans_name;
    TGeoCompositeShape *slope_inner_zoneS = new TGeoCompositeShape(bool_expression_slope_inner_zone);

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dXOuterFrame/2, (YOuterZoneSize+dYInnerFrame+dYOuterFrame)/2, ZModuleSize/2);
    TGeoShape *horizontal_long_frameS = new TGeoBBox("horizontal_long_frameS", XOuterZoneSize, dYOuterFrame/2, (ZModuleSize-dZSensitiveVolume)/2);
    TGeoShape *horizontal_short_frameS = new TGeoBBox("horizontal_short_frameS", (XOuterZoneSize-hole_radius+dXInnerFrame)/2, dYInnerFrame/2, (ZModuleSize-dZSensitiveVolume)/2);
    TGeoShape *circle_inner_frameS = new TGeoTubeSeg("circle_inner_frameS", hole_radius-dXInnerFrame, hole_radius, ZModuleSize/2, 0., -180.0);

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
    TGeoTranslation *slope_inner_zone_position = new TGeoTranslation(-XOuterZoneSize/2+XMaxSlopeZoneSize/2, -YOuterZoneSize/2 + YSlopeZoneSize/2, 0);
    TGeoTranslation *rect_inner_zone_position = new TGeoTranslation(-XOuterZoneSize/2+XRectangleZoneSize/2, -YOuterZoneSize/2 + YRectangleZoneSize/2, 0);

    //module positions in the station
    TGeoCombiTrans *left_top_module_position = new TGeoCombiTrans();
    left_top_module_position->SetTranslation(XOuterZoneSize/2, YOuterZoneSize/2, dZSensitiveVolume/2);

    TGeoCombiTrans *right_top_module_position = new TGeoCombiTrans();
    right_top_module_position->RotateY(180.0);
    right_top_module_position->SetTranslation(-XOuterZoneSize/2, YOuterZoneSize/2, dZSensitiveVolume/2);

    TGeoCombiTrans *right_bottom_module_position = new TGeoCombiTrans();
    right_bottom_module_position->RotateY(180.0);
    right_bottom_module_position->RotateX(180.0);
    right_bottom_module_position->SetTranslation(-XOuterZoneSize/2, -YOuterZoneSize/2, -dZSensitiveVolume/2);

    TGeoCombiTrans *left_bottom_module_position = new TGeoCombiTrans();
    left_bottom_module_position->RotateX(180.0);
    left_bottom_module_position->SetTranslation(XOuterZoneSize/2, -YOuterZoneSize/2, -dZSensitiveVolume/2);

    //frame part positions in the frame
    TGeoTranslation *left_top_frame_position = new TGeoTranslation(XOuterZoneSize+dXOuterFrame/2, YOuterZoneSize/2+(dYOuterFrame-dYInnerFrame)/2, ZModuleSize/2);
    TGeoTranslation *right_top_frame_position = new TGeoTranslation(-(XOuterZoneSize+dXOuterFrame/2), YOuterZoneSize/2+(dYOuterFrame-dYInnerFrame)/2, ZModuleSize/2);
    TGeoTranslation *top_long_frame_position = new TGeoTranslation(0, YOuterZoneSize+dYOuterFrame/2, (ZModuleSize-dZSensitiveVolume)/2 + dZSensitiveVolume);
    TGeoTranslation *left_top_short_frame_position = new TGeoTranslation((XOuterZoneSize-hole_radius-dXInnerFrame)/2 + hole_radius, -dYInnerFrame/2, (ZModuleSize-dZSensitiveVolume)/2 + dZSensitiveVolume);
    TGeoTranslation *right_top_short_frame_position = new TGeoTranslation(-((XOuterZoneSize-hole_radius-dXInnerFrame)/2 + hole_radius), -dYInnerFrame/2, (ZModuleSize-dZSensitiveVolume)/2 + dZSensitiveVolume);

    //frame positions in the station
    TGeoCombiTrans *upper_frame_trans = new TGeoCombiTrans();

    TGeoCombiTrans *lower_frame_trans = new TGeoCombiTrans();
    lower_frame_trans->ReflectY(true);
    lower_frame_trans->ReflectZ(true);

    //station assembling
    module_with_slope_inner_zoneV->AddNode(slope_inner_zoneV, 0, slope_inner_zone_position);
    module_with_rect_inner_zoneV->AddNode(rectangle_inner_zoneV, 0, rect_inner_zone_position);


    if(start_positive_angle) {
        stationA->AddNode(module_with_slope_inner_zoneV, 0, left_top_module_position);
        stationA->AddNode(module_with_rect_inner_zoneV, 1, right_top_module_position);
        stationA->AddNode(module_with_rect_inner_zoneV, 2, right_bottom_module_position);
        stationA->AddNode(module_with_slope_inner_zoneV, 3, left_bottom_module_position);
    }
    else {
        stationA->AddNode(module_with_rect_inner_zoneV, 0, left_top_module_position);
        stationA->AddNode(module_with_slope_inner_zoneV, 1, right_top_module_position);
        stationA->AddNode(module_with_slope_inner_zoneV, 2, right_bottom_module_position);
        stationA->AddNode(module_with_rect_inner_zoneV, 3, left_bottom_module_position);
    }

    //frames
    composite_frameV->AddNode(vertical_frameV, 0, left_top_frame_position);
    composite_frameV->AddNode(vertical_frameV, 1, right_top_frame_position);
    composite_frameV->AddNode(horizontal_long_frameV, 0, top_long_frame_position);
    composite_frameV->AddNode(horizontal_short_frameV, 0, left_top_short_frame_position);
    composite_frameV->AddNode(horizontal_short_frameV, 1, right_top_short_frame_position);
    composite_frameV->AddNode(circle_inner_frameV, 0, new TGeoTranslation(0, 0, ZModuleSize/2));

    stationA->AddNode(composite_frameV, 0, upper_frame_trans);
    stationA->AddNode(composite_frameV, 1, lower_frame_trans);

    //Station position
    Double_t station_shift = 0.0;
    if(!StationCenterPosition) station_shift = (ZModuleSize*2)/2;
    TGeoTranslation *station_pos = new TGeoTranslation(x_position, y_position, z_position+station_shift);

    mother_volume->AddNode(stationA, 0, station_pos);
}