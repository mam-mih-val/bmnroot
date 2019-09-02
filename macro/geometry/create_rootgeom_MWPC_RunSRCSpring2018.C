#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
//#include "BmnMwpcGeometry.h"

using namespace TMath;

TGeoManager* gGeoMan = NULL;
void create_rootgeom_MWPC_RunSRCSpring2018() {
    
    BmnMwpcGeometry* mwpcGeo = new BmnMwpcGeometry();
    
    //Number of active planes with wires
    const Int_t NofPlanes = mwpcGeo->GetNPlanes();
    const Double_t Zshift  = -574.144 - 71.047; 
    //Detector's position
    const Double_t MWPC0_Xpos = -0.24;
    const Double_t MWPC0_Ypos =-3.342;
    const Double_t MWPC0_Zpos = -206.042 + Zshift;
    
    const Double_t MWPC1_Xpos = -0.109;
    const Double_t MWPC1_Ypos = -3.601;
    const Double_t MWPC1_Zpos = -107.363 + Zshift;  
    
    const Double_t MWPC2_Xpos = 0.271;
    const Double_t MWPC2_Ypos = 6.038;
    const Double_t MWPC2_Zpos = 287.858 + Zshift;
    
    const Double_t MWPC3_Xpos = 0.234;
    const Double_t MWPC3_Ypos = 6.14;
    const Double_t MWPC3_Zpos = 437.568 + Zshift;        

    //           2   
    //        A______B
    //       / \     |\
    //   3  /   \    | \  1
    //     /     \   |  \             */ 
    //     \      \  |  /   
    //   4  \      \ | /  0
    //       \______\|/
    //                C
    //           5   


    //                        X     
    //     _____ <----1----> _____ <-2-> _____
    //    |     |     |     |     |     |     |
    //    |     |     |     |     |     |     |
    //    |     |     |     |     |     |     |
    //    |     |     |     |     |     |     |
    //    |_____|_____|_____|_____|_____|_____|
    //                        X'
    //
    //   1 - GapWireOnePlane
    //   2 - GapWireTwoPlanes

	 //
	 //    |			 |			|			|			|			|			|			 |
	 //    |			 |			|			|			|			|			|			 |
	 //	 |2.1338  |1.0015 |1.0015  |1.0015	|1.0015	|1.0015	|2.1328	 |			
	 //
	 //  
	 //     E__D__________F
	 //      \ |         /
	 //       \|        /
	 //        G______H/
	 //
    //Detector's construct parameters   
    const Double_t ABsize = mwpcGeo->GetPlaneWidth(); //cm
    const Double_t BCsize = mwpcGeo->GetPlaneHeight(); //cm
    const Double_t GapZsize = mwpcGeo->GetPlaneStep(); //cm

    const Double_t XSizeOfActiveVolume = ABsize;
    const Double_t YSizeOfActiveVolume = BCsize;
    const Double_t ZSizeOfActiveVolume = GapZsize * NofPlanes; // cm

    //FIXME!!! make sure! sizes of frame
    //const Double_t ZLengthChamber = ZSizeOfActiveVolume + 1.0;
    const Double_t ZLengthChamber = 9.2741;
    const Double_t CopperLayer = 0.005;
    const Double_t G10Layer = 0.15;
	 
    //shell parameters 
    const Double_t DGsize = 23;
    const Double_t GHsize = 24.9;
    const Double_t EFsize = 51.56;
    const Double_t AluminiumLayer = 0.2;	 
	 	
    // -------   Load media from media file   -----------------------------------
    FairGeoLoader* geoLoad = new FairGeoLoader("TGeo", "FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/";
    TString medFile = geoPath + "media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "MWPC";
    const TString geoDetectorVersion = "RunSRCSpring2018";
    TString geoFileName = geoPath + geoDetectorName + "_" + geoDetectorVersion + ".root";
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
    
    FairGeoMedium* mAluminium = geoMedia->getMedium("aluminium");
    if (!mAluminium) Fatal("Main", "FairMedium aluminium not found");
    geoBuild->createMedium(mAluminium);
    TGeoMedium* pMedAluminium = gGeoMan->GetMedium("aluminium");
    if (!pMedAluminium) Fatal("Main", "Medium aluminium not found");
    
    FairGeoMedium* mG10 = geoMedia->getMedium("G10");
    if (!mG10) Fatal("Main", "FairMedium G10 not found");
    geoBuild->createMedium(mG10);
    TGeoMedium* pMedG10 = gGeoMan->GetMedium("G10");
    if (!pMedG10) Fatal("Main", "Medium G10 not found");
    
    FairGeoMedium* mCopper = geoMedia->getMedium("copper");
    if (!mCopper) Fatal("Main", "FairMedium copper not found");
    geoBuild->createMedium(mCopper);
    TGeoMedium* pMedCopper = gGeoMan->GetMedium("copper");
    if (!pMedCopper) Fatal("Main", "Medium copper not found");
    

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
    TGeoVolume* MWPC = new TGeoVolumeAssembly(geoDetectorName);
    MWPC->SetMedium(pMedAir);
    MWPC->SetTransparency(50);
    
    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos0_trans = new TGeoTranslation("DetPos0_trans", MWPC0_Xpos, MWPC0_Ypos, MWPC0_Zpos);
    TGeoTranslation *DetPos1_trans = new TGeoTranslation("DetPos1_trans", MWPC1_Xpos, MWPC1_Ypos, MWPC1_Zpos);
    TGeoTranslation *DetPos2_trans = new TGeoTranslation("DetPos2_trans", MWPC2_Xpos, MWPC2_Ypos, MWPC2_Zpos);
    TGeoTranslation *DetPos3_trans = new TGeoTranslation("DetPos3_trans", MWPC3_Xpos, MWPC3_Ypos, MWPC3_Zpos);
    //Solids (shapes)  
    //hexagon which contains active wire planes 
    TGeoPgon *MWPCContainerS = new TGeoPgon("MWPCContainerS", 0, 360, NofPlanes, 2);
    MWPCContainerS->DefineSection(0, -ZLengthChamber / 2.0 - AluminiumLayer/2. - CopperLayer - G10Layer, 0.0, YSizeOfActiveVolume / 2.0 + DGsize + 0.5);
    MWPCContainerS->DefineSection(1,  ZLengthChamber / 2.0 + AluminiumLayer/2. + CopperLayer + G10Layer, 0.0, YSizeOfActiveVolume / 2.0 + DGsize + 0.5);
    
    //active wire plane
    TGeoBBox *MWPCActivePlaneS = new TGeoBBox("MWPCActivePlaneS", XSizeOfActiveVolume / 2.0, YSizeOfActiveVolume / 2.0, 0.1);

    //aluminum plates
    TGeoTrap *MWPCAlumPlateS = new TGeoTrap("MWPCAluminiumPlateS",AluminiumLayer/2.0,0,0,DGsize/2.0,GHsize/2.0,EFsize/2.0,0,DGsize/2.0,GHsize/2.0,EFsize/2.0,0); 
    TGeoTrap *MWPCAlumCoverS = new TGeoTrap("MWPCAluminiumCoverS",ZLengthChamber / 2.0,0,0,AluminiumLayer/2.0,EFsize/2.0,EFsize/2.0,0,AluminiumLayer/2.0,EFsize/2.0,EFsize/2.0,0); 
    
    //G10 plates
    TGeoPgon *MWPCG10PlateS = new TGeoPgon("MWPCG10PlateS",0, 360, NofPlanes, 2);
    MWPCG10PlateS->DefineSection(0, -G10Layer/2., 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    MWPCG10PlateS->DefineSection(1,  G10Layer/2. , 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    
    //copper plates
    TGeoPgon *MWPCCopperPlateS = new TGeoPgon("MWPCCopperPlateS",0, 360, NofPlanes, 2);
    MWPCCopperPlateS->DefineSection(0, -CopperLayer/2., 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    MWPCCopperPlateS->DefineSection(1,  CopperLayer/2. , 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
	 
    //Volumes
    TGeoVolume *MWPCContainerV = new TGeoVolume("MWPCContainerV", MWPCContainerS);
    MWPCContainerV->SetMedium(pMedAir);
    MWPCContainerV->SetVisibility(kTRUE);
    MWPCContainerV->SetTransparency(50);

    TGeoVolume *MWPCActivePlaneV = new TGeoVolume("MWPCActivePlaneV", MWPCActivePlaneS);
    MWPCActivePlaneV->SetMedium(pMedArCO27030);
    MWPCActivePlaneV->SetLineColor(kBlue);
    MWPCActivePlaneV->SetTransparency(80);
    
    TGeoVolume *MWPCAlumPlateV = new TGeoVolume("MWPCAluminiumPlateV", MWPCAlumPlateS);   
    MWPCAlumPlateV->SetMedium(pMedAluminium);
    MWPCAlumPlateV->SetLineColor(43);
    MWPCAlumPlateV->SetTransparency(60);
    
    TGeoVolume *MWPCAlumCoverV = new TGeoVolume("MWPCAluminiumCoverV", MWPCAlumCoverS);   
    MWPCAlumCoverV->SetMedium(pMedAluminium);
    MWPCAlumCoverV->SetLineColor(43);
    MWPCAlumCoverV->SetTransparency(60);
    
    TGeoVolume *MWPCG10PlateV = new TGeoVolume("MWPCG10PlateV", MWPCG10PlateS);
    MWPCG10PlateV->SetMedium(pMedG10);
    MWPCG10PlateV->SetVisibility(kTRUE);
    MWPCG10PlateV->SetLineColor(43);
    MWPCG10PlateV->SetTransparency(5);
    
    TGeoVolume *MWPCCopperPlateV = new TGeoVolume("MWPCCopperPlateV", MWPCCopperPlateS);
    MWPCCopperPlateV->SetMedium(pMedCopper);
    MWPCCopperPlateV->SetVisibility(kTRUE);
    MWPCCopperPlateV->SetLineColor(43);
    MWPCCopperPlateV->SetTransparency(20);    
    
    for (Int_t iPlane = 1; iPlane <= NofPlanes; ++iPlane) {
        TGeoTranslation t0(0.0, 0.0, GapZsize * (iPlane - (NofPlanes + 1) / 2.0));
        TGeoRotation r0("r0", 0.0, 0.0, (iPlane - 1) * 60.0);
        MWPCContainerV->AddNode(MWPCActivePlaneV, iPlane - 1, new TGeoCombiTrans(t0, r0));
    }
    
    Double_t Y = YSizeOfActiveVolume/2.0 + DGsize/2.0 + 0.1;
    Double_t Y1 = Y + AluminiumLayer/2. + DGsize/2. + 0.2;
   
    for(Int_t iAlPlate = 1;iAlPlate <= 6; ++iAlPlate){
    
      TGeoTranslation t0(-Y*sin((iAlPlate - 1) * M_PI/3.0), Y*cos((iAlPlate - 1) * M_PI/3.0), ZLengthChamber / 2.0);
      TGeoRotation r0("r0", 0.0, 0.0, (iAlPlate-1) * 60.0);
      MWPCContainerV->AddNode(MWPCAlumPlateV,0,new TGeoCombiTrans(t0, r0));
      
      TGeoTranslation t1(-Y*sin((iAlPlate - 1) * M_PI/3.0), Y*cos((iAlPlate - 1) * M_PI/3.0), -ZLengthChamber / 2.0);
      TGeoRotation r1("r1", 0.0, 0.0, (iAlPlate-1) * 60.0);
      MWPCContainerV->AddNode(MWPCAlumPlateV,0,new TGeoCombiTrans(t1, r1));
      
      TGeoTranslation t2(-Y1*sin((iAlPlate - 1) * M_PI/3.0), Y1*cos((iAlPlate - 1) * M_PI/3.0),0);
      TGeoRotation r2("r2", 0.0, 0.0, (iAlPlate-1) * 60.0);
      MWPCContainerV->AddNode(MWPCAlumCoverV,0,new TGeoCombiTrans(t2, r2));
    }
    
    for(Int_t iSide = 0;iSide <= 1; ++iSide){
    	 
      TGeoTranslation t0(0.0, 0.0, (1-2*iSide)*(ZLengthChamber / 2.0 + AluminiumLayer/2. + G10Layer/2.));
      TGeoRotation r0("r0", 0.0, 0.0, 0.0);
      MWPCContainerV->AddNode(MWPCG10PlateV,0,new TGeoCombiTrans(t0, r0));
      
      TGeoTranslation t1(0.0, 0.0, (1-2*iSide)*(ZLengthChamber / 2.0 + AluminiumLayer/2. + G10Layer + CopperLayer/2.));
      TGeoRotation r1("r1", 0.0, 0.0, 0.0);
      MWPCContainerV->AddNode(MWPCCopperPlateV,0,new TGeoCombiTrans(t1, r1));
    }
    
    
    MWPC->AddNode(MWPCContainerV, 0, DetPos0_trans);
    MWPC->AddNode(MWPCContainerV, 0, DetPos1_trans);
    MWPC->AddNode(MWPCContainerV, 0, DetPos2_trans);
    MWPC->AddNode(MWPCContainerV, 0, DetPos3_trans);
    //MWPC->AddNode(MWPCTextPlaneV, 0, DetPosTest_trans);
    //Adding volumes to the TOP Volume
    top->AddNode(MWPC, 0);
    
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
