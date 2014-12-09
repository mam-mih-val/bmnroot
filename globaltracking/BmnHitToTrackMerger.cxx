/**
 * \file BmnHitTofMerger.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - Original author. First version of code for CBM experiment.
 * \author Sergey Merts <Sergey.Merts@gmail.com> - Modification for BMN experiment.
 * \date 2013-2014
 **/

#include "BmnHitToTrackMerger.h"

#include "CbmHit.h"
#include "CbmGlobalTrack.h"
#include "CbmTofTrack.h"
#include "BmnTrackPropagator.h"
#include "BmnMath.h"
#include "TClonesArray.h"
#include "CbmTofHit.h"
#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"

#include <iostream>
#include <map>

using namespace std;

BmnHitToTrackMerger::BmnHitToTrackMerger():
      fPDG(211),
      fChiSqCut(25.)
{    
      fPropagator = new BmnTrackPropagator();
      fFilter = new BmnKalmanFilter();
}

BmnHitToTrackMerger::~BmnHitToTrackMerger() {}


BmnStatus BmnHitToTrackMerger::NearestTofHitMerge(TClonesArray* tofHits, TClonesArray* glTracks, TClonesArray* tofTracks) {
    // First find hit with minimum Z position and build map from Z hit position
    // to track parameter to improve the calculation speed.
    Double_t zMin = 10e10;
    map<Float_t, FairTrackParam> zParamMap;
    cout << "N TOF HITS = " << tofHits->GetEntriesFast() << endl;
    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
        const CbmTofHit* tofHit = (CbmTofHit*) tofHits->At(hitIdx);
        zMin = min(zMin, tofHit->GetZ());
        zParamMap[tofHit->GetZ()] = FairTrackParam();
    }

    // Loop over input tracks
    for (Int_t trIdx = 0; trIdx < glTracks->GetEntriesFast(); ++trIdx) {
        CbmGlobalTrack* glTrack = (CbmGlobalTrack*) glTracks->At(trIdx);
        if (glTrack->GetFlag() != kBMNGOODMERGE) {continue;}
        glTrack->SetFlag(kBMNGOOD);
        FairTrackParam par(*(glTrack->GetParamLast()));

        // Extrapolate track minimum Z position of hit using magnetic field propagator
        if (fPropagator->TGeoTrackPropagate(&par, zMin, fPDG, NULL, NULL, "field") == kBMNERROR) { break; }
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
        CbmTofHit* minHit = NULL; // Pointer to hit with minimum chi-square
        Int_t minIdx = 10e6;
        FairTrackParam minPar; // Track parameters for closest hit
        for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
            CbmTofHit* tofHit = (CbmTofHit*) tofHits->At(hitIdx);
            if (zParamMap.find(tofHit->GetZ()) == zParamMap.end()) { // This should never happen
                cout << "-E- NearestTofHitMerge: Z position " << tofHit->GetZ() << " not found in map. Something is wrong.\n";
            }
            FairTrackParam tpar(zParamMap[tofHit->GetZ()]);
            Float_t chi = 0.0;
//            cout << "NearestTofHitMerge Update\n";
            fFilter->Update(&tpar, tofHit, chi); //update by KF
            if (chi < fChiSqCut && chi < minChiSq) { // Check if hit is inside validation gate and closer to the track.
                minChiSq = chi;
                minHit = tofHit;
                minPar = tpar;
                minIdx = hitIdx;
            }
        }

        if (minHit != NULL) { // Check if hit was added
            //Create new TOF track            
            new((*tofTracks)[tofTracks->GetEntriesFast()]) CbmTofTrack();
            CbmTofTrack* tofTrack = (CbmTofTrack*) tofTracks->At(tofTracks->GetEntriesFast() - 1);
            tofTrack->SetTrackIndex(trIdx);
            tofTrack->SetTofHitIndex(minIdx);
            tofTrack->SetTrackParameter(&minPar);
            tofTrack->SetDistance(minChiSq);
            CbmTofHit* tofHit = (CbmTofHit*) tofHits->At(minIdx);
            tofHit->SetFlag(kBMNTOFGOOD);
            glTrack->SetNofHits(glTrack->GetNofHits() + 1);
            glTrack->SetFlag(kBMNGOODMERGE);                                    //Need or not?
            glTrack->SetTofHitIndex(minIdx);                                    // Check it necessarily!
        }
    }
    return kBMNSUCCESS;
}

//++++++++++++++++++++++++++++++++++++ALL HITS++++++++++++++++++++++++++++++++//

BmnStatus BmnHitToTrackMerger::AllTofHitsMerge(TClonesArray* tofHits, TClonesArray* glTracks, TClonesArray* tofTracks) { //FIXME!!!!!!!!!!!!!!
//    // First find hit with minimum Z position and build map from Z hit position
//    // to track parameter to improve the calculation speed.
//    Double_t zMin = 10e10;
//    map<Float_t, FairTrackParam> zParamMap;
//    for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
//        const CbmTofHit* tofHit = (CbmTofHit*) tofHits->At(hitIdx);
//        zMin = min(zMin, tofHit->GetZ());
//        zParamMap[tofHit->GetZ()] = FairTrackParam();
//    }
//
//    // Loop over input tracks
//    for (Int_t trIdx = 0; trIdx < glTracks->GetEntriesFast(); ++trIdx) {
//        CbmGlobalTrack* glTrack = (CbmGlobalTrack*) glTracks->At(trIdx);
//        if (glTrack->GetFlag() != kBMNGOODMERGE) {
//            continue;
//        }
//        FairTrackParam par(*(glTrack->GetParamLast()));
//
//        // Extrapolate track minimum Z position of hit using magnetic field propagator.
//        if (fFieldPropagator->Propagate(&par, zMin, fPDG) == kLITERROR) {
//            break;
//        }
//
//        // Extrapolate track parameters to each Z position in the map.
//        // This is done to improve calculation speed.
//        // In case of planar TOF geometry only 1 track extrapolation is required,
//        // since all hits located at the same Z.
//        for (map<Float_t, FairTrackParam>::iterator it2 = zParamMap.begin(); it2 != zParamMap.end(); it2++) {
//            (*it2).second = par;
//            fLinePropagator->Propagate(&(*it2).second, (*it2).first, fPDG);
//        }
//
//        // Loop over hits
//        for (Int_t hitIdx = 0; hitIdx < tofHits->GetEntriesFast(); ++hitIdx) {
//            const CbmTofHit* tofHit = (CbmTofHit*) tofHits->At(hitIdx);
//            if (zParamMap.find(tofHit->GetZ()) == zParamMap.end()) { // This should never happen
//                std::cout << "-E- BmnAllHitsTofMerger::DoMerge: Z position " << tofHit->GetZ() << " not found in map. Something is wrong.\n";
//            }
//            FairTrackParam tpar(zParamMap[tofHit->GetZ()]);
//            Float_t chi = 0.0;
//            fFilter->Update(&tpar, tofHit, chi);
//            if (chi < fChiSqCut) { // Check if hit is inside validation gate and closer to the track.
//                //Create new TOF track
//                new((*tofTracks)[tofTracks->GetEntriesFast()]) CbmTofTrack();
//                CbmTofTrack* tofTrack = (CbmTofTrack*) tofHits->At(tofTracks->GetEntriesFast() - 1);
//                tofTrack->SetTrackIndex(glTrack);
//                tofTrack->SetTofHitIndex(minHit);
//                tofTrack->SetTrackParameter(&minPar);
//                tofTrack->SetDistance(minChiSq);
//            }
//        }
//    }
//    return kLITSUCCESS;
}
