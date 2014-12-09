//---------------------------------------------------------------------------------
// The KFParticleBaseSIMD class
// .
// @author  S.Gorbunov, I.Kisel, I.Kulakov, M.Zyzak
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store the decayed particle parameters.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class describes general mathematics which is used by KFParticle class
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//_________________________________________________________________________________


#ifndef KFParticleBaseSIMD_H
#define KFParticleBaseSIMD_H

#include "TObject.h"


#include <vector>
#include "CbmL1Def.h"

class KFParticleBaseSIMD {
  
 public:

  //*
  //* ABSTRACT METHODS HAVE TO BE DEFINED IN USER CLASS 
  //* 

  //* Virtual method to access the magnetic field

  virtual void GetFieldValue(const fvec xyz[], fvec B[]) const = 0;
  
  //* Virtual methods needed for particle transportation 
  //* One can use particular implementations for collider (only Bz component) 
  //* geometry and for fixed-target (CBM-like) geometry which are provided below 
  //* in TRANSPORT section
 
  //* Get dS to xyz[] space point 

  virtual fvec GetDStoPoint( const fvec xyz[] ) const = 0;

  //* Get dS to other particle p (dSp for particle p also returned) 

  virtual void GetDStoParticle( const KFParticleBaseSIMD &p, 
				fvec &DS, fvec &DSp ) const = 0;
  
  //* Transport on dS value along trajectory, output to P,C

  virtual void Transport( fvec dS, fvec P[], fvec C[] ) const = 0;



  //*
  //*  INITIALIZATION
  //*

  //* Constructor 

  KFParticleBaseSIMD();

  //* Destructor 

  virtual ~KFParticleBaseSIMD() { ; }

 //* Initialisation from "cartesian" coordinates ( X Y Z Px Py Pz )
 //* Parameters, covariance matrix, charge, and mass hypothesis should be provided 

  void Initialize( const fvec Param[], const fvec Cov[], Int_t Charge, fvec Mass );

  //* Initialise covariance matrix and set current parameters to 0.0 

  void Initialize();

  //* Set decay vertex parameters for linearisation 

  void SetVtxGuess( fvec x, fvec y, fvec z );
  void SetVtxErrGuess( fvec& x, fvec& y, fvec& z );

  //* Set consruction method

  void SetConstructMethod(Int_t m) {fConstructMethod = m;}

  //* Set and get mass hypothesis of the particle
  void SetMassHypo(fvec m) { fMassHypo = m;}
  const fvec& GetMassHypo() const { return fMassHypo; }

  //* Returns the sum of masses of the daughters
  const fvec& GetSumDaughterMass() const {return SumDaughterMass;}

  //*
  //*  ACCESSORS
  //*

  //* Simple accessors 

  fvec GetX    () const { return fP[0]; }
  fvec GetY    () const { return fP[1]; }
  fvec GetZ    () const { return fP[2]; }
  fvec GetPx   () const { return fP[3]; }
  fvec GetPy   () const { return fP[4]; }
  fvec GetPz   () const { return fP[5]; }
  fvec GetE    () const { return fP[6]; }
  fvec GetS    () const { return fP[7]; }
  fvec GetQ    () const { return fQ;    }
  fvec GetChi2 () const { return fChi2; }
  fvec GetNDF  () const { return fNDF;  }

  const fvec& X    () const { return fP[0]; }
  const fvec& Y    () const { return fP[1]; }
  const fvec& Z    () const { return fP[2]; }
  const fvec& Px   () const { return fP[3]; }
  const fvec& Py   () const { return fP[4]; }
  const fvec& Pz   () const { return fP[5]; }
  const fvec& E    () const { return fP[6]; }
  const fvec& S    () const { return fP[7]; }
  const fvec& Q    () const { return fQ;    }
  const fvec& Chi2 () const { return fChi2; }
  const fvec& NDF  () const { return fNDF;  }
  
  fvec GetParameter ( Int_t i )        const { return fP[i];       }
  fvec GetCovariance( Int_t i )        const { return fC[i];       }
  fvec GetCovariance( Int_t i, Int_t j ) const { return fC[IJ(i,j)]; }

  //* Accessors with calculations( &value, &estimated sigma )
  //* error flag returned (0 means no error during calculations) 

  fvec GetMomentum    ( fvec &P, fvec &SigmaP ) const ;
  fvec GetPt          ( fvec &Pt, fvec &SigmaPt ) const ;
  fvec GetEta         ( fvec &Eta, fvec &SigmaEta ) const ;
  fvec GetPhi         ( fvec &Phi, fvec &SigmaPhi ) const ;
  fvec GetMass        ( fvec &M, fvec &SigmaM ) const ;
  fvec GetDecayLength ( fvec &L, fvec &SigmaL ) const ;
  fvec GetDecayLengthXY ( fvec &L, fvec &SigmaL ) const ;
  fvec GetLifeTime    ( fvec &T, fvec &SigmaT ) const ;
  fvec GetR           ( fvec &R, fvec &SigmaR ) const ;

