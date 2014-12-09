// Preliminary version of BmnDchHitProducer

#ifndef BmnDchHitProducer_H
#define BmnDchHitProducer_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "FairTask.h"
#include "CbmTofPoint.h"

#include "CbmTofHit.h"

using namespace std;

class TClonesArray;

class BmnDchHitProducer : public FairTask {
public:

    /** Default constructor **/
    BmnDchHitProducer(Int_t num);

    /** Destructor **/
    virtual ~BmnDchHitProducer();

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
    TClonesArray* fBmnDchPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnDchHitsArray;

    Bool_t fOnlyPrimary;
    Int_t fDchNum;

    ClassDef(BmnDchHitProducer, 1);

};

#endif