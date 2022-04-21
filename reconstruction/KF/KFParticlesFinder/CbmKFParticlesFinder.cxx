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
 *  Finds Particles: Lambdas, K0...
 *
 */

#include "CbmKFParticlesFinder.h"
#include "CbmL1Def.h"

#include "KFParticleFinder.h"
#include "KFParticleSIMD.h"
#include "CbmKFVertex.h"
#include "CbmKFTrack.h"
#include "CbmStsTrack.h"

#include "TClonesArray.h"
#include "TStopwatch.h"
#include <iostream>

#include "CbmTrackMatch.h"
#include "CbmMCTrack.h"
//for particle ID from global track
#include "CbmTofHit.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRing.h"
#include "CbmTrdTrack.h"
//for RICH identification
#include "TSystem.h"
// #include "CbmRichElectronIdAnn.h"

#include "CbmL1PFFitter.h"

using std::vector;
using std::ios;

ClassImp(CbmKFParticlesFinder)

CbmKFParticlesFinder::CbmKFParticlesFinder(float cuts[2][3], Int_t usePID, const char *name, const char *title, Int_t iVerbose):
  FairTask(name,iVerbose),
  fCuts(),
  fusePID(usePID),
  flistStsTracks(0),
  fPrimVtx(0),
  fParticles(),
  flistStsTracksMatch(0),
  flistMCTracks(0),
  flsitGlobalTracks(0),
  flistTofHits(0),
  flistRichRings(0),
  flistTrdTracks(0)
//  fElIdAnn(0)
{
  if(cuts)
  {
    for(int i=0; i<2; ++i)
      for(int j=0; j<3; ++j)
        fCuts[i][j] = cuts[i][j];
  }
  else
  {
    fCuts[0][0] = 3.;
    fCuts[0][1] = 3.;
    fCuts[0][2] = -100.;
    fCuts[1][0] = 3.;
    fCuts[1][1] = 3.;
    fCuts[1][2] = -100.;
  }
}

CbmKFParticlesFinder::~CbmKFParticlesFinder()
{
}

void CbmKFParticlesFinder::SetParContainers()
{
}

InitStatus CbmKFParticlesFinder::ReInit()
{
  FairRootManager *fManger = FairRootManager::Instance();

  flistStsTracks = (TClonesArray *)  fManger->GetObject("StsTrack");
  fPrimVtx = (CbmVertex*) fManger->GetObject("PrimaryVertex");
  //for the particle id
  flistStsTracksMatch = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrackMatch") );
  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );

  if (fusePID == 2){
    flsitGlobalTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("GlobalTrack") );
    flistTofHits = dynamic_cast<TClonesArray*>( fManger->GetObject("TofHit") );
/*    flistRichRings = dynamic_cast<TClonesArray*>( fManger->GetObject("RichRing") );
    flistTrdTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("TrdTrack") );

//     if (fRichGeoType != "compact" && fRichGeoType != "large"){
//       fRichGeoType = "compact";
//     }

    std::string richANNFile = gSystem->Getenv("VMCWORKDIR");
//     if (fRichGeoType == "compact"){
      richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
//     }
//     else if (fRichGeoType == "large"){
//       richANNFile += "/parameters/rich/el_id_ann_weights_rich.txt";
//     }

    fElIdAnn = new CbmRichElectronIdAnn(richANNFile);
    fElIdAnn->Init();*/
  }

  return kSUCCESS;
}

InitStatus CbmKFParticlesFinder::Init()
{
  return ReInit();
}