  //*
  //*  MODIFIERS
  //*
  
  fvec & X    () { return fP[0]; }
  fvec & Y    () { return fP[1]; }
  fvec & Z    () { return fP[2]; }
  fvec & Px   () { return fP[3]; }
  fvec & Py   () { return fP[4]; }
  fvec & Pz   () { return fP[5]; }
  fvec & E    () { return fP[6]; }
  fvec & S    () { return fP[7]; }
  fvec & Q    () { return fQ;    }
  fvec & Chi2 () { return fChi2; }
  fvec & NDF  () { return fNDF;  }

  fvec & Parameter ( Int_t i )        { return fP[i];       }
  fvec & Covariance( Int_t i )        { return fC[i];       }
  fvec & Covariance( Int_t i, Int_t j ) { return fC[IJ(i,j)]; }


  //* 
  //* CONSTRUCTION OF THE PARTICLE BY ITS DAUGHTERS AND MOTHER
  //* USING THE KALMAN FILTER METHOD
  //*


  //* Simple way to add daughter ex. D0+= Pion; 

  void operator +=( const KFParticleBaseSIMD &Daughter );  

  //* Add daughter track to the particle 

  void AddDaughter( const KFParticleBaseSIMD &Daughter, Bool_t isAtVtxGuess = 0 );

  void AddDaughterWithEnergyFit( const KFParticleBaseSIMD &Daughter, Bool_t isAtVtxGuess );
  void AddDaughterWithEnergyCalc( const KFParticleBaseSIMD &Daughter, Bool_t isAtVtxGuess );
  void AddDaughterWithEnergyFitMC( const KFParticleBaseSIMD &Daughter, Bool_t isAtVtxGuess  ); 
  //with mass constrained

  //* Set production vertex 

  void SetProductionVertex( const KFParticleBaseSIMD &Vtx );

  //* Set mass constraint 

  void SetNonlinearMassConstraint( fvec Mass );
  void SetMassConstraint( fvec Mass, fvec SigmaMass = 0 );

  //* Set no decay length for resonances

  void SetNoDecayLength();


  //* Everything in one go  

  void Construct( const KFParticleBaseSIMD *vDaughters[], Int_t nDaughters, 
		  const KFParticleBaseSIMD *ProdVtx=0,   Float_t Mass=-1, Bool_t IsConstrained=0,
                  Bool_t isAtVtxGuess = 0  );

  //*
  //*                   TRANSPORT
  //* 
  //*  ( main transportation parameter is S = SignedPath/Momentum )
  //*  ( parameters of decay & production vertices are stored locally )
  //*


  //* Transport the particle to its decay vertex 

  void TransportToDecayVertex();

  //* Transport the particle to its production vertex 

  void TransportToProductionVertex();

  //* Transport the particle on dS parameter (SignedPath/Momentum) 

  void TransportToDS( fvec dS );

  //* Particular extrapolators one can use 

  fvec GetDStoPointBz( fvec Bz, const fvec xyz[] ) const;
  fvec GetDStoPointBy( fvec By, const fvec xyz[] ) const;

  void GetDStoParticleBz( fvec Bz, const KFParticleBaseSIMD &p, fvec &dS, fvec &dS1 ) const ;
  void GetDStoParticleBy( fvec B, const KFParticleBaseSIMD &p, fvec &dS, fvec &dS1 ) const ;

  fvec GetDStoPointCBM( const fvec xyz[] ) const;
  void GetDStoParticleCBM( const KFParticleBaseSIMD &p, fvec &dS, fvec &dS1 ) const ;

  void TransportBz( fvec Bz, fvec dS, fvec P[], fvec C[] ) const;
  void TransportCBM( fvec dS, fvec P[], fvec C[] ) const;  


  //* 
  //* OTHER UTILITIES
  //*

  //* Calculate distance from another object [cm]

  fvec GetDistanceFromVertex( const fvec vtx[] ) const;
  fvec GetDistanceFromVertex( const KFParticleBaseSIMD &Vtx ) const;
  fvec GetDistanceFromParticle( const KFParticleBaseSIMD &p ) const;

  //* Calculate sqrt(Chi2/ndf) deviation from vertex
  //* v = [xyz], Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix

  fvec GetDeviationFromVertex( const fvec v[], 
                                   const fvec Cv[]=0 ) const;
  fvec GetDeviationFromVertex( const KFParticleBaseSIMD &Vtx ) const;
  fvec GetDeviationFromParticle( const KFParticleBaseSIMD &p ) const;  

