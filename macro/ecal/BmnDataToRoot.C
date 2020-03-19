//file: full path to raw file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before
void BmnDataToRoot(TString file, TString outfile = "", Long_t nEvents = 0, Bool_t doConvert = kTRUE, Bool_t doHoldRawRoot = kFALSE)
{
    gSystem->ExpandPathName(file);
    gSystem->ExpandPathName(outfile);

    Int_t iVerbose = 1; ///<- Verbosity level: 0 - Progress Bar; 1 - short info on passed events
    UInt_t period = 7;
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, outfile, nEvents, period);
    // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnSetup stp = (decoder->GetRunId() >= 2041 && decoder->GetRunId() <= 3588) ? kSRCSETUP : kBMNSETUP;
    decoder->SetBmnSetup(stp);
    decoder->SetVerbose(iVerbose);

    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0]  = 1; // TRIGGERS
    setup[1]  = 0; // MWPC
    setup[2]  = 0; // SILICON
    setup[3]  = 0; // GEM
    setup[4]  = 0; // TOF-400
    setup[5]  = 0; // TOF-700
    setup[6]  = 0; // DCH
    setup[7]  = 0; // ZDC
    setup[8]  = 1; // ECAL
    setup[9]  = 0; // LAND
    setup[10] = 0; // CSC
    decoder->SetDetectorSetup(setup);
    decoder->SetAdcDecoMode(kBMNADCMK);

    TString PeriodSetupExt = Form("%d%s.txt", period, ((stp == kBMNSETUP) ? "" : "_SRC"));
    decoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt); 
    decoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt); 
    // decoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    // decoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    // decoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    decoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    ////in case comment out the line decoder->SetTof400Mapping("...")  
    ////the maps of TOF400 will be read from DB (only for JINR network)
    // decoder->SetTOF700ReferenceRun(-1);
    // decoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt); 
    // decoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") +PeriodSetupExt, TString("TOF400_StripMap_RUN") +PeriodSetupExt);
    // if (decoder->GetRunId() >= 4278 && decoder->GetPeriodId() == 7)
        // decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", period));
    // else
        // decoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", period));
    decoder->SetZDCMapping("ZDC_map_period_5.txt");
    decoder->SetZDCCalibration("zdc_muon_calibration.txt");
    decoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
    decoder->SetECALCalibration("");
    // decoder->SetMwpcMapping(TString("MWPC_map_period") + ((period == 6 && decoder->GetRunId() < 1397) ? 5 : PeriodSetupExt));
    // decoder->SetLANDMapping("land_mapping_jinr_triplex.txt");
    // decoder->SetLANDPedestal("r0030_land_clock.hh");
    // decoder->SetLANDTCal("r0030_land_tcal.hh");
    // decoder->SetLANDDiffSync("r352_cosmic1.hh");
    // decoder->SetLANDVScint("neuland_sync_2.txt");
    decoder->InitMaps(); /// <- should be run after all mappings set
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    BmnStatus decoStatus = decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.
    if (!doHoldRawRoot) gSystem->Exec(TString::Format("rm -f %s", decoder->GetRootFileName().Data()));
    if (decoStatus == kBMNSUCCESS)
        printf("\tMacro finished successfully!\n\n"); // marker of successfully execution for software testing systems

    delete decoder;
}
