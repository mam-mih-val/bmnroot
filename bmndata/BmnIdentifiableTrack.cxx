#include "BmnIdentifiableTrack.h"

BmnIdentifiableTrack::BmnIdentifiableTrack(){
  fMassQ = 0.;
  fBeta = 0.;
  fLength = 0.;
  fPq = 0.;
  fTime = 0.;
  fTOF700Plane = -1;
  fTOF700Strip = -1;
  fTOF700Amplitude = 0.;
  fTOF700StripHits = 0;
}

BmnIdentifiableTrack::~BmnIdentifiableTrack(){}

ClassImp(BmnIdentifiableTrack)
