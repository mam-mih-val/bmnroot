void test_alignment(TString fileNumber = "All") {
 gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
 bmnloadlibs(); // load BmnRoot libraries

 Bool_t useMilleOnly = false;
 BmnGemAlignment* gemAlign = new BmnGemAlignment(TString("/nfs/digits_run4/bmn_run00" + fileNumber + "_digi.root").Data(), TString("reco_" + fileNumber + ".root").Data(), useMilleOnly);
  // gemAlign->SetDebugInfo(kTRUE);
 gemAlign->SetNofEvents(10000);
 gemAlign->SetMaxNofHitsPerEvent(30);
 gemAlign->SetSignalToNoise(1000., -2., -3., -2., -3., -2., 1000.); // 1000 is an artificial threshold not to use a station
 gemAlign->SetChi2Max(0.1);
 gemAlign->SetThreshold(0.);
 gemAlign->SetMinHitsAccepted(4);
 gemAlign->SetMaxHitsAccepted(6);
 // gemAlign->SetTxMinMax(-0.05, 0.05);
 // gemAlign->SetTyMinMax(-0.05, 0.05);
 gemAlign->SetAlignmentDim("xy");
 gemAlign->SetSteerFile("steer.txt");
 gemAlign->PrepareData();
 gemAlign->StartMille();
 gemAlign->StartPede(1);
  
 delete gemAlign;
}
