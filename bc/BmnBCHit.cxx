#include "BmnBCHit.h"

BmnBCHit::BmnBCHit(){
}


BmnBCHit::BmnBCHit(Double_t ELoss1, Double_t ELoss2,Double_t ELoss3,Double_t ELoss4, Int_t Hits, vector <Int_t> chargesBC3, vector <Int_t> chargesBC4){
    NHits = Hits;
    fELoss1 = ELoss1;
    fELoss2 = ELoss2;
    fELoss3 = ELoss3;
    fELoss4 = ELoss4;
    chargesInBC3 = chargesBC3;
    chargesInBC4 = chargesBC4;
    const double a_out = 0.0263103;//0.349425;//-0.138267;
    const double b_out = 1130.13;//1098.37;//1183.78;
    const double c_out = 98.8885;//-591.855;//139.747;
    Double_t x = TMath::Sqrt(ELoss3*ELoss4);
    if (x!=0){
        ZOut = TMath::Sqrt(a_out + b_out * x + c_out * x * x);
    }else{
        ZOut = -1;
    }
}

BmnBCHit::~BmnBCHit() {
}

ClassImp(BmnBCHit)
