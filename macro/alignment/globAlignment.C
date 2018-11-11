#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
#include <TString.h>
#include <TStopwatch.h>
        
R__LOAD_LIBRARY(libSilicon.so)        

void globAlignment(UInt_t nEvents = 1e3, TString recoFileName = "bmndst.root", TString addInfo = "", Int_t runPeriod = 7) {
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

    BmnGlobalAlignment* globAlign = new BmnGlobalAlignment(nEvents, recoFileName, runPeriod);
    // BmnGlobalAlignment* globAlign = new BmnGlobalAlignment(recoFileName);
    // globAlign->SetDebug(kTRUE); // default is false
    // globAlign->SetUseConstraints(false);

    // Restrictions on track params:
    // globAlign->SetMinHitsAccepted(7);            // Default value is 3
    // globAlign->SetChi2MaxPerNDF(2.);             // Cut on chi2/ndf for found tracks, default value is not limited
    // globAlign->SetTxMinMax(-.5, +.5);
    // globAlign->SetExclusionRangeTx(-.1, +.1);
    // globAlign->SetTyMinMax(-1., +1.);
    // globAlign->SetExclusionRangeTy(-0.02, 0.02);   
    // globAlign->SetTxMinMax(-1.,   0.  );
    // globAlign->SetTyMinMax(-0.05, 0.05);

    // "GEM", "SILICON"
    globAlign->SetDetectors(kTRUE, kTRUE);

//    globAlign->SetSiFixedRun7(false, false, false, false,
//            false, false,
//            kTRUE, kTRUE, kTRUE, kTRUE, kTRUE, kTRUE, kTRUE, kTRUE);

    globAlign->SetPreSigma(0.001); // Default value is 1
    globAlign->SetAccuracy(0.001); // Default value is 0.001
    // globAlign->SetUseRealHitErrors(kTRUE); // Default value is false
    // globAlign->SetUseRegularization(kTRUE); // Default value is false
    // globAlign->SetHugecut(200.);                    // Default value is 50
    // globAlign->SetChisqcut(5., 2.);                 // Default value is (0., 0.)
    // globAlign->SetEntriesPerParam(5);             // Default value is 10
    globAlign->SetOutlierDownweighting(4); // Default value is 0
    globAlign->SetDwfractionCut(0.5); // Default value is 0, should be less than 0.5

    fRunAna->AddTask(globAlign);

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

    delete globAlign;
}
