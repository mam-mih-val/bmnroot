//------------------------------------------------------------------------------------------------------------------------
#ifndef __BMN_DCH_HIT_ORIGINAL_H
#define __BMN_DCH_HIT_ORIGINAL_H 1

#include "FairHit.h"

#include "TArrayI.h"
//------------------------------------------------------------------------------------------------------------------------

class BmnDchHitOriginal : public FairHit {
public:

    BmnDchHitOriginal();
    BmnDchHitOriginal(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Int_t flag, Int_t trackIndex, UInt_t dchlayer);
    BmnDchHitOriginal(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex, Int_t flag);
    BmnDchHitOriginal(Int_t detectorID, TVector3 pos, TVector3 dpos, Int_t refIndex);
    

    virtual ~BmnDchHitOriginal();

    void Print(const Option_t* opt = 0) const;

    /** Accessors **/
    Int_t GetTrackID() const {
        return fTrackID;
    } // get track ID

    Int_t GetFlag() const {
        return fFlag;
    } // get flag

    Int_t GetNofDim() const {
        return fNofDim;
    } // get number of measurements per point

    Int_t Overlap() const {
        return fIndex.GetSize() - 1;
    } // 

    Int_t GetIndex(Int_t indx = 0) const {
        return fIndex[indx];
    } // 

    UInt_t GetLayer() const {
        return fDchLayer;
    } // 

    Int_t GetDetectorID() const {
        return fDetectorID;
    } // 

    Double_t GetPhi() const {
        return fPhi;
    } // get rotation angle 

    Double_t GetMeas(Int_t indx = 0) const {
        return fMeas[indx];
    } // get measurement 

    Double_t GetError(Int_t indx = 0) const {
        return fError[indx];
    } // get measurement error

    const TArrayI* Index() const {
        return &fIndex;
    } ///< Get index array of the hit

    Double_t GetDrift(void) const {
        return fDriftLength;
    };

    void SetDrift(Double_t v) {
        fDriftLength = v;
    };

    Double_t GetWirePosition(void) const {
        return fWirePos;
    };

    void SetWirePosition(Double_t v) {
        fWirePos = v;
    };

    Double_t GetWireDelay(void) const {
        return fWireDelayLength;
    };

    void SetWireDelay(Double_t v) {
        fWireDelayLength = v;
    };

    Double_t GetTShift(void) const {
        return fTS;
    };

    void SetTShift(Double_t v) {
        fTS = v;
    };

    /** Modifiers **/
    //void SetTrackId(Int_t trackID) { fTrackID = trackID; }

    void SetFlag(Int_t flag) {
        fFlag = flag;
    }

    void SetNofDim(Int_t dim) {
        fNofDim = dim;
    } // set number of measurements per point

    void SetPhi(Double_t phi) {
        fPhi = phi;
    } // set rotation angle 

    void SetMeas(Double_t meas, Int_t indx = 0) {
        fMeas[indx] = meas;
    } // set measurement 

    void SetError(Double_t err, Int_t indx = 0) {
        fError[indx] = err;
    } // set measurement 
    
    void SetIndex(Int_t indx); ///< Add index of detector hit

    Bool_t IsSortable() const {
        return kTRUE;
    }
    Int_t Compare(const TObject* hit) const; ///< sort in ascending order in abs(Z)

    Bool_t IsUsed() const {
        return fUsing;
    }

    void SetUsing(Bool_t use) {
        fUsing = use;
    }



protected:

    Int_t fDetectorID; // track ID
    UInt_t fDchLayer; // Dch layer ID (0-7)
    Int_t fTrackID; // track ID
    Int_t fFlag; // Flag for general purposes [TDC, event tagging...]
    Int_t fNofDim; // number of measurements per point
    Double32_t fPhi; // tube rotation angle
    Double32_t fMeas[2]; // measurements (coordinates)
    Double32_t fError[2]; // measurement errors
    TArrayI fIndex; // array of indices of overlapped MC points

    Double32_t fDriftLength, fWirePos, fWireDelayLength, fTS;

    Bool_t fUsing;

    ClassDef(BmnDchHitOriginal, 1)

};
//------------------------------------------------------------------------------------------------------------------------
#endif
