#include <TObject.h>

#ifndef BMNPARTICLEPAIRCUT_H
#define BMNPARTICLEPAIRCUT_H 1

class BmnParticlePairCut : public TObject {
public:

    BmnParticlePairCut();

    virtual ~BmnParticlePairCut() {
        ;
    }

    void SetCuts(Double_t dca0, Double_t dca12, Double_t dca1, Double_t dca2) {
        fDCA0 = dca0;
        fDCA12 = dca12;
        fDCA1 = dca1;
        fDCA2 = dca2;
    }

    void SetCuts(Int_t part, Int_t det, Int_t nHits) {
        fNhits[part][det] = nHits;
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
    
    Int_t nHits(Int_t part, Int_t det) {
        return fNhits[part][det];
    }

private:

    Double_t fDCA0;
    Double_t fDCA12;
    Double_t fDCA1;
    Double_t fDCA2;

    Int_t fNhits[2][2]; // part1, part2 --> silicon, gem

    ClassDef(BmnParticlePairCut, 1)
};

#endif