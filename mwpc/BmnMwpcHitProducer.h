// Preliminary version of BmnMwpcHitProducer

#ifndef BmnMwpcHitProducer_H
#define BmnMwpcHitProducer_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "FairTask.h"
#include "CbmTofPoint.h"
#include "TClonesArray.h"
#include "BmnEnums.h"
#include "BmnMwpcDigit.h"

#include "CbmTofHit.h"

using namespace std;

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

    BmnStatus ProcessPoints();
    BmnStatus ProcessDigits();


private:

    TString fInputMCBranchName;
    TString fInputDigiBranchName;

    /** Input array of MWPC Points **/
    TClonesArray* fBmnMwpcPointsArray;
    /** Input array of MWPC Digits **/
    TClonesArray* fBmnMwpcDigitsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of MWPC Hits **/
    TClonesArray* fBmnMwpcHitsArray;

    Bool_t fOnlyPrimary;
    TString fRunType; //"points" or "digits"
    Int_t fMwpcNum;

    ClassDef(BmnMwpcHitProducer, 1);

};

#endif
