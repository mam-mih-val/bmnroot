/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 main program
 *
 *====================================================================
 */
#include "CbmL1.h"

#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"
#include "L1Algo/L1Branch.h"
#include "L1Algo/L1Field.h"

#include "CbmKF.h"
#include "CbmMvdHit.h"
#include "CbmGeoStsPar.h"
#include "CbmStsHit.h"
#include "CbmStsStation.h"
#include "CbmStsSector.h"
#include "CbmStsSensor.h" // for field approx.
#include "CbmStsDigiPar.h"
#include "CbmStsFindTracks.h"

#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"

#include "TVectorD.h"
#include "TMatrixD.h"
#include "TFile.h"
#include "TROOT.h"
#include "TRandom3.h"
#include "TProfile2D.h"

#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

static L1Algo algo_static _fvecalignment;

CbmL1 *CbmL1::fInstance = 0;

CbmL1::CbmL1()
  : algo(0), // for access to L1 Algorithm from L1::Instance
    PF(0),
    vRTracks(), // reconstructed tracks
    vHitStore(),
    NStation(0), NMvdStations(0), NStsStations(0), // number of detector stations (all\sts\mvd)
    fPerformance(0),
    fSTAPDataMode(0),
    fSTAPDataDir(""),

    fTrackingLevel(2),  // really doesn't used
    fMomentumCutOff(0.1),  // really doesn't used
    fGhostSuppression(1),  // really doesn't used
    fUseMVD(0),  // really doesn't used

    StsDigi(),
    PrimVtx(),

    listMCTracks (0),
    listStsPts(0),
    listStsDigi(0),
    listStsClusters(0),
    listStsHits(0),

    listMvdPts(0),
    listMvdHits(0),
    listMvdHitMatches(0),
    vStsHits(),
    vMCPoints(),
    vMCTracks(),
    vHitMCRef(),
    vRParticles(),
    vMCParticles(),
    MCtoRParticleId(),
    RtoMCParticleId(),
    histodir(0),
    fFindParticlesMode(),
    fMatBudgetFileName(""),
    fExtrapolateToTheEndOfSTS(false)
{
    if( !fInstance ) fInstance = this;
    PF = new CbmL1ParticlesFinder();
    StsDigi = CbmStsDigiScheme::Instance(); //AZ
}

CbmL1::CbmL1(const char *name, Int_t iVerbose, Int_t _fPerformance, int fSTAPDataMode_, TString fSTAPDataDir_, int findParticleMode_)
  : FairTask(name,iVerbose),
    algo(0), // for access to L1 Algorithm from L1::Instance
    PF(0),

    vRTracks(), // reconstructed tracks
    vHitStore(),
    NStation(0), NMvdStations(0), NStsStations(0), // number of detector stations (all\sts\mvd)
    fPerformance(_fPerformance),
    fSTAPDataMode(fSTAPDataMode_),
    fSTAPDataDir(fSTAPDataDir_),

    fTrackingLevel(2),  // really doesn't used
    fMomentumCutOff(0.1),  // really doesn't used
    fGhostSuppression(1),  // really doesn't used
    fUseMVD(0),  // really doesn't used

    StsDigi(),
    PrimVtx(),

    listMCTracks (0),
    listStsPts(0),
    listStsDigi(0),
    listStsClusters(0),
    listStsHits(0),

    listMvdPts(0),
    listMvdHits(0),
    listMvdHitMatches(0),
    vStsHits(),
    vMCPoints(),
    vMCTracks(),
    vHitMCRef(),
    vRParticles(),
    vMCParticles(),
    MCtoRParticleId(),
    RtoMCParticleId(),
    histodir(0),
    fFindParticlesMode(findParticleMode_),
    fMatBudgetFileName(""),
    fExtrapolateToTheEndOfSTS(false)
{
    if( !fInstance ) fInstance = this;
    PF = new CbmL1ParticlesFinder();
    StsDigi = CbmStsDigiScheme::Instance(); //AZ
}

CbmL1::~CbmL1()
{
  if( fInstance==this ) fInstance = 0;
}


void CbmL1::SetParContainers()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  rtdb->getContainer("FairGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmStsDigiPar");
}

InitStatus CbmL1::ReInit()
{
  //AZ StsDigi.Clear();
  StsDigi->Clear();
  SetParContainers();
  return Init();
}

InitStatus CbmL1::Init()
{
  if( fVerbose>1 ){
    char y[20] = " [0;33;44m"; // yellow
    char Y[20] = " [1;33;44m"; // yellow bold
    char W[20] = " [1;37;44m"; // white bold
    char o[20] = " [0m\n";       // color off
    Y[0] = y[0] = W[0] = o[0] = 0x1B;    // escape character
    
    cout<<endl<<endl;
    cout<<"  "<<W<<"                                                                 "<<o;
    cout<<"  "<<W<<"  ===////======================================================  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =                   "
        <<Y                      <<"L1 on-line finder"
        <<W                                       <<"                       =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =  "
        <<W     <<"Cellular Automaton 3.1 Vector"
        <<y                                  <<" with "
        <<W                                        <<"KF Quadro"
        <<y                                                 <<" technology"
        <<W                                                            <<"  =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  =  "
        <<y     <<"Designed for CBM collaboration"
        <<W                                   <<"                           =  "<<o;
    cout<<"  "<<W<<"  =  "
        <<y     <<"All rights reserved"
        <<W                        <<"                                      =  "<<o;
    cout<<"  "<<W<<"  =                                                           =  "<<o;
    cout<<"  "<<W<<"  ========================================================////=  "<<o;
    cout<<"  "<<W<<"                                                                 "<<o;
    cout<<endl<<endl;
  }

  FairRootManager *fManger = FairRootManager::Instance();

  FairRunAna *Run = FairRunAna::Instance();
  FairRuntimeDb *RunDB = Run->GetRuntimeDb();
  {
    CbmGeoStsPar* StsPar = L1_DYNAMIC_CAST<CbmGeoStsPar*>( RunDB->findContainer("CbmGeoStsPar") );
    CbmStsDigiPar *digiPar = L1_DYNAMIC_CAST<CbmStsDigiPar*>( RunDB->findContainer("CbmStsDigiPar") );
    //AZ StsDigi.Init(StsPar, digiPar);
    StsDigi->Init(StsPar, digiPar);
  }
  {
    fUseMVD = 1;
    CbmStsFindTracks * FindTask = L1_DYNAMIC_CAST<CbmStsFindTracks*>( Run->GetTask("STSFindTracks") );
    if( FindTask ) fUseMVD = FindTask->MvdUsage();
  }

  histodir = gROOT->mkdir("L1");

  if (fPerformance){
    listMCTracks = L1_DYNAMIC_CAST<TClonesArray*>( fManger->GetObject("MCTrack") );
    listStsPts = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("StsPoint") );
    listStsClusters = L1_DYNAMIC_CAST<TClonesArray*>( fManger->GetObject("StsCluster") );
    listStsDigi = L1_DYNAMIC_CAST<TClonesArray*>( fManger->GetObject("StsDigi") );
  }
  else{
    listMCTracks = 0;
    listStsPts = 0;
    listStsClusters = 0;
    listStsDigi = 0;
  }
  listStsHits = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("StsHit") );

  if (fPerformance){
    if( !fUseMVD ){
      listMvdPts = 0;
      listMvdHitMatches = 0;
    } else {
      listMvdPts = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("MvdPoint") );
      listMvdHitMatches = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("MvdHitMatch") );
    }
  }
  else{
    listMvdPts = 0;
    listMvdHitMatches = 0;
  }
  if( !fUseMVD ){
    listMvdHits = 0;
  } else {
    listMvdHits = L1_DYNAMIC_CAST<TClonesArray*>(  fManger->GetObject("MvdHit") );
  }

  NMvdStations = 0;
  NStsStations = 0;
  NStation = 0;
  algo = & algo_static;

  fscal geo[10000];

  int ind = 0;
  for( int i=0; i<3; i++ ){
    Double_t point[3] = { 0,0,2.5*i};
    Double_t B[3] = {0,0,0};
    if( CbmKF::Instance()->GetMagneticField() ) CbmKF::Instance()->GetMagneticField()->GetFieldValue( point, B );
    geo[ind++] = 2.5*i;
    geo[ind++] = B[0];
    geo[ind++] = B[1];
    geo[ind++] = B[2];
  }
      
  NMvdStations = ( fUseMVD ) ? CbmKF::Instance()->vMvdMaterial.size() : 0;
  //AZ NStsStations = StsDigi.GetNStations();
  NStsStations = StsDigi->GetNStations();
  NStation = NMvdStations + NStsStations;
  geo[ind++] = NStation;
  geo[ind++] = NMvdStations;

  // field
  const int M=5; // polinom order
  const int N=(M+1)*(M+2)/2;

