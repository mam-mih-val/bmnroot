//---------------------------
//M.Golubeva (INR RAS), December 2016  
//---------------------------

//create_rootgeom_zdc_oldnames_CBM_20mods_NICA_34mods_54mods_hole_v1(Double_t ZDC_Xpos=0,Double_t ZDC_Ypos=0,Double_t ZDC_Zpos=900,Double_t rotY=0)
//ZDC_Xpos - X coordinate of front surface center [cm]
//ZDC_Ypos - Y coordinate of front surface center [cm]
//ZDC_Zpos - Z coordinate of front surface center [cm]
//rotY - rotation around Y axis [deg.]

//Example of root file creation  
//root -l "create_rootgeom_zdc_oldnames_CBM_20mods_NICA_34mods_54mods_hole_v1(0,0,878.1,0)"


#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../run/bmnloadlibs.C"


//Detector's construct parameters ===================

//Global dimensions for ZDC in cm
Double_t Zdc_X_size = 77.5; //155./2.
Double_t Zdc_Y_size = 52.5; //105./2.
Double_t Zdc_Z_size = 62.4; //10 sect

Double_t Zdc_X_size_NICA = 37.5; //75./2.
Double_t Zdc_Y_size_NICA = 52.5; //105./2.

//Modules NICA
Double_t Zdc_NColumns_NICA = 5; 
Double_t Zdc_NRows_NICA = 7; 
Double_t Zdc_module_X_size_NICA = 7.5; //15./2.
Double_t Zdc_module_Y_size_NICA = 7.5; //15./2.
Double_t Zdc_module_Z_size_NICA = 44.04; //7 sect

//Modules
Double_t Zdc_Y_UP = 2.5;//cm
Double_t Zdc_NColumns = 2; 
Double_t Zdc_NRows = 5; 
Double_t Zdc_module_X_size = 10.; //20./2.
Double_t Zdc_module_Y_size = 10.; //20./2.
Double_t Zdc_module_Z_size = 62.4; //10 sect

//Fe slices
Double_t Zdc_Fe_X_size = 9.9; //(20.-2.*0.1)/2.
Double_t Zdc_Fe_Y_size = 9.85; //(20.-2.*0.15)/2.
Double_t Zdc_Fe_Z_size = 1.; //2./2.

//Pb slices
Double_t Zdc_NPb_in_module = 59;
Double_t Zdc_Pb_X_size = 9.9; //(20.-2.*0.1)/2.
Double_t Zdc_Pb_Y_size = 9.85; //(20.-2.*0.15)/2.
Double_t Zdc_Pb_Z_size = 0.8; //1.6/2.

//Scin. slices
//Double_t Zdc_NSc_in_module = 1;//put into Tyvec
Double_t Zdc_Sc_X_size = 9.88; //(20.-2.*0.1-2.*0.02)/2.
Double_t Zdc_Sc_Y_size = 9.83; //(20.-2.*0.15-2.*0.02)/2.
Double_t Zdc_Sc_Z_size = 0.2; //0.4/2.

//Tyvec slices
Double_t Zdc_NTyvec_in_module = 60;
Double_t Zdc_Tyvec_X_size = 9.9; //(20.-2.*0.1)/2.
Double_t Zdc_Tyvec_Y_size = 9.85; //(20.-2.*0.15)/2.
Double_t Zdc_Tyvec_Z_size = 0.22; //0.44/2.


//media ============================================
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedIron = 0;
TGeoMedium *pMedLead = 0;
TGeoMedium *pMedScint = 0;
TGeoMedium *pMedTyvec = 0;
TGeoMedium *pMedSlot = 0;

class FairGeoMedia;
class FairGeoBuilder;

void DefineRequiredMedia(FairGeoMedia* geoMedia, FairGeoBuilder* geoBuild) {
//vacuum medium
  FairGeoMedium* mVacuum = geoMedia->getMedium("vacuum");
  if ( ! mVacuum ) Fatal("Main", "FairMedium vacuum not found");
  geoBuild->createMedium(mVacuum);
  pMedVacuum = gGeoManager->GetMedium("vacuum");
  if ( ! pMedVacuum ) Fatal("Main", "Medium vacuum not found");

//air medium
  FairGeoMedium* mAir = geoMedia->getMedium("air");
  if ( ! mAir ) Fatal("Main", "FairMedium air not found");
  geoBuild->createMedium(mAir);
  pMedAir = gGeoManager->GetMedium("air");
  if ( ! pMedAir ) Fatal("Main", "Medium air not found");
  
//iron medium
  FairGeoMedium* mIron = geoMedia->getMedium("iron");
  if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mIron);
  pMedIron = gGeoManager->GetMedium("iron");
  if ( ! pMedIron ) Fatal("Main", "Medium iron not found");
    
