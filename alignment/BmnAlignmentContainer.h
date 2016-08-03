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
    
    const vector <BmnGemStripHit*> GetTrackHits() {
        return fTrackHits;
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
    
    void SetTrackHits(vector <BmnGemStripHit*> hits) {
        fTrackHits = hits;
    }
   
    
private:

//    Int_t fEventNumber;
    vector <BmnGemStripHit*> fTrackHits;
    
//    Int_t fStation;
//    Int_t fModule;
//    Int_t fLayer;
    
//    Float_t fX;
//    Float_t fY;
//    Float_t fZ;
//    Float_t fdX;
//    Float_t fdY;
//    Float_t fdZ;
   
    Float_t fX0;
    Float_t fY0;
    Float_t fZ0;
    Float_t fTx;
    Float_t fTy;

    ClassDef(BmnAlignmentContainer, 1)
};

#endif