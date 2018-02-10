/**
 * @brief 
 * Macro for creating geometry root-file for MWPC
 * @author Maria Patsyuk
 * @date 15.02.2018
 * 
 */

#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
//#include "BmnMwpcGeometry.h"

using namespace TMath;

TGeoManager* gGeoMan = NULL;

void create_rootgeom_MWPC_SRC2018() {
    
    // Load the necessary FairRoot libraries 
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    BmnMwpcGeometrySRC* mwpcGeo = new BmnMwpcGeometrySRC();
    
    //Number of active planes with wires
    const Int_t NofPlanes = mwpcGeo->GetNPlanes();

    //Detector's position
    const Double_t MWPC0_Xpos = mwpcGeo->GetChamberCenter(0).X();
    const Double_t MWPC0_Ypos = mwpcGeo->GetChamberCenter(0).Y();
    const Double_t MWPC0_Zpos = mwpcGeo->GetChamberCenter(0).Z();
    
    const Double_t MWPC1_Xpos = mwpcGeo->GetChamberCenter(1).X();
    const Double_t MWPC1_Ypos = mwpcGeo->GetChamberCenter(1).Y();
    const Double_t MWPC1_Zpos = mwpcGeo->GetChamberCenter(1).Z();          

    const Double_t MWPC2_Xpos = mwpcGeo->GetChamberCenter(2).X();
    const Double_t MWPC2_Ypos = mwpcGeo->GetChamberCenter(2).Y();
    const Double_t MWPC2_Zpos = mwpcGeo->GetChamberCenter(2).Z();
    
    const Double_t MWPC3_Xpos = mwpcGeo->GetChamberCenter(3).X();
    const Double_t MWPC3_Ypos = mwpcGeo->GetChamberCenter(3).Y();
    const Double_t MWPC3_Zpos = mwpcGeo->GetChamberCenter(3).Z();          

    //           2   
    //        A______B
    //       / \     |\
    //   3  /   \    | \  1
    //     /     \   |  \   
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


    //Detector's construct parameters   
    const Double_t ABsize = mwpcGeo->GetPlaneWidth(); //cm
    const Double_t BCsize = mwpcGeo->GetPlaneHeight(); //cm
    const Double_t GapZsize = mwpcGeo->GetPlaneStep(); //cm

    const Double_t XSizeOfActiveVolume = ABsize;
    const Double_t YSizeOfActiveVolume = BCsize;
    const Double_t ZSizeOfActiveVolume = GapZsize * NofPlanes; // cm

    //FIXME!!! make sure! sizes of frame
    const Double_t ZLengthChamber = ZSizeOfActiveVolume + 1.0;

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
    const TString geoDetectorVersion = "SRC2018";
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
    MWPCContainerS->DefineSection(0, -ZLengthChamber / 2.0, 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    MWPCContainerS->DefineSection(1, ZLengthChamber / 2.0, 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    
    //active wire plane
    TGeoBBox *MWPCActivePlaneS = new TGeoBBox("MWPCActivePlaneS", XSizeOfActiveVolume / 2.0, YSizeOfActiveVolume / 2.0, 0.1);

    //Volumes
    TGeoVolume *MWPCContainerV = new TGeoVolume("MWPCContainerV", MWPCContainerS);
    MWPCContainerV->SetMedium(pMedAir);
    MWPCContainerV->SetVisibility(kTRUE);
    MWPCContainerV->SetTransparency(75);

    TGeoVolume *MWPCActivePlaneV = new TGeoVolume("MWPCActivePlaneV", MWPCActivePlaneS);
    MWPCActivePlaneV->SetMedium(pMedArCO27030);
    MWPCActivePlaneV->SetLineColor(kBlue);
    MWPCActivePlaneV->SetTransparency(40);
    
    for (Int_t iPlane = 1; iPlane <= NofPlanes; ++iPlane) {
        TGeoTranslation t0(0.0, 0.0, GapZsize * (iPlane - (NofPlanes + 1) / 2.0));
        TGeoRotation r0("r0", 0.0, 0.0, (iPlane - 1) * 60.0);
        MWPCContainerV->AddNode(MWPCActivePlaneV, iPlane - 1, new TGeoCombiTrans(t0, r0));
    }
    
    MWPC->AddNode(MWPCContainerV, 0, DetPos0_trans);
    MWPC->AddNode(MWPCContainerV, 0, DetPos1_trans);
    MWPC->AddNode(MWPCContainerV, 0, DetPos2_trans);
    MWPC->AddNode(MWPCContainerV, 0, DetPos3_trans);
    
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
