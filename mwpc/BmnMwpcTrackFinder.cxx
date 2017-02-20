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
#include <Rtypes.h>

#include "BmnMwpcTrackFinder.h"

static Float_t workTime = 0.0;

BmnMwpcTrackFinder::BmnMwpcTrackFinder(Bool_t isExp) :
fEventNo(0),
expData(isExp) {
    fInputBranchName = "BmnMwpcHit";
    fOutputBranchName = "BmnMwpcTrack";

    fMwpcGeo = new BmnMwpcGeometry();
}

BmnMwpcTrackFinder::~BmnMwpcTrackFinder() {

}

void BmnMwpcTrackFinder::Exec(Option_t* opt) {
    clock_t tStart = clock();
    if (fVerbose) cout << "\n======================== MWPC track finder exec started ===================\n" << endl;
    if (fVerbose) cout << "Event number: " << fEventNo++ << endl;

    fBmnMwpcTracksArray->Clear();
    const Int_t nHitsMin = 4;
    if (fBmnMwpcHitsArray->GetEntriesFast() < nHitsMin)
        return;

    vector <BmnMwpcTrack> seeds_mwpc0, seeds_mwpc1;
    vector <BmnMwpcTrack> foundTracks;
    
    FindSeeds(seeds_mwpc0, 0);
    FindSeeds(seeds_mwpc1, 1);

    if (seeds_mwpc0.size() != 0 && seeds_mwpc1.size() == 0) 
        SingleHitInChamber(seeds_mwpc0, 1, foundTracks);

    FitFoundTracks(foundTracks);

    if (fVerbose) cout << "\n======================== MWPC track finder exec finished ==================" << endl;
    clock_t tFinish = clock();
    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;
}

void BmnMwpcTrackFinder::SingleHitInChamber(vector <BmnMwpcTrack>& cand, Int_t mwpcId, vector <BmnMwpcTrack>& finalTracks) {

    Double_t Zhit = 0.;

    for (Int_t iHit = 0; iHit < fBmnMwpcHitsArray->GetEntriesFast(); iHit++) {
        BmnMwpcHit* hit = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(iHit);
        if (hit->GetMwpcId() != mwpcId)
            continue;

        Zhit = hit->GetZ();
        break;
    }

    if (Zhit > fMwpcGeo->GetZRight(mwpcId))
        return;

    for (Int_t iCand = 0; iCand < cand.size(); iCand++) {
        // Extract found segm. params
        Double_t tx = cand[iCand].GetParamFirst()->GetTx();
        Double_t ty = cand[iCand].GetParamFirst()->GetTy();
        Double_t x0 = cand[iCand].GetParamFirst()->GetX();
        Double_t y0 = cand[iCand].GetParamFirst()->GetY();

        // Define segm. params at Zhit
        Double_t xCurr = tx * Zhit + x0;
        Double_t yCurr = ty * Zhit + y0;

        map <Double_t, BmnMwpcHit*> distToHits;
        for (Int_t iHit = 0; iHit < fBmnMwpcHitsArray->GetEntriesFast(); iHit++) {
            BmnMwpcHit* hit = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(iHit);

            if (hit->GetMwpcId() != mwpcId)
                continue;

            hit->SetHitId(iHit);

            Double_t distCurr = Dist(xCurr, yCurr, hit->GetX(), hit->GetY());

            distToHits.insert(pair <Double_t, BmnMwpcHit*> (distCurr, hit));
        }
        map <Double_t, BmnMwpcHit*>::iterator it = distToHits.begin();

        cand[iCand].AddHit(it->second->GetHitId(), it->second);
        cand[iCand].SortHits();
        
        finalTracks.push_back(cand[iCand]);
    }
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
    delete fMwpcGeo;
    cout << "Work time of the MWPC track finder: " << workTime << " s" << endl;
}

Int_t BmnMwpcTrackFinder::FindSeeds(vector <BmnMwpcTrack>& cand, Int_t mwpcId) {

    Double_t Zc = (fMwpcGeo->GetChamberCenter(mwpcId)).Z();

    for (Int_t iHit = 0; iHit < fBmnMwpcHitsArray->GetEntriesFast(); iHit++) {
        BmnMwpcHit* hit1 = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(iHit);
        if (hit1->GetMwpcId() != mwpcId)
            continue;

        if (hit1->GetZ() < Zc) {
            for (Int_t jHit = 0; jHit < fBmnMwpcHitsArray->GetEntriesFast(); jHit++) {
                if (iHit == jHit)
                    continue;

                BmnMwpcHit* hit2 = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(jHit);

                if (hit2->GetMwpcId() != mwpcId)
                    continue;

                if (hit2->GetZ() > Zc) {
                    BmnMwpcTrack trackCand;
                    trackCand.AddHit(iHit, hit1);
                    trackCand.AddHit(jHit, hit2);
                    trackCand.SortHits();
                    cand.push_back(trackCand);
                }
            }
        }
    }

    for (Int_t iSize = 0; iSize < cand.size(); iSize++) {
        BmnMwpcHit* hit1 = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(cand[iSize].GetHitIndex(0));
        BmnMwpcHit* hit2 = (BmnMwpcHit*) fBmnMwpcHitsArray->UncheckedAt(cand[iSize].GetHitIndex(1));

        Double_t x1 = hit1->GetX();
        Double_t y1 = hit1->GetY();
        Double_t z1 = hit1->GetZ();
        Double_t x2 = hit2->GetX();
        Double_t y2 = hit2->GetY();
        Double_t z2 = hit2->GetZ();
        Double_t deltaZ = hit2->GetZ() - hit1->GetZ();

        cand[iSize].GetParamFirst()->SetTx((x2 - x1) / deltaZ);
        cand[iSize].GetParamFirst()->SetTy((y2 - y1) / deltaZ);
        cand[iSize].GetParamFirst()->SetX((x1 * z2 - x2 * z1) / deltaZ);
        cand[iSize].GetParamFirst()->SetY((y1 * z2 - y2 * z1) / deltaZ);
    }
    return cand.size();
}

BmnStatus BmnMwpcTrackFinder::FitFoundTracks(vector <BmnMwpcTrack> cand) {
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

