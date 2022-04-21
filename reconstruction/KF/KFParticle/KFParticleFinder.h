#ifndef KFParticleFinder_h
#define KFParticleFinder_h

#include "KFParticle.h"
#include "KFParticleSIMD.h"
#include "CbmKFTrackInterface.h"
#include "CbmKFTrack.h"

#include <vector>

class L1FieldRegion;
class KFParticleSIMD;
class CbmL1Track;
class CbmStsTrack;

class KFParticleFinder
{
 public:

  KFParticleFinder();
  ~KFParticleFinder();

  /// Find particles with 2-body decay channel from input tracks vRTracks with primary vertex PrimVtx:
  /// 1. K0s->pi+ pi-
  /// 2. Lambda->p pi-
  /// All particles are put into the Particles array. 3 cuts for each particle are required.
  /// First index in the cuts array sets a particle number (see table above), second index - a cut number:
  /// cut[0][0] - chi to a primary vertex of a track (sqare root from a normalized on a total error of 
  ///             the track and the vertex distance between the track and the primary vertex), only
  ///             element cut[0][0] is used to select tracks, all other elements cut[*][0] are not used;
  /// cut[*][1] - chi2/ndf of the reconstructed particle;
  /// cut[*][2] - z coordinate of the reconstructed particle.
  /// cut[*][3] - chi2/ndf of the reconstructed particle fitted to the PV;

    static void FindParticles(std::vector<CbmKFTrack>& vRTracks, std::vector<float>& ChiToPrimVtx,
                                           std::vector<L1FieldRegion>& vField, std::vector<KFParticle>& Particles,
                               KFParticleSIMD& PrimVtx, const std::vector<int>& vTrackPDG, const float cuts[2][3] = DefaultCuts);

    static void ExtrapolateToPV(std::vector<KFParticle>& vParticles, KFParticleSIMD& PrimVtx);
    static fvec GetChi2BetweenParticles(KFParticleSIMD &p1, KFParticleSIMD &p2);

    static void Find2DaughterDecay(std::vector<CbmKFTrack>& vTracks,
                                   const std::vector<L1FieldRegion>& vField,
                                   std::vector<KFParticle>& Particles,
                                   const int DaughterNegPDG,
                                   const int DaughterPosPDG,
                                   const int MotherPDG,
                                   std::vector<short>& idNeg,
                                   std::vector<short>& idPos,
                                   KFParticleSIMD& PrimVtx,
                                   const float* cuts = 0,
                                   bool isPrimary = 0,
                                   std::vector<float>* vMotherTopoChi2Ndf = 0,
                                   const float* secCuts = 0,
                                   const float massMotherPDG = 0,
                                   const float massMotherPDGSigma = 0,
                                   std::vector<KFParticle>* vMotherPrim = 0,
                                   std::vector<KFParticle>* vMotherSec = 0);

    static void Find2DaughterDecay(std::vector<CbmKFTrack>& vTracks,
                                   const std::vector<L1FieldRegion>& vField,
                                   std::vector<KFParticle>& Particles,
                                   const int DaughterNegPDG,
                                   const int DaughterPosPDG,
                                   const int MotherPDG,
                                   std::vector<short>& idNeg,
                                   std::vector<short>& idPos,
                                   KFParticleSIMD& PrimVtx,
                                   const float* cuts,
                                   bool isPrimary,
                                   const float PtCut,
                                   const float Chi2PrimCut = -100.f,
                                   std::vector<float>* ChiToPrimVtx = 0,
                                   const float* PCut = 0);

  static void FindTrackV0Decay(const int MotherPDG,
                               std::vector<KFParticle>& Particles,
                               std::vector<KFParticle>& vV0,
                               std::vector<CbmKFTrack>& vTracks,
                               const std::vector<L1FieldRegion>& field,
                               const int DaughterPDG,
                               std::vector<short>& idTrack,
                               KFParticleSIMD& PrimVtx,
                               const float* cuts = 0,
                               bool isPrimary = 0,
                               std::vector<float>* ChiToPrimVtx = 0,
                               std::vector<KFParticle>* vHyperonPrim = 0,
                               float hyperonPrimMass = 0,
                               float hyperonPrimMassErr = 0,
                               std::vector<KFParticle>* vHyperonSec = 0);

  static void FindHyperons(int PDG,
                           KFParticleSIMD vDaughters[2],
                           std::vector<int>& daughterIds,
                           std::vector<KFParticle>& vLambdaSec,
                           std::vector<KFParticle>& vHyperon,
                           KFParticleSIMD& PrimVtx,
                           const float *cuts = 0,
                           int startIndex=0);

  static void CombinePartPart(std::vector<KFParticle>& particles1,
                              std::vector<KFParticle>& particles2,
                              std::vector<KFParticle>& Particles,
                              KFParticleSIMD& PrimVtx,
                              const float* cuts=0,
                              bool isPrimary=0,
                              const int MotherPDG=0,
                              bool isSameInputPart=0,
                              std::vector<KFParticle>* vMotherPrim=0,
                              std::vector<KFParticle>* vMotherSec=0,
                              float* SecCuts=0,
                              float massMotherPdg=0,
                              float massMotherPdgSigma=0);

    static void FindDMesLambdac(std::vector<CbmKFTrack>& vTracks,
                                const std::vector<L1FieldRegion>& vField,
                                std::vector<KFParticle>& Particles,
                                const int DaughterPDG[5], //pi, K_b, pi_b, K, p
                                const int MotherPDG[8],
                                std::vector<short>* idTrack[5], //pi, K_b, pi_b, K, p
                                KFParticleSIMD& PrimVtx,
                                const float cuts[8][8],
                                std::vector<float> ChiToPrimVtx);

    static void CombineTrackPart(std::vector<CbmKFTrack>& vTracks,
                                 const std::vector<L1FieldRegion>& vField,
                                 std::vector<KFParticle>& Particles,
                                 KFParticle& part,
                                 const int DaughterPDG,
                                 const int MotherPDG,
                                 std::vector<short>& id,
                                 const float* cuts,
                                 const unsigned short startIndex = 0,
                                 const bool IsSamePart = 0);

  static void SelectParticleCandidates(std::vector<KFParticle>& Particles,
                                       std::vector<KFParticle>& vCandidates,
                                       KFParticleSIMD& PrimVtx,
                                       const float cuts[5]);

  void ConstructPVT(std::vector<CbmKFTrack>& vRTracks);

 private:

  static const float DefaultCuts[2][3];
};

#endif /* !KFParticleFinder_h */

