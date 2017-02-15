#include <stdlib.h>

void monitorSep(TString dirName = "~/bmnroot/build/", TString rawFileName = "", Bool_t runCurrent = kTRUE) {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    std::thread threadDeco(BmnMonitor::threadDecodeWrapper, dirName, rawFileName, runCurrent);
    if (threadDeco.joinable())
        threadDeco.detach();
    BmnMonitor *bm = new BmnMonitor();
    bm->Monitor(dirName, rawFileName, runCurrent);  
    delete bm;
}