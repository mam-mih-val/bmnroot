// Preliminary version of BmnDchHitProducerTmp

#ifndef BmnDchHitProducerTmp_H
#define BmnDchHitProducerTmp_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "FairTask.h"
#include "CbmTofPoint.h"
#include "TClonesArray.h"
#include "BmnEnums.h"
#include "BmnDchDigit.h"

#include "CbmTofHit.h"

using namespace std;

class BmnDchHitProducerTmp : public FairTask {
public:

    /** Default constructor **/
    BmnDchHitProducerTmp(Int_t num);

    /** Destructor **/
    virtual ~BmnDchHitProducerTmp();

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

    /** Input array of DCH Points **/
    TClonesArray* fBmnDchPointsArray;
    /** Input array of DCH Digits **/
    TClonesArray* fBmnDchDigitsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of DCH Hits **/
    TClonesArray* fBmnDchHitsArray;

    Bool_t fOnlyPrimary;
    TString fRunType; //"points" or "digits"
    Int_t fDchNum;

    ClassDef(BmnDchHitProducerTmp, 1);

};

#endif
