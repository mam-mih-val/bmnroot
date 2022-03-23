//---------------------------
//M.Golubeva (INR RAS), January 2021  
//---------------------------

//create_rootgeom_ScWall_oldnames_no_hole_v1(Double_t ScWall_Xpos=0,Double_t ScWall_Ypos=0,Double_t ScWall_Zpos=900,Double_t rotY=0)
//ScWall_Xpos - X coordinate of front surface center [cm]
//ScWall_Ypos - Y coordinate of front surface center [cm]
//ScWall_Zpos - Z coordinate of front surface center [cm]
//rotY - rotation around Y axis [deg.]

//Example of root file creation  
//root -l "create_rootgeom_ScWall_oldnames_with_hole_with_box_v1.C(0,0,900,0)"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../run/bmnloadlibs.C"


//Detector's construct parameters ===================

//Global dimensions for ScWall in cm (Scint)
Double_t ScWall_X_size = 135; //270./2.
Double_t ScWall_Y_size = 60; //120./2.
Double_t ScWall_Z_size = 0.5; //half of 10 mm

Double_t ScWall_X_size_box = 145; //290./2.//Scint+box
Double_t ScWall_Y_size_box = 70; //140./2.
Double_t ScWall_Z_size_box = 2.3; //half of 4.6 cm

Double_t ScWall_X_size_box_left = 80; //160./2.//Scint+box
Double_t ScWall_Y_size_box_left = 70; //140./2.
//Double_t ScWall_Z_size_box = 2.3; //half of 4.6 cm

Double_t ScWall_X_size_box_right = 57.5; //115./2.//Scint+box
Double_t ScWall_Y_size_box_right = 70; //140./2.
//Double_t ScWall_Z_size_box = 2.3; //half of 4.6 cm

Double_t ScWall_X_size_box_top = 7.5; //15./2.//Scint+box
Double_t ScWall_Y_size_box_top = 31.25; //62.5/2.
//Double_t ScWall_Z_size_box = 2.3; //half of 4.6 cm
/*
Double_t ScWall_X_size_box_bottom = 7.5; //15./2.//Scint+box
Double_t ScWall_Y_size_box_bottom = 31.25; //62.5/2.
//Double_t ScWall_Z_size_box = 2.3; //half of 4.6 cm
*/
//Al plates
Double_t ScWall_Z_size_Al_plate = 0.15; //half of 0.3 cm

//Air inside box
Double_t ScWall_Z_size_Air_plate = 1.5; //half of 3 cm

//SMALL cells
Double_t ScWall_NColumns_small = 10; 
Double_t ScWall_NRows_small = 4; 
Double_t ScWall_X_size_small = 37.5; //7.5*5. 
Double_t ScWall_Y_size_small = 15; //7.5*2.
Double_t ScWall_full_cell_X_size_small = 3.75; //7.5/2. //before angles cuts
Double_t ScWall_full_cell_Y_size_small = 3.75; //7.5/2.
Double_t ScWall_full_cell_Z_size_small = 0.5; //half of 10 mm

//LARGE cells
Double_t ScWall_NColumns_large = 18; 
Double_t ScWall_NRows_large = 8; 
Double_t ScWall_X_size_large = 135; //15*9. 
Double_t ScWall_Y_size_large = 60; //15*4.
Double_t ScWall_full_cell_X_size_large = 7.5; //15/2. //before angles cuts
Double_t ScWall_full_cell_Y_size_large = 7.5; //15/2.
Double_t ScWall_full_cell_Z_size_large = 0.5; //half of 10 mm

//Trap dimensions  REAL dimensions
Double_t trd1_side_X = 0.5; //angle side 
Double_t trd1_side_Y = 0.5; //angle side
Double_t trd1_thickness = 0.5; //half of 1 cm
Double_t trd1_size_Z = 0.25; //half of 0.5cm

//Trap dimensions for TEST
//Double_t trd1_side_X = 2; //angle side
//Double_t trd1_side_Y = 2; //angle side
//Double_t trd1_thickness = 0.5; //half of 1 cm
//Double_t trd1_size_Z = 1; //half of 2cm


