
// This Class' Header ------------------
#include <TMath.h>
#include "TStyle.h"
#include "BmnGemTrackFinder.h"
#include "TObjArray.h"
#include "BmnGemHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmStsPoint.h"
#include "FairRunAna.h"

//-----------------------------------------

//some variables for efficiency calculation
static Int_t allFoundCntr = 0;
static Int_t wellFoundCntr = 0;
static Int_t wrongFoundCntr = 0;
static Int_t allHitCntr = 0;
static Int_t goodTrackCntr = 0;
static Int_t allTrackCntr = 0;
static Float_t workTime = 0.0;
//-----------------------------------------

const Float_t thresh = 0.7; // threshold for efficiency calculation (70%)

const UInt_t kNHITSFORSEED = 5;
using std::cout;
using namespace TMath;

BmnGemTrackFinder::BmnGemTrackFinder() :
fPDG(211),
fEventNo(0),
fChiSqCut(25.) {

    fMakeQA = kFALSE;
    fGemHitArray = NULL;
    fGemTracksArray = NULL;
    fMCTracksArray = NULL;
    fGemSeedsArray = NULL;
    fMCPointsArray = NULL;
    fHitsBranchName = "BmnGemStripHit";
    fSeedsBranchName = "BmnGemSeeds";
    fTracksBranchName = "BmnGemTrack";
}

BmnGemTrackFinder::~BmnGemTrackFinder() {
}

InitStatus BmnGemTrackFinder::Init() {

    cout << "======================== GEM track finder init started ====================" << endl;

    //Get ROOT Manager
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fGemHitArray = (TClonesArray*) ioman->GetObject(fHitsBranchName); //in
    fGemSeedsArray = (TClonesArray*) ioman->GetObject(fSeedsBranchName); //in
    fGemTracksArray = new TClonesArray(fTracksBranchName, 100); //out
    ioman->Register("BmnGemTracks", "GEM", fGemTracksArray, kTRUE);

    fMCTracksArray = (TClonesArray*) ioman->GetObject("MCTrack");
    fMCPointsArray = (TClonesArray*) ioman->GetObject("StsPoint");

    fPropagator = new BmnTrackPropagator();

    if (!isHistogramsInitialized && fMakeQA) {
        fHisto = new BmnGemTrackFinderQA();
        fHisto->Initialize();
        isHistogramsInitialized = kTRUE;
    }

    cout << "======================== GEM track finder init finished ===================" << endl;
}

