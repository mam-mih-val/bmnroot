#include "BmnGemHitProducer.h"
#include "CbmStsPoint.h"
#include "CbmMCTrack.h"

#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"

using std::cout;
using namespace TMath;

BmnGemHitProducer::BmnGemHitProducer() :
fOnlyPrimary(kFALSE) {

    fInputBranchName = "StsPoint";
    fTracksBranchName = "BmnGemTrack";
    fOutputHitsBranchName = "BmnHit";
}

BmnGemHitProducer::~BmnGemHitProducer() {

}

InitStatus BmnGemHitProducer::Init() {

    cout << " BmnGemHitProducer::Init() " << endl;
    if (fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");

    fBmnTracksArray = new TClonesArray(fTracksBranchName, 100);
    // ioman->Register("StsTrack", "STS", fBmnTracksArray, kTRUE);
    fBmnHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    ioman->Register("BmnGemStripHit", "GEM", fBmnHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnGemHitProducer::Exec(Option_t* opt) {

    CbmStsPoint* gemPoint;

    fBmnHitsArray->Clear();

    if (!fBmnPointsArray) {
        Error("BmnGemHitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    cout << " BmnGemHitProducer::Exec(), Number of BmnGemPoints = " << fBmnPointsArray->GetEntriesFast() << endl;

    for (Int_t i = 0; i < fBmnPointsArray->GetEntriesFast(); i++) {

        TRandom* rand_gen = new TRandom();

        Float_t err[3] = {0.03, 0.03, 0.03}; // Uncertainties of coordinates

        gemPoint = (CbmStsPoint*) fBmnPointsArray->UncheckedAt(i);

        if (fOnlyPrimary) {
            CbmMCTrack* track = (CbmMCTrack*) fMCTracksArray->UncheckedAt(gemPoint->GetTrackID());
            if (!track) continue;
            if (track->GetMotherId() != -1) continue;
            //            if (track->GetP() < 0.5) continue;
        }

        const Float_t dX = rand_gen->Gaus(0, err[0]);
        const Float_t dY = rand_gen->Gaus(0, err[1]);
        const Float_t dZ = rand_gen->Gaus(0, err[2]);

        const Float_t x_smeared = gemPoint->GetXIn() + dX;
        const Float_t y_smeared = gemPoint->GetYIn() + dY;
        const Float_t z_smeared = gemPoint->GetZIn() + dZ;

        //       CheckGaussDistrib(TVector3(dX, dY, dZ), TVector3(err[0], err[1], err[2]));

//        new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), gemPoint->GetTrackID());
        new((*fBmnHitsArray)[fBmnHitsArray->GetEntriesFast()])BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), i);
        BmnHit* hit = (BmnHit*) fBmnHitsArray->At(fBmnHitsArray->GetEntriesFast() - 1);
        hit->SetIndex(fBmnHitsArray->GetEntriesFast() - 1);
        hit->SetDetId(kGEM);

        const Float_t delta = 6.; /// for station number calculation

        const Float_t z = z_smeared;
        if (Abs(30 - z) < delta) hit->SetStation(0);
        else if (Abs(50 - z) < delta) hit->SetStation(1);
        else if (Abs(70 - z) < delta) hit->SetStation(2);
        else if (Abs(90 - z) < delta) hit->SetStation(3);
        else if (Abs(110 - z) < delta) hit->SetStation(4);
        else if (Abs(140 - z) < delta) hit->SetStation(5);
        else if (Abs(170 - z) < delta) hit->SetStation(6);
        else if (Abs(200 - z) < delta) hit->SetStation(7);
        else if (Abs(240 - z) < delta) hit->SetStation(8);
        else if (Abs(280 - z) < delta) hit->SetStation(9);
        else if (Abs(320 - z) < delta) hit->SetStation(10);
        else if (Abs(360 - z) < delta) hit->SetStation(11);
        else hit->SetStation(-1);

        delete rand_gen;
    }
}

void BmnGemHitProducer::CheckGaussDistrib(TVector3 vec1, TVector3 vec2) {

    TH1F* hX = new TH1F("test_gaus_X", "test_gaus_X", 1000, 0., 0.);
    TH1F* hY = new TH1F("test_gaus_Y", "test_gaus_Y", 1000, 0., 0.);
    TH1F* hZ = new TH1F("test_gaus_Z", "test_gaus_Z", 1000, 0., 0.);

    TRandom* rand_gen = new TRandom();

    hX->Fill(rand_gen->Gaus(0, vec2.X()));
    hY->Fill(rand_gen->Gaus(0, vec2.Y()));
    hZ->Fill(rand_gen->Gaus(0, vec2.Z()));

    //    
    TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
    c1->cd();

    hX->SetLineColor(1);
    hY->SetLineColor(2);
    hZ->SetLineColor(3);

    hX->Draw();
    hX->Draw("same");
    hZ->Draw("same");
    c1->SaveAs("test_gaus.png");
}

void BmnGemHitProducer::Finish() {
}

ClassImp(BmnGemHitProducer)