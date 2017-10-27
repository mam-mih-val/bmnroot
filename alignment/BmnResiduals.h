#ifndef BMNRESIDUALS_H
#define BMNRESIDUALS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>

using namespace std;

class BmnResiduals : public TNamed {
public:

    BmnResiduals();

    BmnResiduals(Int_t, Int_t, Double_t, Double_t, Double_t, Bool_t, Bool_t);

    virtual ~BmnResiduals() {
    };

    void SetResiduals(Double_t dx, Double_t dy, Double_t dz) {
        fdX = dx;
        fdY = dy;
        fdZ = dz;
    }

    void SetdX(Double_t dx) {
        fdX = dx;
    }

    void SetdY(Double_t dy) {
        fdY = dy;
    }

    void SetdZ(Double_t dz) {
        fdZ = dz;
    }

    TVector3 GetResiduals() {
        return TVector3(fdX, fdY, fdZ);
    }

    Int_t GetStation() {
        return fStation;
    }

    Int_t GetModule() {
        return fModule;
    }

    void SetTrackId(Int_t id) {
        fTrackId = id;
    }

    void SetHitId(Int_t id) {
        fHitId = id;
    }

    void SetIsMergedDigits(Bool_t flag) {
        isMergedDigits = flag;
    }

    Bool_t GetIsMergedDigits() {
        return isMergedDigits;
    }

    Int_t GetHitId() {
        return fHitId;
    }

private:
    Double_t fdX;
    Double_t fdY;
    Double_t fdZ;

    Int_t fStation;
    Int_t fModule;

    Int_t fTrackId;
    Int_t fHitId;

    Bool_t isResid; // Calculate resid. or distance
    Bool_t isField; // Is mag. field or not
    Bool_t isMergedDigits; // Is reco file obtained from merged digits or not

    ClassDef(BmnResiduals, 1)
};

#endif
