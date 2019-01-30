/*
 * Baranov D.
 * 16.05.2018
 * Use this macro to create ROOT geometry for the RunSpring2017 configuration (BM@N RUN-7)
 *
 */

#include <TFile.h>
#include <TTree.h>
#include <TRandom.h>
#include <TRandom2.h>
#include <TClonesArray.h>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;
//Set Parameters of GEMS -------------------------------------------------------
const Int_t NStations = 6; //stations in the detector
const Int_t NMaxModules = 2; //max. number of modules in a station

//(X-Y-Z)Positions of stations (sensitive volumes)
const Double_t XStationPositions[NStations] = {+0.5390, +0.7025, +1.9925, +3.0860, +3.7980, +4.5815};
const Double_t YStationPositions[NStations] = {+15.99873, +16.20573, +16.36073, +16.40473, +16.09373, +16.45473};
const Double_t ZStationPositions[NStations] = {+39.702, +64.535, +112.649, +135.330, +160.6635, +183.668};

//(X-Y-Z)Shifts of modules in each station
Double_t XModuleShifts[NStations][NMaxModules] = {
    {163.2 * 0.25, -163.2 * 0.25},
    {163.2 * 0.25, -163.2 * 0.25},
    {163.2 * 0.25, -163.2 * 0.25},
    {163.2 * 0.25, -163.2 * 0.25},
    {163.2 * 0.25, -163.2 * 0.25},
    {163.2 * 0.25, -163.2 * 0.25}
};
Double_t YModuleShifts[NStations][NMaxModules] = {
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0}
};
Double_t ZModuleShifts[NStations][NMaxModules] = {
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0},
    {0.0, 0.0}
};

//GEM plane sizes (66x41 type) -------------------------------------------------
const Double_t XModuleSize_Station66x41 = 66.0;
const Double_t YModuleSize_Station66x41 = 41.0;
const Double_t ZModuleSize_Station66x41 = 0.9;

const Double_t dXFrame_Station66x41 = 5.0;
const Double_t dYFrame_Station66x41 = 3.75;
const Double_t dZFrame_Station66x41 = 2.3;
//------------------------------------------------------------------------------

//GEM plane sizes (163x45 type) ------------------------------------------------
const Double_t XModuleSize_Station163x45 = 163.2 * 0.5;
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
Double_t*** generateMisalign(Double_t, Double_t, Double_t);
void createNewXML(Double_t***, TString, TString);

void createNewXML(Double_t*** mis, TString in, TString out) {
    ifstream input;
    input.open(in.Data(), ios::in);

    FILE* output = fopen(out.Data(), "w");

    Int_t stat = -1, mod = -1;
    Int_t statCounter = -1;

    for (string line; getline(input, line);) {
        TString str = TString(line.c_str());

        if (str.Contains("<Module")) {
            statCounter++;
            if (statCounter % 2 == 0)
                stat++;
            mod++;

            TString tmp = TString::Format("xShift=\"%G\" yShift=\"%G\" zShift=\"%G\"", mis[stat][mod][0], mis[stat][mod][1], mis[stat][mod][2]);
            str.ReplaceAll("xShift=\"0.0\" yShift=\"0.0\" zShift=\"0.0\"", tmp);
        }
        if (mod == 1)
            mod = -1;

        fprintf(output, "%s\n", str.Data());
    }
    fclose(output);
}

