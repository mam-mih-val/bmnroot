
void monitor(TString dirName = "/bmn/test/mon/", TString refDir = "/bmn/test/mon/", TString decoAddr = "localhost") {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->MonitorStreamZ(dirName, refDir, decoAddr);  
    delete bm;
}
