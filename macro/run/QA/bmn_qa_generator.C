// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void bmn_qa_generator(TString recoFile = "$VMCWORKDIR/macro/run/bmndst.root", TString mcFile = "$VMCWORKDIR/macro/run/evetest.root", TString outFile = "qa.root", Int_t nStartEvent = 0, Bool_t isPrimary = kFALSE, Int_t nEvents = 1000000) {
    // ----  Load libraries   -------------------------------------------------
    bmnloadlibs(); // load bmn libraries

    // ------------------------------------------------------------------------

    FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(recoFile);
    fRun->AddFriend(mcFile);
    fRun->SetOutputFile(outFile);
    fRun->SetGenerateRunInfo(false);

    // ============ TASKS ============= //

    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
    fRun->AddTask(mcMatching);
     
    //  BmnClusteringQa* clQa = new BmnClusteringQa();
    //  clQa->SetOnlyPrimes(isPrimary);
    //  fRun->AddTask(clQa);  

    BmnTrackingQa* trQaAll = new BmnTrackingQa(0, "tracking_qa");
    trQaAll->SetOnlyPrimes(isPrimary);
    fRun->AddTask(trQaAll);

    BmnTrackingQa* trQaPos = new BmnTrackingQa(+1, "tracking_qa_positive");
    trQaPos->SetOnlyPrimes(isPrimary);
    fRun->AddTask(trQaPos);

    BmnTrackingQa* trQaNeg = new BmnTrackingQa(-1, "tracking_qa_negative");
    trQaNeg->SetOnlyPrimes(isPrimary);
    fRun->AddTask(trQaNeg);

    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
