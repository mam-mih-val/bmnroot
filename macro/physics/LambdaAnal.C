#include <TString.h>

//void LambdaAnal(UInt_t nEvents = 1e4, TString input = "/nfs/lambda_QGSM/test/bmndst_fact13_100kEv.root", TString output = "test_dst1.root") {

void LambdaAnal(UInt_t nEvents = 1e6, TString input = "/nfs/QGSM_SIMULATIONS/evetest_start999000_nev1000.root", TString output = "testLambda2NEW.root") {
    //void LambdaAnal(UInt_t nEvents = 3e4, TString input = "/nfs/lambda_BOX/bmndst_lambda_BOX.root", TString output = "test_dst1.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    FairSource* fFileSource = new BmnFileSource(input.Data());
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(output);

    BmnLambdaAnalysis* lambda = new BmnLambdaAnalysis(BmnGemStripConfiguration::RunSpring2017);
    // lambda->SetDebugCalculations(kTRUE);

    // Geometry cuts [from, to]-range is acceptable
    lambda->SetVpVpProton(0.1, 100.0);
    lambda->SetVpVpPion(0.1, 100.0);
    lambda->SetV0ProtonPion(0.0, 1.0);
    lambda->SetV0VpDiff(1.0, 30.0);

    // Kinematic cuts, if necessary. [from, to]-range is acceptable
    lambda->SetMomProtonRange(0.1, 5.);
    lambda->SetMomPionRange(0.1, 5.);
    //    lambda->SetTxProtonRange();
    //    lambda->SetTxPionRange();
    //    lambda->SetTyProtonRange();
    //    lambda->SetTyPionRange();
    lambda->SetYProtonRange(0.0, 3.0); // Cuts on rapidity
    lambda->SetYPionRange(0.0, 3.0);
    fRunAna->AddTask(lambda);

    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;

    delete lambda;
}
