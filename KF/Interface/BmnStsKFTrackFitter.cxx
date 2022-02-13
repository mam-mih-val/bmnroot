// -------------------------------------------------------------------------
// -----                BmnStsKFTrackFitter source file                -----
// -----          Created 27/03/21  by A.Zinchenko, D.Zinchenko        -----
// -----                (from CbmStsKFTrackFitter.cxx)                 -----
// -------------------------------------------------------------------------

//AZ #include "CbmStsKFTrackFitter.h"
#include "BmnStsKFTrackFitter.h"

#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmKFStsHit.h"

#include "FairRootManager.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "CbmMvdHit.h"

#include "TClonesArray.h"
#include <TFile.h>
#include <TKey.h>
#include "TMath.h"
#include <TProfile2D.h>

#include <iostream>
#include "math.h"

using std::cout;
using std::endl;
 

//AZ ClassImp(CbmStsKFTrackFitter);
ClassImp(BmnStsKFTrackFitter);

// -------------------------------------------------------------------------

//AZ CbmStsKFTrackFitter::CbmStsKFTrackFitter():
BmnStsKFTrackFitter::BmnStsKFTrackFitter():
  fHits(),
  fMvdHitsArray(0),
  fStsHitsArray(0),
  fIsInitialised(0)
{}

// -------------------------------------------------------------------------

//AZ void CbmStsKFTrackFitter::Init(){ 
void BmnStsKFTrackFitter::Init(){ 
  // Initialisation
  FairRootManager *rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    //AZ cout << "-E- CbmStsKFTrackFitter::Init(): "
    cout << "-E- BmnStsKFTrackFitter::Init(): "
	 << "ROOT manager is not instantiated!" << endl;
    return;
  }
  fStsHitsArray      = reinterpret_cast<TClonesArray*>(rootMgr->GetObject("StsHit"));
  if ( !fStsHitsArray ) {
    //AZ cout << "-W- CbmStsKFTrackFitter::Init: "
    cout << "-W- BmnStsKFTrackFitter::Init: "
	 << "no STS hits array" << endl;
    //return;
  }
  fMvdHitsArray   = reinterpret_cast<TClonesArray*>(rootMgr->GetObject("MvdHit"));
  if( !fMvdHitsArray ) {
    //AZ cout << "-W- CbmStsKFTrackFitter::Init: "
    cout << "-W- BmnStsKFTrackFitter::Init: "
  	 << "no MVD hits array" << endl;
    //return;
  }
  fIsInitialised = 1;
};

// -------------------------------------------------------------------------

//AZ void CbmStsKFTrackFitter::SetKFHits(CbmKFTrack &T, CbmStsTrack* track){
void BmnStsKFTrackFitter::SetKFHits(CbmKFTrack &T, CbmStsTrack* track){
 
  T.fHits.clear();

  if( !fIsInitialised ) Init();
  
  Int_t NStsHits   = ( fStsHitsArray   ) ?track->GetNStsHits()   :0;
  Int_t NMvdHits= ( fMvdHitsArray) ?track->GetNMvdHits()   :0;

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
      fHits[NMvdHits+i].Create( reinterpret_cast<CbmStsHit*>(fStsHitsArray->At(j)) );
      T.fHits.push_back(&(fHits[NMvdHits+i]));
    }
  }
}

// -------------------------------------------------------------------------

//AZ Int_t CbmStsKFTrackFitter::DoFit( CbmStsTrack* track, Int_t pidHypo )
Int_t BmnStsKFTrackFitter::DoFit( CbmStsTrack* track, Int_t pidHypo )
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
  //cout << " zzz " << track->GetParamFirst()->GetZ() << " " << track->GetParamLast()->GetZ() << " " << track->GetNStsHits() << endl; //AZ
  return !ok;
}

// -------------------------------------------------------------------------

