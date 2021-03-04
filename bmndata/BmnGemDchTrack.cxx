#include "BmnGemDchTrack.h"

BmnGemDchTrack::BmnGemDchTrack() :
fGemTrack(),
fDchTrack(),
fLength(0.),
fEventId(-1),
fEventHits(0),
fSiHits(0),
fGemHits(0),
fCSCHits(0),     
fDx(0.),
fDy(0.),
fDtx(0.),
fDty(0.),
fDdx(0.),
fDdy(0.),
fDdtx(0.),
fDdty(0.),
fChi2(0.),
fXpv(0.),
fYpv(0.),
fTrackAngDch(0.),
fTrackAngPV(0.),
fIntMagFieldTr(0.),
fIntMagFieldDch(0.),
fXSigma(0.),
fYSigma(0.),
fFlag(0.),
fZpv(0.) {
}

Double_t BmnGemDchTrack::GetDistance() {
    return fDx*fDx + fDy*fDy;
}

BmnGemDchTrack::~BmnGemDchTrack(){}

ClassImp(BmnGemDchTrack)
