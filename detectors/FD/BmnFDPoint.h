
#ifndef BMNFDPOINT_H
#define BMNFDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "Rtypes.h"   
#include "TVirtualMC.h"
#include "TParticle.h"

using namespace std;

class BmnFDPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  BmnFDPoint();
  
  BmnFDPoint(Int_t trackID, Int_t detID, 
	      Int_t copyNo, 
	       TVector3 posIn, TVector3 posOut,
           TVector3 momIn, TVector3 momOut,
	       Double_t tof, Double_t length, Double_t eLoss, 
		   Int_t isPrimary, Double_t charge, Int_t pdgId, Double_t lightYield,
		   Double_t timeIn, Double_t timeOut, Double_t lengthtrack);
 // UInt_t EventId, 
  /** Copy constructor **/
  BmnFDPoint(const BmnFDPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~BmnFDPoint();
  

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
    Float_t GetLightYield() const {return fLightYield;}

  //void  GetPolarisation(TVector3 &v)  const;	

	
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
    Float_t fLightYield;
	Double_t fTimeIn;
	Double_t fTimeOut;
	Double_t fLengthtrack;
	
			
  ClassDef(BmnFDPoint,5)
//4
};


#endif
