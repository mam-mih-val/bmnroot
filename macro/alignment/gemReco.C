#include <TString.h>
#include <vector>

using namespace std;
// BEAM:   61, 62, 63, 64 and 65
// TARGET: 66, 67 and 68

//void gemReco(Int_t fileNumber = 65, Int_t nEvents = 10000) {
void gemReco(Int_t fileNumber = 65, Int_t nEvents = 10000, TString addInfo = "") {
    TString type = (fileNumber < 66) ? "beam" : "target";

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    FairRunAna *fRun = new FairRunAna();
    FairSource* fFileSource;
    TString num = "";
  //TString addInfo = "";
    num += fileNumber;
  //fFileSource = new BmnFileSource(TString("bmn_run0" + num + "_digi.root").Data());
    fFileSource = new BmnFileSource(TString("bmn_run00"+num+"_digi.root").Data());
    fRun->SetSource(fFileSource);
  //fRun->SetOutputFile(TString("reco_" + num + addInfo + ".root").Data());
    fRun->SetOutputFile(TString("bmn_run00"+num+addInfo+"_reco.root").Data());
    BmnGemReco* gemReco = new BmnGemReco();

    /****DEFINE RECO CONFIG*****/
    // gemReco->SetDebugInfo(kTRUE); // Print debug info
    gemReco->SetMaxNofHitsPerEvent(30);

    // Restrictions on hit params, beam run only
    if (type == "beam") {
        gemReco->SetXMinMax(-3.5, -2.5); // --
        gemReco->SetYMinMax(-0.5, 0.5);
    }

    // Restrictions on track params.
    gemReco->SetMinHitsAccepted(3); // >=
    if (type == "beam") {
        gemReco->SetTxMinMax(-0.005, 0.005);
        gemReco->SetTyMinMax(-0.005, 0.005);
    } else {
        gemReco->SetTxMinMax(-0.28, 0.28);
        gemReco->SetTyMinMax(-0.18, 0.18);
    }

    gemReco->SetChi2MaxPerNDF(30.); // Cut on chi2/ndf for found tracks
    /********************************/

    fRun->AddTask(gemReco);

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