//   { // field at the z=0 plane
//     const double Xmax = 10, Ymax = 10;
//     const double z = 0.;
//     double dx = 1.; // step for the field approximation
//     double dy = 1.;
//     if( dx > Xmax/N/2 ) dx = Xmax/N/4.;
//     if( dy > Ymax/N/2 ) dy = Ymax/N/4.;
// 
//     TMatrixD A(N,N);
//     TVectorD b0(N), b1(N), b2(N);
//     for( int i=0; i<N; i++){
//       for( int j=0; j<N; j++) A(i,j)==0.;
//       b0(i)=b1(i)=b2(i) = 0.;
//     }
//     for( double x=-Xmax; x<=Xmax; x+=dx )
//       for( double y=-Ymax; y<=Ymax; y+=dy )
//     {
//       double r = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
//       if( r>1. ) continue;
//       Double_t w = 1./(r*r+1);
//       Double_t p[3] = { x, y, z};
//       Double_t B[3] = {0.,0.,0.};
//       CbmKF::Instance()->GetMagneticField()->GetFieldValue(p, B);
//       TVectorD m(N);
//       m(0)=1;
//       for( int i=1; i<=M; i++){
//         int k = (i-1)*(i)/2;
//         int l = i*(i+1)/2;
//         for( int j=0; j<i; j++ ) m(l+j) = x*m(k+j);
//         m(l+i) = y*m(k+i-1);
//       }
//         
//       TVectorD mt = m;
//       for( int i=0; i<N; i++){
//         for( int j=0; j<N;j++) A(i,j)+=w*m(i)*m(j);
//         b0(i)+=w*B[0]*m(i);
//         b1(i)+=w*B[1]*m(i);
//         b2(i)+=w*B[2]*m(i);
//       }
//     }
//     double det;
//     A.Invert(&det);
//     TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
//     
//     targetFieldSlice = new L1FieldSlice;
//     for(int i=0; i<N; i++){
//       targetFieldSlice->cx[i] = c0(i);
//       targetFieldSlice->cy[i] = c1(i);
//       targetFieldSlice->cz[i] = c2(i);
//     }
// 
//   } // target field

  
  
  for ( Int_t ist = 0; ist<NStation; ist++ )
  {
    double C[3][N];
    double z = 0;
    double Xmax, Ymax;
    if( ist<NMvdStations ){
      CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
      geo[ind++] = t.z;
      geo[ind++] = t.dz;
      geo[ind++] = t.r;
      geo[ind++] = t.R;
      geo[ind++] = t.RadLength;
      fscal f_phi=0, f_sigma=5.e-4, b_phi=3.14159265358/2., b_sigma=5.e-4;
      f_phi=0; f_sigma=5.e-4; b_phi=3.14159265358/2.; b_sigma=5.e-4;
      geo[ind++] = f_phi;
      geo[ind++] = f_sigma;
      geo[ind++] = b_phi;
      geo[ind++] = b_sigma;
      z = t.z;
      Xmax = Ymax = t.R;
    }else{
      //AZ CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
      CbmStsStation *st = StsDigi->GetStation(ist - NMvdStations);
  
      geo[ind++] = st->GetZ();
      geo[ind++] = st->GetD();
      geo[ind++] = st->GetRmin();
      geo[ind++] = st->GetRmax();
      geo[ind++] = st->GetRadLength();
  
      CbmStsSector* sector = st->GetSector(0);
      fscal f_phi, f_sigma, b_phi, b_sigma; // angle and sigma front/back  side
      f_phi = sector->GetRotation();
      b_phi = sector->GetRotation();
      if( sector->GetType()==1 ){
        b_phi += 3.14159265358/2.;
        b_sigma = sector->GetDy()/sqrt(12.);
        f_sigma = b_sigma; // CHECKME
      }
      else{
        f_phi +=sector->GetStereoF();
        b_phi +=sector->GetStereoB();
        f_sigma = sector->GetDx() / TMath::Sqrt(12);
	// AZ - use "real" errors 
	if (f_sigma < 0.01) f_sigma = 0.01; //Si
	else f_sigma = 0.015; //GEMs
	//AZ 
        b_sigma  = f_sigma;
      }
      //AZ f_sigma *= cos(f_phi);  // TODO: think about this
      //AZ b_sigma *= cos(b_phi);
      geo[ind++] = f_phi;
      geo[ind++] = f_sigma;
      geo[ind++] = b_phi;
      geo[ind++] = b_sigma;
      z = st->GetZ();

      Xmax=-100; Ymax=-100;

      double x,y;
      for(int isec = 0; isec<st->GetNSectors(); isec++)
      {
        CbmStsSector *sect = L1_DYNAMIC_CAST<CbmStsSector*>( st->GetSector(isec) );
        for(int isen = 0; isen < sect->GetNSensors(); isen++)
        {
          x = sect->GetSensor(isen)->GetX0() + sect->GetSensor(isen)->GetLx()/2.;
          y = sect->GetSensor(isen)->GetY0() + sect->GetSensor(isen)->GetLy()/2.;
          if(x>Xmax) Xmax = x;
          if(y>Ymax) Ymax = y;
        }
      }
//      cout << "Station  "<<  ist << ",  Xmax  " << Xmax<<",  Ymax" << Ymax<<endl;
      if (fVerbose) cout << " AZ - station " << ist << " " << f_phi*TMath::RadToDeg() << " " << b_phi*TMath::RadToDeg() << " " << Xmax << " " << Ymax << endl;
    }

    double dx = 1.; // step for the field approximation
    double dy = 1.;

    if( dx > Xmax/N/2 ) dx = Xmax/N/4.;
    if( dy > Ymax/N/2 ) dy = Ymax/N/4.;

    for( int i=0; i<3; i++)
      for( int k=0; k<N; k++) C[i][k] = 0;
    TMatrixD A(N,N);
    TVectorD b0(N), b1(N), b2(N);
    for( int i=0; i<N; i++){
      for( int j=0; j<N; j++) A(i,j) = 0.;
      b0(i)=b1(i)=b2(i) = 0.;
    }
    for( double x=-Xmax; x<=Xmax; x+=dx )
      for( double y=-Ymax; y<=Ymax; y+=dy )
    {
      double r = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
      if( r>1. ) continue;
      Double_t w = 1./(r*r+1);
      Double_t p[3] = { x, y, z};
      Double_t B[3] = {0.,0.,0.};
      CbmKF::Instance()->GetMagneticField()->GetFieldValue(p, B);
      TVectorD m(N);
      m(0)=1;
      for( int i=1; i<=M; i++){
        int k = (i-1)*(i)/2;
        int l = i*(i+1)/2;
        for( int j=0; j<i; j++ ) m(l+j) = x*m(k+j);
        m(l+i) = y*m(k+i-1);
      }
      
      TVectorD mt = m;
      for( int i=0; i<N; i++){
        for( int j=0; j<N;j++) A(i,j)+=w*m(i)*m(j);
        b0(i)+=w*B[0]*m(i);
        b1(i)+=w*B[1]*m(i);
        b2(i)+=w*B[2]*m(i);
      }
    }
    double det;
    A.Invert(&det);
    TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
    for(int i=0; i<N; i++){
      C[0][i] = c0(i);
      C[1][i] = c1(i);
      C[2][i] = c2(i);
    }

    geo[ind++] = N;
    for( int k=0; k<3; k++ ){
      for( int j=0; j<N; j++) geo[ind++] = C[k][j];
    }
  }

  geo[ind++] = fTrackingLevel;
  geo[ind++] = fMomentumCutOff;
  geo[ind++] = fGhostSuppression;

  {
    if(fSTAPDataMode%2 == 1){ // 1,3
      WriteSTAPGeoData( static_cast<void*>(geo), ind);
    };
    if(fSTAPDataMode >= 2){ // 2,3
      int ind2;
      ReadSTAPGeoData( static_cast<void*>(geo), ind2);
      if (ind2 != ind)  cout << "-E- CbmL1: Read geometry from file " << fSTAPDataDir + "geo_algo.txt was NOT successful." << endl;
    };
  }
  
  algo->Init(geo);


  
  algo->fRadThick.resize(algo->NStations);
    // MVD does not use map
  for( int iSta = 0; iSta < algo->NMvdStations;iSta++ ) {
    algo->fRadThick[iSta].SetBins(1, 100); // mvd should be in +-100 cm square
    algo->fRadThick[iSta].table.resize(1);
    algo->fRadThick[iSta].table[0].resize(1);
    algo->fRadThick[iSta].table[0][0] = algo->vStations[iSta].materialInfo.RadThick[0];
  }

    // Read STS Radiation Thickness table
  if (fMatBudgetFileName != "") {
    TFile* oldfile = gFile;
    TFile *rlFile = new TFile(fMatBudgetFileName);

    cout << "STS Material budget file is " << fMatBudgetFileName << "." << endl;
    for( int j = 0, iSta = algo->NMvdStations; iSta < algo->NStations; iSta++, j++ ) {
      TString name = "Radiation Thickness [%]";
      name += ", Station";
      name += j+1; 
    
      TProfile2D* hStaRadLen = (TProfile2D*) rlFile->Get(name);
      if ( !hStaRadLen ) {
        cout << "L1: incorrect " << fMatBudgetFileName << " file. No " << name << endl; exit(1);
      }
      
      const int NBins = hStaRadLen->GetNbinsX(); // should be same in Y
      const float RMax = hStaRadLen->GetXaxis()->GetXmax(); // should be same as min
      algo->fRadThick[iSta].SetBins(NBins,RMax); // TODO
      algo->fRadThick[iSta].table.resize(NBins);
    
      for( int iB = 0; iB < NBins; iB++ ) {
        algo->fRadThick[iSta].table[iB].resize(NBins);
        for( int iB2 = 0; iB2 < NBins; iB2++ ) {
          algo->fRadThick[iSta].table[iB][iB2] = 0.01 * hStaRadLen->GetBinContent(iB,iB2); //0.0034;//0.003209;//
          if ( algo->fRadThick[iSta].table[iB][iB2] < algo->vStations[iSta].materialInfo.RadThick[0] )
            algo->fRadThick[iSta].table[iB][iB2] = algo->vStations[iSta].materialInfo.RadThick[0];
            // cout << iB << " " << iB2 << " " << algo->fRadThick[iSta].table[iB][iB2] << endl; // dbg
        }
      }
    }
    rlFile->Close();
    rlFile->Delete();
    gFile = oldfile;
  }
  else{
    if (fVerbose) cout << "No material budget file is found. Homogenious budget will be used" << endl;
    for( int iSta = algo->NMvdStations; iSta < algo->NStations; iSta++ ) {
      //cout << iSta << endl;
      algo->fRadThick[iSta].SetBins(1, 100); // mvd should be in +-100 cm square
      algo->fRadThick[iSta].table.resize(1);
      algo->fRadThick[iSta].table[0].resize(1);
      algo->fRadThick[iSta].table[0][0] = algo->vStations[iSta].materialInfo.RadThick[0];
    }
  }
  

  
  return kSUCCESS;
}


