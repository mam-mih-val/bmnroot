//---------------------------

#define TOF2_MAX_CHAMBERS 24
#define TOF2_MAX_STRIPS_IN_CHAMBER 32

TGeoManager* gGeoMan = NULL;
TGeoVolume* TOF2Top = NULL;
TGeoMedium* pMedAir = NULL;
TGeoMedium* medAl = NULL;
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

//Detector's construct parameters   
Double_t ChamberThickness_TOF2    = 1.680;  
Double_t AlLayerThickness_TOF2    = 0.150;  
Double_t GasLayerThickness_TOF2   = 0.264;  
Double_t GlassLayerThickness_TOF2 = 0.860/2.;  
Double_t G10LayerThickness_TOF2 = ChamberThickness_TOF2-AlLayerThickness_TOF2-GasLayerThickness_TOF2-2.*GlassLayerThickness_TOF2 + 0.01;  
Double_t XWidthOfModule_TOF2[2] = {0.};
Double_t YWidthOfModule_TOF2[2] = {0.};
Double_t XActWidthOfModule_TOF2[2] = {0.};
Double_t YActWidthOfModule_TOF2[2] = {0.};
Double_t ZWidthOfModule_TOF2 = ChamberThickness_TOF2;

Char_t TOF2_Module_Names[TOF2_MAX_CHAMBERS][8];

Double_t TOF2_X_center[TOF2_MAX_CHAMBERS] = {0.};
Double_t TOF2_Y_center[TOF2_MAX_CHAMBERS] = {0.};
Double_t TOF2_Z_center[TOF2_MAX_CHAMBERS] = {0.};

void create_rootgeom_TOF700_run6_with_support() {
    
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
    TOF2_Nmodules = readGeom("TOF700_geometry_run6.txt");
    // -------   Geometry file name (output)   ----------------------------------
    TString geoDetectorName = "tof700";
    TString geoDetectorVersion = "run6_with_support";
    geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------  
    // Modules Y centers
    Double_t dy = 0.;
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	dy = (ymins[i][nstrips[i]-1] - ymaxs[i][nstrips[i]-2])/2.;
	TOF2_X_center[i] = xcens[i][nstrips[i]-1];
	TOF2_Y_center[i] = (ycens[i][nstrips[i]-1] + ycens[i][0])/2.;
	TOF2_Z_center[i] = zchamb[i];
	printf("Chamber %d  Z = %f\n", i+1, TOF2_Z_center[i]);
	XActWidthOfModule_TOF2[chtype[i]] = xmaxs[i][0] - xmins[i][0];
	YActWidthOfModule_TOF2[chtype[i]] = ymaxs[i][nstrips[i]-1] - ymins[i][0] + dy;
	XWidthOfModule_TOF2[chtype[i]] = xmaxs[i][0] - xmins[i][0] + 3.0;
	YWidthOfModule_TOF2[chtype[i]] = ymaxs[i][nstrips[i]-1] - ymins[i][0] + dy + 3.0;
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

    // aluminium
    FairGeoMedium* matAl = geoMedia->getMedium("aluminium");
    if (!matAl) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(matAl);
    TGeoMedium* medAl = gGeoMan->GetMedium("aluminium");
    if (!medAl) Fatal("Main", "Medium aluminium not found");

    // G10
    FairGeoMedium* matG10 = geoMedia->getMedium("G10");
    if (!matG10) Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(matG10);
    TGeoMedium* medG10 = gGeoMan->GetMedium("G10");
    if (!medG10) Fatal("Main", "Medium G10 not found");
    
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
    TOF2Top = new TGeoVolumeAssembly(geoDetectorName);
    TOF2Top->SetMedium(pMedAir);
   
    //Transformations (translations, rotations and scales)
    //TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", xoffs, yoffs, zoffs);
    //DetPos_trans->RegisterYourself();
    TGeoTranslation *ModulePosTOF2_trans[TOF2_MAX_CHAMBERS] = {NULL};
    TGeoTranslation *LayerPosTOF2_trans = 0;
    char name[64];
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
	sprintf(name,"Module_%d_PosTOF2_trans", i+1);
	ModulePosTOF2_trans[i] = new TGeoTranslation(name, TOF2_X_center[i], TOF2_Y_center[i], TOF2_Z_center[i]);
//        ModulePosTOF2_trans[i]->RegisterYourself();
    }
    Double_t GlassLayer1Pos = AlLayerThickness_TOF2 + GlassLayerThickness_TOF2/2. - ZWidthOfModule_TOF2/2.;
    sprintf(name,"GlassLayer1PosTOF2_trans");
    GlassLayer1PosTOF2_trans = new TGeoTranslation(name, 0., 0., GlassLayer1Pos);
    Double_t GasLayerPos = AlLayerThickness_TOF2 + GlassLayerThickness_TOF2 + GasLayerThickness_TOF2/2.- ZWidthOfModule_TOF2/2.;
    sprintf(name,"GasLayerPosTOF2_trans");
    GasLayerPosTOF2_trans = new TGeoTranslation(name, 0., 0., GasLayerPos);
    Double_t GlassLayer2Pos = AlLayerThickness_TOF2 + GlassLayerThickness_TOF2 + GasLayerThickness_TOF2 + GlassLayerThickness_TOF2/2. - ZWidthOfModule_TOF2/2.;
    sprintf(name,"GlassLayer2PosTOF2_trans");
    GlassLayer2PosTOF2_trans = new TGeoTranslation(name, 0., 0., GlassLayer2Pos);
    Double_t G10LayerPos = AlLayerThickness_TOF2 + GlassLayerThickness_TOF2 + GasLayerThickness_TOF2 + GlassLayerThickness_TOF2 + G10LayerThickness_TOF2/2. - ZWidthOfModule_TOF2/2.;
    sprintf(name,"G10LayerPosTOF2_trans");
    G10LayerPosTOF2_trans = new TGeoTranslation(name, 0., 0., G10LayerPos);
      
    //Solids (shapes)   
    //TGeoBBox *TOF2ContainerS = new TGeoBBox("TOF2ContainerS", TOF2_XWidth/2, TOF2_YWidth/2, TOF2_ZWidth/2);   
    //TGeoVolume *TOF2ContainerV = new TGeoVolume("TOF2ContainerV", TOF2ContainerS);
    //TOF2ContainerV->SetMedium(pMedAir);
    //TOF2ContainerV->SetVisibility(kFALSE);
    //top->AddNode(TOF2Top, 1, DetPos_trans);
