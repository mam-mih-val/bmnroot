#ifndef BMNSTTHIT_H
#define BMNSTTHIT_H

#include "FairHit.h"

#include "TArrayI.h"

class BmnSttHit : public FairHit
{

 public:

  /** Default constructor **/
  BmnSttHit();

  /** Constructor with hit parameters (1)**/
  BmnSttHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Int_t flag);

  /** Constructor with hit parameters (2) [not the flag]**/
  BmnSttHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex);

  /** Destructor **/
  virtual ~BmnSttHit();

  void Print(const Option_t* opt = 0) const;

  /** Accessors **/
  Int_t GetFlag() const { return fFlag; } // get flag
  Int_t GetNofDim() const { return fNofDim; } // get number of measurements per point
  Int_t Overlap() const { return fIndex.GetSize()-1; } // get number of measurements per point
  Int_t GetIndex(Int_t indx = 0) const { return fIndex[indx]; } // get number of measurements per point
  Int_t GetLayer() const { return GetDetectorID() / 1000; } // get number of measurements per point
  Double_t GetPhi() const { return fPhi; } // get rotation angle 
  Double_t GetMeas(Int_t indx = 0) const { return fMeas[indx]; } // get measurement 
  Double_t GetError(Int_t indx = 0) const { return fError[indx]; } // get measurement 
  const TArrayI* Index() const { return &fIndex; } ///< Get index array of the hit

  /** Modifiers **/
  void SetFlag(Int_t flag) { fFlag = flag; }
  void SetNofDim(Int_t dim) { fNofDim = dim; } // set number of measurements per point
  void SetPhi(Double_t phi) { fPhi = phi; } // set rotation angle 
  void SetMeas(Double_t meas, Int_t indx = 0) { fMeas[indx] = meas; } // set measurement 
  void SetError(Double_t err, Int_t indx = 0) { fError[indx] = err; } // set measurement 
  void SetIndex(Int_t indx); ///< Add index of detector hit
  
  Bool_t IsSortable() const { return kTRUE; }
  Int_t Compare(const TObject* hit) const; ///< sort in ascending order in abs(Z)

 protected:

  Int_t fTrackID;
  Int_t fFlag; ///< Flag for general purposes [TDC, event tagging...]
  Int_t fNofDim; ///< number of measurements per point
  Double32_t fPhi; ///< tube rotation angle
  Double32_t fMeas[2]; ///< measurements (coordinates)
  Double32_t fError[2]; ///< measurement errors
  TArrayI fIndex; ///< array of indices of overlapped MC points

  ClassDef(BmnSttHit,1)

};


#endif
