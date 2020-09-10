#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
/* Arguments to be passed:
 * nEvents is a desirable number of exp. events to be processed when embedding ...
 * @lambdas is a file obtained with the $VMCWORKDIR/macro/embedding/selectLambdas.C macro
 * @givenEntryInStore means a store from @lambdas to be processed, -1 means all set of data to be used
 * @outPath is a directory to be used as a storage for MC-info with lambdas passed through the detector
 * @reco is a file with a revision of dst-files (after reco). It is used to get events with primary vertices 
 * satisfied some conditions and subsequently to be used for embedding
 * A set of dst-files can be got from the NICA cluster at /eos/nica/bmn/users/batyuk/bmnDst_InnTracker+tof400+csc+tof700+dch/linksToData/
 */        

void pass(
        Int_t nEvents, 
        TString lambdas = "", 
        Int_t givenEntryInStore = -1,
        TString outPath = "",
        TString reco = "") {

    BmnLambdaEmbedding* emb = new BmnLambdaEmbedding("", "", reco, "", nEvents, lambdas);
    emb->SetStoreToProcess(givenEntryInStore);
    emb->SetStorePath(outPath);

    emb->Embedding();

    delete emb;
}
