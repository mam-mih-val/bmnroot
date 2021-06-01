#include <Rtypes.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

using namespace std;
using namespace TMath;

// Used for stage 1 (producing inputs to calculate trigger efficiency)
// Short description of arguments: 
// *input* is a directory filled with data produced by two-particle decay anal.
// *output* is a desirable output filename (file with nom. and denom. histograms)
// *trigger* is a trigger to process (five trigger conditions right now are supported: BT+BD2, BT+BD3, BT+FD2, BT+FD3, BT+BD1+FD2)
// Output is a file having name convention as folows "out_TRIGGER_start(STARTRUN)_finish(FINISHRUN).root"
// STARTRUN and FINISHRUN are set by default with 3589 and 4710 if not redefined by user

void trigEffRun7(TString input, TString trigger) {

    BmnTriggerEfficiencyRun7* eff = new BmnTriggerEfficiencyRun7(input);
    eff->SetTrigger(trigger);

    // Resetting multiplicity bins ...
    vector <pair <Int_t, Int_t>> bins;

    bins.push_back(make_pair(2, 4));
    bins.push_back(make_pair(4, 6));
    bins.push_back(make_pair(6, 8));
    bins.push_back(make_pair(8, 20));
    eff->SetMultiplicityMap(bins);

    vector <TString> targets{"Pb", "Sn", "Cu", "Al"};

    // Common cuts for all targets (to get #Lambda^{0} mass spectrum) ...
    for (TString target : targets) {
        eff->SetCuts(target, 1.2, 0.3, 2.2, 0.7, 12.);
        eff->SetTarget(target);
    }

    eff->triggerEfficiency();

    delete eff;
}

// Used for stage 2 (calculating trigger efficiency, namely)
// Short description of arguments: 
// *input1* is a directory filled with data produced by two-particle decay anal.
// *input2* is a file produced at stage1 (file with nom. and denom. histograms)
// *output* (should contain a formalized trigger condition as part, e.g. out_BT+FD2.root or something else)
// *trigger* is a trigger to process (five trigger conditions right now are supported: BT+BD2, BT+BD3, BT+FD2, BT+FD3, BT+BD1+FD2)

void trigEffRun7(TString input1,
        TString input2,
        TString output,
        TString trigger) {

    BmnTriggerEfficiencyRun7* eff = new BmnTriggerEfficiencyRun7(input1);
    eff->SetTrigger(trigger);
    eff->SetSpectraFile(input2);

    // Setting eff. files (for triggers containing either BD and FD conditions) ...
    eff->SetEffFiles(output);

    // Resetting multiplicity bins ...
    vector <pair <Int_t, Int_t>> bins;

    bins.push_back(make_pair(2, 4));
    bins.push_back(make_pair(4, 6));
    bins.push_back(make_pair(6, 8));
    bins.push_back(make_pair(8, 20));
    eff->SetMultiplicityMap(bins);

    vector <TString> targets{"Pb", "Sn", "Cu", "Al"};

    // Common cuts for all targets (to get #Lambda^{0} mass spectrum) ...
    for (TString target : targets) {
        eff->SetCuts(target, 1.2, 0.3, 2.2, 0.7, 12.);
        eff->SetTarget(target);
    }

    eff->triggerEfficiency();

    delete eff;
}