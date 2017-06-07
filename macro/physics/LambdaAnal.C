#include <TString.h>

//void LambdaAnal(UInt_t nEvents = 1e4, TString input = "/nfs/lambda_QGSM/test/bmndst_fact13_100kEv.root", TString output = "test_dst1.root") {

void LambdaAnal(UInt_t nEvents = 1e4, TString input = "/nfs/lambda_QGSM/test/bmndst_fact13_10kEv_newTracking.root", TString output = "test_dst1.root") {
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
    // lambda->SetObtainCutsOnly(kTRUE);

    // Geometry cuts [from, to]-range is acceptable
    lambda->SetVpVpProton(1., 10.);
    lambda->SetVpVpPion(1., 10.);
    lambda->SetV0ProtonPion(0., 1.);
    lambda->SetV0VpDiff(10., 30.);
    
    // Kinematic cuts, if necessary. [from, to]-range is acceptable
    //    lambda->SetMomProtonRange(1.8 - 0.8, 1.8 + 0.8); 
    //    lambda->SetMomPionRange(0.3 - 0.2, 0.3 + 0.2);  
    //    lambda->SetMomProtonRange(0.1, 3.);  
    //    lambda->SetMomPionRange(0.1, 1.);   
    //    lambda->SetTxProtonRange(1.29e-02 - 1.53e-01, 1.29e-02 + 1.53e-01);
    //    lambda->SetTxPionRange(3.58e-03 - 2.42e-01, 3.58e-03 + 2.42e-01);
    //    lambda->SetTyProtonRange(-1.20e-2 - 1.63e-1, -1.20e-2 + 1.63e-1);
    //    lambda->SetTyPionRange(1.91e-02 - 2.53e-01, 1.91e-02 + 2.53e-01);
    //    lambda->SetYProtonRange(1.30 - 0.43, 1.30 + 0.43); // Cuts on rapidity
    //    lambda->SetYPionRange(1.33 - 0.56, 1.33 + 0.56);
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
