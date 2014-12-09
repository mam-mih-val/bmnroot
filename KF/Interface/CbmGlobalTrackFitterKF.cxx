// ------------------------------------------------------------------
// -----                  CbmGlobalTrackFitterKF                -----
// -----              Created 07/03/2006 by D.Kresan            -----
// ------------------------------------------------------------------
#include "CbmGlobalTrackFitterKF.h"

#include "CbmKFStsHit.h"
#include "CbmKFTrdHit.h"
#include "CbmKFTrack.h"

#include "FairRootManager.h"
#include "CbmStsHit.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"
#include "CbmStsTrack.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmVertex.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;

//___________________________________________________________________
//
// CbmGlobalTrackFitterKF
//
// Concrete implementation of global track fitting based on the
// Kalman filter
//


// ------------------------------------------------------------------
CbmGlobalTrackFitterKF::CbmGlobalTrackFitterKF():
  fArrayStsHit(NULL),
  fArrayTrdHit(NULL),
  fArrayTofHit(NULL),
  fArrayStsTrack(NULL),
  fArrayTrdTrack(NULL),
  fPrimVertex(NULL),
  fKfTrack(NULL)
{
  // Default constructor

  fKfTrack = new CbmKFTrack();
  // Set mass hypothesis
  fKfTrack->SetPID(211);
  fVerbose = 0;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmGlobalTrackFitterKF::~CbmGlobalTrackFitterKF()
{
  // Destructor
  delete fKfTrack;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmGlobalTrackFitterKF::Init()
{
  // Initialisation

  // Get pointer to the ROOT I/O manager
  FairRootManager* rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    cout << "-E- CbmGlobalTrackFitterKF::Init :"
	 << " ROOT manager is not instantiated" << endl;
    return;
  }
  // Get hit arrays
  fArrayStsHit = (TClonesArray*) rootMgr->GetObject("StsHit");
  if(NULL == fArrayStsHit) {
    cout << "-W- CbmGlobalTrackFitterKF::Init :"
	 << " no Sts hit array" << endl;
  }
  fArrayTrdHit = (TClonesArray*) rootMgr->GetObject("TrdHit");
  if(NULL == fArrayTrdHit) {
    cout << "-W- CbmGlobalTrackFitterKF::Init :"
	 << " no TRD hit array" << endl;
  }
  fArrayTofHit = (TClonesArray*) rootMgr->GetObject("TofHit");
  if(NULL == fArrayTofHit) {
    cout << "-W- CbmGlobalTrackFitterKF::Init :"
	 << " no TOF hit array" << endl;
  }
  // Get track arrays
  fArrayStsTrack = (TClonesArray*) rootMgr->GetObject("StsTrack");
  if(NULL == fArrayStsTrack) {
    cout << "-W- CbmGlobalTrackFitterKF::Init : "
	 << "no STS track array!" << endl;
  }
  fArrayTrdTrack = (TClonesArray*) rootMgr->GetObject("TrdTrack");
  if(NULL == fArrayTrdTrack) {
    cout << "-W- CbmGlobalTrackFitterKF::Init : "
	 << "no TRD track array!" << endl;
  }
  fPrimVertex = (CbmVertex*) rootMgr->GetObject("PrimaryVertex");
  if(NULL == fPrimVertex) {
    cout << "-W- CbmGlobalTrackFitterKF::Init : "
	 << "no Primary Vertex!" << endl;
  }
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmGlobalTrackFitterKF::DoFit(CbmGlobalTrack* glbTrack)
{
  // Implementation of the fitting algorithm
  if(NULL==glbTrack || NULL==fArrayStsTrack || NULL==fArrayTrdTrack ||
     NULL==fArrayStsHit ||  NULL==fArrayTrdHit || NULL==fPrimVertex) return;


  Double_t x_old;
  Double_t y_old;
  Double_t z_old;
  Double_t x_new;
  Double_t y_new;
  Double_t z_new;
  Double_t z = fPrimVertex->GetZ();
  Double_t length = 0.;
  
  
  // Get STS track index
  Int_t stsTrackIndex = glbTrack->GetStsTrackIndex();
  if(-1 == stsTrackIndex) {
    return;
  }
  // Get STS track
  CbmStsTrack* stsTrack = (CbmStsTrack*) fArrayStsTrack->
    At(stsTrackIndex);
  if(NULL == stsTrack) {
    return;
  }
  FairTrackParam *paramFirst;
  paramFirst = stsTrack->GetParamFirst();
  fKfTrack->SetTrackParam(*paramFirst);
  fKfTrack->Extrapolate(z);
  x_old = fKfTrack->GetTrack()[0];
  y_old = fKfTrack->GetTrack()[1];
  z_old = z;
  Double_t p = 1.;
  if(paramFirst->GetQp()) {
    p = TMath::Abs(1./paramFirst->GetQp());
  }

  // Int_t stsHitIndex;
  // CbmStsHit* stsHit;
  // // Loop over hits of the STS track
  // for(Int_t iHit = 0; iHit < stsTrack->GetNStsHits(); iHit++) {
  //   // Get hit index
  //   stsHitIndex = stsTrack->GetStsHitIndex(iHit);
  //   // Get hit
  //   stsHit = (CbmStsHit*) fArrayStsHit->At(stsHitIndex);
  //   x_new = stsHit->GetX();
  //   y_new = stsHit->GetY();
  //   z_new = stsHit->GetZ();
  //   length += TMath::Sqrt(TMath::Power(x_new-x_old, 2) +
  // 			  TMath::Power(y_new-y_old, 2) +
  // 			  TMath::Power(z_new-z_old, 2));
  //   // cout << z_old << " ->  " << z_new << " => " << length << endl;
  //   x_old = x_new;
  //   y_old = y_new;
  //   z_old = z_new;
  // }


  // Get TRD track index
  Int_t trdTrackIndex = glbTrack->GetTrdTrackIndex();
  if(-1 == trdTrackIndex) {
    return;
  }
  // Get TRD track
  CbmTrdTrack* trdTrack = (CbmTrdTrack*) fArrayTrdTrack->
    At(trdTrackIndex);
  if(NULL == trdTrack) {
    return;
  }
  if(trdTrack->GetNofHits() < 2) {
    return;
  }
  Int_t trdHitIndex = trdTrack->GetHitIndex(0);
  CbmTrdHit* trdHit = (CbmTrdHit*) fArrayTrdHit->At(trdHitIndex);

  while(z < (trdHit->GetZ()-2.)) {
    z += p*1.;

    fKfTrack->Extrapolate(z);
    x_new = fKfTrack->GetTrack()[0];
    y_new = fKfTrack->GetTrack()[1];
    z_new = z;

    length += TMath::Sqrt(TMath::Power(x_new-x_old, 2) +
  			  TMath::Power(y_new-y_old, 2) +
  			  TMath::Power(z_new-z_old, 2));
    x_old = x_new;
    y_old = y_new;
    z_old = z_new;
  }

  // Loop over hits of the TRD track
  for(Int_t iTrd = 1; iTrd < trdTrack->GetNofHits(); iTrd++) {
    // Get hit index
    trdHitIndex = trdTrack->GetHitIndex(iTrd);
    // Get hit
    trdHit = (CbmTrdHit*) fArrayTrdHit->At(trdHitIndex);
    z = trdHit->GetZ();
    fKfTrack->Extrapolate(z);
    if(trdHit->GetDx() > trdHit->GetDy()) {
      x_new = fKfTrack->GetTrack()[0];
      y_new = trdHit->GetY();
    } else {
      x_new = trdHit->GetX();
      y_new = fKfTrack->GetTrack()[1];
    }
    z_new = z;
    length += TMath::Sqrt(TMath::Power(x_new-x_old, 2) +
			  TMath::Power(y_new-y_old, 2) +
			  TMath::Power(z_new-z_old, 2));
    x_old = x_new;
    y_old = y_new;
    z_old = z_new;
  }

  
  Int_t tofIndex = glbTrack->GetTofHitIndex();
  if(-1 == tofIndex) {
    return;
  }
  CbmTofHit *tofHit = (CbmTofHit*)fArrayTofHit->At(tofIndex);
  x_new = tofHit->GetX();
  y_new = tofHit->GetY();
  z_new = tofHit->GetZ();
  length += TMath::Sqrt(TMath::Power(x_new-x_old, 2) +
			TMath::Power(y_new-y_old, 2) +
			TMath::Power(z_new-z_old, 2));
  

  glbTrack->SetLength(length);
}
// ------------------------------------------------------------------


ClassImp(CbmGlobalTrackFitterKF);