void CbmL1::Exec(Option_t * option)
{
}

void CbmL1::Reconstruct()
{
  static int nevent=0;
  if( fVerbose>1 ) cout << endl << "CbmL1::Exec event " << ++nevent << " ..." << endl << endl;

    // repack data
  ReadEvent();
  
  { // save data for standalone
    if(fSTAPDataMode%2 == 1){ // 1,3
      WriteSTAPAlgoData();
      WriteSTAPPerfData();
    };
    if(fSTAPDataMode >= 2){  // 2,3
      ReadSTAPAlgoData();
      ReadSTAPPerfData();
    };
  }

  if(0){  // correct hits on MC // dbg 
    TRandom3 random; 
    vector< int > sF, sB, zP; 
    sF.clear(); 
    sB.clear(); 
    zP.clear(); 
    for( unsigned int iH = 0; iH < algo->vStsHits.size(); ++iH ) { 
        // for( unsigned int iH = algo->vStsHits.size() - 1; iH >= 0; --iH ) {  
      if (vStsHits[iH].mcPointIds.size() == 0) continue; 
      L1StsHit &h = algo->vStsHits[iH]; 
      const CbmL1MCPoint &mcp = vMCPoints[vStsHits[iH].mcPointIds[0]]; 
      const int ista = algo->vSFlag[h.f]/4; 
      const L1Station &sta = algo->vStations[ista]; 
      if ( std::find( sF.begin(), sF.end(), h.f ) != sF.end() ) { // separate strips 
        algo->vSFlag.push_back(algo->vSFlag[h.f]); 
        h.f = algo->vStsStrips.size(); 
        algo->vStsStrips.push_back(L1Strip()); 
      } 
      sF.push_back(h.f); 
      if ( std::find( sB.begin(), sB.end(), h.b ) != sB.end() ) { 
        algo->vSFlagB.push_back(algo->vSFlagB[h.b]); 
        h.b = algo->vStsStripsB.size(); 
        algo->vStsStripsB.push_back(L1Strip()); 
      } 
      sB.push_back(h.b); 
      if ( std::find( zP.begin(), zP.end(), h.iz ) != zP.end() ) { // TODO why do we need it??gives prob=0 
        h.iz = algo->vStsZPos.size(); 
        algo->vStsZPos.push_back(0); 
      } 
      zP.push_back(h.iz); 
 	       
      const fscal idet = 1/(sta.xInfo.sin_phi*sta.yInfo.sin_phi - sta.xInfo.cos_phi*sta.yInfo.cos_phi)[0]; 
#if 1 // GAUSS 
      algo->vStsStrips[h.f]  = idet * ( + sta.yInfo.sin_phi[0]*mcp.x - sta.xInfo.cos_phi[0]*mcp.y ) + random.Gaus(0,sqrt(sta.frontInfo.sigma2)[0]); 
      algo->vStsStripsB[h.b] = idet * ( - sta.yInfo.cos_phi[0]*mcp.x + sta.xInfo.sin_phi[0]*mcp.y ) + random.Gaus(0,sqrt(sta.backInfo.sigma2)[0]); 
#else // UNIFORM 
      algo->vStsStrips[h.f]  = idet * ( + sta.yInfo.sin_phi[0]*mcp.x - sta.xInfo.cos_phi[0]*mcp.y ) + random.Uniform(-sqrt(sta.frontInfo.sigma2)[0]*3.5, sqrt(sta.frontInfo.sigma2)[0]*3.5); 
      algo->vStsStripsB[h.b] = idet * ( - sta.yInfo.cos_phi[0]*mcp.x + sta.xInfo.sin_phi[0]*mcp.y ) + random.Uniform(-sqrt(sta.backInfo.sigma2)[0]*3.5, sqrt(sta.backInfo.sigma2)[0]*3.5); 
#endif   
      algo->vStsZPos[h.iz] = mcp.z; 
    } 
  } 

  
      // input performance
  if (fPerformance){
    InputPerformance();
  }
//  FieldApproxCheck();
//  FieldIntegralCheck();

  if( fVerbose>1 ) cout<<"L1 Track finder..."<<endl;
  algo->CATrackFinder();
//  IdealTrackFinder();
  if( fVerbose>1 ) cout<<"L1 Track finder ok"<<endl;
  algo->L1KFTrackFitter( fExtrapolateToTheEndOfSTS );
//  algo->KFTrackFitter_simple();
  if( fVerbose>1 ) cout<<"L1 Track fitter  ok"<<endl;
  
    // save recontstructed tracks
  vRTracks.clear();
  int start_hit = 0;
  for(vector<L1Track>::iterator it = algo->vTracks.begin(); it!=algo->vTracks.end(); it++){
    CbmL1Track t;
    for( int i=0; i<6; i++) t.T[i] = it->TFirst[i];
    for( int i=0; i<15; i++) t.C[i] = it->CFirst[i];
    for( int i=0; i<6; i++) t.TLast[i] = it->TLast[i];
    for( int i=0; i<15; i++) t.CLast[i] = it->CLast[i];
    t.chi2 = it->chi2;
    t.NDF = it->NDF;
    //t.T[4] = it->Momentum;
    for( int i=0; i<it->NHits; i++ ){
      t.StsHits.push_back( algo->vRecoHits[start_hit++]);
    }
    t.mass = 0.1395679; // pion mass
    t.is_electron = 0;

    t.SetId(vRTracks.size());
    vRTracks.push_back(t);
  }

  //Find Primary vertex, Ks, Lambdas,...
//  static CbmL1ParticlesFinder PF;

    // output performance
  if (fPerformance){
    if( fVerbose>1 ) cout<<"Performance..."<<endl;
    TrackMatch();
  }

  if ( fFindParticlesMode ) {
      //Find Primary vertex, Ks, Lambdas,...
    PF->FindParticles(vRTracks);
    vRParticles = PF->GetParticles();
  }
  
  if (fPerformance){
    EfficienciesPerformance();
    HistoPerformance();
    TrackFitPerformance();

    if ( fFindParticlesMode ) {
      GetMCParticles();
      FindReconstructableMCParticles();
      MatchParticles();
      PartEffPerformance();
      PartHistoPerformance();
    }
///    WriteSIMDKFData();
  }
  if( fVerbose>1 ) cout<<"End of L1"<<endl;


  static bool ask = 0;
  char symbol;
  if (ask){
    std::cout << std::endl << "L1 run (any/r/q) > ";
    do{
      std::cin.get(symbol);
      if (symbol == 'r')
        ask = false;
      if ((symbol == 'e')||(symbol == 'q'))
        exit(0);
    } while (symbol != '\n');
  }
}

