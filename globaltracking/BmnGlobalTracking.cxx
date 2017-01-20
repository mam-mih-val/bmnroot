
/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include "BmnGlobalTracking.h"
using namespace TMath;

//some variables for efficiency calculation
static Float_t workTime = 0.0;
//-----------------------------------------

const Float_t thresh = 0.7; // threshold for efficiency calculation (70%)

BmnGlobalTracking::BmnGlobalTracking() :
fDetConf(31), //31 means that all detectors are presented
fMcTracks(NULL),
fGemTracks(NULL),
fGemHits(NULL),
fTof1Hits(NULL),
fTof2Hits(NULL),
fDch1Hits(NULL),
fDch2Hits(NULL),
fGlobalTracks(NULL),
fGemMcPoints(NULL),
fTof1McPoints(NULL),
fTof2McPoints(NULL),
fDch1McPoints(NULL),
fDch2McPoints(NULL),
fPDG(211),
fChiSqCut(100.),
fEventNo(0) {
    fMerger = new BmnHitToTrackMerger();
    fFinder = new BmnTrackFinder();
    fPropagator = new BmnTrackPropagator();
    fUpdater = new BmnKalmanFilter();
    //    fFitter = new BmnTrackFitter(fPropagator, fUpdater);
    isRUN1 = kFALSE;
}

BmnGlobalTracking::~BmnGlobalTracking() {
    delete fMerger;
    //    delete fFitter;
    delete fFinder;
    delete fPropagator;
    delete fUpdater;
}

InitStatus BmnGlobalTracking::Init() {
    cout << "BmnGlobalTracking::Init started\n";

    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        Fatal("Init", "FairRootManager is not instantiated");
    }

    fDet.DetermineSetup();
    cout << fDet.ToString();

    // ----------------- MWPC1 initialization -----------------//
    if (fDet.GetDet(kMWPC1)) {
        fMwpc1Hits = (TClonesArray*) ioman->GetObject("BmnMwpc1Hit");
        if (!fMwpc1Hits) {
            Fatal("Init", "No BmnMwpc1Hit array!");
        }
    } else {
        cout << "\nWARNING! Detector MWPC1 is excluded from global tracking!!!" << endl;
    }


    // ----------------- MWPC2 initialization -----------------//
    if (fDet.GetDet(kMWPC2)) {
        fMwpc2Hits = (TClonesArray*) ioman->GetObject("BmnMwpc2Hit");
        if (!fMwpc2Hits) {
            Fatal("Init", "No BmnMwpc2Hit array!");
        }
    } else {
        cout << "\nWARNING! Detector MWPC2 is excluded from global tracking!!!" << endl;
    }


    // ----------------- MWPC3 initialization -----------------//
    if (fDet.GetDet(kMWPC3)) {
        fMwpc3Hits = (TClonesArray*) ioman->GetObject("BmnMwpc3Hit");
        if (!fMwpc3Hits) {
            Fatal("Init", "No BmnMwpc3Hit array!");
        }
    } else {
        cout << "\nWARNING! Detector MWPC3 is excluded from global tracking!!!" << endl;
    }

    // ----------------- GEM initialization -----------------//
    if (fDet.GetDet(kGEM)) {

        //        if (isRUN1) {
        //            fSeeds = (TClonesArray*) ioman->GetObject("BmnSeeds"); //in
        //        } else {
        //            fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTracks");
        //            if (!fGemTracks) {
        //                Fatal("Init", "No BmnGemTracks array!");
        //            }
        //        }

        fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
        if (!fGemHits) {
            Fatal("Init", "No BmnGemStripHit array!");
        }

        //        fGemMcPoints = (TClonesArray*) ioman->GetObject("StsPoint");
        //        if (!fGemMcPoints) {
        //            Fatal("Init", "No StsPoint array!");
        //        }
    } else {
        cout << "\nERROR!GEM stations are excluded from global tracking!!!" << endl;
        //Fatal("Init", "No GEM stations!");
    }

    if (isRUN1) {
        fSeeds = (TClonesArray*) ioman->GetObject("BmnSeeds"); //in
    } else {
        fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTracks");
        if (!fGemTracks) {
            Fatal("Init", "No BmnGemTracks array!");
        }
    }
    // ------------------------------------------------------//

    // ----------------- TOF1 initialization -----------------//
    if (fDet.GetDet(kTOF1)) {
        fTof1Hits = (TClonesArray*) ioman->GetObject("TOF1Hit");
        if (!fTof1Hits) {
            cout << "BmnGlobalTracking::Init: No BmnTof1Hit array!" << endl;
        } else {
            fTof1McPoints = (TClonesArray*) ioman->GetObject("TOF1Point");
            if (!fTof1McPoints) {
                cout << "BmnGlobalTracking::Init: No TOF1Point array!" << endl;
            }
        }
    } else {
        cout << "\nWARNING! Detector TOF1 is excluded from global tracking!!!" << endl;
    }
    // ------------------------------------------------------//

    // ----------------- TOF2 initialization -----------------//
    if (fDet.GetDet(kTOF)) {
        fTof2Hits = (TClonesArray*) ioman->GetObject("BmnTof2Hit");
        if (!fTof2Hits) {
            cout << "BmnGlobalTracking::Init: No BmnTof2Hit array!" << endl;
        } else {
            fTof2McPoints = (TClonesArray*) ioman->GetObject("TofPoint");
            if (!fTof2McPoints) {
                cout << "BmnGlobalTracking::Init: No TofPoint array!" << endl;
            }
        }
    } else {
        cout << "\nWARNING! Detector TOF2 is excluded from global tracking!!!" << endl;
    }
    // ------------------------------------------------------//

    // ----------------- DCH initialization -----------------// 
    fDchTracks = (TClonesArray*) ioman->GetObject("DchTracks");
    if (!fDchTracks)
        cout << "WARNING! No DchTracks array!" << endl;
    // ------------------------------------------------------// 

    // ----------------- DCH1 initialization -----------------// 
