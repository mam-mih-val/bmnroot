// -------------------------------------------------------------------------
// -----                       FairEventManagerEditor                  -----
// -----                  Created 16/12/07  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#ifndef ROOT_FAIREVENTMANAGEREDITOR
#define ROOT_FAIREVENTMANAGEREDITOR

#include "FairEventManager.h"

#include "TGedFrame.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TEveGValuators.h"
#include "TGLabel.h"
#include "TMutex.h"
#include "TSemaphore.h"
#include "TGFileDialog.h"

#include <vector>

class FairEventManagerEditor;
struct ThreadParam_OnlineDisplay
{
    FairEventManager* fEventManager;
    FairEventManagerEditor* fManagerEditor;
    FairRootManager* fRootManager;
    int iCurrentEvent;
    bool isZDCRedraw;
};

// multithread functions
void* RunOnlineDisplay(void* ptr);

class FairEventManagerEditor : public TGedFrame
{
  private:
    TObject* fObject;
    FairEventManager* fManager;
    TGNumberEntry* fCurrentPDG;
    TGCheckButton* fVizPri;
    TEveGValuator* fMinEnergy, *fMaxEnergy;
    TGLabel* fEventTime;
    TGCompositeFrame* title1;
    TGGroupFrame* groupData;
    TGCheckButton* fShowMCPoints, *fShowMCTracks, *fShowRecoPoints, *fShowRecoTracks;

    // current event number
    int iEventNumber;

  public:
    FairEventManagerEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30,
                           UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
    FairEventManagerEditor(const FairEventManagerEditor&);
    FairEventManagerEditor& operator=(const FairEventManagerEditor&);
    virtual ~FairEventManagerEditor() { }

    void SetModel(TObject* obj);
    virtual void SelectEvent();
    virtual void UpdateEvent();
    virtual void SelectPDG();
    void DoVizPri();
    virtual void MaxEnergy();
    virtual void MinEnergy();
    virtual void Init();

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
    void BlockUI();
    void UnblockUI();

    // save screenshot of the eve display
    virtual void SaveImage();

    // event count
    int iEventCount;
    // 'Update' button
    TGTextButton* fUpdate;
    TGPictureButton* fSave;
    // 'Current Event Number' textbox with spin buttons
    TGNumberEntry* fCurrentEvent;
    // 'Show Geometry' checkbox
    TGCheckButton* fGeometry;

    int iThreadState;

    ClassDef(FairEventManagerEditor, 0); // Specialization of TGedEditor for proper update propagation to TEveManager
};

#endif
