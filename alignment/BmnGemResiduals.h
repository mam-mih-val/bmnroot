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

    BmnGemResiduals() {};
    BmnGemResiduals(Double_t);
    virtual ~BmnGemResiduals() {};

    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();
    
    void SetUseDistance(Bool_t flag) {
        if (flag)
            isResid = kFALSE;
    }

   
private:
    void Residuals();
    void Distances();
    
    Bool_t fDebug;
        
    Bool_t isField;
    Bool_t isResid; // true --> resid, false --> dist
    
    TString fBranchGemHits;
    TString fBranchGemTracks;
    TString fBranchResiduals;
    
    TClonesArray* fGemTracks;
    TClonesArray* fGemHits;
    
    TClonesArray* fGemResiduals;
      
    BmnGemStripStationSet* fDetector; // Detector geometry
    BmnGemStripConfiguration::GEM_CONFIG fGeometry;
    
    // tmp histos to fit resid. 
    TH1F* hRes[6][2][2]; // stat -- mod -- resX (Y)
  
    ClassDef(BmnGemResiduals, 1)
};

#endif