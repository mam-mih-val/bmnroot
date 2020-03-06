//------------------------------------------------------------------------------------------------------------------------
#ifndef __BMN_DCH_HIT_H
#define __BMN_DCH_HIT_H 1

#include "BmnHit.h"

#include "TArrayI.h"
//------------------------------------------------------------------------------------------------------------------------
class BmnDchHit : public BmnHit {

public:

  BmnDchHit();
  BmnDchHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Int_t flag, Int_t trackIndex, UChar_t dchlayer);
  BmnDchHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Int_t flag);
  BmnDchHit(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex);


  virtual ~BmnDchHit();

  void Print(const Option_t* opt = 0) const;

  /** Accessors **/
  Int_t GetTrackID() const { return fTrackID; } // get track ID
  Int_t GetFlag() const { return fFlag; } // get flag
  Int_t GetNofDim() const { return fNofDim; } // get number of measurements per point
  //Int_t Overlap() const { return fIndex.GetSize()-1; } //
  //Int_t GetIndex(Int_t indx = 0) const { return fIndex[indx]; } //
  UChar_t GetLayer() const { return  fDchLayer; } //
  UShort_t GetLayerNumber() const { return  fDchLayerNumber; } //
  TString GetLayerType() const { return fDchLayerType;}
  Int_t GetWireNumber() { return fWireNumber; }
  Double_t GetDistanceToWire() { return fDistanceToWire; }
  Int_t GetDetectorID() const { return  fDetectorID; } //
  Double_t GetPhi() const { return fPhi; } // get rotation angle
  Double_t GetMeas(Int_t indx = 0) const { return fMeas[indx]; } // get measurement
  Double_t GetError(Int_t indx = 0) const { return fError[indx]; } // get measurement error
  //const TArrayI* Index() const { return &fIndex; } ///< Get index array of the hit
  UChar_t GetDchId() const { return fDchId; }
  UShort_t GetDchIdNumber() const { return fDchIdNumber; }
  Int_t GetHitId() const { return fID; }
  Int_t GetEventID() const { return fEventID; }

        Double_t	GetDrift(void) const {return fDriftLength; };
	void		SetDrift(Double_t v) { fDriftLength = v; };
	Double_t	GetWirePosition(void) const {return fWirePos; };
	void		SetWirePosition(Double_t v) { fWirePos = v; };
	Double_t	GetWireDelay(void) const {return fWireDelayLength; };
	void		SetWireDelay(Double_t v) { fWireDelayLength = v; };
	Double_t	GetTShift(void) const {return fTS; };
	void		SetTShift(Double_t v) { fTS = v; };
        Double_t        GetDriftTime(void) const {return fTS; };
        void            SetDriftTime(Double_t v) { fTS = v; };

  /** Modifiers **/
  //void SetTrackId(Int_t trackID) { fTrackID = trackID; }
  void SetFlag(Int_t flag) { fFlag = flag; }
  void SetDchId(UChar_t id) { fDchId = id; }
  void SetDchIdNumber(UShort_t id) { fDchIdNumber = id; }
  void SetDchLayerNumber(UShort_t id) { fDchLayerNumber = id; }
  void SetDchLayerType(TString layer_type) { fDchLayerType = layer_type; }
  void SetWireNumber(Int_t wire_num) { fWireNumber = wire_num; }
  void SetDistanceToWire(Double_t dist) { fDistanceToWire = dist; }
  void SetNofDim(Int_t dim) { fNofDim = dim; } // set number of measurements per point
  void SetPhi(Double_t phi) { fPhi = phi; } // set rotation angle
  void SetMeas(Double_t meas, Int_t indx = 0) { fMeas[indx] = meas; } // set measurement
  void SetError(Double_t err, Int_t indx = 0) { fError[indx] = err; } // set measurement
  void SetIndex(Int_t indx); ///< Add index of detector hit
  void SetHitId(Int_t idx) { fID = idx; }
  void SetEventID(Int_t iev) { fEventID = iev; }

  Bool_t IsSortable() const { return kTRUE; }
  Int_t Compare(const TObject* hit) const; ///< sort in ascending order in abs(Z)

  Bool_t IsUsed() const {
       return fUsing;
  }

  void SetUsing(Bool_t use) {
        fUsing = use;
  }



protected:

  Int_t fEventID; // identifier of event
  Int_t fID; // identifier of hit in hits array
  Int_t fDetectorID;            // Detector ID
  UChar_t fDchLayer;            // Dch layer ID (0-7)
  UShort_t fDchLayerNumber;     // Dch layer ID (0-7)
  TString fDchLayerType;        // DCH layer type (x, y, v, u)
  Int_t fWireNumber;            // Nearest wire to a MC-track
  Double_t fDistanceToWire;     // Distance from a MC-point to the nearest wire
  Int_t fTrackID;               // track ID
  Int_t fFlag; 			// Flag for general purposes [TDC, event tagging...]
  Int_t fNofDim; 		// number of measurements per point
  Double32_t fPhi; 		// track angle
  Double32_t fMeas[2]; 		// measurements (coordinates)
  Double32_t fError[2]; 	// measurement errors
  //TArrayI fIndex; 		//!    // array of indices of overlapped MC points
  UChar_t fDchId;               // DCH Id (1, 2)
  UShort_t fDchIdNumber;        // DCH Id (1, 2)

  Double32_t	fDriftLength, fWirePos, fWireDelayLength, fTS;

  Bool_t fUsing;

  ClassDef(BmnDchHit,1)

};
//------------------------------------------------------------------------------------------------------------------------
#endif
