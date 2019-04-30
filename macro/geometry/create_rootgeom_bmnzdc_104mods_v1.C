//---------------------------
//M.Golubeva (INR RAS), February 2019
//S.Morozov (INR RAS), April 2019 (code optimisation)
//---------------------------

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../run/bmnloadlibs.C"

//Detector's construct parameters ===================

Int_t nSlicesInModule = 64;

//Global dimensions for ZDC in cm
Double_t Zdc_X_size = 82.5; //165./2.
Double_t Zdc_Y_size = 52.5; //105./2.
Double_t Zdc_Z_size = 52.5; //68 scint

Double_t Zdc_X_size_small = 22.5; //45./2. //small mods
Double_t Zdc_Y_size_small = 22.5; //45./2.

//Modules small //small modules
Double_t Zdc_NColumns_small = 6;
Double_t Zdc_NRows_small = 6;
Double_t Zdc_module_X_size_small = 3.75; //7.5/2.
Double_t Zdc_module_Y_size_small = 3.75; //7.5/2.
Double_t Zdc_module_Z_size_small = 52.5; //105./2.

//Modules //large modules
Double_t Zdc_NColumns = 11;
Double_t Zdc_NRows = 7;
Double_t Zdc_module_X_size = 7.5; //15./2.
Double_t Zdc_module_Y_size = 7.5; //15./2.
Double_t Zdc_module_Z_size = 52.5; //105./2.


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
  if ( ! mTyvec ) Fatal("Main", "FairMedium tyvec_ZDC not found");
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

/*
  // medium FscScint (Scint.)
  FairGeoMedium* mPolystyrene = geoMedia->getMedium("FscScint");
  if ( ! mPolystyrene ) Fatal("Main", "FairMedium FscScint not found");
  geoBuild->createMedium(mPolystyrene);
  pMedScint = gGeoManager->GetMedium("FscScint");
  if ( ! pMedScint ) Fatal("Main", "Medium FscScint not found");
*/


// medium plastic (slot)
  FairGeoMedium* mPlastic = geoMedia->getMedium("plasticWLS_ZDC");
  if ( ! mPlastic ) Fatal("Main", "FairMedium plastic not found");
  geoBuild->createMedium(mPlastic);
  pMedSlot = gGeoManager->GetMedium("plasticWLS_ZDC");
  if ( ! pMedSlot ) Fatal("Main", "Medium plasticWLS_ZDC not found");


}//DefineRequiredMedia


void FillModule_small(TGeoVolume* zdcModuleV_small);  //small modules
void FillModule(TGeoVolume* zdcModuleV); //large modules


