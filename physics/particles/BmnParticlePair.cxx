// @(#)bmnroot/physics/particles:$Id$
// Author: Pavel Batyuk <pavel.batyuk@jinr.ru> 2017-06-20

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// BmnParticlePair                                                            //
//                                                                            //
//  A class to consider selected pair of particles                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "BmnParticlePair.h"

BmnParticlePair::BmnParticlePair() :
fV0XZ(0.), fV0YZ(0.),
fDCA1(0.),
fDCA2(0.),
fDCA12X(0.), fDCA12Y(0.),
fPathX(0.), fPathY(0.),
fInvMassX(0.), fInvMassY(0.),
fMomPart1(0.), fMomPart2(0.),
fEtaPart1(0.), fEtaPart2(0.),
fAlphaX(0.), fPtPodolX(0.),
fAlphaY(0.), fPtPodolY(0.),
fRecoTrackIdPart1(-1), fRecoTrackIdPart2(-1),
fMCTrackIdPart1(-1), fMCTrackIdPart2(-1) {
    fMCMomPart1.SetXYZ(0., 0., 0.);
    fMCMomPart2.SetXYZ(0., 0., 0.);
    fRecoMomPart1.SetXYZ(0., 0., 0.);
    fRecoMomPart2.SetXYZ(0., 0., 0.);
}

// Setters

void BmnParticlePair::SetMCMomPart1(Double_t px, Double_t py, Double_t pz) {
    fMCMomPart1.SetXYZ(px, py, pz);
}

void BmnParticlePair::SetMCMomPart2(Double_t px, Double_t py, Double_t pz) {
       fMCMomPart2.SetXYZ(px, py, pz);
}

// Getters

TVector3 BmnParticlePair::GetMCMomPart1() {
    return fMCMomPart1;
}

TVector3 BmnParticlePair::GetMCMomPart2() {
      return fMCMomPart2;
}

Int_t BmnParticlePair::GetMCTrackIdPart1() {
    return fMCTrackIdPart1;
}

Int_t BmnParticlePair::GetMCTrackIdPart2() {
    return fMCTrackIdPart2;
}

Int_t BmnParticlePair::GetRecoTrackIdPart1() {
    return fRecoTrackIdPart1;
}

Int_t BmnParticlePair::GetRecoTrackIdPart2() {
    return fRecoTrackIdPart2;
}

void BmnParticlePair::SetMCTrackIdPart1(Int_t id) {
    fMCTrackIdPart1 = id;
}

void BmnParticlePair::SetMCTrackIdPart2(Int_t id) {
    fMCTrackIdPart2 = id;
}

void BmnParticlePair::SetRecoTrackIdPart1(Int_t id) {
    fRecoTrackIdPart1 = id;
}

void BmnParticlePair::SetRecoTrackIdPart2(Int_t id) {
    fRecoTrackIdPart2 = id;
}

BmnParticlePair::~BmnParticlePair() {

}