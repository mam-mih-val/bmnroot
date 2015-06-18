
/** BmnGlobalTracking.cxx
 * \author Sergey Merts <Sergey.Merts@gmail.com>
 * @since 2014
 * @version 1.0
 **/

#include "BmnGlobalTracking.h"
using namespace TMath;

//some variables for efficiency calculation
static Int_t allFoundCntr = 0;
static Int_t wellFoundCntr = 0;
static Int_t wrongFoundCntr = 0;
static Int_t allHitCntr = 0;
static Int_t goodTrackCntr = 0;
static Int_t allTrackCntr = 0;
static Int_t allMcTrackCntr = 0;
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
fEventNo(0),
fIsHistogramsInitialized(kFALSE),
fMakeQA(kTRUE),
fTof1Histo(NULL),
fTof2Histo(NULL),
fDch1Histo(NULL),
fDch2Histo(NULL),
fGlobHisto(NULL) {
    fMerger = new BmnHitToTrackMerger();
    fFinder = new BmnTrackFinder();
    fPropagator = new BmnTrackPropagator();
    fUpdater = new BmnKalmanFilter();
    fFitter = new BmnTrackFitter(fPropagator, fUpdater);
    isRUN1 = kFALSE;
}

BmnGlobalTracking::~BmnGlobalTracking() {
    delete fMerger;
    delete fFitter;
    delete fFinder;
    delete fPropagator;
    delete fGlobHisto;
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

        fGemMcPoints = (TClonesArray*) ioman->GetObject("StsPoint");
        if (!fGemMcPoints) {
            Fatal("Init", "No StsPoint array!");
        }
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

    // ----------------- DCH1 initialization -----------------// 
    if (fDet.GetDet(kDCH1)) {
        fDch1Hits = (TClonesArray*) ioman->GetObject("BmnDch1Hit0");
        if (!fDch1Hits) {
            cout << "BmnGlobalTracking::Init: No BmnDch1Hit0 array!" << endl;
        } else {
            fDch1McPoints = (TClonesArray*) ioman->GetObject("DCH1Point");
            if (!fDch1McPoints) {
                cout << "BmnGlobalTracking::Init: No DCH1Point array!" << endl;
            }
        }
    } else {
        cout << "\nWARNING! Detector DCH1 is excluded from global tracking!!!" << endl;
    }
    // ------------------------------------------------------// 

    // ----------------- DCH2 initialization -----------------// 
    if (fDet.GetDet(kDCH2)) {
        fDch2Hits = (TClonesArray*) ioman->GetObject("BmnDch2Hit0");
        if (!fDch2Hits) {
            cout << "BmnGlobalTracking::Init: No BmnDch2Hit0 array!" << endl;
        } else {
            fDch2McPoints = (TClonesArray*) ioman->GetObject("DCH2Point");
            if (!fDch2McPoints) {
                cout << "BmnGlobalTracking::Init: No DCH2Point array!" << endl;
            }
        }
    } else {
        cout << "\nWARNING! Detector DCH2 is excluded from global tracking!!!" << endl;
    }
    // ------------------------------------------------------// 

    // Create and register track arrays
    fGlobalTracks = new TClonesArray("BmnGlobalTrack", 100);
    ioman->Register("GlobalTrack", "GLOBAL", fGlobalTracks, kTRUE);

    fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    if (!fMcTracks) {
        Fatal("Init", "No MCTrack array!");
    }
    ioman->Register("MCTrack", "MC", fMcTracks, kTRUE);

    if (!fIsHistogramsInitialized && fMakeQA) {
        fTof1Histo = new BmnHitMatchingQA(TString("TOF1"));
        fTof2Histo = new BmnHitMatchingQA(TString("TOF2"));
        fDch1Histo = new BmnHitMatchingQA(TString("DCH1"));
        fDch2Histo = new BmnHitMatchingQA(TString("DCH2"));
        fGlobHisto = new BmnGlobalTrackingQA();
        fGlobHisto->Initialize();
        fTof1Histo->Initialize();
        fTof2Histo->Initialize();
        fDch1Histo->Initialize();
        fDch2Histo->Initialize();
        fIsHistogramsInitialized = kTRUE;
    }

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
            glTr->SetParamFirst(seed->GetParamFirst());
            glTr->SetParamLast(seed->GetParamLast());
            //glTr->SetGemTrackIndex(i);
            glTr->SetNofHits(seed->GetNHits());
            glTr->SetRefId(seed->GetRef());
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
            glTr->SetParamFirst(gemTrack->GetParamFirst());
            glTr->SetParamLast(gemTrack->GetParamLast());
            glTr->SetGemTrackIndex(i);
            glTr->SetNofHits(gemTrack->GetNHits());
            glTr->SetRefId(gemTrack->GetRef());

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
        if ((globalTrack->GetChi2() / (globalTrack->GetNofHits() - 1)  > fChiSqCut)) globalTrack->SetFlag(kBMNBAD);
    }
    
    clock_t tFinish = clock();

    workTime += ((Float_t) (tFinish - tStart)) / CLOCKS_PER_SEC;

    //    EfficiencyCalculation();

    if (fMakeQA) {
        for (Int_t i = 0; i < fGlobalTracks->GetEntriesFast(); ++i) {
            BmnGlobalTrack* globalTrack = (BmnGlobalTrack*) fGlobalTracks->At(i);
            if (globalTrack->GetFlag() == kBMNBAD) continue;
            Int_t numHitsInTrack = 0;
            Int_t nodeIdx = 0;


            if (fDet.GetDet(kGEM)) {
                BmnGemTrack* gemTr = (BmnGemTrack*) fGemTracks->At(globalTrack->GetGemTrackIndex());
                numHitsInTrack += gemTr->GetNHits();
                for (Int_t idx = 0; idx < gemTr->GetNHits(); ++idx) {
                    BmnHit* hit = (BmnHit*) fGemHits->At(gemTr->GetHitIndex(idx));
                    if (!hit) continue;
                    FillGlobHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()));
                    nodeIdx++;
                }
            }
            if (fDet.GetDet(kTOF1) && fTof1Hits) {
                if (globalTrack->GetTof1HitIndex() != -1) {
                    CbmHit* hit = (CbmHit*) fTof1Hits->At(globalTrack->GetTof1HitIndex());
                    if (!hit) continue;
                    FillGlobHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()));
                    FillMatchHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()), fTof1Histo);
                    numHitsInTrack++;
                    nodeIdx++;
                }
            }

            if (fDet.GetDet(kDCH1) && fDch1Hits) {
                if (globalTrack->GetDch1HitIndex() != -1) {
                    BmnHit* hit = (BmnHit*) fDch1Hits->At(globalTrack->GetDch1HitIndex());
                    if (!hit) continue;
                    FillGlobHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()));
                    FillMatchHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()), fDch1Histo);
                    numHitsInTrack++;
                    nodeIdx++;
                }
            }

            if (fDet.GetDet(kDCH2) && fDch2Hits) {
                if (globalTrack->GetDch2HitIndex() != -1) {
                    BmnHit* hit = (BmnHit*) fDch2Hits->At(globalTrack->GetDch2HitIndex());
                    if (!hit) continue;
                    FillGlobHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()));
                    FillMatchHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()), fDch2Histo);
                    numHitsInTrack++;
                    nodeIdx++;
                }
            }

            if (fDet.GetDet(kTOF) && fTof2Hits) {
                if (globalTrack->GetTof2HitIndex() != -1) {
                    CbmHit* hit = (CbmHit*) fTof2Hits->At(globalTrack->GetTof2HitIndex());
                    if (!hit) continue;
                    FillGlobHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()));
                    FillMatchHistoQA(globalTrack, nodeIdx, TVector3(hit->GetX(), hit->GetY(), hit->GetZ()), fTof2Histo);
                    numHitsInTrack++;
                    nodeIdx++;
                }
            }

            fGlobHisto->_hNumOfHitsDistr->Fill(numHitsInTrack);
            TVector3 mom;
            globalTrack->GetParamLast()->Momentum(mom);
            fGlobHisto->_hMomentumDistr->Fill(mom.Mag());
            fGlobHisto->_hPx->Fill(mom.X());
            fGlobHisto->_hPy->Fill(mom.Y());
            fGlobHisto->_hPz->Fill(mom.Z());
            fGlobHisto->_hPt->Fill(Sqrt(mom.X() * mom.X() + mom.Z() * mom.Z()));
        }
    }
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
        tr->SetParamLast(&minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
        if (num == 1)
            tr->SetTof1HitIndex(minIdx);
        else
            tr->SetTof2HitIndex(minIdx);
        tr->SetNofHits(tr->GetNofHits() + 1);
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
        tr->SetParamLast(&minPar);
        tr->SetChi2(tr->GetChi2() + minChiSq);
        minHit->SetUsing(kTRUE);
//        cout << "z = " << minHit->GetZ() << " DCH" << num << endl;
        if (num == 1)
            tr->SetDch1HitIndex(minIdx);
        else
            tr->SetDch2HitIndex(minIdx);
        tr->SetNofHits(tr->GetNofHits() + 1);
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

    vector<BmnFitNode> nodes(tr->GetNofHits());
    Int_t nodeIdx = tr->GetNofHits() - 1;
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

    tr->SetParamFirst(&par);
    tr->SetFitNodes(nodes);
    return kBMNSUCCESS;
}

