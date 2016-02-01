void RawToRoot(char *fname="mpd_run0362.data")
{
gROOT->ProcessLine(".L BmnData.C");
gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
bmnloadlibs();
gSystem->Load("libBmnData");
BmnData(fname);
}
