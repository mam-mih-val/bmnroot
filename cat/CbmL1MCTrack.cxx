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
 *  L1 Monte Carlo information
 *
 *====================================================================
 */
 
#include "CbmL1.h"
#include "CbmL1MCTrack.h"
#include "CbmL1Constants.h"
#include "L1Algo/L1Algo.h"

CbmL1MCTrack::CbmL1MCTrack(double mass_, double q_, TVector3 vr, TLorentzVector vp, int _ID, int _mother_ID, int _pdg):
  mass(mass_),q(q_),p(0),x(0),y(0),z(0),px(0),py(0),pz(0),ID(_ID), mother_ID(_mother_ID), pdg(_pdg),Points(),StsHits(),
  nMCContStations(0),nHitContStations(0),maxNStaMC(0),maxNSensorMC(0),maxNStaHits(0),nStations(0),nMCStations(0),isReconstructable(0),isAdditional(),
     rTracks(),tTracks()
{
  x = vr.X();
  y = vr.Y();
  z = vr.Z();
  px = vp.Px();
  py = vp.Py();
  pz = vp.Pz();
  p = sqrt( fabs(px*px + py*py + pz*pz ));
};

// CbmL1MCTrack::CbmL1MCTrack(TmpMCPoints &mcPoint, TVector3 vr, TLorentzVector vp, int ID, int mother_ID):
//      ID(_ID), mother_ID(_mother_ID)
// {
//   mass = mcPoint->mass;
//   q = mcPoint->q;
//   pdg  = mcPoint->pdg;
//   
//   x = vr.X();
//   y = vr.Y();
//   z = vr.Z();
//   px = vp.Px();
//   py = vp.Py();
//   pz = vp.Pz();
//   p = sqrt( fabs(px*px + py*py + pz*pz ));
// };

void CbmL1MCTrack::Init()
{
  CbmL1* L1 = CbmL1::Instance();
     // get stsHits
  StsHits.clear();
  for (unsigned int iP = 0; iP < Points.size(); iP++){
    CbmL1MCPoint* point = &(L1->vMCPoints[Points[iP]]);
    for (unsigned int iH = 0; iH < point->hitIds.size(); iH++){
      const int iih = point->hitIds[iH];
      if( std::find(StsHits.begin(), StsHits.end(), iih) == StsHits.end() )
        StsHits.push_back(iih);
    }
  }
  
  CalculateMCCont();
  CalculateHitCont();
  CalculateMaxNStaMC();
  CalculateMaxNStaHits();
  CalculateIsReconstructable();
} // void CbmL1MCTrack::Init()

void CbmL1MCTrack::CalculateMCCont()
{
  CbmL1* L1 = CbmL1::Instance();
  
  int nPoints = Points.size();
  nMCContStations = 0;
  int istaold = -1, ncont=0;
  for( int ih=0; ih<nPoints; ih++ ){
    CbmL1MCPoint &h = L1->vMCPoints[Points[ih]];
    int ista = h.iStation;
    if (ista - istaold == 1) ncont++;
    else if(ista - istaold > 1){
      if( nMCContStations < ncont ) nMCContStations = ncont;
      ncont = 1;
    }
    if (ista <= istaold ) continue; // backward direction
    istaold = ista;
  }
  if( nMCContStations < ncont ) nMCContStations = ncont;
}; // void CbmL1MCTrack::CalculateMCCont()

void CbmL1MCTrack::CalculateHitCont()
{
  CbmL1* L1 = CbmL1::Instance();
  L1Algo* algo = L1->algo;

  int nhits = StsHits.size();
  nHitContStations = 0;
  int istaold = -1, ncont=0;
  for( int ih=0; ih<nhits; ih++ ){
    int jh = StsHits[ih];
    L1StsHit &h = algo->vStsHits[jh];
    int ista = algo->vSFlag[h.f]/4;
    if (ista - istaold == 1) ncont++;
    else if(ista - istaold > 1){
      if( nHitContStations < ncont ) nHitContStations = ncont;
      ncont = 1;
    }

    if ( !( ista >= istaold ) ) { // tracks going in backward direction are not reconstructable
      nHitContStations = 0;
      return;
    }
    if (ista == istaold ) continue; // backward direction
    istaold = ista;
  }
  if( nHitContStations<ncont ) nHitContStations=ncont;
}; // void CbmL1MCTrack::CalculateHitCont()

