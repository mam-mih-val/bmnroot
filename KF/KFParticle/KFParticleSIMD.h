//---------------------------------------------------------------------------------
// The KFParticleSIMD class
// .
// @author  S.Gorbunov, I.Kisel
// @version 1.0
// @since   13.05.07
// 
// Class to reconstruct and store the decayed particle parameters.
// The method is described in CBM-SOFT note 2007-003, 
// ``Reconstruction of decayed particles based on the Kalman filter'', 
// http://www.gsi.de/documents/DOC-2007-May-14-1.pdf
//
// This class is ALICE interface to general mathematics in KFParticleBaseSIMD
// 
//  -= Copyright &copy ALICE HLT and CBM L1 Groups =-
//_________________________________________________________________________________

#define NonhomogeneousField
//#define HomogeneousField

#ifndef KFPARTICLESIMD_H
#define KFPARTICLESIMD_H

#include "KFParticleBaseSIMD.h"
#include "TMath.h"

#ifdef HomogeneousField
class AliVTrack;
class AliVVertex;
#endif

#ifdef NonhomogeneousField
#include "CbmKF.h"
class CbmKFTrackInterface;
class CbmKFVertexInterface;
#include "L1Field.h"
#endif

class KFParticle;

class KFParticleSIMD :public KFParticleBaseSIMD
{
  
 public:

  //*
  //*  INITIALIZATION
  //*

  //* Set magnetic field for all particles
#ifdef HomogeneousField
  static void SetField( fvec Bz );
#endif
#ifdef NonhomogeneousField
  void SetField(const L1FieldRegion &field, bool isOneEntry=0, const int iVec=0)
  {
    if(!isOneEntry)
      fField = field;
    else
      fField.SetOneEntry(field,iVec);
  }
#endif
  //* Constructor (empty)

  KFParticleSIMD():KFParticleBaseSIMD()
  #ifdef NonhomogeneousField
    , fField()
  #endif
  { ; }

  //* Destructor (empty)

  ~KFParticleSIMD(){ ; }

  //* Construction of mother particle by its 2-3-4 daughters

  KFParticleSIMD( const KFParticleSIMD &d1, const KFParticleSIMD &d2, Bool_t gamma = kFALSE );

  KFParticleSIMD( const KFParticleSIMD &d1, const KFParticleSIMD &d2,  const KFParticleSIMD &d3 );

  KFParticleSIMD( const KFParticleSIMD &d1, const KFParticleSIMD &d2, const KFParticleSIMD &d3, const KFParticleSIMD &d4 );

 //* Initialisation from "cartesian" coordinates ( X Y Z Px Py Pz )
 //* Parameters, covariance matrix, charge and PID hypothesis should be provided 

  void Create( const fvec Param[], const fvec Cov[], Int_t Charge, fvec mass /*Int_t PID*/ );

#ifdef HomogeneousField
 //* Initialisation from ALICE track, PID hypothesis shoould be provided 

  KFParticleSIMD( const AliVTrack &track, Int_t PID );

  //* Initialisation from VVertex 

  KFParticleSIMD( const AliVVertex &vertex );
#endif

#ifdef NonhomogeneousField
  KFParticleSIMD( CbmKFTrackInterface* Track[], int NTracks, Int_t *qHypo=0, const Int_t *pdg=0 );
  KFParticleSIMD( CbmKFTrackInterface &Track, Int_t *qHypo=0, const Int_t *pdg=0);
  KFParticleSIMD( CbmKFVertexInterface &vertex );
  void Create(CbmKFTrackInterface* Track[], int NTracks, Int_t *qHypo=0, const Int_t *pdg=0);
#endif
  KFParticleSIMD( KFParticle* part[], const int nPart = 0 );
  KFParticleSIMD( KFParticle &part );

  //* Initialise covariance matrix and set current parameters to 0.0 

  void Initialize();

  //* Set decay vertex parameters for linearisation 

  void SetVtxGuess( fvec x, fvec y, fvec z );

  //*
  //*  ACCESSORS
  //*

  //* Simple accessors 