// -----   Finish CbmStsFitPerformanceTask task   -----------------------------
void CbmL1::Finish(){
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  // Open output file and write histograms
  TFile* outfile = new TFile("L1_histo.root","RECREATE");
  outfile->cd();
  writedir2current(histodir);
  outfile->Close();
  outfile->Delete();
  gFile = currentFile;
  gDirectory = curr;
}


void CbmL1::writedir2current( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = (L1_DYNAMIC_CAST<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) writedir2current(obj1);
    cur->cd();
  }
}

/// -----   Ideal Tracking   -----------------------------

void CbmL1::IdealTrackFinder()
{
  algo->vTracks.clear();
  algo->vRecoHits.clear();

  for( vector<CbmL1MCTrack>::iterator i = vMCTracks.begin(); i != vMCTracks.end(); ++i){
    CbmL1MCTrack &MC = *i;

    if (!MC.IsReconstructable()) continue;
    if (!(MC.ID >= 0)) continue;

    if (MC.StsHits.size() < 4) continue;
    L1Track algoTr;
    algoTr.NHits = 0;
    int lastStation = -1;
    for (unsigned int iH = 0; iH < MC.StsHits.size(); iH++){
      const int hitI = MC.StsHits[iH];
      const CbmL1StsHit& hit = vStsHits[hitI];
      if ( vMCPoints[hit.mcPointIds[0]].iStation <= lastStation ) { // one hit per station
        continue;
      }
      lastStation = vMCPoints[hit.mcPointIds[0]].iStation;
      algo->vRecoHits.push_back(hitI);
      algoTr.NHits++;
    }
    algoTr.Momentum = MC.p;
          
    algo->vTracks.push_back(algoTr);

  }
  
}; // void CbmL1::IdealTrackFinder()


/// -----   STandAlone Package service-functions  -----------------------------

void CbmL1::WriteSTAPGeoData(void* geo_, int size)
{
  fscal *geo = static_cast<fscal*>( geo_ );
    // write geo in file
  TString fgeo_name = fSTAPDataDir + "geo_algo.txt";
  ofstream fgeo(fgeo_name);
  fgeo.setf(ios::scientific,ios::floatfield);
  fgeo.precision(20);
  for (int i = 0; i < size; i++){
    fgeo << geo[i] << endl;
  };
  fgeo.close();
  cout << "-I- CbmL1: Geometry data has been written in " << fgeo_name << endl;
} // void CbmL1::WriteSTAPGeoData(void* geo_, int size)

