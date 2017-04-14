#ifndef BMNMATH_H_
#define BMNMATH_H_

#include "FairTrackParam.h"
#include "BmnGemHit.h"
#include "BmnGemTrack.h"
#include "BmnGemStripHit.h"
#include "TLorentzVector.h"
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

Float_t ChiSq(const TVector3* par, const BmnGemTrack* tr, const TClonesArray* arr, const TString type);
TVector3 SpiralFit(BmnGemTrack* tr, const TClonesArray* arr);
TLorentzVector SpiralParabolicFit(BmnGemTrack* tr, const TClonesArray* arr);
TVector3 SpiralFitByTwoPoints(const BmnGemTrack* tr, const TClonesArray* arr);
Bool_t IsParCorrect(const FairTrackParam* par);
Float_t NumericalRootFinder(TF1 f, Float_t left, Float_t right);
TVector3 LineFit(BmnTrack* track, const TClonesArray* arr, TString type);
TVector3 CircleFit(BmnGemTrack* track, const TClonesArray* arr, Double_t &chi2);
TVector3 CircleBy3Hit(BmnGemTrack* track, const TClonesArray* arr);
TVector3 CircleBy3Hit(BmnGemTrack* track, const BmnGemStripHit* h0, const BmnGemStripHit* h1, const BmnGemStripHit* h2);
Float_t Sqr(Float_t x);
Float_t NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22);
Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);

void fit_seg(Double_t*, Double_t*, Double_t*, Double_t*, Int_t, Int_t);

#endif /*BMNMATH_H_*/
