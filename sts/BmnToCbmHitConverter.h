#ifndef BMNTOCBMHITCONVERTER_H
#define BMNTOCBMHITCONVERTER_H 1

#include <iostream>

#include "TClonesArray.h"

#include "FairTask.h"
#include "TSystem.h"
#include "FairMCPoint.h"
#include "BmnGemStripHit.h"
#include "BmnSiliconHit.h"
#include "CbmStsHit.h"
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"

class BmnToCbmHitConverter : public FairTask {
public:

    BmnToCbmHitConverter();
    BmnToCbmHitConverter(Int_t iVerbose);

    virtual ~BmnToCbmHitConverter();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

private:

    TString fBmnGemHitsBranchName;
    TString fBmnSilHitsBranchName;
    TString fCbmHitsBranchName;

    /** Input array of BNM GEM Hits **/
    TClonesArray* fBmnGemHitsArray;
    
    /** Input array of BNM Silicon Hits **/
    TClonesArray* fBmnSilHitsArray;

    /** Output array of CBM Hits **/
    TClonesArray* fCbmHitsArray;

    BmnGemStripStationSet *GemStationSet; //Entire GEM detector
    BmnSiliconStationSet *SilStationSet;  //Entire SILICON detector

    ClassDef(BmnToCbmHitConverter,1);
};

#endif
