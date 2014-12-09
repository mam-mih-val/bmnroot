// Preliminary version of BmnGemHitProducer

#ifndef BMNGEMHITPRODUCER_H
#define BMNGEMHITPRODUCER_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "FairTask.h"

#include "CbmStsPoint.h"
#include "CbmStsTrack.h"

#include "BmnHit.h"

using namespace std;

class TClonesArray;

class BmnGemHitProducer : public FairTask {
public:

    /** Default constructor **/
    BmnGemHitProducer();

    /** Destructor **/
    virtual ~BmnGemHitProducer();

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
    TString fTracksBranchName;
    TString fOutputHitsBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of Gem Tracks **/   // <------------ ???
    TClonesArray* fBmnTracksArray;
    
    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnHitsArray;

    // CbmStsHitProducerIdeal(const CbmStsHitProducerIdeal&);
    // CbmStsHitProducerIdeal& operator=(const CbmStsHitProducerIdeal&);

    Bool_t fOnlyPrimary;

    void CheckGaussDistrib(TVector3, TVector3);

    ClassDef(BmnGemHitProducer, 1);

};

#endif