void BmnGemTrackFinder::Exec(Option_t* opt) {

    cout << "\n====================== GEM track finder exec started ======================" << endl;
    cout << "\n Event number: " << fEventNo++ << endl;
    
    clock_t tStart = clock();
    fGemTracksArray->Clear();    
    for (Int_t i = 0; i < fGemSeedsArray->GetEntriesFast(); ++i) {
        BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(i);
        BmnGemTrack tr = *track;
        Refit(&tr); //refit seeds with KF
        for (Int_t iStat = kNHITSFORSEED; iStat < 12; ++iStat) {
            if (NearestHitMerge(iStat, &tr) != kBMNSUCCESS) continue; //Attaching hits on each GEM-station to track
            Refit(&tr); //refit seeds with KF
        }
        new((*fGemTracksArray)[fGemTracksArray->GetEntriesFast()]) BmnGemTrack(tr);
    }
    clock_t tFinish = clock();
    cout << "GEM_TRACKING: Number of found tracks: " << fGemTracksArray->GetEntriesFast() << endl;

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;


    if (fMakeQA) {
        for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
            BmnHit* hit = GetHit(hitIdx);
            Float_t x = hit->GetX();
            Float_t y = hit->GetY();
            Float_t z = hit->GetZ();
            Float_t R = Sqrt(x * x + y * y + z * z);
            Int_t station = hit->GetStation() + 1;
            fHisto->_hyRxR_all->Fill(x / R, y / R, station);
            fHisto->_hXYAllHits->Fill(x, y, station);
            fHisto->_hZYAllHits->Fill(z, y, station);
            fHisto->_hZXAllHits->Fill(z, x, station);
            fHisto->_hStationAllHits->Fill(station);
            if (hit->IsUsed()) {
                fHisto->_hXYGoodHits->Fill(x, y, station);
                fHisto->_hZYGoodHits->Fill(z, y, station);
                fHisto->_hZXGoodHits->Fill(z, x, station);
                fHisto->_hStationUsedHits->Fill(station);
                fHisto->_hyRxR_good->Fill(x / R, y / R, station);
            } else {
                fHisto->_hXYBadHits->Fill(x, y, station);
                fHisto->_hZYBadHits->Fill(z, y, station);
                fHisto->_hZXBadHits->Fill(z, x, station);
                fHisto->_hStationNotUsedHits->Fill(station);
                fHisto->_hyRxR_bad->Fill(x / R, y / R, station);
            }
        }
        for (Int_t iTr = 0; iTr < fGemSeedsArray->GetEntriesFast(); ++iTr) {
            BmnGemTrack* track = (BmnGemTrack*) fGemSeedsArray->At(iTr);
            for (Int_t iHit = 0; iHit < track->GetNHits(); ++iHit) {
                BmnHit* hit = GetHit(track->GetHitIndex(iHit));
                if (!hit) continue;
                fHisto->_hXYFoundHits->Fill(hit->GetX(), hit->GetY(), iTr + 1);
                fHisto->_hZYFoundHits->Fill(hit->GetZ(), hit->GetY(), iTr + 1);
                fHisto->_hZXFoundHits->Fill(hit->GetZ(), hit->GetX(), iTr + 1);
            }
        }
    }


    //    Int_t GemTrackId = -1;
    //    Int_t max = -1;

    for (Int_t i = 0; i < fGemTracksArray->GetEntriesFast(); ++i) {
        Int_t max = -1;
        Int_t refId = -1;
        map<Int_t, Int_t> indexes; //pairs of trackId and number of hits corresponded this trackId
        BmnGemTrack* track = (BmnGemTrack*) fGemTracksArray->At(i);
        BmnGemTrack* seed = (BmnGemTrack*) fGemSeedsArray->At(i);
        allFoundCntr += (track->GetNHits() - seed->GetNHits());

        if (fMakeQA) {
            fHisto->_hNumOfHitsDistr->Fill(track->GetNHits());
            TVector3 mom;
            track->GetParamLast()->Momentum(mom);
            fHisto->_hMomentumDistr->Fill(mom.Mag());
            fHisto->_hPx->Fill(mom.X());
            fHisto->_hPy->Fill(mom.Y());
            fHisto->_hPz->Fill(mom.Z());
            fHisto->_hPt->Fill(Sqrt(Sqr(mom.X()) + Sqr(mom.Z())));
        }

        for (Int_t j = 0; j < track->GetNHits(); ++j) { //loop over hits from the second to the last. Needed for comparing id of hits
            BmnHit* hit = GetHit(track->GetHitIndex(j));
            Int_t refId = hit->GetRefIndex();
            if (refId < 0) continue;
            CbmStsPoint* point = (CbmStsPoint*) fMCPointsArray->At(refId);
            if (!point) {
                cout << "GEM_TRACKING: There is no MC-point corresponded to current hit" << endl;
                continue;
            }
            
            if (indexes.find(point->GetTrackID()) == indexes.end()) {
                indexes.insert(pair<Int_t, Int_t > (point->GetTrackID(), 1));
            } else {
                (indexes.find(point->GetTrackID())->second)++;
            }
            
            if (hit->GetStation() < kNHITSFORSEED) continue;
            if (point->GetTrackID() == seed->GetRef()) {
                wellFoundCntr++;
            } else {
                wrongFoundCntr++;
            }

        }
        for (map<Int_t, Int_t>::iterator it = indexes.begin(); it != indexes.end(); it++) {
            if ((*it).second > max) {
                max = (*it).second;
                refId = (*it).first;
            }
            if ((*it).second > track->GetNHits() * thresh) goodTrackCntr++;
        }
        track->SetRef(refId);
        if(fMakeQA) {
            fHisto->_hNumMcTrack->Fill(indexes.size());
        }
    }

    allTrackCntr += fGemSeedsArray->GetEntriesFast();
    for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
        if (GetHit(hitIdx)->GetStation() + 1 <= kNHITSFORSEED) continue;
        allHitCntr++;
    }

    cout << "\n====================== GEM track finder exec finished =====================" << endl;

}

