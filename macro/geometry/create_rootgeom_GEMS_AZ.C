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
const Double_t XSizeOfStation[nStations] = {60, 60, 80, 130, 160, 160, 240, 240, 240, 240, 240, 240};
const Double_t YSizeOfStation[nStations] = {60, 60, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80};
const Double_t ZStantionPosition[nStations] = {30, 50, 70, 90, 110, 140, 170, 200, 240, 280, 320, 360};

//Beam Pipe Hole Parameters
const Double_t BeamPipeMinRadius = 2.5; // radius of the hole in first GEM-station
const Double_t BeamPipeMaxRadius = 5.0; // radius of the hole in last GEM-station
      Double_t BeamPipeRadius[nStations];

//Module Parameters
const Int_t    nModulesInStation = 4;
const Double_t dXOuterFrame = 5.0;
const Double_t dXInnerFrame = 1.7;
const Double_t dYOuterFrame = dXOuterFrame;
const Double_t dYInnerFrame = dXInnerFrame;
const Double_t ZModuleSize = 1.5;
//AZ const Double_t dZSensitiveVolume = 1.0;
const Double_t dZSensitiveVolume = 0.02;
const Double_t xyShort = 8.0; //AZ - short strip area close to the beam
      Double_t XModuleSize[nStations];
      Double_t YModuleSize[nStations];

//Position of GEM-Detector
const Bool_t StationCenterPosition = kFALSE;                               
const Double_t ZDisplacementOfDetector = (ZModuleSize*4)/2;

    // -----------------------------------------------------------------------
void CalculateParameters() {
    
    //Calculate the Beam Pipe Radius for each GEM-stations
    Double_t BeamPipeAngle = (ATan((BeamPipeMaxRadius-BeamPipeMinRadius) / (ZStantionPosition[nStations-1] - ZStantionPosition[0])))*180/Pi();
    BeamPipeRadius[0] = BeamPipeMinRadius;
    BeamPipeRadius[nStations-1] = BeamPipeMaxRadius;
    Double_t delta_const = BeamPipeMaxRadius - BeamPipeMinRadius;
    for(UInt_t iStation = 1; iStation < nStations-1; iStation++) {         
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
        XModuleSize[iStation] = XSizeOfStation[iStation]/2 - BeamPipeRadius[iStation];
        YModuleSize[iStation] = YSizeOfStation[iStation]/2 - BeamPipeRadius[iStation];       
        cout << "     Modules["<<iStation<<"] = " << XModuleSize[iStation] << " :  " << YModuleSize[iStation] << "\n";
    }
    cout << "----------------------------" <<"\n";   
}

    // -----------------------------------------------------------------------
