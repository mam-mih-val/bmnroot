//---------------------------

#define TOF2_MAX_UBEAMS 42
#define TOF2_UBEAMS_ORIENTATIONS 4

//Detector's construct parameters   
Double_t Support_TOF2_long    = 3.;  
Double_t Support_TOF2_short   = 2.;  
Double_t Support_TOF2_thickness   = 0.15;  

Double_t Support_TOF2_X_center[TOF2_MAX_UBEAMS] = {0.};
Double_t Support_TOF2_Y_center[TOF2_MAX_UBEAMS] = {0.};
Double_t Support_TOF2_Z_center[TOF2_MAX_UBEAMS] = {0.};
Double_t Support_TOF2_length[TOF2_MAX_UBEAMS] = {0.};
Int_t Support_TOF2_orientation[TOF2_MAX_UBEAMS] = {0};

int readGeomSupport(char *geomfile);

void create_rootgeom_TOF700_support() {
    
    // -------   Geometry text file name (input)   ----------------------------------
    readGeomSupport((char *)"TOF700_geometry_support.txt");

    TGeoTranslation *ChannelPosTOF2_trans[TOF2_MAX_UBEAMS] = {NULL};
    TGeoTranslation *AirPosTOF2_trans[4] = {NULL};
    char name[64];
    for (int i = 0; i < TOF2_MAX_UBEAMS; i++)
    {
	sprintf(name,"Channel_%d_PosTOF2_trans", i+1);
	ChannelPosTOF2_trans[i] = new TGeoTranslation(name, Support_TOF2_X_center[i], Support_TOF2_Y_center[i], Support_TOF2_Z_center[i]);
    }
    AirPosTOF2_trans[0] = new TGeoTranslation("AirPos0", 0., +Support_TOF2_thickness, 0.);
    AirPosTOF2_trans[1] = new TGeoTranslation("AirPos1", 0., -Support_TOF2_thickness, 0.);
    AirPosTOF2_trans[2] = new TGeoTranslation("AirPos2", +Support_TOF2_thickness, 0., 0.);
    AirPosTOF2_trans[3] = new TGeoTranslation("AirPos3", -Support_TOF2_thickness, 0., 0.);
   
    char nTOF2ChannelS[TOF2_MAX_UBEAMS][32];
    char nTOF2ChannelAirS[TOF2_MAX_UBEAMS][32];
    char nTOF2ChannelV[TOF2_MAX_UBEAMS][32];
    char nTOF2ChannelAirV[TOF2_MAX_UBEAMS][32];
    TGeoBBox *TOF2ChannelS[TOF2_MAX_UBEAMS];
    TGeoBBox *TOF2ChannelAirS[TOF2_MAX_UBEAMS];
    TGeoVolume *TOF2ChannelV[TOF2_MAX_UBEAMS];
    TGeoVolume *TOF2ChannelAirV[TOF2_MAX_UBEAMS];

    for (int i = 0; i<TOF2_MAX_UBEAMS; i++)
    {
	sprintf(nTOF2ChannelS[i],"TOF2ChannelS%d",i+1);
	sprintf(nTOF2ChannelAirS[i],"TOF2ChannelAirS%d",i+1);
	sprintf(nTOF2ChannelV[i],"TOF2ChannelV%d",i+1);
	sprintf(nTOF2ChannelAirV[i],"TOF2ChannelAirV%d",i+1);
	switch (Support_TOF2_orientation[i])
	{
	  case 0:
	    TOF2ChannelS[i] = new TGeoBBox(nTOF2ChannelS[i], Support_TOF2_length[i]/2., Support_TOF2_short, Support_TOF2_long);   
	    TOF2ChannelAirS[i] = new TGeoBBox(nTOF2ChannelAirS[i], Support_TOF2_length[i]/2., Support_TOF2_short-Support_TOF2_thickness, Support_TOF2_long-Support_TOF2_thickness);   
    
	    //Volumes
    
	    TOF2ChannelV[i] = new TGeoVolume(nTOF2ChannelV[i], TOF2ChannelS[i]);
	    TOF2ChannelV[i]->SetMedium(medAl);
	    TOF2ChannelV[i]->SetLineColor(kYellow);
	    TOF2ChannelV[i]->SetVisibility(kTRUE);
	    //TOF2ChannelV[i]->SetTransparency(80);
    
	    TOF2ChannelAirV[i] = new TGeoVolume(nTOF2ChannelAirV[i], TOF2ChannelAirS[i]);
	    TOF2ChannelAirV[i]->SetMedium(pMedAir);
//	    TOF2ChannelAirV[i]->SetVisibility(kFALSE);
//	    TOF2ChannelAirV[i]->SetLineColor(kGreen);
	    TOF2ChannelAirV[i]->SetVisibility(kTRUE);
	    TOF2ChannelAirV[i]->SetLineColor(kWhite);
    
	    TOF2ChannelV[i]->AddNode(TOF2ChannelAirV[i], 1, AirPosTOF2_trans[0]);
	    break;
	  case 1:
	    TOF2ChannelS[i] = new TGeoBBox(nTOF2ChannelS[i], Support_TOF2_length[i]/2., Support_TOF2_short, Support_TOF2_long);   
	    TOF2ChannelAirS[i] = new TGeoBBox(nTOF2ChannelAirS[i], Support_TOF2_length[i]/2., Support_TOF2_short-Support_TOF2_thickness, Support_TOF2_long-Support_TOF2_thickness);   
    
	    //Volumes
    
	    TOF2ChannelV[i] = new TGeoVolume(nTOF2ChannelV[i], TOF2ChannelS[i]);
	    TOF2ChannelV[i]->SetMedium(medAl);
	    TOF2ChannelV[i]->SetLineColor(kYellow);
	    TOF2ChannelV[i]->SetVisibility(kTRUE);
	    //TOF2ChannelV[i]->SetTransparency(80);
    
	    TOF2ChannelAirV[i] = new TGeoVolume(nTOF2ChannelAirV[i], TOF2ChannelAirS[i]);
	    TOF2ChannelAirV[i]->SetMedium(pMedAir);
//	    TOF2ChannelAirV[i]->SetVisibility(kFALSE);
//	    TOF2ChannelAirV[i]->SetLineColor(kGreen);
	    TOF2ChannelAirV[i]->SetVisibility(kTRUE);
	    TOF2ChannelAirV[i]->SetLineColor(kWhite);
    
	    TOF2ChannelV[i]->AddNode(TOF2ChannelAirV[i], 1, AirPosTOF2_trans[1]);
	    break;
	  case 2:
	    TOF2ChannelS[i] = new TGeoBBox(nTOF2ChannelS[i], Support_TOF2_short, Support_TOF2_length[i]/2., Support_TOF2_long);   
	    TOF2ChannelAirS[i] = new TGeoBBox(nTOF2ChannelAirS[i], Support_TOF2_short-Support_TOF2_thickness, Support_TOF2_length[i]/2., Support_TOF2_long-Support_TOF2_thickness);   
    
	    //Volumes
    
	    TOF2ChannelV[i] = new TGeoVolume(nTOF2ChannelV[i], TOF2ChannelS[i]);
	    TOF2ChannelV[i]->SetMedium(medAl);
	    TOF2ChannelV[i]->SetLineColor(kYellow);
	    TOF2ChannelV[i]->SetVisibility(kTRUE);
	    //TOF2ChannelV[i]->SetTransparency(80);
    
	    TOF2ChannelAirV[i] = new TGeoVolume(nTOF2ChannelAirV[i], TOF2ChannelAirS[i]);
	    TOF2ChannelAirV[i]->SetMedium(pMedAir);
//	    TOF2ChannelAirV[i]->SetVisibility(kFALSE);
//	    TOF2ChannelAirV[i]->SetLineColor(kGreen);
	    TOF2ChannelAirV[i]->SetVisibility(kTRUE);
	    TOF2ChannelAirV[i]->SetLineColor(kWhite);
    
	    TOF2ChannelV[i]->AddNode(TOF2ChannelAirV[i], 1, AirPosTOF2_trans[2]);
	    break;
	  case 3:
	    TOF2ChannelS[i] = new TGeoBBox(nTOF2ChannelS[i], Support_TOF2_short, Support_TOF2_length[i]/2., Support_TOF2_long);   
	    TOF2ChannelAirS[i] = new TGeoBBox(nTOF2ChannelAirS[i], Support_TOF2_short-Support_TOF2_thickness, Support_TOF2_length[i]/2., Support_TOF2_long-Support_TOF2_thickness);   
    
	    //Volumes
    
	    TOF2ChannelV[i] = new TGeoVolume(nTOF2ChannelV[i], TOF2ChannelS[i]);
	    TOF2ChannelV[i]->SetMedium(medAl);
	    TOF2ChannelV[i]->SetLineColor(kYellow);
	    TOF2ChannelV[i]->SetVisibility(kTRUE);
	    //TOF2ChannelV[i]->SetTransparency(80);
    
	    TOF2ChannelAirV[i] = new TGeoVolume(nTOF2ChannelAirV[i], TOF2ChannelAirS[i]);
	    TOF2ChannelAirV[i]->SetMedium(pMedAir);
//	    TOF2ChannelAirV[i]->SetVisibility(kFALSE);
//	    TOF2ChannelAirV[i]->SetLineColor(kGreen);
	    TOF2ChannelAirV[i]->SetVisibility(kTRUE);
	    TOF2ChannelAirV[i]->SetLineColor(kWhite);
    
	    TOF2ChannelV[i]->AddNode(TOF2ChannelAirV[i], 1, AirPosTOF2_trans[3]);
	    break;
	default: break;
	}
    }
      
    //Adding volumes to the TOP Volume
    for (int i = 0; i < TOF2_MAX_UBEAMS; i++)
    {
      TOF2Top->AddNode(TOF2ChannelV[i], i+1, ChannelPosTOF2_trans[i]);
    }
}

int readGeomSupport(char *geomfile)
{
	char fname[128];
	FILE *fg = 0;
	int n = 0;
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

	int c = 0;
	while(fscanf(fg,"%d %lf %lf %lf %lf %d\n", &n,
		&Support_TOF2_X_center[c], &Support_TOF2_Y_center[c], &Support_TOF2_Z_center[c], &Support_TOF2_length[c], &Support_TOF2_orientation[c]) == 6)
	{
		Support_TOF2_X_center[c] += xoffs;
		Support_TOF2_Y_center[c] += yoffs;
		Support_TOF2_Z_center[c] += zoffs;
		c++;
		if (c >= TOF2_MAX_UBEAMS) break;
	}
	fclose(fg);
	return 1;
}
