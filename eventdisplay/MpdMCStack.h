// -------------------------------------------------------------------------
// -----                      MpdMCStack header file                 -----
// -----                Created 09/10/08  by M. Al-Turany              -----
// -------------------------------------------------------------------------

#ifndef MPDMCSTACK_H
#define MPDMCSTACK_H

#include "FairTask.h"
#include "FairGeanePro.h"
#include "MpdEventManager.h"
#include "FairTrajFilter.h"

#include "TEveTrackPropagator.h"
#include "TString.h"
#include "TObjArray.h"
#include "TEveTrack.h"
#include "TGeant3.h"


class MpdMCStack : public FairTask
{
  public:
    /** Default constructor **/
    MpdMCStack();
    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    MpdMCStack(const char* name, Int_t iVerbose = 0);
    /** Destructor **/
    virtual ~MpdMCStack();

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

  protected:
    MpdEventManager* fEventManager;  //!

    TClonesArray*  fTrackList;  //!
    TObjArray* fEveTrList;
    TEveTrackPropagator* fTrPr;
    TEveTrackList* fTrList;  //!

    Float_t x1[3], x2[3], p1[3], p2[3], ein[15];
    TGeant3* gMC3;  //!
    Double_t MinEnergyLimit, MaxEnergyLimit;
    Double_t PEnergy;
    FairGeanePro* fPro; //!
    FairTrajFilter* fTrajFilter; //!

    ClassDef(MpdMCStack,1);
};

#endif
