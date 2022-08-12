#ifndef BMNTOCBMHITCONVERTER_H
#define BMNTOCBMHITCONVERTER_H 1

#include "BmnGemStripHit.h"
#include "BmnSiliconHit.h"
#include "CbmStsHit.h"
#include "BmnGemStripStationSet.h"
#include "BmnSiliconStationSet.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "TClonesArray.h"
#include "TSystem.h"

#include <iostream>


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

    void SetFixedErrors(Float_t dXgem = 0.015, Float_t dYgem = 0.058, Float_t dXsil = 0.003, Float_t dYsil = 0.021);

private:
    TString fBmnGemHitsBranchName;
    TString fBmnSilHitsBranchName;
    TString fCbmHitsBranchName;

    /** Input array of BNM GEM Hits **/
    TClonesArray *fBmnGemHitsArray;         //!
    TClonesArray *fBmnGemUpperClusters;     //!
    TClonesArray *fBmnGemLowerClusters;     //!

    /** Input array of BNM Silicon Hits **/
    TClonesArray *fBmnSilHitsArray;         //!
    TClonesArray *fBmnSilUpperClusters;     //!
    TClonesArray *fBmnSilLowerClusters;     //!

    /** Output array of CBM Hits **/
    TClonesArray *fCbmHitsArray;            //!

    TString fGemConfigFile;
    TString fSilConfigFile;

    BmnGemStripStationSet *GemStationSet;   //! Entire GEM detector
    BmnSiliconStationSet *SilStationSet;    //! Entire SILICON detector

    Bool_t fUseFixedErrors;
    Float_t fDXgem;
    Float_t fDYgem;
    Float_t fDXsil;
    Float_t fDYsil;    

  ClassDef(BmnToCbmHitConverter, 1);
};

#endif
