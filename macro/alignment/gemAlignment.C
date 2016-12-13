#include <TString.h>
#include <TStopwatch.h>

void gemAlignment(UInt_t nEvents = 1e6, TString recoFileName = "bmndst_noCorr_Stat_12345_origErrors.root") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    FairRunAna *fRun = new FairRunAna();
    FairSource* fFileSource;
    fFileSource = new BmnFileSource(recoFileName.Data());
    fRun->SetSource(fFileSource);
    fRun->SetOutputFile("align.root");

    BmnGemAlignment* gemAlign = new BmnGemAlignment();
    TString type = "beam";

    TString fixedStats[7] = {"fixed", "", "", "", "", "", "fixed"}; // St. 6 consists of two separate modules that could be either fixed or not simultaneously
    gemAlign->SetStatNumFixed(fixedStats);
    gemAlign->SetRunType(type);
    gemAlign->SetPreSigma(0.001); // Default value is 1
    gemAlign->SetAccuracy(1e-3);  // Default value is 1e-3
    gemAlign->SetUseRealHitErrors(kTRUE); // Default value is false
    gemAlign->SetUseRegularization(kTRUE); // Default value is false
    gemAlign->SetHugecut(20.); // Default value is 50
    gemAlign->SetChisqcut(5., 3.); //Default value is (0., 0.)
    gemAlign->SetEntriesPerParam(300); //Default value is 10
    gemAlign->SetOutlierdownweighting(4); // Default value is 0
    gemAlign->SetDwfractioncut(0.3); // Default value is 0, should be less than 0.5 
    gemAlign->SetFixDetector(true, true, true); // Default values are false(X), false(Y) and false(Z)     
    // gemAlign->SetDebugInfo(kTRUE);      // Default value is false
    // gemAlign->SetAlignmentTypeByHands("xy");

    // Restrictions on track params.
    // gemAlign->SetMinHitsAccepted(3); // Default value is 3
    // gemAlign->SetChi2MaxPerNDF(2.); // Cut on chi2/ndf for found tracks, default value is not limited
    // gemAlign->SetTxMinMax(-0.005, 0.005);
    // gemAlign->SetTyMinMax(-0.005, 0.005);
 
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
