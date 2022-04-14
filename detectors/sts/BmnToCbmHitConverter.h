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

class BmnToCbmHitConverter : public FairTask
{
public:
    BmnToCbmHitConverter();
    BmnToCbmHitConverter(Int_t iVerbose);

    virtual ~BmnToCbmHitConverter();

    virtual InitStatus Init();

    virtual void Exec(Option_t *opt);

    virtual void Finish();

    void SetGemConfigFile(TString file) { fGemConfigFile = file; }
    void SetSilConfigFile(TString file) { fSilConfigFile = file; }

    void SetFixedErrors() { fUseFixedErrors = kTRUE; }

private:
    TString fBmnGemHitsBranchName;
    TString fBmnSilHitsBranchName;
    TString fCbmHitsBranchName;

    /** Input array of BNM GEM Hits **/
    TClonesArray *fBmnGemHitsArray;
    TClonesArray *fBmnGemUpperClusters;
    TClonesArray *fBmnGemLowerClusters;

    /** Input array of BNM Silicon Hits **/
    TClonesArray *fBmnSilHitsArray;
    TClonesArray *fBmnSilUpperClusters;
    TClonesArray *fBmnSilLowerClusters;

    /** Output array of CBM Hits **/
    TClonesArray *fCbmHitsArray;

    TString fGemConfigFile;
    TString fSilConfigFile;

    BmnGemStripStationSet *GemStationSet; // Entire GEM detector
    BmnSiliconStationSet *SilStationSet;  // Entire SILICON detector

    Bool_t fUseFixedErrors;

    ClassDef(BmnToCbmHitConverter, 1);
};

#endif
