#include "BmnDchHitProducer.h"
#include "CbmMCTrack.h"

#include "TRandom.h"

#include "BmnDchPoint.h"
#include "BmnDchHit.h"

using std::cout;

BmnDchHitProducer::BmnDchHitProducer() {
    fInputBranchName = "DCHPoint";
    fOutputHitsBranchName = "BmnDchHit";
}

BmnDchHitProducer::~BmnDchHitProducer() {
}

InitStatus BmnDchHitProducer::Init() {

    cout << " BmnDchHitProducer::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    ioman->Register(fOutputHitsBranchName, "DCH", fBmnHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnDchHitProducer::Exec(Option_t* opt) {

    fBmnHitsArray->Delete();

    if (!fBmnPointsArray) {
        Error("BmnDchHitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    Float_t err[3] = {0.03, 0.03, 0.03}; // Uncertainties of coordinates
//    Float_t err[3] = {0.0, 0.0, 0.0}; // Uncertainties of coordinates

    for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

        TRandom* rand_gen = new TRandom();

        BmnDchPoint* dchPoint = (BmnDchPoint*) fBmnPointsArray->UncheckedAt(iPoint);

        const Float_t dX = rand_gen->Gaus(0, err[0]);
        const Float_t dY = rand_gen->Gaus(0, err[1]);
        const Float_t dZ = rand_gen->Gaus(0, err[2]);

        const Float_t x_smeared = dchPoint->GetX() + dX;
        const Float_t y_smeared = dchPoint->GetY() + dY;
        const Float_t z_smeared = dchPoint->GetZ() + dZ;


        new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])
               BmnDchHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), iPoint);

        BmnDchHit* hit = (BmnDchHit*) fBmnHitsArray->At(fBmnHitsArray->GetEntriesFast() - 1);

        delete rand_gen;
    }
}

void BmnDchHitProducer::Finish() {
}

ClassImp(BmnDchHitProducer)
