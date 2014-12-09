/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail :
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _CbmL1ParticlesFinder_h_
#define _CbmL1ParticlesFinder_h_

#include "CbmKFParticle.h"
#include "CbmKFVertex.h"
#include "CbmKFPrimaryVertexFinder.h"

#include <vector>

class CbmL1Track;

class CbmL1ParticlesFinder
{
 public:
  CbmL1ParticlesFinder();
  ~CbmL1ParticlesFinder();

  void FindParticles(vector<CbmL1Track> &vRTracks);

  vector<CbmKFParticle>& GetParticles()  {return fParticles;}
  CbmKFVertex* GetPV() {return &fPrimVtx;}

 private:

  void FindPV(vector<CbmL1Track> &vRTracks);

//for primary vertex
  CbmKFPrimaryVertexFinder fPVFinder;
  CbmKFVertex fPrimVtx;

  vector<CbmKFParticle> fParticles;
};

#endif

