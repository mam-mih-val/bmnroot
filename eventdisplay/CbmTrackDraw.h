// -------------------------------------------------------------------------
// -----                     CbmTrackDraw header file                  -----
// -----                Created 02/12/15  by K. Gertsenberger          -----
// -------------------------------------------------------------------------


#ifndef CBMTRACKDRAW_H
#define CBMTRACKDRAW_H

#include "FairTask.h"
#include "FairEventManager.h"
#include "FairGeanePro.h"
#include "TEveTrack.h"
#include "FairTrajFilter.h"

#include "TString.h"
#include "TParticle.h"
#include "TObjArray.h"

class CbmTrackDraw : public FairTask
{
  public:
    /** Default constructor **/
    CbmTrackDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    CbmTrackDraw(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~CbmTrackDraw();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose);
    /** Executed task **/
    virtual void Exec(Option_t* option);
    virtual InitStatus Init();
    virtual void SetParContainers();

    /** Action after each event**/
    virtual void Finish();
    void Reset();
    TEveTrackList* GetTrGroup(TParticle* P);

    void InitGeant3();

  protected:
    TChain* bmn_data_tree;          //!
    TClonesArray*  fTrackList;      //!
    FairEventManager* fEventManager;//!
    TEveTrackList* fTrList;         //!
    FairGeanePro* fPro;             //!

    TEveTrackPropagator* fTrPr;     //!
    TObjArray* fEveTrList;
    FairTrajFilter* fTrajFilter;    //!
    TString fEvent;                 //!
    TGeant3* gMC3;                  //!

    Float_t x1[3];
    Float_t p1[3];
    Float_t x2[3];
    Float_t p2[3];
    Double_t MinEnergyLimit;
    Double_t MaxEnergyLimit;
    Double_t PEnergy;

    ClassDef(CbmTrackDraw,1);
};

#endif
