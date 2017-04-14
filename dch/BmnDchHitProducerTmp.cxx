#include "BmnDchHitProducerTmp.h"
#include "FairHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "BmnDchHit.h"
#include "TGeoManager.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TH1F.h"

using namespace std;
using namespace TMath;

BmnDchHitProducerTmp::BmnDchHitProducerTmp(Int_t num = 1) :
fOnlyPrimary(kFALSE) {
    fDchNum = num;
    fRunType = "points";
    TString str;
    str.Form("%d", fDchNum);
    fInputMCBranchName = TString("DCH") + str + TString("Point");
    fInputDigiBranchName = TString("bmn_dch_digit");
}

BmnDchHitProducerTmp::~BmnDchHitProducerTmp() {

}

InitStatus BmnDchHitProducerTmp::Init() {

    cout << " BmnDchHitProducerTmp::Init() " << endl;
    if (fOnlyPrimary) cout << " Only primary particles are processed!!! " << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnDchPointsArray = (TClonesArray*) ioman->GetObject(fInputMCBranchName);
    if (fBmnDchPointsArray == NULL) { // if there is no MC-points array, try to get experimental digits
        fRunType = "digits";
        fBmnDchDigitsArray = (TClonesArray*) ioman->GetObject(fInputDigiBranchName);
        if (fBmnDchDigitsArray == NULL) {
            cout << "[WARNING] BmnDchHitProducerTmp: No input arrays!" << endl;
        }
    }

    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
    fBmnDchHitsArray = new TClonesArray("BmnHit", 100);
    TString name = "BmnDchHit";
    TString folder = "DCH";
    ioman->Register(name.Data(), folder.Data(), fBmnDchHitsArray, kTRUE);

    return kSUCCESS;
}

void BmnDchHitProducerTmp::Exec(Option_t* opt) {
    cout << "BmnDchHitProducerTmp::Exec() started!" << endl;

    fBmnDchHitsArray->Clear();

    if (fBmnDchPointsArray == NULL && fBmnDchDigitsArray == NULL) {
        cout << "NO INPUT ARRAY!" << endl;
        return;
    }

    if (fRunType == "points") {
        cout << "BmnDchHitProducerTmp: Monte Carlo points!" << endl;
        cout << "Number of points = " << fBmnDchPointsArray->GetEntriesFast() << endl;
        ProcessPoints();
    } else if (fRunType == "digits") {
        cout << "BmnDchHitProducerTmp: Experimental digits!" << endl;
        cout << "Number of digits = " << fBmnDchDigitsArray->GetEntriesFast() << endl;
        ProcessDigits();
    }
    cout << "BmnDchHitProducerTmp::Exec() finished!" << endl;
}

BmnStatus BmnDchHitProducerTmp::ProcessPoints() {
    Double_t err[3] = {0.25 / Sqrt(12.0), 0.25 / Sqrt(12.0), 0.25 / Sqrt(12.0)}; // Uncertainties of coordinates
    map<Int_t, vector<FairMCPoint*> > sameTrack; // needed for storing points corresponded same track
    map<Int_t, Int_t > trackPoint; // corresponding of trackId and first point on this track. Needed for storing MC-reference

    for (Int_t i = 0; i < fBmnDchPointsArray->GetEntriesFast(); i++) {
        FairMCPoint* point = (FairMCPoint*) fBmnDchPointsArray->At(i);
        Float_t pZ = point->GetZ();
        Float_t pX = point->GetX();
        Float_t pY = point->GetY();
        Int_t id = point->GetTrackID();
        TGeoNode* curNode = gGeoManager->FindNode(pX, pY, pZ);
        TString nameNode = TString(curNode->GetName());
        vector<FairMCPoint*> vec;
        if (sameTrack.find(id) == sameTrack.end()) {
            sameTrack.insert(pair<Int_t, vector<FairMCPoint*> > (id, vec));
            sameTrack.at(id).push_back(point);
            trackPoint.insert(pair<Int_t, Int_t > (id, i));
        } else {
            (sameTrack.find(id)->second).push_back(point);
        }
    }
    map<Int_t, vector<FairMCPoint*> >::iterator it;
    for (it = sameTrack.begin(); it != sameTrack.end(); ++it) {

        TRandom* rand_gen = new TRandom();
        vector<FairMCPoint*> points = it->second;
        Int_t trId = it->first;
        Int_t pointId = trackPoint[trId];

        Float_t pX = 0.0;
        Float_t pY = 0.0;
        Float_t pZ = 0.0;
        for (Int_t pi = 0; pi < points.size(); ++pi) {
            FairMCPoint* point = (FairMCPoint*) points.at(pi);
            pZ += point->GetZ();
            pX += point->GetX();
            pY += point->GetY();
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

        new((*fBmnDchHitsArray)[fBmnDchHitsArray->GetEntriesFast()]) BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), pointId);
        BmnHit* hit = (BmnHit*) fBmnDchHitsArray->At(fBmnDchHitsArray->GetEntriesFast() - 1);
        hit->SetIndex(fBmnDchHitsArray->GetEntriesFast() - 1);
        hit->SetDetId(kDCH);
        hit->SetType(1);

        delete rand_gen;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnDchHitProducerTmp::ProcessDigits() {    
    return kBMNSUCCESS;
}

void BmnDchHitProducerTmp::Finish() {
}

ClassImp(BmnDchHitProducerTmp)