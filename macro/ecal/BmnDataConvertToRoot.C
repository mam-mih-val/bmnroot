#include "../../bmndata/BmnEnums.h"

void BmnDataConvertToRoot(char *file = "/ceph/bmn/run/seans_55/4720-5186_BMN_Krypton/mpd_run_trigCode_4926.data") {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries

    UInt_t period = 7, nEvents = 0;
    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, period);
    decoder->SetBmnSetup(stp);
    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigMapping(TString("Trig_map_Run") + PeriodSetupExt);
    decoder->SetTrigINLFile("TRIG_INL_076D-16A8.txt");//INL only for TDC data 
    decoder->SetECALMapping("ECAL_map_period_7.txt");
    decoder->SetECALCalibration("");
    decoder->InitMaps();

  decoder->ConvertRawToRoot();
  cout << "RUN#" << decoder->GetRunId() << " contains " << decoder->GetNevents() << " events" << endl;
  delete decoder;
}
