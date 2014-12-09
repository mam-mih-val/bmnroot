//______________________________________________________________________________
/** FairEventManager
*  class for event management and navigation.
*  M. Al-Turany 06.12.2007
**/
#include "FairEventManager.h"

#include "FairRootManager.h"            // for FairRootManager
#include "FairRunAna.h"                 // for FairRunAna

#include "TDatabasePDG.h"               // for TDatabasePDG
#include "TEveGeoNode.h"                // for TEveGeoTopNode
#include "TEveManager.h"                // for TEveManager, gEve
#include "TGeoManager.h"                // for gGeoManager, TGeoManager

#include <TGLCameraOverlay.h>
#include <TGLLightSet.h>
#include <TEveProjectionAxes.h>
#include <TEveBrowser.h>

#include "constants.h"

#include <iostream>
using namespace std;

ClassImp(FairEventManager)

FairEventManager* FairEventManager::fgRinstance= 0;
//_____________________________________________________________________________
FairEventManager* FairEventManager::Instance()
{
  return fgRinstance;
}
//______________________________________________________________________________
FairEventManager::FairEventManager()
  :TEveEventManager("FairEventManager", ""),
   fRootManager(FairRootManager::Instance()),
   fEntry(0),
   fRunAna(FairRunAna::Instance()),
   fEvent(0),
   fPriOnly(kFALSE),
   fCurrentPDG(0),
   fMinEnergy(0),
   fMaxEnergy(25),
   fEvtMinEnergy(0),
   fEvtMaxEnergy(12),
   fRPhiMng(0),
   fRhoZMng(0),
   fMulti3DView(0),
   fMultiRPhiView(0),
   fMultiRhoZView(0),
   fRPhiView(0),
   fRhoZView(0),
   fRPhiGeomScene(0),
   fRhoZGeomScene(0),
   fRPhiEventScene(0),
   fRhoZEventScene(0),
   background_color(1),
   EveMCPoints(NULL),
   EveMCTracks(NULL),
   EveRecoPoints(NULL),
   EveRecoTracks(NULL)
{
    fgRinstance = this;

    AddParticlesToPdgDataBase();

    InitColorStructure();
}

