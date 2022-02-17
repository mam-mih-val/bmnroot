
#include "TSystem.h"

R__LOAD_LIBRARY(libFairMQ)
//#include "/home/ilnur/bmnroot/macro/run/bmnloadlibs.C"
//R__ADD_INCLUDE_PATH($VMCWORKDIR)
void monitor(
        Int_t periodID = 8,
        TString dirName = "/home/ilnur/bmnroot/build/", 
        TString refDir = "/home/ilnur/bmnroot/build/", 
        TString decoAddr = "localhost",
        Int_t webPort = 9001) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
//    bmnloadlibs();
//    gSystem->Load("libBmnMonitor");
    BmnMonitor *bm = new BmnMonitor();
    bm->SetPeriodID(periodID);
    bm->SetBmnSetup(kSRCSETUP);
    bm->MonitorStreamZ(dirName, refDir, decoAddr, webPort);  
    delete bm;
}
