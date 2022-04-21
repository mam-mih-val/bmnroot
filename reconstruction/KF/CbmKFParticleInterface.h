#ifndef CbmKFParticleInterface_h
#define CbmKFParticleInterface_h

#include "CbmKFParticle.h"
#include "CbmKFParticle_simd.h"
#include "CbmKFTrackInterface.h"
#include "CbmKFTrack.h"

#include "CbmKFVertexInterface.h"
#include "CbmKFVertex.h"

class L1FieldRegion;
class CbmL1Track;
class CbmStsTrack;

class CbmKFParticleInterface
{
 public:

  CbmKFParticleInterface();
  CbmKFParticleInterface(CbmKFTrackInterface* Track[]);
  CbmKFParticleInterface(CbmKFParticle* part[], const int nPart = 0);
  ~CbmKFParticleInterface();

  CbmKFParticleInterface(const CbmKFParticleInterface& c);
  CbmKFParticleInterface operator=(const CbmKFParticleInterface& c);

  void SetField(const L1FieldRegion &field, bool isOneEntry=0, const int iVec=0) { KFPart.SetField(field, isOneEntry, iVec);}

  void Construct(CbmKFTrackInterface* vDaughters[][fvecLen], int NDaughters, CbmKFVertexInterface *Parent = 0, float Mass=-1, float CutChi2=-1);
  void Construct(CbmKFParticle_simd  vDaughters[],          int NDaughters, CbmKFVertexInterface *Parent = 0, float Mass=-1, float CutChi2=-1, bool isAtVtxGuess = 0);

  void TransportToProductionVertex();
  void TransportToDecayVertex();

  void Extrapolate( CbmKFParticle_simd*  Particle, fvec r0[], fvec T );
  void Extrapolate( fvec r0[], fvec T );
  void ExtrapolateLine( CbmKFParticle_simd*  Particle, fvec r0[], fvec T );

  fvec GetDStoPoint( const CbmKFParticle_simd&  Particle, const fvec xyz[] ) const;
  fvec GetDStoPoint( const fvec xyz[] ) const;

  void SetVtxGuess(fvec &x, fvec &y, fvec &z);
  void SetVtxErrGuess(fvec &d_x, fvec &d_y, fvec &d_z);

  //* Simple accessors 
  fvec GetX    ()  const { return KFPart.r[0]; }
  fvec GetY    ()  const { return KFPart.r[1]; }
  fvec GetZ    ()  const { return KFPart.r[2]; }
  fvec GetPx   ()  const { return KFPart.r[3]; }
  fvec GetPy   ()  const { return KFPart.r[4]; }
  fvec GetPz   ()  const { return KFPart.r[5]; }
  fvec GetE    ()  const { return KFPart.r[6]; }
  fvec GetS    ()  const { return KFPart.r[7]; }
  fvec GetQ    ()  const { return KFPart.GetQ();    }
  fvec GetChi2 ()  const { return KFPart.GetChi2(); }
  fvec GetNDF  ()  const { return KFPart.GetNDF();  }
  fvec *GetParameters()  { return KFPart.GetParameters(); }
  const fvec *GetParameters() const  { return KFPart.GetParameters(); }
  fvec *GetCovMatrix()   { return KFPart.GetCovMatrix(); }

  fvec GetParameter ( Int_t i )          const { return KFPart.r[i]; }
  fvec GetCovariance( Int_t i )          const { return KFPart.C[i]; }
  fvec GetCovariance( Int_t i, Int_t j ) const { return KFPart.C[IJ(i,j)]; }

  //*
  //*  MODIFIERS
  //*

  fvec & rX    () { return KFPart.rX(); }
  fvec & rY    () { return KFPart.rY(); }
  fvec & rZ    () { return KFPart.rZ(); }
  fvec & rPx   () { return KFPart.rPx();}
  fvec & rPy   () { return KFPart.rPy();}
  fvec & rPz   () { return KFPart.rPz();}
  fvec & rE    () { return KFPart.rE(); }
  fvec & rS    () { return KFPart.rS(); }
  fvec & rQ    () { return KFPart.rQ(); }
  fvec & rChi2 () { return KFPart.rChi2();}
  fvec & rNDF  () { return KFPart.rNDF();}

  fvec & rParameter ( Int_t i )        { return KFPart.r[i];}
  fvec & rCovariance( Int_t i )        { return KFPart.C[i];}
  fvec & rCovariance( Int_t i, Int_t j ) { return KFPart.C[IJ(i,j)];}

  void GetMomentum( fvec &P, fvec &Error );
  void GetMass( fvec &M, fvec &Error );

  fvec GetTx    ()  const { return KFPart.GetTx(); }
  fvec GetTy    ()  const { return KFPart.GetTy(); }

  void MeasureMass(CbmKFParticle_simd*  Particle, fvec r0[], fvec Mass );
  void MeasureProductionVertex(CbmKFParticle_simd*  Particle, fvec r0[], CbmKFVertexInterface *Parent);
  void Convert(CbmKFParticle_simd*  Particle, fvec r0[], bool ToProduction );
  void multQSQt( const fvec Q[], fvec S[] );

  void multQSQt1( const fvec J[11], fvec S[] );

  void GetKFVertex( CbmKFVertex *vtx);
  void GetKFParticle( CbmKFParticle &Part, int iPart = 0);
  void GetKFParticle( CbmKFParticle *Part, int nPart = 0);
  CbmKFVertex GetKFVertexJ(int j);

  void GetKFVertexJ(int j, CbmKFVertex *vtx);

  void SetPDG ( fvec pdg ) { KFPart.SetPDG( pdg ); }
  const fvec& GetPDG () const { return KFPart.GetPDG(); }

