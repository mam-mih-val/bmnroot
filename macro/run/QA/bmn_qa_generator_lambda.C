// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void bmn_qa_generator_lambda(TString vPrefix = "", Bool_t useMCFile = kTRUE, Bool_t useRecoFile = kFALSE,
        TString recoFile = "",
        TString mcFile = "/home/merz/batyuk/nfs/evetest_ArPb_10k",
//        TString mcFile = "$VMCWORKDIR/macro/run/evetest_1lam_1k",
        TString lambdaFile = "",
        TString lambdaEveFile = "/home/merz/BMN/08_05_18/macro/physics/tmp",
//        TString lambdaEveFile = "lambdaEve_forQA",
        TString outFile = "qa.root", Int_t nStartEvent = 0, Bool_t isPrimary = kFALSE, Int_t nEvents = 1000) {
    // ----  Load libraries   -------------------------------------------------
    bmnloadlibs(); // load bmn libraries

    // ------------------------------------------------------------------------

    FairRunAna *fRun = new FairRunAna();

    fRun->SetInputFile(mcFile + vPrefix + ".root");
    fRun->AddFriend(lambdaEveFile + vPrefix + ".root");
//    fRun->AddFriend(lambdaFile + vPrefix + ".root");
//    fRun->AddFriend(recoFile + vPrefix + ".root");
    fRun->SetOutputFile(outFile);

    fRun->SetGenerateRunInfo(false);

    // ============ TASKS ============= //

//    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
//    fRun->AddTask(mcMatching);

    BmnLambdaQa* lambdaQa3 = new BmnLambdaQa(useMCFile, useRecoFile, 'b', "lambda_qa_gem" + vPrefix + "MC", vPrefix + "_____(MC)", kTRUE);
    fRun->AddTask(lambdaQa3);

//    BmnLambdaQa* lambdaQa4 = new BmnLambdaQa(kTRUE, kTRUE, 'a', "lambda_qa_gem" + vPrefix + "RecoMC", vPrefix + "_____(recoWithMatches)", kTRUE);
//    fRun->AddTask(lambdaQa4);
//
//    BmnLambdaQa* lambdaQa5 = new BmnLambdaQa(kFALSE, kTRUE, 'a', "lambda_qa_gem" + vPrefix + "Reco", vPrefix + "_____(reco)", kTRUE);
//    fRun->AddTask(lambdaQa5);

//    BmnClusteringQa* clust = new BmnClusteringQa();
//    fRun->AddTask(clust);

    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
