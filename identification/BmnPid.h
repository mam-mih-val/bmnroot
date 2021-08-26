#ifndef BMNPID_H
#define BMNPID_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
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
    BmnPid(Int_t power = 1);

    virtual ~BmnPid();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetWeights();

    


private:
    // Private Methods -------------

    Int_t EnumToPdg(PidParticles part);
    
    void NormalizeWeights(vector<Double_t>& vec);
    Double_t GetSum(const vector<Double_t>& vec);
    
    
    Double_t EvalSimpleWeight(Double_t p, Double_t beta, Double_t mass, Int_t power);
    
    // Private Data Members ------------
    TString fGlobalTracksBranchName; 
    TDatabasePDG* db = nullptr;

    Int_t fModelPower; // number of using identification model
                       // 1 - first order distance model
                       // 2 - second order distance model
                       // 3 - third order distance model
                       // ...
                            
    Int_t fEventNo; // event counter
    
    TClonesArray* fGlobalTracksArray;
   

    ClassDef(BmnPid, 1);
};


#endif /* BMNPID_H */


