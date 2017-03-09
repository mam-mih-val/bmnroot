// -----------------------------------------------------------------------------
// This macro determins misalignments of the BM@N GEM station modules in terms
// of X, Y, and Z coordinates in cm.
//
// Anatoly.Solomin@jinr.ru 2017-02-16
//
// Derived from original gemAlignment.C macro created by Pavel.Batyk@jinr.ru 2016-11

#include  <fstream>
#include  <string>
#include  <TString.h>
#include  <TStopwatch.h>
using namespace std;

void determine_align_corrections_gem(TString bmndstFileListFileName = "filelist_bmndst_it00.txt",
                                     TString newAlignCorrFileName   = "",
                                     UInt_t  nEvents                =  10000)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    if ( ! CheckFileExist(bmndstFileListFileName) ) return;
    if (newAlignCorrFileName == "") return;
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    FairRunAna* fRunAna = new FairRunAna();
    ifstream bmndstFiles(bmndstFileListFileName);
    cout <<"bmndstFileListFileName = "+bmndstFileListFileName<< endl;
    TString fname;
    string  fnamestr;
    // to create the FairSource* fFileSource object, here read the first file name only:
    bmndstFiles >> fnamestr;
    // because fstream produces string, while we need TString, we reassign:
    fname = fnamestr;
    cout <<"fname                  = "+fname<< endl;
    FairSource* fFileSource = new BmnFileSource(fname);
    // the rest of the files should be AddFile'ed to form a chain in the BmnFileSource object:
    while (bmndstFiles >> fnamestr) {
        fname = fnamestr;
        cout <<"fname                  = "+fname<< endl;
        fFileSource->BmnFileSource::AddFile(fname);
    }
    bmndstFiles.close();
    fRunAna->SetSource(fFileSource);
    cout <<"newAlignCorrFileName   = "+newAlignCorrFileName<< endl;
    fRunAna->SetOutputFile(newAlignCorrFileName);
    // resultName will be only essential part without ".root":
    TString resultName = newAlignCorrFileName;
    resultName.ReplaceAll(".root", "");
    cout <<"resultName             = "+resultName<< endl;
    BmnGemAlignment* gemAlign = new BmnGemAlignment();
    gemAlign->SetResultName(resultName);
    gemAlign->SetGeometry(BmnGemStripConfiguration::RunWinter2016);
  //TString type = "target";
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

    // Restrictions on track params.
  //gemAlign->SetMinHitsAccepted(5);               // Default value is 3
  //gemAlign->SetChi2MaxPerNDF(2.);                // Cut on chi2/ndf for found tracks, default value is not limited
  //gemAlign->SetTxMinMax(-1.,   0.);
  //gemAlign->SetTyMinMax(-0.05, 0.05);

    fRunAna->AddTask(gemAlign);
    fRunAna->Init();
    fRunAna->Run(0, nEvents);
  //delete fRunAna;
  //delete fFileSource;
  //delete gemAlign;
    // -----   Finish   --------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout <<"Macro finished successfully."<< endl;
    cout <<"Real time "<<rtime<<" s, CPU time "<<ctime<<" s"<< endl;
    cout << endl;
}
