
//ECAL = 8x7x9x215
//ECAL = 8 SuperModules 
//SuperModule = 7 Modules
//Module = 9 ADCmodules
//ADCmodule = X:4cm x  Y:4cm x Z:215 layers * (0.045 Pb + 0.15 Sci) 
//front steel cover ---PIPEC FIXME
//ADCmodules == modules in the code
//
//	TString geoFileName = "bmnGeoEcal.root";
//________________________________________________________________________________

                                             
//                                            
//ECAL Position and rotation in BM@N
	Double_t ecalX = -175  ;             //-175;     //-55       //ecalX = -167.18
	Double_t ecalY =  12.4;             //12.4;      //12.4      //ecalY = 13.155;
	Double_t ecalZ =   283.1;          //283.1;    //283.1      //ecalZ = 263.437;
	Double_t rotationAngle = -20;       //-20;    // 10;        //rotationAngle = -15.;
//________________________________________________________________________________
//ECAL Parameters	
	int Xmodules = 24; //X modules = ADCmodules 
	int Ymodules = 21;
	int Zlayers = 215;
	Double_t thickPb = 0.030; //lead
	Double_t thickSci = 0.15; //scintilator
	Double_t thickSteel = 0.2; //front cover FIXME
	Double_t thickPaint = 0.0075; //paint
	Double_t thickPbSciPaint = thickPaint + thickPb + thickPaint + thickSci;
	Double_t modwidth = 4;
	Double_t modheight = 4;
	Double_t modlength = thickSteel + thickPbSciPaint * Zlayers;
    

