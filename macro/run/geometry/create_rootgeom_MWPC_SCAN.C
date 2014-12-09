#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"

using namespace TMath;

TGeoManager* gGeoMan = NULL;

//Number of active planes with wires
const Int_t NofPlanes = 20;

//Detector's position
const Double_t MWPC1_Xpos = 0.0;
const Double_t MWPC1_Ypos = 0.0;
const Double_t MWPC1_Zpos = 0.0;

//Detector's construct parameters
const Double_t GapZsize = 1.0; //cm
const Double_t GapWireOnePlane = 0.25; //cm
const Double_t GapWireTwoPlanes = GapWireOnePlane / 2.0; //cm // 0.125

const Double_t XSizeOfActiveVolume = 450;
const Double_t YSizeOfActiveVolume = 100;
const Double_t ZSizeOfActiveVolume = 250;

void create_rootgeom_MWPC_SCAN() {
    
    // Load the necessary FairRoot libraries 
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries
     
    // -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/";
    TString medFile = geoPath + "media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "mwpc_scan";
    const TString geoDetectorVersion = "v1";
    TString geoFileName = geoPath + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------  

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");
  
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030 ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("arco27030");
    if ( ! pMedArCO27030 ) Fatal("Main", "Medium arco27030 not found");
    
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
    TGeoVolume* MWPC1Top = new TGeoVolumeAssembly(geoDetectorName);
    MWPC1Top->SetMedium(pMedAir);
   
    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", MWPC1_Xpos, MWPC1_Ypos, MWPC1_Zpos);

    //Solids (shapes)
    
//    TGeoBBox *MWPC1ContainerS = new TGeoBBox("MWPC1ContainerS", XSizeOfActiveVolume / 2.0 + 1.0, YSizeOfActiveVolume / 2.0 + 1.0, 0.005 + 0.01);
    TGeoBBox *MWPC1ActivePlaneS = new TGeoBBox("MWPC1ActivePlaneS", XSizeOfActiveVolume / 2.0, YSizeOfActiveVolume / 2.0, 0.005);

    //Volumes
//    TGeoVolume *MWPC1ContainerV = new TGeoVolume("MWPC1ContainerV", MWPC1ContainerS);
//    MWPC1ContainerV->SetMedium(pMedAir);
//    MWPC1ContainerV->SetVisibility(kTRUE);
    
    TGeoVolume *MWPC1ActivePlaneV = new TGeoVolume("MWPC1ActivePlaneV", MWPC1ActivePlaneS);
    MWPC1ActivePlaneV->SetMedium(pMedArCO27030);
    MWPC1ActivePlaneV->SetLineColor(kBlue);
      
    //Adding volumes to the TOP Volume
    top->AddNode(MWPC1Top, 1, DetPos_trans);
//    MWPC1Top->AddNode(MWPC1ContainerV, 1);
    const Float_t a = 10.0; //cm
    const Float_t b = 300.0; //cm
    for (Int_t iPlane = 1; iPlane <= NofPlanes; ++iPlane) {
        TGeoTranslation t1(0.0, 0.0, (b - a) / NofPlanes * (iPlane - 1));
        TGeoRotation r1("r1", 0.0, 0.0, 0.0);
        MWPC1Top->AddNode(MWPC1ActivePlaneV, iPlane, new TGeoCombiTrans(t1, r1));
//        MWPC1ContainerV->AddNode(MWPC1ActivePlaneV, iPlane, new TGeoCombiTrans(t1, r1));
    }
 
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
}