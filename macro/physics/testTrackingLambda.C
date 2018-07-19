#include <TString.h>
#include <TString.h>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

// QA-macro related to Lambda0->Pi- + p reconstruction
void testTrackingLambda(UInt_t nEvents, TString recoFile, TString mcFile, TString output) {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    fRunAna->SetInputFile(mcFile);
    fRunAna->AddFriend(recoFile);
    fRunAna->SetOutputFile(output.Data());
    fRunAna->SetGenerateRunInfo(kFALSE);

    BmnMatchRecoToMC* mcMatching = new BmnMatchRecoToMC();
    fRunAna->AddTask(mcMatching);

    BmnTwoParticleDecay* lambda = new BmnTwoParticleDecay(BmnGemStripConfiguration::RunSpring2017);
    lambda->SetUseRealVertex(kTRUE); // default value is false
    
    // Cuts are used in "continue"-loop!
    // Kinem. cuts (min < Ppos < max, Pneg, Ypos, Yneg)
    Double_t kinCuts[4][2] = {{1., 4.}, {0.2, 1.}, {-100, 100}, {-100, 100}}; 
    
    // Geom. cuts (min < dca1 < max, dca2, dca12, path)
    Double_t geomCuts[4][2] = {{0.1, 100.}, {0.1, 100.}, {0., 0.1}, {1., 200.}};
    lambda->SetCuts(kinCuts, geomCuts); 
    
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