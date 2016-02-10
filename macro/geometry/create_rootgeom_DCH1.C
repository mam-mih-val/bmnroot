//---------------------------

TGeoManager* gGeoMan = NULL;


//Detector's position
const Double_t tanpipeangle = 0.3/5.7 ; // tangent of pipe angle
//const Double_t DCH1_Xpos = 0.0;
const Double_t DCH1_Ypos = 0.0;
const Double_t DCH1_Zpos = 550.0; //cm
const Double_t DCH1_Xpos = DCH1_Zpos*tanpipeangle;

//Detector's construct parameters   
//const Double_t XYRadius_DCH1 = 144.84; 
const Double_t ZLength_DCH1 = 20.0;      
const Double_t ZLength_DCH1ActiveVolume = 0.6;      
const Double_t HoleSize_DCH1 = 12.0;

const Double_t SideLengthOfOctagon = 120.0;
const Double_t InnerRadiusOfOctagon = (2.414*SideLengthOfOctagon)/2.0;

const Double_t MaxRadiusOfActiveVolume = 120.0;
const Double_t MinRadiusOfActiveVolume = 12.0;

void create_rootgeom_DCH1() {
    
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
    const TString geoDetectorName = "dch1";
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
    TGeoVolume* DCH1Top = new TGeoVolumeAssembly(geoDetectorName);
    DCH1Top->SetMedium(pMedAir);
   
    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", DCH1_Xpos, DCH1_Ypos, DCH1_Zpos);
    //SetPos_trans->RegisterYourself();
    
    TGeoRotation *rot_Octagon =  new TGeoRotation("rot_Octagon");
    //rot_RB->SetAngles(90, -45, 90, 45, 0, 0);
    rot_Octagon->SetAngles(90, -22.5, 90, 67.5, 0, 0);
    rot_Octagon->RegisterYourself();
    
    //Solids (shapes)
    TGeoPgon *OctagonS = new TGeoPgon("OctagonS", 0, 360, 8, 2);
       OctagonS->DefineSection(0, -ZLength_DCH1/2, 0.0, InnerRadiusOfOctagon);
       OctagonS->DefineSection(1, ZLength_DCH1/2, 0.0, InnerRadiusOfOctagon);
       
    //TGeoBBox *DCH1ContainerS = new TGeoBBox("DCH1ContainerS", InnerRadiusOfOctagon + 0.01, InnerRadiusOfOctagon + 0.01, ZLength_DCH1/2 + 0.01);   
    TGeoBBox *DCH1ContainerS = new TGeoBBox("DCH1ContainerS", InnerRadiusOfOctagon, InnerRadiusOfOctagon, ZLength_DCH1/2);   
    //TGeoTube *DCH1ActiveVolumeS = new TGeoTube(MinRadiusOfActiveVolume, MaxRadiusOfActiveVolume, ZLength_DCH1/2 - 0.01); 
    TGeoTube *DCH1ActiveVolumeS = new TGeoTube(MinRadiusOfActiveVolume, MaxRadiusOfActiveVolume, ZLength_DCH1ActiveVolume/2); 
    //TGeoBBox *DCH1HoleS = new TGeoBBox("DCH1HoleS", HoleSize_DCH1/2, HoleSize_DCH1/2, ZLength_DCH1/2 + 0.1);
    TGeoBBox *DCH1HoleS = new TGeoBBox("DCH1HoleS", HoleSize_DCH1/2, HoleSize_DCH1/2, ZLength_DCH1/2);
      
    //Composite solids (shapes)
    TGeoCompositeShape *DCH1DetS = new TGeoCompositeShape("DCH1DetS", "OctagonS:rot_Octagon-DCH1HoleS");
    
    //Volumes
    TGeoVolume *DCH1ContainerV = new TGeoVolume("DCH1ContainerV", DCH1ContainerS);
    DCH1ContainerV->SetMedium(pMedAir);
    //DCH1ContainerV->SetVisibility(kFALSE);
    
    TGeoVolume *DCH1DetV = new TGeoVolume("DCH1DetV", DCH1DetS);
    DCH1DetV->SetMedium(pMedAir);
    DCH1DetV->SetLineColor(kBlue);
    
    TGeoVolume *DCH1ActiveVolumeV = new TGeoVolume("DCH1ActiveVolumeV", DCH1ActiveVolumeS);
    DCH1ActiveVolumeV->SetMedium(pMedArCO27030);
    DCH1ActiveVolumeV->SetLineColor(kGreen);
    
    //Adding volumes to the TOP Volume
    top->AddNode(DCH1Top, 1, DetPos_trans);
    DCH1Top->AddNode(DCH1ContainerV, 1);
    DCH1ContainerV->AddNode(DCH1DetV, 1);
    //DCH1DetV->AddNode(DCH1ActiveVolumeV, 1);
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 1, new TGeoTranslation(0.,0.,-7.8));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 2, new TGeoTranslation(0.,0.,-6.6));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 3, new TGeoTranslation(0.,0.,-3.0));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 4, new TGeoTranslation(0.,0.,-1.8));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 5, new TGeoTranslation(0.,0.,1.8));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 6, new TGeoTranslation(0.,0.,3.0));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 7, new TGeoTranslation(0.,0.,6.6));
    DCH1DetV->AddNode(DCH1ActiveVolumeV, 8, new TGeoTranslation(0.,0.,7.8));
    
    top->SetVisContainers(kTRUE);

    
    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    //top->Draw("ogl");
    top->Draw("");
}