//    if (fDet.GetDet(kDCH1)) {
//        fDch1Hits = (TClonesArray*) ioman->GetObject("BmnDch1Hit0");
//        if (!fDch1Hits) {
//            cout << "BmnGlobalTracking::Init: No BmnDch1Hit0 array!" << endl;
//        } else {
//            fDch1McPoints = (TClonesArray*) ioman->GetObject("DCH1Point");
//            if (!fDch1McPoints) {
//                cout << "BmnGlobalTracking::Init: No DCH1Point array!" << endl;
//            }
//        }
//    } else {
//        cout << "\nWARNING! Detector DCH1 is excluded from global tracking!!!" << endl;
//    }
    // ------------------------------------------------------// 

    // ----------------- DCH2 initialization -----------------// 
//    if (fDet.GetDet(kDCH2)) {
//        fDch2Hits = (TClonesArray*) ioman->GetObject("BmnDch2Hit0");
//        if (!fDch2Hits) {
//            cout << "BmnGlobalTracking::Init: No BmnDch2Hit0 array!" << endl;
//        } else {
//            fDch2McPoints = (TClonesArray*) ioman->GetObject("DCH2Point");
//            if (!fDch2McPoints) {
//                cout << "BmnGlobalTracking::Init: No DCH2Point array!" << endl;
//            }
//        }
//    } else {
//        cout << "\nWARNING! Detector DCH2 is excluded from global tracking!!!" << endl;
//    }
    // ------------------------------------------------------// 

    // Create and register track arrays
    fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);
    ioman->Register("GlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);

    //    fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    //    if (!fMcTracks) {
    //        Fatal("Init", "No MCTrack array!");
    //    }
    //    ioman->Register("MCTrack", "MC", fMcTracks, kTRUE);

    cout << "BmnGlobalTracking::Init finished\n";
    return kSUCCESS;
}

