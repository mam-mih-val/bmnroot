//--------------------------------------------------------------------------------------------------------------------------------------
#include<assert.h>

#include "FairHit.h"

#include "CbmStsPoint.h"
#include "CbmMCTrack.h"

#include "CbmTofHit.h"
#include "BmnHit.h"

#include "TRandom2.h"
#include "TH1F.h"

#include "BmnTOF1Point.h"
#include "BmnTof1HitProducer.h"

using namespace std;

ClassImp(BmnTof1HitProducer)
//--------------------------------------------------------------------------------------------------------------------------------------
BmnTof1HitProducer::BmnTof1HitProducer(const char *name, Int_t verbose, Bool_t test)
: FairTask(name, verbose), fDoTest(test), fOnlyPrimary(kFALSE), pRandom(new TRandom2),
aTofPoints(NULL), aMCTracks(NULL), aTofHits(NULL) {
    if (fDoTest) {

    }
}
//--------------------------------------------------------------------------------------------------------------------------------------

BmnTof1HitProducer::~BmnTof1HitProducer() {
    delete pRandom;
}
//--------------------------------------------------------------------------------------------------------------------------------------

InitStatus BmnTof1HitProducer::Init() {
    cout << " -I- BmnTof1HitProducer::Init() \n";

    if (fOnlyPrimary) cout << " Only primary particles are processed!!! \n";

    aTofPoints = (TClonesArray*) FairRootManager::Instance()->GetObject("TOF1Point");
    aMCTracks = (TClonesArray*) FairRootManager::Instance()->GetObject("MCTrack");

    // Create and register output array
    aTofHits = new TClonesArray("BmnHit");
    FairRootManager::Instance()->Register("TOF1Hit", "TOF1", aTofHits, kTRUE);

    FairLogger::GetLogger()->Info(MESSAGE_ORIGIN, " Initialization Tof1HitProducer finished succesfully.");

    return kSUCCESS;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Exec(Option_t* opt) {
    aTofHits->Clear();

    if (!aTofPoints) {
        Error("BmnTof1HitProducer::Init()", " !!! Unknown branch name !!! ");
        return;
    }

    Int_t nTofPoint = aTofPoints->GetEntriesFast();
    //	cout<<" -I- BmnTof1HitProducer::Exec(), Number of BmnTof1Points = "<<nTofPoint<<endl;


    Float_t err[3] = {0.03, 0.03, 0.03}; // FIXME: DUMMY VALUES // Uncertainties of coordinates

    Int_t UID, trackID;
    Double_t time;
    BmnTOF1Point *pPoint;
    TVector3 point, hitPosErr;

    for (Int_t i = 0; i < nTofPoint; i++) // <---Loop over the TOF points
    {
        pPoint = (BmnTOF1Point*) aTofPoints->At(i);
        //pPoint->Print(0);
        //cout<<"\n  ------>>> "<<i<<"    trackID="<<pPoint->GetTrackID();

        if (fVerbose > 2) pPoint->Print("");

        trackID = pPoint->GetTrackID();
        UID = pPoint->GetDetectorID();
        time = pRandom->Gaus(pPoint->GetTime(), fTimeSigma); // 100 ps

        if (fOnlyPrimary) {
            assert(pPoint->GetTrackID() >= 0);
            //			if(pPoint->GetTrackID() < 0) continue;  //FIXME!!! It happens, but shouldn't!

            CbmMCTrack* track = (CbmMCTrack*) aMCTracks->At(pPoint->GetTrackID());
            assert(track != 0x0);

            //			if(!track) continue;;  //FIXME!!! 
            if (track->GetMotherId() != -1) continue;
        }

        const Float_t dX = pRandom->Gaus(0, err[0]);
        const Float_t dY = pRandom->Gaus(0, err[1]);
        const Float_t dZ = pRandom->Gaus(0, err[2]);

        const Float_t x_smeared = pPoint->GetX() + dX;
        const Float_t y_smeared = pPoint->GetY() + dY;
        const Float_t z_smeared = pPoint->GetZ() + dZ;

        new((*aTofHits)[aTofHits->GetEntriesFast()]) BmnHit(0, TVector3(x_smeared, y_smeared, z_smeared), TVector3(err[0], err[1], err[2]), i);
        BmnHit* hit = (BmnHit*) aTofHits->At(aTofHits->GetEntriesFast() - 1);
        hit->SetIndex(aTofHits->GetEntriesFast() - 1);
        hit->SetDetId(kTOF1);
        hit->SetStation(12);
    }

    //	cout<<"BmnTof1HitProducer::Exec() finished!"<<endl;
}
//--------------------------------------------------------------------------------------------------------------------------------------

void BmnTof1HitProducer::Finish() {
}
//--------------------------------------------------------------------------------------------------------------------------------------

