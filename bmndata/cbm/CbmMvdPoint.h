// -------------------------------------------------------------------------
// -----                      CbmMvdPoint header file                  -----
// -----                  Created 06/11/06  by V. Friese               -----
// -------------------------------------------------------------------------


/**  CbmMvdPoint.h
 *@author V.Friese <v.friese@gsi.de>
 *
 * Data class for interception of MC track with a MVD detetcor station. 
 * Holds in addition to the base class the coordinates and momentum at the 
 * exit from the active volume.
 *
 * Data level MC
 **/


#ifndef CBMMVDPOINT_H
#define CBMMVDPOINT_H 1

#include "FairMCPoint.h"
#include "CbmMvdDetectorId.h"

#include "TObject.h"
#include "TVector3.h"

class CbmMvdPoint : public FairMCPoint, CbmMvdDetectorId
{

 public:

  /** Default constructor **/
  CbmMvdPoint();


  /** Constructor with arguments
   *@param trackId    Index of MCTrack
   *@param pdgCode    Particle type (PDG code)
   *@param stationNr  Station number
   *@param posIn      Coordinates at entrance to active volume [cm]
   *@param posOut     Coordinates at exit of active volume [cm]
   *@param momIn      Momentum of track at entrance [GeV]
   *@param momOut     Momentum of track at exit [GeV]
   *@param tof        Time since event start [ns]
   *@param length     Track length since creation [cm]
   *@param eLoss      Energy deposit [GeV]
   **/
  CbmMvdPoint(Int_t trackId, Int_t pdgCode, Int_t detId, TVector3 posIn, 
	      TVector3 posOut, TVector3 momIn, TVector3 momOut,
	      Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  //  CbmMvdPoint(const CbmMvdPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~CbmMvdPoint();


  /** Accessors **/
  Double_t GetXOut()   const { return fX_out; }
  Double_t GetYOut()   const { return fY_out; }
  Double_t GetZOut()   const { return fZ_out; }
  Double_t GetPxOut()  const { return fPx_out; }
  Double_t GetPyOut()  const { return fPy_out; }
  Double_t GetPzOut()  const { return fPz_out; }
  Int_t GetPdgCode()   const {return fPdgCode;}
  Int_t GetSystemId()  const { return SystemId(fDetectorID); }
  Int_t GetStationNr() const { return StationNr(fDetectorID); }
  Int_t GetPointId()   const {return fPointId;} // Returns index of this object in its TClonesArray.
						// By default not filled. Used internally in the MvdDigitizer.
  void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out,fY_out,fZ_out); }
  void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }


  /** Modifiers **/
  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);
  void SetPdgCode(Int_t pdg){fPdgCode=pdg;}
  void SetPointId(Int_t myId) {fPointId=myId;}



  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;



 protected:

  Double32_t fX_out,  fY_out,  fZ_out;
  Double32_t fPx_out, fPy_out, fPz_out;
  Int_t      fPdgCode; // index of the object in its TClonesArray. By default not filled => -1.
  Int_t      fPointId; // index of the object in its TClonesArray. By default not filled => -1.



  ClassDef(CbmMvdPoint,1)

};



inline void CbmMvdPoint::SetPositionOut(TVector3 pos) {
  fX_out = pos.X();
  fY_out = pos.Y();
  fZ_out = pos.Z();
}


inline void CbmMvdPoint::SetMomentumOut(TVector3 mom) {
  fPx_out = mom.Px();
  fPy_out = mom.Py();
  fPz_out = mom.Pz();
}



#endif
