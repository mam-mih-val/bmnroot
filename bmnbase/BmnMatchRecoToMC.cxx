/*
 * BmnMatchRecoToMC.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 *
 */

#include "BmnMatchRecoToMC.h"

BmnMatchRecoToMC::BmnMatchRecoToMC() :
FairTask(),
fMCTracks(NULL),
fTrackMatches(NULL),
fGlobalTracks(NULL),
fGemPoints(NULL),
fGemDigis(NULL),
fGemClusters(NULL),
fGemHits(NULL),
fGemTracks(NULL),
fGemDigiMatches(NULL),
fGemClusterMatches(NULL),
fGemHitMatches(NULL),
fGemTrackMatches(NULL),
fTof1Points(NULL),
fTof1Digis(NULL),
fTof1Clusters(NULL),
fTof1Hits(NULL),
fTof1DigiMatches(NULL),
fTof1ClusterMatches(NULL),
fTof1HitMatches(NULL),
fTof2Points(NULL),
fTof2Digis(NULL),
fTof2Clusters(NULL),
fTof2Hits(NULL),
fTof2DigiMatches(NULL),
fTof2ClusterMatches(NULL),
fTof2HitMatches(NULL),
fDch1Points(NULL),
fDch1Digis(NULL),
fDch1Clusters(NULL),
fDch1Hits(NULL),
fDch1DigiMatches(NULL),
fDch1ClusterMatches(NULL),
fDch1HitMatches(NULL),
fDch2Points(NULL),
fDch2Digis(NULL),
fDch2Clusters(NULL),
fDch2Hits(NULL),
fDch2DigiMatches(NULL),
fDch2ClusterMatches(NULL),
fDch2HitMatches(NULL) {

}

BmnMatchRecoToMC::~BmnMatchRecoToMC() {
    if (fGemClusterMatches != NULL) {
        fGemClusterMatches->Delete();
        delete fGemClusterMatches;
    }
    if (fGemHitMatches != NULL) {
        fGemHitMatches->Delete();
        delete fGemHitMatches;
    }
    if (fGemTrackMatches) {
        fGemTrackMatches->Delete();
        delete fGemTrackMatches;
    }

    if (fTof1ClusterMatches != NULL) {
        fTof1ClusterMatches->Delete();
        delete fTof1ClusterMatches;
    }
    if (fTof1HitMatches != NULL) {
        fTof1HitMatches->Delete();
        delete fTof1HitMatches;
    }

    if (fTof2ClusterMatches != NULL) {
        fTof2ClusterMatches->Delete();
        delete fTof2ClusterMatches;
    }
    if (fTof2HitMatches != NULL) {
        fTof2HitMatches->Delete();
        delete fTof2HitMatches;
    }

    if (fDch1ClusterMatches != NULL) {
        fDch1ClusterMatches->Delete();
        delete fDch1ClusterMatches;
    }
    if (fDch1HitMatches != NULL) {
        fDch1HitMatches->Delete();
        delete fDch1HitMatches;
    }

    if (fDch2ClusterMatches != NULL) {
        fDch2ClusterMatches->Delete();
        delete fDch2ClusterMatches;
    }
    if (fDch2HitMatches != NULL) {
        fDch2HitMatches->Delete();
        delete fDch2HitMatches;
    }

}

InitStatus BmnMatchRecoToMC::Init() {
    ReadAndCreateDataBranches();
    return kSUCCESS;
}

