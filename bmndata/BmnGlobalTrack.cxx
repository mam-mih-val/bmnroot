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
    fTof1Hit(-1),
    fTof2Hit(-1),
    fDch1Hit(-1),
    fDch2Hit(-1),
    fFitNodes()
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
BmnGlobalTrack::~BmnGlobalTrack() { }
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void BmnGlobalTrack::Print() const {
  cout << endl << "GemTrack " << fGemTrack << ", Tof1Hit " << fTof1Hit << ", Dch1Hit " << fDch1Hit
       << ", Dch2Hit " << fDch2Hit << ", Tof2Hit " << fTof2Hit << endl;
}
// -------------------------------------------------------------------------


ClassImp(BmnGlobalTrack)
