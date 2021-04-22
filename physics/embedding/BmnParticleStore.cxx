#include "BmnParticleStore.h"

BmnParticleStore::BmnParticleStore() :
fP(-1000.), 
fTx(-1000.), 
fTy(-1000.),
fPdg(0) {  
    
}

BmnParticleStore::BmnParticleStore(Int_t pdg, Double_t p, Double_t tx, Double_t ty) {
    fP = p;
    fTx = tx;
    fTy = ty; 
    fPdg = pdg;
     
}

BmnParticleStore::~BmnParticleStore() {
  
}


