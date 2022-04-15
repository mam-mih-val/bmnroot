#ifndef BMNGEMSTRIPMEDIUM_H
#define BMNGEMSTRIPMEDIUM_H

#include "BmnGemStripMediumConfiguration.h"

#include "TROOT.h"

class BmnGemStripMedium {

public:

    static BmnGemStripMedium& GetInstance() {
        static BmnGemStripMedium medium;
        return medium;
    }

private:
    BmnGemStripMedium();  //this constructor is not available for explicitly calling
    ~BmnGemStripMedium();
    BmnGemStripMedium(BmnGemStripMedium const&) = delete;
    BmnGemStripMedium& operator= (BmnGemStripMedium const&) = delete;

public:
    Bool_t SetCurrentConfiguration(BmnGemStripMediumConfiguration::MEDIUM medium);


//private:
public:
    TString NameOfCurrentMedium;
    Double_t MCD; //Mean collision distance

    //Polynomial coefficients for f(x) = (p0 + p1*x + p2*x^2 + p3*x^3) (describing the average shift of cluster centers)
    Double_t p0_xmean;
    Double_t p1_xmean;
    Double_t p2_xmean;
    Double_t p3_xmean;

    //Polynomial coefficients for f(x) = (p0 + p1*x + p2*x^2 + p3*x^3+ p4*x^4 + p5*x^5) (describing the average scattering of cluster centers)
    Double_t p0_sigma;
    Double_t p1_sigma;
    Double_t p2_sigma;
    Double_t p3_sigma;
    Double_t p4_sigma;
    Double_t p5_sigma;

    ClassDef(BmnGemStripMedium, 1);
};

#endif
