#include "../run/bmnloadlibs.C"
void BatchDirectory(TString dirName = "/bmn/run/current/") {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    BmnOnlineDecoder *bm = new BmnOnlineDecoder();
    bm->BatchDirectory(dirName);  
    delete bm;
}
