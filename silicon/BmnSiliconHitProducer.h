// Preliminary version of BmnSiliconHitProducer

#ifndef BMNSILICONHITPRODUCER_H
#define BMNSILICONHITPRODUCER_H 1

#include <math.h>
#include <iostream>
#include <vector>
#include "TString.h"
#include "TClonesArray.h"
#include "FairTask.h"
#include "BmnSiliconStationSet.h"
#include "BmnSiliconConfiguration.h"


#include "CbmStsPoint.h"
#include "CbmStsTrack.h"

#include "BmnHit.h"

using namespace std;

class BmnSiliconHitProducer : public FairTask {
public:

    /** Default constructor **/
    BmnSiliconHitProducer();

    /** Destructor **/
    virtual ~BmnSiliconHitProducer();

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
    TString fOutputSiliconHitMatchesBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnHitsArray;

    /** Output array of Silicon Hit Matches **/
    TClonesArray* fBmnSiliconHitMatchesArray;
        
    BmnSiliconStationSet *SiliconStationSet; //Entire GEM detector

    ClassDef(BmnSiliconHitProducer, 1);

};

#endif
