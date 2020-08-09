/*
 * Baranov D.
 * 03.07.2018
 * Use this macro to create detailed ROOT geometry for
 * the RunSpring2018 configuration (BM@N RUN-7)
 * with passive volumes (frames, al. cable supports, ...)
 *
 */

//Set Parameters of GEMS -------------------------------------------------------
const Int_t NStations = 6;      //stations in the detector
const Int_t NMaxModules = 2;    //max. number of modules in a station

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = { +0.5390, +0.7025, +1.9925, +3.0860, +3.7980, +4.5815};
const Double_t YStationPositions[NStations] = { +15.99873, +16.20573, +16.36073, +16.40473, +16.09373, +16.45473};
const Double_t ZStationPositions[NStations] = { +39.702, +64.535, +112.649, +135.330, +160.6635, +183.668};

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2*0.25, -163.2*0.25},
    {163.2*0.25, -163.2*0.25},
    {163.2*0.25, -163.2*0.25},
    {163.2*0.25, -163.2*0.25},
    {163.2*0.25, -163.2*0.25},
    {163.2*0.25, -163.2*0.25}
};
const Double_t YModuleShifts[NStations][NMaxModules] = {
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0}
};
const Double_t ZModuleShifts[NStations][NMaxModules] = {
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0}
};

//rotations of stations around y-axis by 180 deg.
const Bool_t YStationRotations[NStations] = {false/*0*/, true/*1*/, true/*2*/, false/*3*/, true/*4*/, false/*5*/};

//rotations of stations around x-axis by 180 deg.
const Bool_t XStationRotations[NStations] = {false/*0*/, false/*1*/, false/*2*/, false/*3*/, false/*4*/, false/*5*/};

//GEM plane sizes (66x41 type) -------------------------------------------------
const Double_t XModuleSize_Station66x41 = 66.0;
const Double_t YModuleSize_Station66x41 = 41.0;
const Double_t ZModuleSize_Station66x41 = 0.9;

const Double_t dXFrame_Station66x41 = 5.0;
const Double_t dYFrame_Station66x41 = 3.75;
const Double_t dZFrame_Station66x41 = 2.3;
//------------------------------------------------------------------------------

//GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2*0.5;
const Double_t YModuleSize_Station163x45 = 45.0;
const Double_t ZModuleSize_Station163x45 = 0.9; //sensitive volume
const Double_t ZGEMSize_Station163x45 = 4.2; //common thickness of GEM (including sens. vol)

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

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);
TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz);
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
    FairGeoMedium* mGlue = geoMedia->getMedium("pvac_glue");
    if ( ! mGlue  ) Fatal("Main", "FairMedium pvac_glue not found");
    geoBuild->createMedium(mGlue);
    pMedGlue= gGeoManager->GetMedium("pvac_glue");
    if ( ! pMedGlue  ) Fatal("Main", "Medium pvac_glue not found");

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
}

