// ------------------------------------------------------------------------
// -----                     BmnEcalPoint header file                  -----
// -----                     litvin@nf.jinr.ru                        -----
// -----                     Last updated 10-02-2016                 -----
// ------------------------------------------------------------------------

#ifndef BMNECALPOINT_H
#define BMNECALPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"

using namespace std;

class BmnEcalPoint : public FairMCPoint
{

 public:

  /** Default constructor **/
  BmnEcalPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID (at present, volume MC number)
   *@param copyNo         Number of active layer inside ECAL module
   *@param copyNoMother   ECAL module number
   *@param pos      Coordinates  [cm]
   *@param mom      Momentum of track [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  
  BmnEcalPoint(Int_t trackID, Int_t detID, 
	      Int_t copyNo, Int_t copyNoMother, 
	      TVector3 pos, TVector3 mom,
	      Double_t tof, Double_t length, 
	      Double_t eLoss, UInt_t EventId=0 );
  
  /** Copy constructor **/
  BmnEcalPoint(const BmnEcalPoint& point) { *this = point; };
  

  /** Destructor **/
  virtual ~BmnEcalPoint();
  

  /** Accessors **/
  Short_t GetCopy()        const {return nCopy; };
  Short_t GetCopyMother()  const {return nCopyMother; };

  /** Modifiers **/
  void SetCopy(Short_t i)          { nCopy    = i; }; 
  void SetCopyMother(Short_t i)    { nCopyMother  = i; }; 
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:

  Short_t nCopy;                // Copy number
  Short_t nCopyMother;          // Copy number of mother volume
    
  ClassDef(BmnEcalPoint,4)

};

#endif
