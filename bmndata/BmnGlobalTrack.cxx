// -------------------------------------------------------------------------
// -----                    BmnGlobalTrack source file                 -----
// -----                          Created S. Merts                     -----
// -------------------------------------------------------------------------
#include "BmnGlobalTrack.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
BmnGlobalTrack::BmnGlobalTrack()
    : fGemTrack(-1),
    fSilTrack(-1),
    fSsdTrack(-1),
    fTof1Hit(-1),
    fTof2Hit(-1),
    fDch1Track(-1),
    fDch2Track(-1),
    fDchTrack(-1),
    fMwpc1Track(-1),
    fMwpc2Track(-1),
    fUpstreamTrack(-1),
    fBeta400(-1000.0),
    fBeta700(-1000.0),
    fdQdNLower(0.0),
    fdQdNUpper(0.0),
    fA(-1),
    fZ(0),
    fPDG(0),
    fIsPrimary(kTRUE) {

    fCscHit.resize(4);
    for (auto &idx : fCscHit)
        idx = -1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnGlobalTrack::~BmnGlobalTrack() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
//void BmnGlobalTrack::Print() const {

    //cout << endl << "MwpcTrack " << fMwpc1Track << ", SilHit " << fSilTrack << ", GemTrack " << fGemTrack << ", Tof1Hit " << fTof1Hit << ", Tof2Hit " << fTof2Hit << ", DchTrack " << fDchTrack << endl;
//}
// -------------------------------------------------------------------------

void BmnGlobalTrack::SetBeta(Double_t b, Int_t tofID) {
    if (tofID == 1)
        fBeta400 = b;
    else if (tofID == 2)
        fBeta700 = b;
}

Double_t BmnGlobalTrack::GetMass2(Int_t tofID) {
    // p^2/gamma^2/beta^2
    Double_t beta = this->GetBeta(tofID);
    if (beta < -999.0) return -1000.0;
    return TMath::Sq(this->GetP()) * (1 / beta / beta - 1);
}

PidParticles BmnGlobalTrack::GetParticleTof400() {
    if (this->GetTof1HitIndex() != -1) {
        Int_t maxInd = std::max_element(fPidTof400.begin(), fPidTof400.end()) - fPidTof400.begin();
        return static_cast<PidParticles>(maxInd);
    }
    else return EndPidEnum;
}

PidParticles BmnGlobalTrack::GetParticleTof700() {
    if (this->GetTof2HitIndex() != -1) {
        Int_t maxInd = std::max_element(fPidTof700.begin(), fPidTof700.end()) - fPidTof700.begin();
        return static_cast<PidParticles>(maxInd);
    }
    else return EndPidEnum;
}

PidParticles BmnGlobalTrack::GetParticle() {
    if (GetParticleTof400() == GetParticleTof700())
        return GetParticleTof400();

    Double_t maxTof400 = 0;
    Double_t maxTof700 = 0;
    if (this->GetTof1HitIndex() != -1) maxTof400 = *std::max_element(fPidTof400.begin(), fPidTof400.end());
    if (this->GetTof2HitIndex() != -1) maxTof700 = *std::max_element(fPidTof700.begin(), fPidTof700.end());

    if (maxTof400 < maxTof700)
        return GetParticleTof700();
    else
        return GetParticleTof400();
}

ClassImp(BmnGlobalTrack)
