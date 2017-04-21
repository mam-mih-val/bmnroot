
void monDecoder(TString dirName = "/bmn/run/current/", TString rawFileName = "", Bool_t runCurrent = kTRUE) {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    BmnMonitor::threadDecodeWrapper(dirName, rawFileName, runCurrent);
}