void CbmL1::WriteSTAPAlgoData()  // must be called after ReadEvent
{
    // write algo data in file
  static int vNEvent = 1;
  fstream fadata;
  
  TString fadata_name = fSTAPDataDir + "data_algo.txt";
//    if ( vNEvent <= maxNEvent ) {
  if ( 1 ) {
 
    if (vNEvent == 1)
      fadata.open(fadata_name,fstream::out);  // begin new file
    else
      fadata.open(fadata_name,fstream::out | fstream::app);
        
    fadata << "Event:" << " ";
    fadata << vNEvent << endl;
      // write vStsStrips
    int n = algo->vStsStrips.size();  // number of elements
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata << algo->vStsStrips[i] << endl;
    };
    if (fVerbose >= 4) cout << "vStsStrips[" << n << "]" << " have been written." << endl;
      // write vStsStripsB
    n = algo->vStsStripsB.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata << algo->vStsStripsB[i] << endl;
    };
    if (fVerbose >= 4) cout << "vStsStripsB[" << n << "]" << " have been written." << endl;
      // write vStsZPos
    n = algo->vStsZPos.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata << algo->vStsZPos[i] << endl;
    };
    if (fVerbose >= 4) cout << "vStsZPos[" << n << "]" << " have been written." << endl;
      // write vSFlag
    n = algo->vSFlag.size();
    fadata << n << endl;
    unsigned char element;
    for (int i = 0; i < n; i++){
      element = algo->vSFlag[i];
      fadata << static_cast<int>(element) << endl;
    };
    if (fVerbose >= 4) cout << "vSFlag[" << n << "]" << " have been written." << endl;
      // write vSFlagB
    n = algo->vSFlagB.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      element = algo->vSFlagB[i];
      fadata << static_cast<int>(element) << endl;
    };
    if (fVerbose >= 4) cout << "vSFlagB[" << n << "]" << " have been written." << endl;
      // write vStsHits
    n = algo->vStsHits.size();
    fadata << n << endl;
    for (int i = 0; i < n; i++){
      fadata  << static_cast<int>(algo->vStsHits[i].f) << " ";
      fadata  << static_cast<int>(algo->vStsHits[i].b) << " ";
      fadata  << static_cast<int>(algo->vStsHits[i].iz) << endl;
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been written." << endl;
      // write StsHitsStartIndex and StsHitsStopIndex
    n = 20;
    for (int i = 0; i < n; i++){
      if (algo->MaxNStations+1 > i) fadata  << algo->StsHitsStartIndex[i] << endl;
      else fadata  << 0 << endl;
    };
    for (int i = 0; i < n; i++){
      if (algo->MaxNStations+1 > i) fadata  << algo->StsHitsStopIndex[i] << endl;
      else fadata  << 0 << endl;
    };

    
    fadata.close();
  }
  cout << "-I- CbmL1: CATrackFinder data for event number " << vNEvent << " have been written in file " << fadata_name << endl;
  vNEvent++;
} // void CbmL1::WriteSTAPAlgoData() 

void CbmL1::WriteSTAPPerfData()  // must be called after ReadEvent
{
  fstream fpdata;
  fpdata << setprecision(8);
  
  static int vNEvent = 1;

  TString fpdata_name = fSTAPDataDir + "data_perfo.txt";
        // write data for performance in file
  //   if ( vNEvent <= maxNEvent )  {
  if ( 1 ) {

    if (vNEvent == 1)
      fpdata.open(fpdata_name,fstream::out);  // begin new file
    else
      fpdata.open(fpdata_name,fstream::out | fstream::app);
  
    fpdata << "Event: " ;
    fpdata << vNEvent << endl;
      // write vMCPoints
    Int_t n = vMCPoints.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vMCPoints[i].xIn << " ";
      fpdata << vMCPoints[i].yIn << " ";
      fpdata << vMCPoints[i].zIn << "  ";
      fpdata << vMCPoints[i].pxIn << " ";
      fpdata << vMCPoints[i].pyIn << " ";
      fpdata << vMCPoints[i].pzIn << " " << endl;
      fpdata << vMCPoints[i].xOut << " ";
      fpdata << vMCPoints[i].yOut << " ";
      fpdata << vMCPoints[i].zOut << "  ";
      fpdata << vMCPoints[i].pxOut << " ";
      fpdata << vMCPoints[i].pyOut << " ";
      fpdata << vMCPoints[i].pzOut << " " << endl;
      
      fpdata << vMCPoints[i].p << "  ";
      fpdata << vMCPoints[i].q << " ";
      fpdata << vMCPoints[i].mass << "   ";
      
      fpdata << vMCPoints[i].pdg << " ";
      fpdata << vMCPoints[i].ID << " ";
      fpdata << vMCPoints[i].mother_ID << " ";
      fpdata << vMCPoints[i].iStation << endl;

      const int nhits = vMCPoints[i].hitIds.size();
      fpdata << nhits << endl << "   ";
      for (int k = 0; k < nhits; k++){
        fpdata << vMCPoints[i].hitIds[k] << " ";
      };
      fpdata << endl;
    };
    if (fVerbose >= 4) cout << "vMCPoints[" << n << "]" << " have been written." << endl;
    
          // write vMCTracks  . without Points
    n = vMCTracks.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vMCTracks[i].x << " ";
      fpdata << vMCTracks[i].y << " ";
      fpdata << vMCTracks[i].z << "  ";
      fpdata << vMCTracks[i].px << " ";
      fpdata << vMCTracks[i].py << " ";
      fpdata << vMCTracks[i].pz << " ";
      fpdata << vMCTracks[i].p << "  ";
      fpdata << vMCTracks[i].q << " ";
      fpdata << vMCTracks[i].mass << "   ";
      
      fpdata << vMCTracks[i].pdg << " ";
      fpdata << vMCTracks[i].ID << " ";
      fpdata << vMCTracks[i].mother_ID << endl;
      
      int nhits = vMCTracks[i].StsHits.size();
      fpdata  << "   " << nhits << endl << "   ";
      for (int k = 0; k < nhits; k++){
        fpdata << vMCTracks[i].StsHits[k] << " ";
      };
      fpdata << endl;

      const int nPoints = vMCTracks[i].Points.size();
      fpdata << nPoints << endl << "   ";
      for (int k = 0; k < nPoints; k++){
        fpdata << vMCTracks[i].Points[k] << " ";
      };
      fpdata << endl;
      
      fpdata << vMCTracks[i].nMCContStations << " ";
      fpdata << vMCTracks[i].nHitContStations << " ";
      fpdata << vMCTracks[i].maxNStaMC << " ";
      fpdata << vMCTracks[i].maxNSensorMC << " ";
      fpdata << vMCTracks[i].maxNStaHits << " ";
      fpdata << vMCTracks[i].nStations << endl;
      
    };
    if (fVerbose >= 4) cout << "vMCTracks[" << n << "]" << " have been written." << endl;
    
        // write vHitMCRef
    n = vHitMCRef.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vHitMCRef[i] << endl;
    };
    if (fVerbose >= 4) cout << "vHitMCRef[" << n << "]" << " have been written." << endl;
    
      // write vHitStore
    n = vHitStore.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vHitStore[i].ExtIndex << "  ";
      fpdata << vHitStore[i].iStation << "  ";
      
      fpdata << vHitStore[i].x << " ";
      fpdata << vHitStore[i].y << endl;
    };
    if (fVerbose >= 4) cout << "vHitStore[" << n << "]" << " have been written." << endl;

          // write vStsHits
    n = vStsHits.size();  // number of elements
    fpdata << n << endl;
    for (int i = 0; i < n; i++){
      fpdata << vStsHits[i].hitId << " ";
      fpdata << vStsHits[i].extIndex << endl;

      const int nPoints = vStsHits[i].mcPointIds.size();
      fpdata << nPoints << endl << "   ";
      for (int k = 0; k < nPoints; k++){
        fpdata << vStsHits[i].mcPointIds[k] << " ";
      };
      fpdata << endl;
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been written." << endl;
    
    fpdata.close();
  }
  cout << "-I- CbmL1: Data for performance of event number " << vNEvent << " have been written in file " << fpdata_name << endl;
  vNEvent++;
} // void CbmL1::WriteSTAPPerfData()