void BmnMatchRecoToMC::Exec(Option_t* opt) {

    static Int_t eventNo = 0;
    cout << "Event #" << eventNo++ << endl;

    if (fGemClusterMatches != NULL) fGemClusterMatches->Delete();
    if (fGemHitMatches != NULL) fGemHitMatches->Delete();
    if (fGemTrackMatches != NULL) fGemTrackMatches->Delete();

    if (fTof1ClusterMatches != NULL) fTof1ClusterMatches->Delete();
    if (fTof1HitMatches != NULL) fTof1HitMatches->Delete();

    if (fTof2ClusterMatches != NULL) fTof2ClusterMatches->Delete();
    if (fTof2HitMatches != NULL) fTof2HitMatches->Delete();

    if (fDch1ClusterMatches != NULL) fDch1ClusterMatches->Delete();
    if (fDch1HitMatches != NULL) fDch1HitMatches->Delete();

    if (fDch2ClusterMatches != NULL) fDch2ClusterMatches->Delete();
    if (fDch2HitMatches != NULL) fDch2HitMatches->Delete();

    // GEM
    if (fGemDigis && fGemClusters && fGemHits) { // MC->digi->cluster->hit->track
        //MatchClusters(fGemDigiMatches, fGemClusters, fGemClusterMatches);
        //MatchHitsGem(fGemClusterMatches, fGemHits, fGemHitMatches);
        //      MatchTracks(fGemHitMatches, fGemPoints, fGemTracks, fGemTrackMatches);
    } else if (fGemHits) { // MC->hit->track
        //        MatchGemHitsToPoints(fGemPoints, fGemHits, fGemHitMatches);
        MatchHitsToPoints(fGemPoints, fGemHits, fGemHitMatches);
        MatchGemTracks(fGemHitMatches, fGemPoints, fGemTracks, fGemTrackMatches);
    }

    // TOF1
    if (fTof1Digis && fTof1Clusters && fTof1Hits) { // MC->digi->cluster->hit->track
        //MatchClusters(fTof1DigiMatches, fTof1Clusters, fTof1ClusterMatches);
        //MatchHitsTof1(fTof1ClusterMatches, fTof1Hits, fTof1HitMatches);
        //      MatchTracks(fTof1HitMatches, fTof1Points, fTof1Tracks, fTof1TrackMatches);
    } else if (fTof1Hits) { // MC->hit->track
        //        MatchTofHitsToPoints(fTof1Points, fTof1Hits, fTof1HitMatches);
        MatchHitsToPoints(fTof1Points, fTof1Hits, fTof1HitMatches);
    }

    // TOF2
    if (fTof2Digis && fTof2Clusters && fTof2Hits) { // MC->digi->cluster->hit->track
        //MatchClusters(fTof2DigiMatches, fTof2Clusters, fTof2ClusterMatches);
        //MatchHitsTof2(fTof2ClusterMatches, fTof2Hits, fTof2HitMatches);
        //      MatchTracks(fTof2HitMatches, fTof2Points, fTof2Tracks, fTof2TrackMatches);
    } else if (fTof2Hits) { // MC->hit->track
        //        MatchTofHitsToPoints(fTof2Points, fTof2Hits, fTof2HitMatches);
        MatchHitsToPoints(fTof2Points, fTof2Hits, fTof2HitMatches);
    }

    // DCH1
    if (fDch1Digis && fDch1Clusters && fDch1Hits) { // MC->digi->cluster->hit->track
        //MatchClusters(fDch1DigiMatches, fDch1Clusters, fDch1ClusterMatches);
        //MatchHitsDch1(fDch1ClusterMatches, fDch1Hits, fDch1HitMatches);
        //      MatchTracks(fDch1HitMatches, fDch1Points, fDch1Tracks, fDch1TrackMatches);
    } else if (fDch1Hits) { // MC->hit->track
        //        MatchDchHitsToPoints(fDch1Points, fDch1Hits, fDch1HitMatches);
        MatchHitsToPoints(fDch1Points, fDch1Hits, fDch1HitMatches);
    }

    // DCH2
    if (fDch2Digis && fDch2Clusters && fDch2Hits) { // MC->digi->cluster->hit->track
        //MatchClusters(fDch2DigiMatches, fDch2Clusters, fDch2ClusterMatches);
        //MatchHitsDch2(fDch2ClusterMatches, fDch2Hits, fDch2HitMatches);
        //      MatchTracks(fDch2HitMatches, fDch2Points, fDch2Tracks, fDch2TrackMatches);
    } else if (fDch2Hits) { // MC->hit->track
        //        MatchDchHitsToPoints(fDch2Points, fDch2Hits, fDch2HitMatches);
        MatchHitsToPoints(fDch2Points, fDch2Hits, fDch2HitMatches);
    }
    MatchGlobalTracks(fGemHitMatches, fTof1HitMatches,
            fTof2HitMatches, fDch1HitMatches,
            fDch2HitMatches, fGemPoints,
            fTof1Points, fTof2Points,
            fDch1Points, fDch2Points,
            fGemTracks, fGlobalTracks, fTrackMatches);
}

void BmnMatchRecoToMC::Finish() {

}

