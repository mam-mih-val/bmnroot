#ifndef BMNALIGNMENTCONTAINER_H
#define BMNALIGNMENTCONTAINER_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include "BmnGemStripHit.h"

using namespace std;

class BmnAlignmentContainer : public TNamed {
public:
    BmnAlignmentContainer();
    //BmnAlignmentContainer();
    
    virtual ~BmnAlignmentContainer();

    // Getters 
    Float_t GetTx() {
        return fTx;
    }
    
    Float_t GetTy() {
        return fTy;
    }
    
    Float_t GetX0() {
        return fX0;
    }
    
    Float_t GetY0() {
        return fY0;
    }
   
    Float_t GetZ0() {
        return fZ0;
    }
    
    TClonesArray* GetTrackHits() {
        return fTrackHits;
    } 
    
    Double_t GetXresMax() {
        return fXresMax;
    }
    
    Double_t GetYresMax() {
        return fYresMax;
    }
    
    Int_t GetTrackIndex() {
        return fIndex;
    }
        
    // Setters   
    void SetX0(Float_t x0) {
        fX0 = x0;
    }
    
    void SetY0(Float_t y0) {
        fY0 = y0;
    }
    
    void SetZ0(Float_t z0) {
        fZ0 = z0;
    }
    
    void SetTx(Float_t tx) {
        fTx = tx;
    }
    
    void SetTy(Float_t ty) {
        fTy = ty;
    }
    
    void SetTrackHits(TClonesArray* hits) {
        fTrackHits = hits;
    }
    
    void SetEventNum(Int_t ev) {
        fEventNumber = ev;
    }
    
    void SetXresMax(Double_t val) {
        fXresMax = val;
    }
    
    void SetYresMax(Double_t val) {
        fYresMax = val;
    }
    
    void SetTrackIndex(Int_t idx) {
        fIndex = idx;
    }
     
private:

    Int_t fEventNumber;
    TClonesArray* fTrackHits;
      
    Float_t fX0;
    Float_t fY0;
    Float_t fZ0;
    Float_t fTx;
    Float_t fTy;
    
    Double_t fXresMax;
    Double_t fYresMax;
    
    Int_t fIndex; // track index

    ClassDef(BmnAlignmentContainer, 1)
};

#endif