//tyvec medium
  FairGeoMedium* mTyvec = geoMedia->getMedium("tyvec_ZDC");
  if ( ! mTyvec ) Fatal("Main", "FairMedium tyvec not found");
  geoBuild->createMedium(mTyvec);
  pMedTyvec = gGeoManager->GetMedium("tyvec_ZDC");
  if ( ! pMedTyvec ) Fatal("Main", "Medium tyvec not found");
  
//lead medium
  FairGeoMedium* mLead = geoMedia->getMedium("lead");
  if ( ! mLead ) Fatal("Main", "FairMedium lead not found");
  geoBuild->createMedium(mLead);
  pMedLead = gGeoManager->GetMedium("lead");
  if ( ! pMedLead ) Fatal("Main", "Medium lead not found");
    
// medium polystyrene (Scint.)
  FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
  if ( ! mPolystyrene ) Fatal("Main", "FairMedium polystyrene not found");
  geoBuild->createMedium(mPolystyrene);
  pMedScint = gGeoManager->GetMedium("polystyrene");
  if ( ! pMedScint ) Fatal("Main", "Medium polystyrene not found");

// medium plastic (slot)
  FairGeoMedium* mPlastic = geoMedia->getMedium("plasticWLS_ZDC");
  if ( ! mPlastic ) Fatal("Main", "FairMedium plastic not found");
  geoBuild->createMedium(mPlastic);
  pMedSlot = gGeoManager->GetMedium("plasticWLS_ZDC");
  if ( ! pMedSlot ) Fatal("Main", "Medium plastic not found");
    

}//DefineRequiredMedia

void FillModule_NICA(TGeoVolume* zdcModuleV_NICA);
void FillModule(TGeoVolume* zdcModuleV);

void create_rootgeom_fhcal_oldnames_CBM_20mods_NICA_34mods_54mods_hole_v1(Double_t ZDC_Xpos=0,Double_t ZDC_Ypos=0,Double_t ZDC_Zpos=900,Double_t rotY=0) {

  bmnloadlibs(); // load libraries

  // ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

  // -------   Geometry file name (output)   ---------------------------------
  const TString geoDetectorName = "fhcal_oldnames_CBM_20mods_NICA_34mods_54mods_hole";
  const TString geoDetectorVersion = "v1";
  Double_t ZDC_center_Zpos = ZDC_Zpos + Zdc_Z_size*TMath::Cos(rotY*TMath::DegToRad());//(878.1+62.4);//cm
  Double_t ZDC_center_Xpos = ZDC_Xpos + Zdc_Z_size*TMath::Sin(rotY*TMath::DegToRad());//(878.1+62.4);//cm

  TString sXshift_1 = "";
  TString sXshift_2 = "";

  TString sYshift_1 = "";
  TString sYshift_2 = "";

  TString sZshift_1 = "";
  TString sZshift_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t Xshift_1 = Int_t(ZDC_Xpos); //cm
  Int_t Xshift_2 = Int_t(0.5+100*(ZDC_Xpos-Int_t(ZDC_Xpos))); //cm

  Int_t Yshift_1 = Int_t(ZDC_Ypos); //cm
  Int_t Yshift_2 = Int_t(100*(ZDC_Ypos-Int_t(ZDC_Ypos))); //cm

  Int_t Zshift_1 = Int_t(ZDC_Zpos); //cm
  Int_t Zshift_2 = Int_t(10*(ZDC_Zpos-Int_t(ZDC_Zpos))); //cm

  Int_t rotY_1 = Int_t(rotY); //2
  Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)));//9

  sXshift_1 += Xshift_1;
  sXshift_2 += Xshift_2;
  sYshift_1 += Yshift_1;
  sYshift_2 += Yshift_2;
  sZshift_1 += Zshift_1;
  sZshift_2 += Zshift_2;

  sRotY_1 +=rotY_1;
  sRotY_2 +=rotY_2;
 
  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + "cm" + "_" + "Yshift_" + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";

  // ----  global geometry parameters  ---------------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  
  // -------   Load media from media file   ----------------------------------
  TString medFile = gPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  geoMedia->list();
  geoMedia->print();
  DefineRequiredMedia(geoMedia, geoBuild);