istream& CbmL1::eatwhite(istream& is) // skip spaces
{
  char c;
  while (is.get(c)) {
    if (isspace(c)==0) {
      is.putback(c);
      break;
    }
  }
  return is;
}

void CbmL1::ReadSTAPGeoData(void* geo_, int &size)
{
  fscal *geo = static_cast<fscal*>( geo_ );
  TString fgeo_name = fSTAPDataDir + "geo_algo.txt";
  ifstream fgeo(fgeo_name);

  cout << "-I- CbmL1: Read geometry from file " << fgeo_name << endl;
  int i;
  for (i = 0; !fgeo.eof(); i++){
    fscal tmp;
    fgeo >> tmp >> eatwhite;
    geo[i] = tmp;
  };
  size = i;
  fgeo.close();
} // void CbmL1::ReadSTAPGeoData(void* geo_, int &size)

void CbmL1::ReadSTAPAlgoData()
{
  static int nEvent = 1;
  static fstream fadata;
  TString fadata_name = fSTAPDataDir + "data_algo.txt";
//  if (nEvent <= maxNEvent){
  if (1){
    if ( nEvent == 1 ){
      fadata.open(fadata_name,fstream::in);
    };
    
    algo->vStsHits.clear();
    algo->vStsStrips.clear();
    algo->vStsStripsB.clear();
    algo->vStsZPos.clear();
    algo->vSFlag.clear();
    algo->vSFlagB.clear();
    
      // check correct position in file
    char s[] = "Event:  ";
    int nEv;
    fadata >> s;
    fadata >> nEv;
    if (nEv != nEvent)  cout << "-E- CbmL1: Can't read event number " << nEvent << " from file " << fadata_name << endl;
    
    int n;  // number of elements
      // read algo->vStsStrips
    fadata >> n;
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsStrips.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsStrips[" << n << "]" << " have been read." << endl;
      // read algo->vStsStripsB
    fadata >> n;
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsStripsB.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsStripsB[" << n << "]" << " have been read." << endl;
      // read algo->vStsZPos
    fadata >> n;
    for (int i = 0; i < n; i++){
      fscal element;
      fadata >> element;
      algo->vStsZPos.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsZPos[" << n << "]" << " have been read." << endl;
      // read algo->vSFlag
    fadata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fadata >> element;
      algo->vSFlag.push_back(static_cast<unsigned char>(element));
    };
    if (fVerbose >= 4) cout << "vSFlag[" << n << "]" << " have been read." << endl;
      // read algo->vSFlagB
    fadata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fadata >> element;
      algo->vSFlagB.push_back(static_cast<unsigned char>(element));
    };
    if (fVerbose >= 4) cout << "vSFlagB[" << n << "]" << " have been read." << endl;
      // read algo->vStsHits
    fadata >> n;
    int element_f;  // for convert
    int element_b;
    int element_iz;
    for (int i = 0; i < n; i++){
      L1StsHit element;
      fadata >> element_f >> element_b >> element_iz;
      element.f = static_cast<THitI>(element_f);
      element.b = static_cast<THitI>(element_b);
      element.iz = static_cast<TZPosI>(element_iz);
      algo->vStsHits.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been read." << endl;
      // read StsHitsStartIndex and StsHitsStopIndex
    n = 20;
    for (int i = 0; i < n; i++){
      int tmp;
      fadata >> tmp;
      if (algo->MaxNStations+1 > i) algo->StsHitsStartIndex[i] = tmp;
    };
    for (int i = 0; i < n; i++){
      int tmp;
      fadata >> tmp;
      if (algo->MaxNStations+1 > i) algo->StsHitsStopIndex[i] = tmp;
    };

    cout << "-I- CbmL1: CATrackFinder data for event " << nEvent << " has been read from file " << fadata_name << " successfully." << endl;
//    if (nEvent == maxNEvent) fadata.close();  
  }
  nEvent++;
} // void CbmL1::ReadSTAPAlgoData()

void CbmL1::ReadSTAPPerfData()
{
  static int nEvent = 1;
  static fstream fpdata;
  TString fpdata_name = fSTAPDataDir + "data_perfo.txt";
//  if (nEvent <= maxNEvent){
  if (1){
    if ( nEvent == 1 ){
      fpdata.open(fpdata_name,fstream::in);
    };

    vMCPoints.clear();
    vMCTracks.clear();
    vHitMCRef.clear();
    vHitStore.clear();
    vStsHits.clear();

      // check if it is right position in file
    char s[] = "EVENT:     ";  // buffer
    int nEv=0;                // event number
    fpdata >> s;
    fpdata >> nEv;

    if (nEv != nEvent)  cout << "-E- CbmL1: Performance: can't read event number " << nEvent << " from file " << "data_perfo.txt" << endl;
      // vMCPoints
    int n;  // number of elements
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1MCPoint element;
      
      fpdata >> element.xIn;
      fpdata >> element.yIn;
      fpdata >> element.zIn;
      fpdata >> element.pxIn;
      fpdata >> element.pyIn;
      fpdata >> element.pzIn;
            
      fpdata >> element.xOut;
      fpdata >> element.yOut;
      fpdata >> element.zOut;
      fpdata >> element.pxOut;
      fpdata >> element.pyOut;
      fpdata >> element.pzOut;
      
      fpdata >> element.p;
      fpdata >> element.q;
      fpdata >> element.mass;
      
      fpdata >> element.pdg;
      fpdata >> element.ID;
      fpdata >> element.mother_ID;
      fpdata >> element.iStation;

      int nhits;
      fpdata >> nhits;
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.hitIds.push_back(helement);
      };
      
      vMCPoints.push_back(element);
    };
    if (fVerbose >= 4) cout << "vMCPoints[" << n << "]" << " have been read." << endl;

      // vMCTracks . without Points
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1MCTrack element;

      fpdata >> element.x;
      fpdata >> element.y;
      fpdata >> element.z;
      fpdata >> element.px;
      fpdata >> element.py;
      fpdata >> element.pz;
      fpdata >> element.p;
      fpdata >> element.q;
      fpdata >> element.mass;

      fpdata >> element.pdg;
      fpdata >> element.ID;
      fpdata >> element.mother_ID;

      int nhits;
      fpdata >> nhits;
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.StsHits.push_back(helement);
      };
      fpdata >> nhits;
      for (int k = 0; k < nhits; k++){
        int helement;
        fpdata >> helement;
        element.Points.push_back(helement);
      };

      fpdata >> element.nMCContStations;
      fpdata >> element.nHitContStations;
      fpdata >> element.maxNStaMC;
      fpdata >> element.maxNSensorMC;
      fpdata >> element.maxNStaHits;
      fpdata >> element.nStations;

      element.CalculateIsReconstructable();
      vMCTracks.push_back(element);
    };
    if (fVerbose >= 4) cout << "vMCTracks[" << n << "]" << " have been read." << endl;
    
        // vHitMCRef
    fpdata >> n;
    for (int i = 0; i < n; i++){
      int element;
      fpdata >> element;
      vHitMCRef.push_back(element);
    };
    if (fVerbose >= 4) cout << "vHitMCRef[" << n << "]" << " have been read." << endl;

      // vHitStore
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1HitStore element;
      fpdata >> element.ExtIndex;
      fpdata >> element.iStation;

      fpdata >> element.x;
      fpdata >> element.y;
      vHitStore.push_back(element);
    };
    if (fVerbose >= 4) cout << "vHitStore[" << n << "]" << " have been read." << endl;
    
          // vStsHits
    fpdata >> n;
    for (int i = 0; i < n; i++){
      CbmL1StsHit element;
      fpdata >> element.hitId;
      fpdata >> element.extIndex;

      int nPoints;
      fpdata >> nPoints;
      for (int k = 0; k < nPoints; k++){
        int id;
        fpdata >> id;
        element.mcPointIds.push_back(id);
      };
      vStsHits.push_back(element);
    };
    if (fVerbose >= 4) cout << "vStsHits[" << n << "]" << " have been read." << endl;
    

    
