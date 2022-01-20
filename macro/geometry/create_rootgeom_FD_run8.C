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

const Double_t FD_Xpos = 38.0;
const Double_t FD_Ypos = 0.0;
const Double_t FD_Zpos = 800.0; //cm

// const Double_t Qu_Xpos = 38.0;
// const Double_t Qu_Ypos = 0.0;
// const Double_t Qu_Zpos = 900.0; //cm

const Double_t Qu_Xsize = 16.0; //cm
const Double_t Qu_Ysize = 16.0; //cm
const Double_t Qu_Zsize = 0.1; //cm

const Double_t Box_Zsize = 25; //cm
const Double_t Box_Ysize = 25.; //cm
const Double_t Box_Xsize = 10.; //cm


void create_rootgeom_FD_run8() {
        
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
    const TString geoDetectorVersion = "run8_v1";
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
    FD_Top->SetMedium(pMedPMTvacuum);
   
    //Transformations (translations, rotations and scales)
   // TGeoTranslation *Qu_trans = new TGeoTranslation("Qu_trans", Qu_Xpos, Qu_Ypos, Qu_Zpos);
   // TGeoTranslation *Pmt_trans = new TGeoTranslation("Pmt_trans", Qu_Xpos, -2, Qu_Zpos);
	
	//TGeoCombiTrans *Quartz_trans = new TGeoCombiTrans("Quartz_trans", 0, 0, 2.8, new TGeoRotation("rot1",0,40,0));
	TGeoCombiTrans *All_trans = new TGeoCombiTrans("All_trans", FD_Xpos, FD_Ypos, FD_Zpos, new TGeoRotation("rot1", 0, 32, 0));
	All_trans->RegisterYourself();
	
	TGeoCombiTrans *Quartz_trans = new TGeoCombiTrans("Quartz_trans", 0, 0, 0, new TGeoRotation("rot2",0,0,0));
	Quartz_trans->RegisterYourself();
	
	//TGeoCombiTrans *PMT_tube_trans = new TGeoCombiTrans("PMT_tube_trans", 0, -29.4, 19, new TGeoRotation("rot3",0,60,0));
	TGeoCombiTrans *PMT_tube_trans = new TGeoCombiTrans("PMT_tube_trans", 0, -31.9, 20.9, new TGeoRotation("rot3",0,60,0));
	PMT_tube_trans->RegisterYourself();
	
	//TGeoCombiTrans *Pmt_trans = new TGeoCombiTrans("Pmt_trans", 0, -12.3, 10, new TGeoRotation("rot2",0,40,0));
	TGeoCombiTrans *Pmt_trans = new TGeoCombiTrans("Pmt_trans", 0, -8.5, 7, new TGeoRotation("rot4",0,60,0));
	Pmt_trans->RegisterYourself();
	
	TGeoCombiTrans *Plane_trans = new TGeoCombiTrans("Plane_trans", 0, 0, -7, new TGeoRotation("rot5",0,0,0));
	Plane_trans->RegisterYourself();
	
  //  TGeoTranslation *Cr_trans = new TGeoTranslation("Cr_trans", Qu_Xpos, +7, Qu_Zpos);
    //SetPos_trans->RegisterYourself();
    		    
  	TGeoBBox *Qu_box = new TGeoBBox("Qu_box", Qu_Xsize/2, Qu_Ysize/2, Qu_Zsize/2);
  	TGeoBBox *PMT_box = new TGeoBBox("PMT_box", Box_Xsize/2, Box_Ysize/2, 20/2);
  	TGeoBBox *Plane_box = new TGeoBBox("Plane_box", 30/2, 30/2, 14/2);
  	
	TGeoTube *PMT_tube = new TGeoTube("PMT_tube", 0, 6/2, 14/2);
  	
	//TGeoCone *Air_Cone = new TGeoCone("Air_Cone", 34/2, 0, 21/2, 0, 4/2);
	TGeoCone *Air_Cone = new TGeoCone("Air_Cone", 40/2, 0, 22/2, 0, 4/2);
	
	//Composite solids (shapes)
  											
	//TGeoCompositeShape *Vac_box_assemb = new TGeoCompositeShape("Vac_box_assemb", "PMT_box:Pmt_trans - Qu_box:Quartz_trans - Plane_box:Plane_trans");
	TGeoCompositeShape *Vac_box_assemb = new TGeoCompositeShape("Vac_box_assemb", "Air_Cone:Pmt_trans - Plane_box:Plane_trans - Qu_box:Quartz_trans");
	
	TGeoVolume *Qu_ActiveVolumeV = new TGeoVolume("Qu_ActiveVolumeV", Qu_box);
    Qu_ActiveVolumeV->SetMedium(pMedFusedSil);
    Qu_ActiveVolumeV->SetLineColor(kAzure+5);
	
	TGeoVolume *Pmt_tube_mat = new TGeoVolume("Pmt_tube_mat", PMT_tube);
    Pmt_tube_mat->SetMedium(pMedAir);
    Pmt_tube_mat->SetLineColor(kBlue+4);
	
	TGeoVolume *Pmt_ActiveVolumeV  = new TGeoVolume("Pmt_ActiveVolumeV", Vac_box_assemb);
    Pmt_ActiveVolumeV->SetMedium(pMedPMTvacuum);
    Pmt_ActiveVolumeV->SetLineColor(kYellow-10);
 											
	//Adding volumes to the TOP Volume
    
	top->AddNode(FD_Top, 1, All_trans);
//	top->AddNode(Qu_Top_1, 1, Qu_trans);
	FD_Top->AddNode(Qu_ActiveVolumeV, 1);
	FD_Top->AddNode(Pmt_ActiveVolumeV, 1);
	FD_Top->AddNode(Pmt_tube_mat, 1, PMT_tube_trans);

   
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
