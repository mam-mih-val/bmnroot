// -------------------------------------------------------------------------
// -----                    MpdGlobalTrackDraw header file                   -----
// -----              created 10/12/13 by K. Gertsenberger             -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------


#ifndef MPDGLOBALTRACKDRAW_H
#define MPDGLOBALTRACKDRAW_H

#include "FairTask.h"
#include "FairEventManager.h"

#include "TEveTrackPropagator.h"
#include "TEveTrack.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TString.h"
#include "TParticle.h"


class MpdGlobalTrackDraw : public FairTask
{
  public:
    // default constructor
    MpdGlobalTrackDraw();

    // constructor: @name - name of task, @iVerbose- verbosity level
    MpdGlobalTrackDraw(const char* name, Int_t iVerbose = 1);

    // destructor
    virtual ~MpdGlobalTrackDraw();

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
    // global tracks collection
    TClonesArray*  fTrackList;          //!
    // kalman tracks collection
    TClonesArray*  fKalmanTrackList;    //!
    // MpdTpcHits collection
    TClonesArray*  fTpcHitList;         //!
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
    MpdGlobalTrackDraw(const MpdGlobalTrackDraw&);
    MpdGlobalTrackDraw& operator=(const MpdGlobalTrackDraw&);

    ClassDef(MpdGlobalTrackDraw,1);
};
#endif
