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
      fCscHit(-1),
      fBeta400(-1000.0),
      fBeta700(-1000.0),
      fdQdNLower(0.0),
      fdQdNUpper(0.0),
      fA(-1),
      fZ(0),
      fPDG(0),
      fIsPrimary(kTRUE) {
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnGlobalTrack::~BmnGlobalTrack() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void BmnGlobalTrack::Print() const {
    //cout << endl << "MwpcTrack " << fMwpc1Track << ", SilHit " << fSilTrack << ", GemTrack " << fGemTrack << ", Tof1Hit " << fTof1Hit << ", Tof2Hit " << fTof2Hit << ", DchTrack " << fDchTrack << endl;
}
// -------------------------------------------------------------------------

void BmnGlobalTrack::SetBeta(Double_t b, Int_t tofID) {
    if (tofID == 1)
        fBeta400 = b;
    else if (tofID = 2)
        fBeta700 = b;
}

Double_t BmnGlobalTrack::GetMass2(Int_t tofID) {
    // p^2/gamma^2/beta^2
    Double_t beta = this->GetBeta(tofID);
    if (beta < -999.0) return -1000.0;
    return TMath::Sq(this->GetP()) * (1 / beta / beta - 1);
}

ClassImp(BmnGlobalTrack)
