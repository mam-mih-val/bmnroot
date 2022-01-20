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


//Detector's position
//const Double_t tanpipeangle = 100./10. ; // tangent of pipe angle
const Double_t Sid_New_Xpos = 0.0;
const Double_t Sid_New_Ypos = 0.0;
//const Double_t SID_Zpos = 16.8; //cm
const Double_t Sid_New_Zpos = 12.; //cm

const Double_t ZLength_SIDActiveVolume = 0.05;      

const Double_t dphia = 5.6;

const Double_t Frame_Xsize = 24.3; //cm
const Double_t Frame_Ysize = 24.3; //cm
const Double_t Frame_Zsize = 0.1; //cm

void create_rootgeom_SiMD() {
     
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
    const TString geoDetectorName = "SiMD";
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
  
    FairGeoMedium* msilicon = geoMedia->getMedium("silicon");
    if ( ! msilicon ) Fatal("Main", "FairMedium silicon not found");
    geoBuild->createMedium(msilicon);
    TGeoMedium* pMedsilicon = gGeoMan->GetMedium("silicon");
    if ( ! pMedsilicon ) Fatal("Main", "Medium silicon not found");
	
	FairGeoMedium* maluminium = geoMedia->getMedium("aluminium");
    if ( ! maluminium ) Fatal("Main", "FairMedium aluminium   not found");
    geoBuild->createMedium(maluminium);
    TGeoMedium* pMedaluminium  = gGeoMan->GetMedium("aluminium");
    if ( ! pMedaluminium ) Fatal("Main", "Medium aluminium  not found");
    
    // --------------------------------------------------------------------------
    
    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* SidNewTop = new TGeoVolumeAssembly(geoDetectorName);
    SidNewTop->SetMedium(pMedAir);
   
    //Transformations (translations, rotations and scales)
    TGeoTranslation *Sid_new_Pos_trans1 = new TGeoTranslation("Sid_new_Pos_trans1", Sid_New_Xpos, Sid_New_Ypos, Sid_New_Zpos);
 //   TGeoTranslation *Sid_new_Pos_trans2 = new TGeoTranslation("Sid_new_Pos_trans2", -9.24, Sid_New_Ypos, Sid_New_Zpos);
    //SetPos_trans->RegisterYourself();
	
	TGeoTranslation *Frame_pos = new TGeoTranslation("Frame_pos", 0, 0, 0);
	
	TGeoCombiTrans *B_tube_pos = new TGeoCombiTrans("B_tube_pos", 0, 0, 0.1, new TGeoRotation("rot1",0,0,0));
	B_tube_pos->RegisterYourself();
	
	TGeoCombiTrans *A_box_pos = new TGeoCombiTrans("A_box_pos", 0, 0, 0.1, new TGeoRotation("rot2",0,0,0));
	A_box_pos->RegisterYourself();
	
	TGeoCombiTrans *left_part_pos = new TGeoCombiTrans("left_part_pos", 15.6, 0, 0.1, new TGeoRotation("rot3",0,0,0));
	left_part_pos->RegisterYourself();
	
	TGeoCombiTrans *right_part_pos = new TGeoCombiTrans("right_part_pos", -15.6, 0, 0.1, new TGeoRotation("rot4",0,0,0));
	right_part_pos->RegisterYourself();
	
	TGeoCombiTrans *skin_tube_pos = new TGeoCombiTrans("skin_tube_pos", 0, 0, -0.3, new TGeoRotation("rot5",0,0,0));
	skin_tube_pos->RegisterYourself();
	
  	TGeoBBox *A_box = new TGeoBBox("A_box", Frame_Xsize/2, Frame_Ysize/2, Frame_Zsize/2);
  	TGeoBBox *Sm_box = new TGeoBBox("Sm_box", 7./2., 18.4/2., Frame_Zsize/2);
  	TGeoTube *B_tube = new TGeoTube("B_tube", 0., 20./2., 4.);
  	TGeoTube *Skin_tube = new TGeoTube("Skin_tube", 5.1/2, 20./2., 0.005);
	
	TGeoCompositeShape *Frame_box = new TGeoCompositeShape("Frame_box", "A_box:A_box_pos - B_tube:B_tube_pos + Sm_box:right_part_pos + Sm_box:left_part_pos");
		
	const Double_t rr = 5.8;
	
	Double_t  
    phi = 90*TMath::DegToRad(),
    delta_phi = 360./64.,
    dphi = delta_phi*TMath::DegToRad();
	
	
	TGeoTrap *sid_trapv4 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv3 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv2 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv1 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv5 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv6 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv7 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv8 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv12 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv11 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv10 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv9 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv13 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv14 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv15 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv16 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv20 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv19 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv18 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv17 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv21 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv22 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv23 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv24 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv28 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv27 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv26 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv25 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv29 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv30 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv31 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv32 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv36 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv35 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv34 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv33 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv37 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv38 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv39 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv40 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv44 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv43 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv42 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv41 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv45 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv46 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv47 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv48 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv52 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv51 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv50 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv49 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv53 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv54 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv55 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv56 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	TGeoTrap *sid_trapv60 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,3,3.1045,0.461,0.155,3);
	TGeoTrap *sid_trapv59 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,8.5,3.1155,0.47,0.158,8.5);
	TGeoTrap *sid_trapv58 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,14,3.128,0.4885,0.1645,14);
	TGeoTrap *sid_trapv57 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,19.5,3.138,0.4355,0.093,19.5);
	
	TGeoTrap *sid_trapv61 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1045,0.461,0.155,-3,3.1045,0.461,0.155,-3);
	TGeoTrap *sid_trapv62 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.1155,0.47,0.158,-8.5,3.1155,0.47,0.158,-8.5);
	TGeoTrap *sid_trapv63 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.128,0.4885,0.1645,-14,3.128,0.4885,0.1645,-14);
	TGeoTrap *sid_trapv64 = new TGeoTrap(ZLength_SIDActiveVolume/2,0,0,3.138,0.4355,0.093,-19.5,3.138,0.4355,0.093,-19.5);
	
	
	
	
 //*****************************************************************************************************
	
	TGeoCombiTrans *position29 = new TGeoCombiTrans(1.95,-5.87,0,new TGeoRotation("rot29",0,0.,22.5));
	TGeoCombiTrans *position30 = new TGeoCombiTrans(1.39,-6.12,0,new TGeoRotation("rot30",0,0.,22.5));
	TGeoCombiTrans *position31 = new TGeoCombiTrans(0.8,-6.37,0,new TGeoRotation("rot31",0,0.,22.5));
	TGeoCombiTrans *position32 = new TGeoCombiTrans(0.25,-6.6,0,new TGeoRotation("rot32",0,0.,22.5));
	
	TGeoCombiTrans *position28 = new TGeoCombiTrans(2.53,-5.64,0,new TGeoRotation("rot28",0,0.,22.5));
	TGeoCombiTrans *position27 = new TGeoCombiTrans(3.1,-5.4,0,new TGeoRotation("rot27",0,0.,22.5));
	TGeoCombiTrans *position26 = new TGeoCombiTrans(3.69,-5.16,0,new TGeoRotation("rot26",0,0.,22.5));
	TGeoCombiTrans *position25 = new TGeoCombiTrans(4.25,-4.94,0,new TGeoRotation("rot25",0,0.,22.5));
	

	TGeoCombiTrans *position21 = new TGeoCombiTrans(5.54,-2.73,0,new TGeoRotation("rot21",0,0.,67.5));
	TGeoCombiTrans *position22 = new TGeoCombiTrans(5.3,-3.3,0,new TGeoRotation("rot22",0,0.,67.5));
	TGeoCombiTrans *position23 = new TGeoCombiTrans(5.06,-3.89,0,new TGeoRotation("rot23",0,0.,67.5));
	TGeoCombiTrans *position24 = new TGeoCombiTrans(4.84,-4.45,0,new TGeoRotation("rot24",0,0.,67.5));
	
	TGeoCombiTrans *position20 = new TGeoCombiTrans(5.77,-2.15,0,new TGeoRotation("rot20",0,0.,67.5));
	TGeoCombiTrans *position19 = new TGeoCombiTrans(6.02,-1.59,0,new TGeoRotation("rot19",0,0.,67.5));
	TGeoCombiTrans *position18 = new TGeoCombiTrans(6.27,-1.,0,new TGeoRotation("rot18",0,0.,67.5));
	TGeoCombiTrans *position17 = new TGeoCombiTrans(6.5,-0.45,0,new TGeoRotation("rot17",0,0.,67.5));
	
	//**********************************************************************************************	
	
	TGeoCombiTrans *position61 = new TGeoCombiTrans(-1.95,5.87,0,new TGeoRotation("rot61",0,0.,202.5));
	TGeoCombiTrans *position62 = new TGeoCombiTrans(-1.39,6.12,0,new TGeoRotation("rot62",0,0.,202.5));
	TGeoCombiTrans *position63 = new TGeoCombiTrans(-0.8,6.37,0,new TGeoRotation("rot63",0,0.,202.5));
	TGeoCombiTrans *position64 = new TGeoCombiTrans(-0.25,6.6,0,new TGeoRotation("rot64",0,0.,202.5));
	
	TGeoCombiTrans *position60 = new TGeoCombiTrans(-2.53,5.64,0,new TGeoRotation("rot60",0,0.,202.5));
	TGeoCombiTrans *position59 = new TGeoCombiTrans(-3.1,5.4,0,new TGeoRotation("rot59",0,0.,202.5));
	TGeoCombiTrans *position58 = new TGeoCombiTrans(-3.69,5.16,0,new TGeoRotation("rot58",0,0.,202.5));
	TGeoCombiTrans *position57 = new TGeoCombiTrans(-4.25,4.94,0,new TGeoRotation("rot57",0,0.,202.5));

	//**********************************************************************************************	

	TGeoCombiTrans *position53 = new TGeoCombiTrans(-5.54,2.73,0,new TGeoRotation("rot53",0,0.,247.5));
	TGeoCombiTrans *position54 = new TGeoCombiTrans(-5.3,3.3,0,new TGeoRotation("rot54",0,0.,247.5));
	TGeoCombiTrans *position55 = new TGeoCombiTrans(-5.06,3.89,0,new TGeoRotation("rot55",0,0.,247.5));
	TGeoCombiTrans *position56 = new TGeoCombiTrans(-4.84,4.45,0,new TGeoRotation("rot56",0,0.,247.5));
	
	TGeoCombiTrans *position52 = new TGeoCombiTrans(-5.77,2.15,0,new TGeoRotation("rot52",0,0.,247.5));
	TGeoCombiTrans *position51 = new TGeoCombiTrans(-6.02,1.59,0,new TGeoRotation("rot51",0,0.,247.5));
	TGeoCombiTrans *position50 = new TGeoCombiTrans(-6.27,1.,0,new TGeoRotation("rot50",0,0.,247.5));
	TGeoCombiTrans *position49 = new TGeoCombiTrans(-6.5,0.45,0,new TGeoRotation("rot49",0,0.,247.5));
	
	//**********************************************************************************************	

	TGeoCombiTrans *position5 = new TGeoCombiTrans(2.73,5.54,0,new TGeoRotation("rot5",0,0.,157.5));
	TGeoCombiTrans *position6 = new TGeoCombiTrans(3.3,5.3,0,new TGeoRotation("rot6",0,0.,157.5));
	TGeoCombiTrans *position7 = new TGeoCombiTrans(3.89,5.06,0,new TGeoRotation("rot7",0,0.,157.5));
	TGeoCombiTrans *position8 = new TGeoCombiTrans(4.45,4.84,0,new TGeoRotation("rot8",0,0.,157.5));
	
	TGeoCombiTrans *position4 = new TGeoCombiTrans(2.15,5.77,0,new TGeoRotation("rot4",0,0.,157.5));
	TGeoCombiTrans *position3 = new TGeoCombiTrans(1.59,6.02,0,new TGeoRotation("rot3",0,0.,157.5));
	TGeoCombiTrans *position2 = new TGeoCombiTrans(1.,6.27,0,new TGeoRotation("rot2",0,0.,157.5));
	TGeoCombiTrans *position1 = new TGeoCombiTrans(0.45,6.5,0,new TGeoRotation("rot1",0,0.,157.5));		

	//**********************************************************************************************	

	TGeoCombiTrans *position37 = new TGeoCombiTrans(-2.73,-5.54,0,new TGeoRotation("rot37",0,0.,337.5));
	TGeoCombiTrans *position38 = new TGeoCombiTrans(-3.3,-5.3,0,new TGeoRotation("rot38",0,0.,337.5));
	TGeoCombiTrans *position39 = new TGeoCombiTrans(-3.89,-5.06,0,new TGeoRotation("ro39",0,0.,337.5));
	TGeoCombiTrans *position40 = new TGeoCombiTrans(-4.45,-4.84,0,new TGeoRotation("rot40",0,0.,337.5));
	
	TGeoCombiTrans *position36 = new TGeoCombiTrans(-2.15,-5.77,0,new TGeoRotation("rot36",0,0.,337.5));
	TGeoCombiTrans *position35 = new TGeoCombiTrans(-1.59,-6.02,0,new TGeoRotation("rot35",0,0.,337.5));
	TGeoCombiTrans *position34 = new TGeoCombiTrans(-1.,-6.27,0,new TGeoRotation("rot34",0,0.,337.5));
	TGeoCombiTrans *position33 = new TGeoCombiTrans(-0.45,-6.5,0,new TGeoRotation("rot33",0,0.,337.5));	
	
	//**********************************************************************************************	

	TGeoCombiTrans *position13 = new TGeoCombiTrans(5.87,1.95,0,new TGeoRotation("rot13",0,0.,112.5));
	TGeoCombiTrans *position14 = new TGeoCombiTrans(6.12,1.39,0,new TGeoRotation("rot14",0,0.,112.5));
	TGeoCombiTrans *position15 = new TGeoCombiTrans(6.37,0.8,0,new TGeoRotation("rot15",0,0.,112.5));
	TGeoCombiTrans *position16 = new TGeoCombiTrans(6.6,0.25,0,new TGeoRotation("rot16",0,0.,112.5));
	
	TGeoCombiTrans *position12 = new TGeoCombiTrans(5.64,2.53,0,new TGeoRotation("rot12",0,0.,112.5));
	TGeoCombiTrans *position11 = new TGeoCombiTrans(5.4,3.1,0,new TGeoRotation("rot11",0,0.,112.5));
	TGeoCombiTrans *position10 = new TGeoCombiTrans(5.16,3.69,0,new TGeoRotation("rot10",0,0.,112.5));
	TGeoCombiTrans *position9 = new TGeoCombiTrans(4.94,4.25,0,new TGeoRotation("rot9",0,0.,112.5));
	
	//**********************************************************************************************	

	TGeoCombiTrans *position45 = new TGeoCombiTrans(-5.87,-1.95,0,new TGeoRotation("rot45",0,0.,292.5));
	TGeoCombiTrans *position46 = new TGeoCombiTrans(-6.12,-1.39,0,new TGeoRotation("rot46",0,0.,292.5));
	TGeoCombiTrans *position47 = new TGeoCombiTrans(-6.37,-0.8,0,new TGeoRotation("rot47",0,0.,292.5));
	TGeoCombiTrans *position48 = new TGeoCombiTrans(-6.6,-0.25,0,new TGeoRotation("rot48",0,0.,292.5));
	
	TGeoCombiTrans *position44 = new TGeoCombiTrans(-5.64,-2.53,0,new TGeoRotation("rot44",0,0.,292.5));
	TGeoCombiTrans *position43 = new TGeoCombiTrans(-5.4,-3.1,0,new TGeoRotation("rot43",0,0.,292.5));
	TGeoCombiTrans *position42 = new TGeoCombiTrans(-5.16,-3.69,0,new TGeoRotation("rot42",0,0.,292.5));
	TGeoCombiTrans *position41 = new TGeoCombiTrans(-4.94,-4.25,0,new TGeoRotation("rot41",0,0.,292.5));
	

	
	
    TGeoVolume *SiD_ActiveVolV1 = new TGeoVolume("SiD_ActiveVolV1", sid_trapv1);
    SiD_ActiveVolV1->SetMedium(pMedsilicon);
    SiD_ActiveVolV1->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV2 = new TGeoVolume("SiD_ActiveVolV2", sid_trapv2);
    SiD_ActiveVolV2->SetMedium(pMedsilicon);
    SiD_ActiveVolV2->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV3 = new TGeoVolume("SiD_ActiveVolV3", sid_trapv3);
    SiD_ActiveVolV3->SetMedium(pMedsilicon);
    SiD_ActiveVolV3->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV4 = new TGeoVolume("SiD_ActiveVolV4", sid_trapv4);
    SiD_ActiveVolV4->SetMedium(pMedsilicon);
    SiD_ActiveVolV4->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV5 = new TGeoVolume("SiD_ActiveVolV5", sid_trapv5);
    SiD_ActiveVolV5->SetMedium(pMedsilicon);
    SiD_ActiveVolV5->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV6 = new TGeoVolume("SiD_ActiveVolV6", sid_trapv6);
    SiD_ActiveVolV6->SetMedium(pMedsilicon);
    SiD_ActiveVolV6->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV7 = new TGeoVolume("SiD_ActiveVolV7", sid_trapv7);
    SiD_ActiveVolV7->SetMedium(pMedsilicon);
    SiD_ActiveVolV7->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV8 = new TGeoVolume("SiD_ActiveVolV8", sid_trapv8);
    SiD_ActiveVolV8->SetMedium(pMedsilicon);
    SiD_ActiveVolV8->SetLineColor(kRed);
	
	
	TGeoVolume *SiD_ActiveVolV9 = new TGeoVolume("SiD_ActiveVolV9", sid_trapv1);
    SiD_ActiveVolV9->SetMedium(pMedsilicon);
    SiD_ActiveVolV9->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV10 = new TGeoVolume("SiD_ActiveVolV10", sid_trapv2);
    SiD_ActiveVolV10->SetMedium(pMedsilicon);
    SiD_ActiveVolV10->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV11 = new TGeoVolume("SiD_ActiveVolV11", sid_trapv3);
    SiD_ActiveVolV11->SetMedium(pMedsilicon);
    SiD_ActiveVolV11->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV12 = new TGeoVolume("SiD_ActiveVolV12", sid_trapv4);
    SiD_ActiveVolV12->SetMedium(pMedsilicon);
    SiD_ActiveVolV12->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV13 = new TGeoVolume("SiD_ActiveVolV13", sid_trapv5);
    SiD_ActiveVolV13->SetMedium(pMedsilicon);
    SiD_ActiveVolV13->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV14 = new TGeoVolume("SiD_ActiveVolV14", sid_trapv6);
    SiD_ActiveVolV14->SetMedium(pMedsilicon);
    SiD_ActiveVolV14->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV15 = new TGeoVolume("SiD_ActiveVolV15", sid_trapv7);
    SiD_ActiveVolV15->SetMedium(pMedsilicon);
    SiD_ActiveVolV15->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV16 = new TGeoVolume("SiD_ActiveVolV16", sid_trapv8);
    SiD_ActiveVolV16->SetMedium(pMedsilicon);
    SiD_ActiveVolV16->SetLineColor(kRed);
	
	
	
	TGeoVolume *SiD_ActiveVolV17 = new TGeoVolume("SiD_ActiveVolV17", sid_trapv1);
    SiD_ActiveVolV17->SetMedium(pMedsilicon);
    SiD_ActiveVolV17->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV18 = new TGeoVolume("SiD_ActiveVolV18", sid_trapv2);
    SiD_ActiveVolV18->SetMedium(pMedsilicon);
    SiD_ActiveVolV18->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV19 = new TGeoVolume("SiD_ActiveVolV19", sid_trapv3);
    SiD_ActiveVolV19->SetMedium(pMedsilicon);
    SiD_ActiveVolV19->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV20 = new TGeoVolume("SiD_ActiveVolV20", sid_trapv4);
    SiD_ActiveVolV20->SetMedium(pMedsilicon);
    SiD_ActiveVolV20->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV21 = new TGeoVolume("SiD_ActiveVolV21", sid_trapv5);
    SiD_ActiveVolV21->SetMedium(pMedsilicon);
    SiD_ActiveVolV21->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV22 = new TGeoVolume("SiD_ActiveVolV22", sid_trapv6);
    SiD_ActiveVolV22->SetMedium(pMedsilicon);
    SiD_ActiveVolV22->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV23 = new TGeoVolume("SiD_ActiveVolV23", sid_trapv7);
    SiD_ActiveVolV23->SetMedium(pMedsilicon);
    SiD_ActiveVolV23->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV24 = new TGeoVolume("SiD_ActiveVolV24", sid_trapv8);
    SiD_ActiveVolV24->SetMedium(pMedsilicon);
    SiD_ActiveVolV24->SetLineColor(kRed);
	
	
	TGeoVolume *SiD_ActiveVolV25 = new TGeoVolume("SiD_ActiveVolV25", sid_trapv1);
    SiD_ActiveVolV25->SetMedium(pMedsilicon);
    SiD_ActiveVolV25->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV26 = new TGeoVolume("SiD_ActiveVolV26", sid_trapv2);
    SiD_ActiveVolV26->SetMedium(pMedsilicon);
    SiD_ActiveVolV26->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV27 = new TGeoVolume("SiD_ActiveVolV27", sid_trapv3);
    SiD_ActiveVolV27->SetMedium(pMedsilicon);
    SiD_ActiveVolV27->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV28 = new TGeoVolume("SiD_ActiveVolV28", sid_trapv4);
    SiD_ActiveVolV28->SetMedium(pMedsilicon);
    SiD_ActiveVolV28->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV29 = new TGeoVolume("SiD_ActiveVolV29", sid_trapv5);
    SiD_ActiveVolV29->SetMedium(pMedsilicon);
    SiD_ActiveVolV29->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV30 = new TGeoVolume("SiD_ActiveVolV30", sid_trapv6);
    SiD_ActiveVolV30->SetMedium(pMedsilicon);
    SiD_ActiveVolV30->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV31 = new TGeoVolume("SiD_ActiveVolV31", sid_trapv7);
    SiD_ActiveVolV31->SetMedium(pMedsilicon);
    SiD_ActiveVolV31->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV32 = new TGeoVolume("SiD_ActiveVolV32", sid_trapv8);
    SiD_ActiveVolV32->SetMedium(pMedsilicon);
    SiD_ActiveVolV32->SetLineColor(kRed);
	
	
	
	TGeoVolume *SiD_ActiveVolV33 = new TGeoVolume("SiD_ActiveVolV33", sid_trapv1);
    SiD_ActiveVolV33->SetMedium(pMedsilicon);
    SiD_ActiveVolV33->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV34 = new TGeoVolume("SiD_ActiveVolV34", sid_trapv2);
    SiD_ActiveVolV34->SetMedium(pMedsilicon);
    SiD_ActiveVolV34->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV35 = new TGeoVolume("SiD_ActiveVolV35", sid_trapv3);
    SiD_ActiveVolV35->SetMedium(pMedsilicon);
    SiD_ActiveVolV35->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV36 = new TGeoVolume("SiD_ActiveVolV36", sid_trapv4);
    SiD_ActiveVolV36->SetMedium(pMedsilicon);
    SiD_ActiveVolV36->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV37 = new TGeoVolume("SiD_ActiveVolV37", sid_trapv5);
    SiD_ActiveVolV37->SetMedium(pMedsilicon);
    SiD_ActiveVolV37->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV38 = new TGeoVolume("SiD_ActiveVolV38", sid_trapv6);
    SiD_ActiveVolV38->SetMedium(pMedsilicon);
    SiD_ActiveVolV38->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV39 = new TGeoVolume("SiD_ActiveVolV39", sid_trapv7);
    SiD_ActiveVolV39->SetMedium(pMedsilicon);
    SiD_ActiveVolV39->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV40 = new TGeoVolume("SiD_ActiveVolV40", sid_trapv8);
    SiD_ActiveVolV40->SetMedium(pMedsilicon);
    SiD_ActiveVolV40->SetLineColor(kRed);
	
	
	TGeoVolume *SiD_ActiveVolV41 = new TGeoVolume("SiD_ActiveVolV41", sid_trapv1);
    SiD_ActiveVolV41->SetMedium(pMedsilicon);
    SiD_ActiveVolV41->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV42 = new TGeoVolume("SiD_ActiveVolV42", sid_trapv2);
    SiD_ActiveVolV42->SetMedium(pMedsilicon);
    SiD_ActiveVolV42->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV43 = new TGeoVolume("SiD_ActiveVolV43", sid_trapv3);
    SiD_ActiveVolV43->SetMedium(pMedsilicon);
    SiD_ActiveVolV43->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV44 = new TGeoVolume("SiD_ActiveVolV44", sid_trapv4);
    SiD_ActiveVolV44->SetMedium(pMedsilicon);
    SiD_ActiveVolV44->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV45 = new TGeoVolume("SiD_ActiveVolV45", sid_trapv5);
    SiD_ActiveVolV45->SetMedium(pMedsilicon);
    SiD_ActiveVolV45->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV46 = new TGeoVolume("SiD_ActiveVolV46", sid_trapv6);
    SiD_ActiveVolV46->SetMedium(pMedsilicon);
    SiD_ActiveVolV46->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV47 = new TGeoVolume("SiD_ActiveVolV47", sid_trapv7);
    SiD_ActiveVolV47->SetMedium(pMedsilicon);
    SiD_ActiveVolV47->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV48 = new TGeoVolume("SiD_ActiveVolV48", sid_trapv8);
    SiD_ActiveVolV48->SetMedium(pMedsilicon);
    SiD_ActiveVolV48->SetLineColor(kRed);
	
	
	TGeoVolume *SiD_ActiveVolV49 = new TGeoVolume("SiD_ActiveVolV49", sid_trapv1);
    SiD_ActiveVolV49->SetMedium(pMedsilicon);
    SiD_ActiveVolV49->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV50 = new TGeoVolume("SiD_ActiveVolV50", sid_trapv2);
    SiD_ActiveVolV50->SetMedium(pMedsilicon);
    SiD_ActiveVolV50->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV51 = new TGeoVolume("SiD_ActiveVolV51", sid_trapv3);
    SiD_ActiveVolV51->SetMedium(pMedsilicon);
    SiD_ActiveVolV51->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV52 = new TGeoVolume("SiD_ActiveVolV52", sid_trapv4);
    SiD_ActiveVolV52->SetMedium(pMedsilicon);
    SiD_ActiveVolV52->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV53 = new TGeoVolume("SiD_ActiveVolV53", sid_trapv5);
    SiD_ActiveVolV53->SetMedium(pMedsilicon);
    SiD_ActiveVolV53->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV54 = new TGeoVolume("SiD_ActiveVolV54", sid_trapv6);
    SiD_ActiveVolV54->SetMedium(pMedsilicon);
    SiD_ActiveVolV54->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV55 = new TGeoVolume("SiD_ActiveVolV55", sid_trapv7);
    SiD_ActiveVolV55->SetMedium(pMedsilicon);
    SiD_ActiveVolV55->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV56 = new TGeoVolume("SiD_ActiveVolV56", sid_trapv8);
    SiD_ActiveVolV56->SetMedium(pMedsilicon);
    SiD_ActiveVolV56->SetLineColor(kRed);
	
	
	TGeoVolume *SiD_ActiveVolV57 = new TGeoVolume("SiD_ActiveVolV57", sid_trapv1);
    SiD_ActiveVolV57->SetMedium(pMedsilicon);
    SiD_ActiveVolV57->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV58 = new TGeoVolume("SiD_ActiveVolV58", sid_trapv2);
    SiD_ActiveVolV58->SetMedium(pMedsilicon);
    SiD_ActiveVolV58->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV59 = new TGeoVolume("SiD_ActiveVolV59", sid_trapv3);
    SiD_ActiveVolV59->SetMedium(pMedsilicon);
    SiD_ActiveVolV59->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV60 = new TGeoVolume("SiD_ActiveVolV60", sid_trapv4);
    SiD_ActiveVolV60->SetMedium(pMedsilicon);
    SiD_ActiveVolV60->SetLineColor(kRed);
