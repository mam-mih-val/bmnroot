//---------------------------


using namespace TMath;

//GeoManager
TGeoManager* gGeoMan = NULL;

const Double_t XStationPosition = 0.0;
const Double_t YStationPosition = -4.5;
const Double_t ZStationPosition = 0.0;

//Geometry parameters (sizes, positions, shifts): all units is in cm!!!
enum ModuleType {RIGHT_ANGLED_MODULE, BEVEL_MODULE};

//Station
const Int_t NModules = 8; //Number of modules in a station

Double_t XModulesPositions[NModules] = { // in bm@n coordinate system
    +12.15, // module 0
    +6.15,  // module 1
    -6.15,  // module 2
    -12.15, // module 3
    +12.15, // module 4
    +6.15,  // module 5
    -6.15,  // module 6
    -12.15  // module 7
};

Double_t YModulesPositions[NModules] = { // in bm@n coordinate system
    -0.15,  // module 0
    -0.15,  // module 1
    -0.15,  // module 2
    -0.15,  // module 3
    +0.15,  // module 4
    +0.15,  // module 5
    +0.15,  // module 6
    +0.15   // module 7
};

Double_t ZModulesPositions[NModules] = { // in bm@n coordinate system
    3.31,   // module 0
    1.78,   // module 1
    3.09,   // module 2
    1.56,   // module 3
    2.05,   // module 4
    3.58,   // module 5
    2.27,   // module 6
    3.80    // module 7
};

ModuleType ModulesTypes[NModules] = {
    RIGHT_ANGLED_MODULE, // module 0
    BEVEL_MODULE,        // module 1
    BEVEL_MODULE,        // module 2
    RIGHT_ANGLED_MODULE, // module 3
    RIGHT_ANGLED_MODULE, // module 4
    BEVEL_MODULE,        // module 5
    BEVEL_MODULE,        // module 6
    RIGHT_ANGLED_MODULE  // module 7
};

//rotations of modules around y-axis by 180 deg.
Bool_t ModulesYRotations[NModules] = {
    true,   // module 0
    true,   // module 1
    false,  // module 2
    false,  // module 3
    true,   // module 4
    true,   // module 5
    false,  // module 6
    false   // module 7
};

//rotations of modules around x-axis by 180 deg.
Bool_t ModulesXRotations[NModules] = {
    false,  // module 0
    false,  // module 1
    false,  // module 2
    false,  // module 3
    true,   // module 4
    true,   // module 5
    true,   // module 6
    true    // module 7
};

//Module -----------------------------------------------------------------------

Double_t YGapBetweenPartsOfModule = 0.0035;

Double_t XModuleSize = 6.3;
Double_t YModuleSizeUpperPart = XModuleSize;
Double_t YModuleSizeLowerPart = XModuleSize;

Double_t YModuleSize = YModuleSizeUpperPart+YModuleSizeLowerPart + YGapBetweenPartsOfModule;
Double_t ZModuleSize = 0.03;

Double_t XBevelSize = 3.82; //x-bevel for a angled cut
Double_t YBevelSize = XBevelSize; //y-bevel for a angled cut

Double_t BevelAngleRad = TMath::ATan(XBevelSize/YBevelSize);
Double_t BevelAngleDeg = BevelAngleRad*TMath::RadToDeg();

Double_t XLeftBorder = 0.1;
Double_t XRightBorder = 0.1;
Double_t YTopBorder = 0.1;
Double_t YMiddleBorderUpperPart = 0.1;
Double_t YMiddleBorderLowerPart = 0.1;
Double_t YBottomBorder = 0.1;
Double_t XYBevelBorder = 0.1;
//------------------------------------------------------------------------------

//Sensor zones -----------------------------------------------------------------
Double_t XSensorZoneSize = XModuleSize - (XLeftBorder + XRightBorder);
Double_t YSensorZoneSizeUpperPart = YModuleSizeUpperPart - (YTopBorder + YMiddleBorderUpperPart);
Double_t YSensorZoneSizeLowerPart = YModuleSizeLowerPart - (YBottomBorder + YMiddleBorderLowerPart);
Double_t ZSensorZoneSize = ZModuleSize;
//------------------------------------------------------------------------------

//Module frames ----------------------------------------------------------------
Double_t XModuleFrameSize = XModuleSize;
Double_t YModuleFrameSize = YModuleSize;

Double_t ZModuleFrameSize = 0.18;
//------------------------------------------------------------------------------

