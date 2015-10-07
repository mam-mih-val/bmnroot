//---------------------------

TGeoManager* gGeoMan = NULL;


//Detector's position
const Double_t tanpipeangle = 0.3/5.7 ; // tangent of pipe angle
const Double_t DCH2_Xpos = -7.8; //alignment
const Double_t DCH2_Ypos = -0.7; //alignment
const Double_t DCH2_Zpos = 748.35; //cm
//const Double_t DCH2_Xpos = DCH2_Zpos*tanpipeangle;

//Detector's construct parameters
//const Double_t XYRadius_DCH2 = 144.84;
const Double_t ZLength_DCH2 = 26.5; // 20.0
const Double_t ZLength_DCH2ActiveVolume = 0.6;
const Double_t HoleSize_DCH2 = 12.0;

const Double_t SideLengthOfOctagon = 120.0;
const Double_t InnerRadiusOfOctagon = (2.414*SideLengthOfOctagon)/2.0;

const Double_t MaxRadiusOfActiveVolume = 120.0;
const Double_t MinRadiusOfActiveVolume = 12.0;

void create_rootgeom_DCH2_run2() {

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
    const TString geoDetectorName = "dch2";
    const TString geoDetectorVersion = "v1_run2";
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
    TGeoVolume* DCH2Top = new TGeoVolumeAssembly(geoDetectorName);
    DCH2Top->SetMedium(pMedAir);

    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", DCH2_Xpos, DCH2_Ypos, DCH2_Zpos);
    //SetPos_trans->RegisterYourself();

    TGeoRotation *rot_Octagon =  new TGeoRotation("rot_Octagon");
    //rot_RB->SetAngles(90, -45, 90, 45, 0, 0);
    rot_Octagon->SetAngles(90, -22.5, 90, 67.5, 0, 0);
    rot_Octagon->RegisterYourself();

    //Solids (shapes)
    TGeoPgon *OctagonS = new TGeoPgon("OctagonS", 0, 360, 8, 2);
       OctagonS->DefineSection(0, -ZLength_DCH2/2, 0.0, InnerRadiusOfOctagon);
       OctagonS->DefineSection(1, ZLength_DCH2/2, 0.0, InnerRadiusOfOctagon);

    //TGeoBBox *DCH2ContainerS = new TGeoBBox("DCH2ContainerS", InnerRadiusOfOctagon + 0.01, InnerRadiusOfOctagon + 0.01, ZLength_DCH2/2 + 0.01);
    TGeoBBox *DCH2ContainerS = new TGeoBBox("DCH2ContainerS", InnerRadiusOfOctagon, InnerRadiusOfOctagon, ZLength_DCH2/2);
    //TGeoTube *DCH2ActiveVolumeS = new TGeoTube(MinRadiusOfActiveVolume, MaxRadiusOfActiveVolume, ZLength_DCH2/2 - 0.01);
    TGeoTube *DCH2ActiveVolumeS = new TGeoTube(MinRadiusOfActiveVolume, MaxRadiusOfActiveVolume, ZLength_DCH2ActiveVolume/2);
    //TGeoBBox *DCH2HoleS = new TGeoBBox("DCH2HoleS", HoleSize_DCH2/2, HoleSize_DCH2/2, ZLength_DCH2/2 + 0.1);
    TGeoBBox *DCH2HoleS = new TGeoBBox("DCH2HoleS", HoleSize_DCH2/2, HoleSize_DCH2/2, ZLength_DCH2/2);

    //Composite solids (shapes)
    TGeoCompositeShape *DCH2DetS = new TGeoCompositeShape("DCH2DetS", "OctagonS:rot_Octagon-DCH2HoleS");

    //Volumes
    TGeoVolume *DCH2ContainerV = new TGeoVolume("DCH2ContainerV", DCH2ContainerS);
    DCH2ContainerV->SetMedium(pMedAir);
    //DCH2ContainerV->SetVisibility(kFALSE);

    TGeoVolume *DCH2DetV = new TGeoVolume("DCH2DetV", DCH2DetS);
    DCH2DetV->SetMedium(pMedAir);
    DCH2DetV->SetLineColor(kBlue);

    TGeoVolume *DCH2ActiveVolumeV = new TGeoVolume("DCH2ActiveVolumeV", DCH2ActiveVolumeS);
    DCH2ActiveVolumeV->SetMedium(pMedArCO27030);
    DCH2ActiveVolumeV->SetLineColor(kGreen);

    //Adding volumes to the TOP Volume
    top->AddNode(DCH2Top, 1, DetPos_trans);
    DCH2Top->AddNode(DCH2ContainerV, 1);
    DCH2ContainerV->AddNode(DCH2DetV, 1);
    //DCH2DetV->AddNode(DCH2ActiveVolumeV, 1);
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 1, new TGeoTranslation(0.,0.,-7.8));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 2, new TGeoTranslation(0.,0.,-6.6));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 3, new TGeoTranslation(0.,0.,-3.0));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 4, new TGeoTranslation(0.,0.,-1.8));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 5, new TGeoTranslation(0.,0.,1.8));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 6, new TGeoTranslation(0.,0.,3.0));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 7, new TGeoTranslation(0.,0.,6.6));
    DCH2DetV->AddNode(DCH2ActiveVolumeV, 8, new TGeoTranslation(0.,0.,7.8));

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