// -------------- Create geometry and global top volume ---------------------
  gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoManager->SetName(geoDetectorName + "_geom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  top->SetMedium(pMedAir);
  gGeoManager->SetTopVolume(top);
  //gGeoMan->SetTopVisible(1);

// Define ZDC Geometry
    TString zdc_common_name = "FHCAL_common";
    TGeoVolume *FHCAL_common = new TGeoVolumeAssembly(zdc_common_name);
    FHCAL_common->SetMedium(pMedAir);
    //ZDC_common->SetMedium(pMedVacuum);

    TString zdc_name = "fhcal01";
    TGeoBBox *ZDCS = new TGeoBBox(zdc_name, Zdc_X_size, Zdc_Y_size, Zdc_Z_size);
    TGeoVolume *ZDCV = new TGeoVolume(zdc_name, ZDCS);
    ZDCV->SetMedium(pMedAir);

    TString zdcModule_name = "fhcal01m";
    TGeoBBox *zdcModuleS = new TGeoBBox(zdcModule_name, Zdc_module_X_size, Zdc_module_Y_size, Zdc_module_Z_size);
    TGeoVolume *zdcModuleV = new TGeoVolume(zdcModule_name, zdcModuleS);
    zdcModuleV->SetMedium(pMedIron);

    TString zdcModule_name_NICA = "fhcal01m_NICA";
    TGeoBBox *zdcModuleS_NICA = new TGeoBBox(zdcModule_name_NICA, Zdc_module_X_size_NICA, Zdc_module_Y_size_NICA, Zdc_module_Z_size_NICA);
    TGeoVolume *zdcModuleV_NICA = new TGeoVolume(zdcModule_name_NICA, zdcModuleS_NICA);
    zdcModuleV_NICA->SetMedium(pMedIron);

//Fill ZDC with NICA modules
    Float_t xCur = Zdc_X_size_NICA + Zdc_module_X_size_NICA;//37.5+7.5=45. cm
    Float_t yCur = Zdc_Y_size_NICA + Zdc_module_Y_size_NICA;//52.5+7.5=59. cm

    Int_t iMod=-1, modNb=-1;
    for(Int_t iy=0; iy<Zdc_NRows_NICA; iy++) {//7
      yCur=yCur - 2.*Zdc_module_Y_size_NICA;
      for(Int_t ix=0; ix<Zdc_NColumns_NICA; ix++) {//5
	xCur=xCur - 2.*Zdc_module_X_size_NICA;
	
	TGeoTranslation *zdcModuleV_position_NICA = new TGeoTranslation(xCur,yCur,-(Zdc_module_Z_size-Zdc_module_Z_size_NICA));
	iMod=iy*Zdc_NColumns_NICA + ix;
	
	if(iMod<17) modNb=iMod+1;  //mods 1-17
	if(iMod==17) continue;
	//if(iMod==17) modNb=55;
	if(iMod>17) modNb=iMod;  //mods 18-54
	
	ZDCV->AddNode(zdcModuleV_NICA,modNb,zdcModuleV_position_NICA);
	cout <<"NICA modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;
	if(ix==Zdc_NColumns_NICA-1) xCur=Zdc_X_size_NICA + Zdc_module_X_size_NICA;
      }//for(Int_t ix==0; ix<7; ix++)
    }//for(Int_t iy==0; iy<7; iy++)
    
    //return;

//Fill ZDC with modules
    Float_t xCur_right=0;
    xCur = Zdc_X_size + Zdc_module_X_size;//77.5+10=87.5 cm
    yCur = Zdc_Y_size + Zdc_module_X_size_NICA;//52.5-7.5=60. cm
    iMod=-1; modNb=-1; 

    TGeoTranslation *zdcModuleV_position = new TGeoTranslation(xCur,yCur,0.);

    for(Int_t iy=0; iy<Zdc_NRows; iy++) {//5
      yCur=yCur - 2.*Zdc_module_Y_size;
      for(Int_t ix=0; ix<Zdc_NColumns; ix++) {//2
	xCur=xCur - 2.*Zdc_module_X_size;
	
	iMod=(iy*Zdc_NColumns + ix) + 34;
	modNb=iMod+1; 

	zdcModuleV_position = new TGeoTranslation(xCur,yCur,0.);
	ZDCV->AddNode(zdcModuleV,modNb,zdcModuleV_position);
	cout <<"CBM mods Left iMod,modNb,xxxx,yyyy " <<iMod+1 <<" " <<modNb <<" " <<xCur <<" " <<yCur <<"======= " <<endl;	  	  
	if(ix==0) modNb=iMod+1+11;
	if(ix==1) modNb=iMod+1+9;
	xCur_right = -xCur;
	zdcModuleV_position = new TGeoTranslation(xCur_right,yCur,0.);
	ZDCV->AddNode(zdcModuleV,modNb,zdcModuleV_position);
	cout <<"CBM mods right iMod,modNb,xxxx,yyyy " <<iMod+1 <<" " <<modNb <<" " <<xCur_right <<" " <<yCur <<endl;	  
	
	if(ix==Zdc_NColumns-1) xCur=Zdc_X_size + Zdc_module_X_size;;	
      }//for(Int_t ix==0; ix<7; ix++)
    }//for(Int_t iy==0; iy<7; iy++)

    //return;

    FillModule_NICA(zdcModuleV_NICA);
    FillModule(zdcModuleV);

//Adding the zdc mother volume to the global TOP Volume
    TGeoRotation *rot = new TGeoRotation("rot");
    rot->RotateY(rotY);
    TGeoCombiTrans *zdcZDC = new TGeoCombiTrans("zdcZDC",ZDC_center_Xpos, ZDC_Ypos, ZDC_center_Zpos,rot);
    FHCAL_common->AddNode(ZDCV, 1, zdcZDC);//ZDC right

    top->AddNode(FHCAL_common, 0);
    top->SetVisContainers(kTRUE);

// ---------------   Finish   ----------------------------------------------
    gGeoManager->CloseGeometry();
    gGeoManager->CheckOverlaps(0.001);
    gGeoManager->PrintOverlaps();

    gGeoManager->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();

    //TString gdmlname = gPath + "/geometry/tpc_v7.gdml";
    //gGeoManager->Export(gdmlname);

    //top->Draw("ogl");
    //TGLViewer *v = (TGLViewer*)gPad->GetViewer3D();
    //v->SetStyle(TGLRnrCtx::kOutline);
    //top->Draw("");

}//create_rootgeom_zdc_v1

