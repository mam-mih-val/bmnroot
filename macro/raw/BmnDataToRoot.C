
void BmnDataToRoot(char *file, Long_t nEvents = 0) {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries

  //put here path to raw data
  TString pathToRun4 = "/home/merz/BMN/dataRun4/";
  
  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(pathToRun4 + TString(file), nEvents);
  decoder->SetPedestalRun(kFALSE);
  decoder->SetT0Mapping("T0_map_Run4.txt");
  decoder->SetDchMapping("DCH_map_Run4.txt");
  decoder->SetGemMapping("GEM_map_run4.txt");
  decoder->SetTof400Mapping("");
  decoder->SetTof700Mapping("");
  decoder->ConvertRawToRoot();  // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
  decoder->DecodeDataToDigi();  // Decode data into detector-digits using current mappings.
  
  delete decoder;
}
