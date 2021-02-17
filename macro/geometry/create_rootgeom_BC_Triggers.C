#include "TGeoVolume.h"
#include "TGeoEltu.h"

// http://bmnshift.jinr.ru/wiki/lib/exe/fetch.php?media=src_bm_n_55session.pdf
//---------------------------
// Geometry of BC triggers (more detailed)
// Created by A.Driuk andredryuk@gmail.com
// Date: 06.12.2020
//

//Triggers are centred according the beam position?????
const Int_t NStations = 4;
const Double_t Zshift = -647.5; 
const Double_t Xshift = 0.5;
const Double_t Yshift = -4.7; 

//(X-Y-Z)Positions of stations (sensitive volumes) -----------------------------
const Double_t StationPositions[NStations][3] = {
    {-0.0125 + Xshift, -0.3245 + Yshift, -172.0000 + Zshift}, //BC1
    {-0.4735 + Xshift, +3.5355 + Yshift, -85.50000 + Zshift}, //BC2
    {-0.0955 + Xshift, -0.8540 + Yshift, +167.8390 + Zshift}, //BC3
    {+0.0360 + Xshift,           Yshift, +188.5850 + Zshift} //BC4 было {+0.0360 + Xshift, +2.1435 + Yshift, +188.5850 + Zshift}
};

// X-Y Z Sizes of the triggers
const Double_t StationLength[NStations][3] = {
    {15.000, 15.000, 0.3},//{21.415, 21.415, 0.3}, //BC1
    {12.653, 12.653, 0.8}, //BC2
    { 8.507, 13.007, 0.5}, //BC3
    { 8.402,  8.402, 0.5} //BC4
};

//Angle Rotation of Station along X, Y, Z direction
const Double_t Rotation[NStations][3] = {
    { 0.0,  0.0, 0.0}, //BC1
    { 90.0,  45.0, 0.0}, //BC2
    { 0.0,  0.0, 0.0}, //BC3
    { 0.0,  0.0, 0.0}  //BC4
};

//GeoManager
TGeoManager* gGeoMan = NULL;

//media
TGeoMedium *pMedAir = 0;
TGeoMedium* pMedBC408 = 0;

class FairGeoMedia;
class FairGeoBuilder;

TGeoVolume *CreateStation(TString station_name);


void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {

    //air medium
    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    pMedAir = gGeoManager->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");
  
    FairGeoMedium* mBC408 = geoMedia->getMedium("BC408");
    if ( ! mBC408 ) Fatal("Main", "FairMedium BC408 not found");
    geoBuild->createMedium(mBC408);
    pMedBC408 = gGeoManager->GetMedium("BC408");
    if ( ! pMedBC408 ) Fatal("Main", "Medium BC408 not found");
}

