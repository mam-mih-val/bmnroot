#include "CbmStsKFTrackFitter.h"

#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmKFStsHit.h"

#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "CbmMvdHit.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iostream>
#include "math.h"

using std::cout;
using std::endl;


ClassImp(CbmStsKFTrackFitter);

CbmStsKFTrackFitter::CbmStsKFTrackFitter(Bool_t CheckTrigSi):
  fHits(),
  fMvdHitsArray(0),
  fStsHitsArray(0),
  fIsInitialised(0),
  fCheckTrigSi(CheckTrigSi)
{}

void CbmStsKFTrackFitter::Init(){
  // Initialisation
  FairRootManager *rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    cout << "-E- CbmStsKFTrackFitter::Init(): "
	 << "ROOT manager is not instantiated!" << endl;
    return;
  }
  if(!fCheckTrigSi){
  fStsHitsArray      = reinterpret_cast<TClonesArray*>(rootMgr->GetObject("StsHit"));
  if ( !fStsHitsArray ) {
    cout << "-W- CbmStsKFTrackFitter::Init: "
	 << "no STS hits array" << endl;
    //return;
  }
  } else {

 fStsHitsArray      = reinterpret_cast<TClonesArray*>(rootMgr->GetObject("StsHitSi"));
  if ( !fStsHitsArray ) {
    cout << "-W- CbmStsKFTrackFitter::Init: "
	 << "no STS hits array" << endl;
    //return;
  }
  }
  fMvdHitsArray   = reinterpret_cast<TClonesArray*>(rootMgr->GetObject("MvdHit"));
  if( !fMvdHitsArray ) {
    cout << "-W- CbmStsKFTrackFitter::Init: "
  	 << "no MVD hits array" << endl;
    //return;
  }
  fIsInitialised = 1;
};

void CbmStsKFTrackFitter::SetKFHits(CbmKFTrack &T, CbmStsTrack* track){

  T.fHits.clear();

  if( !fIsInitialised ) Init();

  Int_t NStsHits   = ( fStsHitsArray   ) ?track->GetNStsHits()   :0;
  Int_t NMvdHits= ( fMvdHitsArray) ?track->GetNMvdHits()   :0;
//cout<<" hits arr size: "<<NStsHits<<endl;
  fHits.resize( NMvdHits + NStsHits );
  if ( NMvdHits > 0 ){
    for (Int_t i=0; i<NMvdHits;i++){
      Int_t j = track->GetMvdHitIndex(i);
      fHits[i].Create( reinterpret_cast<CbmMvdHit*>(fMvdHitsArray->At(j)) );
      T.fHits.push_back(&(fHits[i]));
    }
  }
  if( NStsHits>0 && fStsHitsArray ){
    for (Int_t i=0; i<NStsHits;i++){
      Int_t j = track->GetStsHitIndex(i);
    //  cout<<" hits arr index: "<<j<<endl;
  if(!bkg)    fHits[NMvdHits+i].Create( reinterpret_cast<CbmStsHit*>(fStsHitsArray->At(j)) );
  //cout<<" hit: "<<(CbmStsHit*)track->GetStsHitArr()->At(j)<<endl;
  if(bkg)     fHits[NMvdHits+i].Create( reinterpret_cast<CbmStsHit*>(track->GetStsHitArr()->At(i)) );
      T.fHits.push_back(&(fHits[NMvdHits+i]));
    }
  }
}

Int_t CbmStsKFTrackFitter::DoFit( CbmStsTrack* track, Int_t pidHypo )
{
  track->SetPidHypo(pidHypo);

  CbmKFTrack T;
  T.SetPID(pidHypo);
  SetKFHits( T, track);
  for( Int_t i=0; i<6; i++ ) T.GetTrack()[i] = 0.; // no guess taken
  T.Fit( 1 ); // fit downstream
  CheckTrack( T );
  T.Fit( 0 ); // fit upstream
  CheckTrack( T );
  Int_t err = T.Fit( 1 ); // fit downstream
  Bool_t ok = (!err) && CheckTrack( T );
  if( ok ){
    T.GetTrackParam( *track->GetParamLast() ); // store fitted track & cov.matrix
    err = T.Fit( 0 ); // fit upstream
    ok = ok && (!err) && CheckTrack( T );
    if( ok ) T.GetStsTrack( *track, 1 );          // store fitted track & cov.matrix & chi2 & NDF
  }
  if( !ok ){
    Double_t *t = T.GetTrack();
    Double_t *c = T.GetCovMatrix();
    for( int i=0; i<6 ; i++) t[i] = 0;
    for( int i=0; i<15; i++) c[i] = 0;
    c[0] = c[2] = c[5] = c[9] = c[14] = 100.;
    T.GetRefChi2() = 100.;
    T.GetRefNDF() = 0;
    T.GetStsTrack( *track, 0 );
    T.GetStsTrack( *track, 1 );
    track->SetFlag(1);
  } else{
    track->SetFlag(0);
  }
  return !ok;
}


void CbmStsKFTrackFitter::Extrapolate( FairTrackParam* track, Double_t z, FairTrackParam* e_track )
{
  if( !track ) return;
  CbmKFTrack T;
  T.SetTrackParam( *track );
  T.Extrapolate( z );
  if( e_track ) T.GetTrackParam( *e_track );
}


