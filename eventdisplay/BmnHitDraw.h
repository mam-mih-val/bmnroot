// -------------------------------------------------------------------------
// -----                      BmnHitDraw header file                 -----
// -------------------------------------------------------------------------

#ifndef BMNHITDRAW_H
#define BMNHITDRAW_H

#include "FairTask.h"
#include "FairEventManager.h"

#include "TObject.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TEvePointSet.h"

class BmnHitDraw : public FairTask
{
  public:
    /** Default constructor **/
    BmnHitDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    BmnHitDraw(const char* name, Color_t color, Style_t mstyle, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~BmnHitDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) {fVerbose = iVerbose;}
    /** Executed task **/
    virtual void Exec(Option_t* option);
    void Reset();

  protected:
    virtual TVector3 GetVector(TObject* obj);
    virtual TObject* GetValue(TObject* obj,Int_t i);

    //  Verbosity level
    Int_t   fVerbose;
    virtual void SetParContainers();
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();
    TChain* bmn_hit_tree;  //!
    TClonesArray* fHitList; //!
    FairEventManager* fEventManager;   //!
    TEvePointSet* fq;    //!
    Color_t fColor; //!
    Style_t fStyle; //!

  private:
    BmnHitDraw(const BmnHitDraw&);
    BmnHitDraw& operator=(const BmnHitDraw&);

    ClassDef(BmnHitDraw,1);
};

#endif