void BmnGlobalTracking::Exec(Option_t* opt) {

    cout << "\n======================== Global tracking exec started =====================\n" << endl;
    cout << " Event number: " << fEventNo++ << endl;

    clock_t tStart = clock();
    fGlobalTracks->Clear();

    //if (isRUN1) Run1GlobalTrackFinder();

    if (isRUN1) {
        for (Int_t i = 0; i < fSeeds->GetEntriesFast(); ++i) {
            BmnGemTrack* seed = (BmnGemTrack*) fSeeds->At(i);
            new((*fGlobalTracks)[i]) BmnGlobalTrack();
            BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(i);
            glTr->SetParamFirst(*seed->GetParamFirst());
            glTr->SetParamLast(*seed->GetParamLast());
            //glTr->SetGemTrackIndex(i);
            glTr->SetNHits(seed->GetNHits());
            //glTr->SetFlag(kBMNGOOD); //kBMNGOOD or kBMNGOODMERGE???

            //            if (NearestHitMergeGEM(glTr) == kBMNSUCCESS) {
            //                /*Refit(glTr);*/            }
            //            if (NearestHitMergeTOF(glTr, 1) == kBMNSUCCESS) {
            //                /*Refit(glTr);*/            }
            //            if (NearestHitMergeDCH(glTr, 1) == kBMNSUCCESS) {
            //                /*Refit(glTr);*/            }
            //            if (NearestHitMergeDCH(glTr, 2) == kBMNSUCCESS) {
            //                /*Refit(glTr);*/            }
            //            if (NearestHitMergeTOF(glTr, 2) == kBMNSUCCESS) {
            //                /*Refit(glTr);*/            }
            //            if (Refit(glTr) == kBMNERROR)
            //                glTr->SetFlag(kBMNBAD);
            //            else
            //                glTr->SetFlag(kBMNGOOD);
        }
    } else {
        for (Int_t i = 0; i < fGemTracks->GetEntriesFast(); ++i) {
            BmnGemTrack* gemTrack = (BmnGemTrack*) fGemTracks->At(i);
            new((*fGlobalTracks)[i]) BmnGlobalTrack();
            BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(i);
            glTr->SetParamFirst(*(gemTrack->GetParamFirst()));
            glTr->SetParamLast(*(gemTrack->GetParamLast()));
            glTr->SetGemTrackIndex(i);
            glTr->SetNHits(gemTrack->GetNHits());

            if (NearestHitMergeTOF(glTr, 1) == kBMNSUCCESS) {
                /*Refit(glTr);*/            }
            if (NearestHitMergeDCH(glTr, 1) == kBMNSUCCESS) {
                /*Refit(glTr);*/            }
            if (NearestHitMergeDCH(glTr, 2) == kBMNSUCCESS) {
                /*Refit(glTr);*/            }
            if (NearestHitMergeTOF(glTr, 2) == kBMNSUCCESS) {
                /*Refit(glTr);*/            }
            if (Refit(glTr) == kBMNERROR)
                glTr->SetFlag(kBMNBAD);
            else
                glTr->SetFlag(kBMNGOOD);
        }
    }
    CalculateLength();

    for (Int_t i = 0; i < fGlobalTracks->GetEntriesFast(); ++i) {
        BmnGlobalTrack* globalTrack = (BmnGlobalTrack*) fGlobalTracks->At(i);
        //        if ((globalTrack->GetChi2() / (globalTrack->GetNofHits() - 1)  > fChiSqCut) || (globalTrack->GetNofHits() < 6)) globalTrack->SetFlag(kBMNBAD);
        //        if (globalTrack->GetNofHits() < 5) globalTrack->SetFlag(kBMNBAD);
        if ((globalTrack->GetChi2() / (globalTrack->GetNHits() - 1) > fChiSqCut)) globalTrack->SetFlag(kBMNBAD);
    }

    clock_t tFinish = clock();

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    //    EfficiencyCalculation();

    cout << "GLOBAL_TRACKING: Number of merged tracks: " << fGlobalTracks->GetEntriesFast() << endl;

    cout << "\n======================= Global tracking exec finished =====================\n" << endl;
}

