#include <iomanip>
#include <iostream>
#include "TGeoManager.h"

#include "TGeoTube.h"
#include "TGeoPara.h"
#include "TGeoCone.h"
#include "TGeoTrd2.h"
#include "TGeoCompositeShape.h"

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
//---------------------------

TGeoManager* gGeoMan = NULL;

//Detector's position

const Double_t Fd_X_pos = 46.0;
const Double_t Fd_Y_pos = 0.0;
const Double_t Fd_Z_pos = 850.0;
 
//

const Double_t Qu_X_size = 15.0;
const Double_t Qu_Y_size = 15.0;
const Double_t Qu_Z_size = 0.1;

void create_rootgeom_FD_run8() {
    
  // Load the necessary FairRoot libraries 
  // gROOT->LoadMacro("home/lashmanov/prog/bmnroot/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries
    
    // -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "FD";
    const TString geoDetectorVersion = "run8";
    TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------  

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");
  
    FairGeoMedium* mlead  = geoMedia->getMedium("lead");
    if ( ! mlead ) Fatal("Main", "FairMedium lead   not found");
    geoBuild->createMedium(mlead);
    TGeoMedium* pMedlead   = gGeoMan->GetMedium("lead");
    if ( ! pMedlead ) Fatal("Main", "Medium lead  not found");
	
	FairGeoMedium* maluminium = geoMedia->getMedium("aluminium");
    if ( ! maluminium ) Fatal("Main", "FairMedium aluminium   not found");
    geoBuild->createMedium(maluminium);
    TGeoMedium* pMedaluminium  = gGeoMan->GetMedium("aluminium");
    if ( ! pMedaluminium ) Fatal("Main", "Medium aluminium  not found");
	
	FairGeoMedium* mPMTvacuum = geoMedia->getMedium("PMTvacuum");
    if ( ! mPMTvacuum ) Fatal("Main", "FairMedium PMTvacuum   not found");
    geoBuild->createMedium(mPMTvacuum);
    TGeoMedium* pMedPMTvacuum  = gGeoMan->GetMedium("PMTvacuum");
    if ( ! pMedPMTvacuum ) Fatal("Main", "Medium PMTvacuum  not found");
	
	FairGeoMedium* mFusedSil = geoMedia->getMedium("FusedSil");
    if ( ! mFusedSil ) Fatal("Main", "FairMedium FusedSil   not found");
    geoBuild->createMedium(mFusedSil);
    TGeoMedium* pMedFusedSil  = gGeoMan->GetMedium("FusedSil");
    if ( ! pMedFusedSil ) Fatal("Main", "Medium FusedSil  not found");
	
	FairGeoMedium* mFusedSilicon = geoMedia->getMedium("FusedSilicon");
    if ( ! mFusedSilicon ) Fatal("Main", "FairMedium FusedSilicon   not found");
    geoBuild->createMedium(mFusedSilicon);
    TGeoMedium* pMedFusedSilicon  = gGeoMan->GetMedium("FusedSilicon");
    if ( ! pMedFusedSilicon ) Fatal("Main", "Medium FusedSilicon  not found");
    
    // --------------------------------------------------------------------------
    
    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedPMTvacuum);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------
	
    // Define TOP Geometry
    TGeoVolume* FD_Top = new TGeoVolumeAssembly(geoDetectorName);
    //TGeoVolume* Qu_Top_1 = new TGeoVolumeAssembly(geoDetectorName);
    FD_Top->SetMedium(pMedAir);
   
	TGeoTranslation *Fd_pos = new TGeoTranslation("Fd_pos", Fd_X_pos, Fd_Y_pos, Fd_Z_pos);
	
	TGeoCombiTrans *Null_pos = new TGeoCombiTrans("Null_pos", 0, 0, 0, new TGeoRotation("rot01",0,0,0));
	Null_pos->RegisterYourself();
	TGeoCombiTrans *Null_pos1 = new TGeoCombiTrans("Null_pos1", 0, 0, 0, new TGeoRotation("rot02",0,0,0));
	Null_pos1->RegisterYourself();
	
	TGeoCombiTrans *Quartz_pos = new TGeoCombiTrans("Quartz_pos", 0, 0, 0, new TGeoRotation("rot1",0,0,0));
	Quartz_pos->RegisterYourself();

	TGeoCombiTrans *PMT_tube_pos = new TGeoCombiTrans("PMT_pos", 0, -65, 0, new TGeoRotation("rot2",0,90,0));
	PMT_tube_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_1_1_pos = new TGeoCombiTrans("panel_1_1_pos", 0, 0, 6, new TGeoRotation("rot3",0,0,0));
	panel_1_1_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_1_2_pos = new TGeoCombiTrans("panel_1_2_pos", 0, 0, -6, new TGeoRotation("rot4",0,0,0));
	panel_1_2_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_2_left_pos = new TGeoCombiTrans("panel_2_left_pos", 11, 0, 0, new TGeoRotation("rot5",90,90,0));
	panel_2_left_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_2_right_pos = new TGeoCombiTrans("panel_2_right_pos", -11, 0, 0, new TGeoRotation("rot6",90,90,0));
	panel_2_right_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_2_down_pos = new TGeoCombiTrans("panel_2_down_pos", 0, -11, 0, new TGeoRotation("rot7",0,90,0));
	panel_2_down_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_2_up_pos = new TGeoCombiTrans("panel_2_up_pos", 0, 11, 0, new TGeoRotation("rot8",0,90,0));
	panel_2_up_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_3_1_pos = new TGeoCombiTrans("panel_3_1_pos", 0, -31, 6, new TGeoRotation("rot9",0,0,0));
	panel_3_1_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_3_2_pos = new TGeoCombiTrans("panel_3_2_pos", 0, -31, -6, new TGeoRotation("rot10",0,0,0));
	panel_3_2_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_4_1_pos = new TGeoCombiTrans("panel_4_1_pos", 11, -31, 0, new TGeoRotation("rot11",90,90,90));
	panel_4_1_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_4_2_pos = new TGeoCombiTrans("panel_4_2_pos", -11, -31, 0, new TGeoRotation("rot12",90,90,90));
	panel_4_2_pos->RegisterYourself();
	
	TGeoCombiTrans *panel_4_2_d_pos = new TGeoCombiTrans("panel_4_2_d_pos", 0, -51, 0, new TGeoRotation("rot13",0,90,0));
	panel_4_2_d_pos->RegisterYourself();
	
