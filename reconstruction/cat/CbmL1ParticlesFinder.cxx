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

#include "CbmL1ParticlesFinder.h"
#include "CbmL1Def.h"
#include "CbmL1Track.h"

#include "CbmKFParticleInterface.h"


#include "TStopwatch.h"
#include <iostream>
CbmL1ParticlesFinder::CbmL1ParticlesFinder():
  fPVFinder(),
  fPrimVtx(),
  fParticles()
{
}

CbmL1ParticlesFinder::~CbmL1ParticlesFinder()
{
}

void CbmL1ParticlesFinder::FindPV(vector<CbmL1Track> &vRTracks)
{
  fPVFinder.Clear();
  for( unsigned short iTr=0; iTr<vRTracks.size(); iTr++ ){
    if( vRTracks[iTr].GetNOfHits() < 4 ) continue;
    if( vRTracks[iTr].GetRefChi2()<0. || vRTracks[iTr].GetRefChi2()>3.5*3.5*vRTracks[iTr].GetRefNDF() ) continue;
    if( !finite(vRTracks[iTr].GetTrack()[0]) || !finite(vRTracks[iTr].GetCovMatrix()[0])) continue;
    fPVFinder.AddTrack(&vRTracks[iTr]);
  }
  fPVFinder.Fit( fPrimVtx );
//  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!         Tracks " << fPrimVtx.GetRefNTracks() << std::endl;
}

void CbmL1ParticlesFinder::FindParticles(vector<CbmL1Track> &vRTracks)
{
  TStopwatch timerSelect, timerAll, timerPV;

  fParticles.clear();

  timerAll.Start();
  timerPV.Start();

  FindPV(vRTracks);

  timerPV.Stop();
  timerSelect.Start();

  vector<int> vTrackPDG(vRTracks.size(), 211);
//  CbmKFParticleInterface::FindParticles(vRTracks, fParticles, fPrimVtx, vTrackPDG);

  timerSelect.Stop();
  timerAll.Stop();

  static int NEv=0;
  NEv++;
  static double timeSelectCPU=0., timeAllCPU=0., timePVCPU=0.;
  static double timeSelectReal=0., timeAllReal=0., timePVReal=0.;

  timePVCPU += timerPV.CpuTime();
  timeSelectCPU += timerSelect.CpuTime();
  timeAllCPU += timerAll.CpuTime();

  timePVReal += timerPV.RealTime();
  timeSelectReal += timerSelect.RealTime();
  timeAllReal += timerAll.RealTime();

  std::cout << "Particle Finder Times:"<<std::endl;
  std::cout << "  PVFinder:       " <<" Real - "<< timePVReal/NEv << "   CPU - "<< timePVCPU/NEv << std::endl;
  std::cout << "  Particle finding" <<" Real - "<< timeSelectReal/NEv << "   CPU - "<< timeSelectCPU/NEv << std::endl;
  std::cout << "  Total           " <<" Real - "<< timeAllReal/NEv << "   CPU - "<< timeAllCPU/NEv << std::endl;
}
