/******************************************************************************
* $Id: CbmEcalTrackExtrapolationKF.h,v 1.1 2006/02/17 23:13:37 sgorboun Exp $
*
*  Class  : CbmEcalTrackExtrapolationKF
*  Description: "TrackExtrapolation" from STS tracks (Kalman Fitter)
*               It reads the track array form STS and extrapolates those to
*               be projected to the Rich Photodetector to some z-Plane in RICH
*
*  Author : Claudia Hoehne
*  E-mail : c.hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmEcalTrackExtrapolationKF.h,v $
*  Revision 1.1  2006/02/17 23:13:37  sgorboun
*  Update of KF&L1 classes
*
*  Revision 1.2  2006/01/30 11:00:55  hoehne
*  bug fix: Point Array at z-plane was not filled correctly (counters not correct)
*
*  Revision 1.1  2006/01/26 09:53:22  hoehne
*  initial version for track extrapolation (base class + concrete implementations + task) to z-plane in RICH
*
*
*
*******************************************************************************/

#ifndef CBMECALTRACKEXTRAPOLATIONKF
#define CBMECALTRACKEXTRAPOLATIONKF

#include "CbmEcalTrackExtrapolation.h"

class TClonesArray;
class CbmGlobalTrack;

class CbmEcalTrackExtrapolationKF : public CbmEcalTrackExtrapolation
{
public:
  /** Default constructor **/
  CbmEcalTrackExtrapolationKF();
  /** Standard constructor **/
  CbmEcalTrackExtrapolationKF(Int_t verbose);
  /** Destructor **/
  virtual ~CbmEcalTrackExtrapolationKF();
  /** Initialisation **/
  virtual void Init();
  /** Finish **/
  virtual void Finish();
  /** Method DoExtrapolate.
   ** Task: Read the track array and fill the point array at given z-Plane in RICH detector
   ** pointers to which are given as argument
   **
   *@param rTrackArray  Array of tracks
   *@param rTrackParamArray  Array of FairTrackParam
   *@value Number of tracks extrapolated
   **/
  virtual Int_t DoExtrapolate(TClonesArray* gTrackArray, TClonesArray* fTrackParamArray);
private:
  /** Arrays **/
  TClonesArray* fTrackParamArray;
  TClonesArray* fSTSArray;

  Int_t    iEx;            /** number of extrapolated tracks */
  ClassDef(CbmEcalTrackExtrapolationKF,1);

 private:
  CbmEcalTrackExtrapolationKF(const CbmEcalTrackExtrapolationKF&);
  void operator=(const CbmEcalTrackExtrapolationKF&);
};

#endif
