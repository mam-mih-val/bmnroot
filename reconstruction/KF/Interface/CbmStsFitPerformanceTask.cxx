// -------------------------------------------------------------------------
// -----                 CbmStsFitPerformanceTask source file             -----
// -----                  Created 02/02/05  by E. Kryshen              -----
// -------------------------------------------------------------------------

#include "CbmStsFitPerformanceTask.h"

#include "CbmStsKFTrackFitter.h"
#include "CbmPVFinderKF.h"
#include "CbmStsKFSecondaryVertexFinder.h"
#include "CbmKF.h"
#include "CbmKFMath.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"
#include "CbmKFPrimaryVertexFinder.h"
#include "CbmKFPrimaryVertexFinder.h"
#include "CbmKF.h"
#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"
#include "CbmKFTrack.h"
#include "CbmKFVertex.h"

#include "CbmMCTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsHit.h"
#include "FairRootManager.h"
#include "FairMCApplication.h"
#include "CbmStsTrack.h"
#include "CbmVertex.h"
#include "CbmMvdHit.h"

#include "TVector3.h"
#include "TF1.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"


#include <iostream>
#include <iomanip>
#include <cmath>

using std::cout;
using std::endl;
using std::vector;
using std::fabs;

// -------------------------------------------------------------------------

void writedir2current( TObject *obj ){
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

void CbmStsFitPerformanceTask::CreateTrackHisto(TH1D* hist[10], const char* name, const char* title ){
  struct {
    const char *name;
    const char *title;
    Int_t n;
    Double_t l,r;
  } Table[10]=
    {
      {"x",  "Residual X [#mum]",                   100, -100., 100.},
      {"y",  "Residual Y [#mum]",                   100, -100., 100.},
      {"tx", "Residual Tx [mrad]",                  100,   -2.,   2.},
      {"ty", "Residual Ty [mrad]",                  100,   -2.,   2.},
      {"P",  "Resolution P/Q [100%]",               100,   -.1,  .1 },
      {"px", "Pull X [residual/estimated_error]",   100,  -10.,  10.},
      {"py", "Pull Y [residual/estimated_error]",   100,  -10.,  10.},
      {"ptx","Pull Tx [residual/estimated_error]",  100,  -10.,  10.},
      {"pty","Pull Ty [residual/estimated_error]",  100,  -10.,  10.},
      {"pQP","Pull Q/P [residual/estimated_error]", 100,  -10.,  10.}
    };

  for( int i=0; i<10; i++ ){
    char n[225], t[255];
    sprintf(n,"%s_%s",name,Table[i].name);
    sprintf(t,"%s %s",title,Table[i].title);
    hist[i] = new TH1D(n,t, Table[i].n, Table[i].l, Table[i].r);
  }
}

void CbmStsFitPerformanceTask::CreateVertexHisto(TH1D* hist[9], const char* name, const char* title ){
  struct {
    const char *name;
    const char *title;
    Int_t n;
    Double_t l,r;
  } Table[9]=
    {
      {"x",  "Residual X [#mum]",                   100, -5., 5.},
      {"y",  "Residual Y [#mum]",                   100, -5., 5.},
      {"z",  "Residual Z [#mum]",                   100, -40., 40.},
      {"px", "Pull X [residual/estimated_error]",   100,  -10.,  10.},
      {"py", "Pull Y [residual/estimated_error]",   100,  -10.,  10.},
      {"pz", "Pull Z [residual/estimated_error]",   100,  -10.,  10.},
      {"chi2","Chi2/NDF",                           100,  0.,  10.},
      {"prob","Prob(Chi2,NDF)",                     100,  0.,  1.},
      {"ntracks","N tracks",                        100,  0.,  1000.},
    };

  for( int i=0; i<9; i++ ){
    char n[225], t[255];
    sprintf(n,"%s_%s",name,Table[i].name);
    sprintf(t,"%s %s",title,Table[i].title);
    hist[i] = new TH1D(n,t, Table[i].n, Table[i].l, Table[i].r);
  }
}

void CbmStsFitPerformanceTask::CreateD0Histo(TH1D* hist[15], const char* name, const char* title ){
  struct {
    const char *name;
    const char *title;
    Int_t n;
    Double_t l,r;
  } Table[14]=
    {
      {"x",  "Residual X [#mum]",                   100, -100., 100.},
      {"y",  "Residual Y [#mum]",                   100, -100., 100.},
      {"z",  "Residual Z [#mum]",                   100, -500., 500.},
      {"px", "Pull X [residual/estimated_error]",   100,  -10.,  10.},
      {"py", "Pull Y [residual/estimated_error]",   100,  -10.,  10.},
      {"pz", "Pull Z [residual/estimated_error]",   100,  -10.,  10.},
      {"chi2","Chi2/NDF",                           100,  0.,  10.},
      {"prob","Prob(Chi2,NDF)",                     100,  0.,  1.},
      {"mass","Residual Mass",                      100,  -.1,  .1},
      {"pmass", "Pull Mass [residual/estimated_error]",   100,  -10.,  10.},
      {"KaonP", "Kaon P resolution",                100,  -.05,  .05},
      {"PionP", "Pion P resolution",                100,  -.05,  .05},
      {"KaonP0", "Kaon P resolution before fit",    100,  -.05,  .05},
      {"PionP0", "Pion P resolutionbefore fit",     100,  -.05,  .05}
    };

  for( int i=0; i<14; i++ ){
    char n[225], t[255];
    sprintf(n,"%s_%s",name,Table[i].name);
    sprintf(t,"%s %s",title,Table[i].title);
    hist[i] = new TH1D(n,t, Table[i].n, Table[i].l, Table[i].r);
  }
}


// -----   Standard constructor   ------------------------------------------
CbmStsFitPerformanceTask::CbmStsFitPerformanceTask(const char* name, Int_t iVerbose )
                 :FairTask(name,iVerbose),
                  fEvent(0),
                  fVertexAnalysis(0), fD0Analysis(0), fTrackAnalysis(1),

                  fMCTrackArray(0),      ///< MCTracks
                  fStsPointArray(0),     ///< StsPoints
                  fMvdPointArray(0),     ///< StsPoints
                  fRecStsTrackArray(0),  ///< Reconstructed StsTracks
                  fStsHitArray(0),          ///< Sts hits
                  fMvdHitArray(0),          ///< Sts hits
                  fPrimaryVertex(0),     ///< Primary vertex
                  fSTSTrackMatch(0),  ///< Related MC tracks

                  fhChi2(0),      // x=chi2(), y=entries for all
                  fhProb(0),      // x=Prob function(), y=entries for all
                  fhDP(0),
                  fhDP2(0),
                  fhDsP(0),
                  fhDsP2(0),

                  fhZMCf(0), fhZMCl(0),  // z first/last of MC track
                  fhZRecof(0), fhZRecol(0),  // z first/last of Reco track
  
                  fhRes_vs_Mom_f(0), fhRes_vs_Mom_l(0),      // resolution vs momentum
                  fhExtraTracks2ndMVD(0), // extra tracks not detected in the second mvd chamber

                    // TH1D* fhFrst[10](), 
                    // TH1D* fhMid[10](),
                    // TH1D* fhLast[10](),
                    // TH1D* fhImp[10](),
                    // TH1D* fhImpPi[10](),
                    // TH1D* fhVfit[10](),
                    // TH1D* fhVtx[9](),
                    // TH1D* fhV[13][9](),
                  fhPq(),
                    // fhD0[4][14](),

                    // fhHitDensity[10](),
                    // fhTrackDensity[8](),
                  fhTrackDensity0L(0),

                  histodir(0),
                  fFitter()
{
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsFitPerformanceTask::~CbmStsFitPerformanceTask(){
}

// -----   Init CbmStsFitPerformanceTask task   -------------------------------
InitStatus CbmStsFitPerformanceTask::Init(){
  fEvent = 0;

  TDirectory *curdir = gDirectory;
  histodir = gDirectory->mkdir("StsFitPerformance");
  histodir->cd();

  gDirectory->mkdir("TrackFit");
  gDirectory->cd("TrackFit");
  {
    fhChi2 = new TH1D("hChi2","hChi2",100,0,10);
    fhProb = new TH1D("hProb","hProb",100,0,1.0);

    fhDP = new TH1D("hDP","hDP",1000,-.005,.005);
    fhDP2 = new TH2D("hDP2","hDP2",100,0.,5.0,1000,-.005,.005);

    fhDsP = new TH1D("hDsP","hDsP",100,-1,1);
    fhDsP2 = new TH2D("hDsP2","hDsP2",100,0.,5.0,100,-1,1);

    fhZMCf = new TH1D("hZMCf","h Z MC first",102,-1.0,101.0);
    fhZRecof = new TH1D("hZRecof","h Z Reco first",102,-1.0,101.0);
    fhZMCl = new TH1D("hZMCl","h Z MC last",102,-1.0,101.0);
    fhZRecol = new TH1D("hZRecol","h Z Reco last",102,-1.0,101.0);

    fhRes_vs_Mom_f = new TH2D("hRes_vs_Mom_f","TX Resolusion vs Momentum (first hit)",100, 0.0, 5.0, 100, -50.0, 50.0);
    fhRes_vs_Mom_l = new TH2D("hRes_vs_Mom_l","TX Resolusion vs Momentum (last hit)",100, 0.0, 5.0, 100, -10.0, 10.0);

    fhExtraTracks2ndMVD = new TH2D("hExtraTracks2ndMVD","ExtraTracks in the 2nd MVD",200, -10.0, 10.0, 200, -10.0, 10.0);

    gDirectory->mkdir("AtFirstHit");
    gDirectory->cd("AtFirstHit");
    CreateTrackHisto( fhFrst, "fst", "at first STS Hit" );
    gDirectory->cd("..");
    gDirectory->mkdir("AtLastHit");
    gDirectory->cd("AtLastHit");
    CreateTrackHisto( fhLast, "lst", "at last  STS Hit" );
    gDirectory->cd("..");
    gDirectory->mkdir("AtImpactPoint");
    gDirectory->cd("AtImpactPoint");
    CreateTrackHisto( fhImp,  "imp", "at impact point" );
    gDirectory->cd("..");
    gDirectory->mkdir("AtImpactPointPion");
    gDirectory->cd("AtImpactPointPion");
    CreateTrackHisto( fhImpPi,  "impPi", "at impact point" );
    gDirectory->cd("..");
    gDirectory->mkdir("InTheMiddle");
    gDirectory->cd("InTheMiddle");
    CreateTrackHisto( fhMid,  "mid", "at 4th station" );
    gDirectory->cd("..");
    gDirectory->mkdir("FittedToVertex");
    gDirectory->cd("FittedToVertex");
    CreateTrackHisto( fhVfit, "vfit", "for vertex fitted track" );
    gDirectory->cd("..");

    gDirectory->mkdir("PSlidesOnP");
    gDirectory->cd("PSlidesOnP");
    fhPq = new TH2D("Pq","Resolution P/Q at impact point vs P", 100, 0, 10, 100,  -.1 ,  .1 );
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

  for( int i=0; i<10; i++ ){
    char n[225], t[255];
    sprintf(n,"hHitDens%i",i);
    sprintf(t,"Hit Density, Sts station %i",i);
    fhHitDensity[i] = new TH1D(n,t,300,0,300);
  }
  fhTrackDensity[0] = new TH1D("hTrackDensity0","Track Density 0cm",300,0,300);
  fhTrackDensity0L = new TH1D("hTrackDensity0L","Track Density Line 0cm",300,0,300);
  fhTrackDensity[1] = new TH1D("hTrackDensity1","Track Density 1cm",300,0,300);
  fhTrackDensity[2] = new TH1D("hTrackDensity2","Track Density 2cm",300,0,300);
  fhTrackDensity[3] = new TH1D("hTrackDensity3","Track Density 3cm",300,0,300);
  fhTrackDensity[4] = new TH1D("hTrackDensity4","Track Density 4cm",300,0,300);
  fhTrackDensity[5] = new TH1D("hTrackDensity5","Track Density 5cm",300,0,300);
  fhTrackDensity[6] = new TH1D("hTrackDensity10","Track Density 10cm",300,0,300);
  fhTrackDensity[7] = new TH1D("hTrackDensity100","Track Density 1m",300,0,300);

  gDirectory->mkdir("VertexFit");
  gDirectory->cd("VertexFit");
  {
    CreateVertexHisto( fhVtx, "vtx", "for Primary Vertex" );
    gDirectory->mkdir("VertexOnNTracks");
    gDirectory->cd("VertexOnNTracks");
    for( int i=0; i<13; i++){
      char name[225], namedir[225], title[225];
      if( i==0 ){
	sprintf(namedir,"AllTracks");
	sprintf(name,"Vall");
	sprintf(title,"for Primary Vertex on all tracks");
      }else{
	sprintf(namedir,"%iTracks",i*50);
	sprintf(name,"V%i",i*50);
	sprintf(title,"for Primary Vertex on %i tracks",i*50);
      }
      gDirectory->mkdir(namedir);
      gDirectory->cd(namedir);
      CreateVertexHisto( fhV[i], name, title );
      gDirectory->cd("..");
    }
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

  gDirectory->mkdir("D0Fit");
  gDirectory->cd("D0Fit");
  {
    gDirectory->mkdir("No constraints");
    gDirectory->cd("No constraints");
    CreateD0Histo( fhD0[0], "D0", "for D0 Sec. Vertex" );
    gDirectory->cd("..");
    gDirectory->mkdir("Topological constraint");
    gDirectory->cd("Topological constraint");
    CreateD0Histo( fhD0[1], "D0T", "for D0 Topo Sec. Vertex" );
    gDirectory->cd("..");
    gDirectory->mkdir("Mass constraint");
    gDirectory->cd("Mass constraint");
    CreateD0Histo( fhD0[2], "D0M", "for D0 Mass Sec. Vertex" );
    gDirectory->cd("..");
    gDirectory->mkdir("Mass+Topological constraint");
    gDirectory->cd("Mass+Topological constraint");
    CreateD0Histo( fhD0[3], "D0TM", "for D0 Topo+Mass Sec. Vertex" );
    gDirectory->cd("..");
  }
  gDirectory->cd("..");

  curdir->cd();
  return ReInit();
}

// -----   ReInit CbmStsFitPerformanceTask task   -------------------------------
InitStatus CbmStsFitPerformanceTask::ReInit(){

  FairRootManager* fManger =FairRootManager::Instance();
  fMCTrackArray     = reinterpret_cast<TClonesArray*>(fManger->GetObject("MCTrack"));
  fStsPointArray    = reinterpret_cast<TClonesArray*>(fManger->GetObject("StsPoint"));
  fMvdPointArray    = reinterpret_cast<TClonesArray*>(fManger->GetObject("MvdPoint"));
  fRecStsTrackArray = reinterpret_cast<TClonesArray*>(fManger->GetObject("StsTrack"));
  fStsHitArray = reinterpret_cast<TClonesArray*>(fManger->GetObject("StsHit"));
  fMvdHitArray = reinterpret_cast<TClonesArray*>(fManger->GetObject("MvdHit"));
  fPrimaryVertex = reinterpret_cast<CbmVertex*>(fManger->GetObject("PrimaryVertex"));
  fSTSTrackMatch = reinterpret_cast<TClonesArray*>(fManger->GetObject("StsTrackMatch"));
  fFitter.Init();

  return kSUCCESS;
}
// -------------------------------------------------------------------------



// -----   Exec CbmStsFitPerformanceTask task   -------------------------------
void CbmStsFitPerformanceTask::Exec(Option_t * option){
  cout << "Event: " << ++fEvent << " ";

  Int_t nTracks=fRecStsTrackArray->GetEntriesFast();
  cout << "  nTracks: " << nTracks << endl;

  if( !fSTSTrackMatch ) return;

  Double_t mc[6];

  Int_t Quality[nTracks];

  for (Int_t iTrack=0; iTrack<nTracks; iTrack++ ){
    Quality[iTrack]=0;
    CbmStsTrack*   track= (CbmStsTrack*) fRecStsTrackArray->At(iTrack);
    if( !track ) continue;
    CbmTrackMatch *m = (CbmTrackMatch*) fSTSTrackMatch->At(iTrack);
    if( !m ) continue;
    if( m->GetNofTrueHits() <
	0.7*(m->GetNofTrueHits()+m->GetNofWrongHits()+m->GetNofFakeHits())
	) continue;
    Int_t mcTrackID = m->GetMCTrackId();
    if (mcTrackID<0)  continue;
    if (!IsLong(track)) continue;

    CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTrackArray->At(mcTrackID);
    if( !mcTrack ) continue;
    if (fabs((mcTrack->GetStartZ()))>1.) continue;

    if( mcTrack->GetMotherId() !=-1 ) continue;
    Quality[iTrack]=1;
  }

  if(0){
//    CbmKF &KF = *CbmKF::Instance();
    int nStsHits = fStsHitArray->GetEntries();
    int nMvdHits = fMvdHitArray->GetEntries();
    cout<<"Mvd hit density..."<<endl;
    for( int ih=0; ih<nMvdHits; ih++){
      if( ih%100 ==0 ) cout<<ih<<endl;
      CbmMvdHit* h1 = (CbmMvdHit*) fMvdHitArray->At(ih);
      if( !h1 ) continue;
      double V[3] = { 2*h1->GetDx()*h1->GetDx(), 2*0, 2*h1->GetDy()*h1->GetDy()};
      Double_t v = 1./(V[0]*V[2] - V[1]*V[1]) ;
      V[0]*=v;
      V[1]*=v;
      V[2]*=v;
      double D2 = 1.e10;
      for( int jh=0; jh<nMvdHits; jh++){
	if( ih==jh ) continue;
	CbmMvdHit* h2 = (CbmMvdHit*) fMvdHitArray->At(jh);
	if( !h2 ) continue;
	if( h1->GetStationNr() != h2->GetStationNr() ) continue;
	Double_t dx = h1->GetX() - h2->GetX();
	Double_t dy = h1->GetY() - h2->GetY();
	Double_t d2 = fabs(dx*dx*V[0]-2*dx*dy*V[1]+dy*dy*V[2]);
	if( d2<D2 ) D2 = d2;
      }
      fhHitDensity[h1->GetStationNr()-1]->Fill(sqrt(D2/2.));
    }
    cout<<"Sts hit density..."<<endl;
    for( int ih=0; ih<nStsHits; ih++){
      if( ih%1000 ==0 ) cout<<ih<<endl;
      CbmStsHit* h1 = (CbmStsHit*) fStsHitArray->At(ih);
      if( !h1 ) continue;
      double V[3] = { 2*h1->GetDx()*h1->GetDx(), 2*h1->GetCovXY(), 2*h1->GetDy()*h1->GetDy()};
      Double_t v = 1./(V[0]*V[2] - V[1]*V[1]) ;
      V[0]*=v;
      V[1]*=v;
      V[2]*=v;
      double D2 = 1.e10;
      for( int jh=0; jh<nStsHits; jh++){
	if( ih==jh ) continue;
	CbmStsHit* h2 = (CbmStsHit*) fStsHitArray->At(jh);
	if( !h2 ) continue;
	if( h1->GetStationNr() != h2->GetStationNr() ) continue;
	if( h1->GetDigi(1)>=0 ){
	  if( h1->GetDigi(0)!=h2->GetDigi(0) &&
	      h1->GetDigi(1)!=h2->GetDigi(1)     ) continue;
	}
	Double_t dx = h1->GetX() - h2->GetX();
	Double_t dy = h1->GetY() - h2->GetY();
	Double_t d2 = fabs(dx*dx*V[0]-2*dx*dy*V[1]+dy*dy*V[2]);
	if( d2<D2 ) D2 = d2;
      }
      fhHitDensity[CbmKF::Instance()->MvdStationIDMap.size()+h1->GetStationNr()-1]->Fill(sqrt(D2/2));
    }
  }

  if(0){ // track density
    cout<<"Track density..."<<endl;
    bool flag[nTracks];
    double sC[nTracks][8][15];
    double sT[nTracks][8][5];
    for (Int_t iTrack=0;iTrack<nTracks;iTrack++){
      flag[iTrack] = 0;

      CbmStsTrack*   trackI= (CbmStsTrack*) fRecStsTrackArray->At(iTrack);
      if( !IsLong(trackI) ) continue;
      flag[iTrack] = 1;
      double z[8] = {0.2,1,2,3,4,5,10,100};
      for( int iz=0; iz<8; iz++ ){
	FairTrackParam paramI;
	fFitter.Extrapolate(trackI, z[iz], &paramI);
	CbmKFMath::CopyTrackParam2TC( &paramI, sT[iTrack][iz], sC[iTrack][iz] );
	if( !finite(sC[iTrack][iz][0]) || sC[iTrack][iz][0]<0 || sC[iTrack][iz][0]>10. ){
	  flag[iTrack] = 0;
	  break;
	}
      }
    }
    for (Int_t iTrack=0;iTrack<nTracks;iTrack++){

      if( iTrack%10==0 ) cout<<iTrack<<endl;
      if (!flag[iTrack]) continue;
      double Chi2[8];
      for( int iz=0; iz<8; iz++ ) Chi2[iz] = 1.e10;
      double Chi2L=1.e10;

      for (Int_t jTrack=0;jTrack<nTracks;jTrack++){
	if( jTrack==iTrack ) continue;
	if (!flag[jTrack]) continue;

	for( int iz=0; iz<8; iz++ ){
	  Double_t C[15], d[5];
	  for( int i=0; i<15; i++ ) C[i] = sC[iTrack][iz][i] + sC[jTrack][iz][i];
	  for( int i=0; i<5; i++ ) d[i] = sT[iTrack][iz][i] - sT[jTrack][iz][i];
	  CbmKFMath::invS(C,5);

	  double chi2=0;
	  for( int i=0; i<5; i++ ){
	    double dCi=0;
	    for( int j=0; j<5; j++ ) dCi+=d[j]*C[CbmKFMath::indexS(i,j)];
	    chi2+=dCi*d[i];
	  }
	  chi2 = fabs(chi2);
	  if( chi2<Chi2[iz] ) Chi2[iz] = chi2;

	  if( iz==0 ){
	    chi2=0;
	    for( int i=0; i<4; i++ ){
	      double dCi=0;
	      for( int j=0; j<4; j++ ) dCi+=d[j]*C[CbmKFMath::indexS(i,j)];
	      chi2+=dCi*d[i];
	    }
	    chi2 = fabs(chi2);
	    if( chi2<Chi2L ) Chi2L = chi2;
	  }
	}
      }
      for( int iz=0; iz<8; iz++ ){
	fhTrackDensity[iz]->Fill(sqrt(Chi2[iz]/5));
      }
      fhTrackDensity0L->Fill(sqrt(Chi2L/4));
    }
  }
  if( fTrackAnalysis ){
    for (Int_t iTrack=0;iTrack<nTracks;iTrack++){
     if( Quality[iTrack]<1 ) continue;
      CbmStsTrack*   track= (CbmStsTrack*) fRecStsTrackArray->At(iTrack);
      CbmTrackMatch *m = (CbmTrackMatch*) fSTSTrackMatch->At(iTrack);
      CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTrackArray->At(m->GetMCTrackId());

      if( mcTrack->GetMotherId() !=-1 ) continue; // not primary track //IK

      // Get MC points;
      vector<CbmStsPoint*> vPoints;
      for( Int_t i=0; i<track->GetNStsHits(); i++ ){
	Int_t hitID = track->GetStsHitIndex(i);
	if( hitID<0 ) continue;
	CbmStsHit* hit = (CbmStsHit*) fStsHitArray->At(hitID);
	if( !hit ) continue;
	Int_t pointID = hit->GetRefIndex();
	if( pointID<0 ) continue;
	CbmStsPoint *point = (CbmStsPoint*) fStsPointArray->At(pointID);
	if( !point ) continue;
	vPoints.push_back(point);
      }
      vector<CbmMvdPoint*> vMPoints;
      for( Int_t i=0; i<track->GetNMvdHits(); i++ ){
	Int_t hitID = track->GetMvdHitIndex(i);
	if( hitID<0 ) continue;
	CbmMvdHit* hit = (CbmMvdHit*) fMvdHitArray->At(hitID);
	if( !hit ) continue;
	Int_t pointID = hit->GetRefIndex();
	if( pointID<0 ) continue;
	CbmMvdPoint *point = (CbmMvdPoint*) fMvdPointArray->At(pointID);
	if( !point ) continue;
	vMPoints.push_back(point);
      }
      // impact mc track
      Double_t mci[6];
      Double_t pzi = mcTrack->GetPz();
      if( fabs( pzi )>1.e-4 ) pzi = 1./pzi;
      mci[0] = mcTrack->GetStartX();
      mci[1] = mcTrack->GetStartY();
      mci[2] = mcTrack->GetPx() * pzi;
      mci[3] = mcTrack->GetPy() * pzi;
      mci[4] = (fabs(mcTrack->GetP())>1.e-4 ) ? GetCharge(mcTrack) / mcTrack->GetP() :0;
      mci[5] = mcTrack->GetStartZ();
      if( !vPoints.empty() )
	{
	  if( track->GetNStsHits()+track->GetNMvdHits()>=8 ){
	    Double_t p1 = mcTrack->GetP();
	    TVector3 mom;
	    vPoints.back()->MomentumOut(mom);
	    Double_t p2 = mom.Mag();
	    Double_t s = p1*p1*TMath::Sqrt(TMath::Abs(track->GetParamFirst()->GetCovariance(4,4)));
	    Double_t dp = (p1-p2)*TMath::Sqrt(1+mci[2]*mci[2]+mci[3]*mci[3]);
	    fhDP->Fill(dp);
	    fhDP2->Fill(p1,dp );
	    fhDsP->Fill( dp/s );
	    fhDsP2->Fill(p1, dp/s );
	  }
	  // first point
	  if( !vMPoints.empty() )
	    FillMCStateVectors(vMPoints.front(),mc);
	  else FillMCStateVectors(vPoints.front(),mc);
	  FillTrackHisto( mc, track, fhFrst );
	  fhZMCf->Fill(mc[5]);
	  fhZRecof->Fill(track->GetParamFirst()->GetZ());

	  // last point
	  FillMCStateVectors(vPoints.back(),mc,1);
	  FillTrackHisto( mc, track, fhLast );
	  fhZMCl->Fill(mc[5]);
	  fhZRecol->Fill(track->GetParamLast()->GetZ());

	  // impact point

	  if(TMath::Abs(mcTrack->GetPdgCode())==211) FillTrackHisto(mci,track,fhImpPi);
	  else
	    FillTrackHisto(mci,track,fhImp);

	  // 4th station (check smoother)

	  for( vector<CbmStsPoint*>::iterator i=vPoints.begin(); i!=vPoints.end(); i++){
	    int id = (*i)->GetDetectorID();
	    CbmKF &KF = *CbmKF::Instance();
	    if( KF.StsStationIDMap.find(id) == KF.StsStationIDMap.end() ) continue;
	    if( KF.StsStationIDMap[id] != 3 ) continue;
	    FillMCStateVectors(*i,mc,1);
	    //FillTrackHisto( mc, track, fhMid );
	    break;
	  }
	}

      if( fPrimaryVertex && mcTrack->GetMotherId() ==-1 ){

	// fit track to Primary Vertex

	CbmStsTrack tt(*track);
	fFitter.FitToVertex( &tt, fPrimaryVertex, tt.GetParamFirst() );
	//FillTrackHisto(mci,&tt,fhVfit);

	// fill momentum resolution

	double z = mci[5];
	FairTrackParam param;
	fFitter.Extrapolate(track, z, &param);
	if( fabs(mci[4])>1.e-4 && fabs( param.GetQp() )>1.e-4 )
	  fhPq->Fill( fabs(1./mci[4]), (mci[4]/param.GetQp() - 1.) );
      }

      if( track->GetNDF()>0 ){
	fhChi2->Fill(track->GetChi2()/track->GetNDF());
	fhProb->Fill(TMath::Prob(track->GetChi2(),track->GetNDF()));
      }
    }
  }

  if( fPrimaryVertex ){

    // find MC Primary vertex

    TVector3 MC_V(0,0,0);
    Bool_t Is_MC_V = 0;
    {
      TVector3 MC_Vcurr(0,0,0);
      Int_t nvtracks=0, nvtrackscurr=0;
      Int_t nMCTracks = fMCTrackArray->GetEntriesFast();
      for (Int_t iTrack=0;iTrack<nMCTracks;iTrack++){
	CbmMCTrack*  mcTrack= (CbmMCTrack*)  fMCTrackArray->At(iTrack);
	if( mcTrack->GetMotherId() !=-1 ) continue; // not primary track
	double z = mcTrack->GetStartZ();
	if ( !Is_MC_V || fabs(z-MC_Vcurr.Z())>1.e-7 ){// new vertex
	  Is_MC_V = 1;
	  if( nvtrackscurr > nvtracks ){
	    MC_V = MC_Vcurr;
	    nvtracks = nvtrackscurr;
	  }
	  mcTrack->GetStartVertex(MC_Vcurr);
	  nvtrackscurr = 1;
	}else nvtrackscurr++;
      }
      if( nvtrackscurr > nvtracks ) MC_V = MC_Vcurr;
    }


    if( Is_MC_V ){

      // primary vertex fit performance

      FillVertexHisto( MC_V, fPrimaryVertex, fhVtx);

      if( fVertexAnalysis ){

	CbmPVFinderKF Finder;
	TClonesArray TracksToFit("CbmStsTrack");
	Int_t N=0;
	for (Int_t iTrack=0;iTrack<nTracks;iTrack++){
	  if( Quality[iTrack]<1 ) continue;
	  new( TracksToFit[N] ) CbmStsTrack();
	  *(CbmStsTrack*)TracksToFit.At(N) = *(CbmStsTrack*)fRecStsTrackArray->At(iTrack);
	  N++;
	  if( N%50!=0 ) continue;
	  Int_t i = N/50;
	  if( i>=13 ) continue;
	  CbmVertex V;
	  Finder.FindPrimaryVertex( &TracksToFit, &V );
	  FillVertexHisto( MC_V, &V, fhV[i] );
	}
	CbmVertex V;
	Finder.FindPrimaryVertex( &TracksToFit, &V );
	FillVertexHisto( MC_V, &V, fhV[0] );
      }

      // D0 fit performance (if there are)

      if( fD0Analysis&&fabs(MC_V.Z())<1.e-5 ){
	// search for  Kaon
	for (Int_t iK=0;iK<nTracks;iK++){
	  if( Quality[iK]<1 ) continue;
	  CbmStsTrack*   tK= (CbmStsTrack*) fRecStsTrackArray->At(iK);
	  CbmTrackMatch *m = (CbmTrackMatch*) fSTSTrackMatch->At(iK);
	  CbmMCTrack*  mcK = (CbmMCTrack*)  fMCTrackArray->At(m->GetMCTrackId());
	  if( mcK->GetMotherId() !=-1 ) continue; // not primary or D0 track
	  if( abs(mcK->GetPdgCode())!=321 ) continue;
	  double zK = mcK->GetPz();
	  if( zK-MC_V.Z()<1.e-5 ) continue; // primary
	  double MCPK = mcK->GetP();
	  fFitter.DoFit( tK, 321 ); // refit
	  // search for Pion
	  for (Int_t iP=0;iP<nTracks;iP++){
	    if( Quality[iP]<1 ) continue;
	    CbmStsTrack*   tP= (CbmStsTrack*) fRecStsTrackArray->At(iP);
	    m = (CbmTrackMatch*) fSTSTrackMatch->At(iP);
	    CbmMCTrack*  mcP = (CbmMCTrack*)  fMCTrackArray->At(m->GetMCTrackId());
	    if( mcP->GetMotherId() !=-1 ) continue; // not primary or D0 track
	    if( abs(mcP->GetPdgCode())!=211 ) continue;
	    if( mcK->GetPdgCode()*mcP->GetPdgCode() >=0 ) continue; //same charge
	    double zP = mcP->GetStartZ();
	    if( fabs(zP-zK)>1.e-8 ) continue; // different vertex
	    double MCPP = mcP->GetP();

	    const double D0 = 1.8645 ;

	    TVector3 mc_;
	    mcK->GetStartVertex(mc_);

	    CbmStsKFSecondaryVertexFinder SVFinder;
	    SVFinder.AddTrack(tK);
	    SVFinder.AddTrack(tP);

	    for( int iconst=0; iconst<4; iconst++){

	      switch( iconst ){
	      case 0:
		SVFinder.SetMassConstraint(); // no constraints
		SVFinder.SetTopoConstraint();
		break;
	      case 1:
		SVFinder.SetMassConstraint();// Topo constraint
		SVFinder.SetTopoConstraint(fPrimaryVertex);
		break;
	      case 2:
		SVFinder.SetMassConstraint(D0);// Mass constraint
		SVFinder.SetTopoConstraint();
		break;
	      case 3:
		SVFinder.SetMassConstraint(D0);// Topo + Mass constraint
		SVFinder.SetTopoConstraint(fPrimaryVertex);
		break;
	      default:
		break;
	      }
	      SVFinder.Fit();
	      CbmVertex sv;
	      //FairTrackParam KFitted, PFitted;
	      Double_t mass, mass_err;
	      SVFinder.GetVertex(sv);
	      //SVFinder.GetFittedTrack(0, &KFitted );
	      //SVFinder.GetFittedTrack(1, &PFitted );
	      SVFinder.GetMass(&mass, &mass_err);
	      if( sv.GetNDF()<=0) continue;
	      Double_t dx = sv.GetX() - mc_.X();
	      Double_t dy = sv.GetY() - mc_.Y();
	      Double_t dz = sv.GetZ() - mc_.Z();
	      Double_t sx = sv.GetCovariance(0,0);
	      Double_t sy = sv.GetCovariance(1,1);
	      Double_t sz = sv.GetCovariance(2,2);
	      fhD0[iconst][0]->Fill( 1.E4*dx );
	      fhD0[iconst][1]->Fill( 1.E4*dy );
	      fhD0[iconst][2]->Fill( 1.E4*dz );
	      if ( sx >1.e-10 ) fhD0[iconst][3]->Fill( dx/sqrt(sx) );
	      if ( sy >1.e-10 ) fhD0[iconst][4]->Fill( dy/sqrt(sy) );
	      if ( sz >1.e-10 ) fhD0[iconst][5]->Fill( dz/sqrt(sz) );
	      if( sv.GetNDF()>0 ){
		fhD0[iconst][6]->Fill( sv.GetChi2()/sv.GetNDF() );
		fhD0[iconst][7]->Fill( TMath::Prob(sv.GetChi2(),sv.GetNDF()) );
	      }
	      fhD0[iconst][8]->Fill( (mass-D0) );
	      if( mass_err>1.e-10 ) fhD0[iconst][9]->Fill( (mass-D0)/mass_err );
	      //fhD0[iconst][10]->Fill( ( fabs(1./KFitted.GetQp()) - MCPK)/MCPK );
	      //fhD0[iconst][11]->Fill( (fabs(1./PFitted.GetQp()) - MCPP)/MCPP );
	      if( fabs(tK->GetParamFirst()->GetQp())>1.e-4 && MCPK>1.e-4)
		fhD0[iconst][12]->Fill( ( fabs(1./tK->GetParamFirst()->GetQp()) - MCPK)/MCPK );
	      if( fabs(tP->GetParamFirst()->GetQp())>1.e-4 && MCPP>1.e-4)
		fhD0[iconst][13]->Fill( ( fabs(1./tP->GetParamFirst()->GetQp()) - MCPP)/MCPP );
	    }
	  }
	}
      }
    }
  }// vertex analysis
}
// -------------------------------------------------------------------------



// -----   Finish CbmStsFitPerformanceTask task   -----------------------------
void CbmStsFitPerformanceTask::Finish(){
  writedir2current(histodir);
}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------
void CbmStsFitPerformanceTask::FillTrackHisto(const Double_t mc[6], CbmStsTrack *track, TH1D* hist[10]  )
{
  if ((mc[5] > 31.0) ||(mc[5] < 29.0)){
    //cout << " Z out " << track->GetParamFirst()->GetZ() << endl;
    //return;
  }
  //cout << "*Z in  " << track->GetParamFirst()->GetZ() << endl;
  double z = mc[5];
  FairTrackParam param;
  //cout<<1<<endl;
  fFitter.Extrapolate(track, z, &param);
  //cout<<2<<endl;

  double t[6],c[15];
  CbmKFMath::CopyTrackParam2TC( &param, t, c );
  Bool_t ok = 1;
  for( int i=0; i<6; i++ ) ok = ok && finite(t[i]);
  for( int i=0; i<15; i++ ) ok = ok && finite(c[i]);
  if( !ok ) return;
  //fhExtraTracks2ndMVD->Fill(t[0], t[1]);

  hist[0]->Fill(1.E4*(t[0] - mc[0]));
  hist[1]->Fill(1.E4*(t[1] - mc[1]));
  hist[2]->Fill( 1.E3*(t[2] - mc[2]) );
  hist[3]->Fill( 1.E3*(t[3] - mc[3]) );
  if ( fabs( t[4] )>1.e-10 ) hist[4]->Fill( (mc[4]/t[4] - 1.) );
  //if (z < 7.0) // first hit
  //if ( fabs( t[4] )>1.e-10 ) fhRes_vs_Mom_f->Fill(fabs(1.0/t[4]),1.E3*(t[2] - mc[2]));
  //if (z > 80.0) // last hit
  //if ( fabs( t[4] )>1.e-10 ) fhRes_vs_Mom_l->Fill(fabs(1.0/t[4]),1.E3*(t[2] - mc[2]));

  if ( c[ 0] >1.e-10 ) hist[5]->Fill( ( t[0] - mc[0] )/sqrt(c[ 0]) );
  if ( c[ 2] >1.e-10 ) hist[6]->Fill( ( t[1] - mc[1] )/sqrt(c[ 2]) );
  if ( c[ 5] >1.e-10 ) hist[7]->Fill( ( t[2] - mc[2] )/sqrt(c[ 5]) );
  if ( c[ 9] >1.e-10 ) hist[8]->Fill( ( t[3] - mc[3] )/sqrt(c[ 9]) );
  if ( c[14] >1.e-10 ) hist[9]->Fill( ( t[4] - mc[4] )/sqrt(c[14]) );
}
// -------------------------------------------------------------------------

void CbmStsFitPerformanceTask::FillVertexHisto( TVector3 &mc, CbmVertex *V, TH1D* hist[8]  )
{
  Double_t dx = V->GetX() - mc.X() ;
  Double_t dy = V->GetY() - mc.Y() ;
  Double_t dz = V->GetZ() - mc.Z() ;
  Double_t s2x = V->GetCovariance(0,0);
  Double_t s2y = V->GetCovariance(1,1);
  Double_t s2z = V->GetCovariance(2,2);

  hist[0]->Fill(1.E4*dx);
  hist[1]->Fill(1.E4*dy);
  hist[2]->Fill(1.E4*dz);
  if ( s2x >1.e-10 ) hist[3]->Fill( dx/sqrt(s2x) );
  if ( s2y >1.e-10 ) hist[4]->Fill( dy/sqrt(s2y) );
  if ( s2z >1.e-10 ) hist[5]->Fill( dz/sqrt(s2z) );
  hist[6]->Fill( V->GetChi2()/V->GetNDF() );
  hist[7]->Fill( TMath::Prob(V->GetChi2(),V->GetNDF()));
  hist[8]->Fill( V->GetNTracks() );
}

// -------------------------------------------------------------------------
void CbmStsFitPerformanceTask::FillMCStateVectors( CbmStsPoint* point, Double_t mc[], Bool_t out){
  if( !point ) return;
  Int_t mcTrackID = point->GetTrackID();
  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(mcTrackID);
  if( !mcTrack ) return;
  Double_t q=GetCharge(mcTrack);

  // Get MC state vector
  TVector3 r,p;
  if( !out ){
    point->Momentum(p);
    point->Position(r);
  }else{
    point->MomentumOut(p);
    point->PositionOut(r);
  }
  double pzi = p.z();
  if( fabs(pzi)>1.e-4 ) pzi = 1./pzi;
  mc[2]=p.x()*pzi;
  mc[3]=p.y()*pzi;
  mc[4]=p.Mag()>1.e-4 ? q/p.Mag() :0;
  mc[0]=r.x();
  mc[1]=r.y();
  mc[5]=r.z();
}

void CbmStsFitPerformanceTask::FillMCStateVectors( CbmMvdPoint* point, Double_t mc[], Bool_t out){
  if( !point ) return;
  Int_t mcTrackID = point->GetTrackID();
  CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTrackArray->At(mcTrackID);
  if( !mcTrack ) return;
  Double_t q=GetCharge(mcTrack);

  // Get MC state vector
  TVector3 r,p;
  if( !out ){
    point->Momentum(p);
    point->Position(r);
  }else{
    point->MomentumOut(p);
    point->PositionOut(r);
  }
  double pzi = p.z();
  if( fabs(pzi)>1.e-4 ) pzi = 1./pzi;
  mc[2]=p.x()*pzi;
  mc[3]=p.y()*pzi;
  mc[4]=p.Mag()>1.e-4 ? q/p.Mag() :0;
  mc[0]=r.x();
  mc[1]=r.y();
  mc[5]=r.z();
}


// -----   GetCharge   ----------------------------------------------------
Double_t CbmStsFitPerformanceTask::GetCharge(CbmMCTrack* mcTrack){
  Double_t q;
  Int_t pdgCode = mcTrack->GetPdgCode();
  TParticlePDG* particlePDG = TDatabasePDG::Instance()->GetParticle(pdgCode);
  if (particlePDG) q = particlePDG->Charge()/3.; else q = 0;
  return q;
}


// -------------------------------------------------------------------------
Bool_t CbmStsFitPerformanceTask::IsLong(CbmStsTrack* track){
  Int_t nHits = track->GetNStsHits();
  if (nHits <4) return 0;
  Int_t stmin=1000, stmax=-1000;
  Int_t st,iHit,hitID;
  for (iHit=0;iHit<nHits; iHit++){
    hitID=track->GetStsHitIndex(iHit);
    st=((CbmStsHit*) fStsHitArray->At(hitID))->GetDetectorID();
    if (st<stmin) stmin=st;
    if (st>stmax) stmax=st;
  }
  if (stmax-stmin+1<4) return 0;
  return 1;
}

ClassImp(CbmStsFitPerformanceTask)
