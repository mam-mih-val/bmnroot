#include <TString.h>

void gemAlignment(Int_t fileNumber = 65, Int_t nEvents = 10000) {

    TString type = (fileNumber < 66) ? "beam" : "target";

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    FairRunAna *fRun = new FairRunAna();
    FairSource* fFileSource;
    TString num = "";
    // TString addInfo = "";
    num += fileNumber;
    fFileSource = new BmnFileSource(TString("reco_" + num + ".root").Data());

    fRun->SetSource(fFileSource);
    fRun->SetOutputFile("align.root");

    BmnGemAlignment* gemAlign = new BmnGemAlignment();
     /****DEFINE RECO CONFIG*****/
    TString fixedStats[7] = {"fixed", "", "", "", "", "", "fixed"}; // Means that st0 and st6 are considered to be fixed 
    gemAlign->SetStatNumFixed(fixedStats);
    gemAlign->SetRunType(type);
    gemAlign->SetPreSigma(0.01); // Default value is 1
    gemAlign->SetAccuracy(1e-2); // Default value is 1e-3
    gemAlign->SetNumIterations(10);
    // gemAlign->SetDebugInfo(kTRUE);
    /********************************/

    fRun->AddTask(gemAlign);

    fRun->Init();
    fRun->Run(0, nEvents);


    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;
}