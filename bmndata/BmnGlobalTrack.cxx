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
  : TObject(),
    fGemTrack(-1),
    fTof1Hit(-1),
    fTof2Hit(-1),
    fDch1Hit(-1),
    fDch2Hit(-1),
    fParamFirst(),
    fParamLast(),
    fFitNodes(),
    fNofHits(0),
    fChi2(0.),
    fNDF(0),
    fFlag(0),
    fLength(0.)
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
  cout << "Parameters at first plane: " << endl;
  fParamFirst.Print();
  cout << "Parameters at last plane: " << endl;
  fParamLast.Print();
  cout << "chi2 = " << fChi2 << ", NDF = " << fNDF
       << ", Quality flag " << fFlag << endl;
  cout << "length = " << fLength << endl;
}
// -------------------------------------------------------------------------


ClassImp(BmnGlobalTrack)