//media ============================================
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedScint = 0;
TGeoMedium *pMedAlum = 0;

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
    
    // medium polystyrene (Scint.)
    FairGeoMedium* mPolystyrene = geoMedia->getMedium("polystyrene");
    if ( ! mPolystyrene ) Fatal("Main", "FairMedium polystyrene not found");
    geoBuild->createMedium(mPolystyrene);
    pMedScint = gGeoManager->GetMedium("polystyrene");
    if ( ! pMedScint ) Fatal("Main", "Medium polystyrene not found");

    // medium aluminium (ScWall box)
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if ( ! mAluminium ) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    pMedAlum = gGeoManager->GetMedium("aluminium");
    if ( ! pMedAlum ) Fatal("Main", "Medium aluminium not found");

}//DefineRequiredMedia

void FillCell_small(TGeoVolume* scwallCellV_small);
void FillCell_large(TGeoVolume* scwallCellV_large);


void create_rootgeom_ScWall_oldnames_with_hole_with_hole_in_box_v1(Double_t ScWall_Xpos=0,Double_t ScWall_Ypos=0,Double_t ScWall_Zpos=900,Double_t rotY=0) {
// Load necessary libraries
  //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load libraries

// ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

// -------   Geometry file name (output)   ---------------------------------
  const TString geoDetectorName = "ScWall_oldnames_with_hole_with_hole_in_box";
  const TString geoDetectorVersion = "v1";

  Double_t ScWall_center_Zpos = ScWall_Zpos + ScWall_Z_size_box*TMath::Cos(rotY*TMath::DegToRad());//(900+2.3);//cm
  Double_t ScWall_center_Xpos = ScWall_Xpos + ScWall_Z_size_box*TMath::Sin(rotY*TMath::DegToRad());//cm

  TString sXshift_1 = "";
  TString sXshift_2 = "";

  TString sYshift_1 = "";
  TString sYshift_2 = "";

  TString sZshift_1 = "";
  TString sZshift_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t Zshift_1 = Int_t(ScWall_Zpos); //cm
  sZshift_1 += Zshift_1;
  Int_t Zshift_2 = Int_t(10*(ScWall_Zpos-Int_t(ScWall_Zpos))); //cm
  sZshift_2 += Zshift_2;

  Int_t Xshift_1 = Int_t(ScWall_Xpos); //cm
  sXshift_1 += Xshift_1;
  Int_t Xshift_2 = Int_t(0.5+100*(ScWall_Xpos-Int_t(ScWall_Xpos))); //cm
  sXshift_2 += Xshift_2;

  Int_t Yshift_1 = Int_t(ScWall_Ypos); //cm
  sYshift_1 += Yshift_1;
  Int_t Yshift_2 = Int_t(100*(ScWall_Ypos-Int_t(ScWall_Ypos))); //cm
  sYshift_2 += Yshift_2;

  Int_t rotY_1 = Int_t(rotY);
  sRotY_1 +=rotY_1;
  Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)));
  sRotY_2 +=rotY_2;

  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + "cm" + "_" + "Yshift_" + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";

// -------------------------------------------------------------------

// ----  global geometry parameters  ---------------------------------
  FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  
// -------   Load media from media file   ----------------------------
  TString medFile = gPath + "/geometry/media.geo";
  geoFace->setMediaFile(medFile);
  geoFace->readMedia();
  FairGeoMedia*   geoMedia = geoFace->getMedia();
  FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();
  geoMedia->list();
  geoMedia->print();
  DefineRequiredMedia(geoMedia, geoBuild);
// --------------------------------------------------------------------