// COLOR SET:
// white, black, gray,
// blue, azure (темно-синий), cyan (морской волны), teal (бирюзовый),
// green, spring (светло-зеленый), green+2 (темно-зеленый), spring+2 (темно-зеленый), khaki
// yellow, orange (желтый с оттенком), orange+2 (оранжевый кор.), orange+1 (светло-оранжевый кор.), orange+7 (выделенно-оранжевый)
// red, violet, magenta (бардовый), magenta-6 (светло-бардовый), pink (темно-розовый)
void FairEventManager::InitColorStructure()
{
    cntSelectedColoring = 0;
    cntLevelColoring = 0;

    if (gVisualizationColoring == levelColoring)
    {
        // number of described detector levels
        cntLevelColoring = 5;
        arrLevelColoring = new structLevelColoring[cntLevelColoring];

        // if BM@N
        if (gCoordinateSystem == sysLaboratory)
        {
            // LEVEL 1
            arrLevelColoring[0].fill_color =    "gray"; //yellow for white background
            arrLevelColoring[0].isFillLine =    true;
            arrLevelColoring[0].visibility =    true;
            arrLevelColoring[0].transparency =  30;

            // LEVEL 2
            arrLevelColoring[1].fill_color =    "yellow"; //magenta for white background
            arrLevelColoring[1].isFillLine =    true;
            arrLevelColoring[1].visibility =    true;
            arrLevelColoring[1].transparency =  30;

            // LEVEL 3
            arrLevelColoring[2].fill_color =    "cyan"; //green for white background
            arrLevelColoring[2].isFillLine =    true;
            arrLevelColoring[2].visibility =    true;
            arrLevelColoring[2].transparency =  30;

            // LEVEL 4
            arrLevelColoring[3].fill_color =    "white"; //cyan for white background
            arrLevelColoring[3].isFillLine =    true;
            arrLevelColoring[3].visibility =    true;
            arrLevelColoring[3].transparency =  30;

            // LEVEL 5
            arrLevelColoring[4].fill_color =    "green"; //blue for white background
            arrLevelColoring[4].isFillLine =    true;
            arrLevelColoring[4].visibility =    true;
            arrLevelColoring[4].transparency =  30;
        }
        // MPD
        else
        {
            // LEVEL 1
            arrLevelColoring[0].fill_color =    "gray"; //yellow for white background
            arrLevelColoring[0].isFillLine =    true;
            arrLevelColoring[0].visibility =    true;
            arrLevelColoring[0].transparency =  30;

            // LEVEL 2
            arrLevelColoring[1].fill_color =    "cyan"; //magenta for white background
            arrLevelColoring[1].isFillLine =    true;
            arrLevelColoring[1].visibility =    true;
            arrLevelColoring[1].transparency =  30;

            // LEVEL 3
            arrLevelColoring[2].fill_color =    "violet"; //green for white background
            arrLevelColoring[2].isFillLine =    true;
            arrLevelColoring[2].visibility =    true;
            arrLevelColoring[2].transparency =  30;

            // LEVEL 4
            arrLevelColoring[3].fill_color =    "azure"; //cyan for white background
            arrLevelColoring[3].isFillLine =    true;
            arrLevelColoring[3].visibility =    true;
            arrLevelColoring[3].transparency =  30;

            // LEVEL 5
            arrLevelColoring[4].fill_color =    "green"; //blue for white background
            arrLevelColoring[4].isFillLine =    true;
            arrLevelColoring[4].visibility =    true;
            arrLevelColoring[4].transparency =  30;
        }

        return;
    }

    // if BM@N selected coloring
    if (gCoordinateSystem == sysLaboratory)
    {
        int i = 0;
        cntSelectedColoring = 11;
        arrSelectedColoring = new structSelectedColoring[cntSelectedColoring];

        // MAGNET color
        arrSelectedColoring[i].detector_name =          "Magnet";
        arrSelectedColoring[i].detector_color =         "gray";
        arrSelectedColoring[i].detector_transparency =  67;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;
        // coil of magnet
        arrSelectedColoring[i].detector_name =          "Coil";
        arrSelectedColoring[i].detector_color =         "red";
        arrSelectedColoring[i].detector_transparency =  67;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // TARGET color
        arrSelectedColoring[i].detector_name =          "targ";
        arrSelectedColoring[i].detector_color =         "yellow";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // PIPE color
        arrSelectedColoring[i].detector_name =          "pipe1cave";
        arrSelectedColoring[i].detector_color =         "orange";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // RECOIL color
        arrSelectedColoring[i].detector_name =          "recoil01";
        arrSelectedColoring[i].detector_color =         "khaki";
        arrSelectedColoring[i].detector_transparency =  67;
        arrSelectedColoring[i].isRecursiveColoring =  true;
        i++;

        // GEMS color
        arrSelectedColoring[i].detector_name =          "GEMS";
        arrSelectedColoring[i].detector_color =         "cyan";
        arrSelectedColoring[i].detector_transparency =  15;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // TOF1 color
        arrSelectedColoring[i].detector_name =          "tof1";
        arrSelectedColoring[i].detector_color =         "spring";
        arrSelectedColoring[i].detector_transparency =  15;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // DCH1 color
        arrSelectedColoring[i].detector_name =          "dch1";
        arrSelectedColoring[i].detector_color =         "orange+7";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // DCH2 color
        arrSelectedColoring[i].detector_name =          "dch2";
        arrSelectedColoring[i].detector_color =         "orange+7";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        // TOF2 color
        arrSelectedColoring[i].detector_name =          "tof2";
        arrSelectedColoring[i].detector_color =         "spring";
        arrSelectedColoring[i].detector_transparency =  15;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;
        // inner part of TOF2
        arrSelectedColoring[i].detector_name =          "t2reg2mod";
        arrSelectedColoring[i].detector_color =         "spring+2";
        //arrSelectedColoring[i].detector_transparency =  29;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;

        /*
        // ZDC color
        arrSelectedColoring[i].detector_name =          "VETO";
        arrSelectedColoring[i].detector_color =         "yellow";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i].isRecursiveColoring =    true;
        i++;
        */
    }
    // if MPD
    else
    {
        int i = 0;
        cntSelectedColoring = 13;
        arrSelectedColoring = new structSelectedColoring[cntSelectedColoring];

        // MAGNET color
        arrSelectedColoring[i].detector_name =          "ms01yokebarrel";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // cryostat
        arrSelectedColoring[i].detector_name =          "ms01cryostat";
        arrSelectedColoring[i].detector_color =         "gray";
        arrSelectedColoring[i].detector_transparency =  15;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // barrel end
        arrSelectedColoring[i].detector_name =          "ms01yokeendeii";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // barrel end
        arrSelectedColoring[i].detector_name =          "ms01yokeendeio";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // barrel end
        arrSelectedColoring[i].detector_name =          "ms01yokeendeim";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // barrel end
        arrSelectedColoring[i].detector_name =          "ms01yokeendeoi";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // barrel end
        arrSelectedColoring[i].detector_name =          "ms01yokeendeom";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
        // barrel end
        arrSelectedColoring[i].detector_name =          "ms01yokeendeoo";
        arrSelectedColoring[i].detector_color =         "blue";
        arrSelectedColoring[i].detector_transparency =  30;
        arrSelectedColoring[i++].isRecursiveColoring =  true;

        // PIPE color
        arrSelectedColoring[i].detector_name =          "pipe1";
        arrSelectedColoring[i].detector_color =         "orange";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i++].isRecursiveColoring =  true;

        // TPC color
        arrSelectedColoring[i].detector_name =          "tpcChamber1";
        arrSelectedColoring[i].detector_color =         "cyan";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i++].isRecursiveColoring =  true;

        // TOF color
        arrSelectedColoring[i].detector_name =          "tof1";
        arrSelectedColoring[i].detector_color =         "spring";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i++].isRecursiveColoring =  true;

        // EMC color
        arrSelectedColoring[i].detector_name =          "emc1Chamber1";
        arrSelectedColoring[i].detector_color =         "violet";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i++].isRecursiveColoring =  true;

        // ZDC color
        arrSelectedColoring[i].detector_name =          "zdc01";
        arrSelectedColoring[i].detector_color =         "green";
        arrSelectedColoring[i].detector_transparency =  0;
        arrSelectedColoring[i++].isRecursiveColoring =  true;
    }

    return;
}

