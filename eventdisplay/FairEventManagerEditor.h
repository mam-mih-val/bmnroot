#ifndef ROOT_FAIREVENTMANAGEREDITOR
#define ROOT_FAIREVENTMANAGEREDITOR

#include "FairEventManager.h"
#include "RawDataParser.h"

#include "TGedFrame.h"
#include "TGNumberEntry.h"
#include "TGButton.h"
#include "TEveGValuators.h"
#include "TGLabel.h"
#include "TMutex.h"
#include "TSemaphore.h"

#include <vector>

struct ThreadParam_ReadFile
{
    vector<EventData*>* fEventReadData;
    vector<EventData*>* fEventDrawData;
    char* raw_file_name_begin;
    TSemaphore* semEventData;
};

struct ThreadParam_Draw
{
    vector<EventData*>* fEventDrawData;
    FairEventManager* fEventManager;
    TSemaphore* semEventData;
};

class FairEventManagerEditor;
struct ThreadParam_RunTask
{
    FairEventManager* fEventManager;
    FairEventManagerEditor* fManagerEditor;
    bool isZDCRedraw;
    bool isRootManagerReadEvent;
};

// multithread functions
void* ReadMWPCFiles(void* ptr);
void* DrawEvent(void* ptr);
void* RunTasks(void* ptr);

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

    vector<EventData*>* fEventReadData;
    vector<EventData*>* fEventDrawData;
    TSemaphore* semEventData;
    // current event number
    int iEventNumber;

  public:
    FairEventManagerEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30,
                           UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
    FairEventManagerEditor(const FairEventManagerEditor&);
    FairEventManagerEditor& operator=(const FairEventManagerEditor&);
    virtual ~FairEventManagerEditor() { delete semEventData; }

    void SetModel(TObject* obj);
    virtual void SelectEvent();
    virtual void SelectPDG();
    void DoVizPri();
    virtual void MaxEnergy();
    virtual void MinEnergy();
    virtual void Init();

    virtual void SwitchBackground(Bool_t is_on);
    virtual void SwitchTransparency(Bool_t is_on);
    virtual void ShowGeometry(Bool_t is_show);
    virtual void ShowMCPoints(Bool_t is_show);
    virtual void ShowMCTracks(Bool_t is_show);
    virtual void ShowRecoPoints(Bool_t is_show);
    virtual void ShowRecoTracks(Bool_t is_show);

    // event count
    int iEventCount;
    // 'Update' button
    TGTextButton* fUpdate;
    // 'Current Event Number' textbox with spin buttons
    TGNumberEntry* fCurrentEvent;
    // 'Show Geometry' checkbox
    TGCheckButton* fGeometry;

    void RedrawZDC();
    void RestoreZDC();
    void BlockUI();
    void UnblockUI();

    ClassDef(FairEventManagerEditor, 0); // Specialization of TGedEditor for proper update propagation to TEveManager
};

#endif
