#ifndef BMNGEMSTRIPDIGIT_H
#define	BMNGEMSTRIPDIGIT_H

#include <iostream>
#include "Rtypes.h"
#include "TNamed.h"

class BmnGemStripDigit : public TNamed {
public:

    BmnGemStripDigit();
    BmnGemStripDigit(BmnGemStripDigit* digit);
    BmnGemStripDigit(Int_t iStation, Int_t iModule, Int_t iLowStrip, Int_t iUpStrip, Double_t xcoord, Double_t ycoord, Double_t zcoord);

    virtual ~BmnGemStripDigit();

    Double_t GetX() { return fX; }
    Double_t GetY() { return fY; }
    Double_t GetZ() { return fZ; }
    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }
    Int_t GetLowerStrip() { return fLowerStrip; }
    Int_t GetUpperStrip() { return fUpperStrip; }

    void SetX(Double_t x) { fX = x; }
    void SetY(Double_t y) { fY = y; }
    void SetZ(Double_t z) { fZ = z; }
    void SetStation(Int_t st) { fStation = st; }
    void SetModule(Int_t mod) { fModule = mod; }
    void SetLowerStrip(Int_t lstrip) { fLowerStrip = lstrip; }
    void SetUpperStrip(Int_t ustrip) { fUpperStrip = ustrip; }

private:

    Int_t fStation;
    Int_t fModule;
    Int_t fLowerStrip;
    Int_t fUpperStrip;

    Double_t fX;
    Double_t fY;
    Double_t fZ;

    ClassDef(BmnGemStripDigit,1);
};

#endif

