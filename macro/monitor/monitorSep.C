#include <stdlib.h>
#include <unistd.h>
#include "../run/bmnloadlibs.C"
void monitorSep(TString RefDirName = "~/bmnroot/build/", TString RawDataDirName = "~/bmnroot/build/", TString rawFileName = "", Bool_t runCurrent = kTRUE) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    BmnMonitor *bm = new BmnMonitor();
    bm->MonitorStreamZ(RefDirName, RefDirName, runCurrent);
    sleep(2);
    std::thread threadDeco(BmnMonitor::threadDecodeWrapper, RawDataDirName, rawFileName, runCurrent);
    if (threadDeco.joinable()){
        threadDeco.detach();
        printf("detached\n");
    }
    delete bm;
}