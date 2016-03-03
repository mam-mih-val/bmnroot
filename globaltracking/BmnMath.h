#ifndef BMNMATH_H_
#define BMNMATH_H_

#include "FairTrackParam.h"
#include "BmnGemTrack.h"
#include "TF1.h"

class CbmHit;
class CbmStripHit;
class CbmPixelHit;
class CbmGlobalTrack;

namespace lit
{

/* Calculates chi square
 * @param par Pointer to the updated or smoothed track parameter
 * @param hit Pointer to the hit
 * @return chi square contribution for this hit */
Float_t ChiSq(const FairTrackParam* par, const CbmHit* hit);

/* Calculates chi square for strip hits
 * @param par Pointer to the updated or smoothed track parameter
 * @param hit Pointer to the strip hit
 * @return chi square contribution for this strip hit */
Float_t ChiSq(const FairTrackParam* par, const CbmStripHit* hit);

Float_t ChiSq(const FairTrackParam* par, const BmnGemHit* hit);

Int_t NDF(const BmnGemTrack* track);

}

Float_t ChiSq(const TVector3 par, const BmnGemTrack* tr, const TClonesArray* arr);
TVector3 SpiralFit(const BmnGemTrack* tr, const TClonesArray* arr);
Bool_t IsParCorrect(const FairTrackParam* par);
Int_t stationNumber(const string& detName, const Float_t z);
Float_t NumericalRootFinder(TF1 f, Float_t left, Float_t right);
TVector3 LineFit(BmnGemTrack* track, const TClonesArray* arr);
TVector3 CircleBy3Hit(BmnGemTrack* track, const TClonesArray* arr);
Float_t Sqr(Float_t x);

#endif /*BMNMATH_H_*/
