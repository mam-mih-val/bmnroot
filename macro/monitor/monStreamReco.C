//#include "/home/ilnur/bmnroot/macro/run/bmnloadlibs.C"
R__LOAD_LIBRARY(libFairMQ)
//R__ADD_INCLUDE_PATH($VMCWORKDIR)
void monStreamReco(
        Int_t periodID = 7,
        TString dirName = "/home/ilnur/bmnroot/build/", 
        TString refDir = "/home/ilnur/bmnroot/build/", 
        TString decoAddr = "localhost",
        Int_t webPort = 9000) {
//#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
//    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
//#endif
//    bmnloadlibs();
//    gSystem->Load("libBmnMonitor");
    
    
    BmnOnlineReco *rec = new BmnOnlineReco();
//    rec->SetPeriodID(periodID);
    rec->RecoStream(dirName, refDir, decoAddr, webPort);  
    delete rec;
}
