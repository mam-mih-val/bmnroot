#ifndef BMNREALMC_H
#define BMNREALMC_H 1

#include <iostream>
#include <FairTask.h>
#include <TClonesArray.h>
#include <TRandom.h>

#include <BmnGemStripHit.h>
#include <BmnSiliconHit.h>
#include <BmnMatch.h>
#include <BmnEfficiencyProbability.h>
#include <BmnEfficiency.h>

using namespace std;

class BmnRealisticMc : public FairTask {
public:

    BmnRealisticMc();

    virtual ~BmnRealisticMc() {
        if (fEff)
            delete fEff;
    }

    virtual void Exec(Option_t * option);

    virtual InitStatus Init();

    virtual void Finish();


private:
    
    TClonesArray* fSilHitsArray;
    TClonesArray* fGemHitsArray;
    
    TClonesArray* fGemHitsArrayFiltered;
    TClonesArray* fSilHitsArrayFiltered;


   // Support utils ...
   BmnEfficiency* fEff;
   
   void doHitSelection(TClonesArray*, TClonesArray*);


    ClassDef(BmnRealisticMc, 0)
};



#endif
