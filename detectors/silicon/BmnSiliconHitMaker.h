#ifndef BMNSILICONHITMAKER_H
#define BMNSILICONHITMAKER_H

#include "BmnTask.h"
#include "BmnSiliconDigit.h"
#include "BmnSiliconHit.h"
#include "BmnSiliconStationSet.h"
#include "BmnSiliconStation.h"
#include "BmnSiliconModule.h"
#include "BmnSiliconLayer.h"
#include "BmnSiliconConfiguration.h"
#include "BmnSiliconTransform.h"

#include "FairMCPoint.h"

using namespace std;
using namespace TMath;

class BmnSiliconHitMaker : public BmnTask
{
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

    virtual InitStatus OnlineInit();
    virtual InitStatus OnlineRead(const std::unique_ptr<TTree> &dataTree, const std::unique_ptr<TTree> &resultTree);
    virtual void OnlineWrite(const std::unique_ptr<TTree> &dataTree);

  private:
    void createSiliconDetector();

    Int_t fRunPeriod;
    Int_t fRunNumber;

    TString fInputPointsBranchName;
    TString fInputDigitsBranchName;
    TString fInputDigitMatchesBranchName;

    TString fOutputHitsBranchName;

    /** Input array of Silicon Points **/
    TClonesArray* fBmnSiliconPointsArray;           //!
    TClonesArray* fBmnSiliconDigitsArray;           //!
    TClonesArray* fBmnSiliconDigitMatchesArray;     //!

    /** Output array of Silicon Hits **/
    TClonesArray* fBmnSiliconHitsArray;             //!

    /** Output array of Silicon Upper Clusters **/
    TClonesArray* fBmnSiliconUpperClustersArray;    //!
    /** Output array of Silicon Lower Clusters **/
    TClonesArray* fBmnSiliconLowerClustersArray;    //!

    Bool_t fHitMatching;

    BmnSiliconConfiguration::SILICON_CONFIG fCurrentConfig;
    BmnSiliconStationSet *StationSet;               //! Entire detector
    BmnSiliconTransform *TransfSet;                 //! Transformations for detector modules

    Bool_t fIsExp;
    Bool_t fIsSrc;
    TString fBmnEvQualityBranchName;
    TClonesArray* fBmnEvQuality;                    //!

    Double_t*** fAlignCor;                          //!

    Double_t fSignalLow;
    Double_t fSignalUp;

  ClassDef(BmnSiliconHitMaker, 1);
};

#endif /* BMNSILICONHITMAKER_H */
