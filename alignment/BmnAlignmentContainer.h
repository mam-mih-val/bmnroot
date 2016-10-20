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
    
    Int_t GetTrackIndex() {
        return fIndex;
    }
        
    // Setters         
    void SetEventNum(Int_t ev) {
        fEventNumber = ev;
    }
        
    void SetTrackIndex(Int_t idx) {
        fIndex = idx;
    }
     
private:

    Int_t fEventNumber;
    Int_t fIndex; // track index

    ClassDef(BmnAlignmentContainer, 1)
};

#endif