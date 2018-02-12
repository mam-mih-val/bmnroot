#include "../run/bmnloadlibs.C"

//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void BmnDataToRoot(TString file = "mpd_run_Glob_1889.data", Long_t nEvents = 0, Bool_t doConvert = kTRUE)
{
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs(); // load BmnRoot libraries
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, 6); // 6 - run period
    
    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 0; // MWPC
    setup[2] = 0; // SILICON
    setup[3] = 0; // GEM
    setup[4] = 0; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 0; // ZDC
    setup[8] = 0; // ECAL
    setup[9] = 0; // TQDC
    setup[10] = 0; // LAND
    decoder->SetDetectorSetup(setup);
    
    decoder->SetTrigMapping("Trig_map_Run6.txt");
    decoder->SetSiliconMapping("SILICON_map_run6.txt");
    decoder->SetTrigINLFile("TRIG_INL.txt");
    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be readed from DB (only for JINR network)
    decoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
    decoder->SetTof700Mapping("TOF700_map_period_6.txt"); // change for period 7
    decoder->SetTof700Geom("TOF700_geometry_run6.txt"); // change for period 7
//    decoder->SetTOF700ReferenceRun(1889);
//    decoder->SetTOF700ReferenceChamber(19);
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
//    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.

    delete decoder;
}
