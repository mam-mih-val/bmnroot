/** BmnFindGlobalTracks.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * @since 2009-2014
 * @version 1.0
 **/

#include "BmnFindGlobalTracks.h"
#include "BmnTrackingGeometryConstructor.h"
#include "CbmBaseHit.h"
#include "CbmPixelHit.h"
#include "CbmStripHit.h"
#include "CbmStsTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmTofTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmStsHit.h"
#include "TMath.h"
#include <vector>

using namespace TMath;

BmnFindGlobalTracks::BmnFindGlobalTracks() :
fDet(),

fStsTracks(NULL),
fStsHits(NULL),
fTofHits(NULL),
fTofTracks(NULL),
fGlobalTracks(NULL),
fTrackingType("branch"),    //correct it: do we need text-specifier?
fMergerType("nearest_hit"), //correct it: do we need text-specifier?
fFitterType("lit_kalman"),  //correct it: do we need text-specifier?

fTrackingWatch(),
fMergerWatch(),

fEventNo(0),
fIsHistogramsInitialized(kFALSE),
fMakeQA(kFALSE),
fHisto(NULL) {
    fMerger = new BmnHitToTrackMerger();
    fFinder = new BmnTrackFinder();
    fPropagator = new BmnTrackPropagator();
    fUpdater = new BmnKalmanFilter();
    fFitter = new BmnTrackFitter(fPropagator, fUpdater);
}

BmnFindGlobalTracks::~BmnFindGlobalTracks() {
    delete fMerger;
    delete fFitter;
    delete fFinder;
    delete fPropagator;
    delete fHisto;
    delete fUpdater;
}

InitStatus BmnFindGlobalTracks::Init() {
    cout << "BmnFindGlobalTracks::Init started\n";
    fDet.DetermineSetup();
    cout << fDet.ToString();

    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        Fatal("Init", "CbmRootManager is not instantiated");
    }

    //STS data
    if (fDet.GetDet(kSTS)) {
        fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
        if (NULL == fStsTracks) {
            Fatal("Init", "No StsTrack array!");
        }
        fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
        if (NULL == fStsHits) {
            Fatal("Init", "No StsHit array!");
        }
    }

    // //TOF data
    if (fDet.GetDet(kTOF)) {
        fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
//        fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
        if (NULL == fTofHits) {
            Fatal("Init", "No TofHit array!");
        }
        cout << "-I- TofHit branch found in tree" << endl;
        
        fTofTracks = new TClonesArray("CbmTofTrack", 100);
        ioman->Register("TofTrack", "Tof", fTofTracks, kTRUE);
        cout << "-I- TofTracks registered in Tree" << endl;
    }

    // Create and register track arrays
    fGlobalTracks = new TClonesArray("CbmGlobalTrack", 100);
    ioman->Register("GlobalTrack", "Global", fGlobalTracks, kTRUE);
    cout << "-I- GlobalTracks registered in Tree" << endl;

    fTrackingWatch.Reset();
    fMergerWatch.Reset();

    if (!fIsHistogramsInitialized && fMakeQA) {
        fHisto = new BmnGlobalTrackingQA();
        fHisto->Initialize();
        fIsHistogramsInitialized = kTRUE;
    }

    cout << "BmnFindGlobalTracks::Init finished\n";
    return kSUCCESS;
}

