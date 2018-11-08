// Preliminary version of BmnGemHitProducer

#ifndef BMNGEMHITPRODUCER_H
#define BMNGEMHITPRODUCER_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "TClonesArray.h"
#include "FairTask.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripConfiguration.h"


#include "CbmStsPoint.h"
#include "CbmStsTrack.h"

#include "BmnHit.h"

using namespace std;

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
    
    Int_t DefineStationByZ(Double_t z);

private:

    TString fInputBranchName;
    TString fTracksBranchName;
    TString fOutputHitsBranchName;
    TString fOutputGemHitMatchesBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnHitsArray;

    /** Output array of GEM Hit Matches **/
    TClonesArray* fBmnGemStripHitMatchesArray;
    
    BmnGemStripStationSet *GemStationSet; //Entire GEM detector

    ClassDef(BmnGemHitProducer, 1);

};

#endif
