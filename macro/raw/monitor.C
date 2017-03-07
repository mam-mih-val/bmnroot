
void monitor(TString dirName = "/bmn/run/current/", TString refDir = "/bmn/test/mon/", TString decoAddr = "localhost") {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->MonitorStream(dirName, refDir, decoAddr);  
    delete bm;
}
