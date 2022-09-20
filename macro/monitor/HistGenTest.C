#include <TCanvas.h>

#include "/home/ilnur/bmnroot/macro/run/bmnloadlibs.C"
void HistGenTest(
        TString FileName = "pad.json") {
#if ROOT_VERSION_CODE < ROOT_VERSION(5,99,99)
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
#endif
    bmnloadlibs();
    gSystem->Load("libBmnMonitor");
    BmnPadGenerator *g = new BmnPadGenerator();
    g->LoadPTFrom(FileName);
    BmnPadBranch * br = g->GetPadBranch();
    TCanvas* can = new TCanvas("canHits", "", 1920, 1080);
    g->PadTree2Canvas(br, can);
    BmnHist::DrawPadFromTree(br);
    
    can->Update();
    can->Modified();
    can->SaveAs("/home/ilnur/filesbmn/autocan.png");
    delete g;
}
