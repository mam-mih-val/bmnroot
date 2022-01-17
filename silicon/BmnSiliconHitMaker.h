#ifndef BMNSILICONHITMAKER_H
#define BMNSILICONHITMAKER_H

#include "Rtypes.h"

#include "FairTask.h"
#include "FairMCPoint.h"

#include "BmnSiliconDigit.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "BmnSiliconStation.h"
#include "BmnSiliconModule.h"
#include "BmnSiliconLayer.h"
#include <BmnEventQuality.h>

#include "BmnSiliconConfiguration.h"

using namespace std;
using namespace TMath;

class BmnSiliconHitMaker : public FairTask {
public:

    BmnSiliconHitMaker();
    BmnSiliconHitMaker(Int_t run_period, Int_t run_number, Bool_t isExp, Bool_t isSrc = kFALSE);

    virtual ~BmnSiliconHitMaker();

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    void ProcessDigits();

    void SetHitMatching(Bool_t opt = kTRUE) {
        fHitMatching = opt;
    }

    void SetCurrentConfig(BmnSiliconConfiguration::SILICON_CONFIG config) {
        fCurrentConfig = config;
    }

    void SetSignalWindow(Double_t min, Double_t max) {
        fSignalLow = min;
        fSignalUp = max;
    }

private:

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnSiliconPointsArray;
    TClonesArray* fBmnSiliconDigitsArray;
    TClonesArray* fBmnSiliconDigitMatchesArray;

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnSiliconHitsArray;

    /** Output array of Silicon Upper Clusters **/
    TClonesArray* fBmnSiliconUpperClustersArray;
    /** Output array of Silicon Lower Clusters **/
    TClonesArray* fBmnSiliconLowerClustersArray;

    Bool_t fHitMatching;

    BmnSiliconConfiguration::SILICON_CONFIG fCurrentConfig;

    BmnSiliconStationSet *StationSet; //Entire Silicon detector  

    Bool_t fIsExp;
    Bool_t fIsSrc;
    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;
    
    Double_t*** fAlignCor;

    Double_t fSignalLow; 
    Double_t fSignalUp;

    ClassDef(BmnSiliconHitMaker, 1);
};

#endif /* BMNSILICONHITMAKER_H */

