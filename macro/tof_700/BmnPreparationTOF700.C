#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
#include "bmndata/BmnEnums.h"
//file: full path to raw-file
//nEvents: if 0 then decode all events
void BmnPreparationTOF700(TString file = "mpd_run_trigCode_4649.data", Long_t nEvents = 0) {
    bmnloadlibs(); // load BmnRoot libraries
    UInt_t period = 7;
    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
//    BmnSetup stp = kSRCSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnSlewingTOF700* decoder = new BmnSlewingTOF700(file, nEvents, period);
    decoder->SetBmnSetup(stp);

    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt); 
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt); 
    decoder->SetTof700Mapping("TOF700_map_period_7.txt");

    decoder->PreparationTOF700Init();  // Decode data into detector-digits using current mappings.
    BmnTof2Raw2DigitNew *tof700m = decoder->GetTof700Mapper();
    tof700m->Book();
    decoder->PreparationTOF700();  // Analyze TOF700 data for slewing time limits.
    delete decoder;
}
