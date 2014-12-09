// -------------------------------------------------------------------------
// -----                    CbmPsdPoint header file                   -----
// -----                 Created 28/07/04  by V. Friese                -----
// -------------------------------------------------------------------------

/**  CbmPsdPoint.h
 *@author V. Friese
 **
 ** Interception of MC track with the plane representing the ECAL.
 **/


#ifndef CBMPSDPOINT_H
#define CBMPSDPOINT_H 1


#include "FairMCPoint.h"

class Tvector3;

class CbmPsdPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  CbmPsdPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Ccoordinates at entrance to active volume [cm]
   *@param mom      Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  CbmPsdPoint(Int_t trackID, Int_t detID,  TVector3 pos, TVector3 mom, 
	       Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  //  CbmPsdPoint(const CbmPsdPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~CbmPsdPoint();


  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

   /** Modifiers **/
  void SetModuleID(Int_t mod) { fModuleID = mod; }
   /** Accessors **/
  Int_t GetModuleID() const {return fModuleID;}

 private:
  Int_t fModuleID; //number of module

  
  ClassDef(CbmPsdPoint,2)

};


#endif