//    TOF2Top->AddNode(TOF2ContainerV, 1, ContainerPos_trans);
    //TOF2Top->AddNode(TOF2ContainerV, 1);

    char nTOF2ModuleS[2][32] = {"TOF2ChamberS0","TOF2ChamberS1"};
    char nTOF2GlassVolumeS[2][32] = {"TOF2GlassVolumeS0","TOF2GlassVolumeS1"};
    char nTOF2G10VolumeS[2][32] = {"TOF2G10VolumeS0","TOF2G10VolumeS1"};
    char nTOF2ActiveGasVolumeS[2][32] = {"TOF2ActiveGasVolumeS0","TOF2ActiveGasVolumeS1"};
    char nTOF2ModuleV[2][32] = {"TOF2ChamberV0","TOF2ChamberV1"};
    char nTOF2GlassVolumeV[2][32] = {"TOF2GlassVolumeV0","TOF2GlassVolumeV1"};
    char nTOF2G10VolumeV[2][32] = {"TOF2G10VolumeV0","TOF2G10VolumeV1"};
    char nTOF2ActiveGasVolumeV[2][32] = {"TOF2ActiveGasVolumeV0","TOF2ActiveGasVolumeV1"};
    TGeoBBox *TOF2ModuleS[2];
    TGeoBBox *TOF2GlassVolumeS[2];
    TGeoBBox *TOF2ActiveGasVolumeS[2];
    TGeoBBox *TOF2G10VolumeS[2];
    TGeoVolume *TOF2ModuleV[2];
    TGeoVolume *TOF2GlassVolumeV[2];
    TGeoVolume *TOF2ActiveGasVolumeV[2];
    TGeoVolume *TOF2G10VolumeV[2];

    int nstr[2] = {32,16};
    for (int i = 0; i<2; i++)
    {
	TOF2ModuleS[i] = new TGeoBBox(nTOF2ModuleS[i], XWidthOfModule_TOF2[i]/2+0.001, YWidthOfModule_TOF2[i]/2+0.001, ZWidthOfModule_TOF2/2+0.001);   
	TOF2GlassVolumeS[i] = new TGeoBBox(nTOF2GlassVolumeS[i], XActWidthOfModule_TOF2[i]/2, YActWidthOfModule_TOF2[i]/2, GlassLayerThickness_TOF2/2);
	TOF2ActiveGasVolumeS[i] = new TGeoBBox(nTOF2ActiveGasVolumeS[i], XActWidthOfModule_TOF2[i]/2, YActWidthOfModule_TOF2[i]/2, GasLayerThickness_TOF2/2);
	TOF2G10VolumeS[i] = new TGeoBBox(nTOF2G10VolumeS[i], XActWidthOfModule_TOF2[i]/2, YActWidthOfModule_TOF2[i]/2, G10LayerThickness_TOF2/2);
    
	//Composite solids (shapes)
     
	//Volumes
    
	TOF2ModuleV[i] = new TGeoVolume(nTOF2ModuleV[i], TOF2ModuleS[i]);
	TOF2ModuleV[i]->SetMedium(medAl);
	TOF2ModuleV[i]->SetLineColor(kBlue);
	TOF2ModuleV[i]->SetVisibility(kTRUE);
	//TOF2ModuleV[i]->SetTransparency(80);
    
	TOF2GlassVolumeV[i] = new TGeoVolume(nTOF2GlassVolumeV[i], TOF2GlassVolumeS[i]);
	TOF2GlassVolumeV[i]->SetMedium(pMedRPCglass);
	TOF2GlassVolumeV[i]->SetLineColor(kGreen);
    
	TOF2ActiveGasVolumeV[i] = new TGeoVolume(nTOF2ActiveGasVolumeV[i], TOF2ActiveGasVolumeS[i]);
	TOF2ActiveGasVolumeV[i]->SetMedium(pMedRPCgas);
	TOF2ActiveGasVolumeV[i]->SetLineColor(kYellow);
	TOF2ActiveGasVolumeV[i]->Divide("ActiveGasStrip",2,nstr[i],0,-1);
    
	TOF2G10VolumeV[i] = new TGeoVolume(nTOF2G10VolumeV[i], TOF2G10VolumeS[i]);
	TOF2G10VolumeV[i]->SetMedium(medG10);
	TOF2G10VolumeV[i]->SetLineColor(kMagenta);

	TOF2ModuleV[i]->AddNode(TOF2GlassVolumeV[i], 1, GlassLayer1PosTOF2_trans);
	TOF2ModuleV[i]->AddNode(TOF2ActiveGasVolumeV[i], 1, GasLayerPosTOF2_trans);
	TOF2ModuleV[i]->AddNode(TOF2GlassVolumeV[i], 2, GlassLayer2PosTOF2_trans);
	TOF2ModuleV[i]->AddNode(TOF2G10VolumeV[i], 1, G10LayerPosTOF2_trans);

    }
      
    //Adding volumes to the TOP Volume
    Int_t nm[2] = {0};
    for (int i = 0; i < TOF2_Nmodules; i++)
    {
//	TOF2Top->AddNode(TOF2ModuleV[chtype[i]], nm[chtype[i]]+1, ModulePosTOF2_trans[i]);
//	nm[chtype[i]]++;
	TOF2Top->AddNode(TOF2ModuleV[chtype[i]], nm[0]+1, ModulePosTOF2_trans[i]);
	nm[0]++;
    }

    create_rootgeom_TOF700_support();
 
    top->AddNode(TOF2Top, 0);
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
	if (fscanf(fg,"%lf %lf %lf\n", &xoffs, &yoffs, &zoffs) != 3)
	{
	    printf("Wrong first line in TOF700 geometry file %s\n", fname);
	    return 0;
	};
	printf("Zoffs = %f\n", zoffs);
	for (int i=0; i<TOF2_MAX_CHAMBERS; i++) nstrips[i] = 0;
	int c = 0;
	while(fscanf(fg,"%s %d %lf %lf %lf %lf %lf %lf\n", &ic[0], &n, &step, &sy, &sx, &x, &y, &z) == 8)
	{
		strcpy(TOF2_Module_Names[c], ic);
		halfxwidth[c] = sx/20.;
		halfywidth[c] = sy/20.;
		zchamb[c] = z/10. + zoffs;
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
#include "create_rootgeom_TOF700_support.h"