void FillModule(TGeoVolume *mother_volume) {

 //Slot slices (divided by 2)
Double_t Zdc_Y_slot_position = 0.75;

Double_t Zdc_VRFM_X_size = 0.1; //Fe
Double_t Zdc_VRFM_Y_size = 8.6;
Double_t Zdc_VRFM_Z_size = 1.;

Double_t Zdc_VRPM_X_size = 0.1; //Pb
Double_t Zdc_VRPM_Y_size = 8.6;
Double_t Zdc_VRPM_Z_size = 0.8;

Double_t Zdc_VRYM_X_size = 0.01; //Tyvec_Y
Double_t Zdc_VRYM_Y_size = 8.6;
Double_t Zdc_VRYM_Z_size = 0.22;

Double_t Zdc_VRAM_X_size = 0.09; //Tyvec_A
Double_t Zdc_VRAM_Y_size = 8.6;
Double_t Zdc_VRAM_Z_size = 0.01;

Double_t Zdc_VRSM_X_size = 0.09; //Scint
Double_t Zdc_VRSM_Y_size = 8.6;
Double_t Zdc_VRSM_Z_size = 0.2;

 TString Fe_name = "Fe";
 TString slot_Fe_name = "Fe_slot";
 TString Pb_name = "fhcal01a";
 TString slot_Pb_name = "Pb_slot";
 TString SensitiveVolume_name = "fhcal01s";
 TString slot_Sc_name = "Sc_slot";
 TString Tyvec_name = "Tyvec";
 TString slot_Tyvec_Y_name = "Tyvec_slot_Y";
 TString slot_Tyvec_A_name = "Tyvec_slot_A";
 
//Shapes
 TGeoShape *FeS = new TGeoBBox(Fe_name, Zdc_Fe_X_size, Zdc_Fe_Y_size, Zdc_Fe_Z_size);
 TGeoVolume *FeV = new TGeoVolume(Fe_name, FeS);
 TGeoMedium *Fe_medium = pMedIron; //set medium
 if(Fe_medium) FeV->SetMedium(Fe_medium);
 else Fatal("Main", "Invalid medium for Fe!");
 
 TGeoShape *PbS = new TGeoBBox(Pb_name, Zdc_Pb_X_size, Zdc_Pb_Y_size, Zdc_Pb_Z_size);
 TGeoVolume *PbV = new TGeoVolume(Pb_name, PbS);
 TGeoMedium *Pb_medium = pMedLead; //set medium
 if(Pb_medium) PbV->SetMedium(Pb_medium);
 else Fatal("Main", "Invalid medium for Pb!");

 TGeoShape *zdcSensitiveVolumeS = new TGeoBBox(SensitiveVolume_name, Zdc_Sc_X_size, Zdc_Sc_Y_size, Zdc_Sc_Z_size);
 TGeoVolume *zdcSensitiveVolumeV = new TGeoVolume(SensitiveVolume_name, zdcSensitiveVolumeS);
 TGeoMedium *Sc_medium = pMedScint; //set medium
 if(Sc_medium) zdcSensitiveVolumeV->SetMedium(Sc_medium);
 else Fatal("Main", "Invalid medium for Sc!");

 TGeoShape *TyvecS = new TGeoBBox(Tyvec_name, Zdc_Tyvec_X_size, Zdc_Tyvec_Y_size, Zdc_Tyvec_Z_size);
 TGeoVolume *TyvecV = new TGeoVolume(Tyvec_name, TyvecS);
 TGeoMedium *Tyvec_medium = pMedTyvec; //set medium
 if(Tyvec_medium) TyvecV->SetMedium(Tyvec_medium);
 else Fatal("Main", "Invalid medium for Tyvec!");
    
//Slot's shapes
 TGeoShape *slot_FeS = new TGeoBBox(slot_Fe_name, Zdc_VRFM_X_size, Zdc_VRFM_Y_size, Zdc_VRFM_Z_size);
 TGeoVolume *slot_FeV = new TGeoVolume(slot_Fe_name, slot_FeS);
 TGeoMedium *slot_Fe_medium = pMedSlot; //set medium
 if(slot_Fe_medium) slot_FeV->SetMedium(slot_Fe_medium);
 else Fatal("Main", "Invalid medium for slot_Fe!");

 TGeoShape *slot_PbS = new TGeoBBox(slot_Pb_name, Zdc_VRPM_X_size, Zdc_VRPM_Y_size, Zdc_VRPM_Z_size);
 TGeoVolume *slot_PbV = new TGeoVolume(slot_Pb_name, slot_PbS);
 TGeoMedium *slot_Pb_medium = pMedSlot; //set medium
 if(slot_Pb_medium) slot_PbV->SetMedium(slot_Pb_medium);
 else Fatal("Main", "Invalid medium for slot_Pb!");

 TGeoShape *slot_ScS = new TGeoBBox(slot_Sc_name, Zdc_VRSM_X_size, Zdc_VRSM_Y_size, Zdc_VRSM_Z_size);
 TGeoVolume *slot_ScV = new TGeoVolume(slot_Sc_name, slot_ScS);
 TGeoMedium *slot_Sc_medium = pMedSlot; //set medium
 if(slot_Sc_medium) slot_ScV->SetMedium(slot_Sc_medium);
 else Fatal("Main", "Invalid medium for slot_Sc!");

 TGeoShape *slot_Tyvec_YS = new TGeoBBox(slot_Tyvec_Y_name, Zdc_VRYM_X_size, Zdc_VRYM_Y_size, Zdc_VRYM_Z_size);
 TGeoVolume *slot_Tyvec_YV = new TGeoVolume(slot_Tyvec_Y_name, slot_Tyvec_YS);
 TGeoMedium *slot_Tyvec_Y_medium = pMedSlot; //set medium
 if(slot_Tyvec_Y_medium) slot_Tyvec_YV->SetMedium(slot_Tyvec_Y_medium);
 else Fatal("Main", "Invalid medium for slot_Tyvec_Y!");
 
 TGeoShape *slot_Tyvec_AS = new TGeoBBox(slot_Tyvec_A_name, Zdc_VRAM_X_size, Zdc_VRAM_Y_size, Zdc_VRAM_Z_size);
 TGeoVolume *slot_Tyvec_AV = new TGeoVolume(slot_Tyvec_A_name, slot_Tyvec_AS);
 TGeoMedium *slot_Tyvec_A_medium = pMedSlot; //set medium
 if(slot_Tyvec_A_medium) slot_Tyvec_AV->SetMedium(slot_Tyvec_A_medium);
 else Fatal("Main", "Invalid medium for slot_Tyvec_A!");
 
//insert Fe
    mother_volume->AddNode(FeV,1,new TGeoTranslation(0, 0, -(Zdc_module_Z_size-Zdc_Fe_Z_size)));
    FeV->AddNode(slot_FeV,1,new TGeoTranslation(Zdc_Fe_X_size-Zdc_VRFM_X_size, Zdc_Y_slot_position, 0.));
    mother_volume->AddNode(FeV,2,new TGeoTranslation(0, 0, (Zdc_module_Z_size-Zdc_Fe_Z_size)));
    
//insert Tyvec
    mother_volume->AddNode(TyvecV, 1, new TGeoTranslation(0,0, -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size)));
    cout <<"tyvec 1 " <<-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size) <<endl;
    TyvecV->AddNode(slot_Tyvec_YV,1,new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRYM_X_size), Zdc_Y_slot_position, 0));
    TyvecV->AddNode(slot_Tyvec_AV, 1, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position, -(Zdc_Tyvec_Z_size-Zdc_VRAM_Z_size)));
    TyvecV->AddNode(slot_Tyvec_AV, 2, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position,(Zdc_Tyvec_Z_size-Zdc_VRAM_Z_size)));
    
