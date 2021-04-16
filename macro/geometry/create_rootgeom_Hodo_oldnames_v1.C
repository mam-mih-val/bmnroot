//---------------------------
//M.Golubeva (INR RAS), January 2021  
//---------------------------

//create_rootgeom_Hodo_oldnames_v1(Double_t Hodo_Xpos=0,Double_t Hodo_Ypos=0,Double_t Hodo_Zpos=900,Double_t rotY=0)
//Hodo_Xpos - X coordinate of front surface center [cm]
//Hodo_Ypos - Y coordinate of front surface center [cm]
//Hodo_Zpos - Z coordinate of front surface center [cm]
//rotY - rotation around Y axis [deg.]

//Example of root file creation  
//root -l "create_rootgeom_Hodo_oldnames_v1.C(0,0,900,0)"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMedium.h"
#include "TFile.h"

#include "../run/bmnloadlibs.C"


//Detector's construct parameters ===================

//Global dimensions for Hodo in cm
Double_t Hodo_X_size = 8; //16./2.
Double_t Hodo_Y_size = 8; //16./2.
Double_t Hodo_Z_size = 0.2; //half of 0.4 cm

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

}//DefineRequiredMedia

void FillStick(TGeoVolume* hodoStickV);

void create_rootgeom_Hodo_oldnames_v1(Double_t Hodo_Xpos=0,Double_t Hodo_Ypos=0,Double_t Hodo_Zpos=900,Double_t rotY=0) {

  bmnloadlibs(); // load libraries

// ----  set working directory  --------------------------------------------
  TString gPath = gSystem->Getenv("VMCWORKDIR");

// -------   Geometry file name (output)   ---------------------------------
  const TString geoDetectorName = "Hodo_oldnames";
  const TString geoDetectorVersion = "v1";

  Double_t Hodo_center_Zpos = Hodo_Zpos + Hodo_Z_size*TMath::Cos(rotY*TMath::DegToRad());//(900+0.2);//cm
  Double_t Hodo_center_Xpos = Hodo_Xpos + Hodo_Z_size*TMath::Sin(rotY*TMath::DegToRad());//cm

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

  TString hodo_name = "hodo01";
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

  FillStick(hodoStickV);

//Adding the Hodo mother volume to the global TOP Volume
  TGeoRotation *rot = new TGeoRotation("rot");
  rot->RotateY(rotY);
  TGeoCombiTrans *hodoHodo = new TGeoCombiTrans("hodoHodo",Hodo_center_Xpos, Hodo_Ypos, Hodo_center_Zpos,rot);
  Hodo_common->AddNode(HodoV, 1, hodoHodo);
  
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

