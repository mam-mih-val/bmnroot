// ------------------------------------------------------------------------
// -----                     BmnHodoPoint header file                   -----
// ------------------------------------------------------------------------

#ifndef BMNHODOPOINT_H
#define BMNHODOPOINT_H

#include "FairMCPoint.h"
#include "Rtypes.h"
#include "TLorentzVector.h"
#include "TObject.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

using namespace std;

class BmnHodoPoint : public FairMCPoint {
 public:
  /** Default constructor **/
  BmnHodoPoint();

  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param copyNo   Sticksens number
   *@param copyNoMother   Stick number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

  BmnHodoPoint(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, UInt_t EventId=0 );

/** Copy constructor **/
  BmnHodoPoint(const BmnHodoPoint& point) { *this = point; };

/** Destructor **/
  virtual ~BmnHodoPoint();

  Short_t GetCopy()        const {return nCopy; };//sens. stick
  Short_t GetCopyMother()  const {return nCopyMother; };//stick
  Double_t GetTime() const { return fTime; }
  Double_t GetLengthtrack() const { return fLengthtrack; }
  
  Double_t GetPx() const { return fPx; }
  Double_t GetPy() const { return fPy; }
  Double_t GetPz() const { return fPz; }
  
  void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx, fPy, fPz); }
  
  Double_t GetX() const { return fX; }
  Double_t GetY() const { return fY; }
  Double_t GetZ() const { return fZ; }

/** Modifiers **/
  void SetCopy(Short_t i)          { nCopy    = i; }; 
  void SetCopyMother(Short_t i)    { nCopyMother  = i; }; 
  //void SetCopyZdc(Short_t i)    { nCopyZdc  = i; }; 

  void PositionOut(TVector3& pos) { pos.SetXYZ(fX, fY, fZ); }

// ----------------------------------------------------------------------------

/** Output to screen **/
  virtual void Print(const Option_t* opt) const;
  
  void AddSTICK(Int_t trackID, Int_t detID, Int_t idsticksens, Int_t idstick, TVector3 pos,TVector3 mom,Double_t dt, Double_t dl, Double_t de) { 
    if(nCopy != idsticksens)
      cerr << "Warning: idsticksens not equal in BmnHodoPoint::AddSTICK";
    if(nCopyMother != idstick)
      cerr << "Warning: idstick not equal in BmnHodoPoint::AddSTICK";
    fTrackID=trackID; fDetectorID=detID;
    fX=pos.X(); fY=pos.Y(); fZ=pos.Z();
    fPx=mom.Px(); fPy=mom.Py(); fPz=mom.Pz();
    nCopy=idsticksens; nCopyMother=idstick; fELoss += de; fLength += dl; fTime+=dt;
  }

 protected:
  Short_t nCopy;                // Copy number //sticksens
  Short_t nCopyMother;          // Copy number of mother volume //stick
  Double_t fX, fY, fZ;
  Double_t fPx, fPy, fPz;
  Double_t fTime;
  Double_t fLengthtrack;
  
  //ClassDef(BmnHodoPoint, 0)
  ClassDef(BmnHodoPoint, 1)
    };

#endif
