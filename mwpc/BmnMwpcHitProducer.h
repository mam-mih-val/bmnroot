// Preliminary version of BmnMwpcHitProducer

#ifndef BmnMwpcHitProducer_H
#define BmnMwpcHitProducer_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "FairTask.h"
#include "CbmTofPoint.h"

#include "CbmTofHit.h"

using namespace std;

class TClonesArray;

class BmnMwpcHitProducer : public FairTask {
public:

    /** Default constructor **/
    BmnMwpcHitProducer(Int_t num);

    /** Destructor **/
    virtual ~BmnMwpcHitProducer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();

    // Setters

    void SetOnlyPrimary(Bool_t opt = kFALSE) {
        fOnlyPrimary = opt;
    }


private:

    TString fInputBranchName;
    TString fOutputHitsBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnMwpcPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnMwpcHitsArray;

    Bool_t fOnlyPrimary;
    Int_t fMwpcNum;

    ClassDef(BmnMwpcHitProducer, 1);

};

#endif