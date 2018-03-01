#include "../run/bmnloadlibs.C"
#include "../../bmndata/BmnEnums.h"

//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void BmnDataToRoot(TString file, Long_t nEvents = 0, Bool_t doConvert = kTRUE)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, 6); //5 - period
//    decoder->SetBmnSetup(kSRCSETUP);
    decoder->SetBmnSetup(kBMNSETUP); // use kSRCSETUP for Short-Range Correlation program 

    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 1; // MWPC
    setup[2] = 1; // SILICON
    setup[3] = 1; // GEM
    setup[4] = 1; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 1; // ZDC
    setup[8] = 0; // ECAL
    setup[9] = 0; // LAND
    decoder->SetDetectorSetup(setup);

    decoder->SetTrigMapping((decoder->GetBmnSetup() == kBMNSETUP) ? "Trig_map_Run6.txt" : "Trig_map_Run7_SRC.txt");
    decoder->SetTrigINLFile("TRIG_INL.txt");
    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
    decoder->SetTof700Mapping("TOF700_map_period_6.txt");
    decoder->SetZDCMapping("ZDC_map_period_5.txt");
    decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetECALMapping("ECAL_map_period_5.txt");
    decoder->SetECALCalibration("");
    decoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    decoder->SetLANDMapping("land_mapping_jinr_triplex.txt");
    decoder->SetLANDPedestal("r0030_land_clock.hh");
    decoder->SetLANDTCal("r0030_land_tcal.hh");
    decoder->SetLANDDiffSync("r352_cosmic1.hh");
    decoder->SetLANDVScint("neuland_sync_2.txt");
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.

    delete decoder;
}
