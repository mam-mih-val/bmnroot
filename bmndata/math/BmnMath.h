#ifndef BMNMATH_H_
#define BMNMATH_H_

#include "FairTrackParam.h"
#include "BmnGemStripHit.h"
#include "BmnGemTrack.h"
#include "BmnGemStripHit.h"
#include "TLorentzVector.h"
#include "TF1.h"

class CbmHit;
class CbmStripHit;
class CbmPixelHit;
class CbmGlobalTrack;

#define ANSI_COLOR_RED   "\x1b[91m"
#define ANSI_COLOR_BLUE  "\x1b[94m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_YELLOW_BG "\x1b[33;7m"
#define ANSI_COLOR_BLUE_BG "\x1b[94;7m"

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

Float_t ChiSq(const FairTrackParam* par, const BmnGemStripHit* hit);

Int_t NDF(const BmnGemTrack* track);

}

Float_t ChiSq(const TVector3* par, const BmnGemTrack* tr, const TClonesArray* arr, const TString type);
TVector3 SpiralFit(BmnGemTrack* tr, const TClonesArray* arr);
TLorentzVector SpiralParabolicFit(BmnGemTrack* tr, const TClonesArray* arr);
TVector3 SpiralFitByTwoPoints(const BmnGemTrack* tr, const TClonesArray* arr);
Bool_t IsParCorrect(const FairTrackParam* par, const Bool_t isField);
Float_t NumericalRootFinder(TF1 f, Float_t left, Float_t right);
TVector3 LineFit(BmnTrack* track, const TClonesArray* arr, TString type);
void LineFit(Double_t&, Double_t&, BmnGemTrack*, TClonesArray*, Int_t, Int_t);
TVector3 CircleFit(BmnGemTrack* track, const TClonesArray* arr, Double_t &chi2);
TVector3 CircleBy3Hit(BmnGemTrack* track, const TClonesArray* arr);
TVector3 CircleBy3Hit(BmnGemTrack* track, const BmnGemStripHit* h0, const BmnGemStripHit* h1, const BmnGemStripHit* h2);
TVector3 Pol2By3Hit(BmnGemTrack* track, const TClonesArray* arr);
void DrawHits(BmnGemTrack* track, const TClonesArray* arr);
Float_t Sqr(Float_t x);
Float_t NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22);
Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);

void fit_seg(Double_t*, Double_t*, Double_t*, Double_t*, Int_t, Int_t);
void Pol2Fit(BmnGemTrack*, const TClonesArray*, Double_t&, Double_t&, Double_t&, Int_t);

void DrawBar(UInt_t iEv, UInt_t nEv);
void DrawBar(Long64_t iEv, Long64_t nEv);

// Some supplementary functions to calculate Tukey weights
vector <Double_t> dist(vector <Double_t>, Double_t);
vector <Double_t> W(vector <Double_t>, Double_t);
Double_t Sigma(vector <Double_t>, vector <Double_t>);
Double_t Mu(vector <Double_t>, vector <Double_t>);

#endif /*BMNMATH_H_*/
