#include "../run/bmnloadlibs.C"
R__LOAD_LIBRARY(libFairMQ)
void BatchDirectory(TString dirName = "/bmn/run/current/",
        BmnSetup stp = kSRCSETUP, Int_t periodId = 7) {
//#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
//    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
//#endif
//    bmnloadlibs();
//    int64_t gl = gSystem->Load("libBmnMonitor");
//    printf("gl %ld\n", gl);
    BmnOnlineDecoder *bm = new BmnOnlineDecoder();
    bm->SetBmnSetup(stp);
    bm->SetPeriodID(periodId);
    bm->BatchDirectory(dirName);  
    delete bm;
}
