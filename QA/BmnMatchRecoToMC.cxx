#include "BmnMatchRecoToMC.h"
#include "BmnGlobalTrack.h"
#include <TStopwatch.h>

static Double_t workTime = 0.0;

BmnMatchRecoToMC::BmnMatchRecoToMC() :
    FairTask(),
    fGlobalTrackMatches(nullptr),
    fGlobalTracks(nullptr),
    fGemPoints(nullptr),
    fGemTracks(nullptr),
    fStsTracks(nullptr),
    fStsHits(nullptr),
    fSilPoints(nullptr),
    fInnerTrackBranchName("StsTrack"),
    fSilTracks(nullptr) {
}

BmnMatchRecoToMC::~BmnMatchRecoToMC() {
}

InitStatus BmnMatchRecoToMC::Init() {
    ReadAndCreateDataBranches();
    return kSUCCESS;
}

void BmnMatchRecoToMC::Exec(Option_t* opt) {

    TStopwatch sw;
    sw.Start();

    if (fGlobalTrackMatches) fGlobalTrackMatches->Delete();
    MatchGlobalTracks();

    sw.Stop();
    workTime += sw.RealTime();
}

void BmnMatchRecoToMC::Finish() {
    printf("Work time of BmnMatchRecoToMC: %4.2f sec.\n", workTime);
}

void BmnMatchRecoToMC::ReadAndCreateDataBranches() {
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) LOG(fatal) << "BmnMatchRecoToMC::ReadAndCreateDataBranches() nullptr FairRootManager.";

    fGlobalTracks = (TClonesArray*)ioman->GetObject("BmnGlobalTrack");

    // GEM
    fGemPoints = (TClonesArray*)ioman->GetObject("StsPoint");
    fGemTracks = (TClonesArray*)ioman->GetObject("BmnGemTrack");

    // Sil
    fSilPoints = (TClonesArray*)ioman->GetObject("SiliconPoint");
    fSilTracks = (TClonesArray*)ioman->GetObject("BmnSiliconTrack");

    // STS
    fStsHits = (TClonesArray*)ioman->GetObject("StsHit");
    fStsTracks = (TClonesArray*)ioman->GetObject(fInnerTrackBranchName);

    fGlobalTrackMatches = new TClonesArray("BmnTrackMatch", 100);
    ioman->Register("BmnGlobalTrackMatch", "GLOBAL", fGlobalTrackMatches, kTRUE);
}

void BmnMatchRecoToMC::MatchGlobalTracks(
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
    TClonesArray* trackMatches) {

    for (Int_t iTrack = 0; iTrack < globTracks->GetEntriesFast(); ++iTrack) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)(globTracks->At(iTrack));
        BmnTrackMatch* trackMatch = new ((*trackMatches)[iTrack]) BmnTrackMatch();
        Int_t trueCounter = 0;
        Int_t wrongCounter = 0;
        //GEM
        if (glTrack->GetGemTrackIndex() != -1) {
            BmnGemTrack* gemTr = (BmnGemTrack*)gemTracks->At(glTrack->GetGemTrackIndex());
            for (Int_t iHit = 0; iHit < gemTr->GetNHits(); ++iHit)
                LinkToMC(gemHitMatches, gemPoints, gemTr->GetHitIndex(iHit), trackMatch);
            if (!trackMatch->GetNofLinks()) continue;
            CalculateTrackQuality(gemTr, trackMatch, gemHitMatches, gemPoints, trueCounter, wrongCounter);
        }
        //SILICON
        if (glTrack->GetSilTrackIndex() != -1) {
            BmnGemTrack* silTr = (BmnGemTrack*)silTracks->At(glTrack->GetSilTrackIndex());
            for (Int_t iHit = 0; iHit < silTr->GetNHits(); ++iHit)
                LinkToMC(silHitMatches, silPoints, silTr->GetHitIndex(iHit), trackMatch);
            if (!trackMatch->GetNofLinks()) continue;
            CalculateTrackQuality(silTr, trackMatch, silHitMatches, silPoints, trueCounter, wrongCounter);
        }
        //SSD
        if (glTrack->GetSsdTrackIndex() != -1) {
            BmnGemTrack* ssdTr = (BmnGemTrack*)ssdTracks->At(glTrack->GetSsdTrackIndex());
            for (Int_t iHit = 0; iHit < ssdTr->GetNHits(); ++iHit)
                LinkToMC(ssdHitMatches, ssdPoints, ssdTr->GetHitIndex(iHit), trackMatch);
            if (!trackMatch->GetNofLinks()) continue;
            CalculateTrackQuality(ssdTr, trackMatch, ssdHitMatches, ssdPoints, trueCounter, wrongCounter);
        }

        trackMatch->SetNofTrueHits(trueCounter);
        trackMatch->SetNofWrongHits(wrongCounter);
    }
}

