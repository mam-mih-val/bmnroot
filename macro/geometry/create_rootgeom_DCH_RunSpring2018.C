//---------------------------
// Geometry of DCH detector (more detailed)
// Update: D.Baranov
// Date: 07.08.2019
//

TGeoManager* gGeoMan = NULL;

//Detector's position
const Double_t DCH1_Xpos = 0.0;
const Double_t DCH1_Ypos = 0.0;
const Double_t DCH1_Zpos = 514.0;

const Double_t DCH2_Xpos = 0.0;
const Double_t DCH2_Ypos = 0.0;
const Double_t DCH2_Zpos = 713.0;

//Detector's construct parameters
const Double_t ZLengthOfChamber = 23.2; // 4*(0.8+0.62+0.38+0.38+0.44+0.38+0.38+0.62+0.8)+ 2*2.0
const Double_t ActiveVolumeThickness = 0.002; //20 micron (sense wire diameter)
const Double_t CoverFoilThickness = 0.0022; //22 micron (maylar coated with graphite)
const Double_t HoleSize = 16.0;

const Double_t SideLengthOfOctagon = 120.0;
const Double_t OuterRadiusOfOctagon = (2.414 * SideLengthOfOctagon) / 2.0; //incircle radius, where 2.414 is a constant
const Double_t OuterRadiusOfOctagon_ActivePlane = 120.0;
const Double_t InnerRadiusOfOctagon = 16.0;

//Flange parameters
const Double_t InnerRadiusOfFlange = 15.2;
const Double_t OuterRadiusOfFlange = 19.2;
const Double_t ZLengthOfFlange = 7.5;

const Double_t InnerRadiusOfTube = InnerRadiusOfFlange;
const Double_t OuterRadiusOfTube = 15.5;
const Double_t ZLengthOfTube = ZLengthOfChamber;

