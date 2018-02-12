#include "../run/bmnloadlibs.C"
void monitor(TString dirName = "/bmn/test/mon/", TString refDir = "/bmn/test/mon/", TString decoAddr = "localhost") {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    BmnMonitor *bm = new BmnMonitor();
    bm->SetPeriodID(6);
    bm->MonitorStreamZ(dirName, refDir, decoAddr);  
    delete bm;
}
