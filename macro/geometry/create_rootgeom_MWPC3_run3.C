/**
 * @brief
 * Macro for creating geometry root-file describing MWPC3
 * the first BM@N run (<<Shanghai>>)
 * @author Sergey Merts
 * @date 15.10.2014
 *
*/

#include "TGeoManager.h"
#include "TFile.h"
#include "TString.h"
#include "TMath.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"

using namespace TMath;

TGeoManager* gGeoMan = NULL;

//Number of active planes with wires
const Int_t NofPlanes = 6;

//Detector's position
const Double_t MWPC3_Xpos = 0.0;
const Double_t MWPC3_Ypos = 0.0;
const Double_t MWPC3_Zpos = -150.0;//-180.5;

//           x
//        A______B
//       / \     |\
//  v'  /   \    | \  u
//     /     \   |  \
//     \      \  |  /
//  u'  \      \ | /  v
//       \______\|/
//                C
//           x'


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
const Double_t ABsize = 24.6; //cm
const Double_t BCsize = 43.3; //cm
const Double_t ACsize = 49.8; //cm
const Double_t GapZsize = 1.0; //cm
const Double_t GapWireOnePlane = 0.25; //cm
const Double_t GapWireTwoPlanes = GapWireOnePlane / 2.0; //cm // 0.125

const Double_t XSizeOfActiveVolume = ABsize;
const Double_t YSizeOfActiveVolume = BCsize;
const Double_t ZSizeOfActiveVolume = GapZsize * NofPlanes; // cm

const Double_t InnerRadiusOfHexagon = ABsize / 2.0 * Sqrt(3.0);

//FIXME!!! make sure! sizes of frame
const Double_t XWidthChamber  = XSizeOfActiveVolume + 1.0;
const Double_t YHeightChamber = YSizeOfActiveVolume + 1.0;
const Double_t ZLengthChamber = ZSizeOfActiveVolume + 1.0;

void create_rootgeom_MWPC3_run3() {

    // Load the necessary FairRoot libraries
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    // -------   Load media from media file   -----------------------------------
    FairGeoLoader*    geoLoad = new FairGeoLoader("TGeo","FairGeoLoader");
    FairGeoInterface* geoFace = geoLoad->getGeoInterface();
    TString geoPath = TString(gSystem->Getenv("VMCWORKDIR")) + "/geometry/";
    TString medFile = geoPath + "media.geo";
    geoFace->setMediaFile(medFile);
    geoFace->readMedia();
    gGeoMan = gGeoManager;
    // --------------------------------------------------------------------------

    // -------   Geometry file name (output)   ----------------------------------
    const TString geoDetectorName = "mwpc3";
    const TString geoDetectorVersion = "v1_run3";
    TString geoFileName = geoPath + geoDetectorName + "_"+ geoDetectorVersion + ".root";
    // --------------------------------------------------------------------------

    // -----------------   Get and create the required media    -----------------
    FairGeoMedia*   geoMedia = geoFace->getMedia();
    FairGeoBuilder* geoBuild = geoLoad->getGeoBuilder();

    FairGeoMedium* mAir = geoMedia->getMedium("air");
    if ( ! mAir ) Fatal("Main", "FairMedium air not found");
    geoBuild->createMedium(mAir);
    TGeoMedium* pMedAir = gGeoMan->GetMedium("air");
    if ( ! pMedAir ) Fatal("Main", "Medium air not found");

    FairGeoMedium* mArCO27030 = geoMedia->getMedium("arco27030");
    if ( ! mArCO27030 ) Fatal("Main", "FairMedium arco27030 not found");
    geoBuild->createMedium(mArCO27030);
    TGeoMedium* pMedArCO27030 = gGeoMan->GetMedium("arco27030");
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
    TGeoVolume* MWPC3Top = new TGeoVolumeAssembly(geoDetectorName);
    MWPC3Top->SetMedium(pMedAir);
    MWPC3Top->SetTransparency(50);
    //Transformations (translations, rotations and scales)
    TGeoTranslation *DetPos_trans = new TGeoTranslation("DetPos_trans", MWPC3_Xpos, MWPC3_Ypos, MWPC3_Zpos);

    //Solids (shapes)
    //hexagon which contains active wire planes
    TGeoPgon *MWPC3ContainerS = new TGeoPgon("MWPC3ContainerS", 0, 360, 6, 2);
    MWPC3ContainerS->DefineSection(0, -ZLengthChamber / 2.0, 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    MWPC3ContainerS->DefineSection(1, ZLengthChamber / 2.0, 0.0, YSizeOfActiveVolume / 2.0 + 1.0);
    //active wire plane
    TGeoBBox *MWPC3ActivePlaneS = new TGeoBBox("MWPC3ActivePlaneS", XSizeOfActiveVolume / 2.0, YSizeOfActiveVolume / 2.0, 0.1/*GapZsize / 2.0*/);

    //Volumes
    TGeoVolume *MWPC3ContainerV = new TGeoVolume("MWPC3ContainerV", MWPC3ContainerS);
    MWPC3ContainerV->SetMedium(pMedAir);
    MWPC3ContainerV->SetVisibility(kTRUE);
    MWPC3ContainerV->SetTransparency(60);

    TGeoVolume *MWPC3ActivePlaneV = new TGeoVolume("MWPC3ActivePlaneV", MWPC3ActivePlaneS);
    MWPC3ActivePlaneV->SetMedium(pMedArCO27030);
    MWPC3ActivePlaneV->SetLineColor(kBlue);
    MWPC3ActivePlaneV->SetTransparency(40);

    //Adding volumes to the TOP Volume
    top->AddNode(MWPC3Top, 1, DetPos_trans);
    MWPC3Top->AddNode(MWPC3ContainerV, 1);
    for (Int_t iPlane = 1; iPlane <= NofPlanes; ++iPlane) {
        TGeoTranslation t1(0.0, 0.0, GapZsize * (iPlane - (NofPlanes + 1) / 2.0));
        TGeoRotation r1("r1", 0.0, 0.0, (iPlane - 1) * 60.0);
        MWPC3ContainerV->AddNode(MWPC3ActivePlaneV, iPlane, new TGeoCombiTrans(t1, r1));
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