  // functions for particle finding

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

//  template<class T> 
    static void FindParticles(vector<CbmKFTrack>& vRTracks, vector<float>& ChiToPrimVtx,
                                           vector<L1FieldRegion>& vField, vector<CbmKFParticle>& Particles,
                               CbmKFVertex& PrimVtx, const vector<int>& vTrackPDG, const float cuts[2][3] = DefaultCuts);

//   static void FindParticles(vector<CbmL1Track>& vRTracks, vector<CbmKFParticle>& Particles, CbmKFVertex& PrimVtx,
//                             const vector<int>& vTrackPDG, const float cuts[2][3] = DefaultCuts);
//   static void FindParticles(vector<CbmStsTrack>& vRTracks, vector<CbmKFParticle>& Particles, CbmKFVertex& PrimVtx,
//                             const vector<int>& vTrackPDG, const float cuts[2][3] = DefaultCuts);

    static void ExtrapolateToPV(vector<CbmKFParticle>& vParticles, CbmKFVertex& PrimVtx);
    static fvec GetChi2BetweenParticles(CbmKFParticle_simd &p1, CbmKFParticle_simd &p2);
//  template<class T>
    static void Find2DaughterDecay(vector<CbmKFTrack>& vTracks,
                                   const vector<L1FieldRegion>& vField,
                                   vector<CbmKFParticle>& Particles,
                                   const int DaughterNegPDG,
                                   const int DaughterPosPDG,
                                   const int MotherPDG,
                                   vector<short>& idNeg,
                                   vector<short>& idPos,
                                   CbmKFVertex& PrimVtx,
                                   const float* cuts = 0,
                                   bool isPrimary = 0,
                                   vector<float>* vMotherTopoChi2Ndf = 0,
                                   const float* secCuts = 0,
                                   const float massMotherPDG = 0,
                                   const float massMotherPDGSigma = 0,
                                   vector<CbmKFParticle>* vMotherPrim = 0,
                                   vector<CbmKFParticle>* vMotherSec = 0);
//  template<class T>
    static void Find2DaughterDecay(vector<CbmKFTrack>& vTracks,
                                   const vector<L1FieldRegion>& vField,
                                   vector<CbmKFParticle>& Particles,
                                   const int DaughterNegPDG,
                                   const int DaughterPosPDG,
                                   const int MotherPDG,
                                   vector<short>& idNeg,
                                   vector<short>& idPos,
                                   CbmKFVertex& PrimVtx,
                                   const float* cuts,
                                   bool isPrimary,
                                   const float PtCut,
                                   const float Chi2PrimCut = -100.f,
                                   vector<float>* ChiToPrimVtx = 0,
                                   const float* PCut = 0);
//  template<class T>
  static void FindTrackV0Decay(const int MotherPDG,
                               vector<CbmKFParticle>& Particles,
                               vector<CbmKFParticle>& vV0,
                               vector<CbmKFTrack>& vTracks,
                               const vector<L1FieldRegion>& field,
                               const int DaughterPDG,
                               vector<short>& idTrack,
                               CbmKFVertex& PrimVtx,
                               const float* cuts = 0,
                               bool isPrimary = 0,
                               vector<float>* ChiToPrimVtx = 0,
                               vector<CbmKFParticle>* vHyperonPrim = 0,
                               float hyperonPrimMass = 0,
                               float hyperonPrimMassErr = 0,
                               vector<CbmKFParticle>* vHyperonSec = 0);

  static void FindHyperons(int PDG,
                           CbmKFParticle_simd vDaughters[2],
                           vector<int>& daughterIds,
                           vector<CbmKFParticle>& vLambdaSec,
                           vector<CbmKFParticle>& vHyperon,
                           CbmKFVertex& PrimVtx,
                           const float *cuts = 0,
                           int startIndex=0);

//  template<class T>
    static void FindDMesLambdac(vector<CbmKFTrack>& vTracks,
                                const vector<L1FieldRegion>& vField,
                                vector<CbmKFParticle>& Particles,
                                const int DaughterPDG[5], //pi, K_b, pi_b, K, p
                                const int MotherPDG[8],
                                vector<short>* idTrack[5], //pi, K_b, pi_b, K, p
                                CbmKFVertex& PrimVtx,
                                const float cuts[8][8],
                                vector<float> ChiToPrimVtx);

//  template<class T>
    static void CombineTrackPart(vector<CbmKFTrack>& vTracks,
                                 const vector<L1FieldRegion>& vField,
                                 vector<CbmKFParticle>& Particles,
                                 CbmKFParticle& part,
                                 const int DaughterPDG,
                                 const int MotherPDG,
                                 vector<short>& id,
                                 const float* cuts,
                                 const unsigned short startIndex = 0,
                                 const bool IsSamePart = 0);

  static void SelectParticleCandidates(vector<CbmKFParticle>& Particles,
                                       vector<CbmKFParticle>& vCandidates,
                                       CbmKFVertex& PrimVtx,
                                       const float cuts[5]);

//  template<class T> 
    void ConstructPVT(vector<CbmKFTrack>& vRTracks);

//   void ConstructPV(vector<CbmL1Track>& vRTracks);
//   void ConstructPV(vector<CbmStsTrack>& vRTracks);

 protected:

  fvec& Cij( Int_t i, Int_t j ){ 
    return KFPart.C[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i];
  }
 public:
  static Int_t IJ( Int_t i, Int_t j ){ 
    return ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
  }

 public:
  CbmKFParticle_simd KFPart;

 private:

  static const float DefaultCuts[2][3];
};

#endif /* !CbmKFParticleInterface_h */