  //* Subtract the particle from the vertex  

  void SubtractFromVertex( KFParticleBaseSIMD &Vtx ) const;
  void SubtractFromParticle( KFParticleBaseSIMD &Vtx ) const;

  //* Special method for creating gammas

  void ConstructGammaBz( const KFParticleBaseSIMD &daughter1,
                         const KFParticleBaseSIMD &daughter2, fvec Bz  );

  //* return parameters for the Armenteros-Podolanski plot
  static void GetArmenterosPodolanski(KFParticleBaseSIMD& positive, KFParticleBaseSIMD& negative, fvec QtAlfa[2] );

  //* Rotates the KFParticle object around OZ axis, OZ axis is set by the vertex position
  void RotateXY(fvec angle, fvec Vtx[3]);

  fvec Id() const { return fId; };
  int NDaughters() const { return fDaughterIds.size(); };
  std::vector<fvec>& DaughterIds() { return fDaughterIds; };
  
  void SetId( fvec id ){ fId = id; }; // should be always used (manualy)
  void SetNDaughters( int n ) { fDaughterIds.reserve(n); }
  void AddDaughterId( fvec id ){ fDaughterIds.push_back(id); };
  void CleanDaughtersId() { fDaughterIds.clear(); }

  void SetPDG ( int pdg ) { fPDG = pdg; }
  const int& GetPDG () const { return fPDG; }

  void GetDistanceToVertexLine( const KFParticleBaseSIMD &Vertex, fvec &l, fvec &dl, fvec *isParticleFromVertex = 0 ) const;

 protected:

  static Int_t IJ( Int_t i, Int_t j ){ 
    return ( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i;
  }

  fvec & Cij( Int_t i, Int_t j ){ return fC[IJ(i,j)]; }

  void Convert( bool ToProduction );
  void TransportLine( fvec S, fvec P[], fvec C[] ) const ;
  fvec GetDStoPointLine( const fvec xyz[] ) const;
  void GetDStoParticleLine( const KFParticleBaseSIMD &p, fvec &dS, fvec &dS1 ) const ;

  void GetDSIter(const KFParticleBaseSIMD &p, fvec const &dS, fvec x[3], fvec dx[3], fvec ddx[3]) const;

  static fvec InvertSym3( const fvec A[], fvec Ainv[] );
  static void InvertCholetsky3(fvec a[6]);

  static void MultQSQt( const fvec Q[], const fvec S[], 
                        fvec SOut[] );

  static void multQSQt1( const fvec J[11], fvec S[] );

  fvec GetSCorrection( const fvec Part[], const fvec XYZ[] ) const;

  void GetMeasurement( const fvec XYZ[], fvec m[], fvec V[], Bool_t isAtVtxGuess = 0 ) const ;

  //* Mass constraint function. is needed for the nonlinear mass constraint and a fit with mass constraint
  void SetMassConstraint( fvec *mP, fvec *mC, fvec mJ[7][7], fvec mass, fvec mask );

  fvec fP[8];  //* Main particle parameters {X,Y,Z,Px,Py,Pz,E,S[=DecayLength/P]}
  fvec fC[36]; //* Low-triangle covariance matrix of fP
  fvec fQ;     //* Particle charge 
  fvec fNDF;   //* Number of degrees of freedom 
  fvec fChi2;  //* Chi^2

  fvec fSFromDecay; //* Distance from decay vertex to current position

  Bool_t fAtProductionVertex; //* Flag shows that the particle error along
                              //* its trajectory is taken from production vertex    
  Bool_t fIsVtxGuess;
  Bool_t fIsVtxErrGuess;

  fvec fVtxGuess[3];  //* Guess for the position of the decay vertex 
                          //* ( used for linearisation of equations )
  fvec fVtxErrGuess[3]; //* Guess for the initial error of the decay vertex 

  Bool_t fIsLinearized;   //* Flag shows that the guess is present

  Int_t fConstructMethod; //* Determines the method for the particle construction. 
  //* 0 - Energy considered as an independent veriable, fitted independently from momentum, without any constraints on mass
  //* 1 - Energy considered as a dependent variable, calculated from the momentum and mass hypothesis
  //* 2 - Energy considered as an independent variable, fitted independently from momentum, with constraints on mass of daughter particle

  fvec SumDaughterMass;  //* sum of the daughter particles masses
  fvec fMassHypo;  //* sum of the daughter particles masses

  fvec fId;                   // id of particle
  std::vector<fvec> fDaughterIds; // id of particles it created from. if size == 1 then this is id of track.

  int fPDG; // pdg hypothesis
};

#endif 