  fvec GetX    () const ; //* x of current position
  fvec GetY    () const ; //* y of current position
  fvec GetZ    () const ; //* z of current position
  fvec GetPx   () const ; //* x-compoment of 3-momentum
  fvec GetPy   () const ; //* y-compoment of 3-momentum
  fvec GetPz   () const ; //* z-compoment of 3-momentum
  fvec GetE    () const ; //* energy
  fvec GetS    () const ; //* decay length / momentum
  fvec GetQ    () const ; //* charge
  fvec GetChi2 () const ; //* chi^2
  fvec GetNDF  () const ; //* Number of Degrees of Freedom

  Bool_t GetAtProductionVertex() const { return fAtProductionVertex; }

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
  
  fvec GetParameter ( int i ) const ;
  fvec GetCovariance( int i ) const ;
  fvec GetCovariance( int i, int j ) const ;

  //* Accessors with calculations, value returned w/o error flag
  
  fvec GetP           () const; //* momentum
  fvec GetPt          () const; //* transverse momentum
  fvec GetEta         () const; //* pseudorapidity
  fvec GetPhi         () const; //* phi
  fvec GetMomentum    () const; //* momentum (same as GetP() )
  fvec GetMass        () const; //* mass
  fvec GetDecayLength () const; //* decay length
  fvec GetDecayLengthXY () const; //* decay length in XY
  fvec GetLifeTime    () const; //* life time
  fvec GetR           () const; //* distance to the origin

  //* Accessors to estimated errors

  fvec GetErrX           () const ; //* x of current position 
  fvec GetErrY           () const ; //* y of current position
  fvec GetErrZ           () const ; //* z of current position
  fvec GetErrPx          () const ; //* x-compoment of 3-momentum
  fvec GetErrPy          () const ; //* y-compoment of 3-momentum
  fvec GetErrPz          () const ; //* z-compoment of 3-momentum
  fvec GetErrE           () const ; //* energy
  fvec GetErrS           () const ; //* decay length / momentum
  fvec GetErrP           () const ; //* momentum
  fvec GetErrPt          () const ; //* transverse momentum
  fvec GetErrEta         () const ; //* pseudorapidity
  fvec GetErrPhi         () const ; //* phi
  fvec GetErrMomentum    () const ; //* momentum
  fvec GetErrMass        () const ; //* mass
  fvec GetErrDecayLength () const ; //* decay length
  fvec GetErrDecayLengthXY () const ; //* decay length in XY
  fvec GetErrLifeTime    () const ; //* life time
  fvec GetErrR           () const ; //* distance to the origin

  //* Accessors with calculations( &value, &estimated sigma )
  //* error flag returned (0 means no error during calculations) 

  fvec GetP           ( fvec &P, fvec &SigmaP ) const ;   //* momentum
  fvec GetPt          ( fvec &Pt, fvec &SigmaPt ) const ; //* transverse momentum
  fvec GetEta         ( fvec &Eta, fvec &SigmaEta ) const ; //* pseudorapidity
  fvec GetPhi         ( fvec &Phi, fvec &SigmaPhi ) const ; //* phi
  fvec GetMomentum    ( fvec &P, fvec &SigmaP ) const ;   //* momentum
  fvec GetMass        ( fvec &M, fvec &SigmaM ) const ;   //* mass
  fvec GetDecayLength ( fvec &L, fvec &SigmaL ) const ;   //* decay length
  fvec GetDecayLengthXY ( fvec &L, fvec &SigmaL ) const ;   //* decay length in XY
  fvec GetLifeTime    ( fvec &T, fvec &SigmaT ) const ;   //* life time
  fvec GetR           ( fvec &R, fvec &SigmaR ) const ; //* R


  //*
  //*  MODIFIERS
  //*
  
  fvec & X    () ;
  fvec & Y    () ;
  fvec & Z    () ;
  fvec & Px   () ;
  fvec & Py   () ;
  fvec & Pz   () ;
  fvec & E    () ;
  fvec & S    () ;
  fvec & Q    () ;
  fvec & Chi2 () ;
  fvec & NDF  () ;