//BmnStatus BmnGlobalTracking::EfficiencyCalculation() {
//
//    for (Int_t i = 0; i < fGlobalTracks->GetEntriesFast(); ++i) {
//        map<Int_t, Int_t> indexes; //pairs of trackId and number of hits corresponded this trackId
//        BmnGlobalTrack* glTrack = (BmnGlobalTrack*) fGlobalTracks->At(i);
//        BmnGemTrack* gemTrack = (BmnGemTrack*) fGemTracks->At(i);
//        Int_t trId = gemTrack->GetRef(); //id of GEM track (id of 70% hits in GEM track)
//
//        for (Int_t j = 0; j < gemTrack->GetNHits(); ++j) { //loop over hits from the second to the last. Needed for comparing id of hits
//            BmnHit* curHit = (BmnHit*) fGemHits->At(gemTrack->GetHitIndex(j));
//            if (!curHit) continue;
//            Int_t id = ((CbmStsPoint*) fGemMcPoints->At(curHit->GetRefIndex()))->GetTrackID();
//            FillIndexMap(indexes, id);
//            if (trId == id) {
//                wellFoundCntr++;
//            } else {
//                wrongFoundCntr++;
//            }
//        }
//        if (fDet.GetDet(kTOF1)) {
//            if (glTrack->GetTof1HitIndex() != -1) {
//                BmnHit* tofHit = (BmnHit*) fTof1Hits->At(glTrack->GetTof1HitIndex());
//                if (!tofHit) continue;
//                Int_t id = ((CbmTofPoint*) fTof1McPoints->At(tofHit->GetRefId()))->GetTrackID();
//                FillIndexMap(indexes, id);
//                IdChecker(trId, id, TVector3(tofHit->GetX(), tofHit->GetY(), tofHit->GetZ()), fTof1Histo);
//                allFoundCntr++;
//            }
//        }
//        if (fDet.GetDet(kDCH1)) {
//            if (glTrack->GetDch1HitIndex() != -1) {
//                BmnHit* dchHit = (BmnHit*) fDch1Hits->At(glTrack->GetDch1HitIndex());
//                if (!dchHit) continue;
//                Int_t id = ((FairMCPoint*) fDch1McPoints->At(dchHit->GetRefIndex()))->GetTrackID();
//                FillIndexMap(indexes, id);
//                IdChecker(trId, id, TVector3(dchHit->GetX(), dchHit->GetY(), dchHit->GetZ()), fDch1Histo);
//                allFoundCntr++;
//            }
//        }
//        if (fDet.GetDet(kDCH2)) {
//            if (glTrack->GetDch2HitIndex() != -1) {
//                BmnHit* dchHit = (BmnHit*) fDch2Hits->At(glTrack->GetDch2HitIndex());
//                if (!dchHit) continue;
//                Int_t id = ((FairMCPoint*) fDch2McPoints->At(dchHit->GetRefIndex()))->GetTrackID();
//                FillIndexMap(indexes, id);
//                IdChecker(trId, id, TVector3(dchHit->GetX(), dchHit->GetY(), dchHit->GetZ()), fDch2Histo);
//                allFoundCntr++;
//            }
//        }
//        if (fDet.GetDet(kTOF)) {
//            if (glTrack->GetTof2HitIndex() != -1) {
//                BmnHit* tofHit = (BmnHit*) fTof2Hits->At(glTrack->GetTof2HitIndex());
//                if (!tofHit) continue;
//                Int_t id = ((CbmTofPoint*) fTof2McPoints->At(tofHit->GetRefId()))->GetTrackID();
//                FillIndexMap(indexes, id);
//                IdChecker(trId, id, TVector3(tofHit->GetX(), tofHit->GetY(), tofHit->GetZ()), fTof2Histo);
//                allFoundCntr++;
//            }
//        }
//
//        Int_t maxNumOfHits = -1;
//        Int_t maxId = -1;
//        for (map<Int_t, Int_t>::iterator it = indexes.begin(); it != indexes.end(); it++) {
//            if ((*it).second > glTrack->GetNofHits() * thresh) goodTrackCntr++;
//            if ((*it).second > maxNumOfHits) {
//                maxNumOfHits = (*it).second;
//                maxId = (*it).first;
//            }
//        }
//
//        glTrack->SetRefId(maxId);
//        if (fMakeQA) {
//            fGlobHisto->_hNumMcTrack->Fill(indexes.size());
//        }
//        allFoundCntr += gemTrack->GetNHits();
//    }
//
//    for (Int_t i = 0; i < fMcTracks->GetEntriesFast(); ++i) {
//        //Only primaries added now
//        //FIXME! Use flag!
//
//        //if (((CbmMCTrack*) fMcTracks->At(i))->GetMotherId() != -1) continue;
//        //if (((CbmMCTrack*) fMcTracks->At(i))->GetP() < 0.5) continue; 
//        allMcTrackCntr++;
//    }
//
//
//    allTrackCntr += fGemTracks->GetEntriesFast();
//    allHitCntr += fGemHits->GetEntriesFast();
//    if (fDet.GetDet(kTOF1)) allHitCntr += fTof1Hits->GetEntriesFast();
//    if (fDet.GetDet(kDCH1)) allHitCntr += fDch1Hits->GetEntriesFast();
//    if (fDet.GetDet(kDCH2)) allHitCntr += fDch2Hits->GetEntriesFast();
//    if (fDet.GetDet(kTOF)) allHitCntr += fTof2Hits->GetEntriesFast();
//}

