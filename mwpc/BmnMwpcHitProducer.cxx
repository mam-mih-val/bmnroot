#include "BmnMwpcHitProducer.h"
#include "FairHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "BmnHit.h"
#include "TGeoManager.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"

using namespace std;
using namespace TMath;

BmnMwpcHitProducer::BmnMwpcHitProducer(Int_t num = 1) :
fOnlyPrimary(kFALSE) {
    fMwpcNum = num;
    TString str;
    str.Form("%d", fMwpcNum);
    fInputBranchName = TString("MWPC") + str + TString("Point");
    fOutputHitsBranchName = "BmnHit";
}

BmnMwpcHitProducer::~BmnMwpcHitProducer() {

}

InitStatus BmnMwpcHitProducer::Init() {

    cout << " BmnMwpcHitProducer::Init() " << endl;
    if (fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnMwpcPointsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);
    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
    fBmnMwpcHitsArray = new TClonesArray(fOutputHitsBranchName, 100);
    TString str;
    str.Form("%d", fMwpcNum);
    TString name = TString("BmnMwpc") + str + TString("Hit");
    TString folder = TString("MWPC") + str;
    ioman->Register(name.Data(), folder.Data(), fBmnMwpcHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnMwpcHitProducer::Exec(Option_t* opt) {

    Int_t idx = 0;
    fBmnMwpcHitsArray->Clear();

    if (!fBmnMwpcPointsArray) {
        Error("BmnMwpcHitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    cout << " BmnMwpcHitProducer::Exec(), Number of BmnMwpcPoints = " << fBmnMwpcPointsArray->GetEntriesFast() << endl;

    Float_t err[3] = {0.25 / Sqrt(12), 0.25 / Sqrt(12), 0.25 / Sqrt(12)}; // Uncertainties of coordinates
    map<Int_t, vector<FairMCPoint*> > sameTrack;

    for (Int_t i = 0; i < fBmnMwpcPointsArray->GetEntriesFast(); i++) {
        FairMCPoint* point = (FairMCPoint*) fBmnMwpcPointsArray->At(i);
        Float_t pZ = point->GetZ();
        Float_t pX = point->GetX();
        Float_t pY = point->GetY();
        Int_t id = point->GetTrackID();
        TGeoNode* curNode = gGeoManager->FindNode(pX, pY, pZ);
        TString nameNode = TString(curNode->GetName());
        Short_t planeNum = TString(nameNode(Int_t(nameNode.Length() - 1), 1)).Atoi();
        vector<FairMCPoint*> vec;
        if (sameTrack.find(id) == sameTrack.end()) {
            sameTrack.insert(pair<Int_t, vector<FairMCPoint*> > (id, vec));
            sameTrack.at(id).push_back(point);
        } else {
            (sameTrack.find(id)->second).push_back(point);
        }
    }
    map<Int_t, vector<FairMCPoint*> >::iterator it;
    for (it = sameTrack.begin(); it != sameTrack.end(); ++it) {

        TRandom* rand_gen = new TRandom();
        vector<FairMCPoint*> points = it->second;
        Int_t trId = it->first;

        Float_t pX = 0.0;
        Float_t pY = 0.0;
        Float_t pZ = 0.0;
        for (Int_t pi = 0; pi < points.size(); ++pi) {
            FairMCPoint* point = (FairMCPoint*) points.at(pi);
            pZ += point->GetZ();
            pX += point->GetX();
            pY += point->GetY();
            //            TGeoNode* curNode = gGeoManager->FindNode(pX, pY, pZ);
            //            TString nameNode = TString(curNode->GetName());
            //            Short_t planeNum = TString(nameNode(Int_t(nameNode.Length() - 1), 1)).Atoi();
        }
        pX /= points.size();
        pY /= points.size();
        pZ /= points.size();

        //        if (fOnlyPrimary) {
        //            if (point->GetTrackID() < 0) continue; //FIXME!!! It happens, but shouldn't!
        //            CbmMCTrack* track = (CbmMCTrack*) fMCTracksArray->At(point->GetTrackID());
        //            if (!track) continue;
        //            if (track->GetMotherId() != -1) continue;
        //        }

        const Float_t dX = rand_gen->Gaus(0, err[0]);
        const Float_t dY = rand_gen->Gaus(0, err[1]);
        const Float_t dZ = rand_gen->Gaus(0, err[2]);

        const Float_t x_smeared = pX + dX;
        const Float_t y_smeared = pY + dY;
        const Float_t z_smeared = pZ + dZ;

        new((*fBmnMwpcHitsArray)[fBmnMwpcHitsArray->GetEntriesFast()]) BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), trId);
        BmnHit* hit = (BmnHit*) fBmnMwpcHitsArray->At(fBmnMwpcHitsArray->GetEntriesFast() - 1);
        hit->SetIndex(fBmnMwpcHitsArray->GetEntriesFast() - 1);
        if (fMwpcNum == 1) hit->SetDetId(kMWPC1);
        else if (fMwpcNum == 2) hit->SetDetId(kMWPC2);
        else if (fMwpcNum == 3) hit->SetDetId(kMWPC3);
        idx++;

        delete rand_gen;
    }
    cout << "BmnMwpcHitProducer::Exec() finished!" << endl;
}

void BmnMwpcHitProducer::Finish() {
}

ClassImp(BmnMwpcHitProducer)