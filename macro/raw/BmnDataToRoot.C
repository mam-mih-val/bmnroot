
//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void BmnDataToRoot(TString file, Long_t nEvents = 0, Bool_t doConvert = kTRUE)
{
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load BmnRoot libraries
    TString dir = "/nfs/run/seans_53/";
    printf("Open file %s%s\n", dir.Data(),file.Data());
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(dir + file, nEvents, 5); //4 - period
    decoder->SetTrigMapping("Trig_map_Run5.txt");
    decoder->SetTrigINLFile("TRIG_INL.txt");
    // in case comment out the line decoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be readed from DB (only for JINR network)
    decoder->SetTof400Mapping("TOF400_PlaceMap_Period5_v3.txt", "TOF400_StripMap_Period5_v3.txt");
    decoder->SetTof700Mapping("TOF700_map_period_5.txt");
    decoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.

    delete decoder;
}