void BmnGlobalTracking::FillIndexMap(map<Int_t, Int_t> &indexes, Int_t id) {
    if (indexes.find(id) == indexes.end()) indexes.insert(pair<Int_t, Int_t > (id, 1));
    else (indexes.find(id)->second)++;
}

void BmnGlobalTracking::IdChecker(Int_t refId, Int_t hitId, TVector3 pos, BmnHitMatchingQA* hist) {
    if (refId == hitId) {
        hist->_hX_well_matched_hits->Fill(pos.X());
        hist->_hY_well_matched_hits->Fill(pos.Y());
        hist->_hZ_well_matched_hits->Fill(pos.Z());
        hist->_hXY_well_matched_hits->Fill(pos.X(), pos.Y());
        hist->_hZX_well_matched_hits->Fill(pos.Z(), pos.X());
        hist->_hZY_well_matched_hits->Fill(pos.Z(), pos.Y());
        hist->_hZXY_well_matched_hits->Fill(pos.Y(), pos.X(), pos.Y());
        wellFoundCntr++;
    } else {
        hist->_hX_wrong_matched_hits->Fill(pos.X());
        hist->_hY_wrong_matched_hits->Fill(pos.Y());
        hist->_hZ_wrong_matched_hits->Fill(pos.Z());
        hist->_hXY_wrong_matched_hits->Fill(pos.X(), pos.Y());
        hist->_hZX_wrong_matched_hits->Fill(pos.Z(), pos.X());
        hist->_hZY_wrong_matched_hits->Fill(pos.Z(), pos.Y());
        hist->_hZXY_wrong_matched_hits->Fill(pos.Y(), pos.X(), pos.Y());
        wrongFoundCntr++;
    }
}

