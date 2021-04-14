
#ifndef BMNSILICONHITPRODUCERSRC_H
#define BMNSILICONHITPRODUCERSRC_H 1

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
#include "TRandom.h"
#include "TList.h"
#include "TH1D.h"
#include "TH2D.h"

using namespace std;

class BmnSiliconHitProducerSRC : public FairTask {
public:

    /** Default constructor **/
    BmnSiliconHitProducerSRC();

    /** Destructor **/
    virtual ~BmnSiliconHitProducerSRC();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Finish **/
    virtual void Finish();

private:
    UInt_t  fEventNo; // event counter
    Bool_t  fDebug = 0;
    TList fList;
    TString fInputBranchName;
    TString fTracksBranchName;
    TString fOutputHitsBranchName;
    TString fOutputHitsBranchName2;
    TString fOutputFileName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnPointsArray;

    /** Input array of MC Tracks **/
    TClonesArray* fMCTracksArray;

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnHitsArray;
    TClonesArray* fBmnHitsArray2;
    
    BmnSiliconStationSet *SiliconStationSet; //Entire GEM detector
    ClassDef(BmnSiliconHitProducerSRC, 1);
    TRandom rand_gen;
    TH1D *hdX, *hdXp;

};

#endif
