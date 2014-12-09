/*
 *=====================================================
 *
 *  CBM Level 1 Reconstruction
 *
 *  Authors: M.Zyzak
 *
 *  e-mail : m.zyzak@gsi.de
 *
 *=====================================================
 *
 *  Finds Particles: Lambdas, K0
 *
 */

#ifndef _CbmKFParticlesFinder_h_
#define _CbmKFParticlesFinder_h_

#include "FairTask.h"
//#include "CbmKFParticle.h"
#include "KFParticle.h"

#include <vector>

class TClonesArry;
class CbmVertex;
class CbmRichElectronIdAnn;

class CbmKFParticlesFinder: public FairTask
{
 public:
  CbmKFParticlesFinder(float cuts[2][3] = 0,
                       Int_t usePID = 0,
                       const char *name ="CbmKFParticlesFinder",
                       const char *title="Cbm KF Particles Finder",
                       Int_t iVerbose = 1);
  ~CbmKFParticlesFinder();

  void SetParContainers();
  InitStatus ReInit();
  InitStatus Init();
  void Exec(Option_t * option);
  void Finish();

//  vector<CbmKFParticle>& GetParticles()  {return fParticles;}
  std::vector<KFParticle>& GetParticles()  {return fParticles;}
  std::vector<float>& GetChiPrim()  {return fChiToPrimVtx;}

  ClassDef(CbmKFParticlesFinder,1);

 private:

  float fCuts[2][3];
  Int_t fusePID;

  TClonesArray* flistStsTracks;
  CbmVertex* fPrimVtx;

//  vector<CbmKFParticle> fParticles;
  std::vector<KFParticle> fParticles;
  std::vector<float> fChiToPrimVtx;

// for the particle id
  TClonesArray* flistStsTracksMatch;
  TClonesArray* flistMCTracks;
  TClonesArray* flsitGlobalTracks;
  TClonesArray* flistTofHits;
  TClonesArray* flistRichRings;
  TClonesArray* flistTrdTracks;

  //CbmRichElectronIdAnn *fElIdAnn;

  CbmKFParticlesFinder(const CbmKFParticlesFinder&);
  CbmKFParticlesFinder& operator=(const CbmKFParticlesFinder&);

};

#endif

