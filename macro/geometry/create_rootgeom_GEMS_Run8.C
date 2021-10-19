/*
 * Baranov D.
 * 25.02.2020 (created)
 * 18.01.2021 (updated)
 * 02.03.2021 (updated): radius of hole in active zone (4.0 -> 6.0 cm)
 * Use this macro to create ROOT geometry for the FutureConfig2020 GEM configuration
 *
 */

//Set Parameters of GEMS -------------------------------------------------------
const Int_t NStations = 7;      //stations in the detector
const Int_t NMaxModules = 4;    //max. number of modules in a station

//(X-Y-Z)Positions of stations (sensitive volumes)
//const Double_t XStationPositions[NStations] = { 0.0/*0*/, 0.0/*1*/, 0.0/*2*/, 0.0/*3*/, 0.0/*4*/, 0.0/*5*/, 0.0/*6*/};
const Double_t XStationPositions[NStations] = { +0.0/*0*/, +0.804/*1*/, +1.568/*2*/, +2.372/*3*/, +3.88/*4*/, +5.442/*5*/, +6.964/*6*/}; //rough x-shifts
const Double_t YStationPositions[NStations] = { 0.0/*0*/, 0.0/*1*/, 0.0/*2*/, 0.0/*3*/, 0.0/*4*/, 0.0/*5*/, 0.0/*6*/};
//const Double_t ZStationPositions[NStations] = { +50.52/*0*/, +81.72/*1*/, +112.92/*2*/, +144.12/*3*/, +175.32/*4*/, +206.52/*5*/, 237.72/*6*/};
const Double_t ZStationPositions[NStations] = { +58.2/*0*/, +89.4/*1*/, +120.6/*2*/, +151.8/*3*/, +183.0/*4*/, +214.2/*5*/, 245.4/*6*/};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 0
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 1
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 2
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 3
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 4
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}, //st. 5
    {163.2*0.25, -163.2*0.25, 163.2*0.25, -163.2*0.25}  //st. 6
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {+22.5, +22.5, -19.5, -19.5},  //st. 0
    {+22.5, +22.5, -19.5, -19.5},  //st. 1
    {+22.5, +22.5, -19.5, -19.5},  //st. 2
    {+22.5, +22.5, -19.5, -19.5},  //st. 3
    {+22.5, +22.5, -19.5, -19.5},  //st. 4
    {+22.5, +22.5, -19.5, -19.5},  //st. 5
    {+22.5, +22.5, -19.5, -19.5}   //st. 6
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {+4.1, +4.1, 0.0, 0.0}, //st. 0
    {0.0, 0.0, +4.1, +4.1}, //st. 1
    {+4.1, +4.1, 0.0, 0.0}, //st. 2
    {0.0, 0.0, +4.1, +4.1}, //st. 3
    {+4.1, +4.1, 0.0, 0.0}, //st. 4
    {0.0, 0.0, +4.1, +4.1}, //st. 5
    {+4.1, +4.1, 0.0, 0.0}  //st. 6
};

//GEM plane sizes (66x41 type) -------------------------------------------------
const Double_t XModuleSize_Station66x41 = 66.0;
const Double_t YModuleSize_Station66x41 = 41.0;
const Double_t ZModuleSize_Station66x41 = 0.9;

const Double_t dXFrame_Station66x41 = 2.0;
const Double_t dYFrame_Station66x41 = 2.0;
const Double_t dZFrame_Station66x41 = 4.1;
//------------------------------------------------------------------------------

//GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2*0.5; //81.6
const Double_t YModuleSize_Station163x45 = 45.0;
const Double_t ZModuleSize_Station163x45 = 0.9;

const Double_t dXFrame_Station163x45 = 2.0;
const Double_t dYFrame_Station163x45 = 2.0;
const Double_t dZFrame_Station163x45 = 4.1;
//------------------------------------------------------------------------------

//GEM plane sizes (163x39 type) ------------------------------------------------
const Double_t XModuleSize_Station163x39 = 163.2*0.5; //81.6
const Double_t YModuleSize_Station163x39 = 39.0;
const Double_t ZModuleSize_Station163x39 = 0.9; //sensitive volume
const Double_t ZGEMSize_Station163x39 = 4.1; //common thickness of GEM (including sens. vol)

const Double_t dXFrame_Station163x39 = 2.0;
const Double_t dYFrame_Station163x39 = 2.0;
const Double_t dZFrame_Station163x39 = 4.1;
//------------------------------------------------------------------------------

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedCarbon = 0;
TGeoMedium *pMedFiberGlass = 0;
TGeoMedium *pMedCopper = 0;
TGeoMedium *pMedArCO27030 = 0;
TGeoMedium *pMedArC4H10_80_20 = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);

TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);
TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz);

TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius);

TGeoVolume *CreateModule_Station163x39(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
TGeoVolume *CreateFrameForModule_Station163x39(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius);


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

    //ArC4H10_80_20 medium
    FairGeoMedium* mArC4H10_80_20 = geoMedia->getMedium("ArC4H10_80_20");
    if ( ! mArC4H10_80_20  ) Fatal("Main", "FairMedium ArC4H10_80_20 not found");
    geoBuild->createMedium(mArC4H10_80_20);
    pMedArC4H10_80_20 = gGeoManager->GetMedium("ArC4H10_80_20");
    if ( ! pMedArC4H10_80_20  ) Fatal("Main", "Medium ArC4H10_80_20 not found");
}

