#ifndef BMNSIRESIDUALS_H
#define BMNSIRESIDUALS_H 1

#include <fstream>
#include <vector>
#include <sstream>
#include <TMath.h>
#include <TNamed.h>
#include "TSystem.h"
#include <TClonesArray.h>
#include <TString.h>
#include <TH1.h>
#include <TFitResult.h>
#include "FairTask.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"

#include "BmnResiduals.h"
#include "BmnMath.h"
#include <BmnSiliconStationSet.h>
#include <BmnSiliconHit.h>
#include <BmnGlobalTrack.h>

using namespace std;
using namespace TMath;

class BmnSiResiduals : public FairTask {
public:

    BmnSiResiduals() {
    };
    BmnSiResiduals(Int_t period = 7, Int_t number = -1, Double_t scale = 0.);

    virtual ~BmnSiResiduals() {
    };

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

//    void SetUseDistance(Bool_t flag) {
//        if (flag)
//            isResid = kFALSE;
//    }
//
//    void SetPrintResToFile(TString fileName) {
//        isPrintToFile = kTRUE;
//        outRes = fopen(fileName.Data(), "w");
//    }

  private:
    void Residuals();
    
    Int_t fPeriod;
    Int_t fNumber;

    Bool_t fDebug;

    Bool_t isField;

    TString fBranchSiHits;
//    TString fBranchGemHits;
    TString fBranchGlobalTracks;
//    TString fBranchGemTracks;
    TString fBranchResiduals;
    TString fBranchFairEventHeader;

    TClonesArray* fGlobalTracks;
    //TClonesArray* fGemTracks;
    //TClonesArray* fGemHits;
    TClonesArray* fSiHits;

    TClonesArray* fSiResiduals;
    
    FairEventHeader* fFairEventHeader;

    BmnSiliconStationSet* fDetector;

    Bool_t isPrintToFile;
    Bool_t isMergedDigits; // file with merged digits from different files
    FILE* outRes;

    ClassDef(BmnSiResiduals, 1)
};

#endif
