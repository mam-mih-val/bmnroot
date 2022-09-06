#ifndef BMNSiBTHITMAKER_H
#define BMNSiBTHITMAKER_H 1

#include "BmnTask.h"
#include "BmnSiBTDigit.h"
#include "BmnSiBTHit.h"
#include "BmnSiBTStationSet.h"
#include "BmnSiBTConfiguration.h"
#include "BmnSiBTTransform.h"
#include <BmnEventQuality.h>

#include "FairField.h"
#include "FairMCPoint.h"

#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class BmnSiBTHitMaker : public BmnTask {
public:

    BmnSiBTHitMaker();
    BmnSiBTHitMaker(Int_t, Int_t, Bool_t, TString alignFile = "default");

    virtual ~BmnSiBTHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnSiBTConfiguration::SiBT_CONFIG config) {
        fCurrentConfig = config;
    }

    virtual InitStatus OnlineInit();
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree);
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree);
    virtual void SetField(const std::unique_ptr<FairField> &magneticField) { fField = magneticField.get(); }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;
    TString fOutputHitMatchesBranchName;

    /** Input array of SiBT Points **/
    TClonesArray* fBmnSiBTPointsArray;          //!
    TClonesArray* fBmnSiBTDigitsArray;          //!
    TClonesArray* fBmnSiBTDigitMatchesArray;    //!

    /** Output array of SiBT Hits **/
    TClonesArray* fBmnSiBTHitsArray;            //!

    /** Output array of SiBT Hit Matches **/
    TClonesArray* fBmnSiBTHitMatchesArray;      //!

    Bool_t fHitMatching;
    Bool_t fIsExp; // Specify type of input data (MC or real data)

    BmnSiBTConfiguration::SiBT_CONFIG fCurrentConfig;

    BmnSiBTStationSet *StationSet;  //!Entire SiBT detector

    BmnSiBTTransform *TransfSet;    //! Transformations for each module of the detector

    FairField* fField;              //!

    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;    //!

    void LoadDetectorConfiguration();

  ClassDef(BmnSiBTHitMaker, 1);
};

#endif