void create_rootgeom_bmnzdc_104mods_v1 (Double_t ZDC_Xpos=0, Double_t ZDC_Ypos=0, Double_t ZDC_Zpos=900) {

  bmnloadlibs(); // load libraries

  // ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

  // -------   Geometry file name (output)   ---------------------------------
  const TString geoDetectorName = "rootgeom_bmnzdc_104mods";
  const TString geoDetectorVersion = "v1";
  Double_t ZDC_center_Zpos = ZDC_Zpos + Zdc_Z_size; //cm
  TString sXshift = "";
  TString sYshift = "";
  TString sZshift = "";
  Int_t Xshift = Int_t(ZDC_Xpos*10.); //mm
  Int_t Yshift = Int_t(ZDC_Ypos*10.); //mm
  Int_t Zshift = Int_t(ZDC_Zpos*10.); //mm
  sXshift += Xshift;
  sYshift += Yshift;
  sZshift += Zshift;
  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_"
                              + geoDetectorVersion + "_"
                              + "Zpos_" + sZshift + "mm" + "_"
                              + "Xshift_" + sXshift + "mm" + "_" + "Yshift_" + sYshift + "mm"
                              + ".root";
  // -------------------------------------------------------------------------

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
  // -------------------------------------------------------------------------

  // --------------   Create geometry and global top volume  -------------
  gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoManager->SetName(geoDetectorName + "_geom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  top->SetMedium(pMedAir);
  gGeoManager->SetTopVolume(top);
  //gGeoMan->SetTopVisible(1);
  // ------------------------------------------------------------------------

// Define ZDC Geometry
  TString zdc_common_name = "ZDC_common";
  TGeoVolume *ZDC_common = new TGeoVolumeAssembly(zdc_common_name);
  ZDC_common->SetMedium(pMedAir);
  //ZDC_common->SetMedium(pMedVacuum);

  TString zdc_name = "zdc01";
  TGeoBBox *ZDCS = new TGeoBBox(zdc_name, Zdc_X_size, Zdc_Y_size, Zdc_Z_size);
  TGeoVolume *ZDCV = new TGeoVolume(zdc_name, ZDCS);
  ZDCV->SetMedium(pMedAir);

  TString zdcModule_name = "zdc01m"; //large mods
  TGeoBBox *zdcModuleS = new TGeoBBox(zdcModule_name, Zdc_module_X_size, Zdc_module_Y_size, Zdc_module_Z_size);
  TGeoVolume *zdcModuleV = new TGeoVolume(zdcModule_name, zdcModuleS);
  zdcModuleV->SetMedium(pMedIron);

  TString zdcModule_name_small = "zdc01m_small"; //small mods
  TGeoBBox *zdcModuleS_small = new TGeoBBox(zdcModule_name_small, Zdc_module_X_size_small, Zdc_module_Y_size_small, Zdc_module_Z_size_small);
  TGeoVolume *zdcModuleV_small = new TGeoVolume(zdcModule_name_small, zdcModuleS_small);
  zdcModuleV_small->SetMedium(pMedIron);

  cout <<"ZDC_pos " <<ZDC_Xpos <<" " <<ZDC_Ypos <<endl;


  int iMod = 0;

  for (int i = 0; i < 77; i++) {
    int iCol = i / 7;
    int iRow = i % 7;
    if((i >= 30 && i <= 32) || (i >= 37 && i <= 39) || (i >= 44 && i <= 46)) continue;
    Double_t xMod = 75. - 15. * iCol;
    Double_t yMod = -45. + 15. * iRow;
      TGeoTranslation *zdcModuleV_position = new TGeoTranslation(xMod, yMod, 0.);
      ZDCV->AddNode(zdcModuleV, iMod+1, zdcModuleV_position);
    std::cout << "iMod = " << iMod+1 << " at " << xMod << ", " << yMod << std::endl;
    iMod++;
  }


  for (int i = 0; i < 36; i++) {
    int iCol = i / 6;
    int iRow = i % 6;
    Double_t xMod = 18.75 - 7.5 * iCol;
    Double_t yMod = -18.75 + 7.5 * iRow;
    TGeoTranslation *zdcModuleV_position = new TGeoTranslation(xMod, yMod, 0.);
    ZDCV->AddNode(zdcModuleV_small, iMod+1, zdcModuleV_position);
    std::cout << "iMod = " << iMod+1 << " at " << xMod << ", " << yMod << std::endl;
    iMod++;
  }

//Fill module with Fe-Sc-Lead slices
  FillModule_small(zdcModuleV_small);
  FillModule(zdcModuleV);

//Adding the zdc mother volume to the global TOP Volume
  TGeoTranslation *zdcZDC = new TGeoTranslation("zdcZDC",ZDC_Xpos, ZDC_Ypos, ZDC_center_Zpos);
  ZDC_common->AddNode(ZDCV, 1, zdcZDC);//ZDC

  top->AddNode(ZDC_common, 0);
  top->SetVisContainers(kTRUE);

  // ---------------   Finish   ----------------------------------------------
  gGeoManager->CloseGeometry();
  gGeoManager->CheckOverlaps(0.001);
  gGeoManager->PrintOverlaps();

  gGeoManager->Test();

  TFile* geoFile = new TFile(geoFileName, "RECREATE");
  top->Write();
  geoFile->Close();

  //TString gdmlname = gPath + "/geometry/zdc_v1.gdml";
  //gGeoManager->Export(gdmlname);

  //top->Draw("ogl");
  //TGLViewer *v = (TGLViewer*)gPad->GetViewer3D();
  //v->SetStyle(TGLRnrCtx::kOutline);
  //top->Draw("");

}//create_rootgeom_zdc_v1

void FillModule(TGeoVolume *mother_volume) {

//Fe slices //large modules
  Double_t Zdc_Fe_X_size = 7.3; //(15.-2.*0.2)/2.
  Double_t Zdc_Fe_Y_size = 7.3; //(15.-2.*0.2)/2.
  Double_t Zdc_Fe_Z_size = 1.; //2./2.

//Pb slices //large modules
  Double_t Zdc_Pb_X_size = 7.3; //(15.-2.*0.2)/2.
  Double_t Zdc_Pb_Y_size = 7.1; //(15.-2.*0.2-0.4)/2.
  Double_t Zdc_Pb_Z_size = 0.5; //1./2.

//Scin. slices //large modules
  Double_t Zdc_Sc_X_size = 7.3; //(15.-2.*0.2)/2.
  Double_t Zdc_Sc_Y_size = 7.1; //(15.-2.*0.2-0.4)/2.
  Double_t Zdc_Sc_Z_size = 0.25; //0.5/2.

  Double_t Zdc_Y_Pb_Sc_position = -0.2;

//Slot slices (divided by 2)
  Double_t Zdc_VRMM_X_size = 7.3; //Mod
  Double_t Zdc_VRMM_Y_size = 0.2;
  Double_t Zdc_VRMM_Z_size = 50.5;

  Double_t Zdc_Y_slot_position;
  Zdc_Y_slot_position = Zdc_Fe_Y_size - Zdc_VRMM_Y_size;//7.1
  cout <<"Zdc_Y_slot_position " <<Zdc_Y_slot_position <<endl;

  TString Fe_name = "Fe";
  TString Pb_name = "zdc01a";
  TString SensitiveVolume_name = "zdc01s";
  TString slot_Mod_name = "Mod_slot";

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

//Slot shape
  TGeoShape *slot_ModS = new TGeoBBox(slot_Mod_name, Zdc_VRMM_X_size, Zdc_VRMM_Y_size, Zdc_VRMM_Z_size);
  TGeoVolume *slot_ModV = new TGeoVolume(slot_Mod_name, slot_ModS);
  TGeoMedium *slot_Mod_medium = pMedSlot; //set medium
  if(slot_Mod_medium) slot_ModV->SetMedium(slot_Mod_medium);
  else Fatal("Main", "Invalid medium for slot_Mod!");


  Double_t zPos = -Zdc_module_Z_size + Zdc_Fe_Z_size;

//insert Fe
  mother_volume->AddNode(FeV, 1, new TGeoTranslation(0, 0, zPos));
  zPos = zPos + Zdc_Fe_Z_size;

//insert Mod_slot
  mother_volume->AddNode(slot_ModV, 1, new TGeoTranslation(0, Zdc_Y_slot_position, 0));
  slot_ModV->SetLineColor(kBlue);

//insert 1st Scint
  zPos = zPos + Zdc_Sc_Z_size;
  mother_volume->AddNode(zdcSensitiveVolumeV, 1, new TGeoTranslation(0, Zdc_Y_Pb_Sc_position, zPos));
  cout <<"Sc 1 " << zPos <<endl;
  zPos = zPos + Zdc_Sc_Z_size;

  for(Int_t iSlice = 0; iSlice < (nSlicesInModule - 1); iSlice++) {

    //insert Pb
    zPos = zPos + Zdc_Pb_Z_size;
    mother_volume->AddNode(PbV, iSlice+1, new TGeoTranslation(0, Zdc_Y_Pb_Sc_position, zPos));
    cout << "Pb " << iSlice+1 << " " << zPos <<endl;
    zPos = zPos + Zdc_Pb_Z_size;

    //insert Sc
    zPos = zPos + Zdc_Sc_Z_size;
    mother_volume->AddNode(zdcSensitiveVolumeV, iSlice+2, new TGeoTranslation(0, Zdc_Y_Pb_Sc_position, zPos));
    cout << "Sc " << iSlice+2 << " " << zPos <<endl;
    zPos = zPos + Zdc_Sc_Z_size;

  }

}//FillModule


void FillModule_small(TGeoVolume *mother_volume_small) {

//Detector's construct parameters ===================

//Fe slices
  Double_t Zdc_Fe_X_size = 3.55; //(7.5-2.*0.2)/2.
  Double_t Zdc_Fe_Y_size = 3.55; //(7.5-2.*0.2)/2.
  Double_t Zdc_Fe_Z_size = 1.; //2./2.

//Pb slices
  Double_t Zdc_Pb_X_size = 3.55; //(7.5-2.*0.2)/2.
  Double_t Zdc_Pb_Y_size = 3.35; //(7.5-2.*0.2-0.4)/2.
  Double_t Zdc_Pb_Z_size = 0.5; //1./2.

//Scin. slices
  Double_t Zdc_Sc_X_size = 3.55; //(7.5-2.*0.2)/2.
  Double_t Zdc_Sc_Y_size = 3.35; //(7.5-2.*0.2-0.4)/2.
  Double_t Zdc_Sc_Z_size = 0.25; //0.5/2.

  Double_t Zdc_Y_Pb_Sc_position = -0.2;

//Slot slices (divided by 2)
  Double_t Zdc_VRMM_X_size = 3.55;
  Double_t Zdc_VRMM_Y_size = 0.2;
  Double_t Zdc_VRMM_Z_size = 50.5;

  Double_t Zdc_Y_slot_position;

  Zdc_Y_slot_position = Zdc_Fe_Y_size-Zdc_VRMM_Y_size;//3.35
  cout <<"Zdc_Y_slot_position small " <<Zdc_Y_slot_position <<endl;

  TString Fe_name_small = "Fe_small";
  TString Pb_name_small = "zdc01a_small";
  TString SensitiveVolume_name_small = "zdc01s_small";
  TString slot_Mod_name = "Mod_slot";

//Shapes
  TGeoShape *FeS_small = new TGeoBBox(Fe_name_small, Zdc_Fe_X_size, Zdc_Fe_Y_size, Zdc_Fe_Z_size);
  TGeoVolume *FeV_small = new TGeoVolume(Fe_name_small, FeS_small);
  TGeoMedium *Fe_medium_small = pMedIron; //set medium
  if(Fe_medium_small) FeV_small->SetMedium(Fe_medium_small);
  else Fatal("Main", "Invalid medium for Fe_small!");

  TGeoShape *PbS_small = new TGeoBBox(Pb_name_small, Zdc_Pb_X_size, Zdc_Pb_Y_size, Zdc_Pb_Z_size);
  TGeoVolume *PbV_small = new TGeoVolume(Pb_name_small, PbS_small);
  TGeoMedium *Pb_medium_small = pMedLead; //set medium
  if(Pb_medium_small) PbV_small->SetMedium(Pb_medium_small);
  else Fatal("Main", "Invalid medium for Pb_small!");

  TGeoShape *zdcSensitiveVolumeS_small = new TGeoBBox(SensitiveVolume_name_small, Zdc_Sc_X_size, Zdc_Sc_Y_size, Zdc_Sc_Z_size);
  TGeoVolume *zdcSensitiveVolumeV_small = new TGeoVolume(SensitiveVolume_name_small, zdcSensitiveVolumeS_small);
  TGeoMedium *Sc_medium_small = pMedScint; //set medium
  if(Sc_medium_small) zdcSensitiveVolumeV_small->SetMedium(Sc_medium_small);
  else Fatal("Main", "Invalid medium for Sc_small!");

//Slot's shape
  TGeoShape *slot_ModS = new TGeoBBox(slot_Mod_name, Zdc_VRMM_X_size, Zdc_VRMM_Y_size, Zdc_VRMM_Z_size);
  TGeoVolume *slot_ModV = new TGeoVolume(slot_Mod_name, slot_ModS);
  TGeoMedium *slot_Mod_medium = pMedSlot; //set medium
  if(slot_Mod_medium) slot_ModV->SetMedium(slot_Mod_medium);
  else Fatal("Main", "Invalid medium for slot_Mod!");

  Double_t zPos = -Zdc_module_Z_size_small + Zdc_Fe_Z_size;

//insert Fe
  mother_volume_small->AddNode(FeV_small, 1, new TGeoTranslation(0, 0, zPos));
  zPos = zPos + Zdc_Fe_Z_size;

//insert Mod_slot
  mother_volume_small->AddNode(slot_ModV, 1, new TGeoTranslation(0, Zdc_Y_slot_position, 0));
  slot_ModV->SetLineColor(kBlue);

//insert 1st Scint
  zPos = zPos + Zdc_Sc_Z_size;
  mother_volume_small->AddNode(zdcSensitiveVolumeV_small, 1, new TGeoTranslation(0, Zdc_Y_Pb_Sc_position, zPos));
  cout <<"Sc 1 " << zPos <<endl;
  zPos = zPos + Zdc_Sc_Z_size;

  for(Int_t iSlice = 0; iSlice < (nSlicesInModule - 1); iSlice++) {

    //insert Pb
    zPos = zPos + Zdc_Pb_Z_size;
    mother_volume_small->AddNode(PbV_small, iSlice+1, new TGeoTranslation(0, Zdc_Y_Pb_Sc_position, zPos));
    cout << "Pb " << iSlice+1 << " " << zPos <<endl;
    zPos = zPos + Zdc_Pb_Z_size;

    //insert Sc
    zPos = zPos + Zdc_Sc_Z_size;
    mother_volume_small->AddNode(zdcSensitiveVolumeV_small, iSlice+2, new TGeoTranslation(0, Zdc_Y_Pb_Sc_position, zPos));
    cout << "Sc " << iSlice+2 << " " << zPos <<endl;
    zPos = zPos + Zdc_Sc_Z_size;

  }


}//FillModule_small

