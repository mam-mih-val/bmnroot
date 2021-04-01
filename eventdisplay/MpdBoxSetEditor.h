// -------------------------------------------------------------------------
// -----              MpdBoxSetEditor header file                       -----
// -----          Created 26/03/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------

/** MpdBoxSetEditor
 * @author T. Stockmanns
 * @since 26.3.2009
 *   Base class to display 3D Points in Eve as a BoxSet
 *   One has to overwrite the method GetVector which takes a TObject and writes out a TVector3 which is then taken as an input
 *   to place the points.
 *   If one wants to change the color of the points one has to overwrite the method GetValue. This method takes a TObject and
 *   an integer and translates this into an integer as input for the EveBoxSet method DigitValue
 **
 **/

#ifndef MPDBOXSETEDITOR_H
#define MPDBOXSETEDITOR_H

#include "MpdBoxSet.h"

#include "TGedFrame.h"
#include "TGWindow.h"
#include "GuiTypes.h"           // for Pixel_t
#include "TGFrame.h"            // for EFrameType::kChildFrame, etc
#include "TGNumberEntry.h"


class MpdBoxSetEditor : public TGedFrame
{
  public:
    MpdBoxSetEditor(const TGWindow* p = 0, Int_t width = 170, Int_t height = 30,
                    UInt_t options = kChildFrame, Pixel_t back = GetDefaultFrameBackground());
    virtual ~MpdBoxSetEditor() {}

    virtual void Init();

    virtual void SetModel(TObject* obj)
    {
        fM = dynamic_cast<MpdBoxSet*>(obj);
        if (fM)
        {
            fTimeWindowPlus->SetNumber(fM->GetTimeWindowPlus());
            fTimeWindowMinus->SetNumber(fM->GetTimeWindowMinus());
        }
    }

    virtual void TimeWindow();

    TGVerticalFrame* fInfoFrame;
    TGNumberEntry* fTimeWindowPlus;
    TGNumberEntry* fTimeWindowMinus;

  protected:
    TObject* fObject;
    MpdBoxSet* fM;

  private:
    MpdBoxSetEditor(const MpdBoxSetEditor&) = delete;
    MpdBoxSetEditor& operator=(const MpdBoxSetEditor&) = delete;

    ClassDef(MpdBoxSetEditor,2);
};

#endif
