// ------------------------------------------------------------------------
// -----                     BmnNdetPoint header file                  -----
// -----                     litvin@nf.jinr.ru                        -----
// -----                     Last updated 22-Feb-2012                 -----
//
//                           Modified by M.Golubeva July 2022
// ------------------------------------------------------------------------

#ifndef BMNNDETPOINT_H
#define BMNNDETPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

using namespace std;

class BmnNdetPoint : public FairMCPoint
{

 public:

/** Default constructor **/
  BmnNdetPoint();


/** Constructor with arguments
 *@param trackID  Index of MCTrack
 *@param detID    Detector ID (at present, volume MC number)
 *@param copyNo         Number of active layer inside NDET module
 *@param copyNoMother   NDET module number
 *@param pos      Coordinates  [cm]
 *@param mom      Momentum of track [GeV]
 *@param tof      min Time in scint [ns]
 *@param length   Track length since creation [cm]
 *@param eLoss    Energy deposit [GeV]
 #*@param nHits    number of hits in scint
 **/
  
  BmnNdetPoint(Int_t trackID, Int_t detID, 
	      Int_t copyNo, Int_t copyNoMother, 
	      TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, 
	      Double_t eLoss, UInt_t EventId=0 );
  
/** Copy constructor **/
  BmnNdetPoint(const BmnNdetPoint& point) { *this = point; };
  
/** Destructor **/
  virtual ~BmnNdetPoint();
  
  /** Accessors **/
  Short_t GetCopy()        const {return nCopy; };
  Short_t GetCopyMother()  const {return nCopyMother; };
  //int GetNHits()   {return fNHits; }; 

/** Modifiers **/
  void SetCopy(Short_t i)          { nCopy    = i; }; 
  void SetCopyMother(Short_t i)    { nCopyMother  = i; }; 
  //void SetNHits(Int_t i)    { fNHits  = i; }; 
   
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
  //  //fNHits      = nHits;
  //  fEventId

  void AddVSC(Int_t trackID, Int_t detID, Int_t idvsc, Int_t idmod, TVector3 pos,TVector3 mom,Double_t dt, Double_t dl, Double_t de, UInt_t EventId) { 
    if(nCopy != idvsc)
      cerr << "Warning: idvsc not equal in BmnNdetPoint::AddVSC";
    if(nCopyMother != idmod)
      cerr << "Warning: idmod not equal in BmNNdetPoint::AddVSC";
    fTrackID=trackID; fDetectorID=detID; 
    fX=pos.X(); fY=pos.Y(); fZ=pos.Z();
    fPx=mom.Px(); fPy=mom.Py(); fPz=mom.Pz();
    //nCopy=idvsc; nCopyMother=idmod; fELoss += de; fLength += dl; fTime+=dt;
    nCopy=idvsc; nCopyMother=idmod; fELoss += de; fLength += dl; fTime = dt;
    //nCopy=idvsc; nCopyMother=idmod; fELoss += de; fLength += dl; fTime = dt; fEventID=EventId;
    //nCopy=idvsc; nCopyMother=idmod; fELoss += de; fLength += dl; fTime = dt; fNHits += nhits;;
  }
  
 protected:

  Short_t nCopy;                // Copy number 
  Short_t nCopyMother;          // Copy number of mother volume 
  //Int_t fNHits                 //number of hits in slice

  ClassDef(BmnNdetPoint,2)
    
};

#endif
