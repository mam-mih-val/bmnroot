//---------------------------
#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"

using namespace TMath;

//GeoManager
TGeoManager* gGeoMan = NULL;

//Set Parameters of GEMS
const Int_t nStations = 12;

//Stantion Parameters
const Double_t XSizeOfStation[nStations] = {60, 60, 80, 132, 160, 160, 240, 240, 240, 240, 240, 240};
const Double_t YSizeOfStation[nStations] = {60, 60, 80, 82, 80, 80, 80, 80, 80, 80, 80, 80};
const Double_t ZStantionPosition[nStations] = {30, 50, 70, 90, 110, 140, 170, 200, 240, 280, 320, 360};

//Beam Pipe Hole Parameters
const Double_t BeamPipeMinRadius = 2.5; // radius of the hole in first GEM-station
const Double_t BeamPipeMaxRadius = 5.0; // radius of the hole in last GEM-station
      Double_t BeamPipeRadius[nStations];

//Module Parameters
const Int_t    nModulesInStation = 4;
const Double_t dXOuterFrame = 5.0;
const Double_t dXInnerFrame = 1.7;
const Double_t dYOuterFrame = 3.75;
const Double_t dYInnerFrame = 1.7;
const Double_t dZSensitiveVolume = 1.0;
const Double_t dZSensor = 0.02; //AZ
const Double_t xyShort = 8.0; //AZ - short strip area close to the beam
const Double_t ZModuleSize = 2.5;

      Double_t XModuleSize[nStations];
      Double_t YModuleSize[nStations];

const Double_t XSizeSpacer = 10.0;
const Double_t YSizeSpacer = 10.0;
const Double_t SpacerWidth = 0.2;
const Double_t SpacerThickness = dZSensitiveVolume;

//Position of GEM-Detector
const Bool_t StationCenterPosition = kFALSE;
const Double_t ZDisplacementOfDetector = (ZModuleSize*2)/2;

//Add spacers or not?
const Bool_t WithSpacers = kTRUE;

void CalculateParameters() {
    //Calculate the Beam Pipe Radius for each GEM-stations
    Double_t BeamPipeAngle = (ATan((BeamPipeMaxRadius-BeamPipeMinRadius) / (ZStantionPosition[nStations-1] - ZStantionPosition[0])))*180/Pi();
    BeamPipeRadius[0] = BeamPipeMinRadius;
    BeamPipeRadius[nStations-1] = BeamPipeMaxRadius;
    Double_t delta_const = BeamPipeMaxRadius - BeamPipeMinRadius;
    for (UInt_t iStation = 1; iStation < nStations-1; iStation++) {
      //AZ BeamPipeRadius[iStation] = delta_const + (ZStantionPosition[iStation] - ZStantionPosition[0])*(BeamPipeAngle*Pi()/180);
      BeamPipeRadius[iStation] = delta_const + 
	(ZStantionPosition[iStation] - ZStantionPosition[0]) * TMath::Tan(BeamPipeAngle*TMath::DegToRad()); //AZ
    }
    cout << "Beam Pipe Angle = " <<  BeamPipeAngle << " deg" << "\n";
    cout << "Beam Pipe Radiuses:" << "\n";
    for(UInt_t iStation = 0; iStation < nStations; iStation++) {
        cout << "     Station["<<iStation<<"] = " << BeamPipeRadius[iStation] << " cm" << "\n";
    }
    cout << "----------------------------" <<"\n";

    //Calculate sizes of Modules
    cout << "Modules Size ( X : Y ):" << "\n";
    for(UInt_t iStation = 0; iStation < nStations; iStation++) {
        XModuleSize[iStation] = XSizeOfStation[iStation]/2;
        YModuleSize[iStation] = YSizeOfStation[iStation]/2;
        cout << "     Modules["<<iStation<<"] = " << XModuleSize[iStation] << " :  " << YModuleSize[iStation] << "\n";
    }
    cout << "----------------------------" <<"\n";
}

