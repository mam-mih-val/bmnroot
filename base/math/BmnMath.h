#ifndef BMNMATH_H_
#define BMNMATH_H_

#include "FairTrackParam.h"
#include "BmnTrack.h"
#include "BmnHit.h"
#include "TLorentzVector.h"
#include "TVectorD.h"
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
#define ANSI_BOLD "\e[1m"
#define ANSI_DEFAULT "\e[0m"

#define DBG(a) printf(ANSI_BOLD "Trace %s: %s" ANSI_DEFAULT " (%s:%d)\n", __func__, a, __FILE__, __LINE__);
#define DBGERR(a) fprintf(stderr, ANSI_COLOR_RED "!!! %s error in %s: %s" ANSI_DEFAULT " (%s:%d)\n", a, __func__, strerror(errno), __FILE__, __LINE__);

namespace lit
{


Float_t ChiSq(const FairTrackParam* par, const BmnHit* hit);

Int_t NDF(const BmnTrack* track);

}

Bool_t IsParCorrect(const FairTrackParam* par, const Bool_t isField);
Bool_t IsParCorrect(const FairTrackParam* par);
Float_t NumericalRootFinder(TF1 f, Float_t left, Float_t right);
TVector3 LineFit(BmnTrack* track, const TClonesArray* arr, TString type);
void LineFit(Double_t&, Double_t&, BmnTrack*, TClonesArray*, Int_t, Int_t);
TVector3 LineFitBy3Hits(const BmnHit* h0, const BmnHit* h1, const BmnHit* h2);
TVector3 CircleFit(BmnTrack* track, const TClonesArray* arr, Double_t &chi2);
TVector3 CircleFit(vector<BmnHit*> hits, Int_t idSkip);
TVector3 CircleBy3Hit(BmnTrack* track, const TClonesArray* arr);
TVector3 CircleBy3Hit(const BmnHit* h0, const BmnHit* h1, const BmnHit* h2);
TVector3 Pol2By3Hit(BmnTrack* track, const TClonesArray* arr);
Double_t CalcTx(const BmnHit* h0, const BmnHit* h1, const BmnHit* h2);
void DrawHits(BmnTrack* track, const TClonesArray* arr);
Float_t NewtonSolver(Float_t A0, Float_t A1, Float_t A2, Float_t A22);
Float_t Dist(Float_t x1, Float_t y1, Float_t x2, Float_t y2);

void fit_seg(Double_t*, Double_t*, Double_t*, Double_t*, Int_t, Int_t);
void Pol2Fit(BmnTrack*, const TClonesArray*, Double_t&, Double_t&, Double_t&, Int_t);
TVector3 Pol2Fit(vector <BmnHit*>, Int_t);
TVector2 LineFit(vector<BmnHit*>, Int_t, TString);

void DrawBar(UInt_t iEv, UInt_t nEv);
void DrawBar(Long64_t iEv, Long64_t nEv);

// Some supplementary functions to calculate Tukey weights
vector <Double_t> dist(vector <Double_t>, Double_t);
vector <Double_t> W(vector <Double_t>, Double_t);
Double_t Sigma(vector <Double_t>, vector <Double_t>);
Double_t Mu(vector <Double_t>, vector <Double_t>);

Double_t GetVZByTwoStraightTracks(BmnTrack* tr0, BmnTrack* tr1, Double_t& dist);
Double_t GetVzByVectorStraightTracks(vector<BmnTrack> tr, Double_t& dist);

void UpdateTrackParam(FairTrackParam* initPar, const FairTrackParam* detPar, Double_t& chiSq);

#endif /*BMNMATH_H_*/
