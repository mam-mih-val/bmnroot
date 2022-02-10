// ------------------------------------------------------------------------
// -----                     BmnFHCalPoint header file                  -----
// -----                     litvin@nf.jinr.ru                        -----
// -----                     Last updated 22-Feb-2012                 -----
//
//                           Modified by M.Golubeva
// ------------------------------------------------------------------------

#ifndef BMNFHCALPOINT_H
#define BMNFHCALPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

using namespace std;

class BmnFHCalPoint : public FairMCPoint {

public:

  /** Default constructor **/
  BmnFHCalPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param copyNo         Number of active layer inside FHCal module
   *@param copyNoMother   FHCal module number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/

  BmnFHCalPoint(Int_t trackID, Int_t detID,
    Int_t copyNo, Int_t copyNoMother,
    TVector3 pos, TVector3 mom,
    Double_t tof, Double_t length,
    Double_t eLoss, UInt_t EventId = 0);

  /** Copy constructor **/
  BmnFHCalPoint(const BmnFHCalPoint& point) { *this = point; };

  /** Destructor **/
  virtual ~BmnFHCalPoint();

  /** Accessors **/
  Short_t GetCopy()        const { return nCopy; };
  Short_t GetCopyMother()  const { return nCopyMother; };
  //Short_t GetCopyFHCal()  const {return nCopyFHCal; };//FHCal (left, right)

/** Modifiers **/
  void SetCopy(Short_t i) { nCopy = i; };
  void SetCopyMother(Short_t i) { nCopyMother = i; };
  //void SetCopyFHCal(Short_t i)    { nCopyFHCal  = i; }; 

/** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  //  fTrackID    = trackID;
  //  fDetectorID = detID; 

  //  fX          = pos.X();
  //  fY          = pos.Y();
  //  fZ          = pos.Z();
  //  fPx         = mom.Px();
  //  fPy         = mom.Py();
  //  fPz         = mom.Pz();
  //  fTime       = tof;
  //  fLength     = length;
  //  fELoss      = eLoss;
  //  fEventId

  void AddVSC(Int_t trackID, Int_t detID, Int_t idvsc, Int_t idmod, TVector3 pos, TVector3 mom, Double_t dt, Double_t dl, Double_t de) {
    if (nCopy != idvsc)
      cerr << "Warning: idvsc not equal in BmnFHCalPoint::AddVSC";
    if (nCopyMother != idmod)
      cerr << "Warning: idmod not equal in BmNFHCalPoint::AddVSC";
    fTrackID = trackID; fDetectorID = detID;
    fX = pos.X(); fY = pos.Y(); fZ = pos.Z();
    fPx = mom.Px(); fPy = mom.Py(); fPz = mom.Pz();
    nCopy = idvsc; nCopyMother = idmod; fELoss += de; fLength += dl; fTime += dt;
  }

protected:

  Short_t nCopy;                // Copy number 
  Short_t nCopyMother;          // Copy number of mother volume 
  //Short_t nCopyFHCal;             // Copy number of FHCal (left, right)

  ClassDef(BmnFHCalPoint, 1)

};

#endif
