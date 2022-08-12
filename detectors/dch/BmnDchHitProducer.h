/*
 * class: BmnDchHitProducer
 * Created: 11.09.2019
 * Author: D. Baranov
 */

#ifndef BMNDCHHITPRODUCER_H
#define BMNDCHHITPRODUCER_H 1

#include "TClonesArray.h"
#include "FairTask.h"
#include "TRandom.h"
#include "TH1F.h"

#include <iostream>

using namespace std;

class BmnDchHitProducer : public FairTask
{
  public:
    /** Default constructor **/
    BmnDchHitProducer();
    /** Destructor **/
    virtual ~BmnDchHitProducer();

    /** Virtual method Init **/
    virtual InitStatus Init();
    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);
    /** Virtual method Finish **/
    virtual void Finish();

  private:
    TString fInputBranchName;
    TString fTracksBranchName;
    TString fOutputHitsBranchName;

    /** Input array of Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Hits **/
    TClonesArray* fBmnHitsArray;

    // DCH parameters
    const UInt_t fNActivePlanes = 8; //number of active wire planes in DHC
    TString *fPlaneTypes;   //[fNActivePlanes]
    TList fhList2;
    TRandom rand_gen;

    TH1F* hMCtr_ion_dc1;
    TH1F* hMCtr_ion_dc2;
    TH1F* hMCtr_ionID_dc1;
    TH1F* hMCtr_ionID_dc2;

    ClassDef(BmnDchHitProducer, 1);
};

#endif