//______________________________________________________________________________
void FairEventManager::Init(Int_t visopt, Int_t vislvl, Int_t maxvisnds)
{
  TEveManager::Create();
  fRunAna->Init();

  // if no gGeoManager in input file - get it from evetest.root directly for BMNRoot and MPDRoot
  if (!gGeoManager)
  {
      std::cout<<"\ngGeoManager is NULL. It's loading manually"<<std::endl;
      TFile* f = NULL;
      if (gCoordinateSystem == sysLaboratory) //BMNRoot
          f = new TFile("$VMCWORKDIR/macro/run/evetest.root");
      else                                    //MPDRoot
          f = new TFile("$VMCWORKDIR/macro/mpd/evetest.root");

      f->Get("FairBaseParSet");
  }

  TGeoNode* N = gGeoManager->GetTopNode();
  TEveGeoTopNode* TNod = new TEveGeoTopNode(gGeoManager, N, visopt, vislvl, maxvisnds);

  // change color and visibility of geometry nodes
  if (gVisualizationColoring != defaultColoring)
  {
    if (gVisualizationColoring == selectedColoring)
        SelectedGeometryColoring();
    else
        LevelChangeNodeProperty(N, 0);
  }

  gEve->AddGlobalElement(TNod);
  gEve->FullRedraw3D(kTRUE);
  fEvent = gEve->AddEvent(this);

  // create projection managers
  fRPhiMng = new TEveProjectionManager();
  fRPhiMng->SetProjection(TEveProjection::kPT_RPhi);
  gEve->AddToListTree(fRPhiMng, kFALSE);

  fRhoZMng = new TEveProjectionManager();
  fRhoZMng->SetProjection(TEveProjection::kPT_RhoZ);
  gEve->AddToListTree(fRhoZMng, kFALSE);

  // create axes for viewers
  TEveProjectionAxes* axes = new TEveProjectionAxes(fRPhiMng);
  axes->SetMainColor(kWhite);

  // first 3D viewer
  gEve->GetDefaultViewer()->SetElementName("3D View");
  // switch off left and right light sources for first window
  gEve->GetDefaultViewer()->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
  gEve->GetDefaultViewer()->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
  gEve->GetDefaultViewer()->GetGLViewer()->SetClearColor(background_color);

  // add window in EventDisplay for RPhi projection
  TEveWindowSlot *RPhiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  TEveWindowPack *RPhiPack = RPhiSlot->MakePack();
  RPhiPack->SetElementName("RPhi View");
  RPhiPack->SetShowTitleBar(kFALSE);
  RPhiPack->NewSlot()->MakeCurrent();
  fRPhiView = gEve->SpawnNewViewer("RPhi View", "");
  fRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  Double_t eqRPhi[4] = {0.0, 0.0, 1.0, 0.0};
  // set clip plane and camera parameters
  fRPhiView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  fRPhiView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRPhi);
  fRPhiView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
  fRPhiView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
  // switch off left, right, top and bottom light sources
  fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
  fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
  fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightTop, false);
  fRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightBottom, false);
  fRPhiView->GetGLViewer()->SetClearColor(background_color);
  // create scene holding projected geometry for the RPhi view
  fRPhiGeomScene  = gEve->SpawnNewScene("RPhi Geometry", "Scene holding projected geometry for the RPhi view.");
  // add axes for scene of RPhi view
  fRPhiGeomScene->AddElement(axes);
  // create scene holding projected event-data for the RPhi view
  fRPhiEventScene = gEve->SpawnNewScene("RPhi Event Data", "Scene holding projected event-data for the RPhi view.");
  // add both scenes to RPhi View
  fRPhiView->AddScene(fRPhiGeomScene);
  fRPhiView->AddScene(fRPhiEventScene);

  // add window in EvenDisplay for RhoZ projection
  TEveWindowSlot *RhoZSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  TEveWindowPack *RhoZPack = RhoZSlot->MakePack();
  RhoZPack->SetElementName("RhoZ View");
  RhoZPack->SetShowTitleBar(kFALSE);
  RhoZPack->NewSlot()->MakeCurrent();
  fRhoZView = gEve->SpawnNewViewer("RhoZ View", "");
  fRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
  Double_t eqRhoZ[4] = {-1.0, 0.0, 0.0, 0.0};
  // set clip plane and camera parameters
  fRhoZView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  fRhoZView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRhoZ);
  fRhoZView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
  fRhoZView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
  // switch off left, right and front light sources
  fRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
  fRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
  fRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightFront, false);
  fRhoZView->GetGLViewer()->SetClearColor(background_color);
  // create scene holding projected geometry for the RhoZ view.
  fRhoZGeomScene  = gEve->SpawnNewScene("RhoZ Geometry", "Scene holding projected geometry for the RhoZ view.");
  // add axes for scene of RPhoZ view
  fRhoZGeomScene->AddElement(axes);
  // create scene holding projected event-data for the RhoZ view
  fRhoZEventScene = gEve->SpawnNewScene("RhoZ Event Data", "Scene holding projected event-data for the RhoZ view.");
  // add both scenes to RhoZView
  fRhoZView->AddScene(fRhoZGeomScene);
  fRhoZView->AddScene(fRhoZEventScene);

  // add window in EvenDisplay for MultiView
  TEveWindowSlot *MultiSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  TEveWindowPack *MultiPack = MultiSlot->MakePack();
  MultiPack->SetElementName("Multi View");
  MultiPack->SetHorizontal();
  MultiPack->SetShowTitleBar(kFALSE);
  MultiPack->NewSlot()->MakeCurrent();
  fMulti3DView = gEve->SpawnNewViewer("3D View (multi)", "");
  // switch off left and right light sources for 3D MultiView
  fMulti3DView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
  fMulti3DView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
  fMulti3DView->GetGLViewer()->SetClearColor(background_color);
  // add 3D scenes (first tab) to 3D MultiView
  fMulti3DView->AddScene(gEve->GetGlobalScene());
  fMulti3DView->AddScene(gEve->GetEventScene());

  // add slot for RPhi projection on Multi View tab
  MultiPack = MultiPack->NewSlot()->MakePack();
  MultiPack->SetShowTitleBar(kFALSE);
  MultiPack->NewSlot()->MakeCurrent();
  fMultiRPhiView = gEve->SpawnNewViewer("RPhi View (multi)", "");
  fMultiRPhiView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  // set clip plane and camera parameters
  fMultiRPhiView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  fMultiRPhiView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRPhi);
  fMultiRPhiView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
  fMultiRPhiView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
  // switch off left, right, top and bottom light sources
  fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
  fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
  fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightTop, false);
  fMultiRPhiView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightBottom, false);
  fMultiRPhiView->GetGLViewer()->SetClearColor(background_color);
  // add RPhi scenes (second tab) to RPhi MultiView
  fMultiRPhiView->AddScene(fRPhiGeomScene);
  fMultiRPhiView->AddScene(fRPhiEventScene);

  // add slot for RhoZ projection on Multi View tab
  MultiPack->NewSlot()->MakeCurrent();
  fMultiRhoZView = gEve->SpawnNewViewer("RhoZ View (multi)", "");
  fMultiRhoZView->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoZOY);
  // set clip plane and camera parameters
  fMultiRhoZView->GetGLViewer()->GetClipSet()->SetClipType(TGLClip::kClipPlane);
  fMultiRhoZView->GetGLViewer()->GetClipSet()->SetClipState(TGLClip::kClipPlane, eqRhoZ);
  fMultiRhoZView->GetGLViewer()->GetCameraOverlay()->SetOrthographicMode(TGLCameraOverlay::kAxis);
  fMultiRhoZView->GetGLViewer()->GetCameraOverlay()->SetShowOrthographic(kTRUE);
  // switch off left, right and front light sources
  fMultiRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightLeft, false);
  fMultiRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightRight, false);
  fMultiRhoZView->GetGLViewer()->GetLightSet()->SetLight(TGLLightSet::kLightFront, false);
  fMultiRhoZView->GetGLViewer()->SetClearColor(background_color);
  // add RhoZ scenes (second tab) to RhoZ MultiView
  fMultiRhoZView->AddScene(fRhoZGeomScene);
  fMultiRhoZView->AddScene(fRhoZEventScene);

  // copy geometry and event scene for RPhi and RhoZ views from global scene (3D)
  fRPhiGeomScene->AddElement(gEve->GetGlobalScene());
  fRPhiEventScene->AddElement(gEve->GetEventScene());
  fRhoZGeomScene->AddElement(gEve->GetGlobalScene());
  fRhoZEventScene->AddElement(gEve->GetEventScene());

  // update all scenes
  fRPhiView->GetGLViewer()->UpdateScene(kTRUE);
  fRhoZView->GetGLViewer()->UpdateScene(kTRUE);
  fMulti3DView->GetGLViewer()->UpdateScene(kTRUE);
  fMultiRPhiView->GetGLViewer()->UpdateScene(kTRUE);
  fMultiRhoZView->GetGLViewer()->UpdateScene(kTRUE);

  // don't change reposition camera on each update
  fRPhiView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
  fRhoZView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
  fMulti3DView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
  fMultiRPhiView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
  fMultiRhoZView->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
}