// --------------   Create geometry and global top volume  ------------
  gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoManager->SetName(geoDetectorName + "_geom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  top->SetMedium(pMedAir);
  gGeoManager->SetTopVolume(top);
  //gGeoMan->SetTopVisible(1);
// --------------------------------------------------------------------

// Define ScWAll Geometry
  TString ScWall_common_name = "ScWAll_common";
  TGeoVolume *ScWall_common = new TGeoVolumeAssembly(ScWall_common_name);
  ScWall_common->SetMedium(pMedAir);
  //ScWall_common->SetMedium(pMedVacuum);

  TString scwall_name = "scwall01";//only Scint
  TGeoBBox *ScWallS = new TGeoBBox(scwall_name, ScWall_X_size, ScWall_Y_size, ScWall_Z_size);
  TGeoVolume *ScWallV = new TGeoVolume(scwall_name, ScWallS);
  ScWallV->SetMedium(pMedAir);
  
  TString scwallCell_name_large = "scwall01large"; //full cells
  
  TGeoBBox *scwallCellS_large = new TGeoBBox(scwallCell_name_large, ScWall_full_cell_X_size_large, ScWall_full_cell_Y_size_large, ScWall_full_cell_Z_size_large);
  TGeoVolume *scwallCellV_large = new TGeoVolume(scwallCell_name_large, scwallCellS_large);
  
  scwallCellV_large->SetMedium(pMedAir);
  
  
  TString scwallCell_name_small = "scwall01small"; //full cells
  
  TGeoBBox *scwallCellS_small = new TGeoBBox(scwallCell_name_small, ScWall_full_cell_X_size_small, ScWall_full_cell_Y_size_small, ScWall_full_cell_Z_size_small);
  TGeoVolume *scwallCellV_small = new TGeoVolume(scwallCell_name_small, scwallCellS_small);
  
  scwallCellV_small->SetMedium(pMedAir);

    
//If Hodo is in use cells 17,18 and 27,28 should be excluded
//Fill ScWall with small cells (1-40)
  //Float_t xCur = ScWall_X_size_small + ScWall_full_cell_X_size_small;
  Float_t xCur = ScWall_X_size_small - ScWall_full_cell_X_size_small;
  Float_t yCur = ScWall_Y_size_small + ScWall_full_cell_Y_size_small;
  Float_t zCur = 0;
  //Float_t xCur1=0;
  
  Int_t iMod=-1, modNb=-1;
  
  for(Int_t iy=0; iy<ScWall_NRows_small; iy++) {
    yCur=yCur - 2.*ScWall_full_cell_Y_size_small;
    for(Int_t ix=0; ix<ScWall_NColumns_small; ix++) {
      xCur=xCur - 2.*ScWall_full_cell_X_size_small;
      //xCur1 = xCur + 2.*ScWall_full_cell_X_size_small;
      //xCur1 = xCur;
      
      TGeoTranslation *scwallCellV_position_small = new TGeoTranslation(xCur,yCur, zCur);
      iMod=iy*ScWall_NColumns_small + ix;
      modNb = iMod+1;

      if((iMod+1)!=17 && (iMod+1)!=18 && (iMod+1)!=27 && (iMod+1)!=28) { 
	ScWallV->AddNode(scwallCellV_small,modNb,scwallCellV_position_small);
	cout <<"ScWall small modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;
      }    
 
      if(ix==ScWall_NColumns_small-1) {
	xCur=ScWall_X_size_small - ScWall_full_cell_X_size_small;
      }
    }//for(Int_t ix==0; ix<7; ix++)
  }//for(Int_t iy==0; iy<7; iy++)
  
//Fill ScWall with large cells (41-174)
  xCur = ScWall_X_size_large + ScWall_full_cell_X_size_large;
  yCur = ScWall_Y_size_large + ScWall_full_cell_Y_size_large;
  zCur = 0;
  
  iMod=-1; modNb=-1;
  for(Int_t iy=0; iy<ScWall_NRows_large; iy++) {
    yCur=yCur - 2.*ScWall_full_cell_Y_size_large;
    for(Int_t ix=0; ix<ScWall_NColumns_large; ix++) {
      xCur=xCur - 2.*ScWall_full_cell_X_size_large;
      
      TGeoTranslation *scwallCellV_position_large = new TGeoTranslation(xCur,yCur, zCur);
      iMod=iy*ScWall_NColumns_large + ix;
      
      //if(iMod<60) modNb=iMod+1+40;  
      //if(iMod>=60 && iMod<=64) continue;
      //if(iMod>64) modNb=iMod-5+1+40;
      if(iMod<61) modNb=iMod+1+40;  
      if(iMod>=61 && iMod<=65) continue;
      if(iMod>65) modNb=iMod-5+1+40;
      
      //if(iMod>=78 && iMod<=82) continue;
      //if(iMod>82) modNb=iMod-10+1+40;     
      if(iMod>=79 && iMod<=83) continue;
      if(iMod>83) modNb=iMod-10+1+40;
      
      ScWallV->AddNode(scwallCellV_large,modNb,scwallCellV_position_large);
      cout <<"ScWall modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;
      
      if(ix==ScWall_NColumns_large-1) xCur=ScWall_X_size_large + ScWall_full_cell_X_size_large;
	
    }//for(Int_t ix==0; ix<7; ix++)
  }//for(Int_t iy==0; iy<7; iy++)
            
//Fill ScWall box 
  TString scwall_name_box = "scwall01_box";//Scint+box
  TGeoBBox *ScWallS_box = new TGeoBBox(scwall_name_box, ScWall_X_size_box, ScWall_Y_size_box, ScWall_Z_size_box);
  TGeoVolume *ScWallV_box = new TGeoVolume(scwall_name_box, ScWallS_box);
  ScWallV_box->SetMedium(pMedAir);
  /*
  TString scwall_name_Al_plate = "scwall01_Al_plate";
  TGeoBBox *ScWallS_Al_plate = new TGeoBBox(scwall_name_Al_plate, ScWall_X_size_box, ScWall_Y_size_box, ScWall_Z_size_Al_plate);
  TGeoVolume *ScWallV_Al_plate = new TGeoVolume(scwall_name_Al_plate, ScWallS_Al_plate);
  ScWallV_Al_plate->SetMedium(pMedAlum);
  */
  TString scwall_name_Al_plate_left = "scwall01_Al_plate_left";
  TGeoBBox *ScWallS_Al_plate_left = new TGeoBBox(scwall_name_Al_plate_left, ScWall_X_size_box_left, ScWall_Y_size_box_left, ScWall_Z_size_Al_plate);
  TGeoVolume *ScWallV_Al_plate_left = new TGeoVolume(scwall_name_Al_plate_left, ScWallS_Al_plate_left);
  ScWallV_Al_plate_left->SetMedium(pMedAlum);

  TString scwall_name_Al_plate_right = "scwall01_Al_plate_right";
  TGeoBBox *ScWallS_Al_plate_right = new TGeoBBox(scwall_name_Al_plate_right, ScWall_X_size_box_right, ScWall_Y_size_box_right, ScWall_Z_size_Al_plate);
  TGeoVolume *ScWallV_Al_plate_right = new TGeoVolume(scwall_name_Al_plate_right, ScWallS_Al_plate_right);
  ScWallV_Al_plate_right->SetMedium(pMedAlum);

  TString scwall_name_Al_plate_top = "scwall01_Al_plate_top";
  TGeoBBox *ScWallS_Al_plate_top = new TGeoBBox(scwall_name_Al_plate_top, ScWall_X_size_box_top, ScWall_Y_size_box_top, ScWall_Z_size_Al_plate);
  TGeoVolume *ScWallV_Al_plate_top = new TGeoVolume(scwall_name_Al_plate_top, ScWallS_Al_plate_top);
  ScWallV_Al_plate_top->SetMedium(pMedAlum);

  TString scwall_name_Air_plate = "scwall01_Air_plate";
  TGeoBBox *ScWallS_Air_plate = new TGeoBBox(scwall_name_Air_plate, ScWall_X_size_box, ScWall_Y_size_box, ScWall_Z_size_Air_plate);
  TGeoVolume *ScWallV_Air_plate = new TGeoVolume(scwall_name_Air_plate, ScWallS_Air_plate);
  ScWallV_Air_plate->SetMedium(pMedAir);
  /*
  zCur = -ScWall_Z_size_box+ScWall_Z_size_Al_plate; //Al left;
  TGeoTranslation *scwallBox = new TGeoTranslation(0.,0.,zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate,1,scwallBox);
  cout <<"ScWall box Al_left " <<zCur <<endl;
  
  scwallBox = new TGeoTranslation(0.,0.,-zCur);//Al right
  ScWallV_box->AddNode(ScWallV_Al_plate,2,scwallBox);
  cout <<"ScWall box Al_right " <<-zCur <<endl;
  */
  zCur = -ScWall_Z_size_box+ScWall_Z_size_Al_plate; //Al before;
  xCur = 65;//left
  yCur = 0;
  TGeoTranslation *scwallBox_left = new TGeoTranslation(xCur,yCur,zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_left,1,scwallBox_left);
  cout <<"ScWall box Al_left_before_Sc " <<xCur <<" " <<yCur <<" " <<zCur <<endl;

  scwallBox_left = new TGeoTranslation(xCur,yCur,-zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_left,2,scwallBox_left);
  cout <<"ScWall box Al_left_after_Sc " <<xCur <<" " <<yCur <<" " <<-zCur <<endl;


  xCur = -87.5;//right
  yCur = 0;
  TGeoTranslation *scwallBox_right = new TGeoTranslation(xCur,yCur,zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_right,1,scwallBox_right);
  cout <<"ScWall box Al_right_before Sc " <<xCur <<" " <<yCur <<" " <<zCur <<endl;

  scwallBox_right = new TGeoTranslation(xCur,yCur,-zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_right,2,scwallBox_right);
  cout <<"ScWall box Al_right_after_Sc " <<xCur <<" " <<yCur <<" " <<-zCur <<endl;

  xCur = -22.5;//top
  yCur = 38.75;
  TGeoTranslation *scwallBox_top = new TGeoTranslation(xCur,yCur,zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_top,1,scwallBox_top);
  cout <<"ScWall box Al_top_before_Sc " <<xCur <<" " <<yCur <<" " <<zCur <<endl;

  scwallBox_top = new TGeoTranslation(xCur,yCur,-zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_top,2,scwallBox_top);
  cout <<"ScWall box Al_top_after_Sc " <<xCur <<" " <<yCur <<" " <<-zCur <<endl;

  xCur = -22.5;//bottom
  yCur = -38.75;
  TGeoTranslation *scwallBox_bottom = new TGeoTranslation(xCur,yCur,zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_top,3,scwallBox_bottom);
  cout <<"ScWall box Al_bottom_before_Sc " <<xCur <<" " <<yCur <<" " <<zCur <<endl;

  scwallBox_bottom = new TGeoTranslation(xCur,yCur,-zCur);
  ScWallV_box->AddNode(ScWallV_Al_plate_top,4,scwallBox_bottom);
  cout <<"ScWall box Al_bottom_after_Sc " <<xCur <<" " <<yCur <<" " <<-zCur <<endl;

  zCur = ScWall_Z_size_box-2.*ScWall_Z_size_Al_plate-ScWall_Z_size_Air_plate;//Air
  TGeoTranslation *scwallBox = new TGeoTranslation(0.,0.,zCur);
  ScWallV_box->AddNode(ScWallV_Air_plate,1,scwallBox);
  cout <<"ScWall box Air " <<zCur <<endl;
  
  zCur = -ScWall_Z_size_box+2.*ScWall_Z_size_Al_plate+ScWall_Z_size;//Scint
  scwallBox = new TGeoTranslation(0.,0.,zCur);
  ScWallV_box->AddNode(ScWallV,1,scwallBox);
  cout <<"ScWall box Scint " <<zCur <<endl;
  //return;

  FillCell_small(scwallCellV_small);
  FillCell_large(scwallCellV_large);

//Adding the ScWall box mother volume to the global TOP Volume
  TGeoRotation *rot = new TGeoRotation("rot");
  rot->RotateY(rotY);
  TGeoCombiTrans *scwallScWall = new TGeoCombiTrans("scwallScWall",ScWall_center_Xpos, ScWall_Ypos, ScWall_center_Zpos,rot);
  //ScWall_common->AddNode(ScWallV, 1, scwallScWall);
  ScWall_common->AddNode(ScWallV_box, 1, scwallScWall);
  
  top->AddNode(ScWall_common, 0);
  top->SetVisContainers(kTRUE);

// ---------------   Finish   ---------------------------------------
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
  
}//end macro


