
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

#ifndef CBMKFVERTEXINTERFACE_H
#define CBMKFVERTEXINTERFACE_H

#include "TObject.h"

class CbmVertex;

class CbmKFVertexInterface :public TObject{

 public:  

  CbmKFVertexInterface(){}
  virtual ~CbmKFVertexInterface(){}

  /** I/O parameters for vertex fit routines
   */

  virtual Double_t &GetRefX(); 
  virtual Double_t &GetRefY(); 
  virtual Double_t &GetRefZ(); 
  virtual Double_t *GetCovMatrix();    /// Array[6] of covariance matrix
  virtual Double_t &GetRefChi2();      /// Chi^2 after fit
  virtual Int_t    &GetRefNDF();       /// Number of Degrees of Freedom after fit
  virtual Int_t    &GetRefNTracks();   /// Number of tracks used during fit

  void SetVertex( CbmVertex &v );
  void GetVertex( CbmVertex &v );


  ClassDef( CbmKFVertexInterface, 1 )
};

#endif /* !CBMKFVERTEXINTERFACE_H */
