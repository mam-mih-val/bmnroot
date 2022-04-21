#include "KFParticleFinder.h"

#define cnst static const fvec

//for particle finding
#include "CbmL1Track.h"
#include "CbmStsTrack.h"
#include "CbmKFTrack.h"

#include <map>
using std::map;

const float KFParticleFinder::DefaultCuts[2][3]  = {{3.,3.,-100.},{3.,3.,-100.}};

KFParticleFinder::KFParticleFinder()
{
}

void KFParticleFinder::FindParticles(vector<CbmKFTrack> &vRTracks, vector<float>& ChiToPrimVtx,
                                     vector<L1FieldRegion>& vField, vector<KFParticle>& Particles,
                                     KFParticleSIMD& PrimVtx, const vector<int>& vTrackPDG, const float cuts[2][3])
{
  //* The function find the full set of particles at once.
  //* As an input it takes a set of tracks (vector<CbmKFTrack> &vRTracks),
  //* chi to the primary vertex for each track (vector<float>& ChiToPrimVtx) - a distance between the
  //* track and the PV in the plane of the target normalized to the total error of track an PV
  //* ChiToPrimVtx = srqt( (x_pv - x_track)^2 + (y_pv - y_track)^2 )/ (error_pv + error_track),
  //* field for particle extrapolation (vector<L1FieldRegion>& vField),
  //* an output set of found particles (vector<KFParticle>& Particles),
  //* a primary vertex (CbmKFVertex& PrimVtx),
  //* PDG hypothesis for the tracks (const vector<int>& vTrackPDG),
  //* set of cuts (const float cuts[2][3]).
  
  //* At first all particles are divided into 2 sets: positive and negative
  //* Each set in its turn is divided into 8 groups according to the provided PDG hypothesis:
  //* e, mu, pi, K, p, pi+unidentified particles, K+unidentified, p+unidentified

  //* Then mother particles are combined from tracks or already reconstructed particles
  //* according to the particle hypothesis and a decay channel under consideration

  static const int NTrackTypes = 8;

  int pdgPos[NTrackTypes]={-11,-13, 211, 321, 2212,  211, 321, 2212};
  int pdgNeg[NTrackTypes]={ 11, 13,-211,-321,-2212, -211,-321,-2212};

  vector<short> idPosSec[NTrackTypes];
  vector<short> idNegSec[NTrackTypes];

  vector<short> idPosPrim[NTrackTypes];
  vector<short> idNegPrim[NTrackTypes];

  for(unsigned short iTr=0; iTr < vRTracks.size(); iTr++)
  {
    CbmKFTrack &kfTrack = vRTracks[iTr];
    bool ok = 1;
    for(unsigned short iT=0; iT<5; iT++)
      ok = ok && finite(kfTrack.GetTrack()[iT]);
    for(unsigned short iC=0; iC<15; iC++)
      ok = ok && finite(kfTrack.GetCovMatrix()[iC]);
      ok = ok && (kfTrack.GetCovMatrix()[0] < 1. && kfTrack.GetCovMatrix()[0] > 0.)
              && (kfTrack.GetCovMatrix()[2] < 1. && kfTrack.GetCovMatrix()[2] > 0.)
              && (kfTrack.GetCovMatrix()[5] < 1. && kfTrack.GetCovMatrix()[5] > 0.)
              && (kfTrack.GetCovMatrix()[9] < 1. && kfTrack.GetCovMatrix()[9] > 0.)
              && (kfTrack.GetCovMatrix()[14] < 1. && kfTrack.GetCovMatrix()[14] > 0.);
      ok = ok && kfTrack.GetRefChi2() < 10*kfTrack.GetRefNDF();
    if(!ok) continue;

    const int pdg = abs(vTrackPDG[iTr]);

    short pdgIndex = -1;
    switch (pdg)
    {
      case   11: pdgIndex = 0; break;
      case   13: pdgIndex = 1; break;
      case  211: pdgIndex = 2; break;
      case  321: pdgIndex = 3; break;
      case 2212: pdgIndex = 4; break;
    }

    short incr = 3;
    short pdgIndexMax = pdgIndex+incr;

    if(pdgIndex<2) 
    {
      incr = 1;
      pdgIndexMax = pdgIndex;
    }

    if(pdgIndex < 0)
    {
      pdgIndex = 5;
      pdgIndexMax = 7;
      incr = 1;
    }

    if( ChiToPrimVtx[iTr] < cuts[0][0] )
    {
      if(kfTrack.GetTrack()[4] >= 0.)
      {
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idPosPrim[ipdg].push_back(iTr);
      }
      if(kfTrack.GetTrack()[4] < 0.)
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idNegPrim[ipdg].push_back(iTr);
    }
    else
    {
      if(kfTrack.GetTrack()[4] >= 0.)
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idPosSec[ipdg].push_back(iTr);
      if(kfTrack.GetTrack()[4] < 0.)
        for(int ipdg = pdgIndex; ipdg<=pdgIndexMax; ipdg+=incr )
          idNegSec[ipdg].push_back(iTr);
    }
  }

  const int nPart = idPosSec[5].size() * idNegSec[5].size()+
                    idPosSec[5].size() * idNegSec[7].size()+
                    idPosSec[7].size() * idNegSec[5].size()+
                    idPosPrim[2].size() * idNegPrim[3].size() + 
                    idPosPrim[3].size() * idNegPrim[2].size() + 
                    idPosPrim[3].size() * idNegPrim[3].size() + 
                    idPosPrim[4].size() * idNegPrim[3].size() + 
                    idPosPrim[3].size() * idNegPrim[4].size() + 
                    idPosPrim[0].size() * idNegPrim[0].size() + 
                    idPosPrim[1].size() * idNegPrim[1].size();

//std::cout << "NPart estim " << nPart << std::endl;
  Particles.reserve(vRTracks.size() + nPart);

  const float massLambdaPDG = 1.115683;
  const float massK0sPDG = 0.497614;
  const float massKsiPDG = 1.32171;

  for(unsigned short iTr=0; iTr < vRTracks.size(); iTr++) {
    CbmKFTrack& kfTrack = vRTracks[iTr] ;
    kfTrack.SetId(iTr);
    KFParticle tmp(&kfTrack);
    tmp.SetPDG(211);//vTrackPDG[iTr]);
    tmp.SetId(Particles.size());
    tmp.SetNDaughters(1);
    tmp.AddDaughterId( iTr );
    Particles.push_back(tmp);
  }

  vector<float> vLambdaTopoChi2Ndf;
  vector<KFParticle> vLambdaSec;
  vector<KFParticle> vLambdaPrim;

  vector<float> vLambdaBarTopoChi2Ndf;
  vector<KFParticle> vLambdaBarSec;
  vector<KFParticle> vLambdaBarPrim;

  vector<float> vK0sTopoChi2Ndf;
  vector<KFParticle> vK0sPrim;

  vector<float> vGammaTopoChi2Ndf;
  vector<KFParticle> vGammaPrim;
  vector<KFParticle> vGammaSec;

  vector<KFParticle> vPi0Prim;
  vector<KFParticle> vPi0Sec;

  vector<KFParticle> vXiPrim;
  vector<KFParticle> vXiSec;
  vector<KFParticle> vXiBarPrim;

  vector<KFParticle> vXiStarPrim;
  vector<KFParticle> vXiStarBarPrim;

  const float SecCuts[3] = {3.f,5.f,10.f};
//  const float SecCuts[3] = {3.f,5.f,4.f};

  //K0s -> pi+ pi-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[5], pdgPos[5], 310,
                     idNegSec[5], idPosSec[5], PrimVtx, cuts[0], 0, &vK0sTopoChi2Ndf,
                     SecCuts, massK0sPDG, 0.0022, &vK0sPrim, 0);
  //Lambda -> p pi-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[5], pdgPos[7], 3122,
                     idNegSec[5], idPosSec[7], PrimVtx, cuts[1], 0,&vLambdaTopoChi2Ndf,
                     SecCuts, massLambdaPDG, 0.0012, &vLambdaPrim, &vLambdaSec);
  //Lambda_bar -> pi+ p-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgPos[5], pdgNeg[4], -3122,
                     idPosSec[5], idNegSec[4], PrimVtx, cuts[1], 0, &vLambdaBarTopoChi2Ndf,
                     SecCuts, massLambdaPDG, 0.0012, &vLambdaBarPrim, &vLambdaBarSec);
  //K*0 -> K+ pi-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgPos[3], pdgNeg[2], 313,
                     idPosPrim[3], idNegPrim[2], PrimVtx, cuts[1], 1);
  //K*0_bar -> pi+ K-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[3], pdgPos[2], -313,
                     idNegPrim[3], idPosPrim[2], PrimVtx, cuts[1], 1);
  //Lambda* -> p K-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[3], pdgPos[4], 3124,
                     idNegPrim[3], idPosPrim[4], PrimVtx, cuts[1], 1);
  //Lambda*_bar -> K+ p-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[4], pdgPos[3], -3124,
                     idNegPrim[4], idPosPrim[3], PrimVtx, cuts[1], 1);
  //phi -> K+ K-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[3], pdgPos[3], 333,
                     idNegPrim[3], idPosPrim[3], PrimVtx, cuts[1], 1);
