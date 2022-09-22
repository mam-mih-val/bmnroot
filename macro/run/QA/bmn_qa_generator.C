// --------------------------------------------------------------------------
// Macro for generating QA-reports
// --------------------------------------------------------------------------
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)

void bmn_qa_generator(
    TString recoFile = "$VMCWORKDIR/macro/run/bmndst_10k_L1.root",
    TString mcFile = "$VMCWORKDIR/macro/run/bmnsim_10k.root",
    TString outFile = "qa_0.root",
    Int_t nStartEvent = 0,
    Bool_t isPrimary = kFALSE,
    Int_t nEvents = 1000000) {

    FairRunAna* fRun = new FairRunAna();

    FairFileSource* fFileSource = new FairFileSource(recoFile);
    fFileSource->AddFriend(mcFile);
    fRun->SetSource(fFileSource);
    fRun->SetSink(new FairRootFileSink(outFile));

    fRun->SetGenerateRunInfo(false);

    Int_t period = 8;

    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");
    TString gPathGemConfig = gPathConfig + "/parameters/gem/XMLConfigs/";
    TString gPathSilConfig = gPathConfig + "/parameters/silicon/XMLConfigs/";
    TString gPathCscConfig = gPathConfig + "/parameters/csc/XMLConfigs/";
    TString confGem = gPathGemConfig + ((period == 8) ? "GemRun8.xml" : (period == 7) ? "GemRunSpring2018.xml" : (period == 6) ? "GemRunSpring2017.xml" : "GemRunSpring2017.xml");
    TString confSil = gPathSilConfig + ((period == 8) ? "SiliconRun8_4stations.xml" : (period == 7) ? "SiliconRunSpring2018.xml" : (period == 6) ? "SiliconRunSpring2017.xml" : "SiliconRunSpring2017.xml");
    TString confCSC = gPathCscConfig + ((period == 8) ? "CSCRun8.xml" : "");

    // ============ TASKS ============= //

    if (period < 8) {
        BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
        fRun->AddTask(mcMatching);
    }

    //  BmnClusteringQa* clQa = new BmnClusteringQa();
    //  clQa->SetOnlyPrimes(isPrimary);
    //  fRun->AddTask(clQa);

    BmnTrackingQa* trQaAll = new BmnTrackingQa(0, "tracking_qa", confGem, confSil, confCSC);
    trQaAll->SetDetectorPresence(kSILICON, kTRUE);
    trQaAll->SetDetectorPresence(kSSD, kFALSE);
    trQaAll->SetDetectorPresence(kGEM, kTRUE);
    trQaAll->SetOnlyPrimes(isPrimary);
    trQaAll->SetInnerTracksBranchName("StsTrack");
    //trQaAll->SetInnerTracksBranchName("StsVector");
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