void BmnGemTrackFinder::Finish() {
    
    cout.precision(2);
    cout.setf(ios::fixed, ios::floatfield );
    
//    cout << "\n\t-----------------------------------------------------------------------------------------" << endl;
//    cout << "\t|                              Efficiency of GEM-tracking                               |" << endl;
//    cout << "\t-----------------------------------------------------------------------------------------" << endl;
//    cout << "\t|  Percent of connected hits:\t\t\t|\t" << allFoundCntr << " / " << allHitCntr << "\t|  " << allFoundCntr * 100.0 / allHitCntr << "%\t|" << endl;
//    cout << "\t|  Percent of well connected hits:\t\t|\t" << wellFoundCntr << " / " << allFoundCntr << "\t|  " << wellFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
//    cout << "\t|  Percent of wrong connected hits:\t\t|\t" << wrongFoundCntr << " / " << allFoundCntr << "\t|  " << wrongFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
//    cout << "\t|  Percent of well found tracks (thr = " << thresh << "):\t|\t" << goodTrackCntr << " / " << allTrackCntr << "\t|  " << goodTrackCntr * 100.0 / allTrackCntr << "%\t|" << endl;
//    cout << "\t|  Work time: full / per one event:\t\t|\t" << workTime << " sec.\t|  " << workTime / fEventNo << " sec.\t|" << endl;
//    cout << "\t-----------------------------------------------------------------------------------------" << endl;

    if (fMakeQA) {
        toDirectory("QA/GEM/TRACKS");
        fHisto->Write();
        gFile->cd();
    }

}

