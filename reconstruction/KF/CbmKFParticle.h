/** The CbmKFParticle class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   24.11.06
 * 
 * Class to fit decayed particle with the Kalman Filter method
 *
 */
#ifndef CBMKFPARTICLE_H
#define CBMKFPARTICLE_H

#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"

#include <vector>
using namespace std;

class CbmKFParticle: public TObject {

 public:
  int Id() const { return fId; };
  int NDaughters() const { return fDaughtersIds.size(); };
  const vector<int>& DaughterIds() const { return fDaughtersIds; };
  
  void SetId( int id ){ fId = id; } // should be always used (manualy)
  void SetNDaughters( int n ) { fDaughtersIds.reserve(n); }
  void AddDaughter( int id ){ fDaughtersIds.push_back(id); }
  void CleanDaughtersId() { fDaughtersIds.clear(); }

 private:
  
  int fId;                   // id of particle
  vector<int> fDaughtersIds; // id of particles it created from. if size == 1 then this is id of track. TODO use in functions. why unsigned short int doesn't work???

  int fPDG; // pdg hypothesis
  
 public:

  Double_t r[8], C[36];
  Int_t NDF;
  Double_t Chi2, Q;
  Bool_t AtProductionVertex;

  float fieldRegion[10];

  CbmKFParticle():fId(-1), fDaughtersIds(), fPDG(0), NDF(0), Chi2(0), Q(0), AtProductionVertex(0) {};
  CbmKFParticle( CbmKFTrackInterface* Track, Double_t *z0=0,  Int_t *qHypo=0, Int_t *pdg=0);

  ~CbmKFParticle(){};

  // Construction

  void Construct( vector<CbmKFTrackInterface*> &vDaughters, 
		  CbmKFVertexInterface *Parent=0, 
		  Double_t Mass=-1, Double_t CutChi2=-1  );

  void ConstructFromKFParticle( vector<CbmKFParticle*> &vDaughters, CbmKFVertexInterface *Parent=0, Double_t Mass=-1, Double_t CutChi2=-1  );

  // Transportation

  void TransportToProductionVertex();  
  void TransportToDecayVertex();

  Double_t GetDStoPoint( const Double_t xyz[] ) const;

  // Accessors ?

  //* Simple accessors 
  Double_t GetX    ()  const { return r[0]; }
  Double_t GetY    ()  const { return r[1]; }
  Double_t GetZ    ()  const { return r[2]; }
  Double_t GetPx   ()  const { return r[3]; }
  Double_t GetPy   ()  const { return r[4]; }
  Double_t GetPz   ()  const { return r[5]; }
  Double_t GetE    ()  const { return r[6]; }
  Double_t GetS    ()  const { return r[7]; }
  Double_t GetQ    ()  const { return Q;    }
  Double_t GetChi2 ()  const { return Chi2; }
  Int_t    GetNDF  ()  const { return NDF;  }
  Double_t *GetParameters()  { return r; }
  Double_t *GetCovMatrix()   { return C; }

  Bool_t GetAtProductionVertex() const { return AtProductionVertex; }

  
  Double_t GetParameter ( Int_t i )          const { return r[i]; }
  Double_t GetCovariance( Int_t i )          const { return C[i]; }
  Double_t GetCovariance( Int_t i, Int_t j ) const { return C[IJ(i,j)]; }

  //*
  //*  MODIFIERS
  //*
  
  Double_t & rX    () { return r[0]; }
  Double_t & rY    () { return r[1]; }
  Double_t & rZ    () { return r[2]; }
  Double_t & rPx   () { return r[3]; }
  Double_t & rPy   () { return r[4]; }
  Double_t & rPz   () { return r[5]; }
  Double_t & rE    () { return r[6]; }
  Double_t & rS    () { return r[7]; }
  Double_t & rQ    () { return Q;    }
  Double_t & rChi2 () { return Chi2; }
  Int_t    & rNDF  () { return NDF;  }

  Double_t & rParameter ( Int_t i )        { return r[i];       }
  Double_t & rCovariance( Int_t i )        { return C[i];       }
  Double_t & rCovariance( Int_t i, Int_t j ) { return C[IJ(i,j)]; }

  //* Accessor with calculations

  void GetKFTrack( CbmKFTrackInterface* Track);
  void GetKFVertex( CbmKFVertexInterface  &vtx );
  void GetMomentum( Double_t &P, Double_t &Error );
  void GetMass( Double_t &M, Double_t &Error );
  void GetDecayLength( Double_t &L, Double_t &Error );
  void GetLifeTime( Double_t &T, Double_t &Error );

  Double_t GetRapidity() const;
  Double_t GetPt()       const;
  Double_t GetTheta()    const;
  Double_t GetPhi()      const;

  void Extrapolate( Double_t r0[], double T );
  void ExtrapolateLine( Double_t r0[], double T );


  void SetPDG ( int pdg ) { fPDG = pdg; }
  int GetPDG () const { return fPDG; }
 protected:

  Double_t& Cij( Int_t i, Int_t j ){ 
    return C[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i];
  }
  static Int_t IJ( Int_t i, Int_t j ){ 
    return ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
  }

  void MeasureMass( Double_t r0[], Double_t Mass );
  void MeasureProductionVertex( Double_t r0[], CbmKFVertexInterface *Parent);
  void Convert( Double_t r0[], bool ToProduction );

  
  ClassDef( CbmKFParticle, 1 );
};
#endif /* !CBMKFPARTICLE_H */
