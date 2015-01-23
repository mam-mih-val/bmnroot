//---------------------------

TGeoManager* gGeoMan = NULL;


//Detector's position
const Double_t TOF1_Xpos = 0.0;
const Double_t TOF1_Ypos = 0.0;
const Double_t TOF1_Zpos = 440.0; //cm

//Detector's construct parameters   
const Double_t XWidthOfModule_TOF1 = 100.0;
const Double_t YHeightOfModule_TOF1 = 200.0;
const Double_t ZLengthOfModule_TOF1 = 20.0;  
const Double_t GapSize_TOF1 = 100.0;

const Double_t XSizeOfActiveVolume_TOF1 = XWidthOfModule_TOF1;
const Double_t YSizeOfActiveVolume_TOF1 = YHeightOfModule_TOF1;
const Double_t ZSizeOfActiveVolume_TOF1 = ZLengthOfModule_TOF1;

void create_rootgeom_TOF1() {
    
    // Load the necessary FairRoot libraries 
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
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
    const TString geoDetectorName = "tof1";
    const TString geoDetectorVersion = "v1";
    geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
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
    TGeoVolume* TOF1Top = new TGeoVolumeAssembly(geoDetectorName);
    TOF1Top->SetMedium(pMedAir);
   
    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", TOF1_Xpos, TOF1_Ypos, TOF1_Zpos);
    //DetPos_trans->RegisterYourself();
    TGeoTranslation *LeftModulePosTOF1_trans = new TGeoTranslation("LeftModulePosTOF1_trans", GapSize_TOF1, 0.0, 0.0);
    //LeftModulePosTOF1_trans->RegisterYourself();
    TGeoTranslation *RightModulePosTOF1_trans = new TGeoTranslation("RightModulePosTOF1_trans", -GapSize_TOF1, 0.0, 0.0);
    //RightModulePosTOF1_trans->RegisterYourself();
      
    //Solids (shapes)   
    TGeoBBox *TOF1ContainerS = new TGeoBBox("TOF1ContainerS", ((XWidthOfModule_TOF1*2)+GapSize_TOF1)/2, YHeightOfModule_TOF1/2, ZLengthOfModule_TOF1/2);   
    TGeoBBox *TOF1ModuleS = new TGeoBBox("TOF1ModuleS", XWidthOfModule_TOF1/2, YHeightOfModule_TOF1/2, ZLengthOfModule_TOF1/2);   
    TGeoBBox *TOF1ActiveVolumeS = new TGeoBBox("TOF1ActiveVolumeS", XSizeOfActiveVolume_TOF1/2, YSizeOfActiveVolume_TOF1/2, ZSizeOfActiveVolume_TOF1/2);
    
    //Composite solids (shapes)
     
    //Volumes
    TGeoVolume *TOF1ContainerV = new TGeoVolume("TOF1ContainerV", TOF1ContainerS);
    TOF1ContainerV->SetMedium(pMedAir);
    //TOF1ContainerV->SetVisibility(kFALSE);
    
    TGeoVolume *TOF1ModuleV = new TGeoVolume("TOF1ModuleV", TOF1ModuleS);
    TOF1ModuleV->SetMedium(pMedAir);
    TOF1ModuleV->SetLineColor(kBlue);
    //TOF1ModuleV->SetTransparency(80);
    
    TGeoVolume *TOF1ActiveVolumeV = new TGeoVolume("TOF1ActiveVolumeV", TOF1ActiveVolumeS);
    TOF1ActiveVolumeV->SetMedium(pMedArCO27030);
    TOF1ActiveVolumeV->SetLineColor(kGreen);
      
    //Adding volumes to the TOP Volume
    top->AddNode(TOF1Top, 1, DetPos_trans);
    TOF1Top->AddNode(TOF1ContainerV, 1);
    
    TOF1ContainerV->AddNode(TOF1ModuleV, 1, LeftModulePosTOF1_trans);
    TOF1ContainerV->AddNode(TOF1ModuleV, 2, RightModulePosTOF1_trans);
    
    TOF1ModuleV->AddNode(TOF1ActiveVolumeV, 1);
 
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