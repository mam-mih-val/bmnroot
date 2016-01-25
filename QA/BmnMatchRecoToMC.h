/**
 * \file BmnMatchRecoToMC.h
 * \brief FairTask for matching RECO data to MC.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de> - original author for CBM experiment
 * \author Sergey Merts <sergey.merts@gmail.com> - modifications for BMN experiment
 * \date 2013-2014
 *
 */

#ifndef BMNMATCHRECOTOMC_H_
#define BMNMATCHRECOTOMC_H_

#include "FairTask.h"
#include "BmnMatch.h"
#include "BmnTrackMatch.h"
#include "BmnGlobalTrack.h"
#include "BmnGemTrack.h"
#include "BmnGemStripHit.h"
#include "CbmTofHit.h"
#include "BmnDchHit.h"
#include "FairMCPoint.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "BmnEnums.h"
#include "BmnLink.h"

class TClonesArray;

class BmnMatchRecoToMC : public FairTask {
public:
    /**
     * \brief Constructor.
     */
    BmnMatchRecoToMC();

    /**
     * \brief Destructor.
     */
    virtual ~BmnMatchRecoToMC();

    /**
     * \brief Derived from FairTask.
     */
    virtual InitStatus Init();

    /**
     * \brief Derived from FairTask.
     */
    virtual void Exec(
            Option_t* opt);

    /**
     * \brief Derived from FairTask.
     */
    virtual void Finish();

private:
    /**
     * \brief Read and create data branches.
     */
    void ReadAndCreateDataBranches();

    //    void MatchClusters(
    //          const TClonesArray* digiMatches,
    //          const TClonesArray* clusters,
    //          TClonesArray* clusterMatches);
    //
    //    void MatchHits(
    //          const TClonesArray* matches,
    //          const TClonesArray* hits,
    //          TClonesArray* hitMatches);
    //
    //    void MatchHitsGem(
    //          const TClonesArray* matches,
    //          const TClonesArray* hits,
    //          TClonesArray* hitMatches);
    //
    //    void MatchHitsToPoints(
    //          const TClonesArray* points,
    //          const TClonesArray* hits,
    //          TClonesArray* hitMatches);
    
    BmnStatus LinkToMC(const TClonesArray* hitMatchArr, const TClonesArray* points, Int_t id, BmnTrackMatch* trackMatch);
    void MatchHitsToPoints(
            const TClonesArray* points,
            const TClonesArray* hits,
            TClonesArray* hitMatches);

    void MatchTofHitsToPoints(
            const TClonesArray* points,
            const TClonesArray* hits,
            TClonesArray* hitMatches);

    void MatchDchHitsToPoints(
            const TClonesArray* points,
            const TClonesArray* hits,
            TClonesArray* hitMatches);

    void MatchGemHitsToPoints(
            const TClonesArray* points,
            const TClonesArray* hits,
            TClonesArray* hitMatches);

    void MatchGemTracks(
            const TClonesArray* hitMatches,
            const TClonesArray* points,
            const TClonesArray* tracks,
            TClonesArray* trackMatches);
    
    void MatchGemSeeds(
            const TClonesArray* hitMatches,
            const TClonesArray* points,
            const TClonesArray* seeds,
            TClonesArray* seedMatches);

    void MatchGlobalTracks(
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
            TClonesArray* trackMatches);

    // Pointers to data arrays

    // GEM
    TClonesArray* fGemPoints; // BmnStsPoint array
    TClonesArray* fGemDigis; // BmnStsDigi array
    TClonesArray* fGemClusters; // BmnStsCluster array
    TClonesArray* fGemHits; // BmnStsHit array
    TClonesArray* fGemTracks; // BmnStsTrack array
    TClonesArray* fGemDigiMatches; // Output BmnMatch array
    TClonesArray* fGemClusterMatches; // Output BmnMatch array
    TClonesArray* fGemHitMatches; // Output BmnMatch array
    TClonesArray* fGemTrackMatches; // Output BmnTrackMatchNew array
    TClonesArray* fGemSeeds;
    TClonesArray* fGemSeedMatches;
    
    
    // TOF1
    TClonesArray* fTof1Points; // BmnStsPoint array
    TClonesArray* fTof1Digis; // BmnStsDigi array
    TClonesArray* fTof1Clusters; // BmnStsCluster array
    TClonesArray* fTof1Hits; // BmnStsHit array
    TClonesArray* fTof1DigiMatches; // Output BmnMatch array
    TClonesArray* fTof1ClusterMatches; // Output BmnMatch array
    TClonesArray* fTof1HitMatches; // Output BmnMatch array   

    // TOF2
    TClonesArray* fTof2Points; // BmnStsPoint array
    TClonesArray* fTof2Digis; // BmnStsDigi array
    TClonesArray* fTof2Clusters; // BmnStsCluster array
    TClonesArray* fTof2Hits; // BmnStsHit array
    TClonesArray* fTof2DigiMatches; // Output BmnMatch array
    TClonesArray* fTof2ClusterMatches; // Output BmnMatch array
    TClonesArray* fTof2HitMatches; // Output BmnMatch array   

    // DCH1
    TClonesArray* fDch1Points; // BmnStsPoint array
    TClonesArray* fDch1Digis; // BmnStsDigi array
    TClonesArray* fDch1Clusters; // BmnStsCluster array
    TClonesArray* fDch1Hits; // BmnStsHit array
    TClonesArray* fDch1DigiMatches; // Output BmnMatch array
    TClonesArray* fDch1ClusterMatches; // Output BmnMatch array
    TClonesArray* fDch1HitMatches; // Output BmnMatch array   

    // DCH2
    TClonesArray* fDch2Points; // BmnStsPoint array
    TClonesArray* fDch2Digis; // BmnStsDigi array
    TClonesArray* fDch2Clusters; // BmnStsCluster array
    TClonesArray* fDch2Hits; // BmnStsHit array
    TClonesArray* fDch2DigiMatches; // Output BmnMatch array
    TClonesArray* fDch2ClusterMatches; // Output BmnMatch array
    TClonesArray* fDch2HitMatches; // Output BmnMatch array  

    // GLOBAL
    TClonesArray* fGlobalTracks;
    TClonesArray* fTrackMatches;
    TClonesArray* fMCTracks;

    BmnMatchRecoToMC(const BmnMatchRecoToMC&);
    BmnMatchRecoToMC& operator=(const BmnMatchRecoToMC&);

    ClassDef(BmnMatchRecoToMC, 1);
};

#endif /* BMNMATCHRECOTOMC_H_ */
