//---------------------------
//M.Golubeva (INR RAS), January 2021  
//---------------------------

//create_rootgeom_Hodo_oldnames_v1(Double_t Hodo_Xpos=0,Double_t Hodo_Ypos=0,Double_t Hodo_Zpos=900,Double_t rotY=0)
//Hodo_Xpos - X coordinate of front surface center [cm]
//Hodo_Ypos - Y coordinate of front surface center [cm]
//Hodo_Zpos - Z coordinate of front surface center [cm]
//rotY - rotation around Y axis [deg.]

//Example of root file creation  
//root -l "create_rootgeom_Hodo_oldnames_with_boxv1.C(0,0,900,0)"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../run/bmnloadlibs.C"


//Detector's construct parameters ===================

//Global dimensions for Hodo in cm (only Hodo scint)
Double_t Hodo_X_size = 8; //16./2.
Double_t Hodo_Y_size = 8; //16./2.
Double_t Hodo_Z_size = 0.2; //half of 0.4 cm

Double_t Hodo_X_size_box = 9.7; //19.4/2.
Double_t Hodo_Y_size_box = 10; //20./2.
Double_t Hodo_Z_size_box = 2.25; //half of 4.5 cm (4.4+2.*0.05)

//Al plates
Double_t Hodo_X_size_Al_plate = 0.15; //half of 0.3 cm
Double_t Hodo_Y_size_Al_plate = 9.7; //half of 19.4 cm
Double_t Hodo_Z_size_Al_plate = 2.2; //half of 4.4 cm

Double_t Hodo_X_size_Al_plate_horiz = 9.7; //half of 19.4 cm
Double_t Hodo_Y_size_Al_plate_horiz = 0.15; //half of 0.3 cm
Double_t Hodo_Z_size_Al_plate_horiz = 2.2; //half of 4.4 cm

//Fe plates
Double_t Hodo_thickness_Fe_plate = 0.025; //half of 0.05 cm

//HODO sticks
Double_t Hodo_NColumns = 16; 
Double_t Hodo_NRows = 1; 
Double_t Hodo_stick_X_size = 0.5; //1./2. 
Double_t Hodo_stick_Y_size = 8; //16./2. 
Double_t Hodo_stick_Z_size = 0.2; //0.4/2. 

//media ============================================
TGeoMedium *pMedVacuum = 0;
TGeoMedium *pMedAir = 0;
TGeoMedium *pMedScint = 0;
TGeoMedium *pMedAlum = 0;
TGeoMedium *pMedIron = 0;

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

// medium aluminium (Hodo box)
  FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
  if ( ! mAluminium ) Fatal("Main", "FairMedium aluminium not found");
  geoBuild->createMedium(mAluminium);
  pMedAlum = gGeoManager->GetMedium("aluminium");
  if ( ! pMedAlum ) Fatal("Main", "Medium aluminium not found");

// medium iron (Hodo box)
  FairGeoMedium* mIron = geoMedia->getMedium("iron");
  if ( ! mIron ) Fatal("Main", "FairMedium iron not found");
  geoBuild->createMedium(mIron);
  pMedIron = gGeoManager->GetMedium("iron");
  if ( ! pMedIron ) Fatal("Main", "Medium iron not found");

}//DefineRequiredMedia

void FillStick(TGeoVolume* hodoStickV);

