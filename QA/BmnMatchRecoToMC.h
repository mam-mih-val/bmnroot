#ifndef BMNMATCHRECOTOMC_H_
#define BMNMATCHRECOTOMC_H_

#include "FairTask.h"
#include "BmnMatch.h"
#include "BmnTrackMatch.h"
#include "BmnGemTrack.h"
#include "CbmStsTrack.h"
#include "CbmStsHit.h"
#include "BmnGemStripHit.h"
#include "FairMCPoint.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "BmnEnums.h"
#include "BmnLink.h"

class TClonesArray;

class BmnMatchRecoToMC : public FairTask {
public:
    BmnMatchRecoToMC();
    virtual ~BmnMatchRecoToMC();

    virtual InitStatus Init();
    virtual void Exec(Option_t* opt);
    virtual void Finish();

    void SetInnerTracksBranchName(TString name) {
        fInnerTrackBranchName = name;
    }

private:

    void ReadAndCreateDataBranches();

    BmnStatus LinkToMC(const TClonesArray* hitMatchArr, const TClonesArray* points, Int_t id, BmnTrackMatch* trackMatch);
    BmnStatus LinkToMC(Int_t id, BmnTrackMatch* trackMatch);

    void MatchGlobalTracks();
    void MatchGlobalTracks(
        const TClonesArray* gemHitMatches,
        const TClonesArray* silHitMatches,
        const TClonesArray* ssdHitMatches,
        const TClonesArray* gemPoints,
        const TClonesArray* silPoints,
        const TClonesArray* ssdPoints,
        const TClonesArray* gemTracks,
        const TClonesArray* silTracks,
        const TClonesArray* ssdTracks,
        const TClonesArray* globTracks,
        TClonesArray* trackMatches);

    // STS
    TClonesArray* fStsHits; // BmnStsHit array
    TClonesArray* fStsTracks; // BmnStsTrack array

    // GEM
    TClonesArray* fGemPoints; // BmnStsPoint array
    TClonesArray* fGemTracks; // BmnGemTrack array

    // SILICON
    TClonesArray* fSilPoints;
    TClonesArray* fSilTracks;
    
    // GLOBAL
    TClonesArray* fGlobalTracks;
    TClonesArray* fGlobalTrackMatches;

    TString fInnerTrackBranchName;

    BmnMatchRecoToMC(const BmnMatchRecoToMC&) = delete;
    BmnMatchRecoToMC& operator=(const BmnMatchRecoToMC&) = delete;

    void CalculateTrackQuality(BmnGemTrack* locTr, BmnTrackMatch* trMatch, const TClonesArray* hitMatches, const TClonesArray* points, Int_t& trueCntr, Int_t& falseCntr);
    void CalculateTrackQuality(CbmStsTrack* locTr, BmnTrackMatch* trMatch);

    ClassDef(BmnMatchRecoToMC, 1);
};

#endif /* BMNMATCHRECOTOMC_H_ */