void CbmL1MCTrack::CalculateMaxNStaHits()
{
  CbmL1* L1 = CbmL1::Instance();

  maxNStaHits = 0;
  nStations = 0;
  int lastSta = -1;
  int cur_maxNStaHits = 0;
  for(unsigned int iH = 0; iH < StsHits.size(); iH++){
    CbmL1HitStore& sh = L1->vHitStore[StsHits[iH]];
    if (sh.iStation == lastSta){ 
      cur_maxNStaHits++;
    }
    else{ // new station
      if ( !(sh.iStation > lastSta) ) {// tracks going in backward direction are not reconstructable
        maxNStaHits = 0;
        return;
      }
      if (cur_maxNStaHits > maxNStaHits) maxNStaHits = cur_maxNStaHits;
      cur_maxNStaHits = 1;
      lastSta = sh.iStation;
      nStations++;
    }
  };
  if (cur_maxNStaHits > maxNStaHits) maxNStaHits = cur_maxNStaHits;
//   cout << pdg << " " << p << " " << StsHits.size() << " > " << maxNStaHits << endl;
}; // void CbmL1MCTrack::CalculateHitCont()

void CbmL1MCTrack::CalculateMaxNStaMC()
{
  CbmL1* L1 = CbmL1::Instance();

  maxNStaMC = 0;
  maxNSensorMC = 0;
  nMCStations = 0;
  int lastSta = -1;
  float lastz = -1;
  int cur_maxNStaMC = 0 , cur_maxNSensorMC = 0;
  for(unsigned int iH = 0; iH < Points.size(); iH++){
    CbmL1MCPoint& mcP = L1->vMCPoints[Points[iH]];
    if (mcP.iStation == lastSta)
      cur_maxNStaMC++;
    else{ // new station
      if (cur_maxNStaMC > maxNStaMC) maxNStaMC = cur_maxNStaMC;
      cur_maxNStaMC = 1;
      lastSta = mcP.iStation;
      nMCStations++;
    }
    
    if (mcP.z == lastz)   // TODO: works incorrect because of different z
      cur_maxNSensorMC++;
    else{ // new z
      if (cur_maxNSensorMC > maxNSensorMC) maxNSensorMC = cur_maxNSensorMC;
      cur_maxNSensorMC = 1;
      lastz = mcP.z;
    }
  };
  if (cur_maxNStaMC > maxNStaMC) maxNStaMC = cur_maxNStaMC;
  if (cur_maxNSensorMC > maxNSensorMC) maxNSensorMC = cur_maxNSensorMC;
//   cout << pdg << " " << p << " " << Points.size() << " > " << maxNStaMC << " " << maxNSensorMC << endl;
}; // void CbmL1MCTrack::CalculateMaxNStaMC()

void CbmL1MCTrack::CalculateIsReconstructable()
{
  CbmL1* L1 = CbmL1::Instance();
  
  bool f = 1;
  
    // reject very slow tracks from analysis
  f &= (p > CbmL1Constants::MinRecoMom);
    // detected at least in 4 stations
    //   f &= (nMCContStations >= 4);
  
    // maximul 4 layers for a station.
    //   f &= (maxNStaHits <= 4);
  f &= (maxNStaMC <= 4);
    //   f &= (maxNSensorMC <= 1);
  
  if (L1->fPerformance == 3) isReconstructable = f & (nMCContStations  >= CbmL1Constants::MinNStations); // L1-MC
  if (L1->fPerformance == 2) isReconstructable = f & (nStations        >= CbmL1Constants::MinNStations); // QA definition
  if (L1->fPerformance == 1) isReconstructable = f & (nHitContStations >= CbmL1Constants::MinNStations); // L1 definition

  isAdditional = f &
    (nHitContStations == nStations) & (nMCContStations == nStations) & (nMCStations == nStations) &
    (nHitContStations >= 3) &
    (L1->vMCPoints[Points[0]].iStation == 0);
  isAdditional &= !isReconstructable;
}; // bool CbmL1MCTrack::IsReconstructable()
