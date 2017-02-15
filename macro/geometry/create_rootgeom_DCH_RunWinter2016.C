//---------------------------
//VERY PRELIMINARY! NO INNER STRUCTURE!!!
//

TGeoManager* gGeoMan = NULL;

//Detector's position

const Double_t DCH1_Xpos = 0.0;
const Double_t DCH2_Xpos = 0.0;
const Double_t DCH1_Ypos = 0.0;
const Double_t DCH2_Ypos = 0.0;
const Double_t DCH1_Zpos = 514.0;
const Double_t DCH2_Zpos = 713.0;

//Detector's construct parameters
const Double_t ZLength_DCH1 = 26.5; // 20.0
const Double_t ZLength_DCH1ActiveVolume = 0.6;
const Double_t HoleSize_DCH1 = 12.0;

const Double_t SideLengthOfOctagon = 120.0;
const Double_t OuterRadiusOfOctagon = (2.414 * SideLengthOfOctagon) / 2.0;
const Double_t InnerRadiusOfOctagon = 12.0;

void create_rootgeom_DCH_RunWinter2016() {

    // Load the necessary FairRoot libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "DCH";
    const TString geoDetectorVersion = "RunWinter2016";
    geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia* geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if (!mAir) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if (!pMedAir) Fatal("Main", "Medium air not found");

    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if (!mArCO27030) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("arco27030");
    if (!pMedArCO27030) Fatal("Main", "Medium arco27030 not found");

    // --------------------------------------------------------------------------

    // --------------   Create geometry and top volume  -------------------------
    gGeoMan = (TGeoManager*) gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // --------------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* DCH1Top = new TGeoVolumeAssembly(geoDetectorName);
    DCH1Top->SetMedium(pMedAir);
    TGeoVolume* DCH2Top = new TGeoVolumeAssembly(geoDetectorName);
    DCH2Top->SetMedium(pMedAir);

    //Transformations (translations, rotations and scales)
    //TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", DCH1_Xpos, DCH1_Ypos, DCH1_Zpos);

    TGeoRotation *rot_Octagon = new TGeoRotation("rot_Octagon");
    rot_Octagon->SetAngles(90, -22.5, 90, 67.5, 0, 0);

    //Solids (shapes)
    TGeoPgon *OctagonS = new TGeoPgon("OctagonS", 0, 360, 8, 2);
    OctagonS->DefineSection(0, -ZLength_DCH1 / 2, InnerRadiusOfOctagon, OuterRadiusOfOctagon);
    OctagonS->DefineSection(1, ZLength_DCH1 / 2, InnerRadiusOfOctagon, OuterRadiusOfOctagon);

    TGeoCombiTrans *combi1 = new TGeoCombiTrans(DCH1_Xpos, DCH1_Ypos, DCH1_Zpos, rot_Octagon);
    TGeoCombiTrans *combi2 = new TGeoCombiTrans(DCH2_Xpos, DCH2_Ypos, DCH2_Zpos, rot_Octagon);

    TGeoVolume *DCH1DetV = new TGeoVolume("DCH1DetV", OctagonS);
    DCH1DetV->SetMedium(pMedAir);
    DCH1DetV->SetLineColor(kBlue);
    
    TGeoVolume *DCH2DetV = new TGeoVolume("DCH2DetV", OctagonS);
    DCH2DetV->SetMedium(pMedAir);
    DCH2DetV->SetLineColor(kBlue);

    //Adding volumes to the TOP Volume
    top->AddNode(DCH1Top, 1);
    top->AddNode(DCH2Top, 1);
    DCH1Top->AddNode(DCH1DetV, 1, combi1);
    DCH2Top->AddNode(DCH2DetV, 1, combi2);
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
