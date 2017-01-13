void RawToRoot(char *fname="mpd_run834.data")
{
gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
bmnloadlibs();
gSystem->SetIncludePath(" -Wno-ignored-qualifiers -Wno-overloaded-virtual -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-result -Wno-deprecated -I$ROOTSYS/include/root -I$VMCWORKDIR/bmndata");
gROOT->ProcessLine(".L BmnData.C++");
BmnData(fname);
}
