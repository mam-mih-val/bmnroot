
void BmnDataToRoot(char *file) {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries

  TString pathToRun4 = "/home/merz/BMN/dataRun4/"; //FIXME: DELETE before commit!
  
  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(pathToRun4 + TString(file));
  decoder->ConvertRawToRoot();
  cout << "RUN#" << decoder->GetRunId() << " contains " << decoder->GetNevents() << " events" << endl;
  delete decoder;
}
