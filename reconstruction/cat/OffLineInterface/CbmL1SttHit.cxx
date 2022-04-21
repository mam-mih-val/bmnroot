
#include "CbmL1SttHit.h"
#include "CbmSttHit.h"
#include "CbmKFTrackInterface.h"
#include "CbmKF.h"
#include "TMath.h"

ClassImp(CbmL1SttHit);

void CbmL1SttHit::Create( CbmSttHit *h, int ind )
{
  static const Double_t angle = 10. * TMath::DegToRad();

  Double_t phi = 0.; // rotation angle
  Int_t plane = h->GetPlaneID();
  Int_t irot = (plane - 1) % 6;
  irot = irot / 2;
  //if (irot == 1) phi = angle;
  //else if (irot == 2) phi = -angle;
  if (irot == 1) phi = -angle;
  else if (irot == 2) phi = angle;
  FitPoint.Set (h->GetZ(), h->GetU(), phi, h->GetDx()*h->GetDx());
  MaterialIndex = 0;
  time = 0.;
  iStation = h->GetPlaneID()-1; 

  /*
  FitPoint.x = h->GetX();
  FitPoint.y = h->GetY();
  FitPoint.z = h->GetZ();
  FitPoint.V[0] = h->GetDx()*h->GetDx();
  FitPoint.V[1] = 0;
  FitPoint.V[2] = h->GetDy()*h->GetDy();

  CbmKF *KF = CbmKF::Instance();
  iStation = h->GetStationNr()-1; 
  MaterialIndex = KF->GetMaterialIndex(KF->MuchStation2MCIDMap[iStation]);
  time = h->GetTime(0);
  */
  busy   = 0;
  index = ind;
}

Int_t CbmL1SttHit::Filter( CbmKFTrackInterface &track, Bool_t downstream, Double_t &QP0 )
{
  Bool_t err = 0;
  err = err || track.Propagate( FitPoint.z, QP0 );
  err = err || FitPoint.Filter( track );
  return err;
}