//Y-shift from center of the gap to the global x-axis (for y-alignment of the module)
Double_t YShiftFromCenterToAxis = (YModuleSizeUpperPart-YModuleSizeLowerPart)*0.5;

//X-shift from center of the module to the origin of the local x-axis
Double_t XShiftFromCenterToLocalX0 = +XModuleSize*0.5;

//Y-shift from center of the module to the origin of the local y-axis
Double_t YShiftFromCenterToLocalY0 = +(YModuleSizeLowerPart + YGapBetweenPartsOfModule*0.5);

/*
     ASCII module picture (view from target)

        |<---- XModuleSize ---------->|                     ^
        |                             |                     | Y-axis
        ------------------------------  ---------
        |   |    top border       |   |         ^
        ------------------------------          |
        |   |                     |   |         |
        | l |    rectangle        | r |         |
        | e |      sensor         | i |         |
        | f |       zone          | g |         |
        | t |                     | h |         |
        |   |                     | t |         |
        |   |                     |   |         |
        ------------------------------          |
        |   | middle (upper part) |   |
        ------ gap between parts -----      YModuleSize
        |   | middle (lower part) |   |
        ------------------------------          |
        |   |                     |   |         |
        | b |   bevel (or rect.)  | r |         |
        | e |      sensor         | i |         |
         \ v \      zone          | g |         |
          \ e \                   | h |         |
           \ l \                  | t |         |
            \   \                 |   |         |
             -------------------------          |
              \   \  bottom       |   |         v
                ----------------------  ---------

 *  x-axis <----------------------------------(x0,y0) local point

 */

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedSilicon = 0;

class FairGeoMedia;
class FairGeoBuilder;

//function declarations
void CreateSiliconStation(TGeoVolume* mother_volume, TString station_name,
                          Double_t x_station_pos, Double_t y_station_pos, Double_t z_station_pos);

void CreateSiliconModule(TGeoVolume* mother_volume, Int_t module_num, ModuleType module_type,
                         Double_t x_module_pos, Double_t y_module_pos, Double_t z_module_pos,
                         Bool_t y_module_rotated, Bool_t x_module_rotated);

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
    pMedCopper= gGeoManager->GetMedium("copper");
    if ( ! pMedCopper  ) Fatal("Main", "Medium copper not found");

    //silicon medium
    FairGeoMedium* mSilicon = geoMedia->getMedium("silicon");
    if ( ! mSilicon  ) Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(mSilicon);
    pMedSilicon= gGeoManager->GetMedium("silicon");
    if ( ! pMedSilicon  ) Fatal("Main", "Medium silicon not found");
}

void PrintInfoAboutModuleParameters() {
    cout << "\n";
    cout << "Module parameters information: ---------------------------" << "\n";
    cout << "  XModuleSize = " << XModuleSize << " cm" << "\n";
    cout << "  YModuleSize = " << YModuleSize << " cm" << " ( " << YModuleSizeUpperPart << " + " << YGapBetweenPartsOfModule << " + " << YModuleSizeLowerPart << " )"   << "\n";
    cout << "     YModuleSizeUpperPart = " << YModuleSizeUpperPart << " cm" << "\n";
    cout << "     YModuleSizeLowerPart = " << YModuleSizeLowerPart << " cm" << "\n";
    cout << "     YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule << " cm" << "\n";
    cout << "  ZModuleSize = " << ZModuleSize << " cm" << "\n";
    cout << "\n";
    cout << "  XBevelSize = " << XBevelSize << " cm" << "\n";
    cout << "  YBevelSize = " << YBevelSize << " cm" << "\n";
    cout << "     BevelAngleDeg = " << BevelAngleDeg << " deg" << "\n";
    cout << "\n";
    cout << "  XLeftBorder = " << XLeftBorder << " cm" << "\n";
    cout << "  XRightBorder = " << XRightBorder << " cm" << "\n";
    cout << "  YTopBorder = " << YTopBorder << " cm" << "\n";
    cout << "  YMiddleBorderUpperPart = " << YMiddleBorderUpperPart << " cm" << "\n";
    cout << "  YGapBetweenPartsOfModule = " << YGapBetweenPartsOfModule << " cm" << " (gap between two parts)\n";
    cout << "  YMiddleBorderLowerPart = " << YMiddleBorderLowerPart << " cm" << "\n";
    cout << "  YBottomBorder = " << YBottomBorder << " cm" << "\n";
    cout << "  XYBevelBorder = " << XYBevelBorder << " cm" << "\n";
    cout << "\n";
    cout << "  XSensorZoneSize = " << XSensorZoneSize << " cm" << "\n";
    cout << "  YSensorZoneSizeUpperPart = " << YSensorZoneSizeUpperPart << " cm" << "\n";
    cout << "  YSensorZoneSizeLowerPart = " << YSensorZoneSizeLowerPart << " cm" << "\n";
    cout << "  ZSensorZoneSize = " << ZSensorZoneSize << " cm" << "\n";

    cout << "-------------------------------------------------------------\n";
}