void FillCell_small(TGeoVolume *mother_volume) {
  TString scwallSensitiveCell_name_small_cutted = "scwall_sens_small_cutted"; 
  TString scwallSensitiveCell_name_small_trap = "scwall_sens_small_trap";  

//Cell small
  TGeoShape *scwallSensitiveCellS_small_cutted = new TGeoBBox(scwallSensitiveCell_name_small_cutted, ScWall_full_cell_X_size_small, ScWall_full_cell_Y_size_small-trd1_side_Y, ScWall_full_cell_Z_size_small);
  TGeoVolume *scwallSensitiveCellV_small_cutted = new TGeoVolume(scwallSensitiveCell_name_small_cutted, scwallSensitiveCellS_small_cutted);
  TGeoMedium *Sc_medium_cell_small = pMedScint; //set medium
  if(Sc_medium_cell_small) scwallSensitiveCellV_small_cutted->SetMedium(Sc_medium_cell_small);
  else Fatal("Main", "Invalid medium for Sc_medium_cell_small");
  
  mother_volume->AddNode(scwallSensitiveCellV_small_cutted,1,new TGeoTranslation(0,0,0));
  
//Trap small
  TGeoShape *scwallSensitiveCellS_small_trap = new TGeoTrd1(scwallSensitiveCell_name_small_trap, ScWall_full_cell_X_size_small-trd1_side_X, ScWall_full_cell_X_size_small, trd1_thickness, trd1_size_Z);
  TGeoVolume *scwallSensitiveCellV_small_trap = new TGeoVolume(scwallSensitiveCell_name_small_trap, scwallSensitiveCellS_small_trap);
  TGeoMedium *Sc_medium_small_trap = pMedScint; //set medium
  if(Sc_medium_small_trap) scwallSensitiveCellV_small_trap->SetMedium(Sc_medium_small_trap);
  else Fatal("Main", "Invalid medium for Sc_medium_small_trap");
  
  TGeoTranslation *scwallSensitiveCellV_position_small_trap = new TGeoTranslation(0,ScWall_full_cell_Y_size_small-trd1_side_Y+trd1_size_Z,0); 
  
  TGeoRotation *rot1 = new TGeoRotation("rot1"); //bottom trap
  rot1->RotateX(-90); //in deg.
  
  TGeoCombiTrans *scwallScWall_1 = new TGeoCombiTrans("scwallScWall_1",0,-(ScWall_full_cell_Y_size_small-trd1_side_Y+trd1_size_Z),0,rot1);
  mother_volume->AddNode(scwallSensitiveCellV_small_trap,1,scwallScWall_1);
  
  TGeoRotation *rot2 = new TGeoRotation("rot2"); //top trap
  rot2->RotateX(90); //in deg.
  
  TGeoCombiTrans *scwallScWall_2 = new TGeoCombiTrans("scwallScWall_2",0,(ScWall_full_cell_Y_size_small-trd1_side_Y+trd1_size_Z),0,rot2);
  mother_volume->AddNode(scwallSensitiveCellV_small_trap,1,scwallScWall_2);
    
}//FillCell_small


