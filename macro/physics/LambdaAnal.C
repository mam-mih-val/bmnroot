#include <TString.h>
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

// Macro to be used for Lambda0->Pi- + p reconstruction
void LambdaAnal(UInt_t nEvents, Int_t runNumb, TString input, TString output) {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    FairSource* fFileSource = new BmnFileSource(input.Data());
    fRunAna->SetSource(fFileSource);
    fRunAna->SetOutputFile(output);

    BmnTwoParticleDecay* lambda = new BmnTwoParticleDecay(BmnGemStripConfiguration::RunSpring2017, runNumb);
    lambda->SetUseRealVertex(kTRUE); // equal to false by default
    //lambda->SetSiRequired(kTRUE); // equal to false by default

    // Geometry cuts if necessary, [from, to]-range is acceptable
    // lambda->SetVpVpParticle1(0., 100.);
    // lambda->SetVpVpParticle2(0., 100.);
    // lambda->SetV0Particle1Particle2(0., 1.);
    // lambda->SetV0VpDiff(0., 30.);

    // Kinematic cuts, if necessary, [from, to]-range is acceptable
    // lambda->SetMomParticle1Range(0., 5.);
    // lambda->SetMomParticle2Range(0., 5.);
    // lambda->SetYParticle1Range(0., 3.); // Cuts on rapidity
    // lambda->SetYParticle2Range(0., 3.);
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
