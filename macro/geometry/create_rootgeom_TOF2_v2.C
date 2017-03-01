//---------------------------

#define TOF2_MAX_CHAMBERS 24
#define TOF2_MAX_STRIPS_IN_CHAMBER 32
#define TOF2_MAX_GAS_LAYERS 12

TGeoManager* gGeoMan = NULL;
Double_t halfxwidth[TOF2_MAX_CHAMBERS];
Double_t halfywidth[TOF2_MAX_CHAMBERS];
Double_t xoffs, yoffs, zoffs;
Int_t nstrips[TOF2_MAX_CHAMBERS];
Int_t chtype[TOF2_MAX_CHAMBERS];
Double_t zchamb[TOF2_MAX_CHAMBERS];
Double_t xcens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
Double_t ycens[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
Double_t xmins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
Double_t xmaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
Double_t ymins[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];
Double_t ymaxs[TOF2_MAX_CHAMBERS][TOF2_MAX_STRIPS_IN_CHAMBER];

//Number of chambers
Int_t TOF2_Nmodules = 0;
Int_t TOF2_Ngaslayers = TOF2_MAX_GAS_LAYERS;

//Detector's construct parameters   
Double_t GasLayerThickness_TOF2 = 0.025;  
Double_t GlassLayerThickness_TOF2 = 0.085;  
Double_t XWidthOfModule_TOF2[2] = {0.};
Double_t YWidthOfModule_TOF2[2] = {0.};
Double_t ZWidthOfModule_TOF2 = TOF2_Ngaslayers*GasLayerThickness_TOF2 + \
                                     (TOF2_Ngaslayers+1)*GlassLayerThickness_TOF2;

Char_t TOF2_Module_Names[TOF2_MAX_CHAMBERS][8];

Double_t TOF2_X_center[TOF2_MAX_CHAMBERS] = {0.};
Double_t TOF2_Y_center[TOF2_MAX_CHAMBERS] = {0.};
Double_t TOF2_Z_center[TOF2_MAX_CHAMBERS] = {0.};

void create_rootgeom_TOF2_v2() {
    
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
    // -------   Geometry text file name (input)   ----------------------------------
    TOF2_Nmodules = readGeom("TOF700_geometry_v2.txt");
    // -------   Geometry file name (output)   ----------------------------------
    TString geoDetectorName = "tof2";
    TString geoDetectorVersion = "v2";
    geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------  
    // Modules Y centers
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	TOF2_X_center[i] = xcens[i][nstrips[i]-1];
	TOF2_Y_center[i] = (ycens[i][nstrips[i]-1] + ycens[i][0]);
	TOF2_Z_center[i] = zchamb[i];
	XWidthOfModule_TOF2[chtype[i]] = xmaxs[i][0] - xmins[i][0];
	YWidthOfModule_TOF2[chtype[i]] = ymaxs[i][nstrips[i]-1] - ymins[i][0];
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
    Double_t TOF2_X_min = xmin - XWidthOfModule_TOF2[1]/2.;
    Double_t TOF2_Y_min = ymin - YWidthOfModule_TOF2[1]/2.;
    Double_t TOF2_Z_min = zmin - ZWidthOfModule_TOF2[1]/2.;
    Double_t TOF2_X_max = xmax + XWidthOfModule_TOF2[1]/2.;
    Double_t TOF2_Y_max = ymax + YWidthOfModule_TOF2[1]/2.;
    Double_t TOF2_Z_max = zmax + ZWidthOfModule_TOF2[1]/2.;
    
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
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", xoffs, yoffs, zoffs);
    //DetPos_trans->RegisterYourself();
    TGeoTranslation *ModulePosTOF2_trans[TOF2_MAX_CHAMBERS] = {NULL};
    TGeoTranslation *LayerPosTOF2_trans[TOF2_MAX_GAS_LAYERS] = {NULL};
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
    TGeoVolume *TOF2ContainerV = new TGeoVolume("TOF2ContainerV", TOF2ContainerS);
    TOF2ContainerV->SetMedium(pMedAir);
    TOF2ContainerV->SetVisibility(kFALSE);
    top->AddNode(TOF2Top, 1, DetPos_trans);
//    TOF2Top->AddNode(TOF2ContainerV, 1, ContainerPos_trans);
    TOF2Top->AddNode(TOF2ContainerV, 1);

    char nTOF2ModuleS[2][32] = {"TOF2ModuleS0","TOF2ModuleS1"};
    char nTOF2ActiveVolumeS[2][32] = {"TOF2ActiveVolumeS0","TOF2ActiveVolumeS1"};
    char nTOF2ActiveGasVolumeS[2][32] = {"TOF2ActiveGasVolumeS0","TOF2ActiveGasVolumeS1"};
    char nTOF2ModuleV[2][32] = {"TOF2ModuleV0","TOF2ModuleV1"};
    char nTOF2ActiveVolumeV[2][32] = {"TOF2ActiveVolumeV0","TOF2ActiveVolumeV1"};
    char nTOF2ActiveGasVolumeV[2][32] = {"TOF2ActiveGasVolumeV0","TOF2ActiveGasVolumeV1"};
    TGeoBBox *TOF2ModuleS[2];
    TGeoBBox *TOF2ActiveVolumeS[2];
    TGeoBBox *TOF2ActiveGasVolumeS[2];
    TGeoVolume *TOF2ModuleV[2];
    TGeoVolume *TOF2ActiveVolumeV[2];
    TGeoVolume *TOF2ActiveGasVolumeV[2];

    for (int i = 0; i<2; i++)
    {
	TOF2ModuleS[i] = new TGeoBBox(nTOF2ModuleS[i], XWidthOfModule_TOF2[i]/2, YWidthOfModule_TOF2[i]/2, ZWidthOfModule_TOF2/2);   
	TOF2ActiveVolumeS[i] = new TGeoBBox(nTOF2ActiveVolumeS[i], XWidthOfModule_TOF2[i]/2, YWidthOfModule_TOF2[i]/2, ZWidthOfModule_TOF2/2);
	TOF2ActiveGasVolumeS[i] = new TGeoBBox(nTOF2ActiveGasVolumeS[i], XWidthOfModule_TOF2[i]/2, YWidthOfModule_TOF2[i]/2, GasLayerThickness_TOF2/2);
    
	//Composite solids (shapes)
     
	//Volumes
    
	TOF2ModuleV[i] = new TGeoVolume(nTOF2ModuleV[i], TOF2ModuleS[i]);
	TOF2ModuleV[i]->SetMedium(pMedAir);
	TOF2ModuleV[i]->SetLineColor(kBlue);
	//TOF2ModuleV[i]->SetTransparency(80);
    
	TOF2ActiveVolumeV[i] = new TGeoVolume(nTOF2ActiveVolumeV[i], TOF2ActiveVolumeS[i]);
	TOF2ActiveVolumeV[i]->SetMedium(pMedRPCglass);
	TOF2ActiveVolumeV[i]->SetLineColor(kGreen);
    
	TOF2ActiveGasVolumeV[i] = new TGeoVolume(nTOF2ActiveGasVolumeV[i], TOF2ActiveGasVolumeS[i]);
	TOF2ActiveGasVolumeV[i]->SetMedium(pMedRPCgas);
	TOF2ActiveGasVolumeV[i]->SetLineColor(kYellow);

	TOF2ModuleV[i]->AddNode(TOF2ActiveVolumeV[i], 1);

	for (int j = 0; j < TOF2_Ngaslayers; j++)
	{
	    TOF2ActiveVolumeV[i]->AddNode(TOF2ActiveGasVolumeV[i], j+1, LayerPosTOF2_trans[j]);
	}
    }
      
    //Adding volumes to the TOP Volume
    Int_t nm[2] = {0};
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	TOF2ContainerV->AddNode(TOF2ModuleV[chtype[i]], nm[chtype[i]]+1, ModulePosTOF2_trans[i]);
	nm[chtype[i]]++;
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
int readGeom(char *geomfile)
{
	char fname[128];
	FILE *fg = 0;
	char ic[16] = {""};
	int n = 0;
	Double_t step, sx, sy, x, y, z;
	if (strlen(geomfile) == 0)
	{
	    printf("TOF700 geometry file name not defined!\n");
	    return 0;
	}
	TString dir = getenv("VMCWORKDIR");
	sprintf(fname,"%s/geometry/%s",dir.Data(),geomfile);
	fg = fopen(fname,"r");
	if (fg == NULL)
	{
	    printf("TOF700 geometry file %s open error!\n", fname);
	    return 0;
	}
	if (fscanf(fg,"%f %f %f\n", &xoffs, &yoffs, &zoffs) != 3)
	{
	    printf("Wrong first line in TOF700 geometry file %s\n", fname);
	    return 0;
	};
	for (int i=0; i<TOF2_MAX_CHAMBERS; i++) nstrips[i] = 0;
	int c = 0;
	while(fscanf(fg,"%s %d %lf %lf %lf %lf %lf %lf\n", &ic[0], &n, &step, &sy, &sx, &x, &y, &z) == 8)
	{
		strcpy(TOF2_Module_Names[c], ic);
		halfxwidth[c] = sx/20.;
		halfywidth[c] = sy/20.;
		zchamb[c] = z/10.;// + zoffs;
		nstrips[c] = n;
		if (n == 32) chtype[c] = 0;
		else	     chtype[c] = 1;
		for (int ns=n-1; ns>=0; ns--)
		{
		xcens[c][ns] = -x/10. + xoffs;
		ycens[c][ns] = y/10. + yoffs - (n-ns-1)*step/10.;
		xmins[c][ns] = xcens[c][ns] - halfxwidth[c];
		xmaxs[c][ns] = xcens[c][ns] + halfxwidth[c];
		ymins[c][ns] = ycens[c][ns] - halfywidth[c];
		ymaxs[c][ns] = ycens[c][ns] + halfywidth[c];
//		printf("C %d S %d %f %f %f %f %f\n",ic,n,zchamb[c],xmins[c][ns],xmaxs[c][ns],ymins[c][ns],ymaxs[c][ns]);
		}
		printf("%s ns=%d step=%f sx=%f sy=%f x=%f y=%f z=%f\n",ic,n,step,sx,sy,x,y,z);
		c++;
		if (c >= TOF2_MAX_CHAMBERS) break;
	}
	fclose(fg);
	return c;
}