void BmnStsKFTrackFitter::ReadMatBudget(TString &fileName)
{
  // Get material budget histograms

  TFile *f = TFile::Open(fileName,"read");
  const TList *keys = f->GetListOfKeys();
  TIter next(keys);
  TObject *obj;

  while ((obj = next())) {
    TKey *key = (TKey*) obj;
    key->Print();
    if (!(TString(key->GetName()).Contains("Station"))) continue;
    TProfile2D *prof = (TProfile2D*) key->ReadObj();
    //cout << prof->GetName() << endl;
    TString title = prof->GetTitle();
    int pos = title.Last('_');
    TString zstr = title(pos+1,title.Length()-pos);
    Double_t zpos = zstr.Atof() / 10;
    //cout << zstr << " " << zpos << endl;
    fMatHistos[zpos] = prof;
    //cout << prof->GetNbinsX() << " " << prof->GetNbinsY() << endl;
    //file.Get(key->GetName())->Write(key->GetName(),TObject::kSingleKey);
  }
  //fitter.SetMatHistos(&fMatHistos);
}

// -------------------------------------------------------------------------

Int_t BmnStsKFTrackFitter::Fit( CbmStsTrack* track, Int_t pidHypo )
{
  // Track fit with material budget

  track->SetPidHypo(pidHypo);

  CbmKFTrack T;
  T.SetPID(pidHypo);
  SetKFHits( T, track);
  for( Int_t i=0; i<6; i++ ) T.GetTrack()[i] = 0.; // no guess taken
  //AZ T.Fit( 1 ); // fit downstream 
  FitWithMat (T, 1);
  CheckTrack( T );
  //AZ T.Fit( 0 ); // fit upstream  
  FitWithMat (T, 0);
  CheckTrack( T );
  //AZ Int_t err = T.Fit( 1 ); // fit downstream 
  Int_t err = FitWithMat (T, 1); // fit downstream 
  Bool_t ok = (!err) && CheckTrack( T );
  if( ok ){
    T.GetTrackParam( *track->GetParamLast() ); // store fitted track & cov.matrix
    //AZ err = T.Fit( 0 ); // fit upstream
    err = FitWithMat (T, 0); // fit upstream
    ok = ok && (!err) && CheckTrack( T );
    if( ok ) T.GetStsTrack( *track, 1 );          // store fitted track & cov.matrix & chi2 & NDF  
    //cout << " zzz " << 1/track->GetParamLast()->GetQp() << " " << 1/track->GetParamFirst()->GetQp() << " " 
    //	 << 1/track->GetParamFirst()->GetQp()-1/track->GetParamLast()->GetQp() << endl; //AZ
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

// -------------------------------------------------------------------------

Int_t BmnStsKFTrackFitter::FitWithMat (CbmKFTrack& track, Int_t downstream)
{
  // Track fitting procedure with material budget

  CbmKF *KF = CbmKF::Instance();
  Double_t *T = track.GetTrack();
  Double_t *C = track.GetCovMatrix();
  Int_t NHits = track.GetNOfHits();

  //if(line) KF->SetMethod(0);

  Bool_t err = 0;
  if ( NHits==0 ) return 1;

  // use initial momentum
  // this fixed value will be used during fit instead of T[4]

  Double_t qp0 = T[4];

  const Double_t INF = 10000.;

  track.GetRefChi2() = 0;
  track.GetRefNDF() = 0;

  // initialize covariance matrix

  C[ 0] = INF;
  C[ 1] = 0.; C[ 2] = INF;
  C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
  C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
  C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;

  CbmKFMaterial *mat = nullptr;
  CbmKFMaterial matEval;

  try {

    if ( downstream ) {
      CbmKFHit *h = track.GetHit( 0 );
      //AZ err = h->Filter( *this, downstream, qp0 );
      err = h->Filter( track, downstream, qp0 );
      Int_t istold = h->MaterialIndex;
      for( Int_t i=1; i<NHits; i++ ){
	h = track.GetHit( i );
	Int_t ist = h->MaterialIndex;
	//cout << " xxxxxxxxxxxxxxxxxxx " << istold << " " << ist << endl;
	//AZ for (Int_t j = istold+1; j < ist; j++) {
	for (Int_t j = istold + 0; j < ist; j++) { //AZ
	  //AZ err = err || KF->vMaterial[j]->Pass( *this, downstream, qp0 );
	  //err = err || KF->vMaterial[j]->Pass( track, downstream, qp0 ); //AZ
	  if (fMatHistos.size()) {
	    EvalMaterial (track, i, downstream, matEval); //AZ
	    mat = &matEval;
	  } else mat = KF->vMaterial[j];
	  //cout << " Before: " << qp0 << endl;
	  err = err || mat->Pass( track, downstream, qp0 ); //AZ
	  //cout << " After: " << qp0 << endl;
	}	
	//AZ err = err || h->Filter( *this, downstream, qp0 );
	err = err || h->Filter( track, downstream, qp0 );
	istold = ist;
      }
    } else {
      CbmKFHit *h = track.GetHit( NHits-1 );
      err = h->Filter( track, downstream, qp0 );
      Int_t istold = h->MaterialIndex;
      for( Int_t i=NHits-2; i>=0; i-- ){
	h = track.GetHit( i );
	Int_t ist = h->MaterialIndex;
	//cout << " yyyyyyyyyyyyyyyyyyy " << istold << " " << ist << endl;
	//AZ for(Int_t j=istold-1; j>ist; j--) {
	for(Int_t j = istold - 0; j > ist; j--) { //AZ
	  //AZ err = err || KF->vMaterial[j]->Pass( track, downstream, qp0 );
	  if (fMatHistos.size()) {
	    EvalMaterial (track, i, downstream, matEval); //AZ
	    mat = &matEval;
	  } else mat = KF->vMaterial[j];
	  err = err || mat->Pass( track, downstream, qp0 ); //AZ
	}
	err = err || h->Filter( track, downstream, qp0 );
	istold = ist;
      }
    }
    
    // correct NDF value to number of fitted track parameters (straight line(4) or helix(5) )

    track.GetRefNDF() -= (KF->GetMethod()==0) ?4 :5;
  }
  catch(...) {
    track.GetRefChi2() = 0;
    track.GetRefNDF() = 0;
    C[ 0] = INF;
    C[ 1] = 0.; C[ 2] = INF;
    C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
    C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
    C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;
    T[0] = T[1] = T[2] = T[3] = T[4] = T[5] = 0;
    return 1;
  }
  if ( err ) {
    track.GetRefChi2() = 0;
    track.GetRefNDF() = 0;
    C[ 0] = INF;
    C[ 1] = 0.; C[ 2] = INF;
    C[ 3] = 0.; C[ 4] = 0.; C[ 5] = INF;
    C[ 6] = 0.; C[ 7] = 0.; C[ 8] = 0.; C[ 9] = INF;
    C[10] = 0.; C[11] = 0.; C[12] = 0.; C[13] =  0.; C[14] = INF;
    T[0] = T[1] = T[2] = T[3] = T[4] = T[5] = 0;
  }
  return err;
}

// -------------------------------------------------------------------------

void BmnStsKFTrackFitter::EvalMaterial (CbmKFTrack& track, int ihit, Int_t downstream, CbmKFMaterial &mat)
{
  //AZ Evaluate material between hits (stations)

  if ((ihit == 0 && downstream) || (ihit == track.GetNOfHits()-1 && !downstream)) { 
    cout << "!!! BmnStsKFTrackFitter::EvalMaterial - this should not happen " << ihit << " " << downstream 
	 << " " << track.GetNOfHits() << endl; 
    exit(0); 
  }
    
  //CbmKFHit *h = track.GetHit (ihit);
  //CbmKFHit *hprev = track.GetHit (ihit-1);
  BmnKFStsHit *h = (BmnKFStsHit*) track.GetHit (ihit);
  int iprev = (downstream) ? ihit-1 : ihit+1;
  BmnKFStsHit *hprev = (BmnKFStsHit*) track.GetHit (iprev);
  if (!downstream) {
    BmnKFStsHit *htmp = h;
    h = hprev;
    hprev = htmp;
  }
  Double_t zOffset = (h->FitPoint[0].sigma2 < 1.e-4) ? 0.5 : 1.5; // !!! Si or GEM - Attention for sigma2 (based on strip pitch)
  //TProfile2D *histo = nullptr;
  TProfile2D *histo = fMatHistos.lower_bound(h->GetZ()-zOffset)->second;
  TAxis *axis[2] = {histo->GetXaxis(), histo->GetYaxis()};
  int ixy[2] = {axis[0]->FindBin(h->GetX()), axis[1]->FindBin(h->GetY())};
  int ixyprev[2] = {axis[0]->FindBin(hprev->GetX()), axis[1]->FindBin(hprev->GetY())};
  Double_t thick = histo->GetBinContent(ixyprev[0],ixyprev[1]);
  int nbins = 1;
  if ((ixy[0] != ixyprev[0]) || (ixy[1] != ixyprev[1])) {
    int idxy[2], ilong = 0, ishor = 1;
    Double_t dxy[2];
    for (int j = 0; j < 2; ++j) idxy[j] = ixy[j] - ixyprev[j];
    for (int j = 0; j < 2; ++j) dxy[j] = idxy[j] * axis[j]->GetBinWidth(1);
    if (TMath::Abs(idxy[0]) < TMath::Abs(idxy[1])) { ilong = 1; ishor = 0; }
    Double_t slope = dxy[ishor] / dxy[ilong], xyshort = axis[ishor]->GetBinCenter(ixyprev[ishor]);
    int istep = TMath::Sign (1, idxy[ilong]), i = ixyprev[ilong];
    while (1) {
      i += istep;
      ++nbins;
      xyshort += slope;
      int j = axis[ishor]->FindBin (xyshort);
      if (ilong == 0) thick += histo->GetBinContent(i,j);
      else thick += histo->GetBinContent(j,i);
      //if (ilong == 0) thick = TMath::Max (histo->GetBinContent(i,j), thick); // max. value
      //else thick = TMath::Max (histo->GetBinContent(j,i), thick); // max. value
      if (i == ixy[ilong]) break;
    }
  }
  thick /= (nbins*100); // conversion from %
  //thick *= 10; //!!! - test
  mat.ID = 77777;
  mat.ZReference = (h->GetZ() + hprev->GetZ()) / 2;
  //mat.ZReference = (downstream) ? hprev->GetZ() + 0.1 : h->GetZ() - 0.1;
  //mat.ZThickness = TMath::Abs (h->GetZ() - hprev->GetZ());
  mat.ZThickness = TMath::Abs (h->GetZ() - hprev->GetZ()) - h->tube->dz;
  mat.RadLength = mat.ZThickness / thick;
  //mat.Fe = 0.10; // energy loss correction factor - what does it mean?
  //cout << nbins << " " << thick << " " << mat.RadLength << " " << mat.ZThickness << " " << ixy[0] << " " << ixyprev[0] 
  //    << " " << ixy[1] << " " << ixyprev[1] << " " << h->GetZ() << " " << hprev->GetZ() << " " << histo->GetTitle() << endl;
  
}

// -------------------------------------------------------------------------

//AZ void CbmStsKFTrackFitter::Extrapolate( FairTrackParam* track, Double_t z, FairTrackParam* e_track )
void BmnStsKFTrackFitter::Extrapolate( FairTrackParam* track, Double_t z, FairTrackParam* e_track )
{
  if( !track ) return;
  CbmKFTrack T;  
  T.SetTrackParam( *track );
  T.Extrapolate( z );
  if( e_track ) T.GetTrackParam( *e_track );
}


// -------------------------------------------------------------------------

//AZ void CbmStsKFTrackFitter::Extrapolate( CbmStsTrack* track, Double_t z, FairTrackParam* e_track )
void BmnStsKFTrackFitter::Extrapolate( CbmStsTrack* track, Double_t z, FairTrackParam* e_track )
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

// -------------------------------------------------------------------------

//AZ Double_t  CbmStsKFTrackFitter::GetChiToVertex( CbmStsTrack* track,  CbmVertex *vtx )
Double_t  BmnStsKFTrackFitter::GetChiToVertex( CbmStsTrack* track,  CbmVertex *vtx )
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

// -------------------------------------------------------------------------

//AZ Double_t CbmStsKFTrackFitter::FitToVertex( CbmStsTrack* track, CbmVertex *vtx, FairTrackParam *v_track )
Double_t BmnStsKFTrackFitter::FitToVertex( CbmStsTrack* track, CbmVertex *vtx, FairTrackParam *v_track )
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

// -------------------------------------------------------------------------

//AZ Bool_t CbmStsKFTrackFitter::CheckTrack( CbmKFTrack &T )
Bool_t BmnStsKFTrackFitter::CheckTrack( CbmKFTrack &T )
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
