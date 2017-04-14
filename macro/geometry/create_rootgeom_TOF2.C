//---------------------------

TGeoManager* gGeoMan = NULL;


//TOF2 detector position
const Double_t TOF2_Xpos = 0.0;
const Double_t TOF2_Ypos = 0.0;
const Double_t TOF2_Zpos = 600.0; //cm

//Number of chambers
const Int_t TOF2_Nmodules = 15;
const Int_t TOF2_Nstrips = 32;
const Int_t TOF2_StripPitch = 1.1;
const Int_t TOF2_Ngaslayers = 12;

//Detector's construct parameters   
const Double_t GasLayerThickness_TOF2 = 0.025;  
const Double_t GlassLayerThickness_TOF2 = 0.085;  
const Double_t XWidthOfModule_TOF2 = 16.0;
const Double_t YWidthOfModule_TOF2 = 35.1;
const Double_t ZWidthOfModule_TOF2 = TOF2_Ngaslayers*GasLayerThickness_TOF2 + \
                                     (TOF2_Ngaslayers+1)*GlassLayerThickness_TOF2;

//const Double_t TOF2_X_1st_strip[15] = {\
//                       -81.4,-65.3,-51.6,-35.2,-21.8,\
//                       -81.6,-67.5,-51.8,-37.3,-22.0,\
//                       -81.4,-65.3,-51.6,-35.2,-21.8};

const Double_t TOF2_X_1st_strip[15] = {\
                       81.4,65.3,51.6,35.2,21.8,\
                       81.6,67.5,51.8,37.3,22.0,\
                       81.4,65.3,51.6,35.2,21.8};

const Double_t TOF2_Y_1st_strip[15] = {\
                        50.2, 50.2, 50.2, 50.2, 50.2,\
                        16.2, 16.2, 16.2, 16.2, 16.2,\
                       -15.9,-15.9,-15.9,-15.9,-15.9};

const Double_t TOF2_Z_1st_strip[15] = {\
                          .6,  9.4,   .6,  9.4,   .6,\
                        30.6, 39.4, 30.6, 39.4, 30.6,\
                          .6,  9.4,   .6,  9.4,   .6};

const Char_t TOF2_Module_Names[15][8] = {\
                        { "2.1"},{ "3.1"}, {"4.1"}, {"5.1"}, {"6.1"},\
                        {"16.2"},{"17.2"},{"18.2"},{"19.2"},{"20.2"},\
                        {"28.1"},{"29.1"},{"30.1"},{"31.1"},{"32.1"}};

Double_t TOF2_X_center[15] = {0.};
Double_t TOF2_Y_center[15] = {0.};
Double_t TOF2_Z_center[15] = {0.};

const Double_t XSizeOfActiveVolume_TOF2 = XWidthOfModule_TOF2;
const Double_t YSizeOfActiveVolume_TOF2 = YWidthOfModule_TOF2;
const Double_t ZSizeOfActiveVolume_TOF2 = ZWidthOfModule_TOF2;

void create_rootgeom_TOF2() {
    
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
    const TString geoDetectorName = "tof2";
    const TString geoDetectorVersion = "v1";
    geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------  
    // Modules Y centers
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	TOF2_X_center[i] = TOF2_X_1st_strip[i];
	TOF2_Y_center[i] = TOF2_Y_1st_strip[i] - TOF2_StripPitch*((Double_t)TOF2_Nstrips/2.-0.5);
	TOF2_Z_center[i] = TOF2_Z_1st_strip[i];
    }
    // Container size calculations
    Double_t xmin =  10000., ymin =  10000., zmin =  10000.;
    Double_t xmax = -10000., ymax = -10000., zmax = -10000.;
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	if (TOF2_X_center[i] < xmin) xmin = TOF2_X_center[i];
	if (TOF2_Y_center[i] < ymin) ymin = TOF2_Y_center[i];
	if (TOF2_Z_center[i] < zmin) zmin = TOF2_Z_center[i];
	if (TOF2_X_center[i] > xmax) xmax = TOF2_X_center[i];
	if (TOF2_Y_center[i] > ymax) ymax = TOF2_Y_center[i];
	if (TOF2_Z_center[i] > zmax) zmax = TOF2_Z_center[i];
    }
    Double_t TOF2_X_min = xmin - XWidthOfModule_TOF2/2.;
    Double_t TOF2_Y_min = ymin - YWidthOfModule_TOF2/2.;
    Double_t TOF2_Z_min = zmin - ZWidthOfModule_TOF2/2.;
    Double_t TOF2_X_max = xmax + XWidthOfModule_TOF2/2.;
    Double_t TOF2_Y_max = ymax + YWidthOfModule_TOF2/2.;
    Double_t TOF2_Z_max = zmax + ZWidthOfModule_TOF2/2.;
    
    Double_t TOF2_XWidth = TMath::Abs(TOF2_X_max) > TMath::Abs(TOF2_X_min)? (2*TMath::Abs(TOF2_X_max)):(2*TMath::Abs(TOF2_X_min));
    Double_t TOF2_YWidth = TMath::Abs(TOF2_Y_max) > TMath::Abs(TOF2_Y_min)? (2*TMath::Abs(TOF2_Y_max)):(2*TMath::Abs(TOF2_Y_min));
    Double_t TOF2_ZWidth = TMath::Abs(TOF2_Z_max) > TMath::Abs(TOF2_Z_min)? (2*TMath::Abs(TOF2_Z_max)):(2*TMath::Abs(TOF2_Z_min));

    Double_t TOF2_X = (TOF2_X_max + TOF2_X_min)/2.;// + TOF2_Xpos;
    Double_t TOF2_Y = (TOF2_Y_max + TOF2_Y_min)/2.;// + TOF2_Ypos;
    Double_t TOF2_Z = (TOF2_Z_max + TOF2_Z_min)/2.;// + TOF2_Zpos;

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");
  
    FairGeoMedium* mRPCglass = geoMedia->getMedium("RPCglass");
    if ( ! mRPCglass ) Fatal("Main", "FairMedium RPCglass not found");
    geoBuild->createMedium(mRPCglass);
    TGeoMedium* pMedRPCglass = gGeoMan->GetMedium("RPCglass");
    if ( ! pMedRPCglass ) Fatal("Main", "Medium RPCglass not found");
  
    FairGeoMedium* mRPCgas = geoMedia->getMedium("RPCgas");
    if ( ! mRPCgas ) Fatal("Main", "FairMedium RPCgas not found");
    geoBuild->createMedium(mRPCgas);
    TGeoMedium* pMedRPCgas = gGeoMan->GetMedium("RPCgas");
    if ( ! pMedRPCgas ) Fatal("Main", "Medium RPCgas not found");
    
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
    TGeoVolume* TOF2Top = new TGeoVolumeAssembly(geoDetectorName);
    TOF2Top->SetMedium(pMedAir);
   
    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", TOF2_Xpos, TOF2_Ypos, TOF2_Zpos);
    //DetPos_trans->RegisterYourself();
    TGeoTranslation *ModulePosTOF2_trans[15] = {NULL};
    TGeoTranslation *LayerPosTOF2_trans[12] = {NULL};
    char name[64];
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	sprintf(name,"Module_%d_PosTOF2_trans", i+1);
	ModulePosTOF2_trans[i] = new TGeoTranslation(name, TOF2_X_center[i], TOF2_Y_center[i], TOF2_Z_center[i]);
