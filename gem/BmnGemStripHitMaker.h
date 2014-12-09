#ifndef BMNGEMSTRIPHITMAKER_H
#define BMNGEMSTRIPHITMAKER_H 1

//BmnGemStripHitMaker (for geometry GEMS_v2.root)
//makes digits and find hits from MC-points

#include <iostream>

#include "Rtypes.h"
#include "TClonesArray.h"
//#include "TVector3.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnGemStripDigit.h"
#include "BmnGemStripHit.h"
#include "BmnGemStripStationSet.h"

using namespace std;

class BmnGemStripHitMaker : public FairTask {
public:

    BmnGemStripHitMaker();

    virtual ~BmnGemStripHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessMCPoints();

private:

    TString fInputBranchName;
    TString fOutputDigitsBranchName;
    TString fOutputHitsBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;

    /** Input array of MC Tracks **/
    //TClonesArray* fMCTracksArray;

    /** Output array of Gem Digits **/
    TClonesArray* fBmnGemStripDigitsArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnGemStripHitsArray;


    ClassDef(BmnGemStripHitMaker,1);
};


#endif