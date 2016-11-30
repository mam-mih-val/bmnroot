// -------------------------------------------------------------------------
// -----                      BmnDigitDraw header file                 -----
// -------------------------------------------------------------------------

#ifndef BMNDIGITDRAW_H
#define BMNDIGITDRAW_H

#include "FairTask.h"
#include "FairEventManager.h"

#include "TObject.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TEvePointSet.h"

class BmnDigitDraw : public FairTask
{
  public:
    /** Default constructor **/
    BmnDigitDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level,
    *@param det_id      Detector ID:
    * 1 - MWPC, 2 - DCH **/
    BmnDigitDraw(const char* name, Int_t det_id, Color_t color, Style_t mstyle, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~BmnDigitDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) {fVerbose = iVerbose;}
    /** Executed task **/
    virtual void Exec(Option_t* option);
    void Reset();

  protected:
    virtual TObject* GetValue(TObject* obj,Int_t i);

    //  Verbosity level
    Int_t   fVerbose;
    virtual void SetParContainers();
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();
    TChain* bmn_digit_tree;  //!
    TClonesArray* fDigitList; //!
    TClonesArray* fHitList; //!
    FairEventManager* fEventManager;   //!
    TEvePointSet* fq;    //!
    Color_t fColor; //!
    Style_t fStyle; //!
    Int_t fDetectorID; //!

  private:
    BmnDigitDraw(const BmnDigitDraw&);
    BmnDigitDraw& operator=(const BmnDigitDraw&);

    ClassDef(BmnDigitDraw,1);
};

#endif