//BmnStatus BmnGlobalTracking::NearestHitMergeGEM(BmnGlobalTrack* tr) {
//
//    if (!fDet.GetDet(kDCH1)) return kBMNERROR;
//
//    // First find hit with minimum Z position and build map from Z hit position
//    // to track parameter to improve the calculation speed.
//
//    Double_t zMin = 10e10;
//    map<Float_t, FairTrackParam> zParamMap;
//
//    for (Int_t hitIdx = 0; hitIdx < fGemHits->GetEntriesFast(); ++hitIdx) {
//        const BmnHit* hit = (BmnHit*) fGemHits->At(hitIdx);
//        if (hit->IsUsed()) continue;
//        zMin = min(zMin, hit->GetZ());
//        zParamMap[hit->GetZ()] = FairTrackParam();
//    }
//
//    //    tr->SetFlag(kBMNGOOD); //FIXME: check it
//    FairTrackParam par(*(tr->GetParamLast()));
//    // Extrapolate track minimum Z position of hit using magnetic field propagator
//    if (fPropagator->TGeoTrackPropagate(&par, zMin, fPDG, NULL, NULL, "field") == kBMNERROR) {
//        return kBMNERROR;
//    }
//    // Extrapolate track parameters to each Z position in the map.
//    // This is done to improve calculation speed.
//    // In case of planar TOF geometry only 1 track extrapolation is required,
//    // since all hits located at the same Z.
//    for (map<Float_t, FairTrackParam>::iterator it = zParamMap.begin(); it != zParamMap.end(); it++) {
//        (*it).second = par;
//        fPropagator->TGeoTrackPropagate(&(*it).second, (*it).first, fPDG, NULL, NULL, "field");
//    }
//
//    // Loop over hits
//    Float_t minChiSq = 10e10; // minimum chi-square of hit
//    BmnHit* minHit = NULL; // Pointer to hit with minimum chi-square
//    Float_t minDist = 10e6;
//    Int_t minIdx = 0;
//    Float_t dist = 0.0;
//    FairTrackParam minPar; // Track parameters for closest hit
//    for (Int_t hitIdx = 0; hitIdx < fGemHits->GetEntriesFast(); ++hitIdx) {
//        BmnHit* hit = (BmnHit*) fGemHits->At(hitIdx);
//        if (hit->IsUsed()) continue;
//        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) { // This should never happen
//            cout << "GEM_MATCHING: NearestHitMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
//        }
//        FairTrackParam tpar(zParamMap[hit->GetZ()]);
//        Float_t chi = 0.0;
//        fUpdater->Update(&tpar, hit, chi); //update by KF
//        dist = Sqrt((tpar.GetX() - hit->GetX()) * (tpar.GetX() - hit->GetX()) + (tpar.GetY() - hit->GetY()) * (tpar.GetY() - hit->GetY()));
//
//        if (chi < fChiSqCut && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
//            minDist = dist;
//            minChiSq = chi;
//            minHit = hit;
//            minPar = tpar;
//            minIdx = hitIdx;
//        }
//    }
//
//    if (minHit != NULL) { // Check if hit was added
//        tr->SetParamLast(&minPar);
//        tr->SetChi2(tr->GetChi2() + minChiSq);
//        minHit->SetUsing(kTRUE);
//        tr->SetGemTrackIndex(minIdx);
//        tr->SetNofHits(tr->GetNofHits() + 1);
//        return kBMNSUCCESS;
//    } else {
//        return kBMNERROR;
//    }
//
//}

