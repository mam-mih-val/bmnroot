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
    //gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    //bmnloadlibs(); // load libraries

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
    TGeoVolume* DCH = new TGeoVolumeAssembly(geoDetectorName);
    DCH->SetMedium(pMedAir);

    //Transformations (translations, rotations and scales)
    TGeoCombiTrans *combi1 = new TGeoCombiTrans(DCH1_Xpos, DCH1_Ypos, DCH1_Zpos, new TGeoRotation("rot", 45*0.5, 0, 0));
    TGeoCombiTrans *combi2 = new TGeoCombiTrans(DCH2_Xpos, DCH2_Ypos, DCH2_Zpos, new TGeoRotation("rot", 45*0.5, 0, 0));

    TGeoRotation *rot_Octagon = new TGeoRotation("rot_Octagon");
    rot_Octagon->SetAngles(90, -22.5, 90, 67.5, 0, 0);

    //Solids (shapes)
    TGeoPgon *OctagonS = new TGeoPgon("OctagonS", 0, 360, 8, 2);
    OctagonS->DefineSection(0, -ZLength_DCH1 / 2, InnerRadiusOfOctagon, OuterRadiusOfOctagon);
    OctagonS->DefineSection(1, ZLength_DCH1 / 2, InnerRadiusOfOctagon, OuterRadiusOfOctagon);

    TGeoVolume *DCHDetV = new TGeoVolume("DCHDetV", OctagonS);
    DCHDetV->SetMedium(pMedAir);
    DCHDetV->SetLineColor(kBlue);
    DCHDetV->SetTransparency(75);

    TGeoPgon *OctagonPlaneS = new TGeoPgon("OctagonActivePlaneS", 0, 360, 8, 2);
    OctagonPlaneS->DefineSection(0, -0.05, InnerRadiusOfOctagon, OuterRadiusOfOctagon);
    OctagonPlaneS->DefineSection(1, 0.05, InnerRadiusOfOctagon, OuterRadiusOfOctagon);

    TGeoVolume *DCHActivePlaneV = new TGeoVolume("DCHActivePlaneV", OctagonPlaneS);
    DCHActivePlaneV->SetMedium(pMedArCO27030);
    DCHActivePlaneV->SetLineColor(kBlue);
    DCHActivePlaneV->SetTransparency(40);

    for (Int_t iPlane = 1; iPlane <= 8; ++iPlane) {
        TGeoTranslation t0(0.0, 0.0, ZLength_DCH1 / 8 * (iPlane - (8 + 1) / 2.0));
        DCHDetV->AddNode(DCHActivePlaneV, iPlane - 1, new TGeoCombiTrans(t0));
    }

    //Adding volumes to the TOP Volume
    DCH->AddNode(DCHDetV, 0, combi1);
    DCH->AddNode(DCHDetV, 0, combi2);
    top->AddNode(DCH, 0);
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
