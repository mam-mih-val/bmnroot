// -------------------------------------------------------------------------
// -----                  CbmL1SttTrackFinder source file              -----
// -----                 Created 8/03/08  by A. Zinchenko              -----
// -------------------------------------------------------------------------

#include "CbmL1SttTrackFinder.h"

#include "CbmL1SttHit.h"
#include "CbmL1SttTrack.h"

#include "CbmSttHit.h"
#include "CbmSttPoint.h"
#include "CbmSttTrack.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "FairRootManager.h"
#include "CbmKFHit.h"
#include "CbmKFPixelMeasurement.h"
#include "CbmKFMaterial.h"
#include "CbmKFTrackInterface.h"
#include "CbmMuchHit.h"
#include "CbmMuchTrack.h"
#include "CbmMuchPoint.h"
#include "CbmStsTrack.h"
#include "CbmMCTrack.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmVertex.h"
#include "CbmStsTrackMatch.h"

#include "TFile.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
using std::fabs;

ClassImp(CbmL1SttTrackFinder);

CbmL1SttTrackFinder::CbmL1SttTrackFinder(const char *name, Int_t iVerbose ):FairTask(name, iVerbose)
{
  fTrackCollection = new TClonesArray("CbmSttTrack", 100);
  histodir = 0;
}


CbmL1SttTrackFinder::~CbmL1SttTrackFinder()
{
}

InitStatus CbmL1SttTrackFinder::Init()
{
  return ReInit();
}

InitStatus CbmL1SttTrackFinder::ReInit()
{
  fSttPoints=(TClonesArray *) FairRootManager::Instance()->GetObject("SttPoint");
  fSttHits =(TClonesArray *) FairRootManager::Instance()->GetObject("SttHit");
  fMuchTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("MuchTrack");
  fStsTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("StsTrack");
  fMCTracks =(TClonesArray *) FairRootManager::Instance()->GetObject("MCTrack");
  fSTSTrackMatch = (TClonesArray*) FairRootManager::Instance()->GetObject("StsTrackMatch");
  fPrimVtx =  (CbmVertex *) FairRootManager::Instance() ->GetObject("PrimaryVertex");
  fStsFitter.Init();

  FairRootManager::Instance()->Register("SttTrack", "Stt", fTrackCollection, kTRUE);

  cout << " **************************************************" << endl;
  if (fMuchTracks) cout << " *** Using Much tracks for Stt tracking *** " << endl;
  else  cout << " *** Using Sts tracks for Stt tracking *** " << endl;
  cout << " **************************************************" << endl;

  return kSUCCESS;
}

void CbmL1SttTrackFinder::SetParContainers()
{
}

void CbmL1SttTrackFinder::Finish()
{
  Write();
}