  fvec & Parameter ( int i ) ;
  fvec & Covariance( int i ) ;
  fvec & Covariance( int i, int j ) ;
  fvec * Parameters () ;
  fvec * CovarianceMatrix() ;

  void GetKFParticle( KFParticle &Part, int iPart = 0);
  void GetKFParticle( KFParticle *Part, int nPart = 0);

  //* 
  //* CONSTRUCTION OF THE PARTICLE BY ITS DAUGHTERS AND MOTHER
  //* USING THE KALMAN FILTER METHOD
  //*


  //* Add daughter to the particle 

  void AddDaughter( const KFParticleSIMD &Daughter );

  //* Add daughter via += operator: ex.{ D0; D0+=Pion; D0+= Kaon; }

  void operator +=( const KFParticleSIMD &Daughter );  

  //* Set production vertex 

  void SetProductionVertex( const KFParticleSIMD &Vtx );

  //* Set mass constraint 

  void SetMassConstraint( fvec Mass, fvec SigmaMass = 0  );
  
  //* Set no decay length for resonances

  void SetNoDecayLength();

  //* Everything in one go  

  void Construct( const KFParticleSIMD *vDaughters[], int nDaughters, 
                  const KFParticleSIMD *ProdVtx=0,   Float_t Mass=-1, Bool_t IsConstrained=0,
                  Bool_t isAtVtxGuess = 0);

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

  //* Transport the particle close to xyz[] point 

  void TransportToPoint( const fvec xyz[] );

  //* Transport the particle close to VVertex  
#ifdef HomogeneousField
  void TransportToVertex( const AliVVertex &v );
#endif
  //* Transport the particle close to another particle p 

  void TransportToParticle( const KFParticleSIMD &p );

  //* Transport the particle on dS parameter (SignedPath/Momentum) 

  void TransportToDS( fvec dS );

  //* Get dS to a certain space point 

  fvec GetDStoPoint( const fvec xyz[] ) const ;
  
  //* Get dS to other particle p (dSp for particle p also returned) 

  void GetDStoParticle( const KFParticleSIMD &p, 
			fvec &DS, fvec &DSp ) const ;
  
  //* Get dS to other particle p in XY-plane

  void GetDStoParticleXY( const KFParticleBaseSIMD &p, 
			  fvec &DS, fvec &DSp ) const ;
  
  //* 
  //* OTHER UTILITIES
  //*


  //* Calculate distance from another object [cm]

  fvec GetDistanceFromVertex( const fvec vtx[] ) const ;
  fvec GetDistanceFromVertex( const KFParticleSIMD &Vtx ) const ;
#ifdef HomogeneousField
  fvec GetDistanceFromVertex( const AliVVertex &Vtx ) const ;
#endif
  fvec GetDistanceFromParticle( const KFParticleSIMD &p ) const ;

  //* Calculate sqrt(Chi2/ndf) deviation from another object
  //* ( v = [xyz]-vertex, Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix )

  fvec GetDeviationFromVertex( const fvec v[], const fvec Cv[]=0 ) const ;
  fvec GetDeviationFromVertex( const KFParticleSIMD &Vtx ) const ;
#ifdef HomogeneousField
  fvec GetDeviationFromVertex( const AliVVertex &Vtx ) const ;
#endif
  fvec GetDeviationFromParticle( const KFParticleSIMD &p ) const ;
 
  //* Calculate distance from another object [cm] in XY-plane

  fvec GetDistanceFromVertexXY( const fvec vtx[], fvec &val, fvec &err ) const ;
  fvec GetDistanceFromVertexXY( const fvec vtx[], const fvec Cv[], fvec &val, fvec &err ) const ;
  fvec GetDistanceFromVertexXY( const KFParticleSIMD &Vtx, fvec &val, fvec &err ) const ;
#ifdef HomogeneousField
  fvec GetDistanceFromVertexXY( const AliVVertex &Vtx, fvec &val, fvec &err ) const ;
#endif

  fvec GetDistanceFromVertexXY( const fvec vtx[] ) const ;
  fvec GetDistanceFromVertexXY( const KFParticleSIMD &Vtx ) const ;
#ifdef HomogeneousField
  fvec GetDistanceFromVertexXY( const AliVVertex &Vtx ) const ;
#endif
  fvec GetDistanceFromParticleXY( const KFParticleSIMD &p ) const ;

