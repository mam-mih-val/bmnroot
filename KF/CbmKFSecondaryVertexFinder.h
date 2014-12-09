/** The CbmKFSecondaryVertexFinder class
 *
 * @author  S.Gorbunov, I.Kisel
 * @version 1.0
 * @since   06.02.06
 * 
 * Class to fit secondary vertex with the Kalman Filter method
 *
 */
#ifndef CBMKFSECONDARYVERTEXFINDER_H
#define CBMKFSECONDARYVERTEXFINDER_H

#include "CbmKFTrackInterface.h"
#include "CbmKFVertexInterface.h"

#include <vector>

class CbmKFSecondaryVertexFinder:public TObject {
  
  std::vector<CbmKFTrackInterface*> vTracks;
  Int_t NDF;
  Double_t Chi2, MassConstraint;
  Double_t r[8], r0[8], C[36];
  CbmKFVertexInterface *VParent, *VGuess;

  void AddMassConstraint();
  void AddTopoConstraint();
  Double_t& Cij( Int_t i, Int_t j ){ 
    return C[( j<=i ) ? i*(i+1)/2+j :j*(j+1)/2+i];
  }

 public:

  CbmKFSecondaryVertexFinder(): vTracks(), NDF(0), Chi2(0), MassConstraint(0), VParent(0), VGuess(0) { Clear(); };
  ~CbmKFSecondaryVertexFinder(){};

  void Clear();
  void ClearTracks();
  void AddTrack( CbmKFTrackInterface* Track );
  void SetTracks( std::vector<CbmKFTrackInterface*> &vTracks );
  void SetApproximation( CbmKFVertexInterface *Guess=0 );
  void SetMassConstraint( Double_t MotherMass=-1 );
  void SetTopoConstraint( CbmKFVertexInterface *Parent=0 );
  void Fit();
  void GetVertex( CbmKFVertexInterface  &vtx );
  void GetVertex( CbmVertex  &vtx );
  void GetMotherTrack( Double_t T[], Double_t C[] );
  void GetMass( Double_t *M, Double_t *Error );
  void Extrapolate( double T );

  ClassDef( CbmKFSecondaryVertexFinder, 1 );

 private:
  CbmKFSecondaryVertexFinder(const CbmKFSecondaryVertexFinder&);
  void operator=(const CbmKFSecondaryVertexFinder&);
};
#endif /* !CBMKFSECONDARYVERTEXFINDER_H */