BmnStatus BmnGlobalTracking::NearestHitMergeTOF(BmnGlobalTrack* tr, Int_t num) {

    TClonesArray* tofHits = (num == 1 && fTof1Hits) ? fTof1Hits : (num == 2 && fTof2Hits) ? fTof2Hits : NULL;
    if (!tofHits) return kBMNERROR;
    if ((num == 1) && (!fDet.GetDet(kTOF1))) return kBMNERROR;
    if ((num == 2) && (!fDet.GetDet(kTOF))) return kBMNERROR;

    // First find hit with minimum Z position and build map from Z hit position
    // to track parameter to improve the calculation speed.

    Double_t zMin = 10e10;
    map<Float_t, FairTrackParam> zParamMap;

    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        const BmnHit* hit = (BmnHit*) tofHits->At(hitIdx);
        if (hit->IsUsed()) continue;
        zMin = min(zMin, hit->GetZ());
        zParamMap[hit->GetZ()] = FairTrackParam();
    }

    //    tr->SetFlag(kBMNGOOD); //FIXME: check it
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
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) tofHits->At(hitIdx);
        if (hit->IsUsed()) continue;
        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) { // This should never happen
            cout << "TOF_MATCHING: NearestHitMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
        }
        FairTrackParam tpar(zParamMap[hit->GetZ()]);
        Float_t chi = 0.0;
        fUpdater->Update(&tpar, hit, chi); //update by KF
        //        dist = Sqrt((tpar.GetX() - hit->GetX()) * (tpar.GetX() - hit->GetX()) + (tpar.GetY() - hit->GetY()) * (tpar.GetY() - hit->GetY()));
        dist = Sqrt(Sqr(tpar.GetX() - hit->GetX()) + Sqr(tpar.GetY() - hit->GetY()) + Sqr(tpar.GetZ() - hit->GetZ()));

        //        if (chi < fChiSqCut && chi < minChiSq && dist < minDist) { // Check if hit is inside validation gate and closer to the track.
        if (chi < fChiSqCut && chi < minChiSq && dist < minDist) { // Check if hit is inside validation gate and closer to the track.
            minDist = dist;
            minChiSq = chi;
            minHit = hit;
            minPar = tpar;
            minIdx = hitIdx;
        }
    }

    if (minHit != NULL) { // Check if hit was added
        //        cout << "z = " << minHit->GetZ() << " TOF" << num << endl;
        tr->SetParamLast(minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
        if (num == 1)
            tr->SetTof1HitIndex(minIdx);
        else
            tr->SetTof2HitIndex(minIdx);
        tr->SetNHits(tr->GetNHits() + 1);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }
}