void create_rootgeom_Hodo_oldnames_with_box_v1(Double_t Hodo_Xpos=0,Double_t Hodo_Ypos=0,Double_t Hodo_Zpos=900,Double_t rotY=0) {

  bmnloadlibs(); // load libraries

// ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

// -------   Geometry file name (output)   ---------------------------------
  const TString geoDetectorName = "Hodo_oldnames_with_box";
  const TString geoDetectorVersion = "v1";

  Double_t Hodo_center_Zpos = Hodo_Zpos + Hodo_Z_size_box*TMath::Cos(rotY*TMath::DegToRad());//(900+0.2);//cm
  Double_t Hodo_center_Xpos = Hodo_Xpos + Hodo_Z_size_box*TMath::Sin(rotY*TMath::DegToRad());//cm

  TString sXshift_1 = "";
  TString sXshift_2 = "";

  TString sYshift_1 = "";
  TString sYshift_2 = "";

  TString sZshift_1 = "";
  TString sZshift_2 = "";

  TString sRotY_1 = "";
  TString sRotY_2 = "";

  Int_t Zshift_1 = Int_t(Hodo_Zpos); //cm
  sZshift_1 += Zshift_1;
  Int_t Zshift_2 = Int_t(10*(Hodo_Zpos-Int_t(Hodo_Zpos))); //cm
  sZshift_2 += Zshift_2;

  Int_t Xshift_1 = Int_t(Hodo_Xpos); //cm
  sXshift_1 += Xshift_1;
  Int_t Xshift_2 = Int_t(0.5+100*(Hodo_Xpos-Int_t(Hodo_Xpos))); //cm
  sXshift_2 += Xshift_2;

  Int_t Yshift_1 = Int_t(Hodo_Ypos); //cm
  sYshift_1 += Yshift_1;
  Int_t Yshift_2 = Int_t(100*(Hodo_Ypos-Int_t(Hodo_Ypos))); //cm
  sYshift_2 += Yshift_2;

  Int_t rotY_1 = Int_t(rotY);
  sRotY_1 +=rotY_1;
  Int_t rotY_2 = Int_t(100*(rotY-Int_t(rotY)));
  sRotY_2 +=rotY_2;

  TString geoFileName = gPath + "/geometry/" + geoDetectorName + "_" + "Zpos_" + sZshift_1 + "." + sZshift_2 + "cm" + "_" + "Xshift_" + sXshift_1 + "." + sXshift_2 + "cm" + "_" + "Yshift_" + sYshift_1 + "." + sYshift_2 + "cm" + "_" + "rotationY_"  + sRotY_1  +  "." + sRotY_2 + "deg_" + geoDetectorVersion + ".root";

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

// --------------   Create geometry and global top volume  ------------
  gGeoManager = (TGeoManager*)gROOT->FindObject("FAIRGeom");
  gGeoManager->SetName(geoDetectorName + "_geom");
  TGeoVolume* top = new TGeoVolumeAssembly("TOP");
  top->SetMedium(pMedAir);
  gGeoManager->SetTopVolume(top);
  //gGeoMan->SetTopVisible(1);

// Define Hodo Geometry
  TString Hodo_common_name = "Hodo_common";
  TGeoVolume *Hodo_common = new TGeoVolumeAssembly(Hodo_common_name);
  Hodo_common->SetMedium(pMedAir);
  //Hodo_common->SetMedium(pMedVacuum);

  TString hodo_name = "hodo01";//only Hodo
  TGeoBBox *HodoS = new TGeoBBox(hodo_name, Hodo_X_size, Hodo_Y_size, Hodo_Z_size);
  TGeoVolume *HodoV = new TGeoVolume(hodo_name, HodoS);
  HodoV->SetMedium(pMedAir);
  
  TString hodoStick_name = "hodo01stick"; 
  
  TGeoBBox *hodoStickS = new TGeoBBox(hodoStick_name, Hodo_stick_X_size, Hodo_stick_Y_size, Hodo_stick_Z_size);
  TGeoVolume *hodoStickV = new TGeoVolume(hodoStick_name, hodoStickS);
  
  hodoStickV->SetMedium(pMedAir);
      
//Fill Hodo with sticks (1-16)
  Float_t xCur = Hodo_X_size + Hodo_stick_X_size;
  Float_t yCur = 0.;
  Float_t zCur = 0;
  
  Int_t iMod=-1, modNb=-1;
  
  for(Int_t iy=0; iy<Hodo_NRows; iy++) {
    for(Int_t ix=0; ix<Hodo_NColumns; ix++) {
      xCur=xCur - 2.*Hodo_stick_X_size;
      
      TGeoTranslation *hodoStickV_position = new TGeoTranslation(xCur,yCur, zCur);
      iMod=iy*Hodo_NColumns + ix;
      modNb = iMod+1;

      HodoV->AddNode(hodoStickV,modNb,hodoStickV_position);
      cout <<"Hodo modNb,xxxx,yyyy " <<modNb <<" " <<xCur <<" " <<yCur <<endl;

    }//for(Int_t ix==0; ix<7; ix++)
  }//for(Int_t iy==0; iy<7; iy++)

//Fill Hodo box 
  TString hodo_name_box = "hodo01_box";//Hodo+box
  TGeoBBox *HodoS_box = new TGeoBBox(hodo_name_box, Hodo_X_size_box, Hodo_Y_size_box, Hodo_Z_size_box);
  TGeoVolume *HodoV_box = new TGeoVolume(hodo_name_box, HodoS_box);
  HodoV_box->SetMedium(pMedAir);

//Al plates
  TString hodo_name_Al_plate = "hodo01_Al_plate";
  TGeoBBox *HodoS_Al_plate = new TGeoBBox(hodo_name_Al_plate, Hodo_X_size_Al_plate, Hodo_Y_size_Al_plate, Hodo_Z_size_Al_plate);
  TGeoVolume *HodoV_Al_plate = new TGeoVolume(hodo_name_Al_plate, HodoS_Al_plate);
  HodoV_Al_plate->SetMedium(pMedAlum);

  xCur = -Hodo_X_size_box+Hodo_X_size_Al_plate;//Al left
  TGeoTranslation *hodoBox = new TGeoTranslation(xCur,0.,0.);
  HodoV_box->AddNode(HodoV_Al_plate,1,hodoBox);
  cout <<"Hodo box Al_left " <<xCur <<endl;
  
  hodoBox = new TGeoTranslation(-xCur,0.,0.);//Al right
  HodoV_box->AddNode(HodoV_Al_plate,2,hodoBox);
  cout <<"Hodo box Al_right " <<-xCur <<endl;
 
  TString hodo_name_Al_plate_horiz = "hodo01_Al_plate_horiz";
  TGeoBBox *HodoS_Al_plate_horiz = new TGeoBBox(hodo_name_Al_plate_horiz, Hodo_X_size_Al_plate_horiz, Hodo_Y_size_Al_plate_horiz, Hodo_Z_size_Al_plate_horiz);
  TGeoVolume *HodoV_Al_plate_horiz = new TGeoVolume(hodo_name_Al_plate_horiz, HodoS_Al_plate_horiz);
  HodoV_Al_plate_horiz->SetMedium(pMedAlum);

  yCur = -Hodo_Y_size_box+Hodo_Y_size_Al_plate_horiz;//Al bottom 
  hodoBox = new TGeoTranslation(0.,yCur,0.);
  HodoV_box->AddNode(HodoV_Al_plate_horiz,1,hodoBox);//bottom
  cout <<"Hodo box Al_bottom " <<xCur <<endl;

  hodoBox = new TGeoTranslation(0.,-yCur,0.);
  HodoV_box->AddNode(HodoV_Al_plate_horiz,2,hodoBox);//top
  cout <<"Hodo box Al_top " <<xCur <<endl;

//Fe plates  
  TString hodo_name_Fe_plate = "hodo01_Fe_plate";
  TGeoBBox *HodoS_Fe_plate = new TGeoBBox(hodo_name_Fe_plate, Hodo_X_size_box, Hodo_Y_size_box, Hodo_thickness_Fe_plate);
  TGeoVolume *HodoV_Fe_plate = new TGeoVolume(hodo_name_Fe_plate, HodoS_Fe_plate);
  HodoV_Fe_plate->SetMedium(pMedIron);
  
  zCur = -Hodo_Z_size_box+Hodo_thickness_Fe_plate; //Fe upstream 
  hodoBox = new TGeoTranslation(0.,0.,zCur);
  HodoV_box->AddNode(HodoV_Fe_plate,1,hodoBox);
  cout <<"Hodo box Fe_downstream " <<xCur <<endl;

  hodoBox = new TGeoTranslation(0.,0.,-zCur);//Fe downstream 
  HodoV_box->AddNode(HodoV_Fe_plate,2,hodoBox);
  cout <<"Hodo box Fe_upstream " <<xCur <<endl;

//Scint. sticks
  hodoBox = new TGeoTranslation(0.,0.,0.);
  HodoV_box->AddNode(HodoV,1,hodoBox);
  cout <<"Hodo box Scint " <<xCur <<endl;


  /*  
  //with rot 
 xCur = 0;
  yCur = -Hodo_Y_size_box+Hodo_X_size_Al_plate;
  zCur = 0;

  //TGeoTranslation *scwallSensitiveCellV_position_small_trap = new TGeoTranslation(0,ScWall_full_cell_Y_size_small-trd1_side_Y+trd1_size_Z,0); 
  hodoBox = new TGeoTranslation(0,yCur,0); 
  
  TGeoRotation *rot1 = new TGeoRotation("rot1"); //bottom Al
  rot1->RotateX(-90); //in deg.
  
  //TGeoCombiTrans *scwallScWall_1 = new TGeoCombiTrans("scwallScWall_1",0,-(ScWall_full_cell_Y_size_small-trd1_side_Y+trd1_size_Z),0,rot1);
  TGeoCombiTrans *hodoCombi_1 = new TGeoCombiTrans("hodoCombi_1",0,yCur,0,rot1);
  HodoV_box->AddNode(HodoV_Al_plate,3,hodoCombi_1);
  cout <<"Hodo box Al_bottom " <<yCur <<endl;

  TGeoRotation *rot2 = new TGeoRotation("rot2"); //top Al
  rot2->RotateX(90); //in deg.
  TGeoCombiTrans *hodoCombi_2 = new TGeoCombiTrans("hodoCombi_2",0,-yCur,0,rot2);
  HodoV_box->AddNode(HodoV_Al_plate,4,hodoCombi_2);
  cout <<"Hodo box Al_top " <<-yCur <<endl;
  
  //return;
  */

  FillStick(hodoStickV);

//Adding the Hodo box mother volume to the global TOP Volume
  TGeoRotation *rot = new TGeoRotation("rot");
  rot->RotateY(rotY);
  TGeoCombiTrans *hodoHodo = new TGeoCombiTrans("hodoHodo",Hodo_center_Xpos, Hodo_Ypos, Hodo_center_Zpos,rot);
  //Hodo_common->AddNode(HodoV, 1, hodoHodo);
  Hodo_common->AddNode(HodoV_box, 1, hodoHodo);
  
  top->AddNode(Hodo_common, 0);
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


void FillStick(TGeoVolume *mother_volume) {
  TString hodoSensitiveStick_name = "hodo_stick_sens"; 

//Stick
  TGeoShape *hodoSensitiveStickS = new TGeoBBox(hodoSensitiveStick_name, Hodo_stick_X_size, Hodo_stick_Y_size, Hodo_stick_Z_size);
  TGeoVolume *hodoSensitiveStickV = new TGeoVolume(hodoSensitiveStick_name, hodoSensitiveStickS);
  TGeoMedium *Sc_medium_stick = pMedScint; //set medium
  if(Sc_medium_stick) hodoSensitiveStickV->SetMedium(Sc_medium_stick);
  else Fatal("Main", "Invalid medium for Sc_medium_stick");
  
  mother_volume->AddNode(hodoSensitiveStickV,1,new TGeoTranslation(0,0,0));

}//FillStick()