//    if (nEvent == maxNEvent) { // file open on begin of all work class and close at end
//       fpdata.close();
//       cout << " -I- Performance: data read from file " << "data_perfo.txt" << " successfully"<< endl;
//     }
    cout << "-I- CbmL1: L1Performance data for event " << nEvent << " has been read from file " << fpdata_name << " successfully." << endl;

  } // if (nEvent <= maxNEvent)
  nEvent++;
} // void CbmL1::ReadSTAPPerfData()

void CbmL1::WriteSIMDKFData()
{
  static bool first = 1;

  /// Write geometry info
  if(first)
  {
    FairField *dMF = CbmKF::Instance()->GetMagneticField();

    fstream FileGeo;
    FileGeo.open( "geo.dat", ios::out );

    fstream FieldCheck;
    FieldCheck.open( "field.dat", ios::out );

    Double_t bfg[3],rfg[3];

    rfg[0] = 0.; rfg[1] = 0.; rfg[2] = 0.;
    dMF->GetFieldValue( rfg, bfg );
    FileGeo<<rfg[2]<<" "<<bfg[0]<<" "<<bfg[1]<<" "<<bfg[2]<<" "<<endl;

    rfg[0] = 0.; rfg[1] = 0.; rfg[2] = 2.5;
    dMF->GetFieldValue( rfg, bfg );
    FileGeo<<rfg[2]<<" "<<bfg[0]<<" "<<bfg[1]<<" "<<bfg[2]<<" "<<endl;

    rfg[0] = 0.; rfg[1] = 0.; rfg[2] = 5.0;
    dMF->GetFieldValue( rfg, bfg );
    FileGeo<<rfg[2]<<" "<<bfg[0]<<" "<<bfg[1]<<" "<<bfg[2]<<" "<<endl<<endl;
    FileGeo<<NStation<<endl;

    const int M=5; // polinom order
    const int N=(M+1)*(M+2)/2;

    for ( Int_t ist = 0; ist<NStation; ist++ )
    {
      fscal f_phi, f_sigma, b_phi, b_sigma;

      double C[3][N];
      double z = 0;
      double Xmax, Ymax;
      if( ist<NMvdStations ){
        CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
        f_phi=0; f_sigma=5.e-4; b_phi=3.14159265358/2.; b_sigma=5.e-4;
        z = t.z;
        Xmax = Ymax = t.R;
      }else{
        //AZ CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
        CbmStsStation *st = StsDigi->GetStation(ist - NMvdStations);
        CbmStsSector* sector = st->GetSector(0);
        f_phi = sector->GetRotation();
        b_phi = sector->GetRotation();
        if( sector->GetType()==1 ){
          b_phi += 3.14159265358/2.;
          b_sigma = sector->GetDy()/sqrt(12.);
          f_sigma = b_sigma; // CHECKME
        }
        else{
          f_phi +=sector->GetStereoF();
          b_phi +=sector->GetStereoB();
          f_sigma = sector->GetDx() / TMath::Sqrt(12);
          b_sigma  = f_sigma;
        }
        //AZ f_sigma *= cos(f_phi);  // TODO: think about this
        //AZ b_sigma *= cos(b_phi);
        z = st->GetZ();

        Xmax=-100; Ymax=-100;

        double x,y;
        for(int isec = 0; isec<st->GetNSectors(); isec++)
        {
          CbmStsSector *sect = L1_DYNAMIC_CAST<CbmStsSector*>( st->GetSector(isec) );
          for(int isen = 0; isen < sect->GetNSensors(); isen++)
          {
            x = sect->GetSensor(isen)->GetX0() + sect->GetSensor(isen)->GetLx()/2.;
            y = sect->GetSensor(isen)->GetY0() + sect->GetSensor(isen)->GetLy()/2.;
            if(x>Xmax) Xmax = x;
            if(y>Ymax) Ymax = y;
          }
        }
      }

      double dx = 1.; // step for the field approximation
      double dy = 1.;

      if( dx > Xmax/N/2 ) dx = Xmax/N/4.;
      if( dy > Ymax/N/2 ) dy = Ymax/N/4.;

      for( int i=0; i<3; i++)
        for( int k=0; k<N; k++) C[i][k] = 0;
      TMatrixD A(N,N);
      TVectorD b0(N), b1(N), b2(N);
      for( int i=0; i<N; i++){
        for( int j=0; j<N; j++) A(i,j) = 0.;
        b0(i)=b1(i)=b2(i) = 0.;
      }
      for( double x=-Xmax; x<=Xmax; x+=dx )
        for( double y=-Ymax; y<=Ymax; y+=dy )
      {
        double r = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
        if( r>1. ) continue;
        Double_t w = 1./(r*r+1);
        Double_t p[3] = { x, y, z};
        Double_t B[3] = {0.,0.,0.};
        CbmKF::Instance()->GetMagneticField()->GetFieldValue(p, B);
        TVectorD m(N);
        m(0)=1;
        for( int i=1; i<=M; i++){
          int k = (i-1)*(i)/2;
          int l = i*(i+1)/2;
          for( int j=0; j<i; j++ ) m(l+j) = x*m(k+j);
          m(l+i) = y*m(k+i-1);
        }
      
        TVectorD mt = m;
        for( int i=0; i<N; i++){
          for( int j=0; j<N;j++) A(i,j)+=w*m(i)*m(j);
          b0(i)+=w*B[0]*m(i);
          b1(i)+=w*B[1]*m(i);
          b2(i)+=w*B[2]*m(i);
        }
      }
      double det;
      A.Invert(&det);
      TVectorD c0 = A*b0, c1 = A*b1, c2 = A*b2;
      for(int i=0; i<N; i++){
        C[0][i] = c0(i);
        C[1][i] = c1(i);
        C[2][i] = c2(i);
      }

      double c_f = cos(f_phi);
      double s_f = sin(f_phi);
      double c_b = cos(b_phi);
      double s_b = sin(b_phi);

      double det_m = c_f*s_b - s_f*c_b;
      det_m *=det_m;
//      double C00 = ( s_b*s_b*f_sigma*f_sigma + s_f*s_f*b_sigma*b_sigma )/det_m;
//      double C10 =-( s_b*c_b*f_sigma*f_sigma + s_f*c_f*b_sigma*b_sigma )/det_m;
//      double C11 = ( c_b*c_b*f_sigma*f_sigma + c_f*c_f*b_sigma*b_sigma )/det_m;

//      float delta_x = sqrt(C00);
//      float delta_y = sqrt(C11);
      FileGeo<<"    "<<ist<<" ";
      if( ist<NMvdStations )
      {
        CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
        FileGeo<<t.z<<" ";
        FileGeo<<t.dz<<" ";
        FileGeo<<t.RadLength<<" ";
      }
      else if(ist<(NStsStations+NMvdStations))
      {
        //AZ CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
        CbmStsStation *st = StsDigi->GetStation(ist - NMvdStations);
        FileGeo<<st->GetZ()<<" ";
        FileGeo<<st->GetD()<<" ";
        FileGeo<<st->GetRadLength()<<" ";
      }
      FileGeo<<f_sigma<<" "; 
      FileGeo<<b_sigma<<" "; 
      FileGeo<<f_phi<<" "; 
      FileGeo<<b_phi<<endl;
      FileGeo<<"    "<<N<<endl;
      FileGeo<<"       ";
      for(int ik=0; ik<N; ik++)
        FileGeo<< C[0][ik]<<" ";
      FileGeo<<endl;
      FileGeo<<"       ";
      for(int ik=0; ik<N; ik++)
        FileGeo<< C[1][ik]<<" ";
      FileGeo<<endl;
      FileGeo<<"       ";
      for(int ik=0; ik<N; ik++)
        FileGeo<< C[2][ik]<<" ";
      FileGeo<<endl;
    }
    FileGeo.close();
  }

  ///Write Tracks and MC Tracks

  static int TrNumber=0;
  fstream Tracks, McTracksCentr, McTracksIn, McTracksOut;
  if(first)
  {
    Tracks.open( "tracks.dat", fstream::out );
    McTracksCentr.open( "mctrackscentr.dat", fstream::out );
    McTracksIn.open( "mctracksin.dat", fstream::out );
    McTracksOut.open( "mctracksout.dat", fstream::out );
    first = 0;
  }
  else
  {
    Tracks.open( "tracks.dat", fstream::out | fstream::app);
    McTracksCentr.open( "mctrackscentr.dat", fstream::out | fstream::app);
    McTracksIn.open( "mctracksin.dat", fstream::out | fstream::app);
    McTracksOut.open( "mctracksout.dat", fstream::out | fstream::app);
  }

  for (vector<CbmL1Track>::iterator RecTrack = vRTracks.begin(); RecTrack != vRTracks.end(); ++RecTrack)
  {
    if ( RecTrack->IsGhost() ) continue;

    CbmL1MCTrack* MCTrack = RecTrack->GetMCTrack();
    if(!(MCTrack->IsPrimary())) continue;

    int NHits = (RecTrack->StsHits).size();
    float x[20],y[20],z[20];
    int st[20];
    int jHit = 0;
    for(int iHit=0; iHit<NHits; iHit++)
    {
      CbmL1HitStore &h = vHitStore[ RecTrack->StsHits[iHit] ];
      st[jHit] = h.iStation;
      if( h.ExtIndex<0 )
      {
        CbmMvdHit* MvdH = (CbmMvdHit*)listMvdHits->At(-h.ExtIndex-1);
        x[jHit] = MvdH->GetX();
        y[jHit] = MvdH->GetY();
        z[jHit] = MvdH->GetZ();
        jHit++;
      }
      else
      {
        CbmStsHit* StsH = (CbmStsHit*)listStsHits->At(h.ExtIndex);
        x[jHit] = StsH->GetX();
        y[jHit] = StsH->GetY();
        z[jHit] = StsH->GetZ();
        jHit++;
      }
    }

    Tracks <<TrNumber <<" "<<MCTrack->x<<" "<<MCTrack->y<<" "<<MCTrack->z<<" "<<
             MCTrack->px<<" "<<MCTrack->py<<" "<<MCTrack->pz<<" "<<MCTrack->q<<" "<<NHits<<endl;

    float AngleXAxis = 0,AngleYAxis = 0;
    for( int i=0; i<NHits; i++ )
      Tracks << "     " << st[i] <<" "<< x[i] <<" "<<y[i]<<" "<<z[i]<<" "<<AngleXAxis<<" "<<AngleYAxis<<endl;
    Tracks<<endl;

    int NMCPoints = (MCTrack->Points).size();

    McTracksIn <<TrNumber <<" "<<MCTrack->x<<" "<<MCTrack->y<<" "<<MCTrack->z<<" "<<
                 MCTrack->px<<" "<<MCTrack->py<<" "<<MCTrack->pz<<" "<<MCTrack->q<<" "<<NMCPoints<<endl;
    McTracksOut <<TrNumber <<" "<<MCTrack->x<<" "<<MCTrack->y<<" "<<MCTrack->z<<" "<<
                  MCTrack->px<<" "<<MCTrack->py<<" "<<MCTrack->pz<<" "<<MCTrack->q<<" "<<NMCPoints<<endl;
    McTracksCentr <<TrNumber <<" "<<MCTrack->x<<" "<<MCTrack->y<<" "<<MCTrack->z<<" "<<
                    MCTrack->px<<" "<<MCTrack->py<<" "<<MCTrack->pz<<" "<<MCTrack->q<<" "<<NMCPoints<<endl;

    for(int iPoint=0; iPoint < NMCPoints; iPoint++)
    {
      CbmL1MCPoint &MCPoint = vMCPoints[ MCTrack->Points[iPoint] ];
      McTracksIn << "     " << MCPoint.iStation <<" "<< 
                               MCPoint.xIn <<" "<<MCPoint.yIn<<" "<<MCPoint.zIn<<" "<<
                               MCPoint.pxIn <<" "<<MCPoint.pyIn<<" "<<MCPoint.pzIn<<endl;
      McTracksOut << "     " << MCPoint.iStation <<" "<< 
                               MCPoint.xOut <<" "<<MCPoint.yOut<<" "<<MCPoint.zOut<<" "<<
                               MCPoint.pxOut <<" "<<MCPoint.pyOut<<" "<<MCPoint.pzOut<<endl;
      McTracksCentr << "     " << MCPoint.iStation <<" "<< 
                               MCPoint.x <<" "<<MCPoint.y<<" "<<MCPoint.z<<" "<<
                               MCPoint.px <<" "<<MCPoint.py<<" "<<MCPoint.pz<<endl;
    }
    McTracksIn << endl;
    McTracksOut << endl;
    McTracksCentr << endl;

    TrNumber++;
  }
}

ClassImp(CbmL1)
