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

private:
    
    void ReadAndCreateDataBranches();

    BmnStatus LinkToMC(const TClonesArray* hitMatchArr, const TClonesArray* points, Int_t id, BmnTrackMatch* trackMatch);

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

    // GEM
    TClonesArray* fGemPoints; // BmnStsPoint array
    TClonesArray* fGemTracks; // BmnStsTrack array
    TClonesArray* fGemHitMatches; // Output BmnMatch array

    // SILICON
    TClonesArray* fSilPoints;
    TClonesArray* fSilTracks;
    TClonesArray* fSilHitMatches;

    // SSD
    TClonesArray* fSsdPoints;
    TClonesArray* fSsdTracks;
    TClonesArray* fSsdHitMatches;

    // GLOBAL
    TClonesArray* fGlobalTracks;
    TClonesArray* fGlobalTrackMatches;

    BmnMatchRecoToMC(const BmnMatchRecoToMC&);
    BmnMatchRecoToMC& operator=(const BmnMatchRecoToMC&);

    void CalculateTrackQuality(BmnGemTrack* locTr, BmnTrackMatch* trMatch, const TClonesArray* hitMatches, const TClonesArray* points, Int_t &trueCntr, Int_t &falseCntr);

    ClassDef(BmnMatchRecoToMC, 1);
};

#endif /* BMNMATCHRECOTOMC_H_ */
