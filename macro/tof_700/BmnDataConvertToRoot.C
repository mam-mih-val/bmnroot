
void BmnDataConvertToRoot(char *file = "mpd_run815.data") {
  gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
  bmnloadlibs(); // load BmnRoot libraries

  BmnRawDataDecoder* decoder = new BmnRawDataDecoder(TString(file));
  decoder->ConvertRawToRoot();
  cout << "RUN#" << decoder->GetRunId() << " contains " << decoder->GetNevents() << " events" << endl;
  delete decoder;
}