void create_rootgeom_DCH_RunSpring2018() {

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
    const TString geoDetectorVersion = "RunSpring2018";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_" + geoDetectorVersion + ".root";
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

    //ArgonIsobutane7525
    FairGeoMedium* mArgonIsobutane7525 = geoMedia->getMedium("ArgonIsobutane7525");
    if (!mArgonIsobutane7525) Fatal("Main", "FairMedium ArgonIsobutane7525 not found");
    geoBuild->createMedium(mArgonIsobutane7525);
    TGeoMedium* pMedArgonIsobutane7525 = gGeoMan->GetMedium("ArgonIsobutane7525");
    if (!pMedArgonIsobutane7525) Fatal("Main", "Medium ArgonIsobutane7525 not found");

    //maylar
    FairGeoMedium* mMylar = geoMedia->getMedium("mylar");
    if (!mMylar) Fatal("Main", "FairMedium mMylar not found");
    geoBuild->createMedium(mMylar);
    TGeoMedium* pMedMylar = gGeoMan->GetMedium("mylar");
    if (!pMedMylar) Fatal("Main", "Medium mylar not found");

    //aluminium
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if (!mAluminium) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    TGeoMedium* pMedAluminium = gGeoMan->GetMedium("aluminium");
    if (!pMedAluminium) Fatal("Main", "Medium aluminium not found");
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

    //Chamber ------------------------------------------------------------------
    TGeoPgon *OctagonS = new TGeoPgon("OctagonS", 0, 360, 8, 2);
    OctagonS->DefineSection(0, -ZLengthOfChamber*0.5, InnerRadiusOfOctagon, OuterRadiusOfOctagon);
    OctagonS->DefineSection(1, +ZLengthOfChamber*0.5, InnerRadiusOfOctagon, OuterRadiusOfOctagon);

    TGeoVolume *DCHDetV = new TGeoVolume("DCHDetV", OctagonS);
    DCHDetV->SetMedium(pMedArgonIsobutane7525);
    DCHDetV->SetLineColor(kBlue);
    DCHDetV->SetTransparency(75);
    //--------------------------------------------------------------------------

    //Active plane (sense wires) -----------------------------------------------
    TGeoPgon *OctagonActivePlaneS = new TGeoPgon("OctagonActivePlaneS", 0, 360, 8, 2);
    OctagonActivePlaneS->DefineSection(0, -ActiveVolumeThickness*0.5, InnerRadiusOfOctagon, OuterRadiusOfOctagon_ActivePlane);
    OctagonActivePlaneS->DefineSection(1, +ActiveVolumeThickness*0.5, InnerRadiusOfOctagon, OuterRadiusOfOctagon_ActivePlane);

    TGeoVolume *DCHActivePlaneV = new TGeoVolume("DCHActivePlaneV", OctagonActivePlaneS);
    DCHActivePlaneV->SetMedium(pMedArgonIsobutane7525);
    DCHActivePlaneV->SetLineColor(TColor::GetColor("#cc0000"));
    DCHActivePlaneV->SetTransparency(40);
    //--------------------------------------------------------------------------

    //Cover plane (maylar coated with graphite) --------------------------------
    TGeoPgon *OctagonCoverPlaneS = new TGeoPgon("OctagonCoverPlaneS", 0, 360, 8, 2);
    OctagonCoverPlaneS->DefineSection(0, -CoverFoilThickness*0.5, InnerRadiusOfOctagon, OuterRadiusOfOctagon);
    OctagonCoverPlaneS->DefineSection(1, +CoverFoilThickness*0.5, InnerRadiusOfOctagon, OuterRadiusOfOctagon);

    TGeoVolume *DCHCoverLayerV = new TGeoVolume("DCHCoverLayerV", OctagonCoverPlaneS);
    DCHCoverLayerV->SetMedium(pMedMylar);
    DCHCoverLayerV->SetLineColor(TColor::GetColor("#cccccc"));
    DCHCoverLayerV->SetTransparency(40);
    //--------------------------------------------------------------------------

    //Flange -------------------------------------------------------------------
    TGeoTube *FlangeS = new TGeoTube("FlangeS", InnerRadiusOfFlange, OuterRadiusOfFlange, ZLengthOfFlange*0.5);
    TGeoTube *TubeS = new TGeoTube("TubeS", InnerRadiusOfTube, OuterRadiusOfTube, ZLengthOfTube*0.5);

    TGeoVolume *FlangeV = new TGeoVolume("FlangeV", FlangeS);
    FlangeV->SetMedium(pMedAluminium);
    FlangeV->SetLineColor(TColor::GetColor("#00cc00"));
    FlangeV->SetTransparency(40);

    TGeoVolume *TubeV = new TGeoVolume("TubeV", TubeS);
    TubeV->SetMedium(pMedAluminium);
    TubeV->SetLineColor(TColor::GetColor("#00cc00"));
    TubeV->SetTransparency(40);
    //--------------------------------------------------------------------------

    DCHDetV->AddNode(DCHActivePlaneV, 0, new TGeoTranslation(0.,0.,-7.8));
    DCHDetV->AddNode(DCHActivePlaneV, 1, new TGeoTranslation(0.,0.,-6.6));
    DCHDetV->AddNode(DCHActivePlaneV, 2, new TGeoTranslation(0.,0.,-3.0));
    DCHDetV->AddNode(DCHActivePlaneV, 3, new TGeoTranslation(0.,0.,-1.8));
    DCHDetV->AddNode(DCHActivePlaneV, 4, new TGeoTranslation(0.,0.,1.8));
    DCHDetV->AddNode(DCHActivePlaneV, 5, new TGeoTranslation(0.,0.,3.0));
    DCHDetV->AddNode(DCHActivePlaneV, 6, new TGeoTranslation(0.,0.,6.6));
    DCHDetV->AddNode(DCHActivePlaneV, 7, new TGeoTranslation(0.,0.,7.8));

    DCHDetV->AddNode(DCHCoverLayerV, 0, new TGeoTranslation(0.,0.,-9.6 - CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 1, new TGeoTranslation(0.,0.,-4.8 - CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 2, new TGeoTranslation(0.,0.,-4.8 + CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 3, new TGeoTranslation(0.,0.,0.0 - CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 4, new TGeoTranslation(0.,0.,0.0 + CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 5, new TGeoTranslation(0.,0.,4.8 - CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 6, new TGeoTranslation(0.,0.,4.8 + CoverFoilThickness*0.5));
    DCHDetV->AddNode(DCHCoverLayerV, 7, new TGeoTranslation(0.,0.,9.6 + CoverFoilThickness*0.5));


    //Adding volumes to the DCH Volume
    DCH->AddNode(DCHDetV, 0, combi1);
    DCH->AddNode(FlangeV, 1, new TGeoTranslation(DCH1_Xpos, DCH1_Ypos, -11.6 - ZLengthOfFlange*0.5 + DCH1_Zpos));
    DCH->AddNode(FlangeV, 2, new TGeoTranslation(DCH1_Xpos, DCH1_Ypos, +11.6 + ZLengthOfFlange*0.5 + DCH1_Zpos));
    DCH->AddNode(TubeV, 1, new TGeoTranslation(DCH1_Xpos, DCH1_Ypos, DCH1_Zpos));

    DCH->AddNode(DCHDetV, 1, combi2);
    DCH->AddNode(FlangeV, 3, new TGeoTranslation(DCH2_Xpos, DCH2_Ypos, -11.6 - ZLengthOfFlange*0.5 + DCH2_Zpos));
    DCH->AddNode(FlangeV, 4, new TGeoTranslation(DCH2_Xpos, DCH2_Ypos, +11.6 + ZLengthOfFlange*0.5 + DCH2_Zpos));
    DCH->AddNode(TubeV, 2, new TGeoTranslation(DCH2_Xpos, DCH2_Ypos, DCH2_Zpos));


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