void CbmStsKFTrackFitter::Extrapolate( CbmStsTrack* track, Double_t z, FairTrackParam* e_track )
{
  if( !track ) return;
  CbmKFTrack T;
  T.SetPID( track->GetPidHypo() );
  FairTrackParam *fpar = track->GetParamFirst(), *lpar = track->GetParamLast();
  if( z<=fpar->GetZ() ){ // extrapolate first parameters
    T.SetTrackParam( *fpar );
    T.Extrapolate( z );
  }else   if( z<fpar->GetZ()+0.1 ){ // extrapolate first parameters
    T.SetTrackParam( *fpar );
    T.Propagate( z );
  }else if( lpar->GetZ()<=z ){ // extrapolate last parameters
    T.SetTrackParam( *lpar );
    T.Extrapolate( z );
  }else if( lpar->GetZ()-0.1<z ){ // extrapolate last parameters
    T.SetTrackParam( *lpar );
    T.Propagate( z );
  }else { // refit with smoother
    SetKFHits( T, track);
    T.SetTrackParam( *fpar );
    T.Smooth( z );
  }
  if( e_track ) T.GetTrackParam( *e_track );
}


Double_t  CbmStsKFTrackFitter::GetChiToVertex( CbmStsTrack* track,  CbmVertex *vtx )
{
  if( !vtx ){
    FairRootManager *fManger = FairRootManager::Instance();
    vtx = reinterpret_cast<CbmVertex *>(fManger->GetObject("PrimaryVertex"));
    if( !vtx ){
      cout<< "-W- CbmStsKFTrackFitter::GetChiToVertex: No Primary Vertex found!"<<endl;
      return 100.;
    }
  }
  CbmKFTrack T;
  T.SetStsTrack( *track, 1 );
  T.Extrapolate( vtx->GetZ() );

  TMatrixFSym tmp(3);
  vtx->CovMatrix( tmp );
  Double_t Cv[3] = { tmp(0,0), tmp(0,1), tmp(1,1) };

  return CbmKFMath::getDeviation( T.GetTrack()[0], T.GetTrack()[1], T.GetCovMatrix(),
				  vtx->GetX(), vtx->GetY(), Cv );
}


Double_t CbmStsKFTrackFitter::FitToVertex( CbmStsTrack* track, CbmVertex *vtx, FairTrackParam *v_track )
{
  Double_t ret = 100.;
  if( !track || !vtx || !v_track ) return ret;
  CbmKFTrack T( *track );
  CbmKFVertex V( *vtx );
  T.Fit2Vertex( V );
  T.GetTrackParam( *v_track );
  if( T.GetRefNDF()>0 && T.GetRefChi2()>=0 ){
    ret = T.GetRefChi2()/T.GetRefNDF();
    if( finite(ret) ) ret =  sqrt(ret);
  }
  return ret;
}

Bool_t CbmStsKFTrackFitter::CheckTrack( CbmKFTrack &T )
{
  Bool_t ok = 1;
  Double_t *t = T.GetTrack();
  Double_t *c = T.GetCovMatrix();
  for( int i=0; i<6; i++) ok = ok && finite(t[i]) && TMath::Abs(t[i])<1.e5;
  for( int i=0; i<15; i++) ok = ok && finite(c[i]);
  ok = ok && finite( T.GetMass() ) && finite( T.GetRefChi2() );
  if( ok ){
    ok = ok && ( c[ 0] > 0 );
    ok = ok && ( c[ 2] > 0 );
    ok = ok && ( c[ 5] > 0 );
    ok = ok && ( c[ 9] > 0 );
    ok = ok && ( c[14] > 0 );
  }
  if( ok ){ // correct the cov matrix
    Double_t c00 = TMath::Sqrt(c[ 0]);
    Double_t c11 = TMath::Sqrt(c[ 2]);
    Double_t c22 = TMath::Sqrt(c[ 5]);
    Double_t c33 = TMath::Sqrt(c[ 9]);
    Double_t c44 = TMath::Sqrt(c[14]);
    Double_t a = c11*c00;
    if( c[ 1] >  a ) c[ 1] =  a;
    if( c[ 1] < -a ) c[ 1] = -a;
    a = c22*c00;
    if( c[ 3] >  a ) c[ 3] =  a;
    if( c[ 3] < -a ) c[ 3] = -a;
    a = c22*c11;
    if( c[ 4] >  a ) c[ 4] =  a;
    if( c[ 4] < -a ) c[ 4] = -a;
    a = c33*c00;
    if( c[ 6] >  a ) c[ 6] =  a;
    if( c[ 6] < -a ) c[ 6] = -a;
    a = c33*c11;
    if( c[ 7] >  a ) c[ 7] =  a;
    if( c[ 7] < -a ) c[ 7] = -a;
    a = c33*c22;
    if( c[ 8] >  a ) c[ 8] =  a;
    if( c[ 8] < -a ) c[ 8] = -a;
    a = c44*c00;
    if( c[10] >  a ) c[10] =  a;
    if( c[10] < -a ) c[10] = -a;
    a = c44*c11;
    if( c[11] >  a ) c[11] =  a;
    if( c[11] < -a ) c[11] = -a;
    a = c44*c22;
    if( c[12] >  a ) c[12] =  a;
    if( c[12] < -a ) c[12] = -a;
    a = c44*c33;
    if( c[13] >  a ) c[13] =  a;
    if( c[13] < -a ) c[13] = -a;
  }
  if( !ok ){
    for( int i=0; i<6 ; i++) t[i] = 0;
    for( int i=0; i<15; i++) c[i] = 0;
    c[0] = c[2] = c[5] = c[9] = c[14] = 100.;
    T.GetRefChi2() = 100.;
    T.GetRefNDF() = 0;
  }
  return ok;
}
