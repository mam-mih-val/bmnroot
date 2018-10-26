#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
#include <TString.h>
#include <TStopwatch.h>

void globAlignment(UInt_t nEvents = 1e6, TString recoFileName = "bmndst_RUN7_GEM.root", TString addInfo = "", Int_t runPeriod = 7) {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    FairSource* fFileSource = new BmnFileSource(recoFileName.Data());
    fRunAna->SetSource(fFileSource);
    // Output file
    TString outputFileName = recoFileName;
    if (addInfo != "")
        outputFileName.ReplaceAll("bmndst", addInfo + "_align");
    else {
        outputFileName.ReplaceAll("bmndst", "align");
    }
    cout << "outputFileName: " + outputFileName << endl;
    fRunAna->SetOutputFile(outputFileName);

    BmnGlobalAlignment* globAlign = new BmnGlobalAlignment(recoFileName);
    globAlign->SetDoTest(kTRUE);
  
    //                      "GEM",  MWPC", "DCH",  "Vp",  "SILICON"
    globAlign->SetDetectors(kTRUE, kFALSE, kFALSE, kFALSE, kFALSE);

    globAlign->SetPreSigma(0.001); // Default value is 1
    globAlign->SetAccuracy(0.001); // Default value is 0.001
    globAlign->SetOutlierDownweighting(4); // Default value is 0
    globAlign->SetDwfractionCut(0.5); // Default value is 0, should be less than 0.5

    fRunAna->AddTask(globAlign);

    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    // -----------------------------   Finish   ----------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished successfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
    cout << endl;

    delete globAlign;
}