//   //rho -> pi+ pi-
//   Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[2], pdgPos[2], 113,
//                      idNegPrim[2], idPosPrim[2],
//                      PrimVtx, cuts[1]);
  //gamma -> e+ e-

  float SecCutsGamma[3] = {3,3,-100};
  float gammaCuts[3] = {3,100000, -100};
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegPrim[0], idPosPrim[0], PrimVtx, gammaCuts, 1, &vGammaTopoChi2Ndf,
                     SecCutsGamma, 0, 0.006, &vGammaPrim, &vGammaSec);
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegSec[0], idPosSec[0], PrimVtx, gammaCuts, 0, &vGammaTopoChi2Ndf,
                     SecCutsGamma, 0, 0.006, &vGammaPrim, &vGammaSec);
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegSec[0], idPosPrim[0], PrimVtx, gammaCuts, 0, &vGammaTopoChi2Ndf,
                     SecCutsGamma, 0, 0.006, &vGammaPrim, &vGammaSec);
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 22,
                     idNegPrim[0], idPosSec[0], PrimVtx, gammaCuts, 0, &vGammaTopoChi2Ndf,
                     SecCutsGamma, 0, 0.006, &vGammaPrim, &vGammaSec);
  //JPsi-> e+ e-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 443,
                     idNegPrim[0], idPosPrim[0], PrimVtx, cuts[1], 1, 1.f);
  //JPsi-> mu+ mu-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[1], pdgPos[1], 100443,
                     idNegPrim[1], idPosPrim[1], PrimVtx, cuts[1], 1, 1.f);
  //rho, omega, phi -> e+ e-
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[0], pdgPos[0], 100113,
                     idNegPrim[0], idPosPrim[0], PrimVtx, cuts[1], 1, 0.2f);
  //rho, omega, phi -> mu+ mu-
  const float PCut = 1.f;
  Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[1], pdgPos[1], 200113,
                     idNegPrim[1], idPosPrim[1], PrimVtx, cuts[1], 1, 0.2f, -100, 0, &PCut);

  ExtrapolateToPV(vLambdaPrim,PrimVtx);
  ExtrapolateToPV(vLambdaBarPrim,PrimVtx);
  ExtrapolateToPV(vK0sPrim,PrimVtx);
  ExtrapolateToPV(vGammaPrim,PrimVtx);

  float cutSigma0[3] = {-100, 3, 3};
  //Sigma0 -> Lambda Gamma
  CombinePartPart(vGammaPrim, vLambdaPrim, Particles, PrimVtx, cutSigma0, 1, 3212, 0);

  //Sigma0_bar -> Lambda_bar Gamma
  CombinePartPart(vGammaPrim, vLambdaBarPrim, Particles, PrimVtx, cutSigma0, 1, -3212, 0);

  //pi0 -> gamma gamma
  float cutPi0[3] = {-100, 1000000, 3};
  float cutPi0Sec[2] = {3,3};
  CombinePartPart(vGammaPrim, vGammaPrim, Particles, PrimVtx, cutPi0, 1, 111, 1,
                  &vPi0Prim, &vPi0Sec, cutPi0Sec, 0.13498, 0.006);
  CombinePartPart(vGammaPrim, vGammaSec, Particles, PrimVtx, cutPi0, 0, 111, 0,
                  &vPi0Prim, &vPi0Sec, cutPi0Sec, 0.13498, 0.006);
  CombinePartPart(vGammaSec, vGammaPrim, Particles, PrimVtx, cutPi0, 0, 111, 0,
                  &vPi0Prim, &vPi0Sec, cutPi0Sec, 0.13498, 0.006);
  CombinePartPart(vGammaSec, vGammaSec, Particles, PrimVtx, cutPi0, 0, 111, 1,
                  &vPi0Prim, &vPi0Sec, cutPi0Sec, 0.13498, 0.006);

  // Find Sigma+ -> p pi0
  float cutSigmaPlus[3] = {-100.,3.,3.};
  FindTrackV0Decay(3222, Particles, vPi0Sec, vRTracks, vField, pdgPos[4], idPosSec[4],
                   PrimVtx, cutSigmaPlus, 0, 0);
  // Find Sigma+ -> p pi0
  float cutSigmaPlusBar[3] = {-100.,3.,3.};
  FindTrackV0Decay(-3222, Particles, vPi0Sec, vRTracks, vField, pdgNeg[4], idNegSec[4],
                   PrimVtx, cutSigmaPlusBar, 0, 0);
  //Sigma*0 -> Lambda pi0
  CombinePartPart(vPi0Prim, vLambdaPrim, Particles, PrimVtx, cutSigma0, 1, 3214, 0);
  //Sigma*0_bar -> Lambda_bar pi0
  CombinePartPart(vPi0Prim, vLambdaBarPrim, Particles, PrimVtx, cutSigma0, 1, -3214, 0);
  //Xi0 -> Lambda pi0
  CombinePartPart(vPi0Sec, vLambdaSec, Particles, PrimVtx, cutSigma0, 0, 3322, 0);
  //Xi0_bar -> Lambda_bar pi0
  CombinePartPart(vPi0Sec, vLambdaBarSec, Particles, PrimVtx, cutSigma0, 0, -3322, 0);

  // Find Xi-
   float cutXi[3] = {10.,5.,6.};
