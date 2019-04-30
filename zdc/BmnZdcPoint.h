// ------------------------------------------------------------------------
// -----                     BmnZdcPoint header file                  -----
// -----                     litvin@nf.jinr.ru                        -----
// -----                     Last updated 22-Feb-2012                 -----
// ------------------------------------------------------------------------

#ifndef BMNZDCPOINT_H
#define BMNZDCPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

using namespace std;

class BmnZdcPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  BmnZdcPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param module_groupID    Detector ID (at present, volume MC number)
   *@param copyNo         Number of active layer inside ZDC module
   *@param copyNoMother   ZDC module number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  
  BmnZdcPoint(Int_t trackID, Int_t module_groupID, 
	      Int_t copyNo, Int_t copyNoMother, 
	      TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, 
	      Double_t eLoss, UInt_t EventId=0 );
  
  /** Copy constructor **/
  BmnZdcPoint(const BmnZdcPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~BmnZdcPoint();
  

  /** Accessors **/
  Short_t GetCopy()        const {return nCopy; };
  Short_t GetCopyMother()  const {return nCopyMother; };

  /** Modifiers **/
  void SetCopy(Short_t i)          { nCopy    = i; }; 
  void SetCopyMother(Short_t i)    { nCopyMother  = i; }; 
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  void AddVSC(Int_t trackID, Int_t detID, Int_t idvsc, Int_t idmod,
              TVector3 pos,TVector3 mom,Double_t dt, Double_t dl, Double_t de) {
      if(nCopy != idvsc)
        cerr << "Warning: idvsc not equal in BmnZdcPoint::AddVSC";
      if(nCopyMother != idmod)
        cerr << "Warning: idmod not equal in BmnZdcPoint::AddVSC";
      fTrackID = trackID; fDetectorID = detID;
      fX = pos.X(); fY = pos.Y(); fZ = pos.Z();
      fPx = mom.Px(); fPy = mom.Py(); fPz = mom.Pz();
      nCopy = idvsc; nCopyMother = idmod; fELoss += de; fLength += dl; fTime+=dt;
    }

 protected:

  Short_t nCopy;                // Copy number (for scintillator layer)
  Short_t nCopyMother;          // Copy number of mother volume (for Module inside its Group)
    
  ClassDef(BmnZdcPoint,2)

};

#endif
