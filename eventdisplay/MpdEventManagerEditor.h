// Specialization of TGedEditor for proper update propagation to TEveManager

#ifndef MPDEVENTMANAGEREDITOR_H
#define MPDEVENTMANAGEREDITOR_H

#include "MpdEventManager.h"

#include "TGedFrame.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TEveGValuators.h"

#include "string"

class MpdEventManagerEditor;
struct ThreadParam_OnlineDisplay
{
    MpdEventManager* fEventManager;
    MpdEventManagerEditor* fManagerEditor;
    FairRootManager* fRootManager;
    int iCurrentEvent;
    bool isStreamSource;
    bool isZDCRedraw;
};

// multithread functions
void* RunOnlineDisplay(void* ptr);

class MpdEventManagerEditor : public TGedFrame
{
    MpdEventManagerEditor(const MpdEventManagerEditor&) = delete;
    MpdEventManagerEditor& operator=(const MpdEventManagerEditor&) = delete;

  protected:
    TObject* fObject;
    MpdEventManager* fEventManager;

    TGLabel* fEventTime;
    TGTextEntry* fCurrentPDGTextEntry;
    string fCurrentPDGString;
    TGCheckButton* fVizPri;
    TEveGValuator* fMinEnergy, *fMaxEnergy;
    TGHorizontalFrame* fGeometryFrame;
    TGCheckButton* ShowMagnetButton;
    // whether magnet volume was found to use the special checkbov for visibility
    bool isMagnetFound;

  public:
    MpdEventManagerEditor(const TGWindow* p = 0, Int_t width = 170, Int_t height = 30,
                           UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground());
    virtual ~MpdEventManagerEditor() {}

    virtual void Init();
    void SetModel(TObject* obj) { fObject = obj; }

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

    bool RedrawZDC(bool isFull = false, bool isRedraw = true);

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
    TGCheckButton *fShowMCPoints, *fShowMCTracks, *fShowRecoPoints, *fShowRecoTracks;

    ClassDef(MpdEventManagerEditor, 0);
};

#endif
