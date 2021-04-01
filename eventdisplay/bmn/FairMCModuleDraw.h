// -------------------------------------------------------------------------
// -----                      FairMCModuleDraw header file             -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef FAIRMCMODULEDRAW_H
#define FAIRMCMODULEDRAW_H

#include "FairTask.h"
#include "MpdEventManager.h"

#include "TClonesArray.h"
#include "TObject.h"
#include "TVector3.h"

class FairMCModuleDraw : public FairTask
{
  public:
    /** Default constructor **/
    FairMCModuleDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    FairMCModuleDraw(const char* name, Color_t color ,Style_t mstyle, Int_t iVerbose = 0);

    /** Destructor **/
    virtual ~FairMCModuleDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    /** Executed task **/
    virtual void Exec(Option_t* option);
    void Reset();

protected:
    virtual TVector3 GetVector(TObject* obj);
    virtual TObject* GetValue(TObject* obj,Int_t i);
    virtual void SetParContainers();
    virtual InitStatus Init();
    /** Action after each event**/
    virtual void Finish();

    // Verbosity level
    Int_t   fVerbose;
    MpdEventManager* fEventManager; //!
    TClonesArray* fPointList; //!
    TClonesArray* fMCTracks; //!

    Color_t fColor; //!
    Style_t fStyle; //!

  private:
    FairMCModuleDraw(const FairMCModuleDraw&) = delete;
    FairMCModuleDraw& operator=(const FairMCModuleDraw&) = delete;

    ClassDef(FairMCModuleDraw,1);
};

#endif
