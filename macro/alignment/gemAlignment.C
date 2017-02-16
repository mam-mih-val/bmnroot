#include <TString.h>
#include <TStopwatch.h>

void gemAlignment(UInt_t  nEvents      = 1e6
                 ,TString recoFileName = "bmndst.root"
                 ,TString addInfo      = ""                 )
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
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
        outputFileName.ReplaceAll("bmndst", addInfo+"_align");
    else {
        outputFileName.ReplaceAll("bmndst",          "align");
    }
    cout <<"outputFileName: "+outputFileName<< endl;
    fRunAna->SetOutputFile(outputFileName);
    // resultName will be only essential part without ".root":
    TString resultName = outputFileName;
    resultName.ReplaceAll(".root", "");
    cout <<"resultName: "+resultName<< endl;
    BmnGemAlignment* gemAlign = new BmnGemAlignment();
    gemAlign->SetResultName(resultName);
    gemAlign->SetGeometry(BmnGemStripConfiguration::RunWinter2016);
    // GEM stations 4, 5 and 6 consist of two separate modules that can be either fixed or not simultaneously
    //                          0      1   2   3   4   5   6
    TString fixedStats[7] = {"fixed", "", "", "", "", "", ""};
    gemAlign->SetStatNumFixed(fixedStats);
  //gemAlign->SetRunType(type);
    gemAlign->SetPreSigma(0.001);                  // Default value is 1
    gemAlign->SetAccuracy(0.001);                  // Default value is 0.001
    gemAlign->SetUseRealHitErrors(kTRUE);          // Default value is false
  //gemAlign->SetUseTrackWithMinChi2(kTRUE);       // Default value is false
    gemAlign->SetUseRegularization(kTRUE);         // Default value is false
  //gemAlign->SetHugecut(200.);                    // Default value is 50
  //gemAlign->SetChisqcut(5., 2.);                 // Default value is (0., 0.)
  //gemAlign->SetEntriesPerParam(100);             // Default value is 10
    gemAlign->SetOutlierdownweighting(4);          // Default value is 0
    gemAlign->SetDwfractioncut(0.5);               // Default value is 0, should be less than 0.5
    gemAlign->SetFixDetector(kTRUE, kTRUE, kTRUE); // Default values are false(X), false(Y) and false(Z)
  //gemAlign->SetDebugInfo(kTRUE);                 // Default value is false

    // Restrictions on track params:
  //gemAlign->SetMinHitsAccepted(5);               // Default value is 3
  //gemAlign->SetChi2MaxPerNDF(2.);                // Cut on chi2/ndf for found tracks, default value is not limited
  //gemAlign->SetTxMinMax(-1.,   0.);
  //gemAlign->SetTyMinMax(-0.05, 0.05);

    fRunAna->AddTask(gemAlign);
    fRunAna->Init();
    fRunAna->Run(0, nEvents);
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout <<"Macro finished successfully."<< endl;
    cout <<"Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<< endl;
    cout << endl;
}
