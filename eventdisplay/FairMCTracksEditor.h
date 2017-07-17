// Specialization of TGedEditor for proper update propagation to TEveManager.

#ifndef ROOT_FAIREMCTRACKSEDITOR
#define ROOT_FAIREMCTRACKSEDITOR

#include "FairEventManager.h"

#include "TGedFrame.h"
#include "GuiTypes.h"   // for Pixel_t
#include "TGFrame.h"    // for EFrameType::kChildFrame
#include "TGWindow.h"


class FairMCTracksEditor : public TGedFrame
{
    FairMCTracksEditor(const FairMCTracksEditor&);            // Not implemented
    FairMCTracksEditor& operator=(const FairMCTracksEditor&); // Not implemented

  protected:
    TObject* fObject;
    FairEventManager* fManager;

  public:
    FairMCTracksEditor(const TGWindow* p = 0, Int_t width = 170, Int_t height = 30,
                       UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground());
    virtual ~FairMCTracksEditor() {}

    virtual void SetModel(TObject* obj) { fObject = obj; }

    ClassDef(FairMCTracksEditor, 0);
};

#endif