BmnStatus BmnGlobalTracking::NearestHitMergeDCH(BmnGlobalTrack* tr, Int_t num) {

    TClonesArray* dchHits = (num == 1 && fDch1Hits) ? fDch1Hits : (num == 2 && fDch2Hits) ? fDch2Hits : NULL;
    if (!dchHits) return kBMNERROR;
    if ((num == 1) && (!fDet.GetDet(kDCH1))) return kBMNERROR;
    if ((num == 2) && (!fDet.GetDet(kDCH2))) return kBMNERROR;

    // First find hit with minimum Z position and build map from Z hit position
    // to track parameter to improve the calculation speed.

    Double_t zMin = 10e10;
    map<Float_t, FairTrackParam> zParamMap;

    for (Int_t hitIdx = 0; hitIdx < dchHits->GetEntriesFast(); ++hitIdx) {
        const BmnHit* hit = (BmnHit*) dchHits->At(hitIdx);
        if (hit->IsUsed()) continue;
        zMin = min(zMin, hit->GetZ());
        zParamMap[hit->GetZ()] = FairTrackParam();
    }

    //    tr->SetFlag(kBMNGOOD); //FIXME: check it
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
    for (Int_t hitIdx = 0; hitIdx < dchHits->GetEntriesFast(); ++hitIdx) {
        BmnHit* hit = (BmnHit*) dchHits->At(hitIdx);
        if (hit->IsUsed()) continue;
        if (zParamMap.find(hit->GetZ()) == zParamMap.end()) { // This should never happen
            cout << "DCH_MATCHING: NearestHitMerge: Z position " << hit->GetZ() << " not found in map. Something is wrong.\n";
        }
        FairTrackParam tpar(zParamMap[hit->GetZ()]);
        Float_t chi = 0.0;
        fUpdater->Update(&tpar, hit, chi); //update by KF
        //        dist = Sqrt((tpar.GetX() - hit->GetX()) * (tpar.GetX() - hit->GetX()) + (tpar.GetY() - hit->GetY()) * (tpar.GetY() - hit->GetY()));
        dist = Sqrt(Sqr(tpar.GetX() - hit->GetX()) + Sqr(tpar.GetY() - hit->GetY()) + Sqr(tpar.GetZ() - hit->GetZ()));

        //        cout << "dist = " << dist << endl;
        //        if (chi < fChiSqCut && chi < minChiSq && dist < minDist) { // Check if hit is inside validation gate and closer to the track.
        if (chi < fChiSqCut && chi < minChiSq && dist < minDist) { // Check if hit is inside validation gate and closer to the track.
            minDist = dist;
            minChiSq = chi;
            minHit = hit;
            minPar = tpar;
            minIdx = hitIdx;
        }
    }

    if (minHit != NULL) { // Check if hit was added
        tr->SetParamLast(minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
        //        cout << "z = " << minHit->GetZ() << " DCH" << num << endl;
        if (num == 1)
            tr->SetDch1HitIndex(minIdx);
        else
            tr->SetDch2HitIndex(minIdx);
        tr->SetNHits(tr->GetNHits() + 1);
        return kBMNSUCCESS;
    } else {
        return kBMNERROR;
    }

}

BmnStatus BmnGlobalTracking::RefitToDetector(BmnGlobalTrack* tr, Int_t hitId, TClonesArray* hitArr, FairTrackParam* par, Int_t* nodeIdx, vector<BmnFitNode>* nodes) {
    if (tr->GetTof2HitIndex() != -1) {
        BmnHit* hit = (BmnHit*) hitArr->At(hitId);
        Float_t Ze = hit->GetZ();
        Float_t length = 0;
        vector<Double_t> F(25);
        if (fPropagator->TGeoTrackPropagate(par, Ze, 211/*glTr->GetPDG()*/, &F, &length, TString("field")) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            //                cout << "PROP ERROR: hit number = " << nodeIdx << " Ze = " << Ze << " length = " << length << " \npar = ";
            //                par->Print();
            return kBMNERROR;
        }

        nodes->at(*nodeIdx).SetPredictedParam(par);
        nodes->at(*nodeIdx).SetF(F);
        Float_t chi2Hit = 0.;
        if (fUpdater->Update(par, hit, chi2Hit) == kBMNERROR) {
            tr->SetFlag(kBMNBAD);
            //                cout << "UPD ERROR: Ze = " << Ze << " length = " << length << " \npar = ";
            //                par->Print();
            return kBMNERROR;
        }
        //        tr->SetParamLast(par);
        //        tr->SetParamFirst(par);

        nodes->at(*nodeIdx).SetUpdatedParam(par);
        nodes->at(*nodeIdx).SetChiSqFiltered(chi2Hit);
        tr->SetChi2(tr->GetChi2() + chi2Hit);
        tr->SetLength(tr->GetLength() + length);
        (*nodeIdx)--;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnGlobalTracking::Refit(BmnGlobalTrack* tr) {

    vector<BmnFitNode> nodes(tr->GetNHits());
    Int_t nodeIdx = tr->GetNHits() - 1;
    FairTrackParam par = *(tr->GetParamLast());
    //    FairTrackParam par = *(tr->GetParamFirst());

    //TOF2 part
    if (fDet.GetDet(kTOF) && tr->GetTof2HitIndex() != -1 && fTof2Hits) {
        if (RefitToDetector(tr, tr->GetTof2HitIndex(), fTof2Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    }
    //DCH2 part
    if (fDet.GetDet(kDCH2) && tr->GetDch2HitIndex() != -1 && fDch2Hits) {
        if (RefitToDetector(tr, tr->GetDch2HitIndex(), fDch2Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    }
    //DCH1 part
    if (fDet.GetDet(kDCH1) && tr->GetDch1HitIndex() != -1 && fDch1Hits) {
        if (RefitToDetector(tr, tr->GetDch1HitIndex(), fDch1Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    }
    //TOF1 part
    if (fDet.GetDet(kTOF1) && tr->GetTof1HitIndex() != -1 && fTof1Hits) {
        if (RefitToDetector(tr, tr->GetTof1HitIndex(), fTof1Hits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
    }
    //GEM part
    if (fDet.GetDet(kGEM) && tr->GetGemTrackIndex() != -1) {
        BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(tr->GetGemTrackIndex());
        for (Int_t i = gemTr->GetNHits() - 1; i >= 0; --i) {
            if (RefitToDetector(tr, i, fGemHits, &par, &nodeIdx, &nodes) == kBMNERROR) return kBMNERROR;
        }
    }

    tr->SetParamFirst(par);
    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

void BmnGlobalTracking::Finish() {
    cout << "Global tracking finish" << endl;
}

void BmnGlobalTracking::CalculateLength() {
    if (fGlobalTracks == NULL) return;

    /* Calculate the length of the global track
     * starting with (0, 0, 0) and adding all
     * distances between hits
     */
    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); iTr++) {
        BmnGlobalTrack* glTr = (BmnGlobalTrack*) fGlobalTracks->At(iTr);
        vector<Float_t> X, Y, Z;
        X.push_back(0.);
        Y.push_back(0.);
        Z.push_back(0.);

        if (glTr->GetGemTrackIndex() > -1) {
            if (!isRUN1) {
                const BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(glTr->GetGemTrackIndex());
                for (Int_t iGem = 0; iGem < gemTr->GetNHits(); iGem++) {
                    const BmnHit* hit = (BmnHit*) fGemHits->At(gemTr->GetHitIndex(iGem));
                    if (!hit) continue;
                    X.push_back(hit->GetX());
                    Y.push_back(hit->GetY());
                    Z.push_back(hit->GetZ());
                }
            }
        }
        if (fDet.GetDet(kTOF1)) {
            if (glTr->GetTof1HitIndex() > -1 && fTof1Hits) {
                const BmnHit* hit = (BmnHit*) fTof1Hits->At(glTr->GetTof1HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        if (fDet.GetDet(kDCH1)) {
            if (glTr->GetDch1HitIndex() > -1 && fDch1Hits) {
                const BmnHit* hit = (BmnHit*) fDch1Hits->At(glTr->GetDch1HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        if (fDet.GetDet(kDCH2)) {
            if (glTr->GetDch2HitIndex() > -1 && fDch2Hits) {
                const BmnHit* hit = (BmnHit*) fDch2Hits->At(glTr->GetDch2HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        if (fDet.GetDet(kTOF)) {
            if (glTr->GetTof2HitIndex() > -1 && fTof2Hits) {
                const BmnHit* hit = (BmnHit*) fTof2Hits->At(glTr->GetTof2HitIndex());
                if (!hit) continue;
                X.push_back(hit->GetX());
                Y.push_back(hit->GetY());
                Z.push_back(hit->GetZ());
            }
        }
        // Calculate distances between hits
        Float_t length = 0.;
        for (Int_t i = 0; i < X.size() - 1; i++) {
            Float_t dX = X[i] - X[i + 1];
            Float_t dY = Y[i] - Y[i + 1];
            Float_t dZ = Z[i] - Z[i + 1];
            length += Sqrt(dX * dX + dY * dY + dZ * dZ);
        }
        glTr->SetLength(length);
    }
}

Float_t BmnGlobalTracking::Sqr(Float_t x) {
    return x * x;
}

ClassImp(BmnGlobalTracking);