void BmnMatchRecoToMC::CalculateTrackQuality(BmnGemTrack* locTr, BmnTrackMatch* trMatch, const TClonesArray* hitMatches, const TClonesArray* points, Int_t& trueCntr, Int_t& falseCntr) {
    // Calculate number of true and wrong hits
    for (Int_t iHit = 0; iHit < locTr->GetNHits(); iHit++) {
        const BmnMatch* hitMatch = (BmnMatch*)(hitMatches->At(locTr->GetHitIndex(iHit)));
        Int_t nofLinks = hitMatch->GetNofLinks();
        Bool_t hasTrue = kFALSE;
        for (Int_t iLink = 0; iLink < nofLinks; iLink++) {
            const FairMCPoint* point = (const FairMCPoint*)(points->At(hitMatch->GetLink(iLink).GetIndex()));
            if (!point) continue;
            if (point->GetTrackID() == trMatch->GetMatchedLink().GetIndex()) {
                hasTrue = true;
                break;
            }
        }
        if (hasTrue) trueCntr++;
        else falseCntr++;
    }
}

BmnStatus BmnMatchRecoToMC::LinkToMC(const TClonesArray* hitMatchArr, const TClonesArray* points, Int_t id, BmnTrackMatch* trackMatch) {

    if (id == -1) return kBMNERROR;
    const BmnMatch* hitMatch = (BmnMatch*)(hitMatchArr->At(id));
    if (hitMatch == nullptr) return kBMNERROR;
    for (Int_t iLink = 0; iLink < hitMatch->GetNofLinks(); ++iLink) {
        const FairMCPoint* point = (const FairMCPoint*)(points->At(hitMatch->GetLink(iLink).GetIndex()));
        if (nullptr == point) return kBMNERROR;
        trackMatch->AddLink(BmnLink(1, point->GetTrackID()));
    }
    return kBMNSUCCESS;
}

BmnStatus BmnMatchRecoToMC::LinkToMC(Int_t id, BmnTrackMatch* trackMatch) {

    if (id == -1) return kBMNERROR;
    const CbmStsHit* hit = (CbmStsHit*)(fStsHits->At(id));
    if (!hit) return kBMNERROR;

    FairMCPoint* point = nullptr;
    for (Int_t iLink = 0; iLink < hit->GetNLinks(); ++iLink) {
        if (hit->GetSystemId() == kGEM) {
            point = (FairMCPoint*)(fGemPoints->At(hit->GetLink(iLink).GetIndex()));
        } else if (hit->GetSystemId() == kSILICON) {
            point = (FairMCPoint*)(fSilPoints->At(hit->GetLink(iLink).GetIndex()));
        }
        if (!point) return kBMNERROR;
        trackMatch->AddLink(BmnLink(1, point->GetTrackID()));
    }
    return kBMNSUCCESS;
}

//L1 STS case
void BmnMatchRecoToMC::MatchGlobalTracks() {

    for (Int_t iTrack = 0; iTrack < fGlobalTracks->GetEntriesFast(); ++iTrack) {
        BmnGlobalTrack* glTrack = (BmnGlobalTrack*)(fGlobalTracks->At(iTrack));
        BmnTrackMatch* trackMatch = new ((*fGlobalTrackMatches)[iTrack]) BmnTrackMatch();

        if (glTrack->GetGemTrackIndex() == -1) continue; //actually GemTrackIndex is StsTrackIndex
        CbmStsTrack* stsTr = (CbmStsTrack*)fStsTracks->At(glTrack->GetGemTrackIndex());
        for (Int_t iHit = 0; iHit < stsTr->GetNStsHits(); ++iHit)
            LinkToMC(stsTr->GetStsHitIndex(iHit), trackMatch);
        if (trackMatch->GetNofLinks() == 0) continue;
        CalculateTrackQuality(stsTr, trackMatch);

        Int_t refId = (trackMatch->GetTrueOverAllHitsRatio() >= 0.6) ? trackMatch->GetMatchedLink().GetIndex() : -1;
        stsTr->SetTrkID(refId);
        glTrack->SetRefIndex(refId);
    }
}

void BmnMatchRecoToMC::CalculateTrackQuality(CbmStsTrack* locTr, BmnTrackMatch* trMatch) {
    // Calculate number of true and wrong hits
    Int_t trueCounter = 0;
    Int_t wrongCounter = 0;
    for (Int_t iHit = 0; iHit < locTr->GetNStsHits(); iHit++) {
        CbmStsHit* hit = (CbmStsHit*)(fStsHits->At(locTr->GetStsHitIndex(iHit)));
        Bool_t hasTrue = kFALSE;
        for (Int_t iLink = 0; iLink < hit->GetNLinks(); iLink++) {
            FairMCPoint* point = nullptr;
            if (hit->GetSystemId() == kGEM) {
                point = (FairMCPoint*)(fGemPoints->At(hit->GetLink(iLink).GetIndex()));
            } else if (hit->GetSystemId() == kSILICON) {
                point = (FairMCPoint*)(fSilPoints->At(hit->GetLink(iLink).GetIndex()));
            }
            if (!point) continue;
            if (point->GetTrackID() == trMatch->GetMatchedLink().GetIndex()) {
                hasTrue = true;
                break;
            }
        }
        if (hasTrue) trueCounter++;
        else wrongCounter++;
    }

    trMatch->SetNofTrueHits(trueCounter);
    trMatch->SetNofWrongHits(wrongCounter);
}

ClassImp(BmnMatchRecoToMC);
