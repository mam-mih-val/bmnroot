#ifndef BMNPARTICLESTORE_H
#define BMNPARTICLESTORE_H 1

#include <TNamed.h>
#include <TMath.h>
#include <TVector3.h>

using namespace TMath;

class BmnParticleStore : public TNamed {
public:

    BmnParticleStore();
    BmnParticleStore(Int_t, Double_t, Double_t, Double_t);

    ~BmnParticleStore();

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
    
    Int_t GetPdg() {
        return fPdg;
    }

private:
    Double_t fP;
    Double_t fTx;
    Double_t fTy;
    
    Int_t fPdg;

    ClassDef(BmnParticleStore, 1)
};

#endif


