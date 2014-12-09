#include "BmnMath.h"

#include "FairTrackParam.h"
#include "CbmHit.h"
#include "CbmStripHit.h"
#include "CbmPixelHit.h"
#include "CbmGlobalTrack.h"

#include <iostream>
#include <cmath>

namespace lit
{

Float_t ChiSq(const FairTrackParam* par, const CbmHit* hit) {
   Float_t chisq = 0.;                                                          //FIXME
//   if (hit->GetType() == kLITSTRIPHIT) {
//      chisq = ChiSq(par, (CbmStripHit*)hit);
//   } else if (hit->GetType() == kLITPIXELHIT) {
//      chisq = ChiSq(par, (CbmPixelHit*) hit);
//   }
   return chisq;
}

Float_t ChiSq(                                                                  //FIXME
   const FairTrackParam* par,
   const CbmStripHit* hit)
{
//   Float_t duu = hit->GetDu() * hit->GetDu();
//   Float_t phiCos = 0;//hit->GetCosPhi();                                       //FIXME
//   Float_t phiSin = 1;//hit->GetSinPhi();                                       //FIXME
//   Float_t phiCosSq = phiCos * phiCos;
//   Float_t phiSinSq = phiSin * phiSin;
//   Float_t phi2SinCos = 2 * phiCos * phiSin;
//   Float_t C0 = par->GetCovariance(0);
//   Float_t C1 = par->GetCovariance(1);
//   Float_t C5 = par->GetCovariance(5);
//
//   Float_t ru = hit->GetU() - par->GetX() * phiCos - par->GetY() * phiSin;
//
//   return (ru * ru) / (duu - phiCosSq*C0 - phi2SinCos*C1 - phiSinSq*C5);
   return 0;
}

Float_t ChiSq(                                                                  //FIXME
   const FairTrackParam* par,
   const CbmPixelHit* hit)
{
//   Float_t dxx = hit->GetDx() * hit->GetDx();
//   Float_t dxy = hit->GetDxy();
//   Float_t dyy = hit->GetDy() * hit->GetDy();
//   Float_t xmx = hit->GetX() - par->GetX();
//   Float_t ymy = hit->GetY() - par->GetY();
//   Float_t C0 = par->GetCovariance(0);
//   Float_t C1 = par->GetCovariance(1);
//   Float_t C5 = par->GetCovariance(5);
//
//   Float_t norm = dxx * dyy - dxx * C5 - dyy * C0 + C0 * C5
//              - dxy * dxy + 2 * dxy * C1 - C1 * C1;
//   if (norm == 0.) { norm = 1e-10; }
//   return ((xmx * (dyy - C5) - ymy * (dxy - C1)) * xmx
//           +(-xmx * (dxy - C1) + ymy * (dxx - C0)) * ymy) / norm;
    return 0;
}

Int_t NDF(const CbmGlobalTrack* track) {
   Int_t ndf = 0;
//   for (Int_t i = 0; i < track->GetNofHits(); i++) {
//      if (track->GetHit(i)->GetType() == kLITPIXELHIT) { ndf += 2; }
//      else if (track->GetHit(i)->GetType() == kLITSTRIPHIT) { ndf++; }
//   }
//   ndf -= 5;
   ndf = track->GetNofHits();
   if (ndf > 0) { return ndf; }
   else { return 1; }
}

}