void create_rootgeom_GEMS_RunSpring2018_detailed() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "RunSpring2018_detailed";
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

    for(Int_t istation = 0; istation < NStations; ++istation) {
    //for(Int_t istation = 0; istation < 6; ++istation) {

        Int_t stationNum = istation;
        TGeoVolume *station = CreateStation(TString("station")+ TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_")+station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0);

        TGeoVolume *layers0 = CreateLayersForModule_Station163x45(TString("layer0_")+station->GetName());
        TGeoVolume *layers1 = CreateLayersForModule_Station163x45(TString("layer1_")+station->GetName());

        TGeoVolume *frame = CreateFrameForHalfPlane_Station163x45(TString("frame_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            if(XStationRotations[stationNum] == true) module0_transform->RotateX(180.0);
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        if(XStationRotations[stationNum] == true) module1_transform->RotateX(180.0);
            module1_transform->ReflectX(true);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

       TGeoCombiTrans *layers0_transform = new TGeoCombiTrans();
            if(XStationRotations[stationNum] == true) layers0_transform->RotateX(180.0);
            layers0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *layers1_transform = new TGeoCombiTrans();
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

TGeoVolume *CreateLayersForModule_Station163x45(TString layer_name) {

    //frame volumes
    TGeoVolume *layers = new TGeoVolumeAssembly(layer_name);
    layers->SetMedium(pMedAir);

    //Common parameters for all layers
    Double_t layer_XSize = XModuleSize_Station163x45; //cm
    Double_t layer_YSize = YModuleSize_Station163x45; //cm

    //copper layer -------------------------------------------------------------
    Double_t copperLayer_ZSize = 0.006; //cm;
    Double_t copperLayerHole_Radius = 4.0; //cm

    //layer shape
    TGeoShape *copperLayerBlankS = new TGeoBBox(TString("copperLayerBlankS")+=TString("_") + layers->GetName(), layer_XSize*0.5, layer_YSize*0.5, copperLayer_ZSize*0.5);
    TGeoShape *copperLayerHoleS = new TGeoTube(TString("copperLayerHoleS")+=TString("_") + layers->GetName(), 0.0, copperLayerHole_Radius, copperLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *copperLayerHole_pos = new TGeoTranslation();
        copperLayerHole_pos->SetName("copperLayerHole_pos");
        copperLayerHole_pos->SetDx(-XModuleSize_Station163x45*0.5);
        copperLayerHole_pos->SetDy(-YModuleSize_Station163x45*0.5);
        copperLayerHole_pos->SetDz(0.0);
        copperLayerHole_pos->RegisterYourself();

    TGeoCompositeShape *copperLayerS = new TGeoCompositeShape();
    copperLayerS->SetName(TString("copperLayerS")+=TString("_") + layers->GetName());
    {
        TString expression = "copperLayerBlankS"; expression += TString("_") + layers->GetName();
            expression += "-copperLayerHoleS"; expression += TString("_") + layers->GetName();
            expression += ":copperLayerHole_pos";

        copperLayerS->MakeNode(expression);
        copperLayerS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *copperLayerV = new TGeoVolume(TString("copperLayerV")+=TString("_") + layers->GetName(), copperLayerS);

    //volume medium
    TGeoMedium *copperLayerV_medium = pMedCopper;
    if(copperLayerV_medium) {
        copperLayerV->SetMedium(copperLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for copperLayerV!");

    //volume visual property (transparency)
    copperLayerV->SetLineColor(TColor::GetColor("#ff7538"));
    copperLayerV->SetTransparency(0);

    TGeoCombiTrans *copperLayer_transf[2];

    copperLayer_transf[0] = new TGeoCombiTrans();
    copperLayer_transf[0]->SetDx(0.0);
    copperLayer_transf[0]->SetDy(0.0);
    copperLayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize*0.5);

    copperLayer_transf[1] = new TGeoCombiTrans();
    copperLayer_transf[1]->SetDx(0.0);
    copperLayer_transf[1]->SetDy(0.0);
    copperLayer_transf[1]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5);

    layers->AddNode(copperLayerV, 0, copperLayer_transf[0]);
    layers->AddNode(copperLayerV, 1, copperLayer_transf[1]);
    //--------------------------------------------------------------------------

    //glue layer ---------------------------------------------------------------
    Double_t glueLayer_ZSize = 0.01; //cm; half-thickness of all glue layers
    Double_t glueLayerHole_Radius = 4.0; //cm

    //layer shape
    TGeoShape *glueLayerBlankS = new TGeoBBox(TString("glueLayerBlankS")+=TString("_") + layers->GetName(), layer_XSize*0.5, layer_YSize*0.5, glueLayer_ZSize*0.5);
    TGeoShape *glueLayerHoleS = new TGeoTube(TString("glueLayerHoleS")+=TString("_") + layers->GetName(), 0.0, glueLayerHole_Radius, glueLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *glueLayerHole_pos = new TGeoTranslation();
        glueLayerHole_pos->SetName("glueLayerHole_pos");
        glueLayerHole_pos->SetDx(-XModuleSize_Station163x45*0.5);
        glueLayerHole_pos->SetDy(-YModuleSize_Station163x45*0.5);
        glueLayerHole_pos->SetDz(0.0);
        glueLayerHole_pos->RegisterYourself();

    TGeoCompositeShape *glueLayerS = new TGeoCompositeShape();
    glueLayerS->SetName(TString("glueLayerS")+=TString("_") + layers->GetName());
    {
        TString expression = "glueLayerBlankS"; expression += TString("_") + layers->GetName();
            expression += "-glueLayerHoleS"; expression += TString("_") + layers->GetName();
            expression += ":glueLayerHole_pos";

        glueLayerS->MakeNode(expression);
        glueLayerS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *glueLayerV = new TGeoVolume(TString("glueLayerV")+=TString("_") + layers->GetName(), glueLayerS);

    //volume medium
    TGeoMedium *glueLayerV_medium = pMedGlue;
    if(glueLayerV_medium) {
        glueLayerV->SetMedium(glueLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for glueLayerV!");

    //volume visual property (transparency)
    glueLayerV->SetLineColor(TColor::GetColor("#1919ff"));
    glueLayerV->SetTransparency(0);

    TGeoCombiTrans *glueLayer_transf[2];

    glueLayer_transf[0] = new TGeoCombiTrans();
    glueLayer_transf[0]->SetDx(0.0);
    glueLayer_transf[0]->SetDy(0.0);
    glueLayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize*0.5);

    glueLayer_transf[1] = new TGeoCombiTrans();
    glueLayer_transf[1]->SetDx(0.0);
    glueLayer_transf[1]->SetDy(0.0);
    glueLayer_transf[1]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize*0.5);

    layers->AddNode(glueLayerV, 0, glueLayer_transf[0]);
    layers->AddNode(glueLayerV, 1, glueLayer_transf[1]);
    //--------------------------------------------------------------------------

    //epoxide layer ------------------------------------------------------------
    Double_t epoxideLayer_ZSize = 0.1; //cm; half-thickness of all epoxide layers
    Double_t epoxideLayerHole_Radius = 4.0; //cm

    //layer shape
    TGeoShape *epoxideLayerBlankS = new TGeoBBox(TString("epoxideLayerBlankS")+=TString("_") + layers->GetName(), layer_XSize*0.5, layer_YSize*0.5, epoxideLayer_ZSize*0.5);
    TGeoShape *epoxideLayerHoleS = new TGeoTube(TString("epoxideLayerHoleS")+=TString("_") + layers->GetName(), 0.0, epoxideLayerHole_Radius, epoxideLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *epoxideLayerHole_pos = new TGeoTranslation();
        epoxideLayerHole_pos->SetName("epoxideLayerHole_pos");
        epoxideLayerHole_pos->SetDx(-XModuleSize_Station163x45*0.5);
        epoxideLayerHole_pos->SetDy(-YModuleSize_Station163x45*0.5);
        epoxideLayerHole_pos->SetDz(0.0);
        epoxideLayerHole_pos->RegisterYourself();

    TGeoCompositeShape *epoxideLayerS = new TGeoCompositeShape();
    epoxideLayerS->SetName(TString("epoxideLayerS")+=TString("_") + layers->GetName());
    {
        TString expression = "epoxideLayerBlankS"; expression += TString("_") + layers->GetName();
            expression += "-epoxideLayerHoleS"; expression += TString("_") + layers->GetName();
            expression += ":epoxideLayerHole_pos";

        epoxideLayerS->MakeNode(expression);
        epoxideLayerS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *epoxideLayerV = new TGeoVolume(TString("epoxideLayerV")+=TString("_") + layers->GetName(), epoxideLayerS);

    //volume medium
    TGeoMedium *epoxideLayerV_medium = pMedEpoxideCompound;
    if(epoxideLayerV_medium) {
        epoxideLayerV->SetMedium(epoxideLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for epoxideLayerV!");

    //volume visual property (transparency)
    epoxideLayerV->SetLineColor(TColor::GetColor("#34c924"));
    epoxideLayerV->SetTransparency(0);

    TGeoCombiTrans *epoxideLayer_transf[2];

    epoxideLayer_transf[0] = new TGeoCombiTrans();
    epoxideLayer_transf[0]->SetDx(0.0);
    epoxideLayer_transf[0]->SetDy(0.0);
    epoxideLayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize*0.5);

    epoxideLayer_transf[1] = new TGeoCombiTrans();
    epoxideLayer_transf[1]->SetDx(0.0);
    epoxideLayer_transf[1]->SetDy(0.0);
    epoxideLayer_transf[1]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize - epoxideLayer_ZSize*0.5) ;

    layers->AddNode(epoxideLayerV, 0, epoxideLayer_transf[0]);
    layers->AddNode(epoxideLayerV, 1, epoxideLayer_transf[1]);
    //--------------------------------------------------------------------------

    //honeycomb layer ----------------------------------------------------------
    Double_t honeycombLayer_ZSize = 1.5; //cm; half-thickness of all honeycomb layers
    Double_t honeycombLayerHole_Radius = 4.0; //cm

    //layer shape
    TGeoShape *honeycombLayerBlankS = new TGeoBBox(TString("honeycombLayerBlankS")+=TString("_") + layers->GetName(), layer_XSize*0.5, layer_YSize*0.5, honeycombLayer_ZSize*0.5);
    TGeoShape *honeycombLayerHoleS = new TGeoTube(TString("honeycombLayerHoleS")+=TString("_") + layers->GetName(), 0.0, honeycombLayerHole_Radius, honeycombLayer_ZSize*0.5 + 0.01);

    TGeoTranslation *honeycombLayerHole_pos = new TGeoTranslation();
        honeycombLayerHole_pos->SetName("honeycombLayerHole_pos");
        honeycombLayerHole_pos->SetDx(-XModuleSize_Station163x45*0.5);
        honeycombLayerHole_pos->SetDy(-YModuleSize_Station163x45*0.5);
        honeycombLayerHole_pos->SetDz(0.0);
        honeycombLayerHole_pos->RegisterYourself();

    TGeoCompositeShape *honeycombLayerS = new TGeoCompositeShape();
    honeycombLayerS->SetName(TString("honeycombLayerS")+=TString("_") + layers->GetName());
    {
        TString expression = "honeycombLayerBlankS"; expression += TString("_") + layers->GetName();
            expression += "-honeycombLayerHoleS"; expression += TString("_") + layers->GetName();
            expression += ":honeycombLayerHole_pos";

        honeycombLayerS->MakeNode(expression);
        honeycombLayerS->ComputeBBox(); //need to compute a bounding box
    }

    TGeoVolume *honeycombLayerV = new TGeoVolume(TString("honeycombLayerV")+=TString("_") + layers->GetName(), honeycombLayerS);

    //volume medium
    TGeoMedium *honeycombLayerV_medium = pMedHoneyComb;
    if(honeycombLayerV_medium) {
        honeycombLayerV->SetMedium(honeycombLayerV_medium);
    }
    else Fatal("Main", "Invalid medium for honeycombLayerV!");

    //volume visual property (transparency)
    honeycombLayerV->SetLineColor(TColor::GetColor("#ffdf84"));
    honeycombLayerV->SetTransparency(0);

    TGeoCombiTrans *honeycombLayer_transf[2];

    honeycombLayer_transf[0] = new TGeoCombiTrans();
    honeycombLayer_transf[0]->SetDx(0.0);
    honeycombLayer_transf[0]->SetDy(0.0);
    honeycombLayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + copperLayer_ZSize + glueLayer_ZSize + epoxideLayer_ZSize + honeycombLayer_ZSize*0.5);

    honeycombLayer_transf[1] = new TGeoCombiTrans();
    honeycombLayer_transf[1]->SetDx(0.0);
    honeycombLayer_transf[1]->SetDy(0.0);
    honeycombLayer_transf[1]->SetDz(+ZGEMSize_Station163x45*0.5 - copperLayer_ZSize*0.5 - glueLayer_ZSize - epoxideLayer_ZSize - honeycombLayer_ZSize*0.5) ;

    layers->AddNode(honeycombLayerV, 0, honeycombLayer_transf[0]);
    layers->AddNode(honeycombLayerV, 1, honeycombLayer_transf[1]);
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
        backLowerEpoxideFrameHalf_pos1->SetDy(-(backLowerEpoxideFrame_YSize*0.5));
        backLowerEpoxideFrameHalf_pos1->SetDz(0.0);
        backLowerEpoxideFrameHalf_pos1->RegisterYourself();

    TGeoTranslation *backLowerEpoxideFrameHalf_pos2 = new TGeoTranslation();
        backLowerEpoxideFrameHalf_pos2->SetName("backLowerEpoxideFrameHalf1_pos2");
        backLowerEpoxideFrameHalf_pos2->SetDx(-((backLowerEpoxideFrame_XSize*0.5 - backLowerEpoxideFrameArch_Rmin)*0.5 + backLowerEpoxideFrameArch_Rmin));
        backLowerEpoxideFrameHalf_pos2->SetDy(-(backLowerEpoxideFrame_YSize*0.5));
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
    backLowerEpoxideFrame_transf[0]->SetDy(-(YModuleSize_Station163x45*0.5) + 0.2/*margin*/);
    backLowerEpoxideFrame_transf[0]->SetDz(+(backLowerEpoxideFrame_ZSize*0.5 + ZGEMSize_Station163x45*0.5));

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
        frontLowerEpoxideFrameHalf_pos1->SetDy(-(frontLowerEpoxideFrame_YSize*0.5));
        frontLowerEpoxideFrameHalf_pos1->SetDz(0.0);
        frontLowerEpoxideFrameHalf_pos1->RegisterYourself();

    TGeoTranslation *frontLowerEpoxideFrameHalf_pos2 = new TGeoTranslation();
        frontLowerEpoxideFrameHalf_pos2->SetName("frontLowerEpoxideFrameHalf1_pos2");
        frontLowerEpoxideFrameHalf_pos2->SetDx(-((frontLowerEpoxideFrame_XSize*0.5 - frontLowerEpoxideFrameArch_Rmin)*0.5 + frontLowerEpoxideFrameArch_Rmin));
        frontLowerEpoxideFrameHalf_pos2->SetDy(-(frontLowerEpoxideFrame_YSize*0.5));
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
    frontLowerEpoxideFrame_transf[0]->SetDy(-(YModuleSize_Station163x45*0.5) + 0.2/*margin*/);
    frontLowerEpoxideFrame_transf[0]->SetDz(-(frontLowerEpoxideFrame_ZSize*0.5 + ZGEMSize_Station163x45*0.5));

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
    backSideEpoxideFrame_transf[0]->SetDz(+(backSideEpoxideFramePart_ZSize*0.5 + ZGEMSize_Station163x45*0.5));

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
    backUpperEpoxideFrame_transf[0]->SetDz(+(backUpperEpoxideFrame_ZSize*0.5 + ZGEMSize_Station163x45*0.5));

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
    frontSideEpoxideFrame_transf[0]->SetDz(-(frontSideEpoxideFramePart_ZSize*0.5 + ZGEMSize_Station163x45*0.5));

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
    frontUpperEpoxideFrame_transf[0]->SetDz(-(frontUpperEpoxideFrame_ZSize*0.5 + ZGEMSize_Station163x45*0.5));

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
    alumCableSupport_transf[0]->SetDz(-(alumCableSupportPart_ZSize*0.5 + ZGEMSize_Station163x45*0.5 + frontSideEpoxideFramePart_ZSize + 1.0/*shift*/));

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
    amplifierUpperBlock_transf[0]->SetDz(-(amplifierUpperBlockPart1_ZSize*0.5) + ZGEMSize_Station163x45*0.5);

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
    amplifierSideBlock_transf[0]->SetDz(-(amplifierSideBlockPartBig_ZSize*0.5) + ZGEMSize_Station163x45*0.5);

    amplifierSideBlock_transf[1] = new TGeoCombiTrans();
    amplifierSideBlock_transf[1]->RotateY(180.0);
    amplifierSideBlock_transf[1]->SetDx(-86.47/*shift*/);
    amplifierSideBlock_transf[1]->SetDy(-5.5);
    amplifierSideBlock_transf[1]->SetDz(-(amplifierSideBlockPartBig_ZSize*0.5) + ZGEMSize_Station163x45*0.5);

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
    upperPcb_transf[0]->SetDz(-(upperPcb_ZSize*0.5) + ZGEMSize_Station163x45*0.5 - ZGEMSize_Station163x45 - frontSideEpoxideFramePart_ZSize);

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
    sidePcb_transf[0]->SetDz(-(sidePcbPart_ZSize*0.5 + alumCableSupportPart_ZSize + ZGEMSize_Station163x45*0.5 + frontSideEpoxideFramePart_ZSize + 1.0/*shift*/));

    sidePcb_transf[1] = new TGeoCombiTrans();
    sidePcb_transf[1]->RotateY(180.0);
    sidePcb_transf[1]->SetDx(-(sidePcbPart_XSize*0.5 + 84.1/*shift*/));
    sidePcb_transf[1]->SetDy(-6.8/*shift*/);
    sidePcb_transf[1]->SetDz(-(sidePcbPart_ZSize*0.5 + alumCableSupportPart_ZSize + ZGEMSize_Station163x45*0.5 + frontSideEpoxideFramePart_ZSize + 1.0/*shift*/));

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
    alumSideSupport_transf[0]->SetDz(+(ZGEMSize_Station163x45*0.5 - alumSideSupport_ZSize*0.5));

    alumSideSupport_transf[1] = new TGeoCombiTrans();
    alumSideSupport_transf[1]->SetDx(-(alumSideSupport_XSize*0.5 + 88.31/*shift*/));
    alumSideSupport_transf[1]->SetDy(+1.8/*shift*/);
    alumSideSupport_transf[1]->SetDz(+(ZGEMSize_Station163x45*0.5 - alumSideSupport_ZSize*0.5));

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
    boschShape_transf[0]->SetDz(+(ZGEMSize_Station163x45*0.5 + boschShape_ZSize*0.5));

    boschShape_transf[1] = new TGeoCombiTrans();
    boschShape_transf[1]->SetDx(-(boschShape_XSize*0.5 + 90.86/*shift*/));
    boschShape_transf[1]->SetDy(+1.8/*shift*/);
    boschShape_transf[1]->SetDz(+(ZGEMSize_Station163x45*0.5 + boschShape_ZSize*0.5));

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
    upperBrasslayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + upperBrasslayer_ZSize*0.5 + 0.005/*shift*/);

    TGeoCombiTrans *sideBrasslayer_transf[2];

    sideBrasslayer_transf[0] = new TGeoCombiTrans();
    sideBrasslayer_transf[0]->SetDx(+(lowerFiberGlassBlock_XSize*0.5 + sideBrasslayer_XSize*0.5));
    sideBrasslayer_transf[0]->SetDy(+0.45/*shift*/);
    sideBrasslayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + sideBrasslayer_ZSize*0.5 + 0.005/*shift*/);

    sideBrasslayer_transf[1] = new TGeoCombiTrans();
    sideBrasslayer_transf[1]->SetDx(-(lowerFiberGlassBlock_XSize*0.5 + sideBrasslayer_XSize*0.5));
    sideBrasslayer_transf[1]->SetDy(+0.45/*shift*/);
    sideBrasslayer_transf[1]->SetDz(-ZGEMSize_Station163x45*0.5 + sideBrasslayer_ZSize*0.5 + 0.005/*shift*/);

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
    upperSteellayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + upperSteellayer_ZSize*0.5 + 0.005/*shift*/);

    TGeoCombiTrans *sideSteellayer_transf[2];

    sideSteellayer_transf[0] = new TGeoCombiTrans();
    sideSteellayer_transf[0]->SetDx(+(lowerFiberGlassBlock_XSize*0.5 + sideSteellayer_XSize*0.5));
    sideSteellayer_transf[0]->SetDy(+0.45/*shift*/);
    sideSteellayer_transf[0]->SetDz(-ZGEMSize_Station163x45*0.5 + sideSteellayer_ZSize*0.5);

    sideSteellayer_transf[1] = new TGeoCombiTrans();
    sideSteellayer_transf[1]->SetDx(-(lowerFiberGlassBlock_XSize*0.5 + sideSteellayer_XSize*0.5));
    sideSteellayer_transf[1]->SetDy(+0.45/*shift*/);
    sideSteellayer_transf[1]->SetDz(-ZGEMSize_Station163x45*0.5 + sideSteellayer_ZSize*0.5);

    frames->AddNode(upperSteellayerV, 0, upperSteellayer_transf[0]);

    frames->AddNode(sideSteellayerV, 0, sideSteellayer_transf[0]);
    frames->AddNode(sideSteellayerV, 1, sideSteellayer_transf[1]);
    //--------------------------------------------------------------------------

    return frames;
}