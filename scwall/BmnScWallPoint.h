// ------------------------------------------------------------------------
// -----                     BmnScWallPoint header file                   -----
// ------------------------------------------------------------------------

#ifndef BMNSCWALLPOINT_H
#define BMNSCWALLPOINT_H

#include "FairMCPoint.h"
#include "Rtypes.h"
#include "TLorentzVector.h"
#include "TObject.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"

using namespace std;

class BmnScWallPoint : public FairMCPoint {
   public:
/** Default constructor **/
    BmnScWallPoint();

    /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param copyNo   Slice number
   *@param copyNoMother   Cell number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

    BmnScWallPoint(Int_t trackID, Int_t detID, Int_t copyNo, Int_t copyNoMother, TVector3 pos, TVector3 mom, Double_t tof, Double_t length, Double_t eLoss, UInt_t EventId=0 );

/** Copy constructor **/
    BmnScWallPoint(const BmnScWallPoint& point) { *this = point; };

/** Destructor **/
    virtual ~BmnScWallPoint();

    Short_t GetCopy()        const {return nCopy; };//sens. slice
    Short_t GetCopyMother()  const {return nCopyMother; };//cell
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

//---------------------------------------------------------

/** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  void AddCELL(Int_t trackID, Int_t detID, Int_t idslice, Int_t idcell, TVector3 pos,TVector3 mom,Double_t dt, Double_t dl, Double_t de) { 
    if(nCopy != idslice)
      cerr << "Warning: idslice not equal in BmnScWallPoint::AddCELL";
    if(nCopyMother != idcell)
      cerr << "Warning: idcell not equal in BmnScWallPoint::AddCELL";
    fTrackID=trackID; fDetectorID=detID;
    fX=pos.X(); fY=pos.Y(); fZ=pos.Z();
    fPx=mom.Px(); fPy=mom.Py(); fPz=mom.Pz();
    nCopy=idslice; nCopyMother=idcell; fELoss += de; fLength += dl; fTime+=dt;
  }

 protected:
  Short_t nCopy;                // Copy number //slice
  Short_t nCopyMother;          // Copy number of mother volume //cell
  Double_t fX, fY, fZ;
  Double_t fPx, fPy, fPz;
  Double_t fTime;
  Double_t fLengthtrack;
  
  //ClassDef(BmnScWallPoint, 0)
  ClassDef(BmnScWallPoint, 1)
    };

#endif