Double_t*** generateMisalign(Double_t sigX, Double_t sigY, Double_t sigZ) {
    FILE* file = fopen("introducedMisalign.dat", "w");
    gRandom->SetSeed(11);
    Double_t*** misAlign = new Double_t**[NStations];
    for (Int_t iStat = 0; iStat < NStations; iStat++) {
        misAlign[iStat] = new Double_t*[NMaxModules];
        for (Int_t iMod = 0; iMod < NMaxModules; iMod++) {
            misAlign[iStat][iMod] = new Double_t[3];
            for (Int_t iPar = 0; iPar < 3; iPar++) {
                misAlign[iStat][iMod][iPar] = gRandom->Gaus(0., (iPar == 0) ? sigX : (iPar == 1) ? sigY : sigZ);
                fprintf(file, "Stat %d Mod %d Par %d %G\n", iStat, iMod, iPar, misAlign[iStat][iMod][iPar]);
            }
        }
    }
    fclose(file);

    // Fill ROOT-container with misalign.
    TFile* f = new TFile("misAlignment.root", "RECREATE");
    TTree* outTree = new TTree("cbmsim", "cbmsim");
    TClonesArray* misalign = new TClonesArray("BmnGemAlignCorrections");
    outTree->Branch("BmnGemAlignCorrections", &misalign);
    for (Int_t iStat = 0; iStat < NStations; iStat++)
        for (Int_t iMod = 0; iMod < NMaxModules; iMod++)
            new ((*misalign)[misalign->GetEntriesFast()]) BmnGemAlignCorrections(iStat, iMod,
                TVector3(misAlign[iStat][iMod][0], misAlign[iStat][iMod][1], misAlign[iStat][iMod][2]));
    outTree->Fill();
    outTree->Write();
    f->Close();

    return misAlign;
}

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if (!mAir) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if (!pMedAir) Fatal("Main", "Medium air not found");

    //carbon medium
    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if (!mCarbon) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    pMedCarbon = gGeoManager->GetMedium("carbon");
    if (!pMedCarbon) Fatal("Main", "Medium carbon not found");

    //fiberglass medium
    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if (!mFiberGlass) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    pMedFiberGlass = gGeoManager->GetMedium("fiberglass");
    if (!pMedFiberGlass) Fatal("Main", "Medium fiberglass not found");

    //arco27020 medium
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if (!mArCO27030) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    pMedArCO27030 = gGeoManager->GetMedium("arco27030");
    if (!pMedArCO27030) Fatal("Main", "Medium arco27030 not found");
}

