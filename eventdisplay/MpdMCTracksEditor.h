#ifndef MPDMCTRACKSEDITOR_H
#define MPDMCTRACKSEDITOR_H

#include "MpdEventManager.h"

#include "TGedFrame.h"
#include "TGWindow.h"


class MpdMCTracksEditor : public TGedFrame
{
    MpdMCTracksEditor(const MpdMCTracksEditor&) = delete;
    MpdMCTracksEditor& operator=(const MpdMCTracksEditor&) = delete;

  protected:
    TObject* fObject;
    MpdEventManager*  fManager;

  public:
    MpdMCTracksEditor(const TGWindow* p = 0, Int_t width = 170, Int_t height = 30,
                      UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground());
    virtual ~MpdMCTracksEditor() {}

    virtual void SetModel( TObject* obj) { fObject = obj; }

    ClassDef(MpdMCTracksEditor, 0); // Specialization of TGedEditor for proper update propagation to TEveManager.
};

#endif