  //* Calculate sqrt(Chi2/ndf) deviation from another object in XY plane
  //* ( v = [xyz]-vertex, Cv=[Cxx,Cxy,Cyy,Cxz,Cyz,Czz]-covariance matrix )

  fvec GetDeviationFromVertexXY( const fvec v[], const fvec Cv[]=0 ) const ;
  fvec GetDeviationFromVertexXY( const KFParticleSIMD &Vtx ) const ;
#ifdef HomogeneousField
  fvec GetDeviationFromVertexXY( const AliVVertex &Vtx ) const ;
#endif
  fvec GetDeviationFromParticleXY( const KFParticleSIMD &p ) const ;

  //* Calculate opennig angle between two particles

  fvec GetAngle  ( const KFParticleSIMD &p ) const ;
  fvec GetAngleXY( const KFParticleSIMD &p ) const ;
  fvec GetAngleRZ( const KFParticleSIMD &p ) const ;

  //* Subtract the particle from the vertex  

  void SubtractFromVertex( KFParticleSIMD &v ) const ;
  void SubtractFromParticle( KFParticleSIMD &v ) const ;

  //* Special method for creating gammas

  void ConstructGamma( const KFParticleSIMD &daughter1,
		       const KFParticleSIMD &daughter2  );

    // * Pseudo Proper Time of decay = (r*pt) / |pt| * M/|pt|
    // @primVertex - primary vertex
    // @mass - mass of the mother particle (in the case of "Hb -> JPsi" it would be JPsi mass)
    // @*timeErr2 - squared error of the decay time. If timeErr2 = 0 it isn't calculated
  fvec GetPseudoProperDecayTime( const KFParticleSIMD &primVertex, const fvec& mass, fvec* timeErr2 = 0 ) const;

 protected: 
  
  //*
  //*  INTERNAL STUFF
  //* 

  //* Method to access ALICE field 
#ifdef HomogeneousField
  static fvec GetFieldAlice();
#endif
  //* Other methods required by the abstract KFParticleBaseSIMD class 
  
  void GetFieldValue( const fvec xyz[], fvec B[] ) const ;
  void GetDStoParticle( const KFParticleBaseSIMD &p, fvec &DS, fvec &DSp )const ;
  void Transport( fvec dS, fvec P[], fvec C[] ) const ;
  static void GetExternalTrackParam( const KFParticleBaseSIMD &p, fvec &X, fvec &Alpha, fvec P[5]  ) ;

  //void GetDStoParticleALICE( const KFParticleBaseSIMD &p, fvec &DS, fvec &DS1 ) const;


 private:
#ifdef HomogeneousField
  static fvec fgBz;  //* Bz compoment of the magnetic field
#endif
#ifdef NonhomogeneousField
  L1FieldRegion fField;
#endif
};



//---------------------------------------------------------------------
//
//     Inline implementation of the KFParticleSIMD methods
//
//---------------------------------------------------------------------

#ifdef HomogeneousField
inline void KFParticleSIMD::SetField( fvec Bz )
{ 
  fgBz = Bz;
}
#endif

inline KFParticleSIMD::KFParticleSIMD( const KFParticleSIMD &d1, 
                                   const KFParticleSIMD &d2, 
                                   const KFParticleSIMD &d3 )
  #ifdef NonhomogeneousField
    : fField()
  #endif
{
  KFParticleSIMD mother;
  mother+= d1;
  mother+= d2;
  mother+= d3;
  *this = mother;
}

inline KFParticleSIMD::KFParticleSIMD( const KFParticleSIMD &d1, 
                               const KFParticleSIMD &d2, 
                               const KFParticleSIMD &d3, 
                               const KFParticleSIMD &d4 )
  #ifdef NonhomogeneousField
    : fField()
  #endif
{
  KFParticleSIMD mother;
  mother+= d1;
  mother+= d2;
  mother+= d3;
  mother+= d4;
  *this = mother;
}


inline void KFParticleSIMD::Initialize()
{ 
  KFParticleBaseSIMD::Initialize(); 
}