void create_rootgeom_BC_Triggers() {
    // ----  set working directory  --------------------------------------------
    TString gPath = gSystem->Getenv("VMCWORKDIR");

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "BC";
    const TString geoDetectorVersion = "RunSRCSpring2018";
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

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    // Define TOP Geometry
    TGeoVolume* BC = new TGeoVolumeAssembly(geoDetectorName);
    BC->SetMedium(pMedAir);
    BC->SetTransparency(50);

    if(1) {
        Int_t iStat = 0; //station number
        TGeoTranslation *BCPos_trans1 = new TGeoTranslation("BC1Pos_trans", StationPositions[iStat][0], StationPositions[iStat][1], StationPositions[iStat][2]);		
        //TGeoBBox *bodyBC1 = new TGeoBBox("bodyBC1", StationLength[iStat][0]/2.,  StationLength[iStat][1]/2., StationLength[iStat][2]/2.);
        TGeoEltu *bodyBC1 = new TGeoEltu("bodyBC1", StationLength[iStat][0]/2.,  StationLength[iStat][1]/2., StationLength[iStat][2]/2.);
        TGeoVolume *BC1ActiveVolumeV1 = new TGeoVolume("BC1ActiveVolumeV1", bodyBC1);
        BC1ActiveVolumeV1->SetMedium(pMedBC408);
        BC1ActiveVolumeV1->SetLineColor(kGray);
        TGeoRotation* DetPos_rotBC1 = new TGeoRotation("DetPos_rotBC1", Rotation[iStat][0], Rotation[iStat][1], Rotation[iStat][2]);
        BC->AddNode(BC1ActiveVolumeV1, 0, new TGeoCombiTrans(*BCPos_trans1, *DetPos_rotBC1));
    }

    if(1) {
        Int_t iStat = 1; //station number
        TGeoTranslation *BCPos_trans2 = new TGeoTranslation("BC2Pos_trans", StationPositions[iStat][0], StationPositions[iStat][1], StationPositions[iStat][2]);	
        TGeoBBox *bodyBC2 = new TGeoBBox("bodyBC2", StationLength[iStat][0]/2.,  StationLength[iStat][1]/2., StationLength[iStat][2]/2.);	
        TGeoVolume *BC2ActiveVolumeV1 = new TGeoVolume("BC2ActiveVolumeV1", bodyBC2);
        BC2ActiveVolumeV1->SetMedium(pMedBC408);
        BC2ActiveVolumeV1->SetLineColor(kGray);
        TGeoRotation* DetPos_rotBC2 = new TGeoRotation("DetPos_rotBC2", Rotation[iStat][0], Rotation[iStat][1], Rotation[iStat][2]);
        BC->AddNode(BC2ActiveVolumeV1, 0, new TGeoCombiTrans(*BCPos_trans2, *DetPos_rotBC2));
    }

    if(1) {
        Int_t iStat = 2; //station number
        TGeoTranslation *BCPos_trans3 = new TGeoTranslation("BC3Pos_trans", StationPositions[iStat][0], StationPositions[iStat][1], StationPositions[iStat][2]);		
        TGeoBBox *bodyBC3 = new TGeoBBox("bodyBC3", StationLength[iStat][0]/2.,  StationLength[iStat][1]/2., StationLength[iStat][2]/2.);	
        TGeoVolume *BC3ActiveVolumeV1 = new TGeoVolume("BC3ActiveVolumeV1", bodyBC3);
        BC3ActiveVolumeV1->SetMedium(pMedBC408);
        BC3ActiveVolumeV1->SetLineColor(kGray);
        TGeoRotation* DetPos_rotBC3 = new TGeoRotation("DetPos_rotBC3", Rotation[iStat][0], Rotation[iStat][1], Rotation[iStat][2]);
        BC->AddNode(BC3ActiveVolumeV1, 0,  new TGeoCombiTrans(*BCPos_trans3, *DetPos_rotBC3));
    }

    if(1) {
        Int_t iStat = 3; //station number
        TGeoTranslation *BCPos_trans4 = new TGeoTranslation("BC4Pos_trans", StationPositions[iStat][0], StationPositions[iStat][1], StationPositions[iStat][2]);		
        TGeoBBox *bodyBC4 = new TGeoBBox("bodyBC4", StationLength[iStat][0]/2., StationLength[iStat][1]/2., StationLength[iStat][2]/2.);	
        TGeoVolume *BC4ActiveVolumeV1 = new TGeoVolume("BC4ActiveVolumeV1", bodyBC4);
        BC4ActiveVolumeV1->SetMedium(pMedBC408);
        BC4ActiveVolumeV1->SetLineColor(kGray);
        TGeoRotation* DetPos_rotBC4 = new TGeoRotation("DetPos_rotBC4",Rotation[iStat][0], Rotation[iStat][1], Rotation[iStat][2]);
        BC->AddNode(BC4ActiveVolumeV1, 0,  new TGeoCombiTrans(*BCPos_trans4, *DetPos_rotBC4));
    }

    top->AddNode(BC, 0);
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
