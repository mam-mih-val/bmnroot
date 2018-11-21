// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void bmn_qa_generator_lambda(TString vPrefix = "", Bool_t useMCFile = kTRUE, Bool_t useRecoFile = kTRUE,
        TString recoFile = "",      // bmndst.root file with matches
        TString mcFile = "",        // evetest.root 
        TString lambdaFile = "",    // File got from LambdaAnal.root with DST-input
        TString lambdaEveFile = "", // File got from LambdaAnal.root with EVE-input
        TString outFile = "qa.root", Int_t nStartEvent = 0, Int_t nEvents = 1e6) {
    // ----  Load libraries   -------------------------------------------------
    bmnloadlibs(); // load bmn libraries

    // ------------------------------------------------------------------------

    FairRunAna *fRun = new FairRunAna();

    fRun->SetInputFile(mcFile);
    fRun->AddFriend(lambdaEveFile);
    fRun->AddFriend(lambdaFile);
    fRun->AddFriend(recoFile);
    fRun->SetOutputFile(outFile);

    fRun->SetGenerateRunInfo(false);

    // ============ TASKS ============= //

    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
    fRun->AddTask(mcMatching);

    BmnLambdaQa* lambdaQa = new BmnLambdaQa(useMCFile, useRecoFile, 'b', "lambda_qa_gem" + vPrefix + "MC", vPrefix + "_____(MC)", kTRUE);
    fRun->AddTask(lambdaQa);

    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
