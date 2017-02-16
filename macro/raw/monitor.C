
void monitor(TString dirName = "/bmn/run/current/",TString refDir = "") {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->MonitorStream(dirName, refDir);  
    delete bm;
}
