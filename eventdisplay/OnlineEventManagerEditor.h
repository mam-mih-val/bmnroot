// -------------------------------------------------------------------------
// -----                       OnlineEventManagerEditor                  -----
// -----                  Created 16/12/07  by M. Al-Turany            -----
// -------------------------------------------------------------------------
#ifndef ROOT_ONLINEEVENTMANAGEREDITOR
#define ROOT_ONLINEEVENTMANAGEREDITOR

#include "TGedFrame.h"                  // for TGedFrame
#include "TGNumberEntry.h"              // for TGNumberEntry, etc
#include "TGButton.h"                   // for TGCheckButton, TGTextButton
#include "TEveGValuators.h"             // for TEveGValuator
#include "TGLabel.h"                    // for TGLabel
#include "TClonesArray.h"               // for TClonesArray
#include "TMutex.h"
#include "TSemaphore.h"

#include "GuiTypes.h"                   // for Pixel_t
#include "Rtypes.h"                     // for ClassDef
#include "TGFrame.h"                    // for EFrameType::kChildFrame
#include "BmnMwpcDigit.h"

#include <vector>
#include <pthread.h>
#include <stddef.h>                     // for NULL
#include <cerrno>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

class OnlineEventManager;

class EventData : public TObject
{
  public:
    //bool isEventDataFinished;
    //unsigned int uiEventNumber;
    ULong64_t event_timestamp;

    vector<BmnMwpcDigit*> MWPC1Planes[6];
    vector<BmnMwpcDigit*> MWPC2Planes[6];

    EventData()
    {
        //isEventDataFinished = false;
    }

    EventData(const EventData& event_data)
    {
        //uiEventNumber = event_data.uiEventNumber;
        event_timestamp = event_data.event_timestamp;

        for (int i = 0; i < 6; i++)
            MWPC1Planes[i] = event_data.MWPC1Planes[i];

        for (int i = 0; i < 6; i++)
            MWPC2Planes[i] = event_data.MWPC2Planes[i];
    }

    virtual ~EventData() {}
};

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
    OnlineEventManager* fEventManager;
    TSemaphore* semEventData;
};

class OnlineEventManagerEditor : public TGedFrame
{
    OnlineEventManagerEditor(const OnlineEventManagerEditor&);            // Not implemented
    OnlineEventManagerEditor& operator=(const OnlineEventManagerEditor&); // Not implemented

  public:
    TObject* fObject;
    OnlineEventManager*  fManager;
    TGNumberEntry*  fCurrentEvent, *fCurrentPDG;
    TGCheckButton*  fVizPri;
    TEveGValuator *fMinEnergy, *fMaxEnergy;
    TGLabel* fEventTime;

    int iCurrentEvent;
    TGCompositeFrame* title1;
    TGGroupFrame *groupData;
    TGCheckButton* fShowMCPoints, *fShowMCTracks, *fShowRecoPoints, *fShowRecoTracks;

    vector<EventData*>* fEventReadData;
    vector<EventData*>* fEventDrawData;
    TSemaphore* semEventData;
    //void* ReadDetectorFile(void* ptr);
    //int ParseDetectorFile(unsigned int* buffer, long size);
    void RunReadFileThread();
    void RunDrawThread();

    OnlineEventManagerEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30,
                           UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
    virtual ~OnlineEventManagerEditor() { delete semEventData; }
    void SetModel(TObject* obj);
    virtual void SelectEvent();
    virtual void SelectPDG();
    void DoVizPri();
    virtual void MaxEnergy();
    virtual void MinEnergy();
    virtual void Init();

    virtual void SwitchBackground(Bool_t is_on);
    virtual void ShowGeometry(Bool_t is_show);
    virtual void ShowMCPoints(Bool_t is_show);
    virtual void ShowMCTracks(Bool_t is_show);
    virtual void ShowRecoPoints(Bool_t is_show);
    virtual void ShowRecoTracks(Bool_t is_show);

    ClassDef(OnlineEventManagerEditor, 0); // Specialization of TGedEditor for proper update propagation to TEveManager.
};

#endif
