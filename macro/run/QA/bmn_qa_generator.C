// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)

void bmn_qa_generator(
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_ArPb_geo2018.root",
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_ArPb_noLor.root",
        TString recoFile = "$VMCWORKDIR/macro/run8/bmndst.root",
        // TString recoFile = "$VMCWORKDIR/macro/run/bmndst_KrPb_noLor.root",
        // TString mcFile = "/home/merz/batyuk/home/batyuk/bmnroot_run7/macro/run/testParams/evetest_ArPb_geo2018.root",
        // TString mcFile = "/home/merz/batyuk/nfs/evetest_10k_ArPb_noLor.root",
        TString mcFile = "$VMCWORKDIR/macro/run8/bmnsim.root",
        // TString mcFile = "/home/merz/batyuk/nfs/evetest_10k_KrPb_noLor.root",
        TString outFile = "qa.root",
        Int_t nStartEvent = 0,
        Bool_t isPrimary = kFALSE,
        Int_t nEvents = 1000000) {

    FairRunAna *fRun = new FairRunAna();
    fRun->SetInputFile(recoFile);
    fRun->AddFriend(mcFile);
    fRun->SetOutputFile(outFile);
    fRun->SetGenerateRunInfo(false);

    Int_t period = 8;

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString confGem = gPathGemConfig + ((period == 8) ? "GemRun8.xml" : (period == 7) ? "GemRunSpring2018.xml" : (period == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml");
    TString confSil = gPathSilConfig + ((period == 8) ? "SiliconRun8_3stations.xml" : (period == 7) ? "SiliconRunSpring2018.xml" : (period == 6) ? "SiliconRunSpring2017.xml" : "SiliconRunSpring2017.xml");    

    // ============ TASKS ============= //

    if (period < 8) {
        BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
        fRun->AddTask(mcMatching);
    }

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
