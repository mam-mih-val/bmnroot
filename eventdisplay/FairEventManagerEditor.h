// Specialization of TGedEditor for proper update propagation to TEveManager

#ifndef ROOT_FAIREVENTMANAGEREDITOR
#define ROOT_FAIREVENTMANAGEREDITOR

#include "FairEventManager.h"

#include "TGedFrame.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TEveGValuators.h"

class FairEventManagerEditor;
struct ThreadParam_OnlineDisplay
{
    FairEventManager* fEventManager;
    FairEventManagerEditor* fManagerEditor;
    FairRootManager* fRootManager;
    int iCurrentEvent;
    bool isStreamSource;
    bool isZDCRedraw;
};

// multithread functions
void* RunOnlineDisplay(void* ptr);

class FairEventManagerEditor : public TGedFrame
{
  private:
    TObject* fObject;
    FairEventManager* fEventManager;

    TGLabel* fEventTime;
    TGNumberEntry* fCurrentPDG;
    TGCheckButton* fVizPri;
    TEveGValuator* fMinEnergy, *fMaxEnergy;
    TGHorizontalFrame* fGeometryFrame;
    TGCheckButton* ShowMagnetButton;

  public:
    FairEventManagerEditor(const TGWindow* p = 0, Int_t width = 170, Int_t height = 30,
                           UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground());
    FairEventManagerEditor(const FairEventManagerEditor&);
    FairEventManagerEditor& operator=(const FairEventManagerEditor&);
    virtual ~FairEventManagerEditor() {}

    virtual void Init();
    void SetModel(TObject* obj);

    virtual void SelectEvent();
    virtual void UpdateEvent();
    virtual void SelectPDG();
    void DoVizPri();
    virtual void MinEnergy();
    virtual void MaxEnergy();
    virtual void SwitchBackground(Bool_t is_on);
    virtual void SwitchTransparency(Bool_t is_on);
    virtual void ShowGeometry(Bool_t is_show);
    virtual void ShowMagnet(Bool_t is_show);
    virtual void ShowMCPoints(Bool_t is_show);
    virtual void ShowMCTracks(Bool_t is_show);
    virtual void ShowRecoPoints(Bool_t is_show);
    virtual void ShowRecoTracks(Bool_t is_show);

    bool RedrawZDC(bool isRedraw = true);
    void RestoreZDC();

    int iThreadState;
    void BlockUI();
    void UnblockUI();

    // event count
    int iEventCount;
    // true - using event data from DAQ stream; false - using event data from file
    bool isStreamSource;

    TGPictureButton* fSave;
    // save screenshot of the EVE display
    virtual void SaveImage();    

    // 'Update' button
    TGTextButton* fUpdate; 
    // 'Current Event Number' textbox with spin buttons
    TGNumberEntry* fCurrentEvent;
    // 'Show Geometry' checkbox
    TGCheckButton* fGeometry;
    TGCheckButton* fShowMCPoints, *fShowMCTracks, *fShowRecoPoints, *fShowRecoTracks;

    ClassDef(FairEventManagerEditor, 0);
};

#endif
