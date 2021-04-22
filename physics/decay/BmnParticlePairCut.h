#include <TObject.h>
#include <TString.h>

#ifndef BMNPARTICLEPAIRCUT_H
#define BMNPARTICLEPAIRCUT_H 1

class BmnParticlePairCut : public TObject {
public:

    BmnParticlePairCut();

    virtual ~BmnParticlePairCut() {
        ;
    }

    void SetDca0(Double_t v) {
        fDCA0 = v;
    }

    void SetDca1(Double_t v) {
        fDCA1 = v;
    }

    void SetDca2(Double_t v) {
        fDCA2 = v;
    }

    void SetDca12(Double_t v) {
        fDCA12 = v;
    }

    void SetPath(Double_t v) {
        fPath = v;
    }

    void SetNHitsGemPos(Int_t nHits) {
        fNhitsGemPos = nHits;
    }

    void SetNHitsGemNeg(Int_t nHits) {
        fNhitsGemNeg = nHits;
    }

    void SetBackground(Double_t b) {
        fBackground = b;
    }

    void SetSignalAndBackground(Double_t sb) {
        fSignalAndBackground = sb;
    }

    void SetMass(Double_t m) {
        fMass = m;
    }

    void SetWidth(Double_t w) {
        fWidth = w;
    }

    void SetOrigin(TString orig) {
        fOrigin = orig;
    }

    Double_t dca0() {
        return fDCA0;
    }

    Double_t dca12() {
        return fDCA12;
    }

    Double_t dca1() {
        return fDCA1;
    }

    Double_t dca2() {
        return fDCA2;
    }

    Double_t path() {
        return fPath;
    }

    Int_t nHitsGemPos() {
        return fNhitsGemPos;
    }

    Int_t nHitsGemNeg() {
        return fNhitsGemNeg;
    }

    Double_t background() {
        return fBackground;
    }

    Double_t signalAndBackground() {
        return fSignalAndBackground;
    }

    Double_t mass() {
        return fMass;
    }

    Double_t width() {
        return fWidth;
    }

    // Useful "natural" getters ..

    Double_t T() {
        return signalAndBackground();
    }

    Double_t B() {
        return background();
    }

    Double_t S() {
        Double_t tmp = T() - B();
        return tmp;
    }

    TString origin() {
        return fOrigin;
    }

    // Overloading ==  ...

    Bool_t operator== (BmnParticlePairCut& cut) {

        // Check assignement to itself ...
        if (this == &cut)
            return kTRUE;

        if (fDCA0 == cut.dca0() &&
                fDCA1 == cut.dca1() &&
                fDCA2 == cut.dca2() &&
                fDCA12 == cut.dca12() &&
                fPath == cut.path() &&
                fNhitsGemPos == cut.nHitsGemPos() &&
                fNhitsGemNeg == cut.nHitsGemNeg())
            return kTRUE;
        else
            return kFALSE;
    }

private:

    Double_t fDCA0;
    Double_t fDCA12;
    Double_t fDCA1;
    Double_t fDCA2;

    Double_t fPath;

    Int_t fNhitsGemPos;
    Int_t fNhitsGemNeg;

    Double_t fBackground;
    Double_t fSignalAndBackground;

    Double_t fMass;
    Double_t fWidth;

    TString fOrigin; // "source" if defined as wide, otherwise is "improved" 

    ClassDef(BmnParticlePairCut, 1)
};

#endif