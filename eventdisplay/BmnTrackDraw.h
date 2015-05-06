// -------------------------------------------------------------------------
// -----                    BmnTrackDraw header file                   -----
// -----              created 10/12/13 by K. Gertsenberger             -----
// ----- class to visualize reconstructed GlobalTracks in EventDisplay -----
// -------------------------------------------------------------------------


#ifndef BmnTrackDraw_H
#define BmnTrackDraw_H

#include "FairTask.h"
#include "FairEventManager.h"

#include "TEveTrackPropagator.h"
#include "TEveTrack.h"
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TString.h"
#include "TParticle.h"


class BmnTrackDraw : public FairTask
{
  public:
    // default constructor
    BmnTrackDraw();

    // constructor: @name - name of task, @iVerbose- verbosity level
    BmnTrackDraw(const char* name, Int_t iVerbose = 1);

    // destructor
    virtual ~BmnTrackDraw();

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
    TClonesArray*  fTrackList;      //!
    // GEM tracks collection
    TClonesArray*  fGemTrackList;   //!
    // GEM hits collection
    TClonesArray*  fGemHitList;     //!
    // TOF1 hits collection
    TClonesArray*  fTof1HitList;     //!
    // TOF2 hits collection
    TClonesArray*  fTof2HitList;     //!
    // DCH1 hits collection
    TClonesArray*  fDch1HitList;     //!
    // DCH2 hits collection
    TClonesArray*  fDch2HitList;     //!
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
    BmnTrackDraw(const BmnTrackDraw&);
    BmnTrackDraw& operator=(const BmnTrackDraw&);

    ClassDef(BmnTrackDraw,1);
};
#endif