void create_rootgeom_GEMS_AZv2() {

    CalculateParameters();

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
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "v2_AZ";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    FairGeoMedium* mCarbon = geoMedia->getMedium("carbon");
    if ( ! mCarbon) Fatal("Main", "FairMedium carbon not found");
    geoBuild->createMedium(mCarbon);
    TGeoMedium* pMedCarbon = gGeoMan->GetMedium("carbon");
    if ( ! pMedCarbon ) Fatal("Main", "Medium carbon not found");

    FairGeoMedium* mFiberGlass = geoMedia->getMedium("fiberglass");
    if ( ! mFiberGlass ) Fatal("Main", "FairMedium fiberglass not found");
    geoBuild->createMedium(mFiberGlass);
    TGeoMedium* pMedFiberGlass = gGeoMan->GetMedium("fiberglass");
    if ( ! pMedFiberGlass ) Fatal("Main", "Medium fiberglass not found");

    //AZ FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("titanium");
    if ( ! mArCO27030 ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    //AZ TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("arco27030");
    TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("titanium");
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
    TGeoVolume* GEMSTop = new TGeoVolumeAssembly(geoDetectorName);
    GEMSTop->SetMedium(pMedAir);

    TGeoBBox *StationS[nStations];
    TGeoVolume *StationV[nStations];

    TGeoBBox *BeamPipeHoleS[nStations];
    TGeoCompositeShape *StationWithHoleS[nStations];

    TGeoBBox *ModuleS[nStations];
    TGeoVolume *ModuleV[nStations][nModulesInStation];

    TGeoBBox *VerticalOuterFrameS[nStations];
    TGeoBBox *HorizontalOuterFrameS[nStations];
    TGeoBBox *HorizontalInnerFrameS[nStations];
    TGeoBBox *VerticalInnerFrameS[nStations];

    TGeoVolume *VerticalOuterFrameV[nStations];
    TGeoVolume *HorizontalOuterFrameV[nStations];
    TGeoVolume *HorizontalInnerFrameV[nStations];
    TGeoVolume *VerticalInnerFrameV[nStations];

    //AZ TGeoBBox *SensitiveVolumeS[nStations];
    TGeoBBox *SensitiveVolumeS[nStations][nModulesInStation];
    TGeoBBox *SensitiveVolume1S[nStations]; //AZ - short strip area close to the beam
    TGeoVolume *SensitiveVolumeV[nStations][nModulesInStation];
    TGeoVolume *SensitiveVolume1V[nStations]; //AZ - short strips

    TGeoBBox *HorizontalSpacerS[nStations];
    TGeoBBox *VerticalSpacerPartS[nStations];
    TGeoBBox *VerticalSpacerSmallPartS[nStations];

    TGeoVolume *HorizontalSpacerV[nStations];
    TGeoVolume *VerticalSpacerPartV[nStations];
    TGeoVolume *VerticalSpacerSmallPartV[nStations];

    TGeoVolumeAssembly* SpacerBoxV[nStations][nModulesInStation];

    TGeoVolumeAssembly* FrameBoxV[nStations][nModulesInStation];

    TGeoTranslation *VerticalOuterFrameVPosition[nStations][nModulesInStation];
    TGeoTranslation *HorizontalOuterFrameVPosition[nStations][nModulesInStation];
    TGeoTranslation *HorizontalInnerFrameVPosition[nStations][nModulesInStation];
    TGeoTranslation *VerticalInnerFrameVPosition[nStations][nModulesInStation];

    TGeoRotation *SpacerBoxVRot[nModulesInStation];

    TGeoTranslation *SensitiveVolumePosition[nStations][nModulesInStation];

    TGeoTranslation *ModulePosition[nStations][nModulesInStation];

    TGeoTranslation *shortPos[nStations][nModulesInStation] = {{NULL},{NULL}}; //AZ - short strips

    for(Int_t iStation = 0; iStation < nStations; iStation++) {

      // AZ - do not create spacers
      //Creating spacers
      /*
      Int_t NHorizontalSpacers = (Int_t)((YModuleSize[iStation]-(dYInnerFrame+dYOuterFrame))/(YSizeSpacer+SpacerWidth));
      Int_t NVerticalSpacers = (Int_t)((XModuleSize[iStation]-(dXInnerFrame+dXOuterFrame))/(XSizeSpacer+SpacerWidth));
      Double_t YSizeSmallPart = (YModuleSize[iStation]-(dYInnerFrame+dYOuterFrame))/(YSizeSpacer+SpacerWidth);
      YSizeSmallPart -= (Int_t)YSizeSmallPart;
      YSizeSmallPart *= YSizeSpacer+SpacerWidth;
      
      HorizontalSpacerS[iStation] = new TGeoBBox("HorizontalSpacerS", (XModuleSize[iStation]-(dXInnerFrame+dXOuterFrame))/2, SpacerWidth/2, SpacerThickness/2);
      HorizontalSpacerV[iStation] = new TGeoVolume("HorizontalSpacerV", HorizontalSpacerS[iStation]);
      HorizontalSpacerV[iStation]->SetMedium(pMedFiberGlass);
      HorizontalSpacerV[iStation]->SetLineColor(kYellow);
      HorizontalSpacerV[iStation]->SetTransparency(10);
      
      VerticalSpacerPartS[iStation] = new TGeoBBox("VerticalSpacerPartS", SpacerWidth/2, YSizeSpacer/2, SpacerThickness/2);
      VerticalSpacerPartV[iStation] = new TGeoVolume("VerticalSpacerPartV", VerticalSpacerPartS[iStation]);
      VerticalSpacerPartV[iStation]->SetMedium(pMedFiberGlass);
      VerticalSpacerPartV[iStation]->SetLineColor(kYellow);
      VerticalSpacerPartV[iStation]->SetTransparency(10);
      
      VerticalSpacerSmallPartS[iStation] = new TGeoBBox("VerticalSpacerSmallPartS", SpacerWidth/2, YSizeSmallPart/2, SpacerThickness/2);
      VerticalSpacerSmallPartV[iStation] = new TGeoVolume("VerticalSpacerSmallPartV", VerticalSpacerSmallPartS[iStation]);
      VerticalSpacerSmallPartV[iStation]->SetMedium(pMedFiberGlass);
      VerticalSpacerSmallPartV[iStation]->SetLineColor(kYellow);
      VerticalSpacerSmallPartV[iStation]->SetTransparency(10);
      
      for(Int_t iModule = 0; iModule < nModulesInStation; iModule++) {
	SpacerBoxV[iStation][iModule] = new TGeoVolumeAssembly("SpacerBoxV");
	SpacerBoxV[iStation][iModule]->SetMedium(pMedArCO27030);
	
	Int_t hpart_cnt = 0;
	Int_t vpart_cnt = 0;
	for(Int_t ihspacer = 0; ihspacer < NHorizontalSpacers; ihspacer++) {
	  Double_t ydisph = ((YModuleSize[iStation]-(dYInnerFrame+dYOuterFrame))/2 - SpacerWidth/2) - YSizeSpacer*(ihspacer+1) - SpacerWidth*ihspacer;
	  SpacerBoxV[iStation][iModule]->AddNode(HorizontalSpacerV[iStation], hpart_cnt, new TGeoTranslation(0, ydisph , 0));
	  hpart_cnt++;
	  
	  for(Int_t ivspacer = 0; ivspacer < NVerticalSpacers; ivspacer++) {
	    Double_t xdispv = (-(XModuleSize[iStation]-(dXInnerFrame+dXOuterFrame))/2 + SpacerWidth/2) + XSizeSpacer*(ivspacer+1) + SpacerWidth*ivspacer;
	    Double_t ydispv = (YModuleSize[iStation]-(dYInnerFrame+dYOuterFrame))/2 - YSizeSpacer/2 - (YSizeSpacer+SpacerWidth)*ihspacer;
	    SpacerBoxV[iStation][iModule]->AddNode(VerticalSpacerPartV[iStation], vpart_cnt, new TGeoTranslation(xdispv, ydispv, 0));
	    vpart_cnt++;
	    
	    if(ihspacer == NHorizontalSpacers-1) {
	      Double_t ydispvs = (YModuleSize[iStation]-(dYInnerFrame+dYOuterFrame))/2 - YSizeSpacer/2 - (YSizeSpacer+SpacerWidth)*(ihspacer+1) + (YSizeSpacer-YSizeSmallPart)/2;
	      SpacerBoxV[iStation][iModule]->AddNode(VerticalSpacerSmallPartV[iStation], ivspacer, new TGeoTranslation(xdispv, ydispvs, 0));
	    }
	  }
	}
      }
      */
      
      //Creating sensitive shapes and volumes + Adding spacer boxes to sensitive volumes
      //AZ SensitiveVolumeS[iStation] = new TGeoBBox("SensitiveVolumeS", (XModuleSize[iStation] -(dXOuterFrame+dXInnerFrame))/2, (YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame))/2, dZSensitiveVolume/2);
      SensitiveVolume1S[iStation] = new TGeoBBox("SensitiveVolumeSS", xyShort/2, xyShort/2, dZSensor/2+0.0); //AZ - short strips

      for(Int_t iModule = 0; iModule < nModulesInStation; iModule++) {
	//AZ - Subtract short strip area from the large one
	TString solidNameSensor = "SensitiveVolume";
	solidNameSensor += iStation;
	solidNameSensor += iModule;
	solidNameSensor += "S";
	SensitiveVolumeS[iStation][iModule] = new TGeoBBox(solidNameSensor, (XModuleSize[iStation] -(dXOuterFrame+dXInnerFrame))/2, (YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame))/2, dZSensor/2);
	Double_t dx = SensitiveVolumeS[iStation][iModule]->GetDX(), dy = SensitiveVolumeS[iStation][iModule]->GetDY();
	TString transName = "T";
	transName += iStation;
	transName += iModule;
	Double_t ddx = -dx + xyShort/2, ddy = dy - xyShort/2;
	if (iModule == 1) ddx = dx - xyShort/2, ddy = dy - xyShort/2;
	else if (iModule == 2) ddx = dx - xyShort/2, ddy = -dy + xyShort/2;
	else if (iModule == 3) ddx = -dx + xyShort/2, ddy = -dy + xyShort/2;
	shortPos[iStation][iModule] = new TGeoTranslation(transName, ddx, ddy, 0.0);
	gGeoMan->AddTransformation(shortPos[iStation][iModule]);

	//TString solidNameSensor = SensitiveVolumeS[iStation][iModule]->GetName();
        TString booleanExpression = solidNameSensor;
        booleanExpression += "-(";
	booleanExpression += SensitiveVolume1S[iStation]->GetName();
	booleanExpression += ":";
	booleanExpression += shortPos[iStation][iModule]->GetName();
	booleanExpression += ")";
        TGeoCompositeShape *sensorCut = new TGeoCompositeShape(solidNameSensor, booleanExpression);
	SensitiveVolumeS[iStation][iModule] = sensorCut;
	//AZ

	SensitiveVolumeV[iStation][iModule] = new TGeoVolume("SensitiveVolume_SensorV", SensitiveVolumeS[iStation][iModule]);
	SensitiveVolumeV[iStation][iModule]->SetMedium(pMedArCO27030);
	SensitiveVolumeV[iStation][iModule]->SetLineColor(kRed);
	SensitiveVolumeV[iStation][iModule]->SetTransparency(50);
            //SensitiveVolumeV->SetVisibility(kFALSE);
	
	SpacerBoxVRot[iModule] = new TGeoRotation();
	
	switch(iModule) {
	case 0:
	  SpacerBoxVRot[iModule]->SetAngles(0, 0, 0);
	  break;
	  
	case 1:
	  SpacerBoxVRot[iModule]->SetAngles(0, 180, 180);
	  break;
	  
	case 2:
	  SpacerBoxVRot[iModule]->SetAngles(180, 0, 0);
	  break;
	  
	case 3:
	  SpacerBoxVRot[iModule]->SetAngles(0, 180, 0);
	  break;
	}
	
	/*AZ - no spacers
	if(WithSpacers)
	  SensitiveVolumeV[iStation][iModule]->AddNode(SpacerBoxV[iStation][iModule], iModule, SpacerBoxVRot[iModule]);
	*/

 	//AZ - short strip area
	SensitiveVolume1V[iStation] = new TGeoVolume("SensitiveVolume_SensorSV", SensitiveVolume1S[iStation]);
	//SensitiveVolume1V[iStation] = new TGeoVolume("SensitiveVolume_SensoSV", SensitiveVolume1S[iStation]); // - test
	SensitiveVolume1V[iStation]->SetMedium(pMedArCO27030);
	//SensitiveVolume1V[iStation]->SetMedium(pMedAir);
	SensitiveVolume1V[iStation]->SetLineColor(kBlue);    
      }

      //Creating border frames shapes and volumes
      VerticalOuterFrameS[iStation] = new TGeoBBox("VerticalOuterFrameS", dXOuterFrame/2, YModuleSize[iStation]/2, ZModuleSize/2);
      VerticalOuterFrameV[iStation] = new TGeoVolume("VerticalOuterFrameV", VerticalOuterFrameS[iStation]);
      //AZ VerticalOuterFrameV[iStation]->SetMedium(pMedCarbon);
      VerticalOuterFrameV[iStation]->SetMedium(pMedFiberGlass);
      VerticalOuterFrameV[iStation]->SetLineColor(kBlue);
      VerticalOuterFrameV[iStation]->SetTransparency(40);
      
      HorizontalOuterFrameS[iStation] = new TGeoBBox("HorizontalOuterFrameS", (XModuleSize[iStation]-dXOuterFrame)/2, dYOuterFrame/2, (ZModuleSize[iStation]-dZSensitiveVolume)/2);
      HorizontalOuterFrameV[iStation] = new TGeoVolume("HorizontalOuterFrameV", HorizontalOuterFrameS[iStation]);
      //AZ HorizontalOuterFrameV[iStation]->SetMedium(pMedCarbon);
      HorizontalOuterFrameV[iStation]->SetMedium(pMedFiberGlass);
      HorizontalOuterFrameV[iStation]->SetLineColor(kBlue);
      HorizontalOuterFrameV[iStation]->SetTransparency(40);
      
      HorizontalInnerFrameS[iStation] = new TGeoBBox("HorizontalInnerFrameS", (XModuleSize[iStation]-dXOuterFrame)/2, dYInnerFrame/2, (ZModuleSize[iStation]-dZSensitiveVolume)/2);
      HorizontalInnerFrameV[iStation] = new TGeoVolume("HorizontalInnerFrameV", HorizontalInnerFrameS[iStation]);
      //AZ HorizontalInnerFrameV[iStation]->SetMedium(pMedCarbon);
      HorizontalInnerFrameV[iStation]->SetMedium(pMedFiberGlass);
      HorizontalInnerFrameV[iStation]->SetLineColor(kBlue);
      HorizontalInnerFrameV[iStation]->SetTransparency(40);
      
      VerticalInnerFrameS[iStation] = new TGeoBBox("VerticalInnerFrameS", dXInnerFrame/2, (YModuleSize[iStation]-dYInnerFrame-dYOuterFrame)/2, (ZModuleSize[iStation]-dZSensitiveVolume)/2);
      VerticalInnerFrameV[iStation] = new TGeoVolume("VerticalInnerFrameV", VerticalInnerFrameS[iStation]);
      //AZ VerticalInnerFrameV[iStation]->SetMedium(pMedCarbon);
      VerticalInnerFrameV[iStation]->SetMedium(pMedFiberGlass);
      VerticalInnerFrameV[iStation]->SetLineColor(kBlue);
      VerticalInnerFrameV[iStation]->SetTransparency(40);

      //Creating box frames from parts
      for(Int_t iModule = 0; iModule < nModulesInStation; iModule++) {
	FrameBoxV[iStation][iModule] = new TGeoVolumeAssembly("FrameBoxV");
	FrameBoxV[iStation][iModule]->SetMedium(pMedAir);
	
	VerticalOuterFrameVPosition[iStation][iModule] = new TGeoTranslation();
	HorizontalOuterFrameVPosition[iStation][iModule] = new TGeoTranslation();
	HorizontalInnerFrameVPosition[iStation][iModule] = new TGeoTranslation();
	VerticalInnerFrameVPosition[iStation][iModule] = new TGeoTranslation();
	
	switch(iModule) {
	case 0:
	  VerticalOuterFrameVPosition[iStation][iModule]->SetTranslation(XModuleSize[iStation]/2 - dXOuterFrame/2, 0.0, 0.0);
	  HorizontalOuterFrameVPosition[iStation][iModule]->SetTranslation(-dXOuterFrame/2, -(YModuleSize[iStation]/2-dYOuterFrame/2), -(ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2));
	  HorizontalInnerFrameVPosition[iStation][iModule]->SetTranslation(-dXOuterFrame/2, YModuleSize[iStation]/2-dYInnerFrame/2, -(ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2));
	  VerticalInnerFrameVPosition[iStation][iModule]->SetTranslation(-(XModuleSize[iStation]/2 - dXInnerFrame/2), dYOuterFrame/2-dYInnerFrame/2, -(ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2));
	  break;
	  
	case 1:
	  VerticalOuterFrameVPosition[iStation][iModule]->SetTranslation(-(XModuleSize[iStation]/2 - dXOuterFrame/2), 0.0, 0.0);
	  HorizontalOuterFrameVPosition[iStation][iModule]->SetTranslation(dXOuterFrame/2, -(YModuleSize[iStation]/2-dYOuterFrame/2), ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2);
	  HorizontalInnerFrameVPosition[iStation][iModule]->SetTranslation(dXOuterFrame/2, YModuleSize[iStation]/2-dYInnerFrame/2, ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2);
	  VerticalInnerFrameVPosition[iStation][iModule]->SetTranslation(XModuleSize[iStation]/2 - dXInnerFrame/2, dYOuterFrame/2-dYInnerFrame/2, ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2);
	  break;
	  
	case 2:
	  VerticalOuterFrameVPosition[iStation][iModule]->SetTranslation(-(XModuleSize[iStation]/2 - dXOuterFrame/2), 0.0, 0.0);
	  HorizontalOuterFrameVPosition[iStation][iModule]->SetTranslation(dXOuterFrame/2, YModuleSize[iStation]/2-dYOuterFrame/2, -(ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2));
	  HorizontalInnerFrameVPosition[iStation][iModule]->SetTranslation(dXOuterFrame/2, -(YModuleSize[iStation]/2-dYInnerFrame/2), -(ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2));
	  VerticalInnerFrameVPosition[iStation][iModule]->SetTranslation(XModuleSize[iStation]/2 - dXInnerFrame/2, -(dYOuterFrame/2-dYInnerFrame/2), -(ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2));
	  break;
	  
	case 3:
	  VerticalOuterFrameVPosition[iStation][iModule]->SetTranslation(XModuleSize[iStation]/2 - dXOuterFrame/2, 0.0, 0.0);
	  HorizontalOuterFrameVPosition[iStation][iModule]->SetTranslation(-dXOuterFrame/2, YModuleSize[iStation]/2-dYOuterFrame/2, ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2);
	  HorizontalInnerFrameVPosition[iStation][iModule]->SetTranslation(-dXOuterFrame/2, -(YModuleSize[iStation]/2-dYInnerFrame/2), ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2);
	  VerticalInnerFrameVPosition[iStation][iModule]->SetTranslation(-(XModuleSize[iStation]/2 - dXInnerFrame/2), -(dYOuterFrame/2-dYInnerFrame/2), ZModuleSize[iStation]/2 - (ZModuleSize[iStation]-dZSensitiveVolume)/2);
	  break;
	  
	}
	
	FrameBoxV[iStation][iModule]->AddNode(VerticalOuterFrameV[iStation], iModule, VerticalOuterFrameVPosition[iStation][iModule]);
	FrameBoxV[iStation][iModule]->AddNode(HorizontalOuterFrameV[iStation], iModule, HorizontalOuterFrameVPosition[iStation][iModule]);
	FrameBoxV[iStation][iModule]->AddNode(HorizontalInnerFrameV[iStation], iModule, HorizontalInnerFrameVPosition[iStation][iModule]);
	FrameBoxV[iStation][iModule]->AddNode(VerticalInnerFrameV[iStation], iModule, VerticalInnerFrameVPosition[iStation][iModule]);
      }

      //Creating modules from parts
      ModuleS[iStation] = new TGeoBBox("ModuleS", XModuleSize[iStation]/2, YModuleSize[iStation]/2, ZModuleSize/2);

      for(Int_t iModule = 0; iModule < nModulesInStation; iModule++) {
	ModuleV[iStation][iModule] = new TGeoVolume("ModuleV", ModuleS[iStation]);
	ModuleV[iStation][iModule]->SetMedium(pMedAir);
	ModuleV[iStation][iModule]->SetLineColor(kYellow);
	ModuleV[iStation][iModule]->SetTransparency(40);
	//ModuleV[iModule]->SetVisibility(kFALSE);
	
	SensitiveVolumePosition[iStation][iModule] = new TGeoTranslation();

	//AZ - short strip area
	TString transName = "T";
	transName += iStation;
	transName += iModule;
	TGeoTranslation *trans = (TGeoTranslation*) gGeoMan->GetListOfMatrices()->FindObject(transName);
	TGeoTranslation *shortStripPosition = new TGeoTranslation(*trans); 
	shortStripPosition->SetName("aaa");
	//AZ

	switch(iModule) {
	case 0:
	  SensitiveVolumePosition[iStation][iModule]->SetTranslation(-((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame), (YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame, (ZModuleSize-dZSensitiveVolume)/2);
	  break;
	  
	case 1:
	  SensitiveVolumePosition[iStation][iModule]->SetTranslation((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame, (YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame, -(ZModuleSize-dZSensitiveVolume)/2);
	  break;
	  
	case 2:
	  SensitiveVolumePosition[iStation][iModule]->SetTranslation((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame, (YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYOuterFrame, (ZModuleSize-dZSensitiveVolume)/2);
	  break;
	  
	case 3:
	  SensitiveVolumePosition[iStation][iModule]->SetTranslation(-((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame), (YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYOuterFrame, -(ZModuleSize-dZSensitiveVolume)/2);
	  break;
	}
	
	ModuleV[iStation][iModule]->AddNode(FrameBoxV[iStation][iModule], iModule);
	ModuleV[iStation][iModule]->AddNode(SensitiveVolumeV[iStation][iModule], 0, SensitiveVolumePosition[iStation][iModule]);
	shortStripPosition->Add(SensitiveVolumePosition[iStation][iModule]); //AZ
	ModuleV[iStation][iModule]->AddNode(SensitiveVolume1V[iStation], 1, shortStripPosition); //AZ
      }

      //Creating GEM-stations from parts
      TString SolidName = "Station";
      SolidName += iStation;
      SolidName += "S";
      StationS[iStation] = new TGeoBBox(SolidName, (2*XModuleSize[iStation])/2 + BeamPipeRadius[iStation], (2*YModuleSize[iStation])/2 + BeamPipeRadius[iStation], (ZModuleSize*2)/2);
      
      SolidName = "BeamPipeHole";
      SolidName += iStation;
      SolidName += "S";
      BeamPipeHoleS[iStation] = new TGeoBBox(SolidName, BeamPipeRadius[iStation], BeamPipeRadius[iStation], (ZModuleSize*2)/2 + 0.1);
      
      //Subtraction the beam pipe hole from the current GEM-station
      TString BooleanExpression = StationS[iStation]->GetName();
      BooleanExpression += "-";
      BooleanExpression += BeamPipeHoleS[iStation]->GetName();
      StationWithHoleS[iStation] = new TGeoCompositeShape("StationWithHoleS", BooleanExpression);
      StationS[iStation] = StationWithHoleS[iStation];
      
      TString VolumeName = "Station";
      VolumeName += iStation;
      VolumeName += "V";
      StationV[iStation] = new TGeoVolume(VolumeName, StationS[iStation]);
      StationV[iStation]->SetMedium(pMedAir);
      StationV[iStation]->SetLineColor(kGreen);
      StationV[iStation]->SetTransparency(90);
      //StationV[iStation]->SetVisibility(kFALSE);
      
      for(Int_t iModule = 0; iModule < nModulesInStation; iModule++) {
	ModulePosition[iStation][iModule] = new TGeoTranslation();
	
	switch(iModule) {
	case 0:
	  // x > 0, y < 0
	  ModulePosition[iStation][iModule]->SetTranslation(XModuleSize[iStation]/2+BeamPipeRadius[iStation], -(YModuleSize[iStation]/2+BeamPipeRadius[iStation]) + (2*dYInnerFrame + 2*BeamPipeRadius[iStation]), -0.5*ZModuleSize);
	  break;
	  
	case 1:
	  // x < 0, y < 0
	  ModulePosition[iStation][iModule]->SetTranslation(-(XModuleSize[iStation]/2+BeamPipeRadius[iStation]) + (2*dXInnerFrame + 2*BeamPipeRadius[iStation]), -(YModuleSize[iStation]/2+BeamPipeRadius[iStation]), 0.5*ZModuleSize);
	  break;
	  
	case 2:
	  // x < 0, y > 0
	  ModulePosition[iStation][iModule]->SetTranslation(-(XModuleSize[iStation]/2+BeamPipeRadius[iStation]), (YModuleSize[iStation]/2+BeamPipeRadius[iStation]) - (2*dYInnerFrame + 2*BeamPipeRadius[iStation]), -0.5*ZModuleSize);
	  break;
	  
	case 3:
	  // x > 0, y > 0
	  ModulePosition[iStation][iModule]->SetTranslation(XModuleSize[iStation]/2+BeamPipeRadius[iStation] - (2*dXInnerFrame + 2*BeamPipeRadius[iStation]), (YModuleSize[iStation]/2+BeamPipeRadius[iStation]), 0.5*ZModuleSize);
	  break;
	}
	
	StationV[iStation]->AddNode(ModuleV[iStation][iModule], iModule, ModulePosition[iStation][iModule]);
      }
      
      GEMSTop->AddNode(StationV[iStation], iStation, new TGeoTranslation(0.0, 0.0, ZStantionPosition[iStation]));
    }

    if(StationCenterPosition == kTRUE) {
        top->AddNode(GEMSTop, 0);
    }
    else {
        top->AddNode(GEMSTop, 0, new TGeoTranslation(0, 0, ZDisplacementOfDetector));
    }

    top->SetVisContainers(kTRUE);

    // ---------------   Finish   -----------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.0001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();

    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
