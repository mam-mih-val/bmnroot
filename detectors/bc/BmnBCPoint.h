// ------------------------------------------------------------------------
// -----                     BmnBdPoint header file                  -----
// -----                     litvin@nf.jinr.ru                        -----
// -----                     Last updated 10-02-2016                 -----
// ------------------------------------------------------------------------

#ifndef BMNBCPOINT_H
#define BMNBCPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "Rtypes.h"   
#include "TVirtualMC.h"
#include "TParticle.h"

using namespace std;

class BmnBCPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  BmnBCPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param copyNo         Number of active layer inside BD module
   *@param copyNoMother   BD module number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  
  BmnBCPoint(Int_t trackID, Int_t detID, 
	      Int_t copyNo, 
	       TVector3 posIn, TVector3 posOut,
           TVector3 momIn, TVector3 momOut,
	       Double_t tof, Double_t length, Double_t eLoss, Int_t fStat,
		   Int_t isPrimary, Double_t charge, Int_t pdgId, Double_t lightYield,
		   Double_t timeIn, Double_t timeOut, Double_t lengthtrack);
 // UInt_t EventId, 
  /** Copy constructor **/
  BmnBCPoint(const BmnBCPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~BmnBCPoint();
  

  /** Accessors **/
  Short_t GetCopy()        const {return nCopy; };
  //  Short_t GetCopyMother()  const {return nCopyMother; };
  
  Int_t GetIsPrimary() { return fIsPrimary; }
  Double_t GetCharge() { return fCharge; }
  Double_t GetPdgId()  { return fPdgId; }
  Double_t GetTimeIn()       const { return fTimeIn; }
  Double_t GetTimeOut()      const { return fTimeOut; }
  Double_t GetLengthtrack()     const { return fLengthtrack; }	
  Double_t GetStation()     const { return fStation; }	
 
	
	Double_t GetPxOut() const { return fPxOut; }
    Double_t GetPyOut() const { return fPyOut; }
    Double_t GetPzOut() const { return fPzOut; }
	
    void MomentumOut(TVector3& mom) { mom.SetXYZ(fPxOut,fPyOut,fPzOut);}
		
    Double_t GetXOut()  const { return fXOut; }
    Double_t GetYOut()  const { return fYOut; }
    Double_t GetZOut()  const { return fZOut; }
		
	void PositionOut(TVector3& pos) { pos.SetXYZ(fXOut,fYOut,fZOut);}
    
	
	// ---------------------------------------------------------
    Float_t GetLightYield() const {return fLightYield;}

  //void  GetPolarisation(TVector3 &v)  const;	


	/*  
	Double_t GetX()             const { return fX1;      };
    Double_t GetY()             const { return fY1;      };
    Double_t GetZ()             const { return fZ1;      };
	
	void Position(TVector3& posprim) const { posprim.SetXYZ(fX1, fY1, fZ1); }
*/
	
  /** Modifiers **/
   void SetCopy(Short_t i)          { nCopy    = i; }; 
  //  void SetCopyMother(Short_t i)    { nCopyMother  = i; }; 

														 
 
   //void SetProductionVertex(Double_t vx, Double_t vy, Double_t vz)	{fVx=vx; fVy=vy; fVz=vz;}	
														 
	/** Output to screen **/
  virtual void Print(const Option_t* opt) const;

  
 protected:
	
	Double_t fXOut,  fYOut,  fZOut;
    Double_t fPxOut, fPyOut, fPzOut;	
  
	Short_t nCopy;                // Copy number
  //  Short_t nCopyMother;          // Copy number of mother volume

    
	Int_t fIsPrimary;
  Int_t fStation;
  Double_t fCharge;
  Int_t fPdgId;
  Float_t fLightYield;
	Double_t fTimeIn;
	Double_t fTimeOut;
	Double_t fLengthtrack;
	
			
  ClassDef(BmnBCPoint,5)
};


#endif