inline void KFParticleSIMD::SetVtxGuess( fvec x, fvec y, fvec z )
{
  KFParticleBaseSIMD::SetVtxGuess(x,y,z);
}  

inline fvec KFParticleSIMD::GetX    () const 
{ 
  return KFParticleBaseSIMD::GetX();    
}

inline fvec KFParticleSIMD::GetY    () const 
{ 
  return KFParticleBaseSIMD::GetY();    
}

inline fvec KFParticleSIMD::GetZ    () const 
{ 
  return KFParticleBaseSIMD::GetZ();    
}

inline fvec KFParticleSIMD::GetPx   () const 
{ 
  return KFParticleBaseSIMD::GetPx();   
}

inline fvec KFParticleSIMD::GetPy   () const 
{ 
  return KFParticleBaseSIMD::GetPy();   
}

inline fvec KFParticleSIMD::GetPz   () const 
{ 
  return KFParticleBaseSIMD::GetPz();   
}

inline fvec KFParticleSIMD::GetE    () const 
{ 
  return KFParticleBaseSIMD::GetE();    
}

inline fvec KFParticleSIMD::GetS    () const 
{ 
  return KFParticleBaseSIMD::GetS();    
}

inline fvec    KFParticleSIMD::GetQ    () const 
{ 
  return KFParticleBaseSIMD::GetQ();    
}

inline fvec KFParticleSIMD::GetChi2 () const 
{ 
  return KFParticleBaseSIMD::GetChi2(); 
}

inline fvec    KFParticleSIMD::GetNDF  () const 
{ 
  return KFParticleBaseSIMD::GetNDF();  
}

inline fvec KFParticleSIMD::GetParameter ( int i ) const 
{ 
  return KFParticleBaseSIMD::GetParameter(i);  
}

inline fvec KFParticleSIMD::GetCovariance( int i ) const 
{ 
  return KFParticleBaseSIMD::GetCovariance(i); 
}

inline fvec KFParticleSIMD::GetCovariance( int i, int j ) const 
{ 
  return KFParticleBaseSIMD::GetCovariance(i,j);
}


