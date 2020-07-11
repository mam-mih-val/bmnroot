#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/* Arguments to be passed: 
 * @storePath is a directory filled with data produced by $VMCWORKDIR/macro/embedding/passLambdasToSimulations.C
 * @data is a raw data file (see BM@N wiki how to get)
 * @reco is a file with a revision of dst-files (after reco).
 * @out is a resulting file with embedded info. The name must contain "digi" as a part
 */

void embed(Int_t nEvents,
        TString storePath = "",
        TString data = "",
        TString reco = "",
        TString out = "") {

    BmnLambdaEmbedding* emb = new BmnLambdaEmbedding(data, "", reco, out, nEvents);
    emb->SetStorePath(storePath);

    emb->Embedding();

    delete emb;
}
