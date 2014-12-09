/** The CbmKFParticle_simd class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   24.11.06
 * 
 * Class to fit decayed particle with the Kalman Filter method
 *
 */
#ifndef CBMKFPARTICLE_SIMD_H
#define CBMKFPARTICLE_SIMD_H

#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"
class CbmKFParticle;

#include "CbmL1Def.h"
#include "L1Field.h"

#include <vector>

using namespace std;

class CbmKFParticle_simd {

 public:
  void *operator new(size_t bytes) {return _mm_malloc(bytes, 16);}
  void operator delete(void *p) {_mm_free(p);}

 public:

  CbmKFParticle_simd();
  CbmKFParticle_simd( CbmKFTrackInterface *Track[], Int_t *qHypo=0, const Int_t *pdg=0);
  CbmKFParticle_simd( CbmKFTrackInterface &Track, Int_t *qHypo=0, const Int_t *pdg=0);
  CbmKFParticle_simd( CbmKFParticle *part[] );
  CbmKFParticle_simd( CbmKFParticle &part );

  ~CbmKFParticle_simd(){};

  // Construction
  void Create(CbmKFTrackInterface* Track[],int Ntracks = fvecLen, Int_t *qHypo=0, const Int_t *pdg=0);
  void Create(CbmKFParticle *part[],int N = fvecLen);

  // Accessors

  //* Simple accessors 
  fvec GetX    ()  const { return r[0]; }
  fvec GetY    ()  const { return r[1]; }
  fvec GetZ    ()  const { return r[2]; }
  fvec GetPx   ()  const { return r[3]; }
  fvec GetPy   ()  const { return r[4]; }
  fvec GetPz   ()  const { return r[5]; }
  fvec GetE    ()  const { return r[6]; }
  fvec GetS    ()  const { return r[7]; }
  fvec GetQ    ()  const { return Q;    }
  fvec GetChi2 ()  const { return Chi2; }
  fvec GetNDF  ()  const { return NDF;  }
  fvec *GetParameters()  { return r; }
  const fvec *GetParameters() const  { return r; }
  fvec *GetCovMatrix()   { return C; }
  fvec GetParameter ( Int_t i )          const { return r[i]; }
  fvec GetCovariance( Int_t i )          const { return C[i]; }
  
//  fvec GetParameter ( Int_t i )          const { return r[i]; }
//  fvec GetCovariance( Int_t i )          const { return C[i]; }
//  fvec GetCovariance( Int_t i, Int_t j ) const { return C[IJ(i,j)]; }

  fvec GetVtxGuess    ( int i ) const { return fVtxGuess[i];    }
  fvec GetVtxErrGuess ( int i ) const { return fVtxErrGuess[i]; }
  fvec *GetVtxGuess   ()              { return fVtxGuess;       }
  fvec *GetVtxErrGuess()              { return fVtxErrGuess;    }

  //*
  //*  MODIFIERS
  //*
  
  fvec & rX    () { return r[0]; }
  fvec & rY    () { return r[1]; }
  fvec & rZ    () { return r[2]; }
  fvec & rPx   () { return r[3]; }
  fvec & rPy   () { return r[4]; }
  fvec & rPz   () { return r[5]; }
  fvec & rE    () { return r[6]; }
  fvec & rS    () { return r[7]; }
  fvec & rQ    () { return Q;    }
  fvec & rChi2 () { return Chi2; }
  fvec & rNDF  () { return NDF;  }

//  fvec & rParameter ( Int_t i )          { return r[i];       }
//  fvec & rCovariance( Int_t i )          { return C[i];       }
//  fvec & rCovariance( Int_t i, Int_t j ) { return C[IJ(i,j)]; }

  //* Accessor with calculations

  void GetKFTrack( CbmKFTrackInterface** Track);

  fvec GetTx    ()  const { return r[3]/r[5]; }
  fvec GetTy    ()  const { return r[4]/r[5]; }

  void GetMomentum( fvec &P, fvec &Error );
  void GetMass( fvec &M, fvec &Error );
  void GetDecayLength( fvec &L, fvec &Error );
  void GetLifeTime( fvec &T, fvec &Error );

  fvec GetRapidity() const { return 0.5*log((r[6] + r[5])/(r[6] - r[5])); }
  fvec GetPt()       const { return sqrt(r[3]*r[3] + r[4]*r[4]); }
  fvec GetTheta()    const { return atan2(GetPt(),r[5]); }
  fvec GetPhi()      const { return atan2(r[4],r[5]); }
  
  void SetVtxGuess(fvec &x, fvec &y, fvec &z);
  void SetVtxErrGuess(fvec &d_x, fvec &d_y, fvec &d_z);

  void SetField();
  void SetField(CbmKFTrackInterface** Track);
  void SetField(const L1FieldRegion &field, bool isOneEntry=0, const int iVec=0);

  void SetPDG ( fvec pdg ) { fPDG = pdg; }
  const fvec& GetPDG () const { return fPDG; }

  void SetId( fvec id ){ fId = id; };
  void SetNDaughters( int n ) { fDaughterIds.reserve(n); }
  void AddDaughterId( fvec id ){ fDaughterIds.push_back(id); };
  void CleanDaughtersId() { fDaughterIds.clear(); }

  fvec Id() const { return fId; };
  int NDaughters() const { return fDaughterIds.size(); };
  vector<fvec>& DaughterIds() { return fDaughterIds; };

 private:
  fvec fId;                   // id of particle
  vector<fvec> fDaughterIds; // id of particles it created from. if size == 1 then this is id of track.

 public:

  fvec r[8];
  fvec C[36];
  fvec fVtxGuess[3];
  fvec fVtxErrGuess[3];

  fvec NDF _fvecalignment;
  fvec Chi2 _fvecalignment, Q _fvecalignment;

  fvec fPDG; // PDG hypothesis
  
  bool AtProductionVertex;
  bool fIsVtxGuess;
  bool fIsVtxErrGuess;

  L1FieldRegion fField;

} _fvecalignment;
#endif /* !CBMKFPARTICLE_H */

