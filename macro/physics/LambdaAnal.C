#include <TString.h>

void LambdaAnal(UInt_t nEvents = 10000, TString input = "evetest.root", TString output = "lambda.root") {
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