void BmnFindGlobalTracks::Exec(Option_t* opt) {
    cout << "BmnFindGlobalTracks::Exec started\n";
    if (fTofTracks != NULL) fTofTracks->Clear();
    fGlobalTracks->Clear();

    for (Int_t hitId = 0; hitId < fTofHits->GetEntriesFast() - 1; ++hitId) {
        CbmTofHit* hit = (CbmTofHit*) fTofHits->UncheckedAt(hitId);
        hit->SetFlag(-1);
    }
    
//    for (Int_t hitId = 0; hitId < fTofHits->GetEntriesFast() - 1; ++hitId) {
//        CbmTofHit* hit = (CbmTofHit*) fTofHits->UncheckedAt(hitId);
////        if (hit->GetFlag() != -1) continue;
//        for (Int_t hit2Id = hitId + 1; hit2Id < fTofHits->GetEntriesFast(); ++hit2Id) {
//            CbmTofHit* hit2 = (CbmTofHit*) fTofHits->UncheckedAt(hit2Id);
////            if (hit2->GetFlag() != -1) continue;
//            if (hit->GetRefId() == hit2->GetRefId()) {
//                //                if (Abs(hit->GetX() - hit2->GetX()) < 0.001)
//                cout << "TEST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
//                cout << "" " x = " << hit->GetX() << " y = " << hit->GetY() << " z = " << hit->GetZ() << " t = " << hit->GetTime() << endl;
//                cout << " x = " << hit2->GetX() << " y = " << hit2->GetY() << " z = " << hit2->GetZ() << " t = " << hit->GetTime() << endl;
//            }
//        }
//    }

    RunTrackReconstruction();
    CalculateLength();
    
    if (fMakeQA) {
        for (Int_t i = 0; i < fGlobalTracks->GetEntriesFast(); ++i) {
            CbmGlobalTrack* globalTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
            Int_t numTofHitsInTrack = 0;
            Int_t numStsHitsInTrack = 0;
//            if (globalTrack->GetFlag() == kBMNGOODMERGE) {
              if (globalTrack->GetTofHitIndex() != -1) { //FIXME use flags, not indexes!!! (Now there are problems...)
                CbmTofHit* hit = (CbmTofHit*) fTofHits->UncheckedAt(globalTrack->GetTofHitIndex());
                if (hit == NULL) continue; //FIXME!!! Do we need this check???
                const Float_t trX = globalTrack->GetParamFirst()->GetX();
                const Float_t trY = globalTrack->GetParamFirst()->GetY();
                const Float_t x = hit->GetX();
                const Float_t y = hit->GetY();
                const Float_t z = hit->GetZ();
                fHisto->_hTofHitDist->Fill(Sqrt((trX - x) * (trX - x) + (trY - y) * (trY - y)));
                fHisto->_hTofHitXDist->Fill(trX - x);
                fHisto->_hTofHitYDist->Fill(trY - y);
                
                fHisto->_hX_global->Fill(x);
                fHisto->_hY_global->Fill(y);
                fHisto->_hZ_global->Fill(z);
                fHisto->_hXY_global->Fill(x, y);
                fHisto->_hZX_global->Fill(z, x);
                fHisto->_hZY_global->Fill(z, y);
                fHisto->_hZXY_global->Fill(z, x, y);
                fHisto->_hXY_TOF_merged->Fill(x, y);
                numTofHitsInTrack++;
            }
            CbmStsTrack* tr = (CbmStsTrack*) fStsTracks->UncheckedAt(globalTrack->GetStsTrackIndex());
            numStsHitsInTrack += tr->GetNStsHits();
            for (Int_t idx = 0; idx < tr->GetNStsHits(); ++idx) {
                CbmStsHit* stsHit = (CbmStsHit*) fStsHits->UncheckedAt(tr->GetStsHitIndex(idx));
                fHisto->_hX_global->Fill(stsHit->GetX());
                fHisto->_hY_global->Fill(stsHit->GetY());
                fHisto->_hZ_global->Fill(stsHit->GetZ());
                fHisto->_hXY_global->Fill(stsHit->GetX(), stsHit->GetY());
                fHisto->_hZX_global->Fill(stsHit->GetZ(), stsHit->GetX());
                fHisto->_hZY_global->Fill(stsHit->GetZ(), stsHit->GetY());
                fHisto->_hZXY_global->Fill(stsHit->GetZ(), stsHit->GetX(), stsHit->GetY());
            }
            fHisto->_hNumOfHitsDistr->Fill(numTofHitsInTrack + numStsHitsInTrack);
            TVector3 mom;
            globalTrack->GetParamLast()->Momentum(mom);
            fHisto->_hMomentumDistr->Fill(mom.Mag());
            fHisto->_hPx->Fill(mom.X());
            fHisto->_hPy->Fill(mom.Y());
            fHisto->_hPz->Fill(mom.Z());
            fHisto->_hPt->Fill(Sqrt(mom.X() * mom.X() + mom.Z() * mom.Z()));
            fHisto->_hTofMergeEff->Fill(Sqrt(mom.X() * mom.X() + mom.Z() * mom.Z()), numTofHitsInTrack); //FIXME!
        }
        
        for (Int_t hitId = 0; hitId < fTofHits->GetEntriesFast(); ++hitId) {
            CbmTofHit* hit = (CbmTofHit*) fTofHits->UncheckedAt(hitId);
                if (hit->GetFlag() == -1) { //FIXME! Change flag to enumerates
                fHisto->_hXY_bagTofHits->Fill(hit->GetX(), hit->GetY());
                fHisto->_hZX_bagTofHits->Fill(hit->GetZ(), hit->GetX());
                fHisto->_hZY_bagTofHits->Fill(hit->GetZ(), hit->GetY());
                fHisto->_hXY_TOF_notmerged->Fill(hit->GetX(), hit->GetY());
            }
        }

        for (Int_t tofTrId = 0; tofTrId < fTofTracks->GetEntriesFast(); ++tofTrId) {
            CbmTofTrack* tofTr = (CbmTofTrack*) fTofTracks->At(tofTrId);
            fHisto->_hTofHitChiSq->Fill(tofTr->GetDistance());
        }
    }
    
    cout << "BmnFindGlobalTracks::Exec finished\n Event number: " << fEventNo++ << endl;
}

