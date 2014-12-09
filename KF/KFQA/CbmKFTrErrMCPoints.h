/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */

#ifndef _CbmKFTrErrMCPoints_h_
#define _CbmKFTrErrMCPoints_h_

#include <CbmMvdPoint.h>
#include <CbmStsPoint.h>
#include <CbmMvdHit.h>
#include <CbmStsHit.h>
#include <FairMCPoint.h>

#include "TClonesArray.h"
#include <vector>

class CbmMCTrack;

using std::vector;

class CbmKFTrErrMCPoints : public TObject {

 public:

  CbmKFTrErrMCPoints();
  ~CbmKFTrErrMCPoints() {};

  CbmMvdPoint* GetMvdPoint(Int_t i) {return MvdArray[i];}
  CbmStsPoint* GetStsPoint(Int_t i) {return StsArray[i];}
  
  CbmMvdHit* GetMvdHit(Int_t i) {return MvdHitsArray[i];}
  CbmStsHit* GetStsHit(Int_t i) {return StsHitsArray[i];}

  void SetMvdPoint(CbmMvdPoint* mp) { MvdArray.push_back(mp); }
  void SetStsPoint(CbmStsPoint* sp) { StsArray.push_back(sp); }

  int GetNMvdPoints() const { return MvdArray.size(); }
  int GetNStsPoints() const { return StsArray.size(); }
  int GetNMvdHits() const { return MvdHitsArray.size(); }
  int GetNStsHits() const { return StsHitsArray.size(); }
  int GetNConsMCStations();
  int GetNConsHitStations();
  int GetNHitStations();
  int GetNMaxMCPointsOnStation();
  Bool_t IsReconstructable(CbmMCTrack* mcTr, int MinNStations, int PerformanceMode, float MinRecoMom);
  
  double GetMvdPointX(int i) {return MvdArray[i]->FairMCPoint::GetX();}
  double GetMvdPointY(int i) {return MvdArray[i]->FairMCPoint::GetY();}
  double GetMvdPointZ(int i) {return MvdArray[i]->GetZ();}
  double GetMvdPointPx(int i) {return MvdArray[i]->GetPx();}
  double GetMvdPointPy(int i) {return MvdArray[i]->GetPy();}
  double GetMvdPointPz(int i) {return MvdArray[i]->GetPz();}

  double GetStsPointX(int i) {return StsArray[i]->FairMCPoint::GetX();}
  double GetStsPointY(int i) {return StsArray[i]->FairMCPoint::GetY();}
  double GetStsPointZ(int i) {return StsArray[i]->GetZ();}
  double GetStsPointPx(int i) {return StsArray[i]->GetPx();}
  double GetStsPointPy(int i) {return StsArray[i]->GetPy();}
  double GetStsPointPz(int i) {return StsArray[i]->GetPz();}

  vector<CbmStsPoint*> StsArray;
  vector<CbmMvdPoint*> MvdArray;
  
  vector<CbmStsHit*> StsHitsArray;
  vector<CbmMvdHit*> MvdHitsArray;
  
  ClassDef(CbmKFTrErrMCPoints,1);
};

#endif // _CbmKFTrErrMCPoints_h_ 
