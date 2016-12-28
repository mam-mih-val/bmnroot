void BatchDirectory(TString dirName = "/home/ilnur/bmnroot/build/") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->BatchDirectory(dirName);  
    delete bm;
}
