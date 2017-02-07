#include <TString.h>
#include <TStopwatch.h>

void gemAlignment(UInt_t nEvents = 1e6, TString recoFileName = "bmndst.root", TString addInfo = "") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRun = new FairRunAna();
    FairSource* fFileSource;
    fFileSource = new BmnFileSource(recoFileName.Data());
    fRun->SetSource(fFileSource);
    // Output file
    TString outputFile = recoFileName;
    TRegexp re("bmndst");
    if (addInfo == "") {
        outputFile(re) = "align"; // replace bmndst with align
    } else {
        outputFile(re) = addInfo + "_align";
    }
    //fRun->SetOutputFile("align.root");
    fRun->SetOutputFile(outputFile);
    TString resultName = outputFile;
    re = "_align.root";
    resultName(re) = "";
    BmnGemAlignment* gemAlign = new BmnGemAlignment();
    gemAlign->SetResultName(resultName);
    gemAlign->SetGeometry(BmnGemStripConfiguration::RunWinter2016);
    TString fixedStats[7] = {"fixed", "", "", "", "", "", ""}; // St. 4, 5 and 6 consist of two separate modules that could be either fixed or not simultaneously
    gemAlign->SetStatNumFixed(fixedStats);
    gemAlign->SetPreSigma(1e-3); // Default value is 1
    gemAlign->SetAccuracy(1e-3); // Default value is 1e-3
    gemAlign->SetUseRealHitErrors(kTRUE); // Default value is false
    // gemAlign->SetUseTrackWithMinChi2(kTRUE);       // Default value is false
    gemAlign->SetUseRegularization(kTRUE); // Default value is false
    // gemAlign->SetHugecut(200.); // Default value is 50
    // gemAlign->SetChisqcut(5., 2.); // Default value is (0., 0.)
    // gemAlign->SetEntriesPerParam(100); // Default value is 10
    gemAlign->SetOutlierdownweighting(4); // Default value is 0
    gemAlign->SetDwfractioncut(0.5); // Default value is 0, should be less than 0.5
    gemAlign->SetFixDetector(kTRUE, kTRUE, kTRUE); // Default values are false(X), false(Y) and false(Z)
    //gemAlign->SetDebugInfo(kTRUE);                 // Default value is false

    // Restrictions on track params.
    // gemAlign->SetMinHitsAccepted(5);               // Default value is 3
    // gemAlign->SetChi2MaxPerNDF(2.);                // Cut on chi2/ndf for found tracks, default value is not limited
    // gemAlign->SetTxMinMax(-1., 0.);
    // gemAlign->SetTyMinMax(-0.05, 0.05);
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
