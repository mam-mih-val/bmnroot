#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/* A macro to be for a little bit outstanding procedure from embedding.
 * It allows one to prepare stores containing appropriate information with lambda decay products 
 * being reconstructed in the BM@N inner tracker in sense of geometry acceptance
 * Additional information on a possible set of options (cuts) can be found in 
 * $VMCWORKDIR/embedding/BmnLambdaEmbedding.h
  Three passed argumens needed:
 * @@@ path @@@ - a path for the store to be written to 
 * @@@ reco @@@ - a valid dst file to extract information on reconstructed vertex position in event
 * @@@ sim  @@@ - an appropriate file with a BM@N Monte Carlo detector simulation for established target and projectile 
 * to be used when extracting primary lambdas with their kinematic parameters (pseudorapidity, momentum, azimuth angle and so on )
 * to be passed subsequently to the BOX-generator in order to produce a lambda store to be used for embedding.          
 */

void passLambdaStoreToSimulations(
        TString path = "",
        //Double_t etaMin = -1,
        //Double_t etaMax = -1,
        TString data = "",
        TString reco = "",
        TString sim = "",
        TString out = "") {

    BmnLambdaEmbedding* emb = new BmnLambdaEmbedding(data, sim, reco, out, 199000);
    emb->SetStorePath(path);
    // emb->SetLambdaMinMomentum(1.5);
    // emb->SetLambdaEtaRange(etaMin, etaMax);
    emb->DoSimulateLambdaThroughSetup(kTRUE);

    emb->DoRawRootConvertion(kFALSE);
    emb->DoEmbeddingMonitor(kFALSE);
    emb->DoEmbedding(kFALSE);
    emb->DoDecode(kFALSE);

    emb->Embedding();

    delete emb;
}
