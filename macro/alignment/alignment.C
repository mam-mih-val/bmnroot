#include <TString.h>
#include <vector>

using namespace std;

void alignment(TString fileNumber = "All") {
    TString type; 
    if (fileNumber.Atoi() < 66 || fileNumber == "All")
        type = "beam";
    else 
        type = "target";
        
 gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
 bmnloadlibs(); // load BmnRoot libraries

 Bool_t useMilleOnly = false; // true corresponds to the Mille usage only
 TString addInfo = "";
 BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("/nfs/digits_run4/bmn_run00" + fileNumber + "_digi.root").Data(), TString("reco_" + fileNumber + addInfo + ".root").Data(), useMilleOnly);
 // gemAlign->SetDebugInfo(kTRUE);
 gemAlign->SetRunType(type);
 if (useMilleOnly == kFALSE)
     gemAlign->SetNofEvents(0); // 0 corresponds to all data set
 
 // Restrictions on output of the C.F.
 gemAlign->SetMaxNofHitsPerEvent(30);
 gemAlign->SetSignalToNoise(1000., -2., -2., -2., -2., -2., 1000.); // 1000 is an artificial threshold not to use a station
 gemAlign->SetThreshold(0.);
  
 // Restrictions on track params.
 gemAlign->SetMinHitsAccepted(4); // >
 gemAlign->SetMaxHitsAccepted(6); // <
 gemAlign->SetTxMinMax(-0.05, 0.05); // --
 gemAlign->SetTyMinMax(-0.05, 0.05); // are meaningful in case of a beam-run only
  gemAlign->SetXMinMax(-6.0, 0.0);     // --
 // gemAlign->SetYMinMax(min, max);
 gemAlign->SetXresMax(0.1);
 gemAlign->SetYresMax(0.1);
 gemAlign->SetChi2Max(0.1);
 
 // Alignment params.
 gemAlign->SetAlignmentDim("xy");
 
 const Int_t nSteerFiles = 5;
 TString steerFiles[nSteerFiles] = {"steer1.txt", "steer2.txt", "steer3.txt", "steer4.txt", "steer5.txt"}; // Pass different steer-files
 vector <TString> steerFileNames;
 for (Int_t iSize = 0; iSize < nSteerFiles; iSize++)
     steerFileNames.push_back(steerFiles[iSize]);
 gemAlign->SetSteerFile(steerFileNames);
 gemAlign->PrepareData();
 
 // Mille & Pede execution.
 gemAlign->StartMille();
 gemAlign->StartPede(); 
  
 delete gemAlign;
}
