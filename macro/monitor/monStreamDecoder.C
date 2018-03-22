#include "/home/ilnur/bmnroot/macro/run/bmnloadlibs.C"
#include "/home/ilnur/bmnroot/bmndata/BmnEnums.h"
void monStreamDecoder() {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
//    BmnMonitor::threadDecodeWrapper(dirName, rawFileName, runCurrent);
    
    BmnOnlineDecoder *deco = new BmnOnlineDecoder();
    deco->SetPeriodID(7);
    deco->SetBmnSetup(kBMNSETUP);
//    deco->OpenStream();
    deco->ProcessStream();
//    deco->CloseStream();
    delete deco;
}

