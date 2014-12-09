/*
 *====================================================================
 *
 *  KFMCParticle
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : m.zyzak@gsi.de
 *
 *====================================================================
 *
 *  KF Particles Finder performance 
 *
 *====================================================================
 */

#include "KFMCParticle.h"


KFMCParticle::KFMCParticle() :fDaughterIds(), fMCTrackID(-1), fMotherId(-1), fPDG(0), fIsReconstructable(0)
{
}

KFMCParticle::~KFMCParticle()
{
}

void KFMCParticle::AddDaughter( int i )
{
  fDaughterIds.push_back(i);
}

ClassImp(KFMCParticle);