//insert Scint into Tyvec
    TyvecV->AddNode(zdcSensitiveVolumeV, 1, new TGeoTranslation(0., 0., 0.));
    zdcSensitiveVolumeV->AddNode(slot_ScV, 1, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position, 0));
    
//insert Pb
    mother_volume->AddNode(PbV, 2, new TGeoTranslation(0, 0, -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-2.*Zdc_Tyvec_Z_size-Zdc_Pb_Z_size)));
    cout <<"Pb 2 " <<-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-2.*Zdc_Tyvec_Z_size-Zdc_Pb_Z_size) <<endl;
    PbV->AddNode(slot_PbV,1,new TGeoTranslation((Zdc_Pb_X_size-Zdc_VRPM_X_size), Zdc_Y_slot_position, 0));
    slot_PbV->SetLineColor(kBlue);
    
//insert Tyvec slices
    Double_t itInit = -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size); 
    Double_t itCur=0, itDelta=2.*(Zdc_Pb_Z_size+Zdc_Tyvec_Z_size);
    for(Int_t it=1+1; it<Zdc_NTyvec_in_module+1; it++) {//60 slices (1-60), the first slice is already inserted
      itCur =itInit + (it-1)*itDelta;
      mother_volume->AddNode(TyvecV,it,new TGeoTranslation(0,0,itCur));
      cout <<"it,itCur Ty " <<it <<" " <<itCur <<endl;
    }

