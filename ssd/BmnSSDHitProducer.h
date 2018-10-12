// Preliminary version of BmnSSDHitProducer

#ifndef BMNSSDHITPRODUCER_H
#define BMNSSDHITPRODUCER_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "TClonesArray.h"
#include "FairTask.h"


#include "CbmStsPoint.h"
#include "CbmStsTrack.h"

#include "BmnHit.h"

using namespace std;

class BmnSSDHitProducer : public FairTask {
public:

    /** Default constructor **/
    BmnSSDHitProducer();

    /** Destructor **/
    virtual ~BmnSSDHitProducer();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();
    
    Int_t DefineStationByZ(Double_t z, Int_t ssdType);
    
    void SetFakesFraction(Double_t f) {
        fFakesFraction = f;
    }

private:

    TString fInputBranchName;
    TString fTracksBranchName;
    TString fOutputHitsBranchName;
    TString fOutputSSDHitMatchesBranchName;

    /** Input array of SSD Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of SSD Hits **/
    TClonesArray* fBmnHitsArray;

    /** Output array of SSD Hit Matches **/
    TClonesArray* fBmnSSDHitMatchesArray;
    
    Double_t fFakesFraction;

    ClassDef(BmnSSDHitProducer, 1);

};

#endif
