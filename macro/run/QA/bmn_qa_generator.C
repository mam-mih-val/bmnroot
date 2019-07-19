// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void bmn_qa_generator(
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_ArPb_geo2018.root",
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_ArPb_noLor.root",
        TString recoFile = "$VMCWORKDIR/macro/run/bmndst.root",
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_KrPb_noLor.root",
        // TString mcFile = "/home/merz/batyuk/home/batyuk/bmnroot_run7/macro/run/testParams/evetest_ArPb_geo2018.root",
        // TString mcFile = "/home/merz/batyuk/nfs/evetest_10k_ArPb_noLor.root",
        TString mcFile = "/home/merz/pavel/nica/mpd22/BMN_run7_simulations/sim/evetest_25kEv_ArPb_minBias_noLor_0.root",
        // TString mcFile = "/home/merz/batyuk/nfs/evetest_10k_KrPb_noLor.root",
        TString outFile = "qa.root",
        Int_t nStartEvent = 0,
        Bool_t isPrimary = kFALSE,
        Int_t nEvents = 1000000) {
    // ----  Load libraries   -------------------------------------------------

    bmnloadlibs(); // load bmn libraries

    // ------------------------------------------------------------------------

    FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(recoFile);
    fRun->AddFriend(mcFile);
    fRun->SetOutputFile(outFile);
    fRun->SetGenerateRunInfo(false);

    Int_t period = 7;

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString confGem = gPathGemConfig + ((period == 7) ? "GemRunSpring2018.xml" : (period == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml");
    TString confSil = gPathSilConfig + ((period == 7) ? "SiliconRunSpring2018.xml" : (period == 6) ? "SiliconRunSpring2017.xml" : "SiliconRunSpring2017.xml");

    // ============ TASKS ============= //

    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
    fRun->AddTask(mcMatching);

    //  BmnClusteringQa* clQa = new BmnClusteringQa();
    //  clQa->SetOnlyPrimes(isPrimary);
    //  fRun->AddTask(clQa);

    BmnTrackingQa* trQaAll = new BmnTrackingQa(0, "tracking_qa", confGem, confSil);
    trQaAll->SetDetectorPresence(kSILICON, kTRUE);
    trQaAll->SetDetectorPresence(kSSD, kFALSE);
    trQaAll->SetDetectorPresence(kGEM, kTRUE);
    trQaAll->SetOnlyPrimes(isPrimary);
    fRun->AddTask(trQaAll);
//
//    BmnTrackingQa* trQaPos = new BmnTrackingQa(+1, "tracking_qa_positive", confGem, confSil);
//    trQaPos->SetOnlyPrimes(isPrimary);
//    fRun->AddTask(trQaPos);
//
//    BmnTrackingQa* trQaNeg = new BmnTrackingQa(-1, "tracking_qa_negative", confGem, confSil);
//    trQaNeg->SetOnlyPrimes(isPrimary);
//    fRun->AddTask(trQaNeg);

    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
