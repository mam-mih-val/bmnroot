
#include <stdlib.h>


void test_alignment(TString fileNumber = "61") {
 gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
 bmnloadlibs(); // load BmnRoot libraries

 BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("bmn_run00" + fileNumber + "_digi.root").Data(), TString("reco_" + fileNumber + ".root").Data());
 gemAlign->SetNofEvents(100);
 gemAlign->SetMaxNofHitsPerEvent(30);
 gemAlign->SetSignalToNoise(3.);
 // gemAlign->SetChi2Max(50);
 gemAlign->SetThreshold(50.);
 gemAlign->SetMinHitsAccepted(3);
  
 // gemAlign->SetYhitMinMax(-3., 3.);
 // gemAlign->SetXhitMinMax(0., 6.);

 gemAlign->SetAlignmentDim("xy");
 gemAlign->PrepareData();
 gemAlign->StartMille();
 
 
 
 delete gemAlign;
}