void create_rootgeom_GEMS_RunSpring2018_misAlign(Double_t sigX, Double_t sigY, Double_t sigZ,
						 TString geoVersion = "RunSpring2018_misAlign",
						 TString xmlMisalign = "GemRunSpring2018_misAlign.xml") {

    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // Add generated misalignment to GEM geometry
    for (Int_t iStat = 0; iStat < NStations; iStat++)
        for (Int_t iMod = 0; iMod < NMaxModules; iMod++) {
            Double_t* tmp = generateMisalign(sigX, sigY, sigZ)[iStat][iMod];
            XModuleShifts[iStat][iMod] += tmp[0];
            YModuleShifts[iStat][iMod] += tmp[1];
            ZModuleShifts[iStat][iMod] += tmp[2];
        }

    // Generate new XML-file according to introduced misalignment
    const TString xmlPath = gPath + "/parameters/gem/XMLConfigs/";
    createNewXML(generateMisalign(sigX, sigY, sigZ), xmlPath + "GemRunSpring2018.xml", xmlPath + xmlMisalign);

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = geoVersion;
    const TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";

    // ----  global geometry parameters  ---------------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();

    // -------   Load media from media file   ----------------------------------
    TString medFile = gPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    DefineRequiredMedia(geoMedia, geoBuild);
    // -------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
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

    //station 0 (big 163x45 GEM consisting two modules)
    if (1) {
        Int_t stationNum = 0; //station number

        TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        module1_transform->ReflectX(true);
        module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 1 (big 163x45 GEM consisting two modules)
    if (1) {
        Int_t stationNum = 1; //station number

        TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        module1_transform->ReflectX(true);
        module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 2 (big 163x45 GEM consisting two modules)
    if (1) {
        Int_t stationNum = 2; //station number

        TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        module1_transform->ReflectX(true);
        module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 3 (big 163x45 GEM consisting two modules)
    if (1) {
        Int_t stationNum = 3; //station number

        TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        module1_transform->ReflectX(true);
        module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 4 (big 163x45 GEM consisting two modules)
    if (1) {
        Int_t stationNum = 4; //station number

        TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        module1_transform->ReflectX(true);
        module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

        GEMS->AddNode(station, 0, station_transform);
    }

    //station 5 (big 163x45 GEM consisting two modules)
    if (1) {
        Int_t stationNum = 5; //station number

        TGeoVolume *station = CreateStation(TString("station") + TString::Itoa(stationNum, 10));
        TGeoVolume *module0 = CreateModule_Station163x45(TString("Sensor_module0_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *module1 = CreateModule_Station163x45(TString("Sensor_module1_") + station->GetName(), XModuleSize_Station163x45, YModuleSize_Station163x45, ZModuleSize_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame0 = CreateFrameForModule_Station163x45(TString("frame0_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);
        TGeoVolume *frame1 = CreateFrameForModule_Station163x45(TString("frame1_") + station->GetName(), dXFrame_Station163x45, dYFrame_Station163x45, dZFrame_Station163x45, 4.0 + 1.75);

        TGeoCombiTrans *module0_transform = new TGeoCombiTrans();
        module0_transform->SetTranslation(XModuleShifts[stationNum][0], YModuleShifts[stationNum][0], ZModuleShifts[stationNum][0] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *module1_transform = new TGeoCombiTrans();
        module1_transform->ReflectX(true);
        module1_transform->SetTranslation(XModuleShifts[stationNum][1], YModuleShifts[stationNum][1], ZModuleShifts[stationNum][1] + ZModuleSize_Station163x45 * 0.5);

        TGeoCombiTrans *station_transform = new TGeoCombiTrans();
        station_transform->SetTranslation(XStationPositions[stationNum], YStationPositions[stationNum], ZStationPositions[stationNum]);

        station->AddNode(module0, 0, new TGeoCombiTrans(*module0_transform)); //module
        station->AddNode(module1, 0, new TGeoCombiTrans(*module1_transform)); //module

        station->AddNode(frame0, 0, new TGeoCombiTrans(*module0_transform)); //frame
        station->AddNode(frame1, 0, new TGeoCombiTrans(*module1_transform)); //frame

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
    //top->Draw("ogl");
}
//------------------------------------------------------------------------------

TGeoVolume *CreateStation(TString station_name) {

    TGeoVolume *stationA = new TGeoVolumeAssembly(station_name);
    stationA->SetMedium(pMedAir);

    return stationA;
}

TGeoVolume *CreateModule_Station66x41(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize) {

    //module shape
    TGeoShape *moduleS = new TGeoBBox("moduleS", xsize * 0.5, ysize * 0.5, zsize * 0.5);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if (pMedArCO27030) moduleV->SetMedium(pMedArCO27030);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}

TGeoVolume *CreateFrameForModule_Station66x41(TString frame_name, Double_t dx, Double_t dy, Double_t dz) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dx * 0.5, YModuleSize_Station66x41 * 0.5 + dy, dz * 0.5);
    TGeoShape *horizontal_frameS = new TGeoBBox("horizontal_frameS", XModuleSize_Station66x41 * 0.5, dy * 0.5, dz * 0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name + "_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_frameV = new TGeoVolume(frame_name + "_horizontal_frameV", horizontal_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if (pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_frameV->SetMedium(frame_medium);
    } else Fatal("Main", "Invalid medium for frames!");
    if (pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    } else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station66x41 * 0.5 + dXFrame_Station66x41 * 0.5, 0.0, 0.0);

    TGeoCombiTrans *right_vertical_frame_position = new TGeoCombiTrans();
    right_vertical_frame_position->SetTranslation(-XModuleSize_Station66x41 * 0.5 - dXFrame_Station66x41 * 0.5, 0.0, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station66x41 * 0.5 + dYFrame_Station66x41 * 0.5, 0.0);

    TGeoCombiTrans *lower_horizontal_frame_position = new TGeoCombiTrans();
    lower_horizontal_frame_position->SetTranslation(0.0, -YModuleSize_Station66x41 * 0.5 - dYFrame_Station66x41 * 0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(vertical_frameV, 1, right_vertical_frame_position);
    composite_frameV->AddNode(horizontal_frameV, 0, bottom_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_frameV, 1, lower_horizontal_frame_position);

    return composite_frameV;
}

TGeoVolume *CreateModule_Station163x45(TString module_name, Double_t xsize, Double_t ysize, Double_t zsize, Double_t hole_radius) {

    //module shapes
    TGeoShape *module_partS = new TGeoBBox("module_partS", xsize * 0.5, ysize * 0.5, zsize * 0.5);
    TGeoShape *holeS = new TGeoTube("holeS", 0.0, hole_radius, zsize * 0.5 + 0.01);

    gRandom->SetSeed(0);
    Int_t uniqueId = gRandom->Integer(10000);
    TGeoTranslation *hole_module_trans = new TGeoTranslation(TString("hole_module_trans_") + TString::Itoa(uniqueId, 10), -xsize * 0.5, -ysize * 0.5, 0.0);

    hole_module_trans->RegisterYourself();

    TString bool_expression_module = TString(module_partS->GetName()) + TString(" - ") + TString(holeS->GetName()) + TString(":") + TString(hole_module_trans->GetName());
    TGeoCompositeShape *moduleS = new TGeoCompositeShape(bool_expression_module);

    //module volume
    TGeoVolume *moduleV = new TGeoVolume(module_name, moduleS);

    //medium
    if (pMedArCO27030) moduleV->SetMedium(pMedArCO27030);
    else Fatal("Main", "Invalid medium for sensitive modules!");

    //visual parameters
    moduleV->SetLineColor(TColor::GetColor("#ff47ca"));
    moduleV->SetTransparency(20);

    return moduleV;
}

TGeoVolume *CreateFrameForModule_Station163x45(TString frame_name, Double_t dx, Double_t dy, Double_t dz, Double_t hole_radius) {

    //frame shapes
    TGeoShape *vertical_frameS = new TGeoBBox("vertical_frameS", dx * 0.5, YModuleSize_Station163x45 * 0.5 + dy, dz * 0.5);
    TGeoShape *horizontal_upper_frameS = new TGeoBBox("horizontal_upper_frameS", XModuleSize_Station163x45 * 0.5, dy * 0.5, dz * 0.5);
    TGeoShape *horizontal_bottom_frameS = new TGeoBBox("horizontal_upper_frameS", XModuleSize_Station163x45 * 0.5 - hole_radius * 0.5, dy * 0.5, dz * 0.5);

    //frame volumes
    TGeoVolume *composite_frameV = new TGeoVolumeAssembly(frame_name);
    TGeoVolume *vertical_frameV = new TGeoVolume(frame_name + "_vertical_frameV", vertical_frameS);
    TGeoVolume *horizontal_upper_frameV = new TGeoVolume(frame_name + "_horizontal_upper_frameV", horizontal_upper_frameS);
    TGeoVolume *horizontal_bottom_frameV = new TGeoVolume(frame_name + "_horizontal_bottom_frameV", horizontal_bottom_frameS);

    //media
    TGeoMedium *frame_medium = pMedCarbon; //set medium
    if (pMedCarbon) {
        vertical_frameV->SetMedium(frame_medium);
        horizontal_upper_frameV->SetMedium(frame_medium);
        horizontal_bottom_frameV->SetMedium(frame_medium);
    } else Fatal("Main", "Invalid medium for frames!");
    if (pMedAir) {
        composite_frameV->SetMedium(pMedAir);
    } else Fatal("Main", "Invalid medium for frames!");

    //visual parameters
    vertical_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_upper_frameV->SetLineColor(TColor::GetColor("#9999ff"));
    horizontal_bottom_frameV->SetLineColor(TColor::GetColor("#9999ff"));

    //frame part positions in the frame
    TGeoCombiTrans *left_vertical_frame_position = new TGeoCombiTrans();
    left_vertical_frame_position->SetTranslation(XModuleSize_Station163x45 * 0.5 + dXFrame_Station163x45 * 0.5, 0.0, 0.0);

    TGeoCombiTrans *upper_horizontal_frame_position = new TGeoCombiTrans();
    upper_horizontal_frame_position->SetTranslation(0.0, YModuleSize_Station163x45 * 0.5 + dYFrame_Station163x45 * 0.5, 0.0);

    TGeoCombiTrans *bottom_horizontal_frame_position = new TGeoCombiTrans();
    bottom_horizontal_frame_position->SetTranslation(hole_radius * 0.5, -YModuleSize_Station163x45 * 0.5 - dYFrame_Station163x45 * 0.5, 0.0);

    composite_frameV->AddNode(vertical_frameV, 0, left_vertical_frame_position);
    composite_frameV->AddNode(horizontal_upper_frameV, 0, upper_horizontal_frame_position);
    composite_frameV->AddNode(horizontal_bottom_frameV, 0, bottom_horizontal_frame_position);

    return composite_frameV;
}
