// #include <Rtypes.h>
#include <TString.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

// Macro to be used for Lambda0->Pi- + p reconstruction
// inSim or inReco (not both simultaneously) can be empty depending on analysis you do!        
void LambdaAnal(UInt_t nEvents = 1e6,
        TString inSim = "",  // bmnsim.root
        TString inReco = "", // dst file with matches or obtained from exp. data processing
        TString output = "",
        Int_t runId) {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    if (inReco.Contains("root") && inSim.Contains("root")) {
        fRunAna->SetInputFile(inReco);
        fRunAna->AddFriend(inSim);
    }
    else
        fRunAna->SetInputFile(inReco.Contains("root") ? inReco : inSim);

    fRunAna->SetOutputFile(output);

    BmnTwoParticleDecay* lambda = new BmnTwoParticleDecay(BmnGemStripConfiguration::RunSpring2018, runId);
    lambda->SetGeometryFile("");

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
