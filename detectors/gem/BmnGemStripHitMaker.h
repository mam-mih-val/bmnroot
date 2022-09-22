#ifndef BMNGEMSTRIPHITMAKER_H
#define BMNGEMSTRIPHITMAKER_H 1

#include "BmnTask.h"
#include "BmnGemStripDigit.h"
#include "BmnGemStripHit.h"
#include "BmnGemStripStationSet.h"
#include "BmnGemStripConfiguration.h"
#include "BmnGemStripTransform.h"

#include "FairMCPoint.h"
#include "FairField.h"
#include "FairHit.h"

#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class BmnGemStripHitMaker : public BmnTask
{
  public:
    BmnGemStripHitMaker();
    BmnGemStripHitMaker(Int_t per, Int_t run, Bool_t isExp, Bool_t isSrc = kFALSE);
    virtual ~BmnGemStripHitMaker();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnGemStripConfiguration::GEM_CONFIG config) {
        fCurrentConfig = config;
    }

    void SetSrcSetup(Bool_t f) {
        fIsSrc = f;
    }

    void SetFieldScale(Double_t fs) {
        fFieldScale = fs;
    }

    void SetSignalWindow(Double_t min, Double_t max) {
        fSignalLow = min;
        fSignalUp = max;
    }

    virtual InitStatus OnlineInit();
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree);
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree);
    virtual void SetField(const std::unique_ptr<FairField> &magneticField) { fField = magneticField.get(); }

  private:
    void createGemDetector();

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;

    /** Input array of Gem Points **/
    TClonesArray* fBmnGemStripPointsArray;          //!
    TClonesArray* fBmnGemStripDigitsArray;          //!
    TClonesArray* fBmnGemStripDigitMatchesArray;    //!

    /** Output array of Gem Hits **/
    TClonesArray* fBmnGemStripHitsArray;            //!
    /** Output array of Gem Upper Clusters **/
    TClonesArray* fBmnGemUpperClustersArray;        //!
    /** Output array of Gem Lower Clusters **/
    TClonesArray* fBmnGemLowerClustersArray;        //!

    Int_t fRunPeriod;
    Int_t fRunNumber;

    Bool_t fHitMatching;
    Bool_t fIsExp; // Specify type of input data (MC or real data)
    Bool_t fIsSrc; // Specify type of setup (SRC or BM@N)

    BmnGemStripConfiguration::GEM_CONFIG fCurrentConfig;

    BmnGemStripStationSet* StationSet;  //! Entire GEM detector

    BmnGemStripTransform* TransfSet;    //! Transformations for each module of the detector

    FairField* fField;                  //!
    Double_t fFieldScale;

    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;        //!
    
    Double_t*** fAlignCor;              //!
    Double_t** fLorCor;                 //!

    Double_t fSignalLow;
    Double_t fSignalUp;

    ClassDef(BmnGemStripHitMaker, 1);
};

#endif
