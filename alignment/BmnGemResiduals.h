#ifndef BMNGEMRESIDUALS_H
#define BMNGEMRESIDUALS_H 1

#include <fstream>
#include <sstream>
#include <TMath.h>
#include <TNamed.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TH1.h>
#include <TFitResult.h>
#include "FairTask.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"

#include  "BmnGemStripStationSet.h"
#include  "BmnGemStripStationSet_RunWinter2016.h"
#include  "BmnGemStripStationSet_RunSpring2017.h"

#include "BmnResiduals.h"
#include "BmnGemTrack.h"
#include "BmnGemStripHit.h"
#include "BmnMath.h"

using namespace std;
using namespace TMath;

class BmnGemResiduals : public FairTask {
public:

    BmnGemResiduals() {
    };
    BmnGemResiduals(Int_t period = 7, Int_t number = -1, Double_t scale = 0.);

    virtual ~BmnGemResiduals() {
    };

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();


    void SetUseDistance(Bool_t flag) {
        if (flag)
            isResid = kFALSE;
    }

    void SetPrintResToFile(TString fileName) {
        isPrintToFile = kTRUE;
        outRes = fopen(fileName.Data(), "w");
    }

  private:
    void ResidualsAndDistances();
    
    Int_t fPeriod;
    Int_t fNumber;

    Bool_t fDebug;

    Bool_t isField;
    Bool_t isResid; // true --> resid, false --> dist

    TString fBranchGemHits;
    TString fBranchGemTracks;
    TString fBranchResiduals;
    TString fBranchFairEventHeader;

    TClonesArray* fGemTracks;
    TClonesArray* fGemHits;

    TClonesArray* fGemResiduals;
    
    FairEventHeader* fFairEventHeader;

    BmnGemStripStationSet* fDetector; // Detector geometry

    Bool_t isPrintToFile;
    Bool_t isMergedDigits; // file with merged digits from different files
    FILE* outRes;

    ClassDef(BmnGemResiduals, 1)
};

#endif
