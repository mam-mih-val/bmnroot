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
//---------------------------

TGeoManager* gGeoMan = NULL;

const Double_t strip_BD_Xpos = 0.0;
const Double_t strip_BD_Ypos = 0.0;
const Double_t strip_BD_Zpos = -2.5; //cm 
//const Double_t strip_BD_Zpos = -3.5; //cm  

const Double_t tubeFe_Xpos = 0.0;
const Double_t tubeFe_Ypos = 0.0;
const Double_t tubeFe_Zpos = -8.45; //cm
//const Double_t tubeFe_Zpos = -9.45; //cm

const Double_t tubeAl_Xpos = 0.0;
const Double_t tubeAl_Ypos = 0.0;
const Double_t tubeAl_Zpos = -14.3; //cm

const Double_t roh_Xpos = 0.0;
const Double_t roh_Ypos = 0.0;
const Double_t roh_Zpos = -2.65; //cm

const Double_t ZLength_BDActiveVolume = 15.;  
const Double_t ZLength_tubeFe = 29.5;   
const Double_t ZLength_tubeAl = 18.;    
const Double_t ZLength_roh = 5.3; 

const Double_t dphi1 = 360./40;
const Double_t rr = 4.95;

const Double_t MaxRadiusFe = 8.1;
const Double_t MinRadiusFe = 8.;

const Double_t MaxRadiusAl = 7.0;
const Double_t MinRadiusAl = 6.8;

const Double_t MaxRadiusRohacel = 8.0;
const Double_t MinRadiusRohacel = 7.0;