void CbmL1SttTrackFinder::Exec(Option_t * option)
{
  const int MaxBranches = 50;

  static bool first = 1; 
  fTrackCollection->Clear();
  static int EventCounter = 0;    
  EventCounter++;
  cout<<" SttRec event "<<EventCounter<<endl;

  //int MuNStations = CbmKF::Instance()->MuchStation2MCIDMap.size();
  static const Int_t nStations = CbmKF::Instance()->SttStationIDMap.size();
  //const int nStations = 18; // to be taken elsewhere !!!
  
  if ( first ){
    first = 0;
    TDirectory *curdir = gDirectory;
    histodir = gDirectory->mkdir("SttRec");
    histodir->cd();
    fhNBranches = new TH1F("NBranches","N Branches",MaxBranches,0,MaxBranches);
    curdir->cd();
  }
  
  int NHits = fSttHits->GetEntriesFast();
  vector<CbmL1SttHit> vSttHits;

  for( int ih = 0; ih < NHits; ++ih ){
    CbmSttHit *h = (CbmSttHit*) fSttHits->UncheckedAt(ih);
    CbmL1SttHit m( h, ih );
    vSttHits.push_back(m);
  }

  vector<CbmL1SttTrack> vTracks;
  
  Int_t nStsTracks;
  TClonesArray *seedTracks;
  if (fMuchTracks) seedTracks = fMuchTracks;
  else seedTracks = fStsTracks;
  nStsTracks = seedTracks->GetEntriesFast();
  cout << " Seed tracks: " << nStsTracks << endl;
 
  CbmL1SttTrack Branches[MaxBranches];
  Double_t scatAng[MaxBranches] = {0.};

  for( int itr = 0; itr < nStsTracks; ++itr ){
    
    Int_t nOK = 0;
    TObject *sts = seedTracks->UncheckedAt(itr);
    if (!fMuchTracks) {
      if ( ((CbmStsTrack*)sts)->GetNStsHits()+((CbmStsTrack*)sts)->GetNMvdHits()<4 ) continue;
    }

    // MC
    /*
    if( 0&&fSTSTrackMatch && fMCTracks ){
      CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(itr);
      if( !m ) continue;
      Int_t mcTrackID = m->GetMCTrackId();
      if (mcTrackID<0)  continue;  
      CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
      if( !mcTrack ) continue;
      if( abs(mcTrack->GetPdgCode())!=13 ) continue;
    }
    */
    // Check if track passes all the planes
    if (1&&fSTSTrackMatch && fMCTracks ){
      Int_t itr1 = itr;
      if (fMuchTracks) itr1 = ((CbmMuchTrack*)sts)->GetStsTrackID();
      CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(itr1);
      if( !m ) continue;
      Int_t mcTrackID = m->GetMCTrackId();
      //if (mcTrackID<0)  continue;  
      //CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
      //if( !mcTrack ) continue;
      //if( abs(mcTrack->GetPdgCode())!=13 ) continue;
      Int_t hitPlanes[20];
      TVector3 mom0(-1e+7), mom1;
      for (Int_t i = 0; i < nStations; ++i) hitPlanes[i] = -1;
      for( int ih = 0; ih < NHits; ++ih ){
	CbmL1SttHit &h = vSttHits[ih];
	CbmSttHit *hit = (CbmSttHit*) fSttHits->UncheckedAt(h.index);
	CbmSttPoint *p = (CbmSttPoint*) fSttPoints->UncheckedAt(hit->GetRefIndex());
	if (p->GetTrackID() != mcTrackID) continue;
	if (p->GetStationNo() == 1 && 
	    TMath::Sqrt(p->GetX()*p->GetX()+p->GetY()*p->GetY()) > 220) continue;
	if (hitPlanes[h.iStation] < 0) {
	  hitPlanes[h.iStation] = 1;
	  ++nOK;
	}
	if (mom0[0] < -1e+5) p->Momentum(mom0);
	else p->Momentum(mom1);
	if (itr < MaxBranches) scatAng[itr] = TMath::Max (scatAng[itr], mom1.Angle(mom0) * TMath::RadToDeg());
      }
      if (nOK < nStations) {
	//cout << " Track " << mcTrackID << " has " << nOK << " points " << endl;
	//continue;
      }
    }

    if (!fMuchTracks) fStsFitter.DoFit( (CbmStsTrack*)sts, 13 ); // refit with muon mass

    int NBranches = 1;

    fMuchTracks == 0x0 ? Branches[0].SetStsTrack((CbmStsTrack*)sts) : 
                         Branches[0].SetMuchTrack((CbmMuchTrack*)sts);
    Branches[0].StsID = itr;
    Branches[0].NHits = 0;
    Branches[0].NMissed = 0;
    Branches[0].NMissedStations = 0;
    Branches[0].ok = 1;
    Branches[0].stopped = 0;
    Branches[0].vHits.clear();
    //cout<<"Sts track N "<<itr<<" with initial mom="<<1./Branches[0].T[4]<<endl;
    for( Int_t ist=0; ist<nStations; ++ist ){
      
      int NBranchesOld = NBranches;

      for( Int_t ibr=0; ibr<NBranchesOld; ++ibr ){
	CbmL1SttTrack &tr = Branches[ibr];
	if( tr.stopped ) continue;
	//if( ist%3 ==0 ) cout<<" | ";
	//double Zdet = CbmKF::Instance()->vMuchDetectors[ist].ZReference;
	double Zdet = CbmKF::Instance()->vSttMaterial[ist].ZReference;
	//cout << Zdet << endl;
	//double Zdet = zPlanes[ist];
	tr.Extrapolate(Zdet);
	if( fabs(tr.T[4])>100.) tr.stopped = 1;
	if( 1.<0.5 *fabs(tr.T[4]) ) tr.stopped = 1; // 0.5 GeV, stop transport
	//if( tr.stopped ) cout<<"Sts track N "<<itr<<" stopped at Mu station "<<ist
	//<<" with mom="<<1./tr.T[4]<<endl;
	if( tr.stopped ) continue;
	/*
	if( CbmKF::Instance()->vMuchDetectors[ist].IsOutside( tr.T[0], tr.T[1] ) ){
	  //cout<<" out ";
	  tr.NMissedStations++;
	  continue;
	}
	*/

	vector<int> NewHits;
	Int_t firstHit = -1;
	Double_t uTr = 0.;
	for( int ih=0; ih<NHits; ++ih ){
	  CbmL1SttHit &h = vSttHits[ih];
	  if( h.iStation != ist ) continue;

	  if (firstHit < 0) {
	    // Track coordinate transformation
	    uTr = tr.T[0] * h.FitPoint.phi_c + tr.T[1] * h.FitPoint.phi_s;
	    //uTr = tr.T[0] * h.FitPoint.phi_c - tr.T[1] * h.FitPoint.phi_s;
	    firstHit = 1;
	  }
	  CbmSttHit *hit = (CbmSttHit*) fSttHits->UncheckedAt(h.index);
	  //cout << tr.T[0] << " " <<  tr.T[1] << " " << hit->GetX() << " " << hit->GetY() << " " << uTr << " " << h.FitPoint.u << " " << h.FitPoint.phi_c << " " << h.FitPoint.phi_s << endl;

	  /*
	  if(0){ // !!! Cut for the time of flight (ns)
	    double hl = sqrt(h.FitPoint.x*h.FitPoint.x+h.FitPoint.y*h.FitPoint.y+h.FitPoint.z*h.FitPoint.z);
	    double hp = 1./fabs(tr.T[4]);
	    double texp = hl*sqrt(1. - 0.1057*0.1057/(hp*hp))/29.9792458;  //ns
	    if( h.time - texp > 1.0 ) continue;
	  }
	  */
	  /*
	  double dx = tr.T[0] - h.FitPoint.x;
	  double dy = tr.T[1] - h.FitPoint.y;
	  double c0 = tr.C[0] + h.FitPoint.V[0];
	  double c1 = tr.C[1] + h.FitPoint.V[1];
	  double c2 = tr.C[2] + h.FitPoint.V[2];
	  double chi2 = 0.5*(dx*dx*c0-2*dx*dy*c1+dy*dy*c2)/(c0*c2-c1*c1);
	  */
	  Double_t du = uTr - h.FitPoint.u;
	  //Double_t c0 = tr.C[0] + h.FitPoint.sigma2;
	  //Double_t chi2 = du * du / c0; // w/out correlations at the moment !!!
	  Double_t w = h.FitPoint.sigma2 + h.FitPoint.phi_cc*tr.C[0] + 
	    h.FitPoint.phi_2sc*tr.C[1] + h.FitPoint.phi_ss*tr.C[2];
	  Double_t chi21 = du * du / w;
	  //cout << " chi2 " << ist << " " << du << " " << chi21 << " " << chi21 << endl;
	  if ( chi21 <= 20. ) NewHits.push_back( ih );
	  //if ( chi21 <= 100. ) NewHits.push_back( ih );
	}
	int nnew = NewHits.size();
	for( int ih=1; ih<nnew; ++ih){
	  if( NBranches >= MaxBranches ) break;
	  CbmL1SttTrack &t = Branches[NBranches++];
	  t = tr;
	  CbmL1SttHit &h = vSttHits[NewHits[ih]];
	  t.vHits.push_back(&h);
	  t.NHits++;
	  double qp0 = t.T[4];
	  h.Filter(t, 1, qp0);	
	}
	if( nnew >0 ){
	  CbmL1SttHit &h = vSttHits[NewHits[0]];
	  tr.vHits.push_back(&h);
	  tr.NHits++;
	  double qp0 = tr.T[4];
	  h.Filter(tr, 1, qp0);
	}else tr.NMissed++;
      } // for( int ibr=0; ibr<NBranchesOld;
    } // for( int ist=0; ist<nStations;
    int bestbr=0;
    for( int ibr=1; ibr<NBranches; ++ibr ){
      if( (Branches[ibr].NHits  > Branches[bestbr].NHits) ||
	  (Branches[ibr].NHits == Branches[bestbr].NHits)&&
	  (Branches[ibr].chi2<Branches[bestbr].chi2)
	  ) bestbr = ibr;
    }
    vTracks.push_back(Branches[bestbr]);
    // MC
    /*
    if( fSTSTrackMatch && fMCTracks ){
      CbmStsTrackMatch *m = (CbmStsTrackMatch*) fSTSTrackMatch->At(itr);
      if( !m ) continue;
      Int_t mcTrackID = m->GetMCTrackId();
      if (mcTrackID<0)  continue;
      CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTracks->At(mcTrackID);
      if( !mcTrack ) continue;
      if( abs(mcTrack->GetPdgCode())==13 ) fhNBranches->Fill(NBranches);
    }
    */
    if (nOK == nStations) fhNBranches->Fill(NBranches);
    else (vTracks.back()).ok = kFALSE;
    //cout << itr << " " << nOK << " " << (vTracks.back()).ok << endl;
  } // for( int itr=0; itr<nStsTracks;

  int NTracks = vTracks.size();
  cout<<"NTracks="<<NTracks<<endl;
  //sort tracks and check for common muon hits

  vector<CbmL1SttTrack*> vpTracks;
  for( int i=0; i<NTracks; ++i ) vpTracks.push_back(&(vTracks[i]));
  sort( vpTracks.begin(), vpTracks.end(), CbmL1SttTrack::Compare );
  
  int NOutTracks = fTrackCollection->GetEntries();

  for( int it=0; it<NTracks; ++it ){
    CbmL1SttTrack &tr = *vpTracks[it];
    if( tr.NDF<=0 || tr.chi2 > 10.*tr.NDF ){ 
      //tr.ok = 0; 
      //continue; 
    }
    int nbusy = 0;
    for( int ih=0; ih<tr.NHits; ++ih ) if( tr.vHits[ih]->busy ) nbusy++;      
    if( 0 && nbusy>2 ){
      tr.ok = 0;
      continue;
    }
    // Check if track contains hit from all doublets in all stations 
    Int_t nDoubl[20] = {0};
    for (Int_t ih = 0; ih < tr.NHits; ++ih) {
      Int_t i2 = tr.vHits[ih]->iStation / 2;
      if (nDoubl[i2] == 0) nDoubl[i2]++;
    }
    Int_t nHit = 0;
    for (Int_t i = 0; i < nStations/2; ++i) nHit += nDoubl[i];
    if (nHit < nStations/2) continue;
    if (!(tr.ok)) continue;

    { 
      new((*fTrackCollection)[NOutTracks]) CbmSttTrack();
      CbmSttTrack* track = (CbmSttTrack*) fTrackCollection->At(NOutTracks++);
      track->SetChi2(tr.GetRefChi2());
      track->SetNDF(tr.GetRefNDF());
      FairTrackParam tp;
      CbmKFMath::CopyTC2TrackParam( &tp, tr.T, tr.C );
      track->SetSttTrack( &tp );
      track->SetStsTrackID(tr.StsID);
      int nh=0;
      for( vector<CbmL1SttHit* >::iterator i= tr.vHits.begin(); i!=tr.vHits.end(); i++){
	if( ++nh>30 ) break;
	track->AddHitIndex( (*i)->index );
      } 
      track->SetNMissedHits( tr.NMissed );
      track->SetNMissedStations( tr.NMissedStations );
      if (tr.StsID < MaxBranches) track->SetScatAng(scatAng[tr.StsID]);
    }
    for( int ih=0; ih<tr.NHits; ++ih ) tr.vHits[ih]->busy=1;
  }

  if ( EventCounter <  100 && EventCounter % 10  == 0 || 
       EventCounter >= 100 && EventCounter % 100 == 0
       ) Write();
  cout << "end of SttRec " << fTrackCollection->GetEntriesFast() << endl;
}


void CbmL1SttTrackFinder::Write()
{
  TFile HistoFile("SttRec.root","RECREATE");
  writedir2current(histodir);
  HistoFile.Close();
}

void CbmL1SttTrackFinder::writedir2current( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = ((TDirectory*)obj)->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) writedir2current(obj1);
    cur->cd();
  }
}
