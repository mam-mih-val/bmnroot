// -------------------------------------------------------------------------
// -----                     CbmTofPoint header file                   -----
// -----                 Created 28/07/04  by V. Friese                -----
// -------------------------------------------------------------------------
// modified 23/10/2012 nh
//
/**  CbmTofPoint.h
 *@author V. Friese
 **
 ** Interception of MC track with a TOF detector.
 **/


#ifndef CBMTOFPOINT_H
#define CBMTOFPOINT_H 1


#include "FairMCPoint.h"

class TVector3;

class CbmTofPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  CbmTofPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  CbmTofPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, 
	       Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  CbmTofPoint(const CbmTofPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~CbmTofPoint();


  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  ClassDef(CbmTofPoint,2)

};


#endif





