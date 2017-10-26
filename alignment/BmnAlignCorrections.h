#ifndef BMNALIGNCORRECTIONS_H
#define BMNALIGNCORRECTIONS_H 1

#include <iostream>
#include <TNamed.h>
#include <TVector3.h>

using namespace std;

class BmnAlignCorrections : public TNamed {
public:

    BmnAlignCorrections() {
    };

    BmnAlignCorrections(TVector3);

    virtual ~BmnAlignCorrections() {};

    void SetCorrections(Double_t x, Double_t y, Double_t z) {
        fX = x;
        fY = y;
        fZ = z;
    }
    
    void SetCorrections(Double_t* corrs) {
        fX = corrs[0];
        fY = corrs[1];
        fZ = corrs[2];
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

    TVector3 GetCorrections() {
        return TVector3(fX, fY, fZ);
    }

private:
    Double_t fX;
    Double_t fY;
    Double_t fZ;

    ClassDef(BmnAlignCorrections, 1)
};

#endif
