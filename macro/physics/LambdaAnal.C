#include <TString.h>
#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

// Macro to be used for Lambda0->Pi- + p reconstruction
// inSim or inReco (not both simultaneously) can be empty depending on analysis you do!        
void LambdaAnal(UInt_t nEvents = 10e6,
        TString inSim = "",  // evetest.root
        TString inReco = "", // bmndst.root (with matches or obtained from exp. data processing)
        TString output = "out.root") {
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

    BmnTwoParticleDecay* lambda = new BmnTwoParticleDecay(BmnGemStripConfiguration::RunSpring2018);
    lambda->SetUseRealVertex(kTRUE); // equal to false by default

    // Geometry cuts if necessary
    lambda->SetDCA1(0., 1000.);
    lambda->SetDCA2(0., 1000.);
    lambda->SetDCA12(0., 10.);
    lambda->SetPath(0., 300.);

    // Kinematic cuts, if necessary
    lambda->SetMom1(0., 10.);
    lambda->SetMom2(0., 10.);
    lambda->SetEta1(0., 10.);
    lambda->SetEta2(0., 10.);
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
