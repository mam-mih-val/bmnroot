// -------------------------------------------------------------------------
// -----             MpdPointSetDraw header file                       -----
// -----          Created 10/12/07  by M. Al-Turany                    -----
// -------------------------------------------------------------------------

/** MpdPointSetDraw
 * @author M. Al-Turany
 * @since 03.01.08
 *   Task to display MC points
 **
 **/

#ifndef MPDPOINTSETDRAW_H
#define MPDPOINTSETDRAW_H

#include "MpdEventManager.h"

#include "FairTask.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TEvePointSet.h"


class MpdPointSetDraw : public FairTask
{
  public:
    /** Default constructor **/
    MpdPointSetDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    MpdPointSetDraw(const char* name, Color_t color, Style_t mstyle, Int_t iVerbose = 0);

    /** Destructor **/
    virtual ~MpdPointSetDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }

    /** Executed task **/
    virtual void Exec(Option_t* option);
    void Reset();

  protected:
    virtual TVector3 GetVector(TObject* obj) = 0;
    virtual TObject* GetValue(TObject* obj, Int_t i);

    virtual void AddEveElementList() = 0;
    virtual void RemoveEveElementList() = 0;

    virtual void SetParContainers();
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();

    // Verbosity level
    Int_t fVerbose;
    MpdEventManager* fEventManager;     //!
    TClonesArray* fPointList;           //!
    TEvePointSet* fq;                   //!

    Color_t fColor;                     //!
    Style_t fStyle;                     //!

  private:
    MpdPointSetDraw(const MpdPointSetDraw&) = delete;
    MpdPointSetDraw& operator=(const MpdPointSetDraw&) = delete;

    ClassDef(MpdPointSetDraw,1);
};

#endif
