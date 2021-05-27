#include "BmnArmTrigHitProducer.h"
#include "BmnArmTrigPoint.h"
#include "FairRootManager.h"
#include "TSystem.h"



static Float_t workTime = 0.0;
static int entries = 0;

 BmnArmTrigHitProducer:: BmnArmTrigHitProducer()
: fOnlyPrimary(kFALSE), fUseRealEffects(kFALSE){
    fInputBranchName = "ArmTrigPoint";
    fOutputHitsBranchName = "BmnArmTrigHit";
    fOutputHitMatchesBranchName = "BmnArmTrigHitMatch";

    fVerbose = 1;
}

 BmnArmTrigHitProducer::~ BmnArmTrigHitProducer() {
}

InitStatus  BmnArmTrigHitProducer::Init() {

    FairRootManager* ioman = FairRootManager::Instance();

    fBmnArmTrigPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);

    fBmnArmTrigHitsArray = new TClonesArray(fOutputHitsBranchName);
    ioman->Register(fOutputHitsBranchName, "ArmTrig", fBmnArmTrigHitsArray, kTRUE);


    //if (fStripMatching) {
    //    new ((*fBmnSiliconHitMatchesArray)[fBmnArmTrigHitMatchesArray->GetEntriesFast()])
    //            BmnMatch(module->GetStripMatchInZone(zone_id, iStrip));
    //}


    if (fVerbose) cout << " BmnArmTrigHitProducer::Init() finished\n\n";
    return kSUCCESS;
}

void  BmnArmTrigHitProducer::Exec(Option_t* opt) {
    clock_t tStart = clock();
    fBmnArmTrigHitsArray->Delete();
    BmnArmTrigPoint* ArmTrigPoint;
    for (UInt_t ipoint = 0; ipoint < fBmnArmTrigPointsArray->GetEntriesFast(); ipoint++) {
        ArmTrigPoint = (BmnArmTrigPoint*) fBmnArmTrigPointsArray->At(ipoint);
                      
        new ((*fBmnArmTrigHitsArray)[fBmnArmTrigHitsArray->GetEntriesFast()])
            BmnArmTrigHit(ArmTrigPoint->GetDetectorID(), TVector3(ArmTrigPoint->GetX(), ArmTrigPoint->GetY(), ArmTrigPoint->GetZ()), TVector3(0,0,0), -1, 
            ArmTrigPoint->GetPdgId() );
    }


    if (!fBmnArmTrigPointsArray) {
        Error(" BmnArmTrigHitProducer::Exec()", " !!! Unknown branch name !!! ");
        return;
    }

    if (fVerbose) {
        cout << "\n  BmnArmTrigHitProducer::Exec(), event = " << entries << "\n";
        cout << "  BmnArmTrigHitProducer::Exec(), Number of BmnArmTrigPoints = " << fBmnArmTrigPointsArray->GetEntriesFast() << "\n";
    }




    if (fVerbose) cout << "  BmnArmTrigHitProducer::Exec() finished\n\n";
    entries++;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}


void  BmnArmTrigHitProducer::Finish() {

    cout << "Work time of the ArmTrig HitProducer: " << workTime << endl;
}

ClassImp( BmnArmTrigHitProducer)


