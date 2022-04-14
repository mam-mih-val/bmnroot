#ifndef BMNBCHit_H
#define BMNBCHit_H
#include "BmnHit.h"

using namespace std;

class BmnBCHit : public TObject{
public:
    BmnBCHit();
    BmnBCHit(Double_t ELoss1, Double_t ELoss2, Double_t ELoss3, Double_t ELoss4, Int_t Hits, vector <Int_t> chargesBC3, vector <Int_t> chargesBC4); 
    //BmnBCHit(Int_t detID, TVector3 pos, TVector3 dpos, Int_t index);

    vector<Int_t > GetChargesFromBC3() {
        return chargesInBC3;
    }
    vector<Int_t > GetChargesFromBC4() {
        return chargesInBC4;
    }

    void SetCharge(Double_t ZOutTotal) {
        ZOut = ZOutTotal;
    }
    void SetNHits(Int_t Hits) {
        NHits = Hits;
    }

    Double_t GetZOut() {
        return ZOut;
    }
    Double_t GetEnergyLoss1() {
        return fELoss1;
    }
    Double_t GetEnergyLoss2() {
        return fELoss2;
    }
    Double_t GetEnergyLoss3() {
        return fELoss3;
    }
    Double_t GetEnergyLoss4() {
        return fELoss4;
    }

    Int_t GetNHits() {
        return NHits;
    }

    virtual ~BmnBCHit();

private:
    vector <Int_t> chargesInBC4;
    vector <Int_t> chargesInBC3;
    Double_t fELoss1;
    Double_t fELoss2;
    Double_t fELoss3;
    Double_t fELoss4;
    Double_t ZOut; 
    Int_t NHits;

    ClassDef(BmnBCHit, 1);
};

#endif /* BMNBCHit_H */