void CbmKFParticlesFinder::Exec(Option_t * option)
{
  if(!flistStsTracks) return;
//   if(!fPrimVtx) return;

  if(!flistStsTracksMatch) return;
  if(!flistMCTracks) return;

  vector<CbmStsTrack> vRTracks;
  int nTracks = flistStsTracks->GetEntries();
  vRTracks.resize(nTracks);
  for(int iTr=0; iTr<nTracks; iTr++)
    vRTracks[iTr] = *( (CbmStsTrack*) flistStsTracks->At(iTr));

  CbmKFVertex kfVertex;
  if(fPrimVtx)
    kfVertex = CbmKFVertex(*fPrimVtx);
  
  vector<int> vTrackPDG(vRTracks.size(), -1);
  if(fusePID == 1)
  {
    for(int iTr=0; iTr<nTracks; iTr++)
    {
      CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(iTr);
      if(stsTrackMatch -> GetNofMCTracks() == 0) continue;
      const int mcTrackId = stsTrackMatch->GetMCTrackId();
      CbmMCTrack* mcTrack = (CbmMCTrack*)flistMCTracks->At(mcTrackId);
      vTrackPDG[iTr] = mcTrack->GetPdgCode();
    }
  }

  if(fusePID == 2)
  {
    if (NULL == flsitGlobalTracks) { Fatal("KF Particle Finder", "No GlobalTrack array!"); }
    if (NULL == flistTofHits) { Fatal("KF Particle Finder", "No TOF hits array!"); }

    const Double_t m2P  = 0.867681;
    const Double_t m2K  = 0.243707;
    const Double_t m2Pi = 0.019479835;

    Double_t sP[3][5] = { {0.0243467, -0.00937908, 0.00970666, -0.00063638, 2.42128e-05},
                          {0.00691158, -0.000777367, 0.00671164, -0.000123594, -2.17075e-05},
                          {0.0055599, -0.00327273, 0.00981961, -0.00108836, 7.90106e-05} };

//     sP[0][0] =  0.0618927;
//     sP[0][1] = -0.0719277;
//     sP[0][2] =  0.0396255;
//     sP[0][3] = -0.00583356;
//     sP[0][4] =  0.000317072;
// 
//     sP[1][0] =  0.0291881;
//     sP[1][1] = -0.0904978;
//     sP[1][2] =  0.086161;
//     sP[1][3] = -0.0229688;
//     sP[1][4] =  0.00199382;
// 
//     sP[2][0] =  0.162171;
//     sP[2][1] = -0.194007;
//     sP[2][2] =  0.0893264;
//     sP[2][3] = -0.014626;
//     sP[2][4] =  0.00088203;

    const Int_t PdgHypo[4] = {2212, 321, 211, -11};

    for (Int_t igt = 0; igt < flsitGlobalTracks->GetEntriesFast(); igt++) {
      const CbmGlobalTrack* globalTrack = static_cast<const CbmGlobalTrack*>(flsitGlobalTracks->At(igt));

      Int_t stsTrackIndex = globalTrack->GetStsTrackIndex();
      if( stsTrackIndex<0 ) continue;

//       Bool_t isElectronTRD = 0;
//       Bool_t isElectronRICH = 0;
//       Bool_t isElectron = 0;

      FairTrackParam *stsPar = vRTracks[stsTrackIndex].GetParamFirst();
      TVector3 mom;
      stsPar->Momentum(mom);

      Double_t p = mom.Mag();
      Int_t q = stsPar->GetQp() > 0 ? 1 : -1;

//       if(flistRichRings)
//       {
//         Int_t richIndex = globalTrack->GetRichRingIndex();
//         if (richIndex > -1)
//         {
//           CbmRichRing* richRing = (CbmRichRing*)flistRichRings->At(richIndex);
//           if (richRing)
//             if(fElIdAnn->DoSelect(richRing, p) > -0.5) isElectronRICH = 1;
//         }
//       }
// 
//       if(flistTrdTracks)
//       {
//         Int_t trdIndex = globalTrack->GetTrdTrackIndex();
//         if (trdIndex > -1)
//         {
//           CbmTrdTrack* trdTrack = (CbmTrdTrack*)flistTrdTracks->At(trdIndex);
//           if (trdTrack)
//             if (trdTrack->GetPidANN() > 0.979) isElectronTRD = 1;
//         }
//       }

      Double_t l = globalTrack->GetLength();
      if( !((l>1000.) && (l<1400.)) ) continue;

      Double_t time;
      Int_t tofHitIndex = globalTrack->GetTofHitIndex();
      if (tofHitIndex >= 0) {
         const CbmTofHit* tofHit = static_cast<const CbmTofHit*>(flistTofHits->At(tofHitIndex));
         if(!tofHit) continue;
         time = tofHit->GetTime();
      }
      else
        continue;

      if( !((time>29.) && (time<50.)) ) continue;

      Double_t m2 = p*p*(1./((l/time/29.9792458)*(l/time/29.9792458))-1.);

      Double_t sigma[3];
      sigma[0] = sP[0][0] + sP[0][1]*p + sP[0][2]*p*p + sP[0][3]*p*p*p + sP[0][4]*p*p*p*p;
      sigma[1] = sP[1][0] + sP[1][1]*p + sP[1][2]*p*p + sP[1][3]*p*p*p + sP[1][4]*p*p*p*p;
      sigma[2] = sP[2][0] + sP[2][1]*p + sP[2][2]*p*p + sP[2][3]*p*p*p + sP[2][4]*p*p*p*p;

      Double_t dm2[3];
      dm2[0] = fabs(m2 - m2P)/sigma[0];
      dm2[1] = fabs(m2 - m2K)/sigma[1];
      dm2[2] = fabs(m2 - m2Pi)/sigma[2];

      int iPdg=2;
      Double_t dm2min = dm2[2];

//       if(isElectronRICH && isElectronTRD)
//       {
//         if (p >= 1.) {
//           if (m2 < (0.01 + (p - 1.) * 0.09))
//             isElectron = 1;
//         }
//         else {
//           if (m2 < 0.0)
//             isElectron = 1;
//         }
//       }
// 
//       if(!isElectron)
      {
        if(p>12.) continue;
        if(q>0)
        {
          if(dm2[1] < dm2min) { iPdg = 1; dm2min = dm2[1]; }
          if(dm2[0] < dm2min) { iPdg = 0; dm2min = dm2[0]; }

          if(dm2min > 2) iPdg=-1;
        }
        else
        {
          if(dm2[1] < dm2min) { iPdg = 1; dm2min = dm2[1]; }
          if((dm2min>3) && (dm2[0] < dm2min)) { iPdg = 0; dm2min = dm2[0]; }

          if(dm2min > 2) iPdg=-1;
        }

        if(iPdg > -1)
          vTrackPDG[stsTrackIndex] = q*PdgHypo[iPdg];
      }
//       else
//         vTrackPDG[stsTrackIndex] = q*PdgHypo[3];
    }
  }

/*
  for(int iTr=0; iTr<nTracks; iTr++)
  {
    if(vRTracks[iTr].GetParamFirst()->GetQp()<0) vTrackPDG[iTr] *= -1;
  }*/
  
  TStopwatch timerSelect;

  fParticles.clear();
  fChiToPrimVtx.clear();

  CbmL1PFFitter fitter;

//  fitter.Fit(vRTracks); //assumed, that the initial fit should be fixed and must return good results!!!

  vector<L1FieldRegion> vField;
  fitter.GetChiToVertex(vRTracks, vField, fChiToPrimVtx, kfVertex, 3);

//  fitter.CalculateFieldRegion(vRTracks, vField);

  vector<CbmKFTrack> vKFTrack(nTracks);
  for(int iTr=0; iTr<nTracks; iTr++)
    vKFTrack[iTr] = CbmKFTrack(vRTracks[iTr]);

  KFParticleSIMD pVtx(kfVertex);
  KFParticleFinder::FindParticles(vKFTrack, fChiToPrimVtx, vField, fParticles, pVtx, vTrackPDG, fCuts);
//  CbmKFParticleInterface::FindParticles(vKFTrack, ChiToPrimVtx, vField, fParticles, kfVertex, vTrackPDG, fCuts);

  timerSelect.Stop();

  static int NEv=0;
  NEv++;
  static double timeSelectCPU=0.;
  static double timeSelectReal=0.;

  timeSelectCPU += timerSelect.CpuTime();
  timeSelectReal += timerSelect.RealTime();

  if(NEv%100==0)
  {
    std::cout.setf(ios::fixed);
    std::cout.setf(ios::showpoint);
    std::cout.precision(9);
    std::cout << " ---- Particle finder --- " << std::endl;
    std::cout << "KF Particle Finder Times:" <<" Real - "<< timeSelectReal/NEv 
                                            << "   CPU - "<< timeSelectCPU/NEv << std::endl << std::endl;
  }
}

void CbmKFParticlesFinder::Finish(){
}
