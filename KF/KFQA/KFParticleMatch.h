/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Particles Finder performance 
 *
 *====================================================================
 */

#ifndef _KFParticleMatch_h_
#define _KFParticleMatch_h_

#include "TObject.h"
#include <vector>

class KFParticleMatch: public TObject
{
 public:

  KFParticleMatch();
  ~KFParticleMatch();

  Int_t GetMatch() const { return fMatch; }
  void SetMatch(Int_t i) { fMatch=i; }

  void SetMatchType(Short_t i) { fMatchType=i; }

  Bool_t IsCombinatorialBG() const {return (fMatchType==0);}
  Bool_t IsPhysicsBG() const {return (fMatchType==1);}
  Bool_t IsRecoParticle() const {return (fMatchType==2);}

 private:
  Int_t fMatch;
  Short_t fMatchType; // 0 - combinatorial BG, 1 - physics BG, 2 - reconstructed particle

  ClassDef(KFParticleMatch,1);
};


struct KFMatchParticles // used for Reco to MC match as well as for MC to Reco
{
  KFMatchParticles():ids(),idsMI(){};
  
  bool IsMatched() const { return ids.size() != 0 || idsMI.size() != 0; };
  bool IsMatchedWithPdg() const { return ids.size() != 0; };
  int  GetBestMatch() const { 
    if      (ids.size()   != 0) return ids[0];
    else if (idsMI.size() != 0) return idsMI[0];
    else return -1;
  };
  int  GetBestMatchWithPdg() const { 
    if      (ids.size()   != 0) return ids[0];
    else return -1;
  };
  std::vector<int> ids;
  std::vector<int> idsMI; // matched but pdg is different - miss identification
};

#endif // _KFParticleMatch_h_
