// @(#)bmnroot/mwpc:$Id$
// Author: Pavel Batyuk (VBLHEP) <pavel.batyuk@jinr.ru> 2017-02-12

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnMwpcTrackFinder                                                         //
//                                                                            //
//                                                                            //
// The algorithm serves for searching for track segments                      //
// in the MWPC of the BM@N experiment                                         //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
#include "BmnMwpcTrackFinder.h"

static Float_t workTime = 0.0;

BmnMwpcTrackFinder::BmnMwpcTrackFinder(Bool_t isExp) :
fEventNo(0),
expData(isExp) {
    fInputBranchName = "BmnMwpcHit";
    fOutputBranchName = "BmnMwpcTrack";
}

BmnMwpcTrackFinder::~BmnMwpcTrackFinder() {

}

void BmnMwpcTrackFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    if (fVerbose) cout << "\n======================== MWPC track finder exec started ===================\n" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo++ << endl;

    fBmnMwpcTracksArray->Clear();
    if (fBmnMwpcHitsArray->GetEntriesFast() >= 3) {
        vector <BmnMwpcTrack> seeds;
        FindSeeds(seeds);
        FitSeeds(seeds);
    }

    if (fVerbose) cout << "\n======================== MWPC track finder exec finished ==================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

InitStatus BmnMwpcTrackFinder::Init() {
    if (!expData)
        return kERROR;
    if (fVerbose) cout << "BmnMwpcTrackFinder::Init()" << endl;
    FairRootManager* ioman = FairRootManager::Instance();

    fBmnMwpcHitsArray = (TClonesArray*) ioman->GetObject(fInputBranchName);

    fBmnMwpcTracksArray = new TClonesArray(fOutputBranchName.Data());
    ioman->Register(fOutputBranchName.Data(), "MWPC", fBmnMwpcTracksArray, kTRUE);
}

void BmnMwpcTrackFinder::Finish() {
    cout << "Work time of the MWPC track finder: " << workTime << " s" << endl;
}

BmnStatus BmnMwpcTrackFinder::FindSeeds(vector <BmnMwpcTrack>& cand) {
    BmnMwpcTrack trackCand;
    // cout << "fBmnMwpcHitsArray->GetEntriesFast() = " << fBmnMwpcHitsArray->GetEntriesFast() << endl;
    for (Int_t iHit = 0; iHit < fBmnMwpcHitsArray->GetEntriesFast(); iHit++) {
        BmnMwpcHit* hit = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(iHit);
        if (!hit)
            continue;

        trackCand.AddHit(iHit, hit);

    }
    trackCand.SortHits();
    // cout << "trackCand.GetNHits() = " << trackCand.GetNHits() << endl;
    cand.push_back(trackCand);
    return kBMNSUCCESS;
}

BmnStatus BmnMwpcTrackFinder::FitSeeds(vector <BmnMwpcTrack> cand) {
    for (Int_t iTrack = 0; iTrack < cand.size(); iTrack++) {
        BmnMwpcTrack* trackCand = &(cand[iTrack]);
        CalculateTrackParamsLine(trackCand);
    }
    return kBMNSUCCESS;
}

BmnStatus BmnMwpcTrackFinder::CalculateTrackParamsLine(BmnMwpcTrack* tr) {
    //Estimation of track parameters for events w/o magnetic field
    UInt_t nHits = tr->GetNHits();
    BmnMwpcHit* lastHit = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(tr->GetHitIndex(nHits - 1));
    BmnMwpcHit* firstHit = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(tr->GetHitIndex(0));
    if (!firstHit || !lastHit)
        return kBMNERROR;

    TVector3 lineParZY = LineFit(tr, fBmnMwpcHitsArray, "ZY");
    TVector3 lineParZX = LineFit(tr, fBmnMwpcHitsArray, "ZX");

    Float_t lX = lastHit->GetX();
    Float_t lY = lastHit->GetY();
    Float_t lZ = lastHit->GetZ();

    Float_t fX = firstHit->GetX();
    Float_t fY = firstHit->GetY();
    Float_t fZ = firstHit->GetZ();

    FairTrackParam parF;
    parF.SetPosition(TVector3(lineParZX.X() * fZ + lineParZX.Y(), lineParZY.X() * fZ + lineParZY.Y(), fZ));
    parF.SetQp(0.0);
    parF.SetTx(lineParZX.X());
    parF.SetTy(lineParZY.X());

    FairTrackParam parL;
    parL.SetPosition(TVector3(lX, lY, lZ));
    parL.SetPosition(TVector3(lineParZX.X() * lZ + lineParZX.Y(), lineParZY.X() * lZ + lineParZY.Y(), fZ));
    parL.SetTx(lineParZX.X());
    parL.SetTy(lineParZY.X());
    parL.SetQp(0.0);

    tr->SetParamLast(parL);
    tr->SetParamFirst(parF);
    tr->SetB(Sqrt(fX * fX + fY * fY));
    tr->SetLength(Sqrt((fX - lX) * (fX - lX) + (fY - lY) * (fY - lY) + (fZ - lZ) * (fZ - lZ)));
    //tr->SetChi2(lineParZY.Z());
    tr->SetChi2(lineParZX.Z());
    tr->SetNDF(nHits - 2); // -2 because of line fit (2 params)

    if (nHits != 0)
        new((*fBmnMwpcTracksArray)[fBmnMwpcTracksArray->GetEntriesFast()]) BmnMwpcTrack(*tr);

    return kBMNSUCCESS;
}

ClassImp(BmnMwpcTrackFinder)

