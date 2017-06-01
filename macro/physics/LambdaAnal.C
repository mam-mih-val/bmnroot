#include <TString.h>

void LambdaAnal(UInt_t nEvents = 1e5, TString input = "INFILE", TString output = "OUTFILE") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    FairSource* fFileSource = new BmnFileSource(input.Data());
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(output);

    BmnLambdaInvMass* lambda = new BmnLambdaInvMass(BmnGemStripConfiguration::RunSpring2017);
    // lambda->SetDebugCalculations(kTRUE);
    // lambda->SetObtainCutsOnly(kTRUE);
    
    //    Different kinematic cuts on selected pairs can be used if necessary
    //    lambda->SetMomProtonRange(1.8 - 0.8, 1.8 + 0.8); // val1 < mProt < val2 are accepted 
    //    lambda->SetMomPionRange(0.3 - 0.2, 0.3 + 0.2);   // val1 < mPion < val2 are accepted 
    //    lambda->SetTxProtonRange(1.29e-02 - 1.53e-01, 1.29e-02 + 1.53e-01);
    //    lambda->SetTxPionRange(3.58e-03 - 2.42e-01, 3.58e-03 + 2.42e-01);
    //    lambda->SetTyProtonRange(-1.20e-2 - 1.63e-1, -1.20e-2 + 1.63e-1);
    //    lambda->SetTyPionRange(1.91e-02 - 2.53e-01, 1.91e-02 + 2.53e-01);
    //    lambda->SetYProtonRange(1.30 - 0.43, 1.30 + 0.43); // Cuts on rapidity
    //    lambda->SetYPionRange(1.33 - 0.56, 1.33 + 0.56);
    //    lambda->SetInputAccuracy(0., 0., 0., 0., 0., 0.); // dp1, dp2, dTx1, dTx2, dTy1, dTy2 in % 
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