void BmnGlobalTracking::FillGlobHistoQA(BmnGlobalTrack* tr, Int_t nodeId, TVector3 pos) {
    const BmnFitNode* n = tr->GetFitNode(nodeId);
    if (!n) return;
    const FairTrackParam* par = n->GetUpdatedParam();
    if (!par) return;
    const Float_t trX = par->GetX();
    const Float_t trY = par->GetY();
    Float_t x = pos.X();
    Float_t y = pos.Y();
    Float_t z = pos.Z();
    fGlobHisto->_hHitDist->Fill(Sqrt((trX - x) * (trX - x) + (trY - y) * (trY - y)));
    fGlobHisto->_hHitXDist->Fill(trX - x);
    fGlobHisto->_hHitYDist->Fill(trY - y);
    fGlobHisto->_hX_global->Fill(x);
    fGlobHisto->_hY_global->Fill(y);
    fGlobHisto->_hZ_global->Fill(z);
    fGlobHisto->_hXY_global->Fill(x, y);
    fGlobHisto->_hZX_global->Fill(z, x);
    fGlobHisto->_hZY_global->Fill(z, y);
    fGlobHisto->_hZXY_global->Fill(z, x, y);
}

void BmnGlobalTracking::FillMatchHistoQA(BmnGlobalTrack* tr, Int_t nodeId, TVector3 pos, BmnHitMatchingQA* hist) {
    const BmnFitNode* n = tr->GetFitNode(nodeId);
    if (!n) return;
    const FairTrackParam* par = n->GetUpdatedParam();
    if (!par) return;
    const Float_t trX = par->GetX();
    const Float_t trY = par->GetY();
    Float_t x = pos.X();
    Float_t y = pos.Y();
    Float_t z = pos.Z();
    hist->_hX_matched_hits->Fill(x);
    hist->_hY_matched_hits->Fill(y);
    hist->_hZ_matched_hits->Fill(z);
    //    hist->_hX_not_matched_hits->Fill();
    //    hist->_hY_not_matched_hits->Fill();
    //    hist->_hZ_not_matched_hits->Fill();

    hist->_hXY_matched_hits->Fill(x, y);
    hist->_hZX_matched_hits->Fill(z, x);
    hist->_hZY_matched_hits->Fill(z, y);
    //    hist->_hXY_not_matched_hits->Fill();
    //    hist->_hZX_not_matched_hits->Fill();
    //    hist->_hZY_not_matched_hits->Fill();

    hist->_hZXY_matched_hits->Fill(z, x, y);
    //    hist->_hZXY_not_matched_hits->Fill();

    hist->_hRdist_matched_hits->Fill(Sqrt((trX - x) * (trX - x) + (trY - y) * (trY - y)));
    hist->_hXdist_matched_hits->Fill(trX - x);
    hist->_hYdist_matched_hits->Fill(trY - y);
    //    hist->_hRdist_well_matched_hits->Fill();
    //    hist->_hXdist_well_matched_hits->Fill();
    //    hist->_hYdist_well_matched_hits->Fill();
    //    hist->_hRdist_wrong_matched_hits->Fill();
    //    hist->_hXdist_wrong_matched_hits->Fill();
    //    hist->_hYdist_wrong_matched_hits->Fill();
}

