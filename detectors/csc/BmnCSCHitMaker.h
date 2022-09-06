#ifndef BMNCSCHITMAKER_H
#define BMNCSCHITMAKER_H 1

#include "BmnTask.h"
#include "BmnCSCDigit.h"
#include "BmnCSCHit.h"
#include "BmnCSCStationSet.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCTransform.h"

#include "FairMCPoint.h"
#include "FairField.h"

#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class BmnCSCHitMaker : public BmnTask {
  public:

    BmnCSCHitMaker();
    BmnCSCHitMaker(Int_t, Int_t, Bool_t, TString alignFile = "default", Bool_t isSrc = kFALSE);
    virtual ~BmnCSCHitMaker();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnCSCConfiguration::CSC_CONFIG config) {
        fCurrentConfig = config;
        configSetExplicitly = true;
    }

    virtual InitStatus OnlineInit();
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree);
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree);
    virtual void SetField(const std::unique_ptr<FairField> &magneticField) { fField = magneticField.get(); }

  private:
    void initCurrentConfig();

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;

    /** Input array of CSC Points **/
    TClonesArray* fBmnCSCPointsArray;           //!
    TClonesArray* fBmnCSCDigitsArray;           //!
    TClonesArray* fBmnCSCDigitMatchesArray;     //!

    /** Output array of CSC Hits **/
    TClonesArray* fBmnCSCHitsArray;             //!
    /** Output array of CSC Upper Clusters **/
    TClonesArray* fBmnCSCUpperClustersArray;    //!
    /** Output array of CSC Lower Clusters **/
    TClonesArray* fBmnCSCLowerClustersArray;    //!


    Bool_t fHitMatching;
    Bool_t fIsExp; // Specify type of input data (MC or real data)
    Bool_t fIsSrc; // Specify type of setup (SRC or BM@N)

    BmnCSCConfiguration::CSC_CONFIG fCurrentConfig;
    bool configSetExplicitly = false;

    BmnCSCStationSet *StationSet;   //! Entire CSC detector

    BmnCSCTransform *TransfSet;     //! Transformations for each module of the detector

    FairField* fField;              //!

    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;    //!

    void LoadDetectorConfiguration();

  ClassDef(BmnCSCHitMaker, 1);
};

#endif
