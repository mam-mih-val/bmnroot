
void BmnDataToRoot(char *file, Long_t nEvents = 0) {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries

  TString pathToRun4 = "/home/merz/BMN/dataRun4/"; //FIXME: DELETE before commit!
  
  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(pathToRun4 + TString(file), nEvents);
  decoder->SetPedestalRun(kFALSE);
  decoder->SetDchMapping("DCH_map_Run4.txt");
  decoder->SetGemMapping("GEM_map_run4.txt");
  decoder->SetTof400Mapping("");
  decoder->SetTof700Mapping("");
  decoder->ConvertRawToRoot();
  decoder->DecodeDataToDigi();
  
  delete decoder;
}
