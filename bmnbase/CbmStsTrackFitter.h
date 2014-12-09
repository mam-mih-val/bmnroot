// -------------------------------------------------------------------------
// -----                   CbmStsTrackFitter header file               -----
// -----                  Created 18/02/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsTrackFitter
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Abstract base class for concrete STS track fitting algorithm.
 ** Each derived class must implement the method DoFit. This has
 ** to operate on an object of type CbmStsTrack and fill the
 ** parameters fPidHypo, fParamFirst, fParamLast, fFlag and fChi2.
 **/

#ifndef CBMSTSTRACKFITTER
#define CBMSTSTRACKFITTER 1

#include "FairTrackParam.h"

#include "TObject.h"

class CbmStsTrack;


class CbmStsTrackFitter : public TObject
{

 public:

  /** Default constructor **/
  CbmStsTrackFitter() { };


  /** Destructor **/
  virtual ~CbmStsTrackFitter() { };


  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init() { };


  /** Abstract method DoFit. To be implemented in the concrete class.
   ** Task: Make a fit to the hits attached to the track by the track
   ** finder. Fill the track parameter member variables.
   **
   *@param pTrack      Pointer to CbmStsTrack
   *@param pidHypo     PID hypothesis for the fit. Default is pion.
   **/
  virtual Int_t DoFit(CbmStsTrack* pTrack, Int_t pidHypo = 211) = 0;


  /** Abstract method Extrapolate. Gives track parameters at a given z
   ** position.
   **
   *@param track  Pointer to StsTrack
   *@param z      z position
   *@param param  (return value) StsTrackParam at z
   **/
  virtual void Extrapolate( CbmStsTrack* track, Double_t z, 
			    FairTrackParam *param ) = 0;

 private:
 
  CbmStsTrackFitter(const CbmStsTrackFitter&);
      

  ClassDef(CbmStsTrackFitter,1);

};

#endif
