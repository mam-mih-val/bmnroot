/*
 * Author: Baranov D.
 * Created: 11.04.2022
 * Use this macro to create ROOT geometry of Large CSC (Cathode
 * Strip Chamber) detector for the RUN-8 configuration
 *
 * WARNING: all units is in cm!!!
 *
 */

#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

//Set Parameters of CSC --------------------------------------------------------
const Int_t NStations = 1;      //stations in the detector
const Int_t NMaxModules = 8;    //max. number of modules in a station

//(X-Y-Z)Positions of stations
const Double_t XStationPositions[NStations] = { 0.0/*0*/}; //geometry center
const Double_t YStationPositions[NStations] = { 0.0/*0*/}; //geometry center
const Double_t ZStationPositions[NStations] = { 800.0/*0*/}; //outer side nearest to the target

//(X-Y-Z)Shifts of modules in each station
const Double_t XModuleShifts[NStations][NMaxModules] = { //centers of gas volume
    {+81.375/*0*/, +27.125/*1*/, -27.125/*2*/, -81.375/*3*/, +81.375/*4*/, +27.125/*5*/, -27.125/*6*/, -81.375/*7*/} //station 0
};
const Double_t YModuleShifts[NStations][NMaxModules] = { //centers of gas volume
    {+36.5/*0*/, +36.5/*1*/, +36.5/*2*/, +36.5/*3*/, -36.5/*4*/, -36.5/*5*/, -36.5/*6*/, -36.5/*7*/} //station 0
};
const Double_t ZModuleShifts[NStations][NMaxModules] = { //distances from st.side to mod.side
    {1.25/*0*/, 1.25/*1*/, 1.25/*2*/, 1.25/*3*/, 1.25/*4*/, 1.25/*5*/, 1.25/*6*/, 1.25/*7*/} //station 0
};

enum ModuleType {WHOLE_MOD, HOLED_LEFT_MOD, HOLED_RIGHT_MOD};

const ModuleType ModuleTypes[NStations][NMaxModules] = {
    {WHOLE_MOD/*0*/, HOLED_LEFT_MOD/*1*/, HOLED_RIGHT_MOD/*2*/, WHOLE_MOD/*3*/, WHOLE_MOD/*4*/, HOLED_RIGHT_MOD/*5*/, HOLED_LEFT_MOD/*6*/, WHOLE_MOD/*7*/} //station 0
};

const Bool_t ModulesYRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, true/*4*/, true/*5*/, true/*6*/, true/*7*/} //station 0
};

const Bool_t ModulesXRotations[NStations][NMaxModules] = {
    {false/*0*/, false/*1*/, false/*2*/, false/*3*/, true/*4*/, true/*5*/, true/*6*/, true/*7*/} //station 0
};



//Sizes of elements (cm) -------------------------------------------------------

//Gas volume sizes (module)
const Double_t XGasSize = 54.25;
const Double_t YGasSize = 73.0;
const Double_t ZGasSize = 0.76;

const Double_t BeamHole_XSize = 11.125;
const Double_t BeamHole_YSize = 11.625;

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
TGeoVolume *CreateModule(TString module_name, ModuleType module_type);

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

void create_rootgeom_LargeCSC_Run8() {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "LargeCSC";
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

        TGeoVolume *station = CreateStation(Form("station%d", stationNum));

        for(Int_t imod = 0; imod < NMaxModules; ++imod) {

            TGeoVolume *module = CreateModule(Form("module%d_%s", imod, station->GetName()), ModuleTypes[istation][imod]);

            TGeoCombiTrans *module_transform = new TGeoCombiTrans(Form("transr_module%d_%s", imod, station->GetName()));

            if(ModulesYRotations[istation][imod]) module_transform->RotateY(180.0);
            if(ModulesXRotations[istation][imod]) module_transform->RotateX(180.0);

            module_transform->SetTranslation(XModuleShifts[stationNum][imod], YModuleShifts[stationNum][imod], ZModuleShifts[stationNum][imod]+0.5*ZGasSize);

            station->AddNode(module, 0, module_transform);

        }

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

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

TGeoVolume *CreateModule(TString module_name, ModuleType module_type) {

    //shapes
    TGeoShape *moduleS = 0;

    switch(module_type) {
        case ModuleType::WHOLE_MOD: {
            moduleS = new TGeoBBox(Form("moduleS_%s", module_name.Data()), XGasSize*0.5, YGasSize*0.5, ZGasSize*0.5);
            break;
        }
        case ModuleType::HOLED_LEFT_MOD:  {
            moduleS = new TGeoXtru(2);
            moduleS->SetName(Form("moduleS_%s", module_name.Data()));
                Double_t x_pos[6] = {+XGasSize*0.5, -XGasSize*0.5, -XGasSize*0.5, -XGasSize*0.5+BeamHole_XSize, -XGasSize*0.5+BeamHole_XSize, +XGasSize*0.5};
                Double_t y_pos[6] = {+YGasSize*0.5, +YGasSize*0.5, -YGasSize*0.5+BeamHole_YSize, -YGasSize*0.5+BeamHole_YSize, -YGasSize*0.5, -YGasSize*0.5};
                ((TGeoXtru*)moduleS)->DefinePolygon(6, x_pos, y_pos);
                ((TGeoXtru*)moduleS)->DefineSection(0, -ZGasSize*0.5);
                ((TGeoXtru*)moduleS)->DefineSection(1, +ZGasSize*0.5);
            break;
        }
        case ModuleType::HOLED_RIGHT_MOD:  {
            moduleS = new TGeoXtru(2);
            moduleS->SetName(Form("moduleS_%s", module_name.Data()));
                Double_t x_pos[6] = {-XGasSize*0.5, -XGasSize*0.5, +XGasSize*0.5, +XGasSize*0.5, +XGasSize*0.5-BeamHole_XSize, +XGasSize*0.5-BeamHole_XSize};
                Double_t y_pos[6] = {-YGasSize*0.5, +YGasSize*0.5, +YGasSize*0.5, -YGasSize*0.5+BeamHole_YSize, -YGasSize*0.5+BeamHole_YSize, -YGasSize*0.5};
                ((TGeoXtru*)moduleS)->DefinePolygon(6, x_pos, y_pos);
                ((TGeoXtru*)moduleS)->DefineSection(0, -ZGasSize*0.5);
                ((TGeoXtru*)moduleS)->DefineSection(1, +ZGasSize*0.5);
            break;
        }
        default:
            Error("CreateModule()", "Incorrect module type selected!!!");
    }

    //volumes
    TGeoVolume *moduleV = new TGeoVolume(Form("CSCSensor_%s", module_name.Data()), moduleS);

    //medium
    if(pMedArgonIsobutane7525) {
        moduleV->SetMedium(pMedArgonIsobutane7525);
    }
    else Fatal("Main", "Invalid medium for moduleV volume!");

    //volume visual property (transparency)
    moduleV->SetLineColor(TColor::GetColor("#47ffca"));
    moduleV->SetTransparency(30);

    return moduleV;
}
//------------------------------------------------------------------------------
