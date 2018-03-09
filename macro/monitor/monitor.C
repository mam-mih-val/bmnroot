#include "/home/ilnur/bmnroot/macro/run/bmnloadlibs.C"
void monitor(
        Int_t periodID = 7,
        TString dirName = "/home/ilnur/bmnroot/build/", 
        TString refDir = "/home/ilnur/bmnroot/build/", 
        TString decoAddr = "localhost",
        Int_t webPort = 9000) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    BmnMonitor *bm = new BmnMonitor();
    bm->SetPeriodID(periodID);
    bm->MonitorStreamZ(dirName, refDir, decoAddr, webPort);  
    delete bm;
}