void BmnMatchRecoToMC::ReadAndCreateDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (NULL == ioman) {
        LOG(FATAL) << "BmnMatchRecoToMC::ReadAndCreateDataBranches() NULL FairRootManager." << FairLogger::endl;
    }

    fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
    fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
    if (fGlobalTracks != NULL) {
        fTrackMatches = new TClonesArray("BmnTrackMatch", 100);
        ioman->Register("GlobalTrackMatch", "GLOBAL", fTrackMatches, kTRUE);
    }

    // GEM
    fGemPoints = (TClonesArray*) ioman->GetObject("StsPoint");
    //fGemDigis = (TClonesArray*) ioman->GetObject("GemDigi");
    //fGemClusters = (TClonesArray*) ioman->GetObject("GemCluster");
    fGemHits = (TClonesArray*) ioman->GetObject("BmnGemStripHit");
    fGemTracks = (TClonesArray*) ioman->GetObject("BmnGemTracks");
    //fGemDigiMatches = (TClonesArray*) ioman->GetObject("GemDigiMatch");
    if (fGemClusters != NULL) {
        //fGemClusterMatches = new TClonesArray("BmnMatch", 100);
        //ioman->Register("GemClusterMatch", "GEM", fGemClusterMatches, kTRUE);
    }
    if (fGemHits != NULL) {
        fGemHitMatches = new TClonesArray("BmnMatch", 100);
        ioman->Register("GemHitMatch", "GEM", fGemHitMatches, kTRUE);
    }
    if (fGemTracks != NULL) {
        fGemTrackMatches = new TClonesArray("BmnTrackMatch", 100);
        ioman->Register("BmnGemTrackMatch", "GEM", fGemTrackMatches, kTRUE);
    }

    // TOF1
    fTof1Points = (TClonesArray*) ioman->GetObject("TOF1Point");
    //fTof1Digis = (TClonesArray*) ioman->GetObject("Tof1Digi");
    //fTof1Clusters = (TClonesArray*) ioman->GetObject("Tof1Cluster");
    fTof1Hits = (TClonesArray*) ioman->GetObject("TOF1Hit");
    // fTof1DigiMatches = (TClonesArray*) ioman->GetObject("BmnTof1DigiMatch");
    if (fTof1Clusters != NULL) {
        fTof1ClusterMatches = new TClonesArray("BmnMatch", 100);
        //ioman->Register("Tof1ClusterMatch", "TOF1", fTof1ClusterMatches, kTRUE);
    }
    if (fTof1Hits != NULL) {
        fTof1HitMatches = new TClonesArray("BmnMatch", 100);
        ioman->Register("Tof1HitMatch", "TOF1", fTof1HitMatches, kTRUE);
    }

    // TOF2
    fTof2Points = (TClonesArray*) ioman->GetObject("TofPoint");
    //fTof2Digis = (TClonesArray*) ioman->GetObject("Tof2Digi");
    //fTof2Clusters = (TClonesArray*) ioman->GetObject("Tof2Cluster");
    fTof2Hits = (TClonesArray*) ioman->GetObject("BmnTof2Hit");
    //fTof2DigiMatches = (TClonesArray*) ioman->GetObject("BmnTof2DigiMatch");
    if (fTof2Clusters != NULL) {
        //fTof2ClusterMatches = new TClonesArray("BmnMatch", 100);
        //ioman->Register("Tof2ClusterMatch", "TOF2", fTof2ClusterMatches, kTRUE);
    }
    if (fTof2Hits != NULL) {
        fTof2HitMatches = new TClonesArray("BmnMatch", 100);
        ioman->Register("Tof2HitMatch", "TOF2", fTof2HitMatches, kTRUE);
    }

    // DCH1
    fDch1Points = (TClonesArray*) ioman->GetObject("DCH1Point");
    //fDch1Digis = (TClonesArray*) ioman->GetObject("Dch1Digi");
    //fDch1Clusters = (TClonesArray*) ioman->GetObject("Dch1Cluster");
    fDch1Hits = (TClonesArray*) ioman->GetObject("BmnDch1Hit");
    //fDch1DigiMatches = (TClonesArray*) ioman->GetObject("BmnDch1DigiMatch");
    if (fDch1Clusters != NULL) {
        //fDch1ClusterMatches = new TClonesArray("BmnMatch", 100);
        //ioman->Register("Dch1ClusterMatch", "DCH1", fDch1ClusterMatches, kTRUE);
    }
    if (fDch1Hits != NULL) {
        fDch1HitMatches = new TClonesArray("BmnMatch", 70);
        ioman->Register("Dch1HitMatch", "DCH1", fDch1HitMatches, kTRUE);
    }

    // DCH2
    fDch2Points = (TClonesArray*) ioman->GetObject("DCH2Point");
    //fDch2Digis = (TClonesArray*) ioman->GetObject("Dch2Digi");
    //fDch2Clusters = (TClonesArray*) ioman->GetObject("Dch2Cluster");
    fDch2Hits = (TClonesArray*) ioman->GetObject("BmnDch2Hit");
    //fDch2DigiMatches = (TClonesArray*) ioman->GetObject("BmnDch2DigiMatch");
    if (fDch2Clusters != NULL) {
        //fDch2ClusterMatches = new TClonesArray("BmnMatch", 100);
        //ioman->Register("Dch2ClusterMatch", "DCH2", fDch2ClusterMatches, kTRUE);
    }
    if (fDch2Hits != NULL) {
        fDch2HitMatches = new TClonesArray("BmnMatch", 60);
        ioman->Register("Dch2HitMatch", "DCH2", fDch2HitMatches, kTRUE);
    }
}

