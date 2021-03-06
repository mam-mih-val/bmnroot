/*
 * class: BmnDchHitProducer
 * Created: 11.09.2019
 * Author: D. Baranov
 */

#ifndef BMNDCHHITPRODUCER_H
#define BMNDCHHITPRODUCER_H 1

#include <iostream>

#include "TClonesArray.h"
#include "FairTask.h"
#include "TRandom.h"

using namespace std;

class BmnDchHitProducer : public FairTask {
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

    //DCH parameters
    TString *fPlaneTypes;
    TList fhList2;
    TRandom rand_gen;

    ClassDef(BmnDchHitProducer, 1);
};

#endif
