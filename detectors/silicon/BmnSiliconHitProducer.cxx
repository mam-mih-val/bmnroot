#include "BmnSiliconHitProducer.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"

#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "BmnSiliconHit.h"
#include "TSystem.h"

using std::cout;
using namespace TMath;

BmnSiliconHitProducer::BmnSiliconHitProducer() {
    fInputBranchName = "SiliconPoint";
    fOutputHitsBranchName = "BmnHit";
    fOutputSiliconHitMatchesBranchName = "BmnSiliconHitMatch";
}

BmnSiliconHitProducer::~BmnSiliconHitProducer() {


}

InitStatus BmnSiliconHitProducer::Init() {

    cout << " BmnSiliconHitProducer::Init() " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    ioman->Register("BmnSiliconHit", "SILICON", fBmnHitsArray, kTRUE);

    fBmnSiliconHitMatchesArray = new TClonesArray("BmnMatch");
    ioman->Register(fOutputSiliconHitMatchesBranchName, "SILICON", fBmnSiliconHitMatchesArray, kTRUE);

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    gPathConfig += "/parameters/silicon/XMLConfigs/";
    SiliconStationSet = new BmnSiliconStationSet(gPathConfig + "SiliconRunSpring2018.xml");

    return kSUCCESS;
}

void BmnSiliconHitProducer::Exec(Option_t* opt) {

    fBmnHitsArray->Delete();
    fBmnSiliconHitMatchesArray->Delete();

    if (!fBmnPointsArray) {
        Error("BmnSiliconHitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    Float_t err[3] = {0.03, 0.03, 0.03}; // Uncertainties of coordinates
    //    Float_t err[3] = {0.0, 0.0, 0.0}; // Uncertainties of coordinates

    for (Int_t iPoint = 0; iPoint < fBmnPointsArray->GetEntriesFast(); iPoint++) {

        TRandom* rand_gen = new TRandom();

        CbmStsPoint* siliconPoint = (CbmStsPoint*) fBmnPointsArray->UncheckedAt(iPoint);

        const Float_t dX = rand_gen->Gaus(0, err[0]);
        const Float_t dY = rand_gen->Gaus(0, err[1]);
        const Float_t dZ = rand_gen->Gaus(0, err[2]);

        const Float_t x_smeared = siliconPoint->GetXIn() + dX;
        const Float_t y_smeared = siliconPoint->GetYIn() + dY;
        const Float_t z_smeared = siliconPoint->GetZIn() + dZ;

        BmnHit* hit = new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), iPoint);
        //        BmnHit* hit = new ((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()]) BmnHit(kSILICON, TVector3(x_smeared, y_smeared, z_smeared), TVector3(1.0, 1.0, 0.0), iPoint);
        hit->SetIndex(fBmnHitsArray->GetEntriesFast() - 1);
        hit->SetType(1);
        hit->SetStation(SiliconStationSet->GetPointStationOwnership(siliconPoint->GetZIn()));

        BmnMatch* match = new ((*fBmnSiliconHitMatchesArray)[fBmnSiliconHitMatchesArray->GetEntriesFast()]) BmnMatch();
        match->AddLink(100.0, iPoint);

        delete rand_gen;
    }
}

void BmnSiliconHitProducer::Finish() {
}

ClassImp(BmnSiliconHitProducer)
