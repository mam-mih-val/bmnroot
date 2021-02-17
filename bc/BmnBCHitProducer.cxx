#include "BmnBCHitProducer.h"
#include "FairRootManager.h"
#include "BmnBCPoint.h"
#include "TSystem.h"



static Float_t workTime = 0.0;
static int entries = 0;

 BmnBCHitProducer:: BmnBCHitProducer()
: fOnlyPrimary(kFALSE), fUseRealEffects(kFALSE){
    fInputBranchName = "BCPoint";
    fOutputHitsBranchName = "BmnBCHit";
    fOutputHitMatchesBranchName = "BmnBCHitMatch";

    fVerbose = 1;
}

 BmnBCHitProducer::~ BmnBCHitProducer() {
}

InitStatus  BmnBCHitProducer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnBCPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);

    fBmnBCHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "BC", fBmnBCHitsArray, kTRUE);

    if (fVerbose) cout << " BmnBCHitProducer::Init() finished\n\n";
    return kSUCCESS;
}

void  BmnBCHitProducer::Exec(Option_t* opt) {
    clock_t tStart = clock();
    fBmnBCHitsArray->Delete();
    BmnBCPoint* BCPoint;
    Double_t ELoss1 = 0;
    Double_t ELoss2 = 0;
    Double_t ELoss3 = 0;
    Double_t ELoss4 = 0;
    Int_t NumberofHits = 0;
    
    for (UInt_t ipoint = 0; ipoint < fBmnBCPointsArray->GetEntriesFast(); ipoint++) {
        BCPoint = (BmnBCPoint*) fBmnBCPointsArray->At(ipoint);
        if(BCPoint->GetStation() == 1){ ELoss1 += BCPoint->GetEnergyLoss(); }
        else if (BCPoint->GetStation() == 2) { ELoss2 += BCPoint->GetEnergyLoss();}
        else if (BCPoint->GetStation() == 3) { ELoss3 += BCPoint->GetEnergyLoss();}
        else if (BCPoint->GetStation() == 4) { ELoss4 += BCPoint->GetEnergyLoss();}
        NumberofHits++;
    }
    new ((*fBmnBCHitsArray)[fBmnBCHitsArray->GetEntriesFast()])
            BmnBCHit(ELoss1,ELoss2,ELoss3,ELoss4, NumberofHits);
    //vector <Double_t> ELoss = {ELoss1, ELoss2, ELoss3, ELoss4};
    
 

    if (!fBmnBCPointsArray) {
        Error(" BmnBCHitProducer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << "\n  BmnBCHitProducer::Exec(), event = " << entries << "\n";
        cout << "  BmnBCHitProducer::Exec(), Number of BmnBCPoints = " << fBmnBCPointsArray->GetEntriesFast() << "\n";
    }

    if (fVerbose) cout << "  BmnBCHitProducer::Exec() finished\n\n";
    entries++;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

}


void  BmnBCHitProducer::Finish() {

    cout << "Work time of the BC HitProducer: " << workTime << endl;
}

ClassImp( BmnBCHitProducer)