//********************************************************************************	
    TGeoVolume *SiD_ActiveVolV61 = new TGeoVolume("SiD_ActiveVolV61", sid_trapv5);
    SiD_ActiveVolV61->SetMedium(pMedsilicon);
    SiD_ActiveVolV61->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV62 = new TGeoVolume("SiD_ActiveVolV62", sid_trapv6);
    SiD_ActiveVolV62->SetMedium(pMedsilicon);
    SiD_ActiveVolV62->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV63 = new TGeoVolume("SiD_ActiveVolV63", sid_trapv7);
    SiD_ActiveVolV63->SetMedium(pMedsilicon);
    SiD_ActiveVolV63->SetLineColor(kRed);
	
	TGeoVolume *SiD_ActiveVolV64 = new TGeoVolume("SiD_ActiveVolV64", sid_trapv8);
    SiD_ActiveVolV64->SetMedium(pMedsilicon);
    SiD_ActiveVolV64->SetLineColor(kRed);
	
	TGeoVolume *Frame_mat = new TGeoVolume("Frame_mat", Frame_box);
    Frame_mat->SetMedium(pMedaluminium);
    Frame_mat->SetLineColor(kGreen-10);
	
	TGeoVolume *Membrane_mat = new TGeoVolume("Membrane_mat", Skin_tube);
    Membrane_mat->SetMedium(pMedaluminium);
    Membrane_mat->SetLineColor(kAzure-9);
	
    //Adding volumes to the TOP Volume
    top->AddNode(SidNewTop, 1, Sid_new_Pos_trans1);
   // top->AddNode(AlFrame, 2, Sid_new_Pos_trans1);
   
		
    SidNewTop->AddNode(SiD_ActiveVolV8, 1, position1);
    SidNewTop->AddNode(SiD_ActiveVolV7, 2, position2);
    SidNewTop->AddNode(SiD_ActiveVolV6, 3, position3);
    SidNewTop->AddNode(SiD_ActiveVolV5, 4, position4);
	
	SidNewTop->AddNode(SiD_ActiveVolV4, 5, position5);
    SidNewTop->AddNode(SiD_ActiveVolV3, 6, position6);
    SidNewTop->AddNode(SiD_ActiveVolV2, 7, position7);
    SidNewTop->AddNode(SiD_ActiveVolV1, 8, position8);
	
	SidNewTop->AddNode(SiD_ActiveVolV16, 9, position9);
    SidNewTop->AddNode(SiD_ActiveVolV15, 10, position10);
    SidNewTop->AddNode(SiD_ActiveVolV14, 11, position11);
    SidNewTop->AddNode(SiD_ActiveVolV13, 12, position12);
	
	SidNewTop->AddNode(SiD_ActiveVolV12, 13, position13);
    SidNewTop->AddNode(SiD_ActiveVolV11, 14, position14);
    SidNewTop->AddNode(SiD_ActiveVolV10, 15, position15);
    SidNewTop->AddNode(SiD_ActiveVolV9, 16, position16);
	
	SidNewTop->AddNode(SiD_ActiveVolV24, 17, position17);
    SidNewTop->AddNode(SiD_ActiveVolV23, 18, position18);
    SidNewTop->AddNode(SiD_ActiveVolV22, 19, position19);
    SidNewTop->AddNode(SiD_ActiveVolV21, 20, position20);
	
	SidNewTop->AddNode(SiD_ActiveVolV20, 21, position21);
    SidNewTop->AddNode(SiD_ActiveVolV19, 22, position22);
    SidNewTop->AddNode(SiD_ActiveVolV18, 23, position23);
    SidNewTop->AddNode(SiD_ActiveVolV17, 24, position24);
	
	SidNewTop->AddNode(SiD_ActiveVolV32, 25, position25);
    SidNewTop->AddNode(SiD_ActiveVolV31, 26, position26);
    SidNewTop->AddNode(SiD_ActiveVolV30, 27, position27);
    SidNewTop->AddNode(SiD_ActiveVolV29, 28, position28);
	
	SidNewTop->AddNode(SiD_ActiveVolV28, 29, position29);
    SidNewTop->AddNode(SiD_ActiveVolV27, 30, position30);
    SidNewTop->AddNode(SiD_ActiveVolV26, 31, position31);
    SidNewTop->AddNode(SiD_ActiveVolV25, 32, position32);
	
	SidNewTop->AddNode(SiD_ActiveVolV40, 33, position33);
    SidNewTop->AddNode(SiD_ActiveVolV39, 34, position34);
    SidNewTop->AddNode(SiD_ActiveVolV38, 35, position35);
    SidNewTop->AddNode(SiD_ActiveVolV37, 36, position36);
	
	SidNewTop->AddNode(SiD_ActiveVolV36, 37, position37);
    SidNewTop->AddNode(SiD_ActiveVolV35, 38, position38);
    SidNewTop->AddNode(SiD_ActiveVolV34, 39, position39);
    SidNewTop->AddNode(SiD_ActiveVolV33, 40, position40);

	SidNewTop->AddNode(SiD_ActiveVolV48, 41, position41);
    SidNewTop->AddNode(SiD_ActiveVolV47, 42, position42);
    SidNewTop->AddNode(SiD_ActiveVolV46, 43, position43);
    SidNewTop->AddNode(SiD_ActiveVolV45, 44, position44);
	
	SidNewTop->AddNode(SiD_ActiveVolV44, 45, position45);
    SidNewTop->AddNode(SiD_ActiveVolV43, 46, position46);
    SidNewTop->AddNode(SiD_ActiveVolV42, 47, position47);
    SidNewTop->AddNode(SiD_ActiveVolV41, 48, position48);
	
	SidNewTop->AddNode(SiD_ActiveVolV56, 49, position49);
    SidNewTop->AddNode(SiD_ActiveVolV55, 50, position50);
    SidNewTop->AddNode(SiD_ActiveVolV54, 51, position51);
    SidNewTop->AddNode(SiD_ActiveVolV53, 52, position52);
	
	SidNewTop->AddNode(SiD_ActiveVolV52, 53, position53);
    SidNewTop->AddNode(SiD_ActiveVolV51, 54, position54);
    SidNewTop->AddNode(SiD_ActiveVolV50, 55, position55);
    SidNewTop->AddNode(SiD_ActiveVolV49, 56, position56);
	
	SidNewTop->AddNode(SiD_ActiveVolV64, 57, position57);
    SidNewTop->AddNode(SiD_ActiveVolV63, 58, position58);
    SidNewTop->AddNode(SiD_ActiveVolV62, 59, position59);
    SidNewTop->AddNode(SiD_ActiveVolV61, 60, position60);
	
	SidNewTop->AddNode(SiD_ActiveVolV60, 61, position61);
    SidNewTop->AddNode(SiD_ActiveVolV59, 62, position62);
    SidNewTop->AddNode(SiD_ActiveVolV58, 63, position63);
    SidNewTop->AddNode(SiD_ActiveVolV57, 64, position64);
    
	SidNewTop->AddNode(Frame_mat, 65);
	SidNewTop->AddNode(Membrane_mat, 66, skin_tube_pos);
  
    top->SetVisContainers(kTRUE);
    
    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.02);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
    //top->Draw("");
}