void create_rootgeom_Silicon_v1() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "Silicon";
    const TString geoDetectorVersion = "v1";
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

    //Print information about module parameters
    PrintInfoAboutModuleParameters();

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* SILICON = new TGeoVolumeAssembly(geoDetectorName);
    SILICON->SetMedium(pMedAir);

    CreateSiliconStation(SILICON, "station0", XStationPosition, YStationPosition, ZStationPosition);


    top->AddNode(SILICON, 0);
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


void CreateSiliconStation(TGeoVolume* mother_volume, TString station_name,
                          Double_t x_station_pos, Double_t y_station_pos, Double_t z_station_pos) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    for(Int_t imodule = 0; imodule < NModules; ++imodule) {

        CreateSiliconModule(stationA, imodule, ModulesTypes[imodule],
                            XModulesPositions[imodule], YModulesPositions[imodule], ZModulesPositions[imodule],
                            ModulesYRotations[imodule], ModulesXRotations[imodule]);
    }

    //Global position of the station
    TGeoTranslation *station_pos = new TGeoTranslation(x_station_pos, y_station_pos, z_station_pos);

    //Add station volume (as a node) to the mother volume
    mother_volume->AddNode(stationA, 0, station_pos);
}

void CreateSiliconModule(TGeoVolume* mother_volume, Int_t module_num, ModuleType module_type,
                         Double_t x_module_pos, Double_t y_module_pos, Double_t z_module_pos,
                         Bool_t y_module_rotated, Bool_t x_module_rotated) {

    //Shapes -------------------------------------------------------------------
    //module shape
    TGeoShape *moduleS = NULL;
    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            moduleS = new TGeoBBox("moduleS", XModuleSize*0.5, YModuleSize*0.5, ZModuleSize*0.5);
            break;
        }
        case BEVEL_MODULE: {
            moduleS = new TGeoXtru(2);
            moduleS->SetName("moduleS");
                Double_t x_pos[5] = {+XModuleSize*0.5, -XModuleSize*0.5, -XModuleSize*0.5, +XModuleSize*0.5-XBevelSize, +XModuleSize*0.5};
                Double_t y_pos[5] = {+YModuleSize*0.5, +YModuleSize*0.5, -YModuleSize*0.5, -YModuleSize*0.5, -YModuleSize*0.5+YBevelSize};
            ((TGeoXtru*)moduleS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)moduleS)->DefineSection(0, -ZModuleSize*0.5);
            ((TGeoXtru*)moduleS)->DefineSection(1, +ZModuleSize*0.5);
            break;
        }
        default:
            Error("CreateSiliconModule", "Incorrect module type selected!!!");
    }

    //sensor zone shape (upper part of the module)
    TGeoShape *sensor_zone_upper_partS = new TGeoBBox("sensor_zone_upper_partS", XSensorZoneSize*0.5, YSensorZoneSizeUpperPart*0.5, ZSensorZoneSize*0.5);

    //sensor zone shape (lower part of the module: rectangle zone)
    TGeoShape *sensor_zone_lower_part_rectS = new TGeoBBox("sensor_zone_lower_part_rectS", XSensorZoneSize*0.5, YSensorZoneSizeLowerPart*0.5, ZSensorZoneSize*0.5);

    //sensor zone shape (lower part of the module: bevel zone)
    TGeoShape *sensor_zone_lower_part_bevelS = new TGeoXtru(2);
        sensor_zone_lower_part_bevelS->SetName("sensor_zone_lower_part_bevelS");

        //Calculate coordinates of the bottom and the top points of the inner bevel

        //find x and y projections of the bevel border thickness
        Double_t x_delta = XYBevelBorder/TMath::Cos(BevelAngleRad);
        Double_t y_delta = XYBevelBorder/TMath::Sin(BevelAngleRad);

        //find shifts of the sensetive zone from module borders via the equation of the straight line for the inner bevel
        Double_t x_delta_top_BP = XLeftBorder;
        Double_t y_delta_top_BP = TMath::Tan(TMath::PiOver2()+BevelAngleRad)*x_delta_top_BP + (YBevelSize+y_delta);

        Double_t y_delta_bottom_BP = YBottomBorder;
        Double_t x_delta_bottom_BP = (y_delta_bottom_BP - (YBevelSize + y_delta))/(TMath::Tan(TMath::PiOver2()+BevelAngleRad));
        //----------------------------------------------------------------------

        Double_t x_pos_sensor_zone_bevel[5] = { +XModuleSize*0.5-XLeftBorder, -XModuleSize*0.5+XRightBorder, -XModuleSize*0.5+XRightBorder,       +XModuleSize*0.5-x_delta_bottom_BP, +XModuleSize*0.5-XLeftBorder  };
        Double_t y_pos_sensor_zone_bevel[5] = { +YModuleSizeLowerPart*0.5-YMiddleBorderLowerPart, +YModuleSizeLowerPart*0.5-YMiddleBorderLowerPart, -YModuleSizeLowerPart*0.5+YBottomBorder,      -YModuleSizeLowerPart*0.5+YBottomBorder, -YModuleSizeLowerPart*0.5+y_delta_top_BP};

    ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
    ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(0, -ZModuleSize*0.5);
    ((TGeoXtru*)sensor_zone_lower_part_bevelS)->DefineSection(1, +ZModuleSize*0.5);

    //module frame shape
    TGeoShape *frameS = NULL;
    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            frameS = new TGeoBBox(TString("frameS")+=module_num, XModuleFrameSize*0.5, YModuleFrameSize*0.5, ZModuleFrameSize*0.5);
            break;
        }
        case BEVEL_MODULE: {
            frameS = new TGeoXtru(2);
            frameS->SetName(TString("frameS")+=module_num);
                Double_t x_pos[5] = {+XModuleFrameSize*0.5, -XModuleFrameSize*0.5, -XModuleFrameSize*0.5, +XModuleFrameSize*0.5-XBevelSize, +XModuleFrameSize*0.5};
                Double_t y_pos[5] = {+YModuleFrameSize*0.5, +YModuleFrameSize*0.5, -YModuleFrameSize*0.5, -YModuleFrameSize*0.5, -YModuleFrameSize*0.5+YBevelSize};
            ((TGeoXtru*)frameS)->DefinePolygon(5, x_pos, y_pos);
            ((TGeoXtru*)frameS)->DefineSection(0, -ZModuleFrameSize*0.5);
            ((TGeoXtru*)frameS)->DefineSection(1, +ZModuleFrameSize*0.5);
            break;
        }
        default:
            Error("CreateSiliconModule", "Incorrect module type selected!!!");
    }

    //upper frame window shape
    TGeoShape *upper_window_frameS = new TGeoBBox(TString("upper_window_frameS")+=module_num, XSensorZoneSize*0.5, YSensorZoneSizeUpperPart*0.5, (ZModuleFrameSize+0.001)*0.5);

    //lower frame rect window shape
    TGeoShape *lower_window_rect_frameS = new TGeoBBox(TString("lower_window_rect_frameS")+=module_num, XSensorZoneSize*0.5, YSensorZoneSizeLowerPart*0.5, (ZModuleFrameSize+0.001)*0.5);

    //lower frame window shape
    TGeoShape *lower_window_bevel_frameS = new TGeoXtru(2);
        lower_window_bevel_frameS->SetName(TString("lower_window_bevel_frameS")+=module_num);
    ((TGeoXtru*)lower_window_bevel_frameS)->DefinePolygon(5, x_pos_sensor_zone_bevel, y_pos_sensor_zone_bevel);
    ((TGeoXtru*)lower_window_bevel_frameS)->DefineSection(0, -(ZModuleFrameSize+0.001)*0.5);
    ((TGeoXtru*)lower_window_bevel_frameS)->DefineSection(1, +(ZModuleFrameSize+0.001)*0.5);

    TGeoTranslation *upper_window_frame_pos = new TGeoTranslation();
        upper_window_frame_pos->SetName(TString("upper_window_frame_pos")+=module_num);
        upper_window_frame_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
        upper_window_frame_pos->SetDy(+YSensorZoneSizeUpperPart*0.5 + YGapBetweenPartsOfModule*0.5 + YMiddleBorderUpperPart - YShiftFromCenterToAxis);
        upper_window_frame_pos->SetDz(0.0);
        upper_window_frame_pos->RegisterYourself();

    TGeoTranslation *lower_window_rect_frame_pos = new TGeoTranslation();
        lower_window_rect_frame_pos->SetName(TString("lower_window_rect_frame_pos")+=module_num);
        lower_window_rect_frame_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
        lower_window_rect_frame_pos->SetDy(-YSensorZoneSizeLowerPart*0.5 - YGapBetweenPartsOfModule*0.5 - YMiddleBorderLowerPart - YShiftFromCenterToAxis);
        lower_window_rect_frame_pos->SetDz(0.0);
        lower_window_rect_frame_pos->RegisterYourself();

    TGeoTranslation *lower_window_bevel_frame_pos = new TGeoTranslation();
        lower_window_bevel_frame_pos->SetName(TString("lower_window_bevel_frame_pos")+=module_num);
        lower_window_bevel_frame_pos->SetDx(0.0);
        lower_window_bevel_frame_pos->SetDy(-YModuleSizeLowerPart*0.5 - YGapBetweenPartsOfModule*0.5 - YShiftFromCenterToAxis);
        lower_window_bevel_frame_pos->SetDz(0.0);
        lower_window_bevel_frame_pos->RegisterYourself();

    TGeoCompositeShape *frame_with_windowsS = new TGeoCompositeShape();
        frame_with_windowsS->SetName("frame_with_windowsS");

        switch(module_type) {
            case RIGHT_ANGLED_MODULE: {
                TString expression = "(frameS"; expression += module_num;
                    expression +="-upper_window_frameS"; expression += module_num;
                    expression += ":upper_window_frame_pos"; expression += module_num;
                    expression += ")-lower_window_rect_frameS"; expression += module_num;
                    expression += ":lower_window_rect_frame_pos"; expression += module_num;
                frame_with_windowsS->MakeNode(expression);
                break;
            }
            case BEVEL_MODULE: {
                 TString expression = "(frameS"; expression += module_num;
                    expression += "-upper_window_frameS"; expression += module_num;
                    expression += ":upper_window_frame_pos"; expression += module_num;
                    expression += ")-lower_window_bevel_frameS"; expression += module_num;
                    expression += ":lower_window_bevel_frame_pos"; expression += module_num;
                frame_with_windowsS->MakeNode(expression);
                break;
            }
            default:
                Error("CreateSiliconModule", "Incorrect module type selected!!!");
        }

    //--------------------------------------------------------------------------

    //Volumes ------------------------------------------------------------------
    //module volume
    TString module_name = "moduleV_";
    module_name += module_num;

    //TGeoVolume *moduleV = new TGeoVolume("moduleV", moduleS);
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //sensor zone volume (upper part of the module)
    TGeoVolume *sensor_zone_upper_partV = new TGeoVolume("Sensor_zone_upper_partV", sensor_zone_upper_partS);

    //sensor zone volume (lower part of the module: rectangle zone)
    TGeoVolume *sensor_zone_lower_part_rectV = new TGeoVolume("Sensor_zone_lower_part_rectV", sensor_zone_lower_part_rectS);

    //sensor zone volume (lower part of the module: bevel zone)
    TGeoVolume *sensor_zone_lower_part_bevelV = new TGeoVolume("Sensor_zone_lower_part_bevelV", sensor_zone_lower_part_bevelS);

    //frame volume
    TGeoVolume *frameV = new TGeoVolume("frameV", frame_with_windowsS);

    //volumes visual properties
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    sensor_zone_upper_partV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_lower_part_rectV->SetLineColor(TColor::GetColor("#e0b0ff"));
    sensor_zone_lower_part_bevelV->SetLineColor(TColor::GetColor("#e0b0ff"));
    frameV->SetLineColor(TColor::GetColor("#cccccc"));

    //volumes media
    TGeoMedium *silicon_medium = pMedSilicon;
    if(silicon_medium) {
        moduleV->SetMedium(silicon_medium);
        sensor_zone_upper_partV->SetMedium(silicon_medium);
        sensor_zone_lower_part_rectV->SetMedium(silicon_medium);
        sensor_zone_lower_part_bevelV->SetMedium(silicon_medium);
    }
    else Fatal("Main", "Invalid medium for modules and sensor zones!");

    TGeoMedium *carbon_medium = pMedCarbon;
    if(carbon_medium) {
        frameV->SetMedium(carbon_medium);
    }
    else Fatal("Main", "Invalid medium for module frame!");
    //--------------------------------------------------------------------------

    //position of the sensor zone in the upper part of the module
    TGeoTranslation *sensor_zone_upper_part_pos = new TGeoTranslation();
        sensor_zone_upper_part_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
        sensor_zone_upper_part_pos->SetDy(+YSensorZoneSizeUpperPart*0.5 + YGapBetweenPartsOfModule*0.5 + YMiddleBorderUpperPart - YShiftFromCenterToAxis);
        sensor_zone_upper_part_pos->SetDz(0.0);

    //position of the sensor zone in the lower part of the module (rectangle zone)
    TGeoTranslation *sensor_zone_lower_part_rect_pos = new TGeoTranslation();
        sensor_zone_lower_part_rect_pos->SetDx(+(XLeftBorder+XRightBorder)*0.5 - XLeftBorder);
        sensor_zone_lower_part_rect_pos->SetDy(-YSensorZoneSizeLowerPart*0.5 - YGapBetweenPartsOfModule*0.5 - YMiddleBorderLowerPart - YShiftFromCenterToAxis);
        sensor_zone_lower_part_rect_pos->SetDz(0.0);

    //position of the sensor zone in the lower part of the module (bevel zone)
    TGeoTranslation *sensor_zone_lower_part_bevel_pos = new TGeoTranslation();
        sensor_zone_lower_part_bevel_pos->SetDx(0.0);
        sensor_zone_lower_part_bevel_pos->SetDy(-YModuleSizeLowerPart*0.5 - YGapBetweenPartsOfModule*0.5 - YShiftFromCenterToAxis);
        sensor_zone_lower_part_bevel_pos->SetDz(0.0);


    //Place sensors into the module
    moduleV->AddNode(sensor_zone_upper_partV, 0, sensor_zone_upper_part_pos);

    switch(module_type) {
        case RIGHT_ANGLED_MODULE: {
            moduleV->AddNode(sensor_zone_lower_part_rectV, 0, sensor_zone_lower_part_rect_pos);
            break;
        }
        case BEVEL_MODULE: {
            moduleV->AddNode(sensor_zone_lower_part_bevelV, 0, sensor_zone_lower_part_bevel_pos);
            break;
        }
        default:
            Error("CreateSiliconModule", "Incorrect module type selected!!!");
    }

    //volumes visual property (transparency)
    moduleV->SetTransparency(50);
    //frameV->SetTransparency(50);


    //module position ----------------------------------------------------------
    Double_t z_module_shift = +ZModuleSize*0.5; //To z-module position is at the start of the sensitive volume

    TGeoCombiTrans *module_orientation = new TGeoCombiTrans();

        if(y_module_rotated) module_orientation->RotateY(180.0);
        if(x_module_rotated) module_orientation->RotateX(180.0);


        if(y_module_rotated) {
            module_orientation->SetDx(x_module_pos - XShiftFromCenterToLocalX0);
        }
        else {
            module_orientation->SetDx(x_module_pos + XShiftFromCenterToLocalX0);
        }

        if(x_module_rotated) {
            module_orientation->SetDy(y_module_pos + YShiftFromCenterToAxis - YShiftFromCenterToLocalY0);
        }
        else {
            module_orientation->SetDy(y_module_pos + YShiftFromCenterToAxis + YShiftFromCenterToLocalY0);
        }

        module_orientation->SetDz(z_module_pos + z_module_shift);
    //--------------------------------------------------------------------------


    //position of the frame ----------------------------------------------------
    TGeoCombiTrans *frame_orientation = new TGeoCombiTrans();

        if(y_module_rotated) frame_orientation->RotateY(180.0);
        if(x_module_rotated) frame_orientation->RotateX(180.0);

        if(y_module_rotated) {
            frame_orientation->SetDx(x_module_pos - XShiftFromCenterToLocalX0);
        }
        else {
            frame_orientation->SetDx(x_module_pos + XShiftFromCenterToLocalX0);
        }

        if(x_module_rotated) {
            frame_orientation->SetDy(y_module_pos + YShiftFromCenterToAxis - YShiftFromCenterToLocalY0);
        }
        else {
            frame_orientation->SetDy(y_module_pos + YShiftFromCenterToAxis + YShiftFromCenterToLocalY0);
        }

        if(y_module_rotated != x_module_rotated) { //logic XOR
            frame_orientation->SetDz(z_module_pos -(ZModuleFrameSize*0.5));
        }
        else {
            frame_orientation->SetDz(z_module_pos +(ZModuleFrameSize*0.5 + ZModuleSize));
        }
    //--------------------------------------------------------------------------


    mother_volume->AddNode(moduleV, module_num, module_orientation);
    mother_volume->AddNode(frameV, module_num, frame_orientation);
}

//------------------------------------------------------------------------------