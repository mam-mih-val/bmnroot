#include <TString.h>
#include <vector>

using namespace std;
// BEAM:   61, 62, 63, 64 and 65 (artificially coded by 60: bmn_run0060_digi.root)
// TARGET: 66, 67 and 68         (artificially coded by 70: bmn_run0070_digi.root)

void alignment(Int_t fileNumber = 70, Int_t nEvents = 30000, Bool_t isDebug = kFALSE, Bool_t isOnlyHits = kFALSE, Bool_t isUseMilleOnly = kFALSE) {
    TString type = (fileNumber < 66) ? "beam" : "target";
 
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    TString addInfo = "";
    TString num = "";
    num += fileNumber;
    BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("bmn_run00" + num + "_digi.root").Data(), TString("reco_" + num + addInfo + ".root").Data(), isUseMilleOnly);
    gemAlign->SetDebugInfo(isDebug); // Print debug info
    gemAlign->SetWriteHitsOnly(isOnlyHits); // Write hits only, no alignment and track reconstruction are performed
    gemAlign->SetRunType(type);

    if (!isUseMilleOnly)
        gemAlign->SetNofEvents(nEvents); //0 corresponds to all data set

    // Restrictions on output of the C.F.
    gemAlign->SetMaxNofHitsPerEvent(30);
    gemAlign->SetSignalToNoise(-2., -2., -2., -2., -2., -2., -2.); // 1000 is an artificial threshold not to use a station, to be removed in future
    gemAlign->SetThreshold(0.);

    // Restrictions on hit params, xy-alignment only
    if (gemAlign->GetAlignmentDim() == "xy") {
        gemAlign->SetXMinMax(-3.5, -2.5); // --
        gemAlign->SetYMinMax(-0.5, 0.5);
    }

    // Restrictions on track params.
    gemAlign->SetMinHitsAccepted(3); // >=
    gemAlign->SetMaxHitsAccepted(7); // <=
    gemAlign->SetTxMinMax(-0.005, 0.005); // --
    gemAlign->SetTyMinMax(-0.005, 0.005); // are meaningful in case of a beam-run only
    gemAlign->SetChi2MaxPerNDF(30.); // Cut on chi2/ndf for found tracks

    // Probably, no user intervention to the strings below?
    if (type == "beam")
        gemAlign->SetAlignmentDim("xy");
    else
        gemAlign->SetAlignmentDim("xyz");

    // steer_xy.txt and steer_xyz.txt are files to be changed in case of two types of alignment 
    TString steerFile = "";
    if (gemAlign->GetAlignmentDim() == "xy")
        steerFile = "steer_xy.txt";
    else if (gemAlign->GetAlignmentDim() == "xyz")
        steerFile = "steer_xyz.txt";
    gemAlign->SetSteerFile(steerFile);
    gemAlign->PrepareData();

    // Mille & Pede execution.
    if (gemAlign->GetWriteHitsOnly()) {
        delete gemAlign;
        return;
    }
    gemAlign->StartMille();
    gemAlign->StartPede();

    delete gemAlign;
}
