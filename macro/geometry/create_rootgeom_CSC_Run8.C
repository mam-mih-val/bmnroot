/*
 * Baranov D.
 * 12.04.2021
 * Use this macro to create ROOT geometry of CSC (Cathode
 * Strip Chamber) detector for the RUN-8 configuration
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

//Set Parameters of CSC --------------------------------------------------------
const Int_t NStations = 4;      //stations in the detector
const Int_t NMaxModules = 2;    //max. number of modules in a station

//(X-Y-Z)Positions of stations
const Double_t XStationPositions[NStations] = { 110.0/*0*/, -110.0/*1*/, 130.0/*2*/, -130.0/*3*/}; //geometry center
const Double_t YStationPositions[NStations] = { 0.0/*0*/, 0.0/*1*/, 0.0/*2*/, 0.0/*3*/}; //geometry center
const Double_t ZStationPositions[NStations] = { 410.0/*0*/, 410.0/*1*/, 470.0/*2*/, 470.0/*3*/}; //outer side nearest to the target

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = { //centers of gas volume
    {0.0, 0.0}, //station 0
    {0.0, 0.0}, //station 1
    {0.0, 0.0}, //station 2
    {0.0, 0.0}  //station 3
};
const Double_t YModuleShifts[NStations][NMaxModules] = { //centers of gas volume
    {27.1875, -27.1875}, //station 0
    {27.1875, -27.1875}, //station 1
    {27.1875, -27.1875}, //station 2
    {27.1875, -27.1875}  //station 3
};
const Double_t ZModuleShifts[NStations][NMaxModules] = { //distances from st.side to mod.side
    {1.25, 1.25}, //station 0
    {1.25, 1.25}, //station 0
    {1.25, 1.25}, //station 0
    {1.25, 1.25}  //station 0
};

//Sizes of elements (cm) -------------------------------------------------------

//Gas volume sizes (module: half of station)
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

void create_rootgeom_CSC_Run8() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "CSC";
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
    TGeoVolume* CSC = new TGeoVolumeAssembly(geoDetectorName);
    CSC->SetMedium(pMedAir);

    for(Int_t istation = 0; istation < NStations; ++istation) {

        Int_t stationNum = istation; //station number

        TGeoVolume *station = CreateStation(TString("station")+TString::Itoa(stationNum, 10));

        TGeoVolume *module0 = CreateModule(TString("module0_")+station->GetName());
        TGeoVolume *module1 = CreateModule(TString("module1_")+station->GetName());

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
            module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0]+0.5*ZGasSize);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
            module1_transform->RotateZ(180);
            module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1]+0.5*ZGasSize);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform));
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform));

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
    TGeoVolume *sensV = new TGeoVolume(TString("CSCSensor")+=TString("_") + module_name, sensS);

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
