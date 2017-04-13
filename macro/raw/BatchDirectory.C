void BatchDirectory(TString dirName = "/bmn/run/current/") {
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnOnlineDecoder *bm = new BmnOnlineDecoder();
    bm->BatchDirectory(dirName);  
    delete bm;
}
