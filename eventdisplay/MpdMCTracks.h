// -------------------------------------------------------------------------
// -----                      MpdMCTracks header file                 -----
// -----                Created 10/12/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------

/** MpdMCTracks
 * @author M. Al-Turany
 * @since 10.12.07
 **
 **/

#ifndef MPDMCTRACKS_H
#define MPDMCTRACKS_H

#include "FairTask.h"
#include "MpdEventManager.h"

#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TParticle.h"
#include "TString.h"
#include "TClonesArray.h"


class MpdMCTracks : public FairTask
{
  public:
    /** Default constructor **/
    MpdMCTracks();
    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    MpdMCTracks(const char* name, Int_t iVerbose = 0);

    /** Destructor **/
    virtual ~MpdMCTracks();

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) {fVerbose = iVerbose;}
    virtual InitStatus Init();
    /** Executed task **/
    virtual void Exec(Option_t* option);
    virtual void SetParContainers();
    /** Action after each event**/
    virtual void Finish();

    void Reset();
    TEveTrackList* GetTrGroup(TParticle* P);

  protected:
    TClonesArray*  fTrackList;  //!
    TEveTrackPropagator* fTrPr;
    MpdEventManager* fEventManager;  //!
    TObjArray* fEveTrList;
    TEveTrackList* fTrList;  //!

    Double_t MinEnergyLimit;
    Double_t MaxEnergyLimit;
    Double_t PEnergy;

  private:
    MpdMCTracks(const MpdMCTracks&) = delete;
    MpdMCTracks& operator=(const MpdMCTracks&) = delete;

    ClassDef(MpdMCTracks,1);
};

#endif
