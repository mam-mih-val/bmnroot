#include "BmnSSDHitProducer.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"

#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TSystem.h"
#include "BmnMatch.h"

using std::cout;
using namespace TMath;

BmnSSDHitProducer::BmnSSDHitProducer() {
    fInputBranchName = "SSDPoint";
    fOutputHitsBranchName = "BmnHit";
    fOutputSSDHitMatchesBranchName = "BmnSSDHitMatch";
    fFakesFraction = 0.0;
}

BmnSSDHitProducer::~BmnSSDHitProducer() {


}

InitStatus BmnSSDHitProducer::Init() {

    cout << " BmnSSDHitProducer::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    ioman->Register("BmnSSDHit", "SSD", fBmnHitsArray, kTRUE);

    fBmnSSDHitMatchesArray = new TClonesArray("BmnMatch");
    ioman->Register(fOutputSSDHitMatchesBranchName, "SSD", fBmnSSDHitMatchesArray, kTRUE);

    return kSUCCESS;
}

void BmnSSDHitProducer::Exec(Option_t* opt) {

    fBmnHitsArray->Delete();
    fBmnSSDHitMatchesArray->Delete();

    if (!fBmnPointsArray) {
        Error("BmnSSDHitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    Float_t err[3] = {0.01, 0.01, 0.01}; // Uncertainties of coordinates

    Int_t ssdType = 2;

    for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

        TRandom* rand_gen = new TRandom(0);

        CbmStsPoint* ssdPoint = (CbmStsPoint*) fBmnPointsArray->UncheckedAt(iPoint);

        const Float_t dX = rand_gen->Gaus(0, err[0]);
        const Float_t dY = rand_gen->Gaus(0, err[1]);
        const Float_t dZ = rand_gen->Gaus(0, err[2]);

        const Float_t x_smeared = ssdPoint->GetXIn() + dX;
        const Float_t y_smeared = ssdPoint->GetYIn() + dY;
        const Float_t z_smeared = ssdPoint->GetZIn() + dZ;

        BmnHit* hit = new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()]) BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), iPoint);
        hit->SetIndex(fBmnHitsArray->GetEntriesFast() - 1);
        hit->SetType(1);
        hit->SetStation(DefineStationByZ(ssdPoint->GetZIn(), ssdType));

        BmnMatch* match = new ((*fBmnSSDHitMatchesArray)[fBmnSSDHitMatchesArray->GetEntriesFast()]) BmnMatch();
        match->AddLink(100.0, iPoint);

        delete rand_gen;
    }
    
    const Int_t nStations = 4;
    const Int_t nGeoms = 3;
    Double_t xStation[nStations] = {25.0, 25.0, 35.0, 40.0};
    Double_t yStation[nStations] = {10.0, 15.0, 15.0, 20.0};
    Double_t zPos[nGeoms][nStations] = {
        {30, 40, 50, 60},
        {30, 45, 60, 75},
        {30, 50, 70, 90}
    };

    for (Int_t iSt = 0; iSt < nStations; ++iSt) {
        Int_t nHitsOnStation = 0;
        for (Int_t iHit = 0; iHit < fBmnHitsArray->GetEntriesFast(); iHit++)
            if (((BmnHit*) (fBmnHitsArray->At(iHit)))->GetStation() == iSt)
                nHitsOnStation++;
        Int_t nFakesOnStation = Int_t(nHitsOnStation * fFakesFraction);
        for (Int_t iFake = 0; iFake < nFakesOnStation; ++iFake) {
            TRandom* rand_gen = new TRandom(0);
            const Float_t xFake = rand_gen->Uniform(-xStation[iSt], xStation[iSt]);
            const Float_t yFake = rand_gen->Uniform(-yStation[iSt], yStation[iSt]);
            const Float_t zFake = zPos[ssdType][iSt];
            BmnHit* hit = new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()]) BmnHit(0, TVector3(xFake, yFake, zFake), TVector3(err[0], err[1], err[2]), -1);
            hit->SetIndex(fBmnHitsArray->GetEntriesFast() - 1);
            hit->SetStation(DefineStationByZ(zFake, ssdType));
            BmnMatch* match = new ((*fBmnSSDHitMatchesArray)[fBmnSSDHitMatchesArray->GetEntriesFast()]) BmnMatch();
            match->AddLink(100.0, rand_gen->Uniform(0, fBmnPointsArray->GetEntriesFast()));
            delete rand_gen;
        }
    }
}

void BmnSSDHitProducer::Finish() {
}

Int_t BmnSSDHitProducer::DefineStationByZ(Double_t z, Int_t ssdType) {
    const Int_t nGeoms = 3;
    const Int_t nStation = 4;
    Int_t minIdx = -1;
    Double_t minDz = 10000;

    Double_t zPos[nGeoms][nStation] = {
        {30, 40, 50, 60},
        {30, 45, 60, 75},
        {30, 50, 70, 90}
    };

    for (Int_t i = 0; i < nStation; ++i) {
        Double_t dz = Abs(z - zPos[ssdType][i]);
        if (dz < minDz) {
            minDz = dz;
            minIdx = i;
        }
    }
    return minIdx;
}

ClassImp(BmnSSDHitProducer)
