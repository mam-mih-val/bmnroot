// -------------------------------------------------------------------------
// -----                    BmnExpTrackDraw header file                -----
// -----              created 05/10/15 by K. Gertsenberger             -----
// ----- class to visualize GlobalTracks from *.root in EventDisplay   -----
// -------------------------------------------------------------------------


#ifndef BMNEXPTRACKDRAW_H
#define BMNEXPTRACKDRAW_H

#include "FairTask.h"
#include "FairEventManager.h"

#include "TEveTrackPropagator.h"
#include "TEveTrack.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TString.h"
#include "TParticle.h"


class BmnExpTrackDraw : public FairTask
{
  public:
    // default constructor
    BmnExpTrackDraw();

    // constructor: @name - name of task, @iVerbose - verbosity level
    BmnExpTrackDraw(const char* name, Int_t iVerbose = 1);

    // destructor
    virtual ~BmnExpTrackDraw();

    // set verbosity level for this task and all of the subtasks
    void SetVerbose(Int_t iVerbose) {fVerbose = iVerbose;}
    // execute function of this task
    virtual void Exec(Option_t* option);
    // initialization of the track drawing task
    virtual InitStatus Init();
    virtual void SetParContainers();

    // action after each event processing
    virtual void Finish();
    void Reset();

    // return pointer to EVE track list for given particle name. if list don't exist then create it
    TEveTrackList* GetTrGroup(TParticle* P);

  protected:
    TChain* bmn_data_tree;  //!
    // tracks collection
    TClonesArray*  fTrackList;      //!
    // MWPC hits collection corresponding 'tracks collection'
    TClonesArray*  fMwpcHitList;     //!
    // EVE track propagator
    TEveTrackPropagator* fTrPr;
    FairEventManager* fEventManager;    //!
    TObjArray* fEveTrList;
    TString fEvent;                     //!
    TEveTrackList* fTrList;             //!
    Double_t MinEnergyLimit;
    Double_t MaxEnergyLimit;
    Double_t PEnergy;

  private:
    BmnExpTrackDraw(const BmnExpTrackDraw&);
    BmnExpTrackDraw& operator=(const BmnExpTrackDraw&);

    ClassDef(BmnExpTrackDraw,1);
};
#endif
