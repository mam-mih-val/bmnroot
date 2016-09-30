// -------------------------------------------------------------------------
// -----                     BmnTrackDraw header file                  -----
// -----                Created 02/12/15  by K. Gertsenberger          -----
// -------------------------------------------------------------------------


#ifndef BmnTrackDraw_H
#define BmnTrackDraw_H

#include "FairTask.h"
#include "FairEventManager.h"
#include "FairGeanePro.h"
#include "TEveTrack.h"
#include "FairTrajFilter.h"

#include "TString.h"
#include "TParticle.h"
#include "TObjArray.h"

class BmnTrackDraw : public FairTask
{
  public:
    /** Default constructor **/
    BmnTrackDraw();

    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    BmnTrackDraw(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~BmnTrackDraw();

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

    ClassDef(BmnTrackDraw,1);
};

#endif
