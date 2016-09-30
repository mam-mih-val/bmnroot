
//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before
void BmnDataToRoot(TString file, Long_t nEvents = 0, Bool_t doConvert = kTRUE) {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries
  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents);
  decoder->SetPedestalRun(kFALSE);
  decoder->SetTrigMapping("Trig_map_Run4.txt");
  decoder->SetTof700Mapping("");
  if(doConvert) decoder->ConvertRawToRoot();  // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
  decoder->DecodeDataToDigi();  // Decode data into detector-digits using current mappings.
  
  delete decoder;
}
