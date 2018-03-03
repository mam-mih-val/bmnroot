#include "../run/bmnloadlibs.C"
#include "../bmndata/BmnEnums.h"
void monDecoder(TString dirName = "/bmn/run/current/", TString rawFileName = "", Bool_t runCurrent = kTRUE) {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
//    BmnMonitor::threadDecodeWrapper(dirName, rawFileName, runCurrent);
    
    BmnOnlineDecoder *deco = new BmnOnlineDecoder();
    deco->SetPeriodID(7);
    deco->SetBmnSetup(kSRCSETUP);
    deco->Decode(dirName, rawFileName, runCurrent);
    delete deco;
}