//insert Pb slices    
    itInit=-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-2.*Zdc_Tyvec_Z_size-Zdc_Pb_Z_size); 
    itCur=0; itDelta=2.*(Zdc_Pb_Z_size+Zdc_Tyvec_Z_size);
    for(Int_t it=1+2; it<Zdc_NPb_in_module+2; it++) {//59 slices (2-60), the first slice is already inserted
      itCur =itInit + (it-2)*itDelta;
      mother_volume->AddNode(PbV,it,new TGeoTranslation(0,0,itCur));
      cout <<"it,itCur Pb " <<it <<" " <<itCur <<endl;
    }
    
}//FillModule

void FillModule_NICA(TGeoVolume *mother_volume_NICA) {

//Detector's construct parameters ===================

//Global dimensions for ZDC in cm
Double_t Zdc_X_size = 37.5; //75./2.
Double_t Zdc_Y_size = 52.5; //105./2.
Double_t Zdc_Z_size = 44.04; //7 sect

//Modules
Double_t Zdc_module_X_size = 7.5; //15./2.
Double_t Zdc_module_Y_size = 7.5; //15./2.
Double_t Zdc_module_Z_size = 44.04; //7 sect

//Fe slices
Double_t Zdc_Fe_X_size = 7.4; //(15.-2.*0.1)/2.
Double_t Zdc_Fe_Y_size = 7.35; //(15.-2.*0.15)/2.
Double_t Zdc_Fe_Z_size = 1.; //2./2.

//Pb slices
Double_t Zdc_NPb_in_module = 41;//7sect
Double_t Zdc_Pb_X_size = 7.4; //(15.-2.*0.1)/2.
Double_t Zdc_Pb_Y_size = 7.35; //(15.-2.*0.15)/2.
Double_t Zdc_Pb_Z_size = 0.8; //1.6/2.

//Scin. slices
Double_t Zdc_Sc_X_size = 7.38; //(15.-2.*0.1-2.*0.02)/2.
Double_t Zdc_Sc_Y_size = 7.33; //(15.-2.*0.15-2.*0.02)/2.
Double_t Zdc_Sc_Z_size = 0.2; //0.4/2.


//Tyvec slices
Double_t Zdc_NTyvec_in_module = 42;//7sect
Double_t Zdc_Tyvec_X_size = 7.4; //(15.-2.*0.1)/2.
Double_t Zdc_Tyvec_Y_size = 7.35; //(15.-2.*0.15)/2.
Double_t Zdc_Tyvec_Z_size = 0.22; //0.44/2.

//Slot slices (divided by 2)
Double_t Zdc_Y_slot_position = 0.75;

Double_t Zdc_VRFM_X_size = 0.1; //Fe
Double_t Zdc_VRFM_Y_size = 6.1;
Double_t Zdc_VRFM_Z_size = 1.;

Double_t Zdc_VRPM_X_size = 0.1; //Pb
Double_t Zdc_VRPM_Y_size = 6.1;
Double_t Zdc_VRPM_Z_size = 0.8;

Double_t Zdc_VRYM_X_size = 0.01; //Tyvec_Y
Double_t Zdc_VRYM_Y_size = 6.1;
Double_t Zdc_VRYM_Z_size = 0.22;

Double_t Zdc_VRAM_X_size = 0.09; //Tyvec_A
Double_t Zdc_VRAM_Y_size = 6.1;
Double_t Zdc_VRAM_Z_size = 0.01;

Double_t Zdc_VRSM_X_size = 0.09; //Scint
Double_t Zdc_VRSM_Y_size = 6.1;
Double_t Zdc_VRSM_Z_size = 0.2;

 TString Fe_name_NICA = "Fe_NICA";
 TString slot_Fe_name = "Fe_slot";
 TString Pb_name_NICA = "fhcal01a_NICA";
 TString slot_Pb_name = "Pb_slot";
 TString SensitiveVolume_name_NICA = "fhcal01s_NICA";
 TString slot_Sc_name = "Sc_slot";
 TString Tyvec_name_NICA = "Tyvec_NICA";
 TString slot_Tyvec_Y_name = "Tyvec_slot_Y";
 TString slot_Tyvec_A_name = "Tyvec_slot_A";
 
//Shapes
 TGeoShape *FeS_NICA = new TGeoBBox(Fe_name_NICA, Zdc_Fe_X_size, Zdc_Fe_Y_size, Zdc_Fe_Z_size);
 TGeoVolume *FeV_NICA = new TGeoVolume(Fe_name_NICA, FeS_NICA);
 TGeoMedium *Fe_medium_NICA = pMedIron; //set medium
 if(Fe_medium_NICA) FeV_NICA->SetMedium(Fe_medium_NICA);
 else Fatal("Main", "Invalid medium for Fe_NICA!");

 TGeoShape *PbS_NICA = new TGeoBBox(Pb_name_NICA, Zdc_Pb_X_size, Zdc_Pb_Y_size, Zdc_Pb_Z_size);
 TGeoVolume *PbV_NICA = new TGeoVolume(Pb_name_NICA, PbS_NICA);
 TGeoMedium *Pb_medium_NICA = pMedLead; //set medium
 if(Pb_medium_NICA) PbV_NICA->SetMedium(Pb_medium_NICA);
 else Fatal("Main", "Invalid medium for Pb_NICA!");

 TGeoShape *zdcSensitiveVolumeS_NICA = new TGeoBBox(SensitiveVolume_name_NICA, Zdc_Sc_X_size, Zdc_Sc_Y_size, Zdc_Sc_Z_size);
 TGeoVolume *zdcSensitiveVolumeV_NICA = new TGeoVolume(SensitiveVolume_name_NICA, zdcSensitiveVolumeS_NICA);
 TGeoMedium *Sc_medium_NICA = pMedScint; //set medium
 if(Sc_medium_NICA) zdcSensitiveVolumeV_NICA->SetMedium(Sc_medium_NICA);
 else Fatal("Main", "Invalid medium for Sc_NICA!");

 TGeoShape *TyvecS_NICA = new TGeoBBox(Tyvec_name_NICA, Zdc_Tyvec_X_size, Zdc_Tyvec_Y_size, Zdc_Tyvec_Z_size);
 TGeoVolume *TyvecV_NICA = new TGeoVolume(Tyvec_name_NICA, TyvecS_NICA);
 TGeoMedium *Tyvec_medium_NICA = pMedTyvec; //set medium
 if(Tyvec_medium_NICA) TyvecV_NICA->SetMedium(Tyvec_medium_NICA);
 else Fatal("Main", "Invalid medium for Tyvec_NICA!");
   
//Slot's shapes
 TGeoShape *slot_FeS = new TGeoBBox(slot_Fe_name, Zdc_VRFM_X_size, Zdc_VRFM_Y_size, Zdc_VRFM_Z_size);
 TGeoVolume *slot_FeV = new TGeoVolume(slot_Fe_name, slot_FeS);
 TGeoMedium *slot_Fe_medium = pMedSlot; //set medium
 if(slot_Fe_medium) slot_FeV->SetMedium(slot_Fe_medium);
 else Fatal("Main", "Invalid medium for slot_Fe!");
 
 TGeoShape *slot_PbS = new TGeoBBox(slot_Pb_name, Zdc_VRPM_X_size, Zdc_VRPM_Y_size, Zdc_VRPM_Z_size);
 TGeoVolume *slot_PbV = new TGeoVolume(slot_Pb_name, slot_PbS);
 TGeoMedium *slot_Pb_medium = pMedSlot; //set medium
 if(slot_Pb_medium) slot_PbV->SetMedium(slot_Pb_medium);
 else Fatal("Main", "Invalid medium for slot_Pb!");
 
 TGeoShape *slot_ScS = new TGeoBBox(slot_Sc_name, Zdc_VRSM_X_size, Zdc_VRSM_Y_size, Zdc_VRSM_Z_size);
 TGeoVolume *slot_ScV = new TGeoVolume(slot_Sc_name, slot_ScS);
 TGeoMedium *slot_Sc_medium = pMedSlot; //set medium
 if(slot_Sc_medium) slot_ScV->SetMedium(slot_Sc_medium);
 else Fatal("Main", "Invalid medium for slot_Sc!");
 
 TGeoShape *slot_Tyvec_YS = new TGeoBBox(slot_Tyvec_Y_name, Zdc_VRYM_X_size, Zdc_VRYM_Y_size, Zdc_VRYM_Z_size);
 TGeoVolume *slot_Tyvec_YV = new TGeoVolume(slot_Tyvec_Y_name, slot_Tyvec_YS);
 TGeoMedium *slot_Tyvec_Y_medium = pMedSlot; //set medium
 if(slot_Tyvec_Y_medium) slot_Tyvec_YV->SetMedium(slot_Tyvec_Y_medium);
 else Fatal("Main", "Invalid medium for slot_Tyvec_Y!");
 
 TGeoShape *slot_Tyvec_AS = new TGeoBBox(slot_Tyvec_A_name, Zdc_VRAM_X_size, Zdc_VRAM_Y_size, Zdc_VRAM_Z_size);
 TGeoVolume *slot_Tyvec_AV = new TGeoVolume(slot_Tyvec_A_name, slot_Tyvec_AS);
 TGeoMedium *slot_Tyvec_A_medium = pMedSlot; //set medium
 if(slot_Tyvec_A_medium) slot_Tyvec_AV->SetMedium(slot_Tyvec_A_medium);
 else Fatal("Main", "Invalid medium for slot_Tyvec_A!");
    
//insert Fe
 mother_volume_NICA->AddNode(FeV_NICA,1,new TGeoTranslation(0, 0, -(Zdc_module_Z_size-Zdc_Fe_Z_size)));
 FeV_NICA->AddNode(slot_FeV,1,new TGeoTranslation(Zdc_Fe_X_size-Zdc_VRFM_X_size, Zdc_Y_slot_position, 0.));
 mother_volume_NICA->AddNode(FeV_NICA,2,new TGeoTranslation(0, 0, (Zdc_module_Z_size-Zdc_Fe_Z_size)));
 
//insert Tyvec
 mother_volume_NICA->AddNode(TyvecV_NICA, 1, new TGeoTranslation(0,0, -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size)));
 cout <<"NICA tyvec 1 " <<-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size) <<endl;
 TyvecV_NICA->AddNode(slot_Tyvec_YV,1,new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRYM_X_size), Zdc_Y_slot_position, 0));
 TyvecV_NICA->AddNode(slot_Tyvec_AV, 1, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position, -(Zdc_Tyvec_Z_size-Zdc_VRAM_Z_size)));
 TyvecV_NICA->AddNode(slot_Tyvec_AV, 2, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position,(Zdc_Tyvec_Z_size-Zdc_VRAM_Z_size)));
 
