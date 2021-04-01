// -------------------------------------------------------------------------
// -----              MpdBoxSet header file                       -----
// -----          Created 26/03/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------

/** MpdBoxSet
 * @author T. Stockmanns
 * @since 26.3.2009
 *   Base class to display 3D Points in Eve as a BoxSet
 *   One has to overwrite the method GetVector which takes a TObject and writes out a TVector3 which is then taken as an input
 *   to place the points.
 *   If one wants to change the color of the points one has to overwrite the method GetValue. This method takes a TObject and
 *   an integer and translates this into an integer as input for the EveBoxSet method DigitValue
 **
 **/

#ifndef MPDBOXSET_H
#define MPDBOXSET_H

#include "MpdBoxSetDraw.h"
#include "TEveBoxSet.h"


class MpdBoxSet : public TEveBoxSet
{
  public:
    /** Standard constructor **/
    MpdBoxSet(MpdBoxSetDraw* drawer, const char* name = "MpdBoxSet", const char* t = "");
    /** Destructor **/
    virtual ~MpdBoxSet() {}

    void SetTimeWindowPlus(Double_t time) { fDraw->SetTimeWindowPlus(time); }
    void SetTimeWindowMinus(Double_t time) { fDraw->SetTimeWindowMinus(time); }

    Double_t GetTimeWindowPlus() { return fDraw->GetTimeWindowPlus(); }
    Double_t GetTimeWindowMinus() { return fDraw->GetTimeWindowMinus(); }

  private:
    MpdBoxSetDraw* fDraw;

    MpdBoxSet(const MpdBoxSet&) = delete;
    MpdBoxSet operator=(const MpdBoxSet&) = delete;

    ClassDef(MpdBoxSet,1);
};

#endif