void create_bd_root_v13() {
   
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
    const TString geoDetectorName = "geom_BD_det";
    const TString geoDetectorVersion = "v2";
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
  
    FairGeoMedium* mBC408 = geoMedia->getMedium("BC408");
    if ( ! mBC408 ) Fatal("Main", "FairMedium BC408 not found");
    geoBuild->createMedium(mBC408);
    TGeoMedium* pMedBC408 = gGeoMan->GetMedium("BC408");
    if ( ! pMedBC408 ) Fatal("Main", "Medium BC408 not found");
	
	FairGeoMedium* miron   = geoMedia->getMedium("iron");
    if ( ! miron ) Fatal("Main", "FairMedium iron   not found");
    geoBuild->createMedium(miron);
    TGeoMedium* pMediron   = gGeoMan->GetMedium("iron");
    if ( ! pMediron ) Fatal("Main", "Medium iron  not found");
	
	FairGeoMedium* maluminium  = geoMedia->getMedium("aluminium");
    if ( ! maluminium ) Fatal("Main", "FairMedium aluminium   not found");
    geoBuild->createMedium(maluminium);
    TGeoMedium* pMedaluminium   = gGeoMan->GetMedium("aluminium");
    if ( ! pMedaluminium ) Fatal("Main", "Medium aluminium  not found");
	
	FairGeoMedium* mpolypropylene  = geoMedia->getMedium("polypropylene");
    if ( ! mpolypropylene ) Fatal("Main", "FairMedium polypropylene   not found");
    geoBuild->createMedium(mpolypropylene);
    TGeoMedium* pMedpolypropylene   = gGeoMan->GetMedium("polypropylene");
    if ( ! pMedpolypropylene ) Fatal("Main", "Medium polypropylene  not found");
	
    // --------------------------------------------------------------------------
    
    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);

    // --------------------------------------------------------------------------

	TGeoTranslation *BD_Position = new TGeoTranslation("BD_Position", 0, 0, 0);

    // Define TOP Geometry
    TGeoVolume* BD = new TGeoVolumeAssembly("BD");
    BD->SetMedium(pMedAir);
	
	TGeoTranslation *strips_BD1Pos_trans = new TGeoTranslation("strips_BD1Pos_trans", strip_BD_Xpos, strip_BD_Ypos, strip_BD_Zpos);		
	TGeoTranslation *tubefePos_trans = new TGeoTranslation("tubefePos_trans", tubeFe_Xpos, tubeFe_Ypos, tubeFe_Zpos);
	TGeoTranslation *tubealPos_trans = new TGeoTranslation("tubefePos_trans", tubeAl_Xpos, tubeAl_Ypos, tubeAl_Zpos);
	TGeoTranslation *tuberohPos_trans = new TGeoTranslation("tuberohPos_trans", roh_Xpos, roh_Ypos, roh_Zpos);
	
	TGeoBBox *strip = new TGeoBBox("srtip", 0.7/2., 0.7/2., 15./2.);	
	TGeoTube *tubefeContainerS = new TGeoTube("tubefeContainerS", MinRadiusFe/2, MaxRadiusFe/2, ZLength_tubeFe/2); 
	TGeoTube *tubealContainerS = new TGeoTube("tubealContainerS", MinRadiusAl/2, MaxRadiusAl/2, ZLength_tubeAl/2); 
	TGeoTube *tuberohContainerS = new TGeoTube("tuberohContainerS", MinRadiusRohacel/2, MaxRadiusRohacel/2, ZLength_roh/2); 

 	Double_t  
    	phi = 90*TMath::DegToRad(),
    	delta_phi = 360./40.,
    	dphi = delta_phi*TMath::DegToRad();
	

	TGeoCombiTrans *combi1 = new TGeoCombiTrans(rr*TMath::Cos(dphi*10.5),rr*TMath::Sin(dphi*10.5),strip_BD_Zpos, new TGeoRotation("rot1",dphi1*10.5,0.,0.));
	TGeoCombiTrans *combi2 = new TGeoCombiTrans(rr*TMath::Cos(dphi*11.5),rr*TMath::Sin(dphi*11.5),strip_BD_Zpos, new TGeoRotation("rot2",dphi1*11.5,0.,0.));
	TGeoCombiTrans *combi3 = new TGeoCombiTrans(rr*TMath::Cos(dphi*12.5),rr*TMath::Sin(dphi*12.5),strip_BD_Zpos, new TGeoRotation("rot3",dphi1*12.5,0.,0.));
	TGeoCombiTrans *combi4 = new TGeoCombiTrans(rr*TMath::Cos(dphi*13.5),rr*TMath::Sin(dphi*13.5),strip_BD_Zpos, new TGeoRotation("rot4",dphi1*13.5,0.,0.));
	TGeoCombiTrans *combi5 = new TGeoCombiTrans(rr*TMath::Cos(dphi*14.5),rr*TMath::Sin(dphi*14.5),strip_BD_Zpos, new TGeoRotation("rot5",dphi1*14.5,0.,0.));
	TGeoCombiTrans *combi6 = new TGeoCombiTrans(rr*TMath::Cos(dphi*15.5),rr*TMath::Sin(dphi*15.5),strip_BD_Zpos, new TGeoRotation("rot6",dphi1*15.5,0.,0.));
	TGeoCombiTrans *combi7 = new TGeoCombiTrans(rr*TMath::Cos(dphi*16.5),rr*TMath::Sin(dphi*16.5),strip_BD_Zpos, new TGeoRotation("rot7",dphi1*16.5,0.,0.));
	TGeoCombiTrans *combi8 = new TGeoCombiTrans(rr*TMath::Cos(dphi*17.5),rr*TMath::Sin(dphi*17.5),strip_BD_Zpos, new TGeoRotation("rot8",dphi1*17.5,0.,0.));
	TGeoCombiTrans *combi9 = new TGeoCombiTrans(rr*TMath::Cos(dphi*18.5),rr*TMath::Sin(dphi*18.5),strip_BD_Zpos, new TGeoRotation("rot9",dphi1*18.5,0.,0.));
	TGeoCombiTrans *combi10 = new TGeoCombiTrans(rr*TMath::Cos(dphi*19.5),rr*TMath::Sin(dphi*19.5),strip_BD_Zpos, new TGeoRotation("rot10",dphi1*19.5,0.,0.));
	TGeoCombiTrans *combi11 = new TGeoCombiTrans(rr*TMath::Cos(dphi*20.5),rr*TMath::Sin(dphi*20.5),strip_BD_Zpos, new TGeoRotation("rot11",dphi1*20.5,0.,0.));
	TGeoCombiTrans *combi12 = new TGeoCombiTrans(rr*TMath::Cos(dphi*21.5),rr*TMath::Sin(dphi*21.5),strip_BD_Zpos, new TGeoRotation("rot12",dphi1*21.5,0.,0.));
	TGeoCombiTrans *combi13 = new TGeoCombiTrans(rr*TMath::Cos(dphi*22.5),rr*TMath::Sin(dphi*22.5),strip_BD_Zpos, new TGeoRotation("rot13",dphi1*22.5,0.,0.));
	TGeoCombiTrans *combi14 = new TGeoCombiTrans(rr*TMath::Cos(dphi*23.5),rr*TMath::Sin(dphi*23.5),strip_BD_Zpos, new TGeoRotation("rot14",dphi1*23.5,0.,0.));
	TGeoCombiTrans *combi15 = new TGeoCombiTrans(rr*TMath::Cos(dphi*24.5),rr*TMath::Sin(dphi*24.5),strip_BD_Zpos, new TGeoRotation("rot15",dphi1*24.5,0.,0.));
	TGeoCombiTrans *combi16 = new TGeoCombiTrans(rr*TMath::Cos(dphi*25.5),rr*TMath::Sin(dphi*25.5),strip_BD_Zpos, new TGeoRotation("rot16",dphi1*25.5,0.,0.));
	TGeoCombiTrans *combi17 = new TGeoCombiTrans(rr*TMath::Cos(dphi*26.5),rr*TMath::Sin(dphi*26.5),strip_BD_Zpos, new TGeoRotation("rot17",dphi1*26.5,0.,0.));
	TGeoCombiTrans *combi18 = new TGeoCombiTrans(rr*TMath::Cos(dphi*27.5),rr*TMath::Sin(dphi*27.5),strip_BD_Zpos, new TGeoRotation("rot18",dphi1*27.5,0.,0.));
	TGeoCombiTrans *combi19 = new TGeoCombiTrans(rr*TMath::Cos(dphi*28.5),rr*TMath::Sin(dphi*28.5),strip_BD_Zpos, new TGeoRotation("rot19",dphi1*28.5,0.,0.));
	TGeoCombiTrans *combi20 = new TGeoCombiTrans(rr*TMath::Cos(dphi*29.5),rr*TMath::Sin(dphi*29.5),strip_BD_Zpos, new TGeoRotation("rot20",dphi1*29.5,0.,0.));
	TGeoCombiTrans *combi21 = new TGeoCombiTrans(rr*TMath::Cos(dphi*30.5),rr*TMath::Sin(dphi*30.5),strip_BD_Zpos, new TGeoRotation("rot21",dphi1*30.5,0.,0.));
	TGeoCombiTrans *combi22 = new TGeoCombiTrans(rr*TMath::Cos(dphi*31.5),rr*TMath::Sin(dphi*31.5),strip_BD_Zpos, new TGeoRotation("rot22",dphi1*31.5,0.,0.));
	TGeoCombiTrans *combi23 = new TGeoCombiTrans(rr*TMath::Cos(dphi*32.5),rr*TMath::Sin(dphi*32.5),strip_BD_Zpos, new TGeoRotation("rot23",dphi1*32.5,0.,0.));
	TGeoCombiTrans *combi24 = new TGeoCombiTrans(rr*TMath::Cos(dphi*33.5),rr*TMath::Sin(dphi*33.5),strip_BD_Zpos, new TGeoRotation("rot24",dphi1*33.5,0.,0.));
	TGeoCombiTrans *combi25 = new TGeoCombiTrans(rr*TMath::Cos(dphi*34.5),rr*TMath::Sin(dphi*34.5),strip_BD_Zpos, new TGeoRotation("rot25",dphi1*34.5,0.,0.));
	TGeoCombiTrans *combi26 = new TGeoCombiTrans(rr*TMath::Cos(dphi*35.5),rr*TMath::Sin(dphi*35.5),strip_BD_Zpos, new TGeoRotation("rot26",dphi1*35.5,0.,0.));
	TGeoCombiTrans *combi27 = new TGeoCombiTrans(rr*TMath::Cos(dphi*36.5),rr*TMath::Sin(dphi*36.5),strip_BD_Zpos, new TGeoRotation("rot27",dphi1*36.5,0.,0.));
	TGeoCombiTrans *combi28 = new TGeoCombiTrans(rr*TMath::Cos(dphi*37.5),rr*TMath::Sin(dphi*37.5),strip_BD_Zpos, new TGeoRotation("rot28",dphi1*37.5,0.,0.));
	TGeoCombiTrans *combi29 = new TGeoCombiTrans(rr*TMath::Cos(dphi*38.5),rr*TMath::Sin(dphi*38.5),strip_BD_Zpos, new TGeoRotation("rot29",dphi1*38.5,0.,0.));
	TGeoCombiTrans *combi30 = new TGeoCombiTrans(rr*TMath::Cos(dphi*39.5),rr*TMath::Sin(dphi*39.5),strip_BD_Zpos, new TGeoRotation("rot30",dphi1*39.5,0.,0.));
	TGeoCombiTrans *combi31 = new TGeoCombiTrans(rr*TMath::Cos(dphi*40.5),rr*TMath::Sin(dphi*40.5),strip_BD_Zpos, new TGeoRotation("rot31",dphi1*40.5,0.,0.));
	TGeoCombiTrans *combi32 = new TGeoCombiTrans(rr*TMath::Cos(dphi*1.5),rr*TMath::Sin(dphi*1.5),strip_BD_Zpos, new TGeoRotation("rot32",dphi1*1.5,0.,0.));
	TGeoCombiTrans *combi33 = new TGeoCombiTrans(rr*TMath::Cos(dphi*2.5),rr*TMath::Sin(dphi*2.5),strip_BD_Zpos, new TGeoRotation("rot33",dphi1*2.5,0.,0.));
	TGeoCombiTrans *combi34 = new TGeoCombiTrans(rr*TMath::Cos(dphi*3.5),rr*TMath::Sin(dphi*3.5),strip_BD_Zpos, new TGeoRotation("rot34",dphi1*3.5,0.,0.));
	TGeoCombiTrans *combi35 = new TGeoCombiTrans(rr*TMath::Cos(dphi*4.5),rr*TMath::Sin(dphi*4.5),strip_BD_Zpos, new TGeoRotation("rot35",dphi1*4.5,0.,0.));
	TGeoCombiTrans *combi36 = new TGeoCombiTrans(rr*TMath::Cos(dphi*5.5),rr*TMath::Sin(dphi*5.5),strip_BD_Zpos, new TGeoRotation("rot36",dphi1*5.5,0.,0.));
	TGeoCombiTrans *combi37 = new TGeoCombiTrans(rr*TMath::Cos(dphi*6.5),rr*TMath::Sin(dphi*6.5),strip_BD_Zpos, new TGeoRotation("rot37",dphi1*6.5,0.,0.));
	TGeoCombiTrans *combi38 = new TGeoCombiTrans(rr*TMath::Cos(dphi*7.5),rr*TMath::Sin(dphi*7.5),strip_BD_Zpos, new TGeoRotation("rot38",dphi1*7.5,0.,0.));
	TGeoCombiTrans *combi39 = new TGeoCombiTrans(rr*TMath::Cos(dphi*8.5),rr*TMath::Sin(dphi*8.5),strip_BD_Zpos, new TGeoRotation("rot39",dphi1*8.5,0.,0.));
	TGeoCombiTrans *combi40 = new TGeoCombiTrans(rr*TMath::Cos(dphi*9.5),rr*TMath::Sin(dphi*9.5),strip_BD_Zpos, new TGeoRotation("rot40",dphi1*9.5,0.,0.));
	
	TGeoVolume *BD1ActiveVolumeV1 = new TGeoVolume("BD1ActiveVolumeV1", strip);
    BD1ActiveVolumeV1->SetMedium(pMedBC408);
    BD1ActiveVolumeV1->SetLineColor(kYellow-10);
	
	TGeoVolume *tubefeActiveVolumeV = new TGeoVolume("tubefeActiveVolumeV", tubefeContainerS);
    tubefeActiveVolumeV->SetMedium(pMediron);
    tubefeActiveVolumeV->SetLineColor(kOrange-1);
	
	TGeoVolume *tubealActiveVolumeV = new TGeoVolume("tubealActiveVolumeV", tubealContainerS);
    tubealActiveVolumeV->SetMedium(pMedaluminium);
    tubealActiveVolumeV->SetLineColor(kCyan-10);
	
	TGeoVolume *tuberohActiveVolumeV = new TGeoVolume("tuberohActiveVolumeV", tuberohContainerS);
    tuberohActiveVolumeV->SetMedium(pMedpolypropylene);
    tuberohActiveVolumeV->SetLineColor(kGray);
  
	//container1->AddNode(BD1Top, 1);
	
	//BD1Top->AddNode(BD1ActiveVolumeV1, 1, new TGeoCombiTrans(t1, r1));
	
			
	BD->AddNode(BD1ActiveVolumeV1, 1, combi1);			
	BD->AddNode(BD1ActiveVolumeV1, 2, combi2);			
	BD->AddNode(BD1ActiveVolumeV1, 3, combi3);			
	BD->AddNode(BD1ActiveVolumeV1, 4, combi4);			
	BD->AddNode(BD1ActiveVolumeV1, 5, combi5);			
	BD->AddNode(BD1ActiveVolumeV1, 6, combi6);			
	BD->AddNode(BD1ActiveVolumeV1, 7, combi7);			
	BD->AddNode(BD1ActiveVolumeV1, 8, combi8);			
	BD->AddNode(BD1ActiveVolumeV1, 9, combi9);			
	BD->AddNode(BD1ActiveVolumeV1, 10, combi10);			
	BD->AddNode(BD1ActiveVolumeV1, 11, combi11);			
	BD->AddNode(BD1ActiveVolumeV1, 12, combi12);			
	BD->AddNode(BD1ActiveVolumeV1, 13, combi13);			
	BD->AddNode(BD1ActiveVolumeV1, 14, combi14);			
	BD->AddNode(BD1ActiveVolumeV1, 15, combi15);			
	BD->AddNode(BD1ActiveVolumeV1, 16, combi16);			
	BD->AddNode(BD1ActiveVolumeV1, 17, combi17);			
	BD->AddNode(BD1ActiveVolumeV1, 18, combi18);			
	BD->AddNode(BD1ActiveVolumeV1, 19, combi19);			
	BD->AddNode(BD1ActiveVolumeV1, 20, combi20);			
	BD->AddNode(BD1ActiveVolumeV1, 21, combi21);			
	BD->AddNode(BD1ActiveVolumeV1, 22, combi22);			
	BD->AddNode(BD1ActiveVolumeV1, 23, combi23);			
	BD->AddNode(BD1ActiveVolumeV1, 24, combi24);			
	BD->AddNode(BD1ActiveVolumeV1, 25, combi25);			
	BD->AddNode(BD1ActiveVolumeV1, 26, combi26);			
	BD->AddNode(BD1ActiveVolumeV1, 27, combi27);			
	BD->AddNode(BD1ActiveVolumeV1, 28, combi28);			
	BD->AddNode(BD1ActiveVolumeV1, 29, combi29);			
	BD->AddNode(BD1ActiveVolumeV1, 30, combi30);			
	BD->AddNode(BD1ActiveVolumeV1, 31, combi31);			
	BD->AddNode(BD1ActiveVolumeV1, 32, combi32);			
	BD->AddNode(BD1ActiveVolumeV1, 33, combi33);			
	BD->AddNode(BD1ActiveVolumeV1, 34, combi34);			
	BD->AddNode(BD1ActiveVolumeV1, 35, combi35);			
	BD->AddNode(BD1ActiveVolumeV1, 36, combi36);			
	BD->AddNode(BD1ActiveVolumeV1, 37, combi37);			
	BD->AddNode(BD1ActiveVolumeV1, 38, combi38);			
	BD->AddNode(BD1ActiveVolumeV1, 39, combi39);			
	BD->AddNode(BD1ActiveVolumeV1, 40, combi40);					
	
	BD->AddNode(tubefeActiveVolumeV, 41, tubefePos_trans);			
	BD->AddNode(tubealActiveVolumeV, 42, tubealPos_trans);			
	BD->AddNode(tuberohActiveVolumeV, 43, tuberohPos_trans);	
	
	  //Adding volumes to the TOP Volume
    top->AddNode(BD, 1, BD_Position);
    
    top->SetVisContainers(kTRUE);
    
    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
    //top->Draw("");
}