//insert Scint into Tyvec
 TyvecV_NICA->AddNode(zdcSensitiveVolumeV_NICA, 1, new TGeoTranslation(0., 0., 0.));
 zdcSensitiveVolumeV_NICA->AddNode(slot_ScV, 1, new TGeoTranslation((Zdc_Tyvec_X_size-Zdc_VRAM_X_size-2.*Zdc_VRYM_X_size), Zdc_Y_slot_position, 0));
 
 //insert Pb
 mother_volume_NICA->AddNode(PbV_NICA, 2, new TGeoTranslation(0, 0, -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-2.*Zdc_Tyvec_Z_size-Zdc_Pb_Z_size)));
 cout <<"NICA Pb 2 " <<-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-2.*Zdc_Tyvec_Z_size-Zdc_Pb_Z_size) <<endl;
 PbV_NICA->AddNode(slot_PbV,1,new TGeoTranslation((Zdc_Pb_X_size-Zdc_VRPM_X_size), Zdc_Y_slot_position, 0));
 slot_PbV->SetLineColor(kBlue);
    
//insert Tyvec slices
 Double_t itInit = -(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-Zdc_Tyvec_Z_size); 
 Double_t itCur=0, itDelta=2.*(Zdc_Pb_Z_size+Zdc_Tyvec_Z_size);
 for(Int_t it=1+1; it<Zdc_NTyvec_in_module+1; it++) {//60 slices (1-60), the first slice is already inserted
   itCur =itInit + (it-1)*itDelta;
   mother_volume_NICA->AddNode(TyvecV_NICA,it,new TGeoTranslation(0,0,itCur));
   cout <<"NICA it,itCur Ty " <<it <<" " <<itCur <<endl;
 }

//insert Pb slices    
 itInit=-(Zdc_module_Z_size-2.*Zdc_Fe_Z_size-2.*Zdc_Tyvec_Z_size-Zdc_Pb_Z_size); 
 itCur=0; itDelta=2.*(Zdc_Pb_Z_size+Zdc_Tyvec_Z_size);
 for(Int_t it=1+2; it<Zdc_NPb_in_module+2; it++) {//59 slices (2-60), the first slice is already inserted
   itCur =itInit + (it-2)*itDelta;
   mother_volume_NICA->AddNode(PbV_NICA,it,new TGeoTranslation(0,0,itCur));
   cout <<"NICA it,itCur Pb " <<it <<" " <<itCur <<endl;
 }
             
 cout <<"Zdc_module_Z_size " <<Zdc_module_Z_size <<endl;


}//FillModule_NICA

