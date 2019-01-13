#include "../../bmndata/BmnEnums.h"
#include "../run/bmnloadlibs.C"
//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void BmnDataToRoot(TString file = "mpd_run_trigCode_4153.data", Long_t nEvents = 0, Bool_t doConvert = kFALSE)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries

    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, 7); //7 - period
    decoder->SetBmnSetup(stp);
    
    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 0; // MWPC
    setup[2] = 0; // SILICON
    setup[3] = 0; // GEM
    setup[4] = 0; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 0; // DCH
    setup[7] = 0; // ZDC
    setup[8] = 0; // ECAL
    setup[9] = 0; // LAND
    setup[10] = 0; // CSC
    decoder->SetDetectorSetup(setup);
    
    decoder->SetSiliconMapping("SILICON_map_run6.txt");

    decoder->SetTrigMapping("Trig_map_Run7.txt");
    //decoder->SetTrigMapping("Trig_map_Run7_SRC.txt");
    //decoder->SetTrigINLFile("TRIG_INL.txt"); // run period 6
    //decoder->SetTrigINLFile("TRIG_INL_076D-16A8.txt");//run period 7 SRC INL only for TDC data 
    decoder->SetTrigINLFile("TRIG_INL_076D-180A.txt");//run period 7 BMN INL only for TDC data 

    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be readed from DB (only for JINR network)
    decoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
    //
    // tof700 stuff
    decoder->SetTof700Mapping("TOF700_map_period_7.txt");
    decoder->SetTof700Geom("TOF700_geometry_run7.txt");
    // following line contains directive to use slewing parameters not from analyzed run, but from other run
    decoder->SetTOF700ReferenceRun(9649);  // slewing run
    // end tof700 stuff
    //
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