//void BmnFindGlobalTracks::SetParContainers() {
//    FairRunAna* ana = FairRunAna::Instance();
//    FairRuntimeDb* rtdb = ana->GetRuntimeDb();
//
//    rtdb->getContainer("FairBaseParSet");
//    rtdb->getContainer("CbmGeoMuchPar");
//}

void BmnFindGlobalTracks::Finish() {
    
    if (fMakeQA) {
        //WRITE QA IN TREE
        FairRunAna* run = FairRunAna::Instance();
        TFile* output = run->GetOutputFile();
        output->cd();
        toDirectory("QA/GlobalTracking");
        fHisto->Write();
        gFile->cd();
    }
    PrintStopwatchStatistics();
}

void BmnFindGlobalTracks::CalculateLength() {
         if (fGlobalTracks == NULL) return;

    /* Calculate the length of the global track
     * starting with (0, 0, 0) and adding all
     * distances between hits
     */
    for (Int_t iTr = 0; iTr < fGlobalTracks->GetEntriesFast(); iTr++) {
        CbmGlobalTrack* glTr = (CbmGlobalTrack*) fGlobalTracks->At(iTr);
        vector<Float_t> X, Y, Z;
        X.push_back(0.);
        Y.push_back(0.);
        Z.push_back(0.);     
        // get track segments indices
        Int_t stsId = glTr->GetStsTrackIndex();
        Int_t tofId = glTr->GetTofHitIndex();
        if (stsId > -1) {
            const CbmStsTrack* stsTr = (CbmStsTrack*) fStsTracks->At(stsId);
            for (Int_t iSts = 0; iSts < stsTr->GetNStsHits(); iSts++) {
                const CbmStsHit* stsHit = (CbmStsHit*) fStsHits->At(stsTr->GetStsHitIndex(iSts));
                X.push_back(stsHit->GetX());
                Y.push_back(stsHit->GetY());
                Z.push_back(stsHit->GetZ());
            }
        }
        if (tofId > -1) {
            const CbmPixelHit* tofHit = (CbmPixelHit*) fTofHits->At(tofId);
            X.push_back(tofHit->GetX());
            Y.push_back(tofHit->GetY());
            Z.push_back(tofHit->GetZ());
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
            fHisto->_hTrackLength->Fill(length);
        }
    }
}

void BmnFindGlobalTracks::RunTrackReconstruction() {

    // Merging of TOF hits to global tracks
    if (fDet.GetDet(kTOF)) {
        // If there are no TRD or MUCH than merge STS tracks with TOF
        //if (!(fDet.GetDet(kMUCH) || fDet.GetDet(kTRD))) {
            cout << "Number of input STS tracks = " << fStsTracks->GetEntriesFast() << endl;
            for (Int_t trId = 0; trId < fStsTracks->GetEntriesFast(); ++trId) {
                CbmStsTrack* stsTr = (CbmStsTrack*) fStsTracks->At(trId);
                new((*fGlobalTracks)[trId]) CbmGlobalTrack();
                CbmGlobalTrack* glTr = (CbmGlobalTrack*) fGlobalTracks->At(trId);
                glTr->SetParamFirst(stsTr->GetParamFirst());
                glTr->SetParamLast(stsTr->GetParamLast());
                glTr->SetStsTrackIndex(trId);
                glTr->SetNofHits(stsTr->GetNStsHits());
                glTr->SetFlag(kBMNGOOD);  //kBMNGOOd or kBMNGOODMERGE???
            }
        //}
        // Selection of tracks to be merged with TOF
//        if (fDet.GetDet(kMUCH) || fDet.GetDet(kTRD)) {
            SelectTracksForTofMerging();
//        }
        fMergerWatch.Start(kFALSE);
        fMerger->NearestTofHitMerge(fTofHits, fGlobalTracks, fTofTracks);
        cout << "N TOF tracks = " << fTofTracks->GetEntriesFast() << endl;
        fMergerWatch.Stop();
    }

    // Refit found tracks
    for (Int_t trIdx = 0; trIdx < fGlobalTracks->GetEntriesFast(); ++trIdx) {
        CbmGlobalTrack* glTrack = (CbmGlobalTrack*) fGlobalTracks->At(trIdx);
//        fFitter->FitIter(track);
        fFitter->FitImp(glTrack, fStsTracks, fStsHits, fTofHits, kFALSE);
    }
    cout << "Number of output global tracks = " << fGlobalTracks->GetEntriesFast() << endl;
}

void BmnFindGlobalTracks::SelectTracksForTofMerging() {
    // The aim of this procedure is to select only those tracks
    // which have at least one hit in the last station group.
    // Only those tracks will be propagated further and merged
    // with TOF hits.

    Int_t nofStations = BmnTrackingGeometryConstructor::Instance()->GetNofMuchTrdStations();
    //   Int_t stationCut = nofStations - 4;
    Int_t stationCut = 8;

    for (Int_t trIdx = 0; trIdx < fGlobalTracks->GetEntriesFast(); ++trIdx) {
        CbmGlobalTrack* track = (CbmGlobalTrack*) fGlobalTracks->At(trIdx);
        if (track->GetFlag() == kBMNBAD) { continue; }
        const CbmStsTrack* stsTr = (CbmStsTrack*) fStsTracks->At(track->GetStsTrackIndex());
        const Int_t stsHitIdx = stsTr->GetStsHitIndex(stsTr->GetNStsHits() - 1);
        const CbmStsHit* hit = (CbmStsHit*) fStsHits->At(stsHitIdx);
        if (hit->GetStationNr() >= stationCut) {
            // OK select this track for further merging with TOF
            track->SetFlag(kBMNGOODMERGE);
        }
    }
}

void BmnFindGlobalTracks::PrintStopwatchStatistics() {
    cout << "Stopwatch: " << endl;
    cout << "tracking: counts=" << fTrackingWatch.Counter()
            << ", real=" << fTrackingWatch.RealTime() / fTrackingWatch.Counter()
            << "/" << fTrackingWatch.RealTime()
            << " s, cpu=" << fTrackingWatch.CpuTime() / fTrackingWatch.Counter()
            << "/" << fTrackingWatch.CpuTime() << endl;
    cout << "fitter: real=" << fMergerWatch.Counter()
            << ", real=" << fMergerWatch.RealTime() / fMergerWatch.Counter()
            << "/" << fMergerWatch.RealTime()
            << " s, cpu=" << fMergerWatch.CpuTime() / fMergerWatch.Counter()
            << "/" << fMergerWatch.CpuTime() << endl;
}

ClassImp(BmnFindGlobalTracks);