//        ModulePosTOF2_trans[i]->RegisterYourself();
    }
    Double_t LayerPos = GlassLayerThickness_TOF2+GasLayerThickness_TOF2/2. - ZWidthOfModule_TOF2/2.;
    for (int i = 0; i < TOF2_Ngaslayers; i++)
    {
//	printf("Layer %d z %f\n",i,LayerPos);
	sprintf(name,"GasLayer_%d_PosTOF2_trans", i+1);
	LayerPosTOF2_trans[i] = new TGeoTranslation(name, 0., 0., LayerPos);
	LayerPos += GlassLayerThickness_TOF2+GasLayerThickness_TOF2;
//        ModulePosTOF2_trans[i]->RegisterYourself();
    }
      
    //Solids (shapes)   
    TGeoBBox *TOF2ContainerS = new TGeoBBox("TOF2ContainerS", TOF2_XWidth/2, TOF2_YWidth/2, TOF2_ZWidth/2);   
    TGeoBBox *TOF2ModuleS = new TGeoBBox("TOF2ModuleS", XWidthOfModule_TOF2/2, YWidthOfModule_TOF2/2, ZWidthOfModule_TOF2/2);   
    TGeoBBox *TOF2ActiveVolumeS = new TGeoBBox("TOF2ActiveVolumeS", XSizeOfActiveVolume_TOF2/2, YSizeOfActiveVolume_TOF2/2, ZSizeOfActiveVolume_TOF2/2);
    TGeoBBox *TOF2ActiveGasVolumeS = new TGeoBBox("TOF2ActiveGasVolumeS", XSizeOfActiveVolume_TOF2/2, YSizeOfActiveVolume_TOF2/2, GasLayerThickness_TOF2/2);
    
    //Composite solids (shapes)
     
    //Volumes
    TGeoVolume *TOF2ContainerV = new TGeoVolume("TOF2ContainerV", TOF2ContainerS);
    TOF2ContainerV->SetMedium(pMedAir);
    TOF2ContainerV->SetVisibility(kFALSE);
    
    TGeoVolume *TOF2ModuleV = new TGeoVolume("TOF2ModuleV", TOF2ModuleS);
    TOF2ModuleV->SetMedium(pMedAir);
    TOF2ModuleV->SetLineColor(kBlue);
    //TOF2ModuleV->SetTransparency(80);
    
    TGeoVolume *TOF2ActiveVolumeV = new TGeoVolume("TOF2ActiveVolumeV", TOF2ActiveVolumeS);
    TOF2ActiveVolumeV->SetMedium(pMedRPCglass);
    TOF2ActiveVolumeV->SetLineColor(kGreen);
    
    TGeoVolume *TOF2ActiveGasVolumeV = new TGeoVolume("TOF2ActiveGasVolumeV", TOF2ActiveGasVolumeS);
    TOF2ActiveGasVolumeV->SetMedium(pMedRPCgas);
    TOF2ActiveGasVolumeV->SetLineColor(kYellow);
      
    //Adding volumes to the TOP Volume
    top->AddNode(TOF2Top, 1, DetPos_trans);
//    TOF2Top->AddNode(TOF2ContainerV, 1, ContainerPos_trans);
    TOF2Top->AddNode(TOF2ContainerV, 1);
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	TOF2ContainerV->AddNode(TOF2ModuleV, i+1, ModulePosTOF2_trans[i]);
    }
    
    TOF2ModuleV->AddNode(TOF2ActiveVolumeV, 1);

    for (int i = 0; i < TOF2_Ngaslayers; i++)
    {
	TOF2ActiveVolumeV->AddNode(TOF2ActiveGasVolumeV, i+1, LayerPosTOF2_trans[i]);
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
