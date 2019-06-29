//file: full path to raw file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before
void BmnDataToRoot(TString file, Long_t nEvents = 0, Bool_t doConvert = kTRUE, Bool_t doHoldRawRoot = kFALSE)
{
    gSystem->ExpandPathName(file);

    UInt_t period = 7;
    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, period);
    decoder->SetBmnSetup(stp);

    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0]  = 1; // TRIGGERS
    setup[1]  = 1; // MWPC
    setup[2]  = 1; // SILICON
    setup[3]  = 1; // GEM
    setup[4]  = 1; // TOF-400
    setup[5]  = 1; // TOF-700
    setup[6]  = 1; // DCH
    setup[7]  = 1; // ZDC
    setup[8]  = 1; // ECAL
    setup[9]  = 1; // LAND
    setup[10] = 1; // CSC
    decoder->SetDetectorSetup(setup);

    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt); 
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt); 
    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    decoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") +PeriodSetupExt, TString("TOF400_StripMap_RUN") +PeriodSetupExt);
    decoder->SetTof700Mapping("TOF700_map_period_7.txt");
    decoder->SetZDCMapping("ZDC_map_period_5.txt");
    decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetECALMapping("ECAL_map_period_7.txt");
    decoder->SetECALCalibration("");
    decoder->SetMwpcMapping(TString("MWPC_map_period") + PeriodSetupExt);
    decoder->SetLANDMapping("land_mapping_jinr_triplex.txt");
    decoder->SetLANDPedestal("r0030_land_clock.hh");
    decoder->SetLANDTCal("r0030_land_tcal.hh");
    decoder->SetLANDDiffSync("r352_cosmic1.hh");
    decoder->SetLANDVScint("neuland_sync_2.txt");
    decoder->InitMaps();
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.
    if (!doHoldRawRoot) gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));

    delete decoder;
}
