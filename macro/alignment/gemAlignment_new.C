#include  <fstream>
#include  <TString.h>
#include  <TStopwatch.h>

void gemAlignment_new(UInt_t nEvents=1e6, TString bmndstFileListFileName="bmndst_files.txt", TString newAlignCorrFileName="")
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    if ( ! CheckFileExist(bmndstFileListFileName) ) return;
    if (newAlignCorrFileName == "") return;
    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRun = new FairRunAna();
    FairSource* fFileSource;
    ifstream bmndstFiles(bmndstFileListFileName);
    TString fname;
    // the first file only:
    bmndstFiles >> fname;
    fFileSource = new BmnFileSource(fname);
    // the rest of the files should be AddFile'ed to form a chain in the BmnFileSource object:
    while (bmndstFiles >> fname) {
        fFileSource.AddFile(fname);
    }
    bmndstFiles.close();
    fRun->SetSource(fFileSource);
    fRun->SetOutputFile(newAlignCorrFileName);
    // resultName will be only essential part    without ".root":
    TString resultName = newAlignCorrFileName.ReplaceAll(".root", "");
    BmnGemAlignment* gemAlign = new BmnGemAlignment();
    gemAlign->SetResultName(resultName);
    gemAlign->SetGeometry(BmnGemStripConfiguration::RunWinter2016);
    TString type = "target";
    TString fixedStats[7] = {"fixed", "", "", "", "", "", ""}; // St. 4, 5 and 6 consist of two separate modules that could be either fixed or not simultaneously
    gemAlign->SetStatNumFixed(fixedStats);
    gemAlign->SetRunType(type);
    gemAlign->SetPreSigma(0.001);                  // Default value is 1
    gemAlign->SetAccuracy(1e-3);                   // Default value is 1e-3
    gemAlign->SetUseRealHitErrors(kTRUE);          // Default value is false
  //gemAlign->SetUseTrackWithMinChi2(kTRUE);       // Default value is false
    gemAlign->SetUseRegularization(kTRUE);         // Default value is false
    gemAlign->SetHugecut(20.);                     // Default value is 50
  //gemAlign->SetChisqcut(5., 2.);                 // Default value is (0., 0.)
  //gemAlign->SetEntriesPerParam(3);               // Default value is 10
    gemAlign->SetOutlierdownweighting(4);          // Default value is 0
    gemAlign->SetDwfractioncut(0.3);               // Default value is 0, should be less than 0.5
    gemAlign->SetFixDetector(kTRUE, kTRUE, kTRUE); // Default values are false(X), false(Y) and false(Z)
  //gemAlign->SetDebugInfo(kTRUE);                 // Default value is false
  //gemAlign->SetAlignmentTypeByHands("xyz");
  //Restrictions on track params.
  //gemAlign->SetMinHitsAccepted(3);               // Default value is 3
  //gemAlign->SetChi2MaxPerNDF(2.);                // Cut on chi2/ndf for found tracks, default value is not limited
  //gemAlign->SetTxMinMax(-0.005, 0.005);
  //gemAlign->SetTyMinMax(-0.005, 0.005);
    /********************************/
    fRun->AddTask(gemAlign);
    fRun->Init();
    fRun->Run(0, nEvents);
    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout <<"Macro finished successfully."<< endl;
    cout <<"Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<< endl;
    cout << endl;
}
