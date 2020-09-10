#include "BmnLambdaStore.h"

BmnLambdaStore::BmnLambdaStore() :
fP(-1000.), 
fTx(-1000.), 
fTy(-1000.) {  
    
}

BmnLambdaStore::BmnLambdaStore(Double_t p, Double_t tx, Double_t ty) {
    fP = p;
    fTx = tx;
    fTy = ty; 
    
   
}

BmnLambdaStore::~BmnLambdaStore() {
  
}


