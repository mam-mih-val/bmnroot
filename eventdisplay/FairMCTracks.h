// -------------------------------------------------------------------------
// -----                      FairMCTracks header file                 -----
// -----                Created 10/12/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------

/** FairMCTracks
 * @author M. Al-Turany
 * @since 10.12.07
 **
 **/

#ifndef FAIRMCTRACKS_H
#define FAIRMCTRACKS_H

#include "FairTask.h"
#include "FairEventManager.h"

#include "TEveTrack.h"
#include "TEveTrackPropagator.h"
#include "TParticle.h"
#include "TString.h"
#include "TClonesArray.h"


class FairMCTracks : public FairTask
{
  public:
    /** Default constructor **/
    FairMCTracks();
    /** Standard constructor
    *@param name        Name of task
    *@param iVerbose    Verbosity level
    **/
    FairMCTracks(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~FairMCTracks();

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
    FairEventManager* fEventManager;  //!
    TObjArray* fEveTrList;
    TEveTrackList* fTrList;  //!

    Double_t MinEnergyLimit;
    Double_t MaxEnergyLimit;
    Double_t PEnergy;

  private:
    FairMCTracks(const FairMCTracks&);
    FairMCTracks& operator=(const FairMCTracks&);

    ClassDef(FairMCTracks,1);
};

#endif
