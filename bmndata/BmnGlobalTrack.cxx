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
    fTime(0.0),
    fBeta(0.0),
    fMass2(0.0),
    fdQdNLower(0.0),
    fdQdNUpper(0.0),
    fIsPrimary(kTRUE)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
BmnGlobalTrack::~BmnGlobalTrack() { }
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void BmnGlobalTrack::Print() const {
  //cout << endl << "MwpcTrack " << fMwpc1Track << ", SilHit " << fSilTrack << ", GemTrack " << fGemTrack << ", Tof1Hit " << fTof1Hit << ", Tof2Hit " << fTof2Hit << ", DchTrack " << fDchTrack << endl;
}
// -------------------------------------------------------------------------


ClassImp(BmnGlobalTrack)