void FillCell_large(TGeoVolume *mother_volume) {
  TString scwallSensitiveCell_name_large_cutted = "scwall_sens_large_cutted"; 
  TString scwallSensitiveCell_name_large_trap = "scwall_sens_large_trap";  
  
//Cell large
  TGeoShape *scwallSensitiveCellS_large_cutted = new TGeoBBox(scwallSensitiveCell_name_large_cutted, ScWall_full_cell_X_size_large, ScWall_full_cell_Y_size_large-trd1_side_Y, ScWall_full_cell_Z_size_large);
  TGeoVolume *scwallSensitiveCellV_large_cutted = new TGeoVolume(scwallSensitiveCell_name_large_cutted, scwallSensitiveCellS_large_cutted);
  TGeoMedium *Sc_medium_cell_large = pMedScint; //set medium
  if(Sc_medium_cell_large) scwallSensitiveCellV_large_cutted->SetMedium(Sc_medium_cell_large);
  else Fatal("Main", "Invalid medium for Sc_medium_cell_large");
  
  mother_volume->AddNode(scwallSensitiveCellV_large_cutted,1,new TGeoTranslation(0,0,0));

//Trap large
  TGeoShape *scwallSensitiveCellS_large_trap = new TGeoTrd1(scwallSensitiveCell_name_large_trap, ScWall_full_cell_X_size_large-trd1_side_X, ScWall_full_cell_X_size_large, trd1_thickness, trd1_size_Z);
  TGeoVolume *scwallSensitiveCellV_large_trap = new TGeoVolume(scwallSensitiveCell_name_large_trap, scwallSensitiveCellS_large_trap);
  TGeoMedium *Sc_medium_large_trap = pMedScint; //set medium
  if(Sc_medium_large_trap) scwallSensitiveCellV_large_trap->SetMedium(Sc_medium_large_trap);
  else Fatal("Main", "Invalid medium for Sc_medium_large_trap");
  
  TGeoTranslation *scwallSensitiveCellV_position_large_trap = new TGeoTranslation(0,ScWall_full_cell_Y_size_large-trd1_side_Y+trd1_size_Z,0); 
  
  TGeoRotation *rot1 = new TGeoRotation("rot1"); //bottom trap
  rot1->RotateX(-90); //in deg.
  
  TGeoCombiTrans *scwallScWall_1 = new TGeoCombiTrans("scwallScWall_1",0,-(ScWall_full_cell_Y_size_large-trd1_side_Y+trd1_size_Z),0,rot1);
  mother_volume->AddNode(scwallSensitiveCellV_large_trap,1,scwallScWall_1);
  
  TGeoRotation *rot2 = new TGeoRotation("rot2"); //top trap
  rot2->RotateX(90); //in deg.
  
  TGeoCombiTrans *scwallScWall_2 = new TGeoCombiTrans("scwallScWall_2",0,(ScWall_full_cell_Y_size_large-trd1_side_Y+trd1_size_Z),0,rot2);
  mother_volume->AddNode(scwallSensitiveCellV_large_trap,1,scwallScWall_2);
  
}//FillCell_large

