#ifndef BMNLAMBDASTORE_H
#define BMNLAMBDASTORE_H 1

#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>

using namespace TMath;

class BmnLambdaStore : public TNamed {
public:

    BmnLambdaStore();
    BmnLambdaStore(Double_t, Double_t, Double_t);

    ~BmnLambdaStore();

    Double_t GetP() {
        return fP;
    }

    Double_t GetTx() {
        return fTx;
    }

    Double_t GetTy() {
        return fTy;
    }

    Double_t GetEta() {
        Double_t Pz = fP / TMath::Sqrt(1 + fTx * fTx + fTy * fTy);
        return 0.5 * TMath::Log((fP + Pz) / (fP - Pz));
    }
    
    Double_t GetPhi() {
        Double_t Pz = fP / TMath::Sqrt(1 + fTx * fTx + fTy * fTy);
        TVector3 tmp(fTx * Pz, fTy * Pz, Pz);
        return tmp.Phi() * TMath::RadToDeg() + 180.;
    }

private:
    Double_t fP;
    Double_t fTx;
    Double_t fTy;

    ClassDef(BmnLambdaStore, 1)
};

#endif


