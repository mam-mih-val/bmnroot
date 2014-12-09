
/** Abstract base class for vertex is being used by the CbmKF Kalman Filter 
 *
 * @author  S.Gorbunov
 * @version 0.0
 * @since   02.12.05
 * 
 * The class members describes to the CbmKF Kalman Filter routines 
 * how to get acces to data in a derived (or user) class.
 *
 * To be used by high-level utilites like StsTrackFitter
 *
 * Usage is similar to CbmKFTrackInterface class
 *
 */

#include "CbmKFVertexInterface.h"

#include "CbmVertex.h"

ClassImp( CbmKFVertexInterface )

static Double_t gTempD[11];
static Int_t gTempI[2];

Double_t &CbmKFVertexInterface::GetRefX()         { return gTempD[ 0]; }
Double_t &CbmKFVertexInterface::GetRefY()         { return gTempD[ 1]; }
Double_t &CbmKFVertexInterface::GetRefZ()         { return gTempD[ 2]; }
Double_t *CbmKFVertexInterface::GetCovMatrix()    { return gTempD +3 ; }  
Double_t &CbmKFVertexInterface::GetRefChi2()      { return gTempD[ 9]; }
Int_t    &CbmKFVertexInterface::GetRefNDF()       { return gTempI[ 0]; }
Int_t    &CbmKFVertexInterface::GetRefNTracks()   { return gTempI[ 1]; }

void CbmKFVertexInterface::SetVertex( CbmVertex &v )
{
  GetRefX() = v.GetX();
  GetRefY() = v.GetY();
  GetRefZ() = v.GetZ();
  GetRefChi2() = v.GetChi2();
  GetRefNDF()  = v.GetNDF();
  GetRefNTracks()  = v.GetNTracks();
  TMatrixFSym tmp(3);
  v.CovMatrix( tmp );
  for( int i=0, k=0; i<3; i++) 
    for( int j=0; j<=i; j++, k++ ) GetCovMatrix()[k] = tmp(i,j);
}

void CbmKFVertexInterface::GetVertex( CbmVertex &v )
{
  TMatrixFSym covMat(3);
  for(int i=0, k=0;i<3;i++) for(int j=0; j<=i; j++,k++) covMat(i,j) = GetCovMatrix()[k];
  v.SetVertex( GetRefX(), GetRefY(), GetRefZ(), 
	       GetRefChi2(), GetRefNDF(), GetRefNTracks(), covMat );
}
