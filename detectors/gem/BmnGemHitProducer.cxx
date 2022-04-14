#include "BmnGemHitProducer.h"
#include "CbmStsPoint.h"
#include "BmnHit.h"

#include "TRandom.h"
#include "TSystem.h"

using std::cout;

BmnGemHitProducer::BmnGemHitProducer() {
    fInputBranchName = "StsPoint";
    fOutputHitsBranchName = "BmnHit";
    fOutputGemHitMatchesBranchName = "BmnGemStripHitMatch";
}

BmnGemHitProducer::~BmnGemHitProducer() {
}

InitStatus BmnGemHitProducer::Init() {

    cout << " BmnGemHitProducer::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    ioman->Register("BmnGemStripHit", "GEM", fBmnHitsArray, kTRUE);

    fBmnGemStripHitMatchesArray = new TClonesArray("BmnMatch");
    ioman->Register(fOutputGemHitMatchesBranchName, "GEM", fBmnGemStripHitMatchesArray, kTRUE);

    TString gPathGemConfig = gSystem->Getenv("VMCWORKDIR");
    gPathGemConfig += "/parameters/gem/XMLConfigs/";
    GemStationSet = new BmnGemStripStationSet(gPathGemConfig + "GemRunSpring2018.xml");

    return kSUCCESS;
}

void BmnGemHitProducer::Exec(Option_t* opt) {

    fBmnHitsArray->Delete();
    fBmnGemStripHitMatchesArray->Delete();

    if (!fBmnPointsArray) {
        Error("BmnGemHitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    Float_t err[3] = {0.03, 0.03, 0.03}; // Uncertainties of coordinates
//    Float_t err[3] = {0.0, 0.0, 0.0}; // Uncertainties of coordinates

    for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

        TRandom* rand_gen = new TRandom();

        CbmStsPoint* gemPoint = (CbmStsPoint*) fBmnPointsArray->UncheckedAt(iPoint);

        const Float_t dX = rand_gen->Gaus(0, err[0]);
        const Float_t dY = rand_gen->Gaus(0, err[1]);
        const Float_t dZ = rand_gen->Gaus(0, err[2]);

        const Float_t x_smeared = gemPoint->GetXIn() + dX;
        const Float_t y_smeared = gemPoint->GetYIn() + dY;
        const Float_t z_smeared = gemPoint->GetZIn() + dZ;

        BmnHit* hit = new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), iPoint);
//        BmnHit* hit = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()]) BmnHit(kGEM, TVector3(x_smeared, y_smeared, z_smeared), TVector3(1.0, 1.0, 0.0), iPoint);
        hit->SetIndex(fBmnHitsArray->GetEntriesFast() - 1);
        hit->SetType(1);
        hit->SetStation(GemStationSet->GetPointStationOwnership(gemPoint->GetZIn()));
//        hit->SetStation(DefineStationByZ(gemPoint->GetZIn()));

        BmnMatch* match = new ((*fBmnGemStripHitMatchesArray)[fBmnGemStripHitMatchesArray->GetEntriesFast()]) BmnMatch();
        match->AddLink(100.0, iPoint);

        delete rand_gen;
    }
}

Int_t BmnGemHitProducer::DefineStationByZ(Double_t z) {
    const Int_t nStation = 7;
    Int_t minIdx = -1;
    Double_t minDz = 10000;

    Double_t zPos[nStation] = {100.0, 120.0, 140.0, 160.0, 180.0, 200.0, 220.0};

    for (Int_t i = 0; i < nStation; ++i) {
        Double_t dz = Abs(z - zPos[i]);
        if (dz < minDz) {
            minDz = dz;
            minIdx = i;
        }
    }
    return minIdx;
}

void BmnGemHitProducer::Finish() {
}

ClassImp(BmnGemHitProducer)
