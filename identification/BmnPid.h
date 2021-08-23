#ifndef BMNPID_H
#define BMNPID_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TString.h"
#include "BmnGlobalTrack.h"
#include "TMath.h"
#include "BmnEnums.h"
#include "FairRunAna.h"
#include "vector"

using namespace std;

class BmnPid : public FairTask {
public:

    // Constructors/Destructors ---------
    BmnPid(vector<TParticlePDG*> particles, Int_t power);

    virtual ~BmnPid();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetWeights();


private:

    // Private Methods -------------
    Double_t EvalSimpleWeight(Double_t p, Double_t beta, Double_t mass, Int_t power);
    
    // Private Data Members ------------
    TString fGlobalTracksBranchName; 

    Int_t fModelPower; // number of using identification model
                       // 1 - first order distance model
                       // 2 - second order distance model
                       // 3 - third order distance model
                       // ...
                            
    Int_t fEventNo; // event counter
    
    TClonesArray* fGlobalTracksArray;
    vector<TParticlePDG*> fParticles; // additional array

    ClassDef(BmnPid, 1);
};


#endif /* BMNPID_H */


