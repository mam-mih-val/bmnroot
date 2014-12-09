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

#ifndef CbmL1MCTrack_H
#define CbmL1MCTrack_H

#include "TVector3.h"
#include "TLorentzVector.h"
#include "CbmL1MCPoint.h"
#include <vector>
#include <iostream>
using std::vector;

class CbmL1Track;

class CbmL1MCTrack
{
 public:
  double mass, q, p, x, y, z, px, py, pz;
  int ID, mother_ID, pdg;
  vector<int> Points;  // indices of pints in L1::vMCPoints
  vector<int> StsHits; // indices of hits in algo->vStsHits or L1::vStsHits

  CbmL1MCTrack()
    :mass(0),q(0),p(0),x(0),y(0),z(0),px(0),py(0),pz(0),ID(-1),mother_ID(-1),pdg(-1),Points(),StsHits(),
    nMCContStations(0),nHitContStations(0),maxNStaMC(0),maxNSensorMC(0),maxNStaHits(0),nStations(0),nMCStations(0),isReconstructable(0),isAdditional(0),
     rTracks(),tTracks(){};

  CbmL1MCTrack(int _ID)
    :mass(0),q(0),p(0),x(0),y(0),z(0),px(0),py(0),pz(0),ID(_ID),mother_ID(-1),pdg(-1),Points(),StsHits(),
    nMCContStations(0),nHitContStations(0),maxNStaMC(0),maxNSensorMC(0),maxNStaHits(0),nStations(0),nMCStations(),isReconstructable(0),isAdditional(0),
     rTracks(),tTracks(){};
  CbmL1MCTrack(double mass, double q, TVector3 vr, TLorentzVector vp, int ID, int mother_ID, int pdg);
//   CbmL1MCTrack(TmpMCPoints &mcPoint, TVector3 vr, TLorentzVector vp, int ID, int mother_ID);

  bool IsPrimary()         const {return mother_ID < 0;};
  bool IsReconstructable() const {return isReconstructable;};
  bool IsAdditional()      const {return isAdditional; }
  int  NStations()         const {return nStations;};
  int  NMCStations()       const {return nMCStations;};
  int  NMCContStations()   const {return nMCContStations;};
  
  void Init();

  void AddRecoTrack(CbmL1Track* rTr){rTracks.push_back(rTr);}
  vector< CbmL1Track* >&  GetRecoTracks(){ return rTracks;}
  int  GetNClones() const { return rTracks.size() - 1;}
  bool IsReconstructed() const { return rTracks.size(); }
  
  void AddTouchTrack(CbmL1Track* tTr){tTracks.push_back(tTr);}
  bool IsDisturbed() const { return tTracks.size(); }

  friend class CbmL1;
 private:
  int nMCContStations;   // number of consecutive stations with mcPoints
  int nHitContStations;  // number of consecutive stations with hits
  int maxNStaMC;         // max number of mcPoints on station
  int maxNSensorMC;         // max number of mcPoints with same z
  int maxNStaHits;       // max number of hits on station
   
  int nStations;         // number of stations with hits
  int nMCStations;         // number of stations with MCPoints

  bool isReconstructable;
  bool isAdditional; // is not reconstructable, but stil interesting
   
  void CalculateMCCont();
  void CalculateHitCont();
  void CalculateMaxNStaHits();
  void CalculateMaxNStaMC();
  void CalculateIsReconstructable();

    // next members filled and used in Performance
  vector< CbmL1Track* >  rTracks; // array of assosiated recoTracks
  vector< CbmL1Track* >  tTracks; // array of recoTracks wich aren't assosiated with this mcTrack, but use some hits from it.

};


#endif
