/* 
 * File:   BmnCellDuet.h
 * Author: merz
 *
 * Created on July 30, 2018, 1:41 PM
 */

#ifndef BMNCELLDUET_H
#define BMNCELLDUET_H

#include "TNamed.h"
#include "BmnHit.h"
#include <vector>

using namespace std;

class BmnCellDuet : public TNamed {
private:
    Short_t OldState;
    Short_t NewState;
    Int_t FirstIdx;
    Int_t LastIdx;
    Double_t SlopeXZ;
    Double_t SlopeYZ;
    Short_t StartPlane;
    Bool_t Used;

public:

    BmnCellDuet();

    virtual ~BmnCellDuet();

    void SetNewState(Short_t st) {
        NewState = st;
    };

    void SetOldState(Short_t st) {
        OldState = st;
    };

    void SetFirstIdx(Int_t idx) {
        FirstIdx = idx;
    };
    
    void SetLastIdx(Int_t idx) {
        LastIdx = idx;
    };

    void SetSlopeXZ(Double_t s) {
        SlopeXZ = s;
    }
    
    void SetSlopeYZ(Double_t s) {
        SlopeYZ = s;
    }

    void SetStartPlane(Short_t p) {
        StartPlane = p;
    }

    Int_t GetFirstIdx() {
        return FirstIdx;
    }
    
    Int_t GetLastIdx() {
        return LastIdx;
    }

    Double_t GetSlopeXZ() {
        return SlopeXZ;
    }
    
    Double_t GetSlopeYZ() {
        return SlopeYZ;
    }

    Short_t GetOldState() {
        return OldState;
    }

    Short_t GetNewState() {
        return NewState;
    }

    Short_t GetStartPlane() {
        return StartPlane;
    }
    
    void SetUsing(Bool_t u) {
        Used = u;
    }
    
    Bool_t isUsed() {
        return Used;
    }

    ClassDef(BmnCellDuet, 1);
};

#endif /* BMNCELLDUET_H */

