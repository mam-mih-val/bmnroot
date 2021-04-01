// -------------------------------------------------------------------------
// -----              MpdBoxSetDraw header file                       -----
// -----          Created 26/03/09  by T. Stockmanns                   -----
// -------------------------------------------------------------------------

/** MpdBoxSetDraw
 * @author T. Stockmanns
 * @since 26.3.2009
 *   Base class to display 3D Points in Eve as a BoxSet
 *   One has to overwrite the method GetVector which takes a TObject and writes out a TVector3 which is then taken as an input
 *   to place the points.
 *   If one wants to change the color of the points one has to overwrite the method GetValue. This method takes a TObject and
 *   an integer and translates this into an integer as input for the EveBoxSet method DigitValue
 **
 **/

#ifndef MPDBOXSETDRAW_H
#define MPDBOXSETDRAW_H

#include "MpdEventManager.h"

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairTSBufferFunctional.h"     // for StopTime

#include "TClonesArray.h"
#include "TVector3.h"
#include "TObject.h"

class MpdBoxSet;
class MpdBoxSetDraw : public FairTask
{
  public:
    /** Default constructor **/
    MpdBoxSetDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    MpdBoxSetDraw(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~MpdBoxSetDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    virtual void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    virtual void SetRedraw(Bool_t is_redraw) { isRedraw = is_redraw; }

    virtual Double_t GetTimeWindowPlus() { return fTimeWindowPlus; }
    virtual Double_t GetTimeWindowMinus() { return fTimeWindowMinus; }
    virtual void SetTimeWindowMinus(Double_t val) { fTimeWindowMinus = val; }
    virtual void SetTimeWindowPlus(Double_t val) { fTimeWindowPlus = val; }
    virtual void SetStartTime(Double_t val) { fStartTime = val; }
    virtual void UseEventTimeAsStartTime(Bool_t val = kTRUE) { fUseEventTime = val; }

    MpdBoxSet* CreateBoxSet();
    virtual void SetBoxDimensions(Double_t x, Double_t y, Double_t z)
    {
      fX = x;
      fY = y;
      fZ = z;
    }

    /** Executed task **/
    virtual void Exec(Option_t* option);

    void Reset();

  protected:
    virtual void SetParContainers();
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();

    virtual TVector3 GetVector(TObject* obj) = 0;
    virtual Int_t GetValue(TObject* obj, Int_t i);
    virtual void AddBoxes(MpdBoxSet* set, TObject* obj, Int_t i = 0);

    //  Verbosity level
    Int_t fVerbose;

    TClonesArray* fList;                //!
    MpdEventManager* fEventManager;     //!
    FairRootManager* fManager;          //!
    MpdBoxSet* fq;                      //!
    Double_t fX, fY, fZ;

    Double_t fTimeWindowPlus;
    Double_t fTimeWindowMinus;
    Double_t fStartTime;
    Bool_t fUseEventTime;
    Bool_t isRedraw;

  private:
    MpdBoxSetDraw(const MpdBoxSetDraw&) = delete;
    MpdBoxSetDraw& operator=(const MpdBoxSetDraw&) = delete;
    BinaryFunctor* fStartFunctor;
    BinaryFunctor* fStopFunctor;

    ClassDef(MpdBoxSetDraw,1);
};

#endif
