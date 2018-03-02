// -------------------------------------------------------------------------
// -----                     BmnTrackDrawP header file
// -----                Created 02/12/15  by K. Gertsenberger
// -----       class to draw BmnTrack* collection by Geane propagation
// -------------------------------------------------------------------------


#ifndef BMNTRACKDRAWP_H
#define BMNTRACKDRAWP_H

#include "FairTask.h"
#include "MpdEventManager.h"
#include "FairGeanePro.h"
#include "TEveTrack.h"
#include "FairTrajFilter.h"

#include "TParticle.h"
#include "TObjArray.h"

class BmnTrackDrawP : public FairTask
{
  public:
    /** Default constructor **/
    BmnTrackDrawP();

    /** Standard constructor
    *@param name        Name of task and branch with BmnTrack* collection
    *@param iVerbose    Verbosity level
    **/
    BmnTrackDrawP(const char* name, Int_t iVerbose = 0);

    /** Destructor **/
    virtual ~BmnTrackDrawP();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose);
    /** Executed task **/
    virtual void Exec(Option_t* option);

    virtual InitStatus Init();
    virtual void SetParContainers();
    void Reset();
    virtual void Finish();

    // return pointer to EVE track list for given particle name. if list don't exist then create it
    TEveTrackList* GetTrGroup(TParticle* P);

    void InitGeant3();

  protected:
    MpdEventManager* fEventManager;//!
    TClonesArray*  fTrackList;      //!
    TEveTrackList* fTrList;         //!

    FairGeanePro* fPro;             //!
    TEveTrackPropagator* fTrPr;     //!
    TObjArray* fEveTrList;
    FairTrajFilter* fTrajFilter;    //!

    Float_t x1[3];
    Float_t p1[3];
    Float_t x2[3];
    Float_t p2[3];

    Double_t MinEnergyLimit;
    Double_t MaxEnergyLimit;
    Double_t PEnergy;

    ClassDef(BmnTrackDrawP,1);
};

#endif
