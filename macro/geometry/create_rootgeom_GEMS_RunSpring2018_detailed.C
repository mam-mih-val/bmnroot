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
TGeoMedium *pMedAluminium = 0;
TGeoMedium *pMedEpoxideCompound = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);
TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize);
TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz);
TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius);
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
    FairGeoMedium* mEpoxideCompound = geoMedia->getMedium("air"); // fix: temporary material
    if ( ! mEpoxideCompound  ) Fatal("Main", "FairMedium epoxideCompound not found");
    geoBuild->createMedium(mEpoxideCompound);
    pMedEpoxideCompound= gGeoManager->GetMedium("air"); // fix: temporary material
    if ( ! pMedEpoxideCompound  ) Fatal("Main", "Medium epoxideCompound not found");

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

        TGeoVolume *frame = CreateFrameForHalfPlane_Station163x45(TString("frame_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            if(XStationRotations[stationNum] == true) module0_transform->RotateX(180.0);
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        if(XStationRotations[stationNum] == true) module1_transform->RotateX(180.0);
            module1_transform->ReflectX(true);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+ZModuleSize_Station163x45*0.5);

        TGeoCombiTrans *frame_transform = new TGeoCombiTrans();
        if(YStationRotations[stationNum] == true) frame_transform->RotateY(180.0);
        if(XStationRotations[stationNum] == true) frame_transform->RotateX(180.0);
        frame_transform->SetTranslation((XModuleShifts[stationNum][0] + XModuleShifts[stationNum][1]), YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+ZModuleSize_Station163x45*0.5);


        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

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

    return frames;
}