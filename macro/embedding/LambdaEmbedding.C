#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
void LambdaEmbedding(
        TString data = "/mnt/run/seans_55/3590-4707_BMN_Argon/mpd_run_trigCode_4649.data", 
        TString reco = "/nfs/RUN7_res/BMNDST_4649_GOOD.root", 
        TString sim = "/nica/mpd22/BMN_run7_simulations/sim/evetest_25kEv_ArAl_minBias_isLor_0.root", 
        TString out = "bmn_run4649_digi_withLambdaEmbedded.root") {
    
    BmnLambdaEmbedding* emb = new BmnLambdaEmbedding(data, sim, reco, out);
    
    // emb->DoLambdaStore(kFALSE);
    // emb->DoListOfEventsWithReconstructedVertex(kFALSE);
    emb->DoSimulateLambdaThroughSetup(kFALSE);
    
    emb->Embedding();
    
    delete emb;
}