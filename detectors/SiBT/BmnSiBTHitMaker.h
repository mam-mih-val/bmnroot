#ifndef BMNSiBTHITMAKER_H
#define BMNSiBTHITMAKER_H 1

#include <iostream>
#include <fstream>
#include <sstream>

#include "Rtypes.h"
#include "TClonesArray.h"
#include "TRegexp.h"
#include "TString.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "FairField.h"
#include "BmnSiBTDigit.h"
#include "BmnSiBTHit.h"
#include "BmnSiBTStationSet.h"
#include "BmnSiBTConfiguration.h"
#include "BmnSiBTTransform.h"
#include <BmnEventQuality.h>

using namespace std;

class BmnSiBTHitMaker : public FairTask {
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

    ClassDef(BmnSiBTHitMaker, 1);
};


#endif
