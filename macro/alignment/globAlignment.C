#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
        
#include <TString.h>
#include <TStopwatch.h>

void globAlignment(UInt_t nEvents = 1e6, TString recoFileName = "bmndst.root",
        TString addInfo = "") {
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
    // globAlign->SetDebug(kTRUE); // default is false

    // Restrictions on track params:
    // globAlign->SetMinHitsAccepted(5);               // Default value is 3
    // globAlign->SetChi2MaxPerNDF(2.);                // Cut on chi2/ndf for found tracks, default value is not limited
    // globAlign->SetTxMinMax(-1., +1.);
    // globAlign->SetExclusionRangeTx(-0.02, 0.02);
    // globAlign->SetTyMinMax(-1., +1.);
    // globAlign->SetExclusionRangeTy(-0.02, 0.02);   
    //globAlign->SetTxMinMax(-1.,   0.  );
    //globAlign->SetTyMinMax(-0.05, 0.05);

    //                      "GEM",  MWPC", "DCH",  "Vp",  "SILICON"
    globAlign->SetDetectors(kTRUE, kFALSE, kFALSE, kFALSE, kFALSE);

    // Define modules to be fixed (any character) within alignment, if necessary. 
    // st0, st1, st2, st3_modLeft(0), st3_modRight(1), st4_modLeft(0), st4_modRight(1), st5_modLeft(0), st5_modRight(1)
    // globAlign->SetGemFixedRun6(kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE, kFALSE);
    // Set of detectors in chain to be used in glob. alignment
    // A non-empty string means that the det. subsystem is used in the procedure
    // GEM tracker is included by default

    // Si-modules with numbers 0 and 4 did not work in RUN6
    // globAlign->SetSiFixedRun6(kTRUE, kFALSE, kFALSE, kFALSE, kTRUE, kFALSE, kFALSE, kFALSE);

    globAlign->SetPreSigma(0.001); // Default value is 1
    globAlign->SetAccuracy(0.001); // Default value is 0.001
    // globAlign->SetUseRealHitErrors(kTRUE); // Default value is false
    // globAlign->SetUseTrackWithMinChi2(kTRUE);       // Default value is false
    // globAlign->SetUseRegularization(kTRUE); // Default value is false
    // globAlign->SetHugecut(200.);                    // Default value is 50
    // globAlign->SetChisqcut(5., 2.);                 // Default value is (0., 0.)
    // globAlign->SetEntriesPerParam(100);             // Default value is 10
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