//
	
	TGeoBBox *Qu_box = new TGeoBBox("Qu_box", Qu_X_size/2, Qu_Y_size/2, Qu_Z_size/2);
	TGeoTube *PMT_tube = new TGeoTube("PMT_tube", 0, 7.5/2, 28/2);
	
	TGeoBBox *panel_1 = new TGeoBBox("panel_1", 22/2, 22/2, 0.2/2);
	TGeoBBox *panel_1_cut = new TGeoBBox("panel_1_cut", 18/2, 18/2, 1.0/2);
	TGeoBBox *panel_2 = new TGeoBBox("panel_2", 22/2, 12/2, 0.2/2);
	TGeoBBox *panel_2_cut = new TGeoBBox("panel_2_cut", 18/2, 8/2, 1.0/2);
	
	TGeoBBox *panel_2_l = new TGeoBBox("panel_2_l", 26/2, 16/2, 0.4/2);
	
	TGeoBBox *panel_3 = new TGeoBBox("panel_3", 22/2, 40/2, 0.2/2);
	TGeoBBox *panel_4 = new TGeoBBox("panel_4", 12/2, 40/2, 0.2/2);
	
	TGeoCompositeShape *Geom_Cut_1 = new TGeoCompositeShape("Geom_Cut_1", "panel_1:Null_pos - panel_1_cut:Null_pos1");
	TGeoCompositeShape *Geom_Cut_2 = new TGeoCompositeShape("Geom_Cut_2", "panel_2:Null_pos - panel_2_cut:Null_pos1");
	
	TGeoCompositeShape *Al_head = new TGeoCompositeShape("Al_head", "Geom_Cut_1:panel_1_1_pos + Geom_Cut_1:panel_1_2_pos + Geom_Cut_2:panel_2_right_pos + Geom_Cut_2:panel_2_left_pos + panel_2_l:panel_2_down_pos + panel_2:panel_2_up_pos + panel_3:panel_3_1_pos + panel_3:panel_3_2_pos + panel_4:panel_4_1_pos + panel_4:panel_4_2_pos + panel_2_l:panel_4_2_d_pos");
	
//
	
	TGeoVolume *Qu_ActiveVolumeV = new TGeoVolume("Qu_ActiveVolumeV", Qu_box);
    Qu_ActiveVolumeV->SetMedium(pMedFusedSil);
    Qu_ActiveVolumeV->SetLineColor(kAzure+5);
	
	TGeoVolume *Pmt_tube_mat = new TGeoVolume("Pmt_tube_mat", PMT_tube);
    Pmt_tube_mat->SetMedium(pMedAir);
    Pmt_tube_mat->SetLineColor(kBlue+4);
	
	TGeoVolume *Head_mat = new TGeoVolume("Head_mat", Al_head);
    Head_mat->SetMedium(pMedaluminium);
    Head_mat->SetLineColor(kGray+1);
    //Head_mat->SetLineColor(kBlack);
	
	//Adding volumes to the TOP Volume
    
	top->AddNode(FD_Top, 1, Fd_pos);
	FD_Top->AddNode(Qu_ActiveVolumeV, 1, Quartz_pos);	
	FD_Top->AddNode(Pmt_tube_mat, 1, PMT_tube_pos);	
	FD_Top->AddNode(Head_mat, 1);	
	
    top->SetVisContainers(kTRUE);
    
	//CheckVolume(top);
        
    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
   // top->Draw("");
	
}