inline fvec KFParticleSIMD::GetP    () const
{
  fvec par, err;
  fvec mask = KFParticleBaseSIMD::GetMomentum( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetPt   () const
{
  fvec par, err;
  fvec mask = KFParticleBaseSIMD::GetPt( par, err ) ;
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetEta   () const
{
  fvec par, err;
  fvec mask = KFParticleBaseSIMD::GetEta( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetPhi   () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetPhi( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetMomentum    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetMomentum( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetMass        () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetMass( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetDecayLength () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetDecayLength( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetDecayLengthXY () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetDecayLengthXY( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetLifeTime    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetLifeTime( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetR   () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetR( par, err );
  return ((!mask) & par);
}

inline fvec KFParticleSIMD::GetErrX           () const 
{
  return sqrt(fabs( GetCovariance(0,0) ));
}

inline fvec KFParticleSIMD::GetErrY           () const 
{
  return sqrt(fabs( GetCovariance(1,1) ));
}

inline fvec KFParticleSIMD::GetErrZ           () const 
{
  return sqrt(fabs( GetCovariance(2,2) ));
}

inline fvec KFParticleSIMD::GetErrPx          () const 
{
  return sqrt(fabs( GetCovariance(3,3) ));
}

inline fvec KFParticleSIMD::GetErrPy          () const 
{
  return sqrt(fabs( GetCovariance(4,4) ));
}

inline fvec KFParticleSIMD::GetErrPz          () const 
{
  return sqrt(fabs( GetCovariance(5,5) ));
}

inline fvec KFParticleSIMD::GetErrE           () const 
{
  return sqrt(fabs( GetCovariance(6,6) ));
}

inline fvec KFParticleSIMD::GetErrS           () const 
{
  return sqrt(fabs( GetCovariance(7,7) ));
}

inline fvec KFParticleSIMD::GetErrP    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetMomentum( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrPt    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetPt( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrEta    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetEta( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrPhi    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetPhi( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrMomentum    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetMomentum( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrMass        () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetMass( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrDecayLength () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetDecayLength( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrDecayLengthXY () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetDecayLengthXY( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrLifeTime    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetLifeTime( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}

inline fvec KFParticleSIMD::GetErrR    () const
{
  fvec par, err;
  fvec mask = KFParticleSIMD::GetR( par, err );
  fvec ret(1.e10);
  ret = (mask & ret) + ((!mask) & err);
  return ret;
}


inline fvec KFParticleSIMD::GetP( fvec &P, fvec &SigmaP ) const 
{
  return KFParticleBaseSIMD::GetMomentum( P, SigmaP );
}

inline fvec KFParticleSIMD::GetPt( fvec &Pt, fvec &SigmaPt ) const 
{
  return KFParticleBaseSIMD::GetPt( Pt, SigmaPt );
}

inline fvec KFParticleSIMD::GetEta( fvec &Eta, fvec &SigmaEta ) const 
{
  return KFParticleBaseSIMD::GetEta( Eta, SigmaEta );
}

inline fvec KFParticleSIMD::GetPhi( fvec &Phi, fvec &SigmaPhi ) const 
{
  return KFParticleBaseSIMD::GetPhi( Phi, SigmaPhi );
}

inline fvec KFParticleSIMD::GetMomentum( fvec &P, fvec &SigmaP ) const 
{
  return KFParticleBaseSIMD::GetMomentum( P, SigmaP );
}

inline fvec KFParticleSIMD::GetMass( fvec &M, fvec &SigmaM ) const 
{
  return KFParticleBaseSIMD::GetMass( M, SigmaM );
}

inline fvec KFParticleSIMD::GetDecayLength( fvec &L, fvec &SigmaL ) const 
{
  return KFParticleBaseSIMD::GetDecayLength( L, SigmaL );
}

inline fvec KFParticleSIMD::GetDecayLengthXY( fvec &L, fvec &SigmaL ) const 
{
  return KFParticleBaseSIMD::GetDecayLengthXY( L, SigmaL );
}

inline fvec KFParticleSIMD::GetLifeTime( fvec &T, fvec &SigmaT ) const 
{
  return KFParticleBaseSIMD::GetLifeTime( T, SigmaT );
}

inline fvec KFParticleSIMD::GetR( fvec &R, fvec &SigmaR ) const 
{
  return KFParticleBaseSIMD::GetR( R, SigmaR );
}

inline fvec & KFParticleSIMD::X() 
{ 
  return KFParticleBaseSIMD::X();    
}

inline fvec & KFParticleSIMD::Y()
{ 
  return KFParticleBaseSIMD::Y();    
}

inline fvec & KFParticleSIMD::Z() 
{ 
  return KFParticleBaseSIMD::Z();    
}

inline fvec & KFParticleSIMD::Px() 
{ 
  return KFParticleBaseSIMD::Px();   
}

inline fvec & KFParticleSIMD::Py() 
{ 
  return KFParticleBaseSIMD::Py();   
}

inline fvec & KFParticleSIMD::Pz() 
{ 
  return KFParticleBaseSIMD::Pz();   
}

inline fvec & KFParticleSIMD::E() 
{ 
  return KFParticleBaseSIMD::E();    
}

inline fvec & KFParticleSIMD::S() 
{ 
  return KFParticleBaseSIMD::S();    
}

inline fvec    & KFParticleSIMD::Q() 
{ 
  return KFParticleBaseSIMD::Q();    
}

inline fvec & KFParticleSIMD::Chi2() 
{ 
  return KFParticleBaseSIMD::Chi2(); 
}

inline fvec    & KFParticleSIMD::NDF() 
{ 
  return KFParticleBaseSIMD::NDF();  
}

inline fvec & KFParticleSIMD::Parameter ( int i )        
{ 
  return KFParticleBaseSIMD::Parameter(i);
}

inline fvec & KFParticleSIMD::Covariance( int i )        
{ 
  return KFParticleBaseSIMD::Covariance(i);
}

inline fvec & KFParticleSIMD::Covariance( int i, int j ) 
{ 
  return KFParticleBaseSIMD::Covariance(i,j); 
}

inline fvec * KFParticleSIMD::Parameters ()
{
  return fP;
}

inline fvec * KFParticleSIMD::CovarianceMatrix()
{
  return fC;
}


inline void KFParticleSIMD::operator +=( const KFParticleSIMD &Daughter )
{
  KFParticleBaseSIMD::operator +=( Daughter );
}
  

inline void KFParticleSIMD::AddDaughter( const KFParticleSIMD &Daughter )
{
  KFParticleBaseSIMD::AddDaughter( Daughter );
}

inline void KFParticleSIMD::SetProductionVertex( const KFParticleSIMD &Vtx )
{
  KFParticleBaseSIMD::SetProductionVertex( Vtx );
}

inline void KFParticleSIMD::SetMassConstraint( fvec Mass, fvec SigmaMass )
{
  KFParticleBaseSIMD::SetMassConstraint( Mass, SigmaMass );
}

inline void KFParticleSIMD::SetNoDecayLength()
{
  KFParticleBaseSIMD::SetNoDecayLength();
}

inline void KFParticleSIMD::Construct( const KFParticleSIMD *vDaughters[], int nDaughters, 
                                       const KFParticleSIMD *ProdVtx,   Float_t Mass, Bool_t IsConstrained,
                                       Bool_t isAtVtxGuess  )
{
  KFParticleBaseSIMD::Construct( ( const KFParticleBaseSIMD**)vDaughters, nDaughters, 
                                 ( const KFParticleBaseSIMD*)ProdVtx, Mass, IsConstrained, isAtVtxGuess );

  #ifdef NonhomogeneousField
  // calculate a field region for the constructed particle
  L1FieldValue field[3];
  fvec zField[3] = {0, fP[2]/2, fP[2]};

  for(int iPoint=0; iPoint<3; iPoint++)
  {
    for(int iD=0; iD<nDaughters; ++iD)
    {
      L1FieldValue b = const_cast<KFParticleSIMD *>(vDaughters[iD])->fField.Get(zField[iPoint]);
      field[iPoint].x += b.x;
      field[iPoint].y += b.y;
      field[iPoint].z += b.z;
    }
    field[iPoint].x /= nDaughters;
    field[iPoint].y /= nDaughters;
    field[iPoint].z /= nDaughters;
  }

  fField.Set( field[2], zField[2], field[1], zField[1], field[0], zField[0] );
  #endif
}

inline void KFParticleSIMD::TransportToDecayVertex()
{
  KFParticleBaseSIMD::TransportToDecayVertex(); 
}

inline void KFParticleSIMD::TransportToProductionVertex()
{
  KFParticleBaseSIMD::TransportToProductionVertex();
}

inline void KFParticleSIMD::TransportToPoint( const fvec xyz[] )
{
  TransportToDS( GetDStoPoint(xyz) );
}
#ifdef HomogeneousField
inline void KFParticleSIMD::TransportToVertex( const AliVVertex &v )
{
  TransportToPoint( KFParticleSIMD(v).fP );
}
#endif
inline void KFParticleSIMD::TransportToParticle( const KFParticleSIMD &p )
{ 
  fvec dS, dSp;
  GetDStoParticle( p, dS, dSp );
  TransportToDS( dS );
}

inline void KFParticleSIMD::TransportToDS( fvec dS )
{
  KFParticleBaseSIMD::TransportToDS( dS );
} 

inline fvec KFParticleSIMD::GetDStoPoint( const fvec xyz[] ) const 
{
#ifdef HomogeneousField
  return KFParticleBaseSIMD::GetDStoPointBz( GetFieldAlice(), xyz );
#endif
#ifdef NonhomogeneousField
  return KFParticleBaseSIMD::GetDStoPointCBM( xyz );
#endif
}

  
inline void KFParticleSIMD::GetDStoParticle( const KFParticleSIMD &p, 
					    fvec &DS, fvec &DSp ) const 
{
  GetDStoParticleXY( p, DS, DSp );
}


inline fvec KFParticleSIMD::GetDistanceFromVertex( const fvec vtx[] ) const
{
  return KFParticleBaseSIMD::GetDistanceFromVertex( vtx );
}

inline fvec KFParticleSIMD::GetDeviationFromVertex( const fvec v[], 
						       const fvec Cv[] ) const
{
  return KFParticleBaseSIMD::GetDeviationFromVertex( v, Cv);
}

inline fvec KFParticleSIMD::GetDistanceFromVertex( const KFParticleSIMD &Vtx ) const
{
  return KFParticleBaseSIMD::GetDistanceFromVertex( Vtx );
}

inline fvec KFParticleSIMD::GetDeviationFromVertex( const KFParticleSIMD &Vtx ) const
{
  return KFParticleBaseSIMD::GetDeviationFromVertex( Vtx );
}
#ifdef HomogeneousField
inline fvec KFParticleSIMD::GetDistanceFromVertex( const AliVVertex &Vtx ) const
{
  return GetDistanceFromVertex( KFParticleSIMD(Vtx) );
}

inline fvec KFParticleSIMD::GetDeviationFromVertex( const AliVVertex &Vtx ) const
{
  return GetDeviationFromVertex( KFParticleSIMD(Vtx) );
}
#endif
inline fvec KFParticleSIMD::GetDistanceFromParticle( const KFParticleSIMD &p ) const 
{
  return KFParticleBaseSIMD::GetDistanceFromParticle( p );
}

inline fvec KFParticleSIMD::GetDeviationFromParticle( const KFParticleSIMD &p ) const 
{
  return KFParticleBaseSIMD::GetDeviationFromParticle( p );
}

inline void KFParticleSIMD::SubtractFromVertex( KFParticleSIMD &v ) const 
{
  KFParticleBaseSIMD::SubtractFromVertex( v );
}

inline void KFParticleSIMD::SubtractFromParticle( KFParticleSIMD &v ) const 
{
  KFParticleBaseSIMD::SubtractFromParticle( v );
}

#ifdef HomogeneousField
inline fvec KFParticleSIMD::GetFieldAlice()
{ 
  return fgBz; 
}
#endif

#ifdef HomogeneousField
inline void KFParticleSIMD::GetFieldValue( const fvec * /*xyz*/, fvec B[] ) const 
{    
  B[0] = B[1] = 0;
  B[2] = GetFieldAlice();
}
#endif

#ifdef NonhomogeneousField
inline void KFParticleSIMD::GetFieldValue( const fvec xyz[], fvec B[] ) const 
{
  L1FieldValue mB = const_cast<L1FieldRegion&>(fField).Get(xyz[2]);
  B[0] = mB.x;
  B[1] = mB.y;
  B[2] = mB.z;
}
#endif

inline void KFParticleSIMD::GetDStoParticle( const KFParticleBaseSIMD &p, 
					    fvec &DS, fvec &DSp )const
{
  GetDStoParticleXY( p, DS, DSp );
}

inline void KFParticleSIMD::GetDStoParticleXY( const KFParticleBaseSIMD &p, 
				       fvec &DS, fvec &DSp ) const
{ 
#ifdef HomogeneousField
  KFParticleBaseSIMD::GetDStoParticleBz( GetFieldAlice(), p, DS, DSp ) ;
#endif
#ifdef NonhomogeneousField
  KFParticleBaseSIMD::GetDStoParticleCBM( p, DS, DSp ) ;
#endif
  //GetDStoParticleALICE( p, DS, DSp ) ;
}

inline void KFParticleSIMD::Transport( fvec dS, fvec P[], fvec C[] ) const 
{
#ifdef HomogeneousField
  KFParticleBaseSIMD::TransportBz( GetFieldAlice(), dS, P, C );
#endif
#ifdef NonhomogeneousField
  KFParticleBaseSIMD::TransportCBM( dS, P, C );
#endif
}

inline void KFParticleSIMD::ConstructGamma( const KFParticleSIMD &daughter1,
					   const KFParticleSIMD &daughter2  )
{
#ifdef HomogeneousField
  KFParticleBaseSIMD::ConstructGammaBz( daughter1, daughter2, GetFieldAlice() );
#endif
}

#endif 