void create_rootgeom_GEMS_Run8() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ---------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "Run8";
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
    TGeoVolume* GEMS = new TGeoVolumeAssembly(geoDetectorName);
    GEMS->SetMedium(pMedAir);

    for(Int_t istation = 0; istation < NStations; ++istation) {

        Int_t stationNum = istation;
        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));

        //upper part
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0+2.0);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_")+station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0+2.0);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_")+station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0+2.0);

        //lower part
        TGeoVolume *module2 = CreateModule_Station163x39(TString("Sensor_module2_")+station->GetName(), XModuleSize_Station163x39, YModuleSize_Station163x39, ZModuleSize_Station163x39, 4.0+2.0);
        TGeoVolume *module3 = CreateModule_Station163x39(TString("Sensor_module3_")+station->GetName(), XModuleSize_Station163x39, YModuleSize_Station163x39, ZModuleSize_Station163x39, 4.0+2.0);
        TGeoVolume *frame2 = CreateFrameForModule_Station163x39(TString("frame2_")+station->GetName(), dXFrame_Station163x39, dYFrame_Station163x39, dZFrame_Station163x39, 4.0+2.0);
        TGeoVolume *frame3 = CreateFrameForModule_Station163x39(TString("frame3_")+station->GetName(), dXFrame_Station163x39, dYFrame_Station163x39, dZFrame_Station163x39, 4.0+2.0);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->ReflectX(true);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *module2_transform = new TGeoCombiTrans();
            module2_transform->ReflectY(true);
            module2_transform->SetTranslation(XModuleShifts[stationNum][2], YModuleShifts[stationNum][2], ZModuleShifts[stationNum][2]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *module3_transform = new TGeoCombiTrans();
            module3_transform->ReflectY(true);
            module3_transform->ReflectX(true);
            module3_transform->SetTranslation(XModuleShifts[stationNum][3], YModuleShifts[stationNum][3], ZModuleShifts[stationNum][3]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module
        station->AddNode(module2, 0, new TGeoCombiTrans(*module2_transform)); //module
        station->AddNode(module3, 0, new TGeoCombiTrans(*module3_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame
        station->AddNode(frame2, 0, new TGeoCombiTrans(*module2_transform)); //frame
        station->AddNode(frame3, 0, new TGeoCombiTrans(*module3_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
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
    if(pMedArC4H10_80_20) moduleV->SetMedium(pMedArC4H10_80_20);
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

    gRandom->SetSeed(0);
    Int_t uniqueId = gRandom->Integer(100000);

    //module shapes
    TGeoShape *module_partS = new TGeoBBox(TString("module_partS")+TString::Itoa(uniqueId,10), xsize*0.5, ysize*0.5, zsize*0.5);
    TGeoShape *holeS = new TGeoTube(TString("holeS")+TString::Itoa(uniqueId,10), 0.0, hole_radius, zsize*0.5+0.01);

    TGeoTranslation *hole_module_trans = new TGeoTranslation(TString("hole_module_trans_")+TString::Itoa(uniqueId,10), -xsize*0.5, -ysize*0.5, 0.0);

    hole_module_trans->RegisterYourself();

    TString bool_expression_module = TString(module_partS->GetName()) + TString(" - ") + TString(holeS->GetName()) + TString(":") + TString(hole_module_trans->GetName());
    TGeoCompositeShape *moduleS = new TGeoCompositeShape(bool_expression_module);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if(pMedArC4H10_80_20) moduleV->SetMedium(pMedArC4H10_80_20);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}
//------------------------------------------------------------------------------

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

TGeoVolume *CreateModule_Station163x39(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius) {

    return CreateModule_Station163x45(module_name, xsize, ysize, zsize, hole_radius);
}
//------------------------------------------------------------------------------

TGeoVolume *CreateFrameForModule_Station163x39(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox(TString("vertical_frameS")+=TString("_") + frame_name, dx*0.5, YModuleSize_Station163x39*0.5+dy, dz*0.5);
    TGeoShape *horizontal_upper_frameS = new TGeoBBox(TString("horizontal_upper_frameS")+=TString("_") + frame_name, XModuleSize_Station163x39*0.5, dy*0.5, dz*0.5);
    TGeoShape *horizontal_bottom_frameS = new TGeoBBox(TString("horizontal_upper_frameS")+=TString("_") + frame_name, XModuleSize_Station163x39*0.5-hole_radius*0.5, dy*0.5, dz*0.5);

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
    left_vertical_frame_position->SetTranslation(XModuleSize_Station163x39*0.5+dXFrame_Station163x39*0.5, 0.0, 0.0);

    TGeoCombiTrans *upper_horizontal_frame_position = new TGeoCombiTrans();
    upper_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station163x39*0.5+dYFrame_Station163x39*0.5, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(hole_radius*0.5, -YModuleSize_Station163x39*0.5-dYFrame_Station163x39*0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(horizontal_upper_frameV, 0, upper_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_bottom_frameV, 0, bottom_horizontal_frame_position);

    return composite_frameV;
}
//------------------------------------------------------------------------------