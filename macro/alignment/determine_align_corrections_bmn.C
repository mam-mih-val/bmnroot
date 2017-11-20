// -----------------------------------------------------------------------------
// This macro determins misalignment corrections to the geometry description of
// BM@N GEM station modules, MWPC and DCH in terms of X, Y, and Z coordinates of
// the modules in cm.
//
// Anatoly.Solomin@jinr.ru 2017-04-05
//
// Derived from original globAlignment.C macro created by Pavel.Batyk@jinr.ru 2016-11

#include "../../gem/BmnGemStripConfiguration.h"
#include "../run/bmnloadlibs.C"

void determine_align_corrections_bmn(TString bmndstFileListFileName = "filelist_bmndst_it00.txt",
                                     TString newAlignCorrFileName   = "",
                                     UInt_t  nEvents                =  10000)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries
    if ( ! CheckFileExist(bmndstFileListFileName) ) return;
    if (newAlignCorrFileName == "") return;
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // get run period
  //TString substr = bmndstFileListFileName(TPRegexp("bmn_run[\d]+"));
  //substr         = substr(                TPRegexp(       "[\d]+"));
  //int runPeriod  = substr.Atoi();
    TString runPeriod = bmndstFileListFileName; // just to start with
    TPRegexp("bmn_run([\\d]+)[\\w\\d\\.]+").Substitute(runPeriod, "$1");
    int run_period  = runPeriod.Atoi();
    cout <<"run_period                     = "<<run_period<< endl;

    FairRunAna* fRunAna = new FairRunAna();
    ifstream bmndstFiles(bmndstFileListFileName);
    cout <<"bmndstFileListFileName         = "+bmndstFileListFileName<< endl;
    TString fname;
    string  fnamestr;
    // to create the FairSource* fFileSource object, read here only the first file name:
    bmndstFiles >> fnamestr;
    // because fstream produces string, while we need TString, we reassign:
    fname = fnamestr;
    // at least one input bmndst file name needs to be passed to
    // BmnGlobalAlignment to determine fRunId = evHeader->GetRunId(); from the
    // first event in that file, and therefore we memorize name of the first
    // file in the list to use it in construting the
    // BmnGlobalAlignment* globAlign object:
    TString firstBmndstFileName = fname;
    cout <<"fname                          = "+fname<< endl;
    FairSource* fFileSource = new BmnFileSource(fname);
    // the rest of the files should be AddFile'ed to form a chain in the BmnFileSource object:
    while (bmndstFiles >> fnamestr) {
        fname = fnamestr;
        cout <<"fname                          = "+fname<< endl;
        fFileSource->BmnFileSource::AddFile(fname);
    }
    bmndstFiles.close();
    fRunAna->SetSource(fFileSource);
    cout <<"newAlignCorrFileName           = "+newAlignCorrFileName<< endl;
    fRunAna->SetOutputFile(newAlignCorrFileName);

    // Define GEM config. to be used: RunWinter2016 or RunSpring2017
    BmnGemStripConfiguration::GEM_CONFIG gem_config;
    if      (run_period == 6)
        gem_config = BmnGemStripConfiguration::RunSpring2017;
    else if (run_period == 5)
        gem_config = BmnGemStripConfiguration::RunWinter2016;
    BmnGlobalAlignment* globAlign = new BmnGlobalAlignment(gem_config, firstBmndstFileName);
    globAlign->SetDebug(kTRUE);                     // Default is false

  // Restrictions on track params:
  //globAlign->SetMinHitsAccepted(5);               // Default value is 3
  //globAlign->SetChi2MaxPerNDF(2.);                // Cut on chi2/ndf for found tracks, default value is not limited
  //globAlign->SetTxMinMax(-1.,  1.);
  //globAlign->SetExclusionRangeTx(-0.02, 0.02);
  //globAlign->SetTyMinMax(-1.,  1.);
  //globAlign->SetExclusionRangeTy(-0.02, 0.02);

    // Define modules to be fixed (any character) within alignment, if necessary.
    // st0, st1, st2, st3_modLeft(0), st3_modRight(1), st4_modLeft(0), st4_modRight(1), st5_modLeft(0), st5_modRight(1)
    globAlign->SetGemFixedRun6("", "", "", "", "", "", "", "", "");
    // Set of detectors in chain to be used in glob. alignment
    // A non-empty string means that the det. subsystem is used in the procedure
    // GEM tracker is included by default
    //                    "MWPC",  "DCH"
    globAlign->SetDetectors("",     "");

    globAlign->SetPreSigma(0.001);                  // Default value is 1
    globAlign->SetAccuracy(0.001);                  // Default value is 0.001
    globAlign->SetUseRealHitErrors(kTRUE);          // Default value is false
  //globAlign->SetUseTrackWithMinChi2(kTRUE);       // Default value is false
    globAlign->SetUseRegularization(kTRUE);         // Default value is false
  //globAlign->SetHugecut(200.);                    // Default value is 50
  //globAlign->SetChisqcut(5., 2.);                 // Default value is (0., 0.)
  //globAlign->SetEntriesPerParam(100);             // Default value is 10
    globAlign->SetOutlierDownweighting(4);          // Default value is 0
    globAlign->SetDwfractionCut(0.5);               // Default value is 0, should be less than 0.5

    fRunAna->AddTask(globAlign);
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

  //delete fRunAna;
  //delete fFileSource;
    delete globAlign;
}
