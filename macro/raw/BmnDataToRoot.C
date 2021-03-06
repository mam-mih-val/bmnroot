//file: full path to raw file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void BmnDataToRoot(TString file, TString outfile = "", Long_t nEvents = 0, Bool_t doConvert = kTRUE, Bool_t doHoldRawRoot = kFALSE) {
    gSystem->ExpandPathName(file);
    gSystem->ExpandPathName(outfile);

    Int_t iVerbose = 1; ///<- Verbosity level: 0 - Progress Bar; 1 - short info on passed events
    UInt_t period = 8;

    TStopwatch timer;
    timer.Start();

    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, outfile, nEvents, period);
    // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnSetup stp = (decoder->GetRunId() >= 2041 && decoder->GetRunId() <= 3588) ? kSRCSETUP : kBMNSETUP;
    stp = kSRCSETUP;
    decoder->SetBmnSetup(stp);
    decoder->SetVerbose(iVerbose);

    std::map<DetectorId, bool> setup; // flags to determine BM@N setup
    setup.insert(std::make_pair(kBC,        1)); // TRIGGERS
    setup.insert(std::make_pair(kMWPC,      1)); // MWPC
    setup.insert(std::make_pair(kSILICON,   1)); // SILICON
    setup.insert(std::make_pair(kGEM,       1)); // GEM
    setup.insert(std::make_pair(kTOF1,      1)); // TOF-400
    setup.insert(std::make_pair(kTOF,       1)); // TOF-700
    setup.insert(std::make_pair(kDCH,       1)); // DCH
    setup.insert(std::make_pair(kZDC,       0)); // ZDC
    setup.insert(std::make_pair(kECAL,      0)); // ECAL
    setup.insert(std::make_pair(kLAND,      1)); // LAND
    setup.insert(std::make_pair(kTOFCAL,    1)); // LAND
    setup.insert(std::make_pair(kCSC,       1)); // CSC
    setup.insert(std::make_pair(kSCWALL,    1)); // SCWALL
    setup.insert(std::make_pair(kFHCAL,     1)); // FHCAL
    setup.insert(std::make_pair(kHODO,      1)); // HODO
    decoder->SetDetectorSetup(setup);
//    decoder->SetAdcDecoMode(period < 6 ? kBMNADCSM : kBMNADCMK);
    decoder->SetAdcDecoMode(kBMNADCSM);

    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    decoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    decoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    decoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be read from DB (only for JINR network)
    decoder->SetTOF700ReferenceRun(-1);
    decoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt);
    decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt, TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    if (decoder->GetRunId() >= 4278 && decoder->GetPeriodId() == 7)
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", period));
    else
        decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", period));
    decoder->SetZDCMapping("ZDC_map_period_5.txt");
    decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetScWallMapping("SCWALL_map_dry_run_2022.txt");
    decoder->SetScWallCalibration("SCWALL_calibration_2022.txt");
    decoder->SetFHCalMapping("FHCAL_map_dry_run_2022.txt");
    decoder->SetFHCalCalibration("FHCAL_calibration_2022.txt");
    decoder->SetHodoMapping("HODO_Q_map_dry_run_2022.txt");
    decoder->SetHodoCalibration("HODO_Q_calibration_2022.txt");
    decoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
    decoder->SetECALCalibration("");
    decoder->SetMwpcMapping(TString("MWPC_map_period") + ((period == 6 && decoder->GetRunId() < 1397) ? 5 : PeriodSetupExt));

    decoder->SetLANDMapping("land_mapping_jinr_triplex_2022.txt");
    decoder->SetLANDPedestal("r0030_land_clock_2022.hh");
    decoder->SetLANDTCal("r0030_land_tcal_2022.hh");
    decoder->SetLANDDiffSync("r352_cosmic1.hh");
    decoder->SetLANDVScint("neuland_sync_2.txt");    

    decoder->SetTofCalMapping("tofcal_mapping_jinr_triplex.txt.t0");
    decoder->SetTofCalPedestal("tofcal_ped_JK.hh");
    decoder->SetTofCalTCal("tofcal_tcal_JK.hh");
    decoder->SetTofCalDiffSync("tofcal_diffsync_cosmic1.hh");
    decoder->SetTofCalVScint("tofcal_sync_2022.txt");
    decoder->InitMaps(); /// <- should be run after all mappings set
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    BmnStatus decoStatus = decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.
    timer.Stop();
    if (decoStatus == kBMNSUCCESS) {
        if (!doHoldRawRoot) gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));
        printf("\tMacro finished successfully!\n\n"); // marker of successfully execution for software testing systems
        Double_t rtime = timer.RealTime();
        Double_t ctime = timer.CpuTime();
        printf("Real time %f s, CPU time %f s\n", rtime, ctime);
    }

    delete decoder;
}
