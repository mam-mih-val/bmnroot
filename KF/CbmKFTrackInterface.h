
/** Abstract base class for track is being used by the CbmKF Kalman Filter 
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
 * IMPORTANT : you should keep your hits sorted downstream 
 *
 * See how-to-use-it below.
 */

#ifndef CBMKFTRACKINTERFACE_H
#define CBMKFTRACKINTERFACE_H

#include "TObject.h"

class CbmKFHit;
class CbmKFVertexInterface;

class CbmKFTrackInterface {
  
 public:  

  CbmKFTrackInterface():fId(){}
  virtual ~CbmKFTrackInterface(){}

  /** Information not to be changed
   */

  virtual Double_t  GetMass()   { return 0.1396; } /// Mass hypothesis
  virtual Bool_t    IsElectron(){ return 0; }      /// Is it electron

  /** I/O parameters for extrapolation routines
   */

  virtual Double_t *GetTrack()    ; /// array[6] of track parameters(x,y,tx,ty,qp,z)
  virtual Double_t *GetCovMatrix(); /// array[15] of covariance matrix

  /** Parameters during and after the track fit
   */

  virtual Double_t &GetRefChi2(); /// Chi^2 after fit
  virtual Int_t    &GetRefNDF() ; /// Number of Degrees of Freedom after fit

  /** Accessors to the track hits during track fit
   */

  virtual Int_t     GetNOfHits()     { return 0; } /// Number of hits
  virtual CbmKFHit *GetHit( Int_t i ){ return 0; } /// Access to i-th hit

  /** Methods
   */
  Int_t Extrapolate( Double_t z, Double_t *QP0=0 );
  Int_t Fit( Bool_t downstream = 1 );
  void Smooth( Double_t Z );
  void Fit2Vertex( CbmKFVertexInterface  &vtx );

  Int_t Propagate( Double_t z_out, Double_t QP0 );
  Int_t Propagate( Double_t z_out );

  int Id() const { return fId; };
  void SetId( int id ){ fId = id; };
  
 protected:
  
  int fId;
  
  ClassDef( CbmKFTrackInterface, 1 )
};



/******************************************************************
 *
 *  There are few ways to let the Kalman Filter treat your track.
 *
 ******************************************************************
 *
 * // The first way: inherit the CbmKFTrackInterface, like :
 *
 * class CbmMyTrack :public CbmKFTrackInterface{
 *  public:
 *   // here overwrite CbmKF virtual methods
 *   Double_t *GetTrack(){ return fT; } 
 *   ...
 *   // Here is my methods
 *   ...
 *  private:
 *   Double_t fT[6]; 
 *   Double_t fVariablesForMyNeeds;
 * };
 *
 * // Then use your track directly:
 *
 * CbmMyTrack my_track;
 * ...
 * CbmKF::Instance()->FitTrack( my_track );
 *
 *
 ******************************************************************
 *
 *
 * // The second way: make interface class to your track, like:
 *
 * class CbmMyTrack{
 *  public:
 *   Double_t *GetFittedTrackParametersAtThePrimaryVertex();
 *   ...
 * };
 *
 * class CbmMyTrackInterface : public CbmKFTrackInterface{
 *  public:
 *   CbmMyTrackInterface( CbmMyTrack *track ):fTrack(track){}
 *   Double_t *GetTrack(){ 
 *     return ((CbmMyTrack*)fTrack)->GetFittedTrackParametersAtThePrimaryVertex(); 
 *   } 
 *   ...
 *  private:
 *   CbmMyTrackTrack *fTrack;
 * };
 *
 * // Then call the KF routines through the interface, like:
 *
 * CbmMyTrack my_track;
 * ...
 * CbmMyTrackInterface I(my_track);
 * CbmKF::Instance()->FitTrack( I );
 *
 ******************************************************************
 *
 *
 * // Other ways...
 *
 *
 */

#endif /* !CBMKFTRACKINTERFACE_H */
