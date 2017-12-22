#include <stdlib.h>
#include "../run/bmnloadlibs.C"
void monitorSep(TString dirName = "~/bmnroot/build/", TString rawFileName = "", Bool_t runCurrent = kTRUE) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    std::thread threadDeco(BmnMonitor::threadDecodeWrapper, dirName, rawFileName, runCurrent);
    if (threadDeco.joinable())
        threadDeco.detach();
    BmnMonitor *bm = new BmnMonitor();
    bm->Monitor(dirName, rawFileName, runCurrent);  
    delete bm;
}