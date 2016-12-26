void BatchDirectory(TString dirName = "/bmn/run/current/") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->BatchDirectory(dirName);  
    delete bm;
}
