/**
* class for event management and navigation.
* 06.12.07 M.Al-Turany
*/
#ifndef FairEventManager_H
#define FairEventManager_H

#include "TEveEventManager.h"           // for TEveEventManager

#include "FairRunAna.h"                 // for FairRunAna

#include "Rtypes.h"                     // for Float_t, Int_t, Bool_t, etc

//new
#include <TEveViewer.h>
#include <TGLViewer.h>
#include <TEveScene.h>
#include <TGeoNode.h>
#include <TEveProjectionManager.h>

class FairRootManager; //does not work with streamer, reason unclear
class FairTask;
class TGListTreeItem;

class FairEventManager : public TEveEventManager
{
  public:
    struct structSelectedColoring
    {
        TString detector_name;
        TString detector_color;
        int detector_transparency;
        bool isRecursiveColoring;

        structSelectedColoring()
        {
        }

        structSelectedColoring(TString det_name, TString det_color, int det_transparency, bool isRecursive)
        {
            detector_name = det_name;
            detector_color = det_color;
            detector_transparency = det_transparency;
            isRecursiveColoring = isRecursive;
        }
    };

    struct structLevelColoring
    {
        TString fill_color;
        bool isFillLine;
        bool visibility;
        int transparency;

        structLevelColoring()
        {
        }

        structLevelColoring(TString fill, bool isLine, bool vis, int transp)
        {
            fill_color = fill;
            isFillLine = isLine;
            visibility = vis;
            transparency = transp;
        }
    };

    static FairEventManager* Instance();
    FairEventManager();
    virtual ~FairEventManager();
    virtual void Open();
    virtual void GotoEvent(Int_t event);    // *MENU*
    virtual void NextEvent();               // *MENU*
    virtual void PrevEvent();               // *MENU*
    virtual void Close();
    virtual void DisplaySettings();         //  *Menu*
    virtual Int_t Color(Int_t pdg);
    void AddTask(FairTask* t) {fRunAna->AddTask(t);}
    virtual void Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
    virtual Int_t GetCurrentEvent() {return fEntry;}
    virtual void SetPriOnly(Bool_t Pri) {fPriOnly=Pri;}
    virtual Bool_t IsPriOnly() {return fPriOnly;}
    virtual void SelectPDG(Int_t PDG) {fCurrentPDG=PDG;}
    virtual Int_t GetCurrentPDG() {return fCurrentPDG;}

    virtual void SetMaxEnergy(Float_t max) {fMaxEnergy = max;}
    virtual void SetMinEnergy(Float_t min) {fMinEnergy = min;}
    virtual void SetEvtMaxEnergy(Float_t max) {fEvtMaxEnergy = max;}
    virtual void SetEvtMinEnergy(Float_t min) {fEvtMinEnergy = min;}
    virtual Float_t GetEvtMaxEnergy() {return fEvtMaxEnergy;}
    virtual Float_t GetEvtMinEnergy() {return fEvtMinEnergy;}
    virtual Float_t GetMaxEnergy() {return fMaxEnergy;}
    virtual Float_t GetMinEnergy() {return fMinEnergy;}

    void UpdateEditor();
    virtual void AddParticlesToPdgDataBase(Int_t pdg=0);

    //MultiView features
    void SetDepth(Float_t d);
    void ImportGeomRPhi(TEveElement* el);
    void ImportGeomRhoZ(TEveElement* el);
    void ImportEventRPhi(TEveElement* el);
    void ImportEventRhoZ(TEveElement* el);

    // viewer for RPhi projection
    TEveViewer* fRPhiView;
    // viewer for RPhoZ projection
    TEveViewer* fRhoZView;
    // 3D view in multi-viewer
    TEveViewer* fMulti3DView;
    // RPhi projection in multi-viewer
    TEveViewer* fMultiRPhiView;
    // RPhoZ projection in multi-viewer
    TEveViewer* fMultiRhoZView;

    // projection manager for RPhi view
    TEveProjectionManager* fRPhiMng;
    // projection manager for RPho view
    TEveProjectionManager* fRhoZMng;
    // scene for geometry presentation in RPhi plane
    TEveScene* fRPhiGeomScene;
    // scene for geometry presentation in RPhoZ plane
    TEveScene* fRhoZGeomScene;
    // scene for event presenation in RPhi plane
    TEveScene* fRPhiEventScene;
    // scene for event presenation in RPhoZ plane
    TEveScene* fRhoZEventScene;

    int background_color;

    TEveElementList* EveMCPoints, *EveMCTracks, *EveRecoPoints, *EveRecoTracks;

    ClassDef(FairEventManager,1);
  private:
    FairRootManager* fRootManager; //!
    Int_t fEntry;                 //!
    FairRunAna* fRunAna;          //!
    TGListTreeItem*  fEvent;     //!
    Bool_t fPriOnly;             //!
    Int_t fCurrentPDG;           //!
    // the most minimum particle energy for selected event
    Float_t fMinEnergy;         //!
    // the most maximum particle energy for selected event
    Float_t fMaxEnergy;         //!
    // minimum energy to cut particles by energy in selected event
    Float_t fEvtMinEnergy;      //!
    // maximum energy to cut particles by energy in selected event
    Float_t fEvtMaxEnergy;      //!

    static FairEventManager* fgRinstance; //!

    int cntSelectedColoring;
    structSelectedColoring* arrSelectedColoring; //!
    int cntLevelColoring;
    structLevelColoring* arrLevelColoring;       //!

    FairEventManager(const FairEventManager&);
    FairEventManager& operator=(const FairEventManager&);

    // get color id by color name
    Int_t GetColor(TString colorName);

    // changing color and visibility of geometry nodes
    void InitColorStructure();
    void LevelChangeNodeProperty(TGeoNode* node, int level);
    void SelectedGeometryColoring();
    void RecursiveChangeNodeProperty(TGeoNode* parentNode, Int_t color, int transparency);
};

#endif
