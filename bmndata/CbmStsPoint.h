// -------------------------------------------------------------------------
// -----                      CbmStsPoint header file                  -----
// -----                  Created 26/07/04  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmStsPoint.h
 *@author V.Friese <v.friese@gsi.de>
 *
 * Interception of MC track with a STS detetcor. Holds in addition
 * to the base class the coordinates and momentum at the exit from
 * the active volume.
 **/


#ifndef CBMSTSPOINT_H
#define CBMSTSPOINT_H


#include "FairMCPoint.h"
#include "FairLogger.h"

class TVector3;

class CbmStsPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  CbmStsPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   *@param eventId  MC event identifier
   **/
  CbmStsPoint(Int_t trackID, Int_t detID, TVector3 posIn,
	      TVector3 posOut, TVector3 momIn, TVector3 momOut,
	      Double_t tof, Double_t length, Double_t eLoss,
	      Int_t eventId = 0);


  /** Copy constructor with event and epoch time
   ** Re-calculates time w.r.t. epoch time start
   *@param eventId     MC event identifier (negative values keep original event ID)
   *@param eventTime   MC event time [ns]
   *@param epochTime   epoch start time [ns]
   **/
  CbmStsPoint(const CbmStsPoint& point,
	      Int_t    eventId   = -1,
	      Double_t eventTime = 0.,
	      Double_t epochTime = 0.);


  /** Destructor **/
  virtual ~CbmStsPoint();


  /** Accessors **/
  Double_t GetXIn()   const { return fX; }
  Double_t GetYIn()   const { return fY; }
  Double_t GetZIn()   const { return fZ; }
  Double_t GetXOut()  const { return fX_out; }
  Double_t GetYOut()  const { return fY_out; }
  Double_t GetZOut()  const { return fZ_out; }
  Double_t GetPxOut() const { return fPx_out; }
  Double_t GetPyOut() const { return fPy_out; }
  Double_t GetPzOut() const { return fPz_out; }
  void PositionIn(TVector3& pos)  { pos.SetXYZ(fX, fY, fZ); }
  void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out,fY_out,fZ_out); }
  void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }
  Int_t GetStation() const { return fStation; }
  Int_t GetModule() const { return fModule; }


  /** Point coordinates at given z from linear extrapolation **/
  Double_t GetX(Double_t z) const;
  Double_t GetY(Double_t z) const;


  /** Check for distance between in and out **/
  Bool_t IsUsable() const;


  /** Modifiers **/
  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);
  virtual void SetTrackID(Int_t id){
	  //SetLink(kMCTrack, id);
	  FairMCPoint::SetTrackID(id);
  };
  void SetStation(Int_t station) { fStation = station; }
  void SetModule(Int_t module) { fModule = module; }

  /** Output to screen **/
  virtual void Print(const Option_t* opt = "") const { }
  void Info() const;



 protected:

  Double32_t fX_out,  fY_out,  fZ_out;
  Double32_t fPx_out, fPy_out, fPz_out;
  Int_t fStation;
  Int_t fModule;



  ClassDef(CbmStsPoint,1)

};



inline void CbmStsPoint::SetPositionOut(TVector3 pos) {
  fX_out = pos.X();
  fY_out = pos.Y();
  fZ_out = pos.Z();
}


inline void CbmStsPoint::SetMomentumOut(TVector3 mom) {
  fPx_out = mom.Px();
  fPy_out = mom.Py();
  fPz_out = mom.Pz();
}



#endif
