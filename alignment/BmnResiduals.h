#ifndef BMNRESIDUALS_H
#define BMNRESIDUALS_H 1

#include "FairTask.h"
#include "FairRootManager.h"
#include "FairEventHeader.h"
#include "BmnGemTrack.h"
#include "BmnGlobalTrack.h"
#include "BmnHit.h"
#include "BmnMath.h"
#include "BmnSiliconHit.h"
#include "BmnGemStripHit.h"
#include "BmnMath.h"

#include <TString.h>
#include <TClonesArray.h>

using namespace std;

class BmnResiduals : public FairTask {
public:

    BmnResiduals() {};
    BmnResiduals(Int_t, Int_t);
    
    virtual InitStatus Init();

    virtual void Exec(Option_t* opt);

    virtual void Finish();

    virtual ~BmnResiduals() {
    };


private: 
    Int_t fPeriod;
    Int_t fNumber;
    
    TString fBranchGlobTracks;
    TClonesArray* fGlobalTracks;     //!
    
    TString fBranchGemHits;
    TClonesArray* fGemHits;          //!
    
    TString fBranchSilHits;
    TClonesArray* fSilHits;          //!
    
    TString fBranchGemTracks;
    TClonesArray* fGemTracks;        //!
    
    TString fBranchSilTracks;
    TClonesArray* fSilTracks;        //!
    
    TString fBranchFairEventHeader;
    FairEventHeader* fFairEventHeader;   //!

    ClassDef(BmnResiduals, 1)
};

#endif