//  float cutXi[3] = {-300.,10.,10.};
  FindTrackV0Decay(3312, Particles, vLambdaSec, vRTracks, vField, pdgNeg[5], idNegSec[5],
                   PrimVtx, cutXi, 0, 0, &vXiPrim, massKsiPDG, 0.002 );


  float cutLL[3] = {10.,10000000.,3.};
  float cutLL2[3] = {10.,3.,3.};
  vector<KFParticle> vLL;
  FindTrackV0Decay(3002, vLL, vLambdaSec, vRTracks, vField, pdgNeg[5], idNegSec[5],
                   PrimVtx, cutLL, 0, &ChiToPrimVtx);
  // Find H0->Lambda p pi-
  //Find Omega*-
  FindTrackV0Decay(3001, Particles, vLL, vRTracks, vField, pdgPos[4], idPosSec[4],
                   PrimVtx, cutLL2, 0, &ChiToPrimVtx);
  // Find Xi+
  float cutXiPlus[3] = {10.,5.,6.};
  FindTrackV0Decay(-3312, Particles, vLambdaBarSec, vRTracks, vField, pdgPos[5], idPosSec[5],
                   PrimVtx, cutXiPlus, 0, 0, &vXiBarPrim, massKsiPDG, 0.002);
  //Find Omega-
  float cutOmega[3] = {10.,3.,3.};
  FindTrackV0Decay(3334, Particles, vLambdaSec, vRTracks, vField, pdgNeg[6], idNegSec[6],
                   PrimVtx, cutOmega, 0, &ChiToPrimVtx);
  //Find Omega+
  float cutOmegaPlus[3] = {10.,3.,3.};
  FindTrackV0Decay(-3334, Particles, vLambdaBarSec, vRTracks, vField, pdgPos[6], idPosSec[6],
                   PrimVtx, cutOmegaPlus, 0, &ChiToPrimVtx);
  //Find Xi*-
  float cutXiStarMinus[3] = {-100.,3.,3.};
  FindTrackV0Decay(1003314, Particles, vLambdaPrim, vRTracks, vField, pdgNeg[3], idNegPrim[3],
                   PrimVtx, cutXiStarMinus, 1);
  //Find Xi*+
  float cutXiStarPlus[3] = {-100.,3.,3.};
  FindTrackV0Decay(-1003314, Particles, vLambdaBarPrim, vRTracks, vField, pdgPos[3], idPosPrim[3],
                   PrimVtx, cutXiStarPlus, 1);

  ExtrapolateToPV(vXiPrim,PrimVtx);
  ExtrapolateToPV(vXiBarPrim,PrimVtx);

  //Find Xi*0
  float cutXiStar0[3] = {-100.,3.,3.};
  FindTrackV0Decay(3324, Particles, vXiPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutXiStar0, 1, 0, &vXiStarPrim);
  //Find Xi*0 bar
  float cutXiBarStar0[3] = {-100.,3.,3.};
  FindTrackV0Decay(-3324, Particles, vXiBarPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutXiBarStar0, 1, 0, &vXiStarBarPrim);

  //Xi*- -> Xi- pi0
  float cutXiStarMinusXiPi[3] = {-100.,3.,3.};
  CombinePartPart(vPi0Prim, vXiPrim, Particles, PrimVtx, cutXiStarMinusXiPi, 1, 3314, 0);
  //Xi*+ -> Xi+ pi0
  float cutXiStarPlusXiPi[3] = {-100.,3.,3.};
  CombinePartPart(vPi0Prim, vXiBarPrim, Particles, PrimVtx, cutXiStarPlusXiPi, 1, -3314, 0);

  //Find Omega*-
  const float cutOmegaStar[2] = {-100., 3.};
  for(unsigned int iPart=0; iPart<vXiStarPrim.size(); iPart++)
    CombineTrackPart(vRTracks, vField, Particles, vXiStarPrim[iPart], pdgNeg[3], 
                     1003334, idNegPrim[3], cutOmegaStar, 0);
  //Find Omega*+
  for(unsigned int iPart=0; iPart<vXiStarBarPrim.size(); iPart++)
    CombineTrackPart(vRTracks, vField, Particles, vXiStarBarPrim[iPart], pdgPos[3],
                     -1003334, idPosPrim[3], cutOmegaStar, 0);
  // Find K*+
  float cutKStarPlus[3] = {-100.,3.,3.};
  FindTrackV0Decay(323, Particles, vK0sPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutKStarPlus, 1, 0);
  // Find K*-
  float cutKStarMinus[3] = {-100.,3.,3.};
  FindTrackV0Decay(-323, Particles, vK0sPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutKStarMinus, 1, 0);
  // Find Sigma*+
  float cutSigmaStarPlus[3] = {-100.,3.,3.};
  FindTrackV0Decay(3224, Particles, vLambdaPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutSigmaStarPlus, 1, 0);
  // Find Sigma*+ bar
  float cutSigmaStarPlusBar[3] = {-100.,3.,3.};
  FindTrackV0Decay(-3114, Particles, vLambdaBarPrim, vRTracks, vField, pdgPos[5], idPosPrim[5],
                   PrimVtx, cutSigmaStarPlusBar, 1, 0);
  // Find Sigma*-
  float cutSigmaStarMinus[3] = {-100.,3.,3.};
  FindTrackV0Decay(3114, Particles, vLambdaPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutSigmaStarMinus, 1, 0);
  // Find Sigma*- bar
  float cutSigmaStarMinusBar[3] = {-100.,3.,3.};
  FindTrackV0Decay(-3224, Particles, vLambdaBarPrim, vRTracks, vField, pdgNeg[5], idNegPrim[5],
                   PrimVtx, cutSigmaStarMinusBar, 1, 0);
  // Find H-dibarion
  vector<KFParticle> vHdibarion;
  float cutHdb[3] = {3.,3.,3.};
  for(unsigned short iL=0; iL < vLambdaSec.size(); iL++)
  {
    KFParticleSIMD vDaughters[2] = {KFParticleSIMD(),KFParticleSIMD(vLambdaSec[iL])};

    vector<int> daughterIds;
    for(unsigned int iD=0; iD<vLambdaSec[iL].DaughterIds().size(); iD++)
      daughterIds.push_back(vLambdaSec[iL].DaughterIds()[iD]);
    FindHyperons(3000, vDaughters, daughterIds, vLambdaSec, vHdibarion, PrimVtx, cutHdb, iL+1);
  }

  for(unsigned int iH=0; iH<vHdibarion.size(); iH++)
  {
    vHdibarion[iH].SetId(Particles.size());
    Particles.push_back(vHdibarion[iH]);
  }
  //                          chi2_prim  chi2_geo    z     pt  chi2_topo
  const float cutsD[8][8] = {{    6.,        3.,   0.04,  0.3,     3.},  //D0 -> pi+ K-
                             {    6.,        3.,   0.04,  0.3,     3.},  //D+ -> K- pi+ pi+
                             {    6.,        3.,   0.04,  0.3,     3.},  //D0 -> pi+ pi+ pi- K-
                             {    6.,        3.,   0.04,  0.3,     3.},  //Ds+ -> K- K+ pi+
                             {    6.,        3.,   0.04,  0.3,     3.},  //Lambdac -> pi+ K- p
                             {    6.,        3.,  -100.,  0.3,  -100.},  //D*0  -> D+ pi-
                             {    6.,        3.,  -100.,  0.3,  -100.},  //D*+  -> D0 pi+
                             {    6.,        3.,  -100.,  0.3,  -100.}}; //D*+4 -> D04 pi+

  const int DMesLambdcDaughterPDG[5] = { 211, -321, -211, 321, 2212 };
  const int DMesLambdcMotherPDG[8] = { 421, 411, 100421, 431, 4122, 10421, 10411, 20411 };
  vector<short>* DMesLambdcIdTrack[5] = {&idPosSec[5],
                                         &idNegSec[3],
                                         &idNegSec[5],
                                         &idPosSec[3],
                                         &idPosSec[4]};
  FindDMesLambdac(vRTracks, vField, Particles, DMesLambdcDaughterPDG, DMesLambdcMotherPDG,
                  DMesLambdcIdTrack, PrimVtx, cutsD, ChiToPrimVtx);

  const int DMesLambdcBarDaughterPDG[5] = { -211, 321, 211, -321, -2212 };
  const int DMesLambdcBarMotherPDG[8] = { -421, -411, -100421, -431, -4122, -10421, -10411, -20411 };
  vector<short>* DMesLambdcBarIdTrack[5] = {&idNegSec[5],
                                            &idPosSec[3],
                                            &idPosSec[5],
                                            &idNegSec[3],
                                            &idNegSec[4]};
  FindDMesLambdac(vRTracks, vField, Particles, DMesLambdcBarDaughterPDG, DMesLambdcBarMotherPDG,
                  DMesLambdcBarIdTrack, PrimVtx, cutsD, ChiToPrimVtx);
//  std::cout << "NPart  " << Particles.size() << std::endl;
}

void KFParticleFinder::ExtrapolateToPV(vector<KFParticle>& vParticles, KFParticleSIMD& PrimVtx)
{
  for(unsigned int iL=0; iL<vParticles.size(); iL += fvecLen)
  {
    KFParticle* parts[fvecLen];
    unsigned int nPart = vParticles.size();

    unsigned int nEntries = (iL + fvecLen < nPart) ? fvecLen : (nPart - iL);

    for(unsigned short iv=0; iv<nEntries; iv++)
      parts[iv] = &vParticles[iL+iv];


    KFParticleSIMD tmp(parts,nEntries);
    tmp.TransportToPoint(PrimVtx.Parameters());

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      tmp.GetKFParticle(vParticles[iL+iv], iv);
    }
  }
}

fvec KFParticleFinder::GetChi2BetweenParticles(KFParticleSIMD &p1, KFParticleSIMD &p2)
{
  const fvec& x1 = p1.GetX();
  const fvec& y1 = p1.GetY();
  const fvec& z1 = p1.GetZ();

  const fvec& x2 = p2.GetX();
  const fvec& y2 = p2.GetY();
  const fvec& z2 = p2.GetZ();

  const fvec dx = x1 - x2;
  const fvec dy = y1 - y2;
  const fvec dz = z1 - z2;

  const fvec& c0 = p1.GetCovariance(0) + p2.GetCovariance(0);
  const fvec& c1 = p1.GetCovariance(1) + p2.GetCovariance(1);
  const fvec& c2 = p1.GetCovariance(2) + p2.GetCovariance(2);
  const fvec& c3 = p1.GetCovariance(3) + p2.GetCovariance(3);
  const fvec& c4 = p1.GetCovariance(4) + p2.GetCovariance(4);
  const fvec& c5 = p1.GetCovariance(5) + p2.GetCovariance(5);

  const fvec r2 = dx*dx + dy*dy + dz*dz;
  const fvec err2 = c0*dx*dx + c2*dy*dy + c5*dz*dz + 2*( c1*dx*dy + c3*dx*dz + c4*dy*dz );

  return (r2*r2/err2);
}

void KFParticleFinder::Find2DaughterDecay(vector<CbmKFTrack>& vTracks,
                                          const vector<L1FieldRegion>& vField,
                                          vector<KFParticle>& Particles,
                                          const int DaughterNegPDG,
                                          const int DaughterPosPDG,
                                          const int MotherPDG,
                                          vector<short>& idNeg,
                                          vector<short>& idPos,
                                          KFParticleSIMD& PrimVtx,
                                          const float* cuts,
                                          bool isPrimary,
                                          vector<float>* vMotherTopoChi2Ndf,
                                          const float* secCuts,
                                          const float massMotherPDG,
                                          const float massMotherPDGSigma,
                                          vector<KFParticle>* vMotherPrim,
                                          vector<KFParticle>* vMotherSec )
{
  //* The function combines 2 tracks into a mother particle
  //* parameters: array of input tracks (vTracks), array with field approximation for the input tracks
  //* (vField), array of output particles (Particles), PDG hypothesis for tracks (DaughterNegPDG and DaughterPosPDG),
  //* PDG hypothesis of mother particles (MotherPDG), indexes of the first and second group of tracks (idNeg and idPos),
  //* a primary vertex (PrimVtx), 
  //* a set of cuts (cuts): 0 - doesn't used, 1 - cut on reconstructed Chi2/NDF, 2 - cut on l/dl;
  //* a flag (isPrimary), which indicates, that the moter particles decay in the primary vertex region, 
  //* if 1 - daughter tracks should be already transported to the primary vertex,
  //* an array (vMotherTopoChi2Ndf) to store the chi2/ndf of the particles with a production point constraint,
  //* a set of cuts (secCuts) for primary and secondary particles separation: 0 - cut on mass of the reconstructed particles:
  //* mass should be within secCuts[0] sigmas with respect to the PDG mass, 1 - cut on topo chi2/ndf, if chi2ndf_topo< secCuts[1]
  //* particle is considered as primary, if not - secondary, 2 - cut on l/dl of the reconstructed particle,
  //* pdg value of mass of the reconstructed particle (massMotherPDG) and expected width of the peak (massMotherPDGSigma),
  //* arrays to store selected according to secCuts primary (vMotherPrim) and secondary (vMotherSec) particles, if the pointers
  //* sre equal to 0, corresponding set of particles is not stored.

  //* examples of usage:
  //* 1) reconstruction of K0s from secondary tracks with selection and storage of primary K0s:
  //* Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[5], pdgPos[5], 310,
  //*                    idNegSec[5], idPosSec[5], PrimVtx, cuts[0], 0, &vK0sTopoChi2Ndf,
  //*                    SecCuts, massK0sPDG, 0.0022, &vK0sPrim, 0);
  //* 2) reconstruction of Lambda from secondary tracks with selection and storage of primary and secondary Lambdas:
  //*   Find2DaughterDecay(vRTracks, vField, Particles, pdgNeg[5], pdgPos[5], 310,
  //*                      idNegSec[5], idPosSec[5], PrimVtx, cuts[0], 0, &vK0sTopoChi2Ndf,
  //*                      SecCuts, massK0sPDG, 0.0022, &vK0sPrim, 0);
  //* 3) reconstruction of K*0 from primary tracks with no selection and storage of primary and secondary particles:
  //* Find2DaughterDecay(vRTracks, vField, Particles, pdgPos[3], pdgNeg[2], 313,
  //*                    idPosPrim[3], idNegPrim[2], PrimVtx, cuts[1], 1);

  const int NPositive = idPos.size();
  KFParticle mother_temp;

  KFParticleSIMD mother;
  mother.SetPDG( MotherPDG );
  KFParticleSIMD motherTopo;

  const short NPosVect = NPositive%fvecLen == 0 ? NPositive/fvecLen : NPositive/fvecLen+1;
  vector<KFParticleSIMD> posPart(NPosVect);

  // create particles from tracks
  CbmKFTrackInterface*  vvPos[fvecLen];

  int iPosVect = 0;
  for(unsigned short iTrP=0; iTrP < NPositive; iTrP += fvecLen)
  {
    const unsigned short NTracks = (iTrP + fvecLen < NPositive) ? fvecLen : (NPositive - iTrP);

    L1FieldRegion posField;
    for(unsigned short iv=0; iv<NTracks; iv++)
    {
      vvPos[iv] = &vTracks[idPos[iTrP+iv]];

      int entrSIMDPos = idPos[iTrP+iv] % fvecLen;
      int entrVecPos  = idPos[iTrP+iv] / fvecLen;
      posField.SetOneEntry(iv,vField[entrVecPos],entrSIMDPos);
    }

    posPart[iPosVect].Create(vvPos,NTracks,0,&DaughterPosPDG);
    posPart[iPosVect].SetField(posField);
    fvec posId;
    for(int iv=0; iv<NTracks; iv++)
      posId[iv] = idPos[iTrP+iv];
    posPart[iPosVect].SetId(posId);
    iPosVect++;
  }

  for(unsigned short iTrN=0; iTrN < idNeg.size(); iTrN++)
  {
    CbmKFTrack &kfTrackNeg = vTracks[idNeg[iTrN]];
    KFParticleSIMD vDaughters[2] = {KFParticleSIMD(kfTrackNeg,0,&DaughterNegPDG), KFParticleSIMD()};
    int entrSIMD = idNeg[iTrN] % fvecLen;
    int entrVec  = idNeg[iTrN] / fvecLen;
    vDaughters[0].SetField(vField[entrVec],1,entrSIMD);
    vDaughters[0].SetId(idNeg[iTrN]);

    for(unsigned short iTrP=0; iTrP < NPositive; iTrP += fvecLen)
    {
      const unsigned short NTracks = (iTrP + fvecLen < NPositive) ? fvecLen : (NPositive - iTrP);

      vDaughters[1] = posPart[iTrP/fvecLen];

      if(isPrimary)
      {
        fvec errGuess[3] = {100*sqrt(PrimVtx.CovarianceMatrix()[0]),
                            100*sqrt(PrimVtx.CovarianceMatrix()[2]),
                            100*sqrt(PrimVtx.CovarianceMatrix()[5])};
        mother.SetVtxGuess(PrimVtx.X(), PrimVtx.Y(), PrimVtx.Z());
        mother.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
        const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
        mother.Construct(vDaughtersPointer, 2, 0, -1, 0, 0);
      }
      else
      {
        const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
        mother.Construct(vDaughtersPointer, 2, 0);
      }

      if(vMotherTopoChi2Ndf)
      {
        motherTopo = mother;
        motherTopo.SetProductionVertex(PrimVtx);
      }

      for(int iv=0; iv<NTracks; iv++)
      {
        if(!finite(mother.GetChi2()[iv])) continue;
        if(!(mother.GetChi2()[iv] > 0.0f)) continue;
        if(!(mother.GetChi2()[iv]==mother.GetChi2()[iv])) continue;
        fvec l, dl, isParticleFromVertex;
        mother.GetDistanceToVertexLine(PrimVtx, l, dl, &isParticleFromVertex);
        if((l/dl)[iv] < cuts[2]) continue;
        if(isPrimary && ((l/dl)[iv]>3) ) continue;
        if((!isPrimary) && !(isParticleFromVertex[iv])) continue;
        if( mother.GetChi2()[iv]/mother.GetNDF()[iv] < cuts[1] )
        {
          mother.GetKFParticle(mother_temp, iv);
          mother_temp.SetId(Particles.size());
          Particles.push_back(mother_temp);
          float motherTopoChi2Ndf(0);
          if(vMotherTopoChi2Ndf)
            motherTopoChi2Ndf = motherTopo.GetChi2()[iv]/motherTopo.GetNDF()[iv];

          if(vMotherPrim)
          {
            Double_t mass, errMass;

            mother_temp.GetMass(mass, errMass);
            if( (fabs(mass - massMotherPDG)/massMotherPDGSigma) > secCuts[0] ) continue;
            if((l/dl)[iv] < secCuts[2]) continue;

            mother_temp.SetNonlinearMassConstraint(massMotherPDG);

            if( motherTopoChi2Ndf < secCuts[1] )
            {
              vMotherPrim->push_back(mother_temp);
            }
            else if(vMotherSec)
            {
              //if((l/dl)[iv] < secCuts[2]) continue;
              vMotherSec->push_back(mother_temp);
            }
          }
        }
      }
    }
  }
}

void KFParticleFinder::Find2DaughterDecay(vector<CbmKFTrack>& vTracks,
                                                const vector<L1FieldRegion>& vField,
                                                vector<KFParticle>& Particles,
                                                const int DaughterNegPDG,
                                                const int DaughterPosPDG,
                                                const int MotherPDG,
                                                vector<short>& idNeg,
                                                vector<short>& idPos,
                                                KFParticleSIMD& PrimVtx,
                                                const float* cuts,
                                                bool isPrimary,
                                                const float PtCut,
                                                const float Chi2PrimCut,
                                                vector<float>* ChiToPrimVtx,
                                                const float* PCut)
{
  vector<short> idPosPt;
  vector<short> idNegPt;

  for(unsigned int iEl=0; iEl<idPos.size(); iEl++)
  {
    CbmKFTrack& kfTrack = vTracks[idPos[iEl]];
    const float tx = kfTrack.GetTrack()[2];
    const float ty = kfTrack.GetTrack()[3];
    const float p = fabs(1/kfTrack.GetTrack()[4]);
    const float t2 = tx*tx+ty*ty;
    float pt = p*(sqrt(t2/(1+t2)));
    if(PCut)
      if(p < *PCut) continue;
    if(pt<PtCut) continue;
    if(Chi2PrimCut > -1)
      if(ChiToPrimVtx->at(idPos[iEl]) < Chi2PrimCut) continue;
    idPosPt.push_back(idPos[iEl]);
  }

  for(unsigned int iEl=0; iEl<idNeg.size(); iEl++)
  {
    CbmKFTrack& kfTrack = vTracks[idNeg[iEl]];
    const float tx = kfTrack.GetTrack()[2];
    const float ty = kfTrack.GetTrack()[3];
    const float p = fabs(1/kfTrack.GetTrack()[4]);
    const float t2 = tx*tx+ty*ty;
    float pt = p*(sqrt(t2/(1+t2)));
    if(PCut)
      if(p < *PCut) continue;
    if(pt<PtCut) continue;
    if(Chi2PrimCut > -1)
      if(ChiToPrimVtx->at(idNeg[iEl]) < Chi2PrimCut) continue;
    idNegPt.push_back(idNeg[iEl]);
  }

  Find2DaughterDecay(vTracks, vField,
                     Particles, DaughterNegPDG, DaughterPosPDG, MotherPDG,
                     idNegPt, idPosPt,
                     PrimVtx, cuts, isPrimary);
}

void KFParticleFinder::FindTrackV0Decay(const int MotherPDG,
                                              vector<KFParticle>& Particles,
                                              vector<KFParticle>& vV0,
                                              vector<CbmKFTrack>& vTracks,
                                              const vector<L1FieldRegion>& vField,
                                              const int DaughterPDG,
                                              vector<short>& idTrack,
                                              KFParticleSIMD& PrimVtx,
                                              const float* cuts,
                                              bool isPrimary,
                                              vector<float>* ChiToPrimVtx,
                                              vector<KFParticle>* vHyperonPrim,
                                              float hyperonPrimMass,
                                              float hyperonPrimMassErr,
                                              vector<KFParticle>* vHyperonSec)
{
  KFParticle hyperon_temp;
  KFParticleSIMD hyperon;
  hyperon.SetPDG( MotherPDG );

  for(unsigned short iV0=0; iV0 < vV0.size(); iV0++)
  {
    unsigned short nElements = 0;
    KFParticleSIMD vDaughters[2]= {KFParticleSIMD(vV0[iV0]),KFParticleSIMD()};

    CbmKFTrackInterface*  vvTr[fvecLen];
    L1FieldRegion field;
    fvec trId;

    for(unsigned short iTr=0; iTr < idTrack.size(); iTr++)
    {
      bool ok = 1;
      if(ChiToPrimVtx)
        if( (ChiToPrimVtx->at(idTrack[iTr]) < 7) ) ok=0; //TODO 7 for Omega

      if(ok)
      {
        trId[nElements] = idTrack[iTr];
        vvTr[nElements] = &vTracks[idTrack[iTr]];

        int entrSIMD = idTrack[iTr] % fvecLen;
        int entrVec  = idTrack[iTr] / fvecLen;
        field.SetOneEntry(nElements,vField[entrVec],entrSIMD);

        nElements++;
      }
      else if( (iTr != idTrack.size()-1) ) continue;

      if( (nElements == fvecLen) || ((iTr == idTrack.size()-1)&&(nElements>0)) )
      {
        vDaughters[1].Create(vvTr,nElements,0,&DaughterPDG);
        vDaughters[1].SetField(field);
        vDaughters[1].SetId(trId);

        if(isPrimary)
        {
          fvec errGuess[3] = {100*sqrt(PrimVtx.CovarianceMatrix()[0]),
                              100*sqrt(PrimVtx.CovarianceMatrix()[2]),
                              100*sqrt(PrimVtx.CovarianceMatrix()[5])};
          hyperon.SetVtxGuess(PrimVtx.X(), PrimVtx.Y(), PrimVtx.Z());
          hyperon.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
          const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
          hyperon.Construct(vDaughtersPointer, 2, 0, -1, 0, 0);
        }
        else
        {
          const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
          hyperon.Construct(vDaughtersPointer, 2, 0);
        }

        KFParticleSIMD hyperonTopo(hyperon);
        hyperonTopo.SetProductionVertex(PrimVtx);

        for(unsigned int iv=0; iv<nElements; iv++)
        {
          bool isSameTrack = 0;
          for(unsigned short iD=0; iD<vV0[iV0].DaughterIds().size(); iD++)
            if(vV0[iV0].DaughterIds()[iD] == trId[iv]) isSameTrack=1;

          if(isSameTrack) continue;
          if(!finite(hyperon.GetChi2()[iv])) continue;
          if(!(hyperon.GetChi2()[iv] > 0.0f)) continue;
          if(!(hyperon.GetChi2()[iv]==hyperon.GetChi2()[iv])) continue;

          fvec l, dl, isParticleFromVertex;
          hyperon.GetDistanceToVertexLine(PrimVtx, l, dl, &isParticleFromVertex);
          if(!(isParticleFromVertex[iv])) continue;


          if(((l/dl)[iv] < cuts[0]) ) continue;
          if(!isPrimary)
          {
            fvec l1, dl1;
            vDaughters[0].GetDistanceToVertexLine(hyperon, l1, dl1, &isParticleFromVertex);
            if(!(isParticleFromVertex[iv])) continue;
          }

          if(hyperonTopo.GetChi2()[iv]/hyperonTopo.GetNDF()[iv] > cuts[1] ) continue;

          if( hyperon.GetChi2()[iv]/hyperon.GetNDF()[iv] > cuts[2] ) continue;
          hyperon.GetKFParticle(hyperon_temp, iv);

          hyperon_temp.SetId(Particles.size());
          Particles.push_back(hyperon_temp);

          if(vHyperonPrim)
          {
            Double_t mass, errMass;

            hyperon_temp.GetMass(mass, errMass);
            hyperon_temp.SetNonlinearMassConstraint(hyperonPrimMass);

            if( (fabs(mass - hyperonPrimMass)/hyperonPrimMassErr) <= 3 )
              vHyperonPrim->push_back(hyperon_temp);
            else
              if( vHyperonSec )
                vHyperonSec->push_back(hyperon_temp);
          }
        }
        nElements=0;
      }
    }
  }
}

void KFParticleFinder::FindHyperons(int PDG,
                                          KFParticleSIMD vDaughters[2],
                                          vector<int>& daughterIds,
                                          vector<KFParticle>& vLambdaSec,
                                          vector<KFParticle>& vHyperon,
                                          KFParticleSIMD& PrimVtx,
                                          const float *cuts,
                                          int startIndex)
{
  KFParticle* lambdas[fvecLen];
  int nLambdasSec = vLambdaSec.size();

  for(unsigned short iL=startIndex; iL < vLambdaSec.size(); iL += fvecLen)
  {
    unsigned int nEntries = (iL + fvecLen < nLambdasSec) ? fvecLen : (nLambdasSec - iL);

    for(unsigned short iv=0; iv<nEntries; iv++)
      lambdas[iv] = &vLambdaSec[iL+iv];

    vDaughters[0] = KFParticleSIMD(lambdas,nEntries);

    KFParticleSIMD hyperon;

    hyperon.SetPDG( PDG );
    const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
    hyperon.Construct(vDaughtersPointer, 2, 0);

    KFParticleSIMD hyperonTopo(hyperon);
    hyperonTopo.SetProductionVertex(PrimVtx);

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      bool isSameTrack = 0;
      for(unsigned short iD=0; iD<lambdas[iv]->DaughterIds().size(); iD++)
        for(unsigned short iD0=0; iD0<daughterIds.size(); iD0++)
          if(lambdas[iv]->DaughterIds()[iD] == daughterIds[iD0]) isSameTrack=1;

      if(isSameTrack) continue;
       if(!finite(hyperon.GetChi2()[iv])) continue;
      if(!(hyperon.GetChi2()[iv] > 0.0f)) continue;
      if(!(hyperon.GetChi2()[iv]==hyperon.GetChi2()[iv])) continue;

      fvec l, dl, isParticleFromVertex;
      fvec l1, dl1, l2, dl2;
      hyperon.GetDistanceToVertexLine(PrimVtx, l, dl, &isParticleFromVertex);
      vDaughters[0].GetDistanceToVertexLine(hyperon, l1, dl1);
      vDaughters[1].GetDistanceToVertexLine(hyperon, l2, dl2);
      if(!(isParticleFromVertex[iv])) continue;

      if(((l/dl)[iv] < cuts[0]) ) continue;
      if(((l1 - l)[iv] < 0) || ((l2 - l)[iv] < 0)) continue;

      if(hyperonTopo.GetChi2()[iv]/hyperonTopo.GetNDF()[iv] > cuts[1] ) continue;

      if( hyperon.GetChi2()[iv]/hyperon.GetNDF()[iv] > cuts[2] ) continue;
      KFParticle hyperon_temp;
      hyperon.GetKFParticle(hyperon_temp, iv);
      vHyperon.push_back(hyperon_temp);
    }
  }
}

void KFParticleFinder::FindDMesLambdac(vector<CbmKFTrack>& vTracks,
                                             const vector<L1FieldRegion>& vField,
                                             vector<KFParticle>& Particles,
                                             const int DaughterPDG[5], //pi, K_b, pi_b, K, p
                                             const int MotherPDG[8], //D0, D+, D0_4, Ds+, Lc
                                             vector<short>* idTrack[5], //pi, K_b, pi_b, K, p
                                             KFParticleSIMD& PrimVtx,
                                             const float cuts[8][8],
                                             vector<float> ChiToPrimVtx)
{
  vector<short> id[5]; //pi, K_b, pi_b, K, p
  for(int iId=0; iId<5; iId++)
  {
    for(unsigned int iTr=0; iTr<idTrack[iId]->size(); iTr++)
    {
      CbmKFTrack& kfTrack = vTracks[idTrack[iId]->at(iTr)];
      const float tx = kfTrack.GetTrack()[2];
      const float ty = kfTrack.GetTrack()[3];
      const float p = fabs(1/kfTrack.GetTrack()[4]);
      const float t2 = tx*tx+ty*ty;
      float pt = p*(sqrt(t2/(1+t2)));
      if(pt<cuts[0][3]) continue;
      if(ChiToPrimVtx[idTrack[iId]->at(iTr)] < cuts[0][0]) continue;
      id[iId].push_back(idTrack[iId]->at(iTr));
    }
  }

  vector<KFParticle> kpi;
  vector<KFParticle> kpipi;
  vector<KFParticle> kpipipi;
  vector<KFParticle> kpik;
  vector<KFParticle> kpip;

  const float cutskpi[3] = {3., 3., -100.};
  Find2DaughterDecay(vTracks, vField, kpi, DaughterPDG[0], DaughterPDG[1], MotherPDG[0],
                     id[0], id[1], PrimVtx, cutskpi, 0);

  for(unsigned int iKPi=0; iKPi<kpi.size(); iKPi++)
  {
    unsigned short startPiIndex = kpi[iKPi].DaughterIds()[0]+1;

    CombineTrackPart(vTracks,vField,kpipi,kpi[iKPi],DaughterPDG[0], MotherPDG[1], id[0], cuts[1], startPiIndex, 1);
    CombineTrackPart(vTracks,vField,kpik ,kpi[iKPi],DaughterPDG[3], MotherPDG[3], id[3], cuts[3], 0, 1);
    CombineTrackPart(vTracks,vField,kpip ,kpi[iKPi],DaughterPDG[4], MotherPDG[4], id[4], cuts[4], 0, 1);
  }
  for(unsigned int iKPiPi=0; iKPiPi<kpipi.size(); iKPiPi++)
    CombineTrackPart(vTracks,vField,kpipipi,kpipi[iKPiPi],DaughterPDG[2], MotherPDG[2], id[2], cuts[2], 0, 1);

  SelectParticleCandidates(Particles, kpi,     PrimVtx, cuts[0]);
  SelectParticleCandidates(Particles, kpipi,   PrimVtx, cuts[1]);
  SelectParticleCandidates(Particles, kpipipi, PrimVtx, cuts[2]);
  SelectParticleCandidates(Particles, kpik,    PrimVtx, cuts[3]);
  SelectParticleCandidates(Particles, kpip,    PrimVtx, cuts[4]);

  vector<KFParticle> d0pi;
  vector<KFParticle> d2pi;
  vector<KFParticle> d4pi;

  for(unsigned int iKPiPi=0; iKPiPi<kpipi.size(); iKPiPi++)
    CombineTrackPart(vTracks,vField,d0pi,kpipi[iKPiPi],DaughterPDG[2], MotherPDG[5], id[2], cuts[5], 0, 0);

  for(unsigned int iKPi=0; iKPi<kpi.size(); iKPi++)
    CombineTrackPart(vTracks,vField,d2pi,kpi[iKPi],DaughterPDG[0], MotherPDG[6], id[0], cuts[6], 0, 0);

  for(unsigned int iKPiPiPi=0; iKPiPiPi<kpipipi.size(); iKPiPiPi++)
    CombineTrackPart(vTracks,vField,d4pi,kpipipi[iKPiPiPi],DaughterPDG[0], MotherPDG[7], id[0], cuts[7], 0, 0);

  SelectParticleCandidates(Particles, d0pi, PrimVtx, cuts[5]);
  SelectParticleCandidates(Particles, d2pi, PrimVtx, cuts[6]);
  SelectParticleCandidates(Particles, d4pi, PrimVtx, cuts[7]);
}

void KFParticleFinder::CombineTrackPart(vector<CbmKFTrack>& vTracks,
                                              const vector<L1FieldRegion>& vField,
                                              vector<KFParticle>& Particles,
                                              KFParticle& part,
                                              const int DaughterPDG,
                                              const int MotherPDG,
                                              vector<short>& id,
                                              const float* cuts,
                                              const unsigned short startIndex,
                                              const bool IsSamePart)
{
  KFParticleSIMD vDaughters[2] = {KFParticleSIMD(part),KFParticleSIMD()};

  CbmKFTrackInterface*  vTr[fvecLen];

  fvec vtxGuess[3] = {part.GetX(), part.GetY(), part.GetZ()};
  fvec errGuess[3] = {fvec(10.f*sqrt(part.CovarianceMatrix()[0])),
                      fvec(10.f*sqrt(part.CovarianceMatrix()[2])),
                      fvec(10.f*sqrt(part.CovarianceMatrix()[5]))};

  const unsigned short NTr = id.size();
  for(unsigned short iTr=startIndex; iTr < NTr; iTr += fvecLen)
  {
    const unsigned short NTracks = (iTr + fvecLen < NTr) ? fvecLen : (NTr - iTr);

    L1FieldRegion trField;
    for(unsigned short iv=0; iv<NTracks; iv++)
    {
      vTr[iv] = &vTracks[id[iTr+iv]];

      int entrSIMDPos = id[iTr+iv] % fvecLen;
      int entrVecPos  = id[iTr+iv] / fvecLen;
      trField.SetOneEntry(iv,vField[entrVecPos],entrSIMDPos);
    }

    vDaughters[1].Create(vTr,NTracks,0,&DaughterPDG);
    vDaughters[1].SetField(trField);
    fvec trId;
    for(int iv=0; iv<NTracks; iv++)
      trId[iv] = id[iTr+iv];
    vDaughters[1].SetId(trId);

    KFParticleSIMD mother;
    mother.SetPDG( MotherPDG );
    if(IsSamePart)
    {
      mother.SetVtxGuess(vtxGuess[0], vtxGuess[1], vtxGuess[2]);
      mother.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
    }
    const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
    mother.Construct(vDaughtersPointer, 2, 0);

    for(int iv=0; iv<NTracks; iv++)
    {
      if(!finite(mother.GetChi2()[iv])) continue;
      if(!(mother.GetChi2()[iv] > 0.0f)) continue;
      if(!(mother.GetChi2()[iv]==mother.GetChi2()[iv])) continue;

      if( mother.GetChi2()[iv]/mother.GetNDF()[iv] > cuts[1] ) continue;

      bool isSameTrack = 0;
      for(unsigned short iD=0; iD<part.DaughterIds().size(); iD++)
        if(part.DaughterIds()[iD] == id[iTr+iv]) isSameTrack=1;
      if(isSameTrack) continue;

      KFParticle mother_temp;
      mother.GetKFParticle(mother_temp, iv);
      mother_temp.CleanDaughtersId();
      mother_temp.AddDaughter(id[iTr+iv]);
      for(unsigned short iD=0; iD<part.DaughterIds().size(); iD++)
        mother_temp.AddDaughter(part.DaughterIds()[iD]);
      Particles.push_back(mother_temp);
    }
  }
}

void KFParticleFinder::CombinePartPart(vector<KFParticle>& particles1,
                                       vector<KFParticle>& particles2,
                                       vector<KFParticle>& Particles,
                                       KFParticleSIMD& PrimVtx,
                                       const float* cuts,
                                       bool isPrimary,
                                       const int MotherPDG,
                                       bool isSameInputPart,
                                       vector<KFParticle>* vMotherPrim,
                                       vector<KFParticle>* vMotherSec,
                                       float* secCuts,
                                       float massMotherPDG,
                                       float massMotherPDGSigma)
{
  KFParticle mother_temp;
  KFParticleSIMD mother;
  KFParticleSIMD motherTopo;
  mother.SetPDG( MotherPDG );

  KFParticle* tmpPart2[fvecLen];
  int nPart2 = particles2.size();

  for(unsigned short iP1=0; iP1 < particles1.size(); iP1++)
  {
    KFParticleSIMD vDaughters[2] = {KFParticleSIMD(particles1[iP1]), KFParticleSIMD()};

    unsigned short startIndex=0;
    if(isSameInputPart) startIndex=iP1+1;
    for(unsigned short iP2=startIndex; iP2 < nPart2; iP2 += fvecLen)
    {
      unsigned int nEntries = (iP2 + fvecLen < nPart2) ? fvecLen : (nPart2 - iP2);

      for(unsigned short iv=0; iv<nEntries; iv++)
        tmpPart2[iv] = &particles2[iP2+iv];

      vDaughters[1] = KFParticleSIMD(tmpPart2,nEntries);

      if(isPrimary)
      {
        fvec errGuess[3] = {100*sqrt(PrimVtx.CovarianceMatrix()[0]),
                            100*sqrt(PrimVtx.CovarianceMatrix()[2]),
                            100*sqrt(PrimVtx.CovarianceMatrix()[5])};
        mother.SetVtxGuess(PrimVtx.X(), PrimVtx.Y(), PrimVtx.Z());
        mother.SetVtxErrGuess(errGuess[0], errGuess[1], errGuess[2]);
        const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
        mother.Construct(vDaughtersPointer, 2, 0, -1, 0, 0);
      }
      else
      {
        const KFParticleSIMD* vDaughtersPointer[2] = {&vDaughters[0], &vDaughters[1]};
        mother.Construct(vDaughtersPointer, 2, 0);
      }

      motherTopo = mother;
      motherTopo.SetProductionVertex(PrimVtx);

      for(unsigned short iv=0; iv<nEntries; iv++)
      {
        bool isSameTrack = 0;
        for(unsigned short iD=0; iD<tmpPart2[iv]->DaughterIds().size(); iD++)
          for(unsigned short iD0=0; iD0<particles1[iP1].DaughterIds().size(); iD0++)
            if(tmpPart2[iv]->DaughterIds()[iD] == particles1[iP1].DaughterIds()[iD0]) isSameTrack=1;

        if(isSameTrack) continue;
        if(!finite(mother.GetChi2()[iv])) continue;
        if(!(mother.GetChi2()[iv] > 0.0f)) continue;
        if(!(mother.GetChi2()[iv]==mother.GetChi2()[iv])) continue;

        fvec l, dl, isParticleFromVertex;
        mother.GetDistanceToVertexLine(PrimVtx, l, dl, &isParticleFromVertex);
        if(!(isParticleFromVertex[iv])) continue;

        if(((l/dl)[iv] < cuts[0]) ) continue;
        if(!isPrimary)
        {
          fvec l1, dl1;
          vDaughters[0].GetDistanceToVertexLine(mother, l1, dl1, &isParticleFromVertex);
          if(!(isParticleFromVertex[iv])) continue;
        }

        float motherTopoChi2Ndf = motherTopo.GetChi2()[iv]/motherTopo.GetNDF()[iv];
        if(motherTopo.GetChi2()[iv]/motherTopo.GetNDF()[iv] > cuts[1] ) continue;

        if( mother.GetChi2()[iv]/mother.GetNDF()[iv] > cuts[2] ) continue;
        mother.GetKFParticle(mother_temp, iv);

        mother_temp.SetId(Particles.size());
        Particles.push_back(mother_temp);

        if(vMotherPrim)
        {
          Double_t mass, errMass;

          mother_temp.GetMass(mass, errMass);
          if( (fabs(mass - massMotherPDG)/massMotherPDGSigma) > secCuts[0] ) continue;

          mother_temp.SetNonlinearMassConstraint(massMotherPDG);

          if( motherTopoChi2Ndf < secCuts[1] )
          {
            vMotherPrim->push_back(mother_temp);
          }
          else if(vMotherSec)
          {
             vMotherSec->push_back(mother_temp);
          }
        }
      }
    }
  }
}

void KFParticleFinder::SelectParticleCandidates(vector<KFParticle>& Particles,
                                                      vector<KFParticle>& vCandidates,
                                                      KFParticleSIMD& PrimVtx,
                                                      const float cuts[5])
{
  KFParticle* cand[fvecLen];
  int nCand = vCandidates.size();

  for(unsigned short iC=0; iC < nCand; iC += fvecLen)
  {
    unsigned int nEntries = (iC + fvecLen < nCand) ? fvecLen : (nCand - iC);

    for(unsigned short iv=0; iv<nEntries; iv++)
      cand[iv] = &vCandidates[iC+iv];

    KFParticleSIMD candTopo(cand,nEntries);

    candTopo.SetProductionVertex(PrimVtx);

    for(unsigned int iv=0; iv<nEntries; iv++)
    {
      if(!finite(candTopo.GetChi2()[iv])) continue;
      if(!(candTopo.GetChi2()[iv] > 0.0f)) continue;
      if(!(candTopo.GetChi2()[iv]==candTopo.GetChi2()[iv])) continue;

      fvec l, dl, isParticleFromVertex;
      candTopo.GetDistanceToVertexLine(PrimVtx, l, dl, &isParticleFromVertex);
      if(!(isParticleFromVertex[iv])) continue;
      if(((l/dl)[iv] < cuts[2]) ) continue;

      if(candTopo.GetChi2()[iv]/candTopo.GetNDF()[iv] > cuts[4] ) continue;

      Particles.push_back(vCandidates[iC+iv]);
    }
  }
}



void KFParticleFinder::ConstructPVT(vector<CbmKFTrack>& vRTracks)
{

}

#undef cnst