BmnStatus BmnGemTrackFinder::Refit(BmnGemTrack* tr) {

    Float_t totalLength = 0.;
    vector<BmnFitNode> nodes(tr->GetNHits());
    FairTrackParam par = *(tr->GetParamLast());
    //    FairTrackParam par = *(tr->GetParamFirst());

    //    for (Int_t iHit = 0; iHit < tr->GetNHits(); iHit++) {
    for (Int_t iHit = tr->GetNHits() - 1; iHit >= 0; iHit--) {
        BmnHit* hit = GetHit(tr->GetHitIndex(iHit));
        if (!hit) continue;
        Float_t Ze = hit->GetZ();
        Float_t length = 0;
        vector<Double_t> F(25);
        if (fPropagator->TGeoTrackPropagate(&par, Ze, 211/*glTr->GetPDG()*/, &F, &length, TString("field")) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            //cout << "PROP ERROR: hit number = " << iHit << " Ze = " << Ze << " length = " << length << " \npar = ";
            //par.Print();
            return kBMNERROR;
        }

        totalLength += length;
        nodes[iHit].SetPredictedParam(&par);
        nodes[iHit].SetF(F);
        Float_t chi2Hit = 0.;
//        cout << "GOOD \t Xt = " << par.GetX() << "\tYt = " << par.GetY() << "\tZt = " << par.GetZ() << endl;
        if (fUpdate->Update(&par, hit, chi2Hit) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            cout << "UPD ERROR: Ze = " << Ze << " length = " << length << " \npar = ";
            par.Print();
            return kBMNERROR;
        }
        if (iHit == 0) {
            tr->SetParamFirst(par);
        }
        if (iHit == tr->GetNHits() - 1) {
            tr->SetParamLast(par);
        }

        nodes[iHit].SetUpdatedParam(&par);
        nodes[iHit].SetChiSqFiltered(chi2Hit);
        tr->SetChi2(tr->GetChi2() + chi2Hit);
    }
    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

//**************Implementation of Lebedev's algorithm for merging*************//

BmnStatus BmnGemTrackFinder::NearestHitMerge(UInt_t station, BmnGemTrack* tr) {
    // First find hit with minimum Z position and build map from Z hit position
    // to track parameter to improve the calculation speed.

    //    cout << "Attach hits on station №" << station << endl;

    Double_t zMin = 10e10;
    map<Float_t, FairTrackParam> zParamMap;

    for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
        const BmnHit* hit = GetHit(hitIdx);
        if (hit->GetStation() != station || hit->IsUsed()) continue;
        zMin = min(zMin, hit->GetZ());
        zParamMap[hit->GetZ()] = FairTrackParam();
    }

    tr->SetFlag(kBMNGOOD);
    FairTrackParam par(*(tr->GetParamLast()));
    // Extrapolate track minimum Z position of hit using magnetic field propagator
    if (fPropagator->TGeoTrackPropagate(&par, zMin, fPDG, NULL, NULL, "field") == kBMNERROR) {
        return kBMNERROR;
    }
    // Extrapolate track parameters to each Z position in the map.
    // This is done to improve calculation speed.
    // In case of planar TOF geometry only 1 track extrapolation is required,
    // since all hits located at the same Z.
    for (map<Float_t, FairTrackParam>::iterator it = zParamMap.begin(); it != zParamMap.end(); it++) {
        (*it).second = par;
        fPropagator->TGeoTrackPropagate(&(*it).second, (*it).first, fPDG, NULL, NULL, "field");
    }

    // Loop over hits
    Float_t minChiSq = 10e10; // minimum chi-square of hit
    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
    Float_t minDist = 10e6;
    Int_t minIdx = 0;
    Float_t dist = 0.0;
    FairTrackParam minPar; // Track parameters for closest hit
    for (Int_t hitIdx = 0; hitIdx < fGemHitArray->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = GetHit(hitIdx);
        if (hit->GetStation() != station || hit->IsUsed()) continue;
        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) { // This should never happen
            cout << "-E- NearestHitMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
        }
        FairTrackParam tpar(zParamMap[hit->GetZ()]);
        Float_t chi = 0.0;
        fUpdate->Update(&tpar, hit, chi); //update by KF
        dist = Dist(tpar.GetX(), tpar.GetY(), hit->GetX(), hit->GetY());
        if (chi < fChiSqCut && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
            minDist = dist;
            minChiSq = chi;
            minHit = hit;
            minPar = tpar;
            minIdx = hitIdx;
        }
    }

    if (minHit != NULL) { // Check if hit was added

        //        if (fMakeQA) {
        //            fHisto->_hHitsDist->Fill(minDist);
        //            fHisto->_hHitsXDist->Fill(minPar.GetX() - minHit->GetX());
        //            fHisto->_hHitsYDist->Fill(minPar.GetY() - minHit->GetY());
        //        }
        tr->SetParamLast(minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
        tr->AddHit(minIdx, minHit);
        tr->SortHits();
    }
    return kBMNSUCCESS;
}
//****************************************************************************//

Float_t BmnGemTrackFinder::Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2) {
    if (Sqr(x1 - x2) + Sqr(y1 - y2) < 0.0) {
        return 0.0;
    } else {
        return Sqrt(Sqr(x1 - x2) + Sqr(y1 - y2));
    }
}

Float_t BmnGemTrackFinder::Sqr(Float_t x) {
    return x * x;
}

BmnHit* BmnGemTrackFinder::GetHit(Int_t i) {
    BmnHit* hit = (BmnHit*) fGemHitArray->At(i);
    if (!hit) cout << "-W- Wrong attempting to get hit number " << i << " from fGemHitArray, which contains " << fGemHitArray->GetEntriesFast() << " elements" << endl;
    return hit;
}
