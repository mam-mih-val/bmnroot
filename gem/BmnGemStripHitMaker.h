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

    void SetVerbosity(Bool_t verbose);

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void FindHitsAndFakes(Int_t *PointTypeArray, Double_t *PointSignalDiffArray, BmnGemStripStation* station, BmnGemStripReadoutModule* module);

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fOutputHitsBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;
    TClonesArray* fBmnGemStripDigitsArray;

    /** Output array of Gem Hits **/
    TClonesArray* fBmnGemStripHitsArray;

    ClassDef(BmnGemStripHitMaker,1);
};


#endif