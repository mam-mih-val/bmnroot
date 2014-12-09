/** @file CbmTofTrack.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.10.2013
 **
 **/

#include "CbmTofTrack.h"

CbmTofTrack::CbmTofTrack() :
   TObject(),
   fGlbTrack(-1),
   fTofHit(-1),
   fTrackLength(0.),
   fTrackPar(),
   fPidHypo(-1),
   fDistance(0.)
{
}

CbmTofTrack::~CbmTofTrack() {
}

ClassImp(CbmTofTrack)



