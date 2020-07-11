#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

/* Arguments to be passed: 
 *   @sim is a file that contains different lambdas available for selection (e.g. evetest.root got after simulations)
 *   A set of available files (beam-target, LQQGSM) can be taken from the NICA cluster at /nica/mpd22/BMN_run7_simulations/    
 *   @out is an output filename with taken lambdas satisfied the kinematic. conditions (if defined) forbidden to contain "digi" as a part     
 *   
 */

void select(
        TString sim = "",
        TString out = "") {

    BmnLambdaEmbedding* emb = new BmnLambdaEmbedding("", sim, "", out);

    // Kinematical criteria to be used when selecting ...
    // See $VMCWORKDIR/embedding/BmnLambdaEmbedding.h
    emb->SetLambdaMinMomentum(1.5);
    emb->SetLambdaPhiRange(0., 180.);
    emb->SetLambdaEtaRange(1.5, 4.);

    emb->Embedding();

    delete emb;
}
