#include <TString.h>
#include <vector>

using namespace std;
// BEAM:   61, 62, 63, 64 and 65 (artificially coded by 60: bmn_run0060_digi.root)
// TARGET: 66, 67 and 68         (artificially coded by 70: bmn_run0070_digi.root)

void alignment(Int_t fileNumber = 65, Int_t nEvents = 2000, Bool_t isDebug = kFALSE, Bool_t isHitsOnly = kFALSE, Bool_t isMilleOnly = kFALSE) {
    TString type = (fileNumber < 66) ? "beam" : "target";

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    TString addInfo = "";
    TString num = "";
    num += fileNumber;
    BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("bmn_run00" + num + "_digi.root").Data(), TString("reco_" + num + addInfo + ".root").Data(), isMilleOnly);
    gemAlign->SetDebugInfo(isDebug); // Print debug info
    gemAlign->SetWriteHitsOnly(isHitsOnly); // Write hits only, no alignment and track reconstruction are performed
    gemAlign->SetRunType(type);

    if (!isMilleOnly)
        gemAlign->SetNofEvents(nEvents); //0 corresponds to all data set

    // Restrictions on output of the C.F.
    gemAlign->SetMaxNofHitsPerEvent(30);
    gemAlign->SetThreshold(0.);

    // Restrictions on hit params, beam run only
    if (type == "beam") {
        gemAlign->SetXMinMax(-3.5, -2.5); // --
        gemAlign->SetYMinMax(-0.5, 0.5);
    }

    // Restrictions on track params.
    gemAlign->SetMinHitsAccepted(3); // >=
    if (type == "beam") {
        gemAlign->SetTxMinMax(-0.005, 0.005);
        gemAlign->SetTyMinMax(-0.005, 0.005);
    } else {
        gemAlign->SetTxMinMax(-0.28, 0.28);
        gemAlign->SetTyMinMax(-0.18, 0.18);
    }
    
    gemAlign->SetChi2MaxPerNDF(30.); // Cut on chi2/ndf for found tracks

    gemAlign->PrepareData();
    
    TString fixedStats[7] = {"fixed", "", "", "", "", "", "fixed"}; // Means that st0 and st6 are considered to be fixed 
    gemAlign->SetStatNumFixed(fixedStats);
    gemAlign->SetPreSigma(0.01);    // Default value is 1
    // gemAlign->SetAccuracy(1e-2); // Default value is 1e-3
    gemAlign->SetNumIterations(100); // Experimental option, default value is 1

    // Mille & Pede execution.
    if (gemAlign->GetWriteHitsOnly()) {
        delete gemAlign;
        return;
    }
    gemAlign->StartMille();
    gemAlign->StartPede();

    delete gemAlign;
}
