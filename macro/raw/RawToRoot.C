void RawToRoot(char *fname="mpd_run362.data")
{
gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
bmnloadlibs();
gSystem->SetIncludePath(" -Wno-ignored-qualifiers -Wno-overloaded-virtual -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-result -Wno-deprecated -I/usr/local/fairsoft/include/root -I/home/petukhov/bmnroot/bmndata");
gROOT->ProcessLine(".L BmnData.C++");
BmnData(fname);
}
