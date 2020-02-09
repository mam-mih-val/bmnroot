#include "BmnLambdaEmbeddingMonitor.h"

BmnLambdaEmbeddingMonitor::BmnLambdaEmbeddingMonitor() : 
id (-1), 
isEmbedded(kFALSE),
fVx(-1000.),
fVy(-1000.),
fVz(-1000.),
store(-1),
vertex(-1),
event(-1), 
nHitsProton(-1),
nHitsPion(-1) {

    
    
    
}

BmnLambdaEmbeddingMonitor::~BmnLambdaEmbeddingMonitor() {

    
    
    
}

ClassImp(BmnLambdaEmbeddingMonitor)