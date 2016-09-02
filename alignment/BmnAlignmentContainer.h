#ifndef BMNALIGNMENTCONTAINER_H
#define BMNALIGNMENTCONTAINER_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>
#include <BmnGemTrack.h>
#include <float.h>

using namespace std;

class BmnAlignmentContainer : public BmnGemTrack {
public:
    BmnAlignmentContainer();
    
    virtual ~BmnAlignmentContainer();

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
      
    Double_t fXresMax;
    Double_t fYresMax;
    
    Int_t fIndex; // track index

    ClassDef(BmnAlignmentContainer, 1)
};

#endif