void test_alignment(TString fileNumber = "All") {
 gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
 bmnloadlibs(); // load BmnRoot libraries

 Bool_t useMilleOnly = false; // true corresponds to the Mille usage only
 BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("/nfs/digits_run4/bmn_run00" + fileNumber + "_digi.root").Data(), TString("reco_" + fileNumber + ".root").Data(), useMilleOnly);
 gemAlign->SetDebugInfo(kTRUE);
 gemAlign->SetNofEvents(100000); // 0 corresponds to all data set
 
 // Restrictions on output of the C.F.
 gemAlign->SetMaxNofHitsPerEvent(30);
 gemAlign->SetSignalToNoise(1000., -2., -3., -2., -3., -2., 1000.); // 1000 is an artificial threshold not to use a station
 gemAlign->SetThreshold(0.);
  
 // Restrictions on track params.
 gemAlign->SetMinHitsAccepted(4); // >
 gemAlign->SetMaxHitsAccepted(6); // <
 gemAlign->SetTxMinMax(-0.05, 0.05);
 gemAlign->SetTyMinMax(-0.05, 0.05);
 gemAlign->SetXMinMax(0.0, 6.0);
 // gemAlign->SetYMinMax(min, max);
 gemAlign->SetChi2Max(0.1);
 
 // Alignment params.
 gemAlign->SetAlignmentDim("xy");
 gemAlign->SetSteerFile("steer.txt");
 gemAlign->PrepareData();
 
 // Mille & Pede execution.
 gemAlign->StartMille();
 gemAlign->StartPede(true); // if true, the Pede is started
  
 delete gemAlign;
}