// changing of geometry color
void FairEventManager::SelectedGeometryColoring()
{
    TGeoVolume* curVolume;
    for (int i = 0; i < cntSelectedColoring; i++)
    {
        curVolume = gGeoManager->GetVolume(arrSelectedColoring[i].detector_name);
        if (!curVolume)
        {
            cout<<"There is no volume with given name: "<< arrSelectedColoring[i].detector_name<<endl;
            continue;
        }
        Int_t curColor = GetColor(arrSelectedColoring[i].detector_color);
        Int_t curTransparency = arrSelectedColoring[i].detector_transparency;

        curVolume->SetFillColor(curColor);
        curVolume->SetLineColor(curColor);
        curVolume->SetTransparency(curTransparency);

        if (arrSelectedColoring[i].isRecursiveColoring)
        {
            for (int j = 0; j < curVolume->GetNdaughters(); j++)
            {
                TGeoNode* child = curVolume->GetNode(j);
                TGeoVolume* subVolume = child->GetVolume();

                subVolume->SetFillColor(curColor);
                subVolume->SetLineColor(curColor);
                subVolume->SetTransparency(curTransparency);

                if (child->GetNdaughters() != 0)
                    RecursiveChangeNodeProperty(child, curColor, curTransparency);
            }
        }
    }

    return;
}

void FairEventManager::RecursiveChangeNodeProperty(TGeoNode* node, Int_t color, int transparency)
{
    for(int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        TGeoVolume* curVolume = child->GetVolume();

        curVolume->SetFillColor(color);
        curVolume->SetLineColor(color);
        curVolume->SetTransparency(transparency);

        if (child->GetNdaughters() != 0)
            RecursiveChangeNodeProperty(child, color, transparency);
    }
}

// hierarchical changing of nodes' properties: visibility, transparency, fill color and line color
void FairEventManager::LevelChangeNodeProperty(TGeoNode* node, int level)
{
    for(int i = 0; i < node->GetNdaughters(); i++)
    {
        TGeoNode* child = node->GetDaughter(i);
        if (level < cntLevelColoring)
        {
            TGeoVolume* curVolume = child->GetVolume();

            curVolume->SetVisibility(arrLevelColoring[level].visibility);
            curVolume->SetTransparency(arrLevelColoring[level].transparency);
            curVolume->SetFillColor(GetColor(arrLevelColoring[level].fill_color));
            if (arrLevelColoring[level].isFillLine) curVolume->SetLineColor(GetColor(arrLevelColoring[level].fill_color));

            if (child->GetNdaughters() != 0)
            {
                level++;
                LevelChangeNodeProperty(child, level);
            }
        }//if (level < arr_size)
    }
}

// return integer value of color by color name (default, blue)
// support following colors:
// white, black, gray,
// green, spring,
// blue, cyan (бирюзовый), azure, teal,
// red, pink (розовый), magenta, violet (фиолетовый),
// yellow, orange
Int_t FairEventManager::GetColor(TString colorName)
{
    colorName.ToLower();

    if (colorName == "white") return 0;
    else if (colorName == "black") return 1;
    else if (colorName == "gray") return 920;
    else if (colorName == "blue") return 600;
    else if (colorName == "red") return 632;
    else if (colorName == "green") return 416;
    else if (colorName == "yellow") return 400;
    else if (colorName == "magenta") return 616;
    else if (colorName == "cyan") return 432;
    else if (colorName == "orange") return 800;
    else if (colorName == "pink") return 900;
    else if (colorName == "violet") return 880;
    else if (colorName == "azure") return 860;
    else if (colorName == "teal") return 840;
    else if (colorName == "spring") return 820;

    else if (colorName == "green+2") return 418;
    else if (colorName == "spring+2") return 823;
    else if (colorName == "orange+1") return 801;
    else if (colorName == "orange+2") return 802;
    else if (colorName == "orange+7") return 807;
    else if (colorName == "magenta-6") return 610;
    else if (colorName == "khaki") return 403;
    else
    {
        std::cout<<colorName<<" not found. Color set to default blue" << std::endl;
        return 600;
    }
}

//______________________________________________________________________________
void FairEventManager::SetDepth(Float_t d)
{
    fRPhiMng->SetCurrentDepth(d);
    fRhoZMng->SetCurrentDepth(d);
}
//______________________________________________________________________________
void FairEventManager::ImportGeomRPhi(TEveElement* el)
{
    fRPhiMng->ImportElements(el, fRPhiGeomScene);
}
//______________________________________________________________________________
void FairEventManager::ImportGeomRhoZ(TEveElement* el)
{
    fRhoZMng->ImportElements(el, fRhoZGeomScene);
}
//______________________________________________________________________________
void FairEventManager::ImportEventRPhi(TEveElement* el)
{
    fRPhiMng->ImportElements(el, fRPhiEventScene);
}
//______________________________________________________________________________
void FairEventManager::ImportEventRhoZ(TEveElement* el)
{
    fRhoZMng->ImportElements(el, fRhoZEventScene);
}

//______________________________________________________________________________
void FairEventManager::UpdateEditor()
{
}

// FairEventManager destructor (empty now)
FairEventManager::~FairEventManager()
{
}

//______________________________________________________________________________
void FairEventManager::Open()
{
}

// go to FairRunAna event with given number for scene data getting
void FairEventManager::GotoEvent(Int_t event)
{
  fEntry=event;
  fRunAna->Run((Long64_t)event);
}
// go to next FairRunAna event for scene data getting
void FairEventManager::NextEvent()
{
  fEntry+=1;
  fRunAna->Run((Long64_t)fEntry);
}
// go to previous FairRunAna event for scene data getting
void FairEventManager::PrevEvent()
{
  fEntry-=1;
  fRunAna->Run((Long64_t)fEntry);
}

//______________________________________________________________________________
void FairEventManager::Close()
{
}

//______________________________________________________________________________

void FairEventManager::DisplaySettings()
{
}

// return integer value of color for track by particle pdg (default, white)
Int_t FairEventManager::Color(int pdg)
{
  switch(pdg) {
  case   22     :
    return  623;    // photon
  case   -2112  :
    return  2 ;   // anti-neutron
  case   -11    :
    return  3;    // e+
  case   -3122  :
    return  4;   // anti-Lambda
  case   11     :
    return  5;    // e-
  case   -3222  :
    return  6;   // Sigma-
  case   12     :
    return  7;    // e-neutrino (NB: flavour undefined by Geant)
  case   -3212  :
    return  8;   // Sigma0
  case   -13    :
    return  9;    // mu+
  case   -3112  :
    return  10;   // Sigma+ (PB)*/
  case   13     :
    return  11;    // mu-
  case   -3322  :
    return  12;   // Xi0
  case   111    :
    return  13;    // pi0
  case   -3312  :
    return  14;   // Xi+
  case   211    :
    return  15;    // pi+
  case   -3334  :
    return  16;   // Omega+ (PB)
  case   -211   :
    return  17;    // pi-
  case   -15    :
    return  18;   // tau+
  case   130    :
    return  19;   // K long
  case   15     :
    return  20;   // tau-
  case   321    :
    return  21;   // K+
  case   411    :
    return  22;   // D+
  case   -321   :
    return  23;   // K-
  case   -411   :
    return  24;   // D-
  case   2112   :
    return  25;   // n
  case   421    :
    return  26;   // D0
  case   2212   :
    return  27;   // p
  case   -421   :
    return  28;   // D0
  case   -2212  :
    return  29;   // anti-proton
  case   431    :
    return  30;   // Ds+
  case   310    :
    return  31;   // K short
  case   -431   :
    return  32;   // anti Ds-
  case   221    :
    return  33;   // eta
  case   4122   :
    return  34;   // Lamba_c+
  case   3122   :
    return  35;   // Lambda
  case   24     :
    return  36;   // W+
  case   3222   :
    return  37;   // Sigma+
  case   -24    :
    return  38;   // W-
  case   3212   :
    return  39;   // Sigma0
  case   23     :
    return  40;   // Z
  case   3112   :
    return  41;   // Sigma-
  case   3322   :
    return  42;   // Xi0
  case   3312   :
    return  43;   // Xi-
  case   3334   :
    return  44;   // Omega- (PB)
  case   50000050   :
    return  801;   // Cerenkov
  case   1000010020  :
    return  45;
  case   1000010030  :
    return  48;
  case   1000020040   :
    return  50;
  case   1000020030   :
    return  55;
  default  :
    return 0;
  }//switch
}

// add particles to the PDG data base: Deuteron, Triton, Alpha, HE3; Cherenkov, FeedbackPhoton
void FairEventManager::AddParticlesToPdgDataBase(Int_t pdg)
{
  TDatabasePDG* pdgDB = TDatabasePDG::Instance();

  const Double_t kAu2Gev=0.9314943228;
  const Double_t khSlash = 1.0545726663e-27;
  const Double_t kErg2Gev = 1/1.6021773349e-3;
  const Double_t khShGev = khSlash*kErg2Gev;
  const Double_t kYear2Sec = 3600*24*365.25;

  // Ions
  if ( !pdgDB->GetParticle(1000010020) )
    pdgDB->AddParticle("Deuteron","Deuteron",2*kAu2Gev+8.071e-3,kTRUE,
                       0,3,"Ion",1000010020);

  if ( !pdgDB->GetParticle(1000010030) )
    pdgDB->AddParticle("Triton","Triton",3*kAu2Gev+14.931e-3,kFALSE,
                       khShGev/(12.33*kYear2Sec),3,"Ion",1000010030);

  if ( !pdgDB->GetParticle(1000020040) )
    pdgDB->AddParticle("Alpha","Alpha",4*kAu2Gev+2.424e-3,kTRUE,
                       khShGev/(12.33*kYear2Sec),6,"Ion",1000020040);

  if ( !pdgDB->GetParticle(1000020030) )
    pdgDB->AddParticle("HE3","HE3",3*kAu2Gev+14.931e-3,kFALSE,
                       0,6,"Ion",1000020030);

   // Special particles
  if ( !pdgDB->GetParticle(50000050) )
    pdgDB->AddParticle("Cherenkov","Cherenkov",0,kFALSE,
                       0,0,"Special",50000050);

  if ( !pdgDB->GetParticle(50000051) )
    pdgDB->AddParticle("FeedbackPhoton","FeedbackPhoton",0,kFALSE,
                       0,0,"Special",50000051);
}
