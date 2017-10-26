#ifndef BMNGEMALIGNMENTCORRECTIONS_H
#define BMNGEMALIGNMENTCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>

using namespace std;

class BmnGemAlignmentCorrections : public TNamed {
public:

    BmnGemAlignmentCorrections() {
    };

    BmnGemAlignmentCorrections(Int_t, Int_t, TVector3);
    
    virtual ~BmnGemAlignmentCorrections() {
    };

    void SetStation(Int_t num) {
        fStation = num;
    }

    void SetModule(Int_t num) {
        fModule = num;
    }

    void SetCorrections(Double_t x, Double_t y, Double_t z) {
        fX = x;
        fY = y;
        fZ = z;
    }
    
    void SetX(Double_t x) {
        fX = x;  
    }
    
    void SetY(Double_t y) {
        fY = y;  
    }
    
    void SetZ(Double_t z) {
        fZ = z;  
    }
    
    Int_t GetStation() {
        return fStation;
    }
    
    Int_t GetModule() {
        return fModule;
    }

    TVector3 GetCorrections() {
        return TVector3(fX, fY, fZ);
    }


private:
    Int_t fStation;
    Int_t fModule;
    Double_t fX;
    Double_t fY;
    Double_t fZ;


    ClassDef(BmnGemAlignmentCorrections, 1)
};

#endif
