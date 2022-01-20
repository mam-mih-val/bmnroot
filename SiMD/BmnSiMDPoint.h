// ------------------------------------------------------------------------
// -----                     BmnFdPoint header file                   -----
// ------------------------------------------------------------------------

#ifndef BMNSiMDPOINT_H
#define BMNSiMDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "Rtypes.h"   
#include "TVirtualMC.h"
#include "TParticle.h"

using namespace std;

class BmnSiMDPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  BmnSiMDPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param copyNo         Number of active layer inside SiMD module
   *@param copyNoMother   SiMD module number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  
  BmnSiMDPoint(Int_t trackID, Int_t detID, 
	      Int_t copyNo, 
	       TVector3 posIn, TVector3 posOut,
           TVector3 momIn, TVector3 momOut,
	       Double_t tof, Double_t length, Double_t eLoss, 
		   Int_t isPrimary, Double_t charge, Int_t pdgId,
		   Double_t timeIn, Double_t timeOut, Double_t lengthtrack);
 // UInt_t EventId, 
  /** Copy constructor **/
  BmnSiMDPoint(const BmnSiMDPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~BmnSiMDPoint();
  

  /** Accessors **/
  Short_t GetCopy()        const {return nCopy; };
  //  Short_t GetCopyMother()  const {return nCopyMother; };
  
  Int_t GetIsPrimary() { return fIsPrimary; }
  Double_t GetCharge() { return fCharge; }
  Double_t GetPdgId()  { return fPdgId; }
  Double_t GetTimeIn()       const { return fTimeIn; }
  Double_t GetTimeOut()      const { return fTimeOut; }
  Double_t GetLengthtrack()     const { return fLengthtrack; }	
 
	
	Double_t GetPxOut() const { return fPxOut; }
    Double_t GetPyOut() const { return fPyOut; }
    Double_t GetPzOut() const { return fPzOut; }
	
    void MomentumOut(TVector3& mom) { mom.SetXYZ(fPxOut,fPyOut,fPzOut);}
		
    Double_t GetXOut()  const { return fXOut; }
    Double_t GetYOut()  const { return fYOut; }
    Double_t GetZOut()  const { return fZOut; }
		
	void PositionOut(TVector3& pos) { pos.SetXYZ(fXOut,fYOut,fZOut);}
    
	// ---------------------------------------------------------

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
    Double_t fCharge;
    Int_t fPdgId;
	Double_t fTimeIn;
	Double_t fTimeOut;
	Double_t fLengthtrack;
	
			
  ClassDef(BmnSiMDPoint,5)
//4
};


#endif
