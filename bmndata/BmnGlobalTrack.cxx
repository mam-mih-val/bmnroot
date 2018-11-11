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
    fDch1Hit(-1),
    fDch2Hit(-1),
    fDchTrack(-1),
    fMwpcTrack(-1)
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
BmnGlobalTrack::~BmnGlobalTrack() { }
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void BmnGlobalTrack::Print() const {
  cout << endl << "MwpcTrack " << fMwpcTrack << ", SilHit " << fSilTrack << ", GemTrack " << fGemTrack << ", Tof1Hit " << fTof1Hit << ", Tof2Hit " << fTof2Hit << ", DchTrack " << fDchTrack << endl;
}
// -------------------------------------------------------------------------


ClassImp(BmnGlobalTrack)
