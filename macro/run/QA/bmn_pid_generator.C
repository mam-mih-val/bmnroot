// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

void bmn_pid_generator(
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_ArPb_geo2018.root",
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_ArPb_noLor.root",
        TString recoFile = "$VMCWORKDIR/macro/run/bmndst.root",
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_KrPb_noLor.root",
        // TString mcFile = "/home/merz/batyuk/home/batyuk/bmnroot_run7/macro/run/testParams/evetest_ArPb_geo2018.root",
        // TString mcFile = "/home/merz/batyuk/nfs/evetest_10k_ArPb_noLor.root",
        TString mcFile = "$VMCWORKDIR/macro/run/bmnsim.root",
        // TString mcFile = "/home/merz/batyuk/nfs/evetest_10k_KrPb_noLor.root",
        TString outFile = "pid.root",
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

    BmnPidQa* pidQaAll = new BmnPidQa("pid_qa","pidStorage");
    pidQaAll->SetOnlyPrimes(isPrimary);
    fRun->AddTask(pidQaAll);

    // ============ TASKS ============= //

    // -----   Intialise and run   --------------------------------------------
    fRun->Init();
    cout << "Starting run" << endl;
    fRun->Run(nStartEvent, nStartEvent + nEvents);
    // ------------------------------------------------------------------------
}