void BmnMatchRecoToMC::MatchHitsToPoints(const TClonesArray* points, const TClonesArray* hits, TClonesArray* hitMatches) {
    if (!(hits && hitMatches && points)) return;
    Int_t nofHits = hits->GetEntriesFast();
    for (Int_t iHit = 0; iHit < nofHits; ++iHit) {
        const BmnHit* hit = (const BmnHit*) (hits->At(iHit));
        BmnMatch* hitMatch = new ((*hitMatches)[iHit]) BmnMatch();
        Int_t refId = hit->GetRefIndex();
        if (refId < 0) continue;
        const FairMCPoint* point = (const FairMCPoint*) (points->At(refId));
        if (point == NULL) continue;
        hitMatch->AddLink(BmnLink(point->GetEnergyLoss(), refId));
    }
}

void BmnMatchRecoToMC::MatchGemTracks(
        const TClonesArray* hitMatches,
        const TClonesArray* points,
        const TClonesArray* tracks,
        TClonesArray* trackMatches) {
    if (!(hitMatches && points && tracks && trackMatches)) return;

    Bool_t editMode = (trackMatches->GetEntriesFast() != 0);

    Int_t nofTracks = tracks->GetEntriesFast();
    for (Int_t iTrack = 0; iTrack < nofTracks; ++iTrack) {
        const BmnGemTrack* track = (const BmnGemTrack*) (tracks->At(iTrack));
        BmnTrackMatch* trackMatch = (editMode) ? (BmnTrackMatch*) (trackMatches->At(iTrack)) : new ((*trackMatches)[iTrack]) BmnTrackMatch();
        Int_t nofHits = track->GetNHits();
        for (Int_t iHit = 0; iHit < nofHits; ++iHit) {
            const BmnMatch* hitMatch = (BmnMatch*) (hitMatches->At(track->GetHitIndex(iHit)));
            Int_t nofLinks = hitMatch->GetNofLinks();
            for (Int_t iLink = 0; iLink < nofLinks; ++iLink) {
                const FairMCPoint* point = (const FairMCPoint*) (points->At(hitMatch->GetLink(iLink).GetIndex()));
                if (NULL == point) continue;
                trackMatch->AddLink(BmnLink(1., point->GetTrackID()));
            }
        }
        // Calculate number of true and wrong hits
        Int_t trueCounter = trackMatch->GetNofTrueHits();
        Int_t wrongCounter = trackMatch->GetNofWrongHits();
        for (Int_t iHit = 0; iHit < nofHits; ++iHit) {
            const BmnMatch* hitMatch = (BmnMatch*) (hitMatches->At(track->GetHitIndex(iHit)));
            Int_t nofLinks = hitMatch->GetNofLinks();
            Bool_t hasTrue = false;
            for (Int_t iLink = 0; iLink < nofLinks; ++iLink) {
                const FairMCPoint* point = (const FairMCPoint*) (points->At(hitMatch->GetLink(iLink).GetIndex()));
                if (NULL == point) continue;
                if (point->GetTrackID() == trackMatch->GetMatchedLink().GetIndex()) {
                    hasTrue = true;
                    break;
                }
            }
            if (hasTrue)
                trueCounter++;
            else
                wrongCounter++;
        }
        trackMatch->SetNofTrueHits(trueCounter);
        trackMatch->SetNofWrongHits(wrongCounter);
    }
}

void BmnMatchRecoToMC::MatchGlobalTracks(
        const TClonesArray* gemHitMatches,
        const TClonesArray* tof1HitMatches,
        const TClonesArray* tof2HitMatches,
        const TClonesArray* dch1HitMatches,
        const TClonesArray* dch2HitMatches,
        const TClonesArray* gemPoints,
        const TClonesArray* tof1Points,
        const TClonesArray* tof2Points,
        const TClonesArray* dch1Points,
        const TClonesArray* dch2Points,
        const TClonesArray* gemTracks,
        const TClonesArray* globTracks,
        TClonesArray* trackMatches) {

    if (!(globTracks && trackMatches)) return;

    Bool_t editMode = (trackMatches->GetEntriesFast() != 0);

    for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); ++iTrack) {
        const BmnGlobalTrack* track = (const BmnGlobalTrack*) (globTracks->At(iTrack));
        BmnTrackMatch* trackMatch = (editMode) ? (BmnTrackMatch*) (trackMatches->At(iTrack)) : new ((*trackMatches)[iTrack]) BmnTrackMatch();
        if (!trackMatch) continue;

        //GEM
        BmnGemTrack* gemTr = (BmnGemTrack*) gemTracks->At(track->GetGemTrackIndex());
        for (Int_t iHit = 0; iHit < gemTr->GetNHits(); ++iHit)
            if (LinkToMC(gemHitMatches, gemPoints, gemTr->GetHitIndex(iHit), trackMatch) == kBMNERROR) continue;
        //TOF1
        if (LinkToMC(tof1HitMatches, tof1Points, track->GetTof1HitIndex(), trackMatch) == kBMNERROR) continue;
        //TOF2
        if (LinkToMC(tof2HitMatches, tof2Points, track->GetTof2HitIndex(), trackMatch) == kBMNERROR) continue;
        //DCH1
        if (LinkToMC(dch1HitMatches, dch1Points, track->GetDch1HitIndex(), trackMatch) == kBMNERROR) continue;
        //DCH2
        if (LinkToMC(dch2HitMatches, dch2Points, track->GetDch2HitIndex(), trackMatch) == kBMNERROR) continue;

        
        //=== Calculate number of true and wrong hits ===//

        Int_t trueCounter = trackMatch->GetNofTrueHits();
        Int_t wrongCounter = trackMatch->GetNofWrongHits();

        //GEM
        for (Int_t iHit = 0; iHit < gemTr->GetNHits(); ++iHit)
            if (TrueAndWrongCalc(gemHitMatches, gemPoints, gemTr->GetHitIndex(iHit), trackMatch, &trueCounter, &wrongCounter) == kBMNERROR) continue;
        //TOF1
        if (TrueAndWrongCalc(tof1HitMatches, tof1Points, track->GetTof1HitIndex(), trackMatch, &trueCounter, &wrongCounter) == kBMNERROR) continue;
        //TOF2
        if (TrueAndWrongCalc(tof2HitMatches, tof2Points, track->GetTof2HitIndex(), trackMatch, &trueCounter, &wrongCounter) == kBMNERROR) continue;
        //DCH1
        if (TrueAndWrongCalc(dch1HitMatches, dch1Points, track->GetDch1HitIndex(), trackMatch, &trueCounter, &wrongCounter) == kBMNERROR) continue;
        //DCH2
        if (TrueAndWrongCalc(dch2HitMatches, dch2Points, track->GetDch2HitIndex(), trackMatch, &trueCounter, &wrongCounter) == kBMNERROR) continue;

        trackMatch->SetNofTrueHits(trueCounter);
        trackMatch->SetNofWrongHits(wrongCounter);
    }
}

BmnStatus BmnMatchRecoToMC::LinkToMC(const TClonesArray* hitMatchArr, const TClonesArray* points, Int_t id, BmnTrackMatch* trackMatch) {

    if (id != -1) {
        const BmnMatch* hitMatch = (BmnMatch*) (hitMatchArr->At(id));
        if (hitMatch == NULL) return kBMNERROR;
        for (Int_t iLink = 0; iLink < hitMatch->GetNofLinks(); ++iLink) {
            const FairMCPoint* point = (const FairMCPoint*) (points->At(hitMatch->GetLink(iLink).GetIndex()));
            if (NULL == point) return kBMNERROR;
            trackMatch->AddLink(BmnLink(1., point->GetTrackID()));
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnMatchRecoToMC::TrueAndWrongCalc(const TClonesArray* hitMatchArr, const TClonesArray* points, Int_t id, BmnTrackMatch* trackMatch, Int_t* trueCounter, Int_t* wrongCounter) {

    if (id != -1) {
        const BmnMatch* hitMatch = (BmnMatch*) (hitMatchArr->At(id));
        if (hitMatch == NULL) return kBMNERROR;
        Bool_t hasTrue = kFALSE;
        for (Int_t iLink = 0; iLink < hitMatch->GetNofLinks(); ++iLink) {
            const FairMCPoint* point = (const FairMCPoint*) (points->At(hitMatch->GetLink(iLink).GetIndex()));
            if (NULL == point) return kBMNERROR;
            if (point->GetTrackID() == trackMatch->GetMatchedLink().GetIndex()) {
                hasTrue = true;
                break;
            }
        }
        if (hasTrue) {
            (*trueCounter)++;
        } else {
            (*wrongCounter)++;
        }
    }
    return kBMNSUCCESS;
}

ClassImp(BmnMatchRecoToMC);
