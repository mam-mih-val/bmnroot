#include "CbmKFParticleDatabase.h"

CbmKFParticleDatabase* CbmKFParticleDatabase::fDatabase = 0;

CbmKFParticleDatabase::CbmKFParticleDatabase()
  : fMass()
{
  fMass[11]    = 0.000510999;
  fMass[-11]   = 0.000510999;
  fMass[211]   = 0.13957;
  fMass[-211]  = 0.13957;
  fMass[2212]  = 0.9382723;
  fMass[-2212] = 0.9382723;
  fMass[321]   = 0.493667;
  fMass[-321]  = 0.493667;
  fMass[13]    = 0.105658;
  fMass[-13]    = 0.105658;

  fDatabase = this;
}

CbmKFParticleDatabase kfPartDatabase; //create instance
