#include <TString.h>
#include <vector>

using namespace std;

void alignment(TString fileNumber = "65") {
    TString type;
    if (fileNumber.Atoi() < 66 || fileNumber == "All")
        type = "beam";
    else
        type = "target";

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries

    Bool_t useMilleOnly = false; // true corresponds to the Mille usage only
    TString addInfo = "";
    BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("bmn_run00" + fileNumber + "_digi.root").Data(), TString("reco_" + fileNumber + addInfo + ".root").Data(), useMilleOnly);
    // gemAlign->SetDebugInfo(kTRUE); // Print debug info
    // gemAlign->SetWriteHitsOnly(kTRUE); // Write hits only, no alignment and track reconstruction are performed
    gemAlign->SetRunType(type);

    if (useMilleOnly == kFALSE)
        gemAlign->SetNofEvents(10000); //0 corresponds to all data set

    // Restrictions on output of the C.F.
    gemAlign->SetMaxNofHitsPerEvent(30);
    gemAlign->SetSignalToNoise(-2., -2., -2., -2., -2., -2., -2.); // 1000 is an artificial threshold not to use a station, to be removed in future
    gemAlign->SetThreshold(0.);

    // Restrictions on hit params.
    gemAlign->SetXMinMax(-3.5, -2.5); // --
    gemAlign->SetYMinMax(-0.5, 0.5);

    // Restrictions on track params.
    gemAlign->SetMinHitsAccepted(3); // >
    gemAlign->SetMaxHitsAccepted(8); // <
    gemAlign->SetTxMinMax(-0.002, 0.002); // --
    gemAlign->SetTyMinMax(-0.002, 0.002); // are meaningful in case of a beam-run only
    gemAlign->SetChi2MaxPerNDF(30.); // Cut on chi2/ndf for found tracks

    // Alignment types
//     gemAlign->SetAlignmentDim("xy");
    gemAlign->SetAlignmentDim("xyz");

    // steer_xy.txt and steer_xyz.txt are files to be changed in case of two types of alignment 
    const Int_t nSteerFiles = 1;
    TString steerFiles[nSteerFiles] = {""};
    if (gemAlign->GetAlignmentDim() == "xy")
        steerFiles[0] = "steer_xy.txt";
    else if (gemAlign->GetAlignmentDim() == "xyz")
        steerFiles[0] = "steer_xyz.txt";
    vector <TString> steerFileNames;
    for (Int_t iSize = 0; iSize < nSteerFiles; iSize++)
        steerFileNames.push_back(steerFiles[iSize]);
    gemAlign->SetSteerFile(steerFileNames);  
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
