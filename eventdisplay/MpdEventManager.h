//MpdEventManager: class for event management and navigation

#ifndef MPDEVENTMANAGER_H
#define MPDEVENTMANAGER_H

#include "FairRunAna.h"
#include "FairTask.h"
#include "MpdXMLNode.h"

#include <TEveViewer.h>
#include "TEveEventManager.h"
#include <TEveScene.h>
#include <TGeoNode.h>
#include <TEveProjectionManager.h>
#include "TGListTree.h"
#include <TEveProjectionAxes.h>

#include <vector>
#include <unordered_set>

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

    virtual void Open();
    virtual void GotoEvent(Int_t event);    // *MENU*
    virtual void NextEvent();               // *MENU*
    virtual void PrevEvent();               // *MENU*
    virtual void DisplaySettings();         // *Menu*
    virtual void Close();
    void UpdateEditor();

    virtual void Init(Int_t visopt = 1, Int_t vislvl = 3, Int_t maxvisnds = 10000);
    void SetEventEditor(MpdEventManagerEditor* event_editor) { fEventEditor = event_editor; }
    MpdEventManagerEditor* GetEventEditor() { return fEventEditor; }

    // FairRunAna to init and to execute visualization tasks
    FairRunAna* fRunAna; //!
    void AddTask(FairTask* t) { fRunAna->AddTask(t); }

    // assign different colors for differrent particles
    // return integer value of color for track by particle pdg (default, white)
    virtual Int_t Color(Int_t pdg);
    virtual void AddParticlesToPdgDataBase();

    virtual void SetCurrentEvent(Int_t event_number) { iCurrentEvent = event_number; }
    virtual Int_t GetCurrentEvent() { return iCurrentEvent; }

    virtual void SetPriOnly(Bool_t Pri) { fPriOnly = Pri; }
    virtual Bool_t IsPriOnly() { return fPriOnly; }

    virtual void SetMaxEnergy(Float_t max) { fMaxEnergy = max; }
    virtual void SetMinEnergy(Float_t min) { fMinEnergy = min; }
    virtual void SetEvtMaxEnergy(Float_t max) { fEvtMaxEnergy = max; }
    virtual void SetEvtMinEnergy(Float_t min) { fEvtMinEnergy = min; }
    virtual Float_t GetEvtMaxEnergy() { return fEvtMaxEnergy; }
    virtual Float_t GetEvtMinEnergy() { return fEvtMinEnergy; }
    virtual Float_t GetMaxEnergy() { return fMaxEnergy; }
    virtual Float_t GetMinEnergy() { return fMinEnergy; }

    TEveScene* GetRPhiScene() { return fRPhiGeomScene; }
    TEveScene* GetRhoZScene() { return fRhoZGeomScene; }
    virtual void SetRPhiPlane(Double_t a, Double_t b, Double_t c, Double_t d)
    { fRPhiPlane[0] = a; fRPhiPlane[1] = b; fRPhiPlane[2] = c; fRPhiPlane[3] = d; }
    virtual void SetRhoZPlane(Double_t a, Double_t b, Double_t c, Double_t d)
    { fRhoZPlane[0] = a; fRhoZPlane[1] = b; fRhoZPlane[2] = c; fRhoZPlane[3] = d; }

    // Event Elements of Event Scene
    TEveElementList* EveMCPoints, *EveMCTracks, *EveRecoPoints, *EveRecoTracks;
    void AddEventElement(TEveElement* element, ElementList element_list);

    // coloring method
    enum VisualizationColoring {selectedColoring, levelColoring, defaultColoring};
    VisualizationColoring gVisualizationColoring;
    // set high transparency for detector geometry
    void SetTransparentGeometry(bool is_on);
    virtual void SetXMLConfig(TString xml_config) { fXMLConfig = xml_config; }

    // background color of EVE Viewers
    int background_color;
    // whether background color is dark
    bool isDarkColor;
    // whether Online of Offline mode
    bool isOnline;
    // data source: 0 - simulation data; 1 - raw detector data
    int iDataSource;

    // ZDC module visibility flags. NULL if there are no ZDC modules to show
    bool* isZDCModule;  //!
    // current value of "mc points", "reco points", "reco tracks" checkbox - FOR CALORIMETER TOWERS
    bool fgShowShowMCPoints;
    bool fgShowShowRecoPoints;
    bool fgShowShowRecoTracks;
    // require event redraw after "mc points", "reco points", "reco tracks" checkbox value is changed - FOR CALORIMETER TOWERS
    bool fgRedrawRecoPointsReqired;

    unordered_set<Int_t> fCurrentPDG;

 protected:
    TEveViewer* GetRPhiView() const { return fRPhiView; }
    TEveViewer* GetRhoZView() const { return fRhoZView; }
    TEveViewer* GetMultiView() const { return fMulti3DView; }
    TEveViewer* GetMultiRPhiView() const { return fMultiRPhiView; }
    TEveViewer* GetMultiRhoZView() const { return fMultiRhoZView; }
    TEveProjectionManager* GetRhoZProjManager() const { return fRhoZMng; }
    TEveProjectionManager* GetRPhiProjManager() const { return fRPhiMng; }
    TEveProjectionAxes* GetRPhiAxes() const { return fAxesPhi; }
    TEveProjectionAxes* GetRhoZAxes() const { return fAxesRho; }

    virtual void LoadXMLSettings();
    void LoadXMLDetector(TGeoNode *node, MpdXMLNode *xml, Int_t depth=0);

  private:
    MpdEventManagerEditor* fEventEditor; //!
    // skeleton Singleton Instance
    static MpdEventManager* fgRinstance; //!

    MpdEventManager(const MpdEventManager&) = delete;
    MpdEventManager& operator=(const MpdEventManager&) = delete;

    // VIEWERS for RPhi and RPhoZ projections, 3D view in multi-viewer, RPhi and RPhoZ projections in multi-viewer
    TEveViewer *fRPhiView, *fRhoZView, *fMulti3DView, *fMultiRPhiView, *fMultiRhoZView;
    // projection managers for RPhi and Rho views
    TEveProjectionManager *fRPhiMng, *fRhoZMng;
    // scenes for geometry presentation in RPhi plane and RPhoZ plane
    TEveScene *fRPhiGeomScene, *fRhoZGeomScene;
    TEveProjectionAxes *fAxesPhi, *fAxesRho;
    Double_t fRPhiPlane[4]; Double_t fRhoZPlane[4];

    TGListTreeItem* fEvent; //!
    // current event number
    Int_t iCurrentEvent;
    Bool_t fPriOnly;
    // the most minimum particle energy for selected event
    Float_t fMinEnergy;
    // the most maximum particle energy for selected event
    Float_t fMaxEnergy;
    // minimum energy to cut particles by energy in selected event
    Float_t fEvtMinEnergy;
    // maximum energy to cut particles by energy in selected event
    Float_t fEvtMaxEnergy;

    // the last color indice of Color Creating from rgb triple
    Int_t fLastUsedColor;

    TString fXMLConfig;
    map<int,int> fPDGToColor;

    // arrays with color sturctures for detector and hierarchical coloring
    vector<structSelectedColoring*> vecSelectedColoring;    //!
    vector<structLevelColoring*> vecLevelColoring;          //!

    void SetViewers(TEveViewer* RPhi, TEveViewer* RhoZ);

    // changing color and visibility of geometry nodes
    void InitColorStructure();
    // get color id by color name or RGB triple
    Int_t StringToColor(TString color);
    // returns loaded xml if successful of NULL if validation failed
    bool ValidateXml(const char *XMLFileName, const char *XSDFileName);
    void LevelChangeNodeProperty(TGeoNode* node, int level);
    void SelectedGeometryColoring();
    void RecursiveChangeNodeProperty(TGeoNode* parentNode, Int_t color, int transparency);
    void RecursiveChangeNodeTransparent(TGeoNode* parentNode, int transparency);

    ClassDef(MpdEventManager,1);
};

#endif
