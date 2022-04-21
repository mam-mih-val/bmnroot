#ifndef CbmKFParticleDatabase_H
#define CbmKFParticleDatabase_H

#include <map>

class CbmKFParticleDatabase
{
 public:
  CbmKFParticleDatabase();

  ~CbmKFParticleDatabase() {};

  float GetMass(int pdg)
  {
    std::map<int, float>::iterator it;
    it=fMass.find(pdg);
    if(it != fMass.end()) return it->second;
    else return 0.13957;
  }

  static CbmKFParticleDatabase* Instance() { return fDatabase; }

 private:
  std::map<int, float> fMass;

  static CbmKFParticleDatabase* fDatabase;
};

#endif