void create_rootgeom_GEMS_AZ() {
      
    CalculateParameters();
   
    // Load the necessary FairRoot libraries 
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries
     
    // -------   Load media from media file   --------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    TString medFile = geoPath + "/geometry/media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // -----------------------------------------------------------------------

    // -------   Geometry file name (output)   -------------------------------
    const TString geoDetectorName = "GEMS";
    const TString geoDetectorVersion = "v1_AZ";
    const TString geoFileName = geoPath + "/geometry/" + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // -----------------------------------------------------------------------  

    // -----------------   Get and create the required media    --------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");
  
    //AZ FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    FairGeoMedium* mArCO27030 = geoMedia->getMedium("titanium");
    if ( ! mArCO27030 ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    //AZ TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("arco27030");
    TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("titanium");
    if ( ! pMedArCO27030 ) Fatal("Main", "Medium arco27030 not found");
    
    // -----------------------------------------------------------------------
    
    // --------------   Create geometry and top volume  ----------------------
    gGeoMan = (TGeoManager*)gROOT->FindObject("FAIRGeom");
    gGeoMan->SetName(geoDetectorName + "_geom");
    TGeoVolume* top = new TGeoVolumeAssembly("TOP");
    top->SetMedium(pMedAir);
    gGeoMan->SetTopVolume(top);
    //gGeoMan->SetTopVisible(1);
    // -----------------------------------------------------------------------

    // Define TOP Geometry
    TGeoVolume* GEMSTop = new TGeoVolumeAssembly(geoDetectorName);
    GEMSTop->SetMedium(pMedAir);
    
    //Transformations (translations, rotations and scales)
    TGeoTranslation *SensitiveVolumePosition = new TGeoTranslation("SensitiveVolumePosition", (XModuleSize[0]-(XModuleSize[0]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame, (YModuleSize[0]-(YModuleSize[0]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame, 0.0);
    //SetPos_trans->RegisterYourself();
    
    //Solids (shapes)
    TGeoBBox *StationS[nStations];  
    TGeoBBox *ModuleS[nStations][nModulesInStation];
    TGeoBBox *SensitiveVolumeS[nStations][nModulesInStation];
    TGeoBBox *SensitiveVolume1S[nStations]; //AZ - short strip area close to the beam
    TGeoBBox *BeamPipeHoleS[nStations];
    
    for(UInt_t iStation = 0; iStation < nStations; iStation++) {
        TString SolidNameStation = "Station";
        SolidNameStation += iStation;
        SolidNameStation += "S";
        StationS[iStation] = new TGeoBBox(SolidNameStation, XSizeOfStation[iStation]/2, YSizeOfStation[iStation]/2, (ZModuleSize*4)/2);    
        
        for(UInt_t iModule = 0; iModule < nModulesInStation; iModule++) {
            TString SolidNameModule = "Module";
            SolidNameModule += iModule;
            SolidNameModule += "S";
            ModuleS[iStation][iModule] = new TGeoBBox(SolidNameModule, XModuleSize[iStation]/2, YModuleSize[iStation]/2, ZModuleSize/2);      
            TString SolidNameSensor = "SensitiveVolume";
            SolidNameSensor += iStation;
            SolidNameSensor += iModule;
            SolidNameSensor += "S";
	    SensitiveVolumeS[iStation][iModule] = new TGeoBBox(SolidNameSensor, (XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame))/2, (YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame))/2, dZSensitiveVolume/2);       
        }
        
        //SensitiveVolumeS[iStation] = new TGeoBBox("SensitiveVolumeS", (XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame))/2, (YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame))/2, dZSensitiveVolume/2);       
	SensitiveVolume1S[iStation] = new TGeoBBox("SensitiveVolumeSS", xyShort/2, xyShort/2, dZSensitiveVolume/2+0.0); //AZ - short strips
        
        TString SolidNameHole = "BeamPipeHole";
        SolidNameHole += iStation;
        SolidNameHole += "S";
        BeamPipeHoleS[iStation] = new TGeoBBox(SolidNameHole, BeamPipeRadius[iStation], BeamPipeRadius[iStation], (ZModuleSize*4)/2 + 0.1);
   
        //Subtraction the beam pipe hole from the current GEM-station 
        TString BooleanExpression = StationS[iStation]->GetName();
        BooleanExpression += "-";
        BooleanExpression += BeamPipeHoleS[iStation]->GetName();
        TGeoCompositeShape *StationWithHoleS = new TGeoCompositeShape(SolidNameStation, BooleanExpression);
        StationS[iStation] = StationWithHoleS;
    }

    //AZ - Subtract short strip area from the large one
    TGeoTranslation *shortPos[nStations][nModulesInStation] = {{NULL},{NULL}};
    for (UInt_t iStation = 0; iStation < nStations; iStation++) {
      for (UInt_t iModule = 0; iModule < nModulesInStation; iModule++) {
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

	TString solidNameSensor = SensitiveVolumeS[iStation][iModule]->GetName();
        TString booleanExpression = solidNameSensor;
        booleanExpression += "-(";
	booleanExpression += SensitiveVolume1S[iStation]->GetName();
	booleanExpression += ":";
	booleanExpression += shortPos[iStation][iModule]->GetName();
	booleanExpression += ")";
        TGeoCompositeShape *sensorCut = new TGeoCompositeShape(solidNameSensor, booleanExpression);
	SensitiveVolumeS[iStation][iModule] = sensorCut;
      }
    }

    //Volumes
    TGeoVolume *StationV[nStations]; 
    TGeoVolume *ModuleV[nStations][nModulesInStation];
    TGeoVolume *SensitiveVolumeV[nStations][nModulesInStation];
    TGeoVolume *SensitiveVolume1V[nStations]; //AZ - short strips
    for(UInt_t iStation = 0; iStation < nStations; iStation++) {
        TString VolumeName = "Station";
        VolumeName += iStation;
        VolumeName += "V";
        StationV[iStation] = new TGeoVolume(VolumeName, StationS[iStation]);
        StationV[iStation]->SetMedium(pMedAir);
        StationV[iStation]->SetLineColor(kGreen);
        StationV[iStation]->SetTransparency(95);
        //StationV[iStation]->SetVisibility(kFALSE);
        
        for(UInt_t iModule = 0; iModule < nModulesInStation; iModule++) {
            TString VolumeName = "Module";
            VolumeName += iModule;
            VolumeName += "V";
            ModuleV[iStation][iModule] = new TGeoVolume(VolumeName, ModuleS[iStation][iModule]);
            ModuleV[iStation][iModule]->SetMedium(pMedAir);
            ModuleV[iStation][iModule]->SetLineColor(kYellow);
            ModuleV[iStation][iModule]->SetTransparency(40);
            //ModuleV[iStation][iModule]->SetVisibility(kFALSE);
 
	    VolumeName = "SensitiveVolume_Sensor";
            VolumeName += iModule;
            VolumeName += "V";
 	    SensitiveVolumeV[iStation][iModule] = new TGeoVolume(VolumeName, SensitiveVolumeS[iStation][iModule]);
	    SensitiveVolumeV[iStation][iModule]->SetMedium(pMedArCO27030);
	    SensitiveVolumeV[iStation][iModule]->SetLineColor(kRed);    
        }   
        
	//SensitiveVolumeV[iStation] = new TGeoVolume("SensitiveVolume_SensorV", SensitiveVolumeS[iStation]);
	//SensitiveVolumeV[iStation]->SetMedium(pMedArCO27030);
	//SensitiveVolumeV[iStation]->SetLineColor(kRed);    
	//SensitiveVolumeV[iStation]->SetVisibility(kFALSE);
	//AZ - short strip area
	SensitiveVolume1V[iStation] = new TGeoVolume("SensitiveVolume_SensorSV", SensitiveVolume1S[iStation]);
	//SensitiveVolume1V[iStation] = new TGeoVolume("SensitiveVolume_SensoSV", SensitiveVolume1S[iStation]);
	SensitiveVolume1V[iStation]->SetMedium(pMedArCO27030);
	//SensitiveVolume1V[iStation]->SetMedium(pMedAir);
	SensitiveVolume1V[iStation]->SetLineColor(kBlue);    
    }
         
    //Adding volumes to the TOP Volume
    for(UInt_t iStation = 0; iStation < nStations; iStation++) {
        
        GEMSTop->AddNode(StationV[iStation], iStation, new TGeoTranslation(0.0, 0.0, ZStantionPosition[iStation]));
        
        for(UInt_t iModule = 0; iModule < nModulesInStation; iModule++) {
            TGeoTranslation *ModulePosition = new TGeoTranslation();
            TGeoTranslation *ActiveVolumePosition = new TGeoTranslation();
	    //AZ - short strip area
	    TString transName = "T";
	    transName += iStation;
	    transName += iModule;
	    TGeoTranslation *trans = (TGeoTranslation*) gGeoMan->GetListOfMatrices()->FindObject(transName);
            TGeoTranslation *shortStripPosition = new TGeoTranslation(*trans); 
	    shortStripPosition->SetName("aaa");
	    //Double_t dx = SensitiveVolumeS[iStation][iModule]->GetDX(), dy = SensitiveVolumeS[iStation][iModule]->GetDY();
            switch (iModule) {
                case 0:                
		  // x > 0, y < 0
                    ModulePosition->SetTranslation(XModuleSize[iStation]/2+BeamPipeRadius[iStation], -(YModuleSize[iStation]/2+BeamPipeRadius[iStation]) + (2*dYInnerFrame + 2*BeamPipeRadius[iStation]), -(1.5*ZModuleSize));
                    ActiveVolumePosition->SetTranslation(-((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame), (YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame, 0);
		    //shortStripPosition->SetTranslation(-dx + xyShort/2, dy - xyShort/2, 0.0); //AZ 
                    break;
                case 1:                
		  // x < 0, y < 0
                    ModulePosition->SetTranslation(-(XModuleSize[iStation]/2+BeamPipeRadius[iStation]) + (2*dXInnerFrame + 2*BeamPipeRadius[iStation]), -(YModuleSize[iStation]/2+BeamPipeRadius[iStation]), -(0.5*ZModuleSize));
                    ActiveVolumePosition->SetTranslation((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame, (YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame, 0);
		    //shortStripPosition->SetTranslation(dx - xyShort/2, dy - xyShort/2, 0.0); //AZ 
                    break;
                case 2:                
		  // x < 0, y > 0
                    ModulePosition->SetTranslation(-(XModuleSize[iStation]/2+BeamPipeRadius[iStation]), (YModuleSize[iStation]/2+BeamPipeRadius[iStation]) - (2*dYInnerFrame + 2*BeamPipeRadius[iStation]), 0.5*ZModuleSize);
                    ActiveVolumePosition->SetTranslation((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame, -((YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame), 0);
		    //shortStripPosition->SetTranslation(dx - xyShort/2, -dy + xyShort/2, 0.0); //AZ 
                    break;
                case 3:                
		  // x > 0, y > 0
                    ModulePosition->SetTranslation(XModuleSize[iStation]/2+BeamPipeRadius[iStation] - (2*dXInnerFrame + 2*BeamPipeRadius[iStation]), (YModuleSize[iStation]/2+BeamPipeRadius[iStation]), 1.5*ZModuleSize);
                    ActiveVolumePosition->SetTranslation(-((XModuleSize[iStation]-(XModuleSize[iStation]-(dXOuterFrame+dXInnerFrame)))/2 - dXInnerFrame), -((YModuleSize[iStation]-(YModuleSize[iStation]-(dYOuterFrame+dYInnerFrame)))/2 - dYInnerFrame), 0);
		    //shortStripPosition->SetTranslation(-dx + xyShort/2, -dy + xyShort/2, 0.0); //AZ 
                    break;                                
            }
            
	    //SensitiveVolumeV[iStation][iModule]->AddNode(SensitiveVolume1V[iStation], iModule, shortStripPosition); //AZ - short
            StationV[iStation]->AddNode(ModuleV[iStation][iModule], iModule, ModulePosition);      
	    shortStripPosition->Add(ActiveVolumePosition);
            ModuleV[iStation][iModule]->AddNode(SensitiveVolume1V[iStation], 1, shortStripPosition); //AZ
            //ModuleV[iStation][iModule]->AddNodeOverlap(SensitiveVolumeV[iStation][iModule], iModule, ActiveVolumePosition);       
            ModuleV[iStation][iModule]->AddNode(SensitiveVolumeV[iStation][iModule], 2, ActiveVolumePosition);       

        } 
    }
    
    if(StationCenterPosition == kTRUE) {
        top->AddNode(GEMSTop, 0);
    }
    else {
        top->AddNode(GEMSTop, 0, new TGeoTranslation(0, 0, ZDisplacementOfDetector));
    }
    
    top->SetVisContainers(kTRUE);
    
    // ---------------   Finish   --------------------------------------------
    gGeoMan->CloseGeometry();
    gGeoMan->CheckOverlaps(0.001);
    gGeoMan->PrintOverlaps();
    gGeoMan->Test();
    
    TFile* geoFile = new TFile(geoFileName, "RECREATE");
    top->Write();
    geoFile->Close();
    top->Draw("ogl");
}
