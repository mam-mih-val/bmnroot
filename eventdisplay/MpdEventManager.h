//MpdEventManager: class for event management and navigation

#ifndef MpdEventManager_H
#define MpdEventManager_H

#include "FairRunAna.h"
#include "FairTask.h"
#include "FairXMLNode.h"

#include <TEveViewer.h>
#include "TEveEventManager.h"
#include <TEveScene.h>
#include <TGeoNode.h>
#include <TEveProjectionManager.h>
#include "TGListTree.h"
#include <TEveProjectionAxes.h>

#include <vector>
using namespace std;

enum ElementList {MCPointList, MCTrackList, RecoPointList, RecoTrackList};  // enum for Event Element lists
class MpdEventManagerEditor;

class MpdEventManager : public TEveEventManager
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

    static MpdEventManager* Instance();
    MpdEventManager();
    virtual ~MpdEventManager();
    virtual void SetXMLConfig(TString xml_config){fXMLConfig = xml_config;};
    virtual void Open();
    virtual void GotoEvent(Int_t event);    // *MENU*
    virtual void NextEvent();               // *MENU*
    virtual void PrevEvent();               // *MENU*
    virtual void DisplaySettings();         // *Menu*
    virtual void Close();

    virtual void Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
    void UpdateEditor();

    // assign different colors for differrent particles
    // return integer value of color for track by particle pdg (default, white)
    virtual Int_t Color(Int_t pdg);
    virtual void AddParticlesToPdgDataBase( Int_t pdg = 0 );

    virtual Int_t GetCurrentEvent() { return iCurrentEvent; }
    virtual void SetPriOnly(Bool_t Pri) { fPriOnly = Pri; }
    virtual Bool_t IsPriOnly() { return fPriOnly; }
    virtual void SelectPDG(Int_t PDG) { fCurrentPDG = PDG; }
    virtual Int_t GetCurrentPDG() { return fCurrentPDG; }
    virtual void SetCurrentEvent(Int_t event_number) { iCurrentEvent = event_number; }
    virtual void SetMaxEnergy(Float_t max) { fMaxEnergy = max; }
    virtual void SetMinEnergy(Float_t min) { fMinEnergy = min; }
    virtual void SetEvtMaxEnergy(Float_t max) { fEvtMaxEnergy = max; }
    virtual void SetEvtMinEnergy(Float_t min) { fEvtMinEnergy = min; }
    virtual Float_t GetEvtMaxEnergy() { return fEvtMaxEnergy; }
    virtual Float_t GetEvtMinEnergy() { return fEvtMinEnergy; }
    virtual Float_t GetMaxEnergy() { return fMaxEnergy; }
    virtual Float_t GetMinEnergy() { return fMinEnergy; }
    virtual void SetRPhiPlane(Double_t a, Double_t b, Double_t c, Double_t d);
    virtual void SetRhoZPlane(Double_t a, Double_t b, Double_t c, Double_t d);

    TEveScene* GetRhoZScene() {return fRhoZGeomScene;};
    TEveScene* GetRPhiScene() {return fRPhiGeomScene;};

    void SetEventEditor(MpdEventManagerEditor* event_editor) { fEventEditor = event_editor; }
    MpdEventManagerEditor* GetEventEditor() { return fEventEditor; }

    // background color of EVE Viewers
    int background_color;
    // whether background color is dark
    bool isDarkColor;
    // whether Online of Offline mode
    bool isOnline;
    // data source: 0 - simulation data; 1 - raw detector data
    int iDataSource;

    // Event Elements of Event Scene
    TEveElementList* EveMCPoints, *EveMCTracks, *EveRecoPoints, *EveRecoTracks;
    void AddEventElement(TEveElement* element, ElementList element_list);

    // ZDC module visibility flags. NULL if there are no ZDC modules to show
    bool* isZDCModule; //!
    // current value of "reco points" checkbox - FOR CALORIMETER TOWERS
    bool fgShowRecoPointsIsShow;
    // require event redraw after "reco points" checkbox value is changed - FOR CALORIMETER TOWERS
    bool fgRedrawRecoPointsReqired;

    // set high transparency for detector geometry
    void SetTransparentGeometry(bool is_on);
    void RecursiveChangeNodeTransparent(TGeoNode* parentNode, int transparency);

    // FairRunAna to init and to execute visualization tasks
    FairRunAna* fRunAna; //!
    void AddTask(FairTask* t) { fRunAna->AddTask(t); }

    //returns loaded xml if successful of NULL if validation failed
    bool ValidateXml(const char *XMLFileName, const char *XSDFileName);
    //coloring method
    enum VisualizationColoring {selectedColoring, levelColoring, defaultColoring};
    VisualizationColoring gVisualizationColoring;

 protected:
  TEveViewer* GetRPhiView() const {return fRPhiView;};
  TEveViewer *GetRhoZView() const {return fRhoZView;};
  TEveViewer *GetMultiView() const {return fMulti3DView;};
  TEveViewer *GetMultiRPhiView() const { return fMultiRPhiView;};
  TEveViewer *GetMultiRhoZView() const {return fMultiRhoZView;};
  TEveProjectionManager* GetRhoZProjManager() const {return fRhoZMng;};
  TEveProjectionManager* GetRPhiProjManager() const {return fRPhiMng;};
  TEveProjectionAxes *GetRPhiAxes() const { return fAxesPhi;};
  TEveProjectionAxes *GetRhoZAxes() const {return fAxesRho;};
  virtual void LoadXMLSettings();
  void LoadXMLDetector(TGeoNode *node, FairXMLNode *xml, Int_t depth=0);
  Int_t StringToColor(TString color) const;

  private:
    MpdEventManagerEditor* fEventEditor; //!

    Double_t fRPhiPlane[4];	//!
    Double_t fRhoZPlane[4]; //!
    // VIEWERS for RPhi and RPhoZ projections, 3D view in multi-viewer, RPhi and RPhoZ projections in multi-viewer
    TEveViewer *fRPhiView, *fRhoZView, *fMulti3DView, *fMultiRPhiView, *fMultiRhoZView;
    // projection manager for RPhi view
    TEveProjectionManager* fRPhiMng;
    // projection manager for RPho view
    TEveProjectionManager* fRhoZMng;
    // scene for geometry presentation in RPhi plane
    TEveScene* fRPhiGeomScene;
    // scene for geometry presentation in RPhoZ plane
    TEveScene* fRhoZGeomScene;
    // scene for event presenation in RPhi plane
    //TEveScene* fRPhiEventScene;
    // scene for event presenation in RPhoZ plane
    //TEveScene* fRhoZEventScene;
    TEveProjectionAxes *fAxesPhi;
    TEveProjectionAxes *fAxesRho;

    TGListTreeItem* fEvent; //!
    // current event number
    Int_t iCurrentEvent; //!
    Bool_t fPriOnly; //!
    Int_t fCurrentPDG; //!
    // the most minimum particle energy for selected event
    Float_t fMinEnergy; //!
    // the most maximum particle energy for selected event
    Float_t fMaxEnergy; //!
    // minimum energy to cut particles by energy in selected event
    Float_t fEvtMinEnergy; //!
    // maximum energy to cut particles by energy in selected event
    Float_t fEvtMaxEnergy; //!

    // the last color indice of Color Creating from rgb triple
    Int_t fLastUsedColor; //!

    TString fXMLConfig;
    map<int,int> fPDGToColor;

    // arrays with color sturctures for detector and hierarchical coloring
    vector<structSelectedColoring*> vecSelectedColoring; //!
    vector<structLevelColoring*> vecLevelColoring; //!

    // skeleton Singleton Instance
    static MpdEventManager* fgRinstance; //!
    MpdEventManager(const MpdEventManager&);
    MpdEventManager& operator=(const MpdEventManager&);
    void SetViewers(TEveViewer* RPhi, TEveViewer* RhoZ);

    // get color id by color name
    Int_t GetColor(TString colorName);

    // changing color and visibility of geometry nodes
    void InitColorStructure();
    void LevelChangeNodeProperty(TGeoNode* node, int level);
    void SelectedGeometryColoring();
    void RecursiveChangeNodeProperty(TGeoNode* parentNode, Int_t color, int transparency);

    ClassDef(MpdEventManager,1);
};

#endif
