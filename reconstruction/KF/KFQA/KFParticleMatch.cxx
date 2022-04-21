/*
 *====================================================================
 *
 *  KFParticleMatch
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

#include "KFParticleMatch.h"


KFParticleMatch::KFParticleMatch()
 : TObject(),
   fMatch(-1),
   fMatchType(-1)
{
}

KFParticleMatch::~KFParticleMatch()
{
}

ClassImp(KFParticleMatch)