void BmnGlobalTracking::Finish() {


    cout.precision(2);
    cout.setf(ios::fixed, ios::floatfield);

    //    cout << "\n\t-----------------------------------------------------------------------------------------" << endl;
    //    cout << "\t|                              Efficiency of Global tracking                            |" << endl;
    //    cout << "\t-----------------------------------------------------------------------------------------" << endl;
    //    cout << "\t|  Percent of connected hits:\t\t\t|\t" << allFoundCntr << " / " << allHitCntr << "\t|  " << allFoundCntr * 100.0 / allHitCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of well connected hits:\t\t|\t" << wellFoundCntr << " / " << allFoundCntr << "\t|  " << wellFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of wrong connected hits:\t\t|\t" << wrongFoundCntr << " / " << allFoundCntr << "\t|  " << wrongFoundCntr * 100.0 / allFoundCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of well found tracks (thr = " << thresh << "):\t|\t" << goodTrackCntr << " / " << allTrackCntr << "\t|  " << goodTrackCntr * 100.0 / allTrackCntr << "%\t|" << endl;
    //    cout << "\t|  Percent of found tracks:\t\t\t|\t" << allTrackCntr << " / " << allMcTrackCntr << "\t|  " << allTrackCntr * 100.0 / allMcTrackCntr << "%\t|" << endl;
    //    cout << "\t|  Work time: full / per one event:\t\t|\t" << workTime << " sec.\t|  " << workTime / fEventNo << " sec.\t|" << endl;
    //    cout << "\t-----------------------------------------------------------------------------------------" << endl;

    if (fMakeQA) {
        //WRITE QA IN TREE
        FairRunAna* run = FairRunAna::Instance();
        TFile* output = run->GetOutputFile();
        output->cd();
        toDirectory("QA/GlobalTracking");
        fGlobHisto->Write();
        toDirectory("QA/TOF1");
        fTof1Histo->Write();
        toDirectory("QA/DCH1");
        fDch1Histo->Write();
        toDirectory("QA/DCH2");
        fDch2Histo->Write();
        toDirectory("QA/TOF2");
        fTof2Histo->Write();
        gFile->cd();
    }
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
        if (fMakeQA) {
            fGlobHisto->_hTrackLength->Fill(length);
        }
    }
}

Float_t BmnGlobalTracking::Sqr(Float_t x) {
    return x * x;
}

ClassImp(BmnGlobalTracking);