//________________________________________________________________________________
void createEcalGeo1(){
	
	//gStyle->SetCanvasPreferGL(true);
	//gSystem->Load("libGeom");
	// Load the necessary FairRoot libraries 
//    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
//    bmnloadlibs();
    
//	TGeoRotation   *rot = new TGeoRotation("rot1", rotationAngle, 0., 0.);
//	TGeoRotation   *rot = new TGeoRotation("rot1", 0., rotationAngle, 0.);
//	TGeoRotation   *rot = new TGeoRotation("rot1", 0., 0., rotationAngle);
	

	TGeoRotation *rot = new TGeoRotation("rot1", 0., 0., 0.);
        rot->Print();
        rot->RotateY(rotationAngle);
        TGeoCombiTrans *combi1 = new TGeoCombiTrans(ecalX, ecalY, ecalZ, rot);
//GeoManager
	TGeoManager* gGeoMan = NULL;//TGeoManager *geom = new TGeoManager("geom", "ECAL geometry");
// -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
   	gGeoMan = gGeoManager;
    
    //Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "ECALlalyoPOS5";
    const TString geoDetectorVersion = "v1";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    cout<<geoFileName<<endl;

	//--- define some media
	FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mvacuum      = geoMedia->getMedium("vacuum");    
	if ( ! mvacuum ) Fatal("Main", "FairMedium vacuum not found"); 
	geoBuild->createMedium(mvacuum);
	TGeoMedium* pvacuum = gGeoMan->GetMedium("vacuum"); 
	if ( ! pvacuum ) Fatal("Main", "Medium vacuum not found"); 
  
    FairGeoMedium* mFscScint      = geoMedia->getMedium("FscScint"); 
	if ( ! mFscScint ) Fatal("Main", "FairMedium FscScint not found"); 
	geoBuild->createMedium(mFscScint);
	TGeoMedium* pFscScint  = gGeoMan->GetMedium("FscScint"); 
	if ( ! pFscScint  ) Fatal("Main", "Medium FscScint not found"); 
        
 	FairGeoMedium* mPb      = geoMedia->getMedium("lead");  
	if ( ! mPb ) Fatal("Main", "FairMedium Pb not found"); 
	geoBuild->createMedium(mPb);
	TGeoMedium* pPb = gGeoMan->GetMedium("lead"); 
	if ( ! pPb ) Fatal("Main", "Medium Pb not found"); 
	
	FairGeoMedium* msteel      = geoMedia->getMedium("air");  //FIXME
	if ( ! msteel ) Fatal("Main", "FairMedium steel not found"); //FIXME
	geoBuild->createMedium(msteel);
	TGeoMedium* psteel = gGeoMan->GetMedium("air"); //FIXME
	if ( ! psteel ) Fatal("Main", "Medium steel not found"); //FIXME
	
	FairGeoMedium* mTiO2      = geoMedia->getMedium("TiO2");  
	if ( ! mTiO2  ) Fatal("Main", "FairMedium TiO2 not found"); 
	geoBuild->createMedium(mTiO2);
	TGeoMedium* pTiO2 = gGeoMan->GetMedium("TiO2"); 
	if ( ! pTiO2 ) Fatal("Main", "Medium TiO2 not found"); 
	//------------------------- VOLUMES -----------------------------------------

// --------------   Create geometry and top volume  -------------------------
	TGeoVolume* top = new TGeoVolumeAssembly("TOP");
	top->SetMedium(pvacuum); //FIXME
	top->SetLineColor(kBlue);
	top->SetVisibility(kTRUE);
	top->SetTransparency(100);
	
	gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
	gGeoMan->SetName("ECALgeom");
	gGeoMan->SetTopVolume(top);
        gGeoMan->SetTopVisible(0);
	
	TGeoVolume *ecal = gGeoMan->MakeBox("ecal",pvacuum,Xmodules*modwidth/2,Ymodules*modheight/2,modlength/2);
	ecal->SetVisibility(kTRUE);//kFALSE
	ecal->SetTransparency(100);
//	combi1->RotateY(rotationAngle);
	top->AddNode(ecal, 0, combi1);
	
	TGeoVolume *module = gGeoMan->MakeBox("mod", pvacuum, modwidth/2, modheight/2, modlength/2);
	module->SetVisibility(kTRUE);//kFALSE
	module->SetTransparency(100);
  	module->SetLineColor(kRed);
  	module->SetLineWidth(0);
	TGeoVolume *layerPb = gGeoMan->MakeBox("layPb", pPb, modwidth/2, modheight/2, thickPb/2);
	layerPb->SetVisibility(kTRUE);//kFALSE
  	layerPb->SetLineColor(kBlue);
  	layerPb->SetLineWidth(0);
	layerPb->SetTransparency(100);  	
	
	TGeoVolume *layerPaint = gGeoMan->MakeBox("layPaint", pTiO2, modwidth/2, modheight/2, thickPaint/2); 
	layerPaint->SetVisibility(kTRUE);//kFALSE
  	layerPaint->SetLineColor(kWhite);
  	layerPaint->SetLineWidth(0);
	layerPaint->SetTransparency(100);  	
	
	TGeoVolume *layerSci = gGeoMan->MakeBox("laySci", pFscScint, modwidth/2, modheight/2, thickSci/2); 
  	layerSci->SetLineColor(kWhite+1);
	layerSci->SetTransparency(100);
	TGeoVolume *covSteel = gGeoMan->MakeBox("covSteel", air, modwidth/2, modheight/2, thickSteel/2);//FIXME 
	covSteel->SetVisibility(kTRUE);//kFALSE
  	covSteel->SetLineColor(kWhite);
	covSteel->SetTransparency(100);
  	

//------------------ STRUCTURE  -----------------------------------------
	module->AddNode(covSteel, 0, new TGeoTranslation(0,0, thickSteel/2 - modlength/2));
	Double_t z;
//	Zlayers = 1;
	Int_t nlay;
	for(int zlayer = 0; zlayer < Zlayers; zlayer++){
		z = thickSteel + thickPbSciPaint * zlayer;
		//TString layerNamePb = TString::Format("layerPb%d",zlayer);
		//TString layerNameSci = TString::Format("layerSci%d",zlayer);	
		module->AddNode(layerPaint, nlay, new TGeoTranslation(0,0, z + thickPaint/2 -modlength/2));
		module->AddNode(layerPb, nlay, new TGeoTranslation(0,0, z + thickPaint + thickPb/2 - modlength/2));
		module->AddNode(layerPaint, nlay, new TGeoTranslation(0,0, z + thickPaint + thickPb + thickPaint/2 -modlength/2));
		module->AddNode(layerSci, zlayer, new TGeoTranslation(0,0, z + thickPaint + thickPb + thickPaint + thickSci/2 - modlength/2));
	}
	
	int nmod = 0;
//	Xmodules = 1;
//	Ymodules = 1;
	Double_t x0 = modwidth*Xmodules/2 - modwidth/2;
	Double_t y0 = modheight*Ymodules/2 - modheight/2;
	for(int xmod = 0 ; xmod < Xmodules; xmod++){
		for(int ymod = 0; ymod < Ymodules; ymod++){
			TString moduleName = TString::Format("mod_X:%d_Y:%d",xmod,ymod);
			
			module->SetName(moduleName);
			cout<<module->GetName()<<endl;
			ecal->AddNode(module, nmod++, new TGeoTranslation(x0 - modwidth * xmod,	-y0 + modheight * ymod,0));
		}//ymod
	}//xmod			

    top->SetVisContainers(kTRUE);
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    gGeoMan->SetVisLevel(3);
    //gGeoMan->SetVisOption(1);
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}

