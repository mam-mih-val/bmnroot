#include <Rtypes.h>
#include <TString.h>
#include <TStopwatch.h>
#include <TFile.h>
#include <TKey.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

// DIGITS_GEM_SILICON - in1 (MYSILICON and STRIPGEM for SILICON and GEM)
// DIGITS_ZDC - in2 (ZDC digits only)
// DIGITS_OTHER_DETECTORS - in3 (trigger counters and other detectors)

void digiMerger(TString in1 = "",
        TString in2 = "",
        TString in3 = "",
        TString out = "") {
    bmnloadlibs(); // load BmnRoot libraries
    // -----   Timer   ---------------------------------------------------------
    TStopwatch timer;
    timer.Start();

    BmnDigiMergeTask* mergeTask = new BmnDigiMergeTask(in1, in2, in3, out);
    // mergeTask->SetNevsToBeProcessed(10000);
    // mergeTask->SetNevsInSample(50);
    mergeTask->ProcessEvents();
 
    delete mergeTask;
}