
void monitor(TString dirName = "/bmn/run/current/", TString rawFileName = "", Bool_t runCurrent = kTRUE) {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->MonitorStream(dirName, rawFileName, runCurrent);  
    delete bm;
}
