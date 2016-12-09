#include "TH1I.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TClonesArray.h"
#include <TMemFile.h>
#include "TGraph.h"
#include "TMath.h"
#include "TImage.h"
#include <stdlib.h>
#include "/home/ilnur/bmnroot/monitor/BmnHistGem.h"
#include <unistd.h>
#include <vector>
#include "THttpServer.h"
#include "TSystem.h"
#include "TFolder.h"


void monitor(TString digiName = "$VMCWORKDIR/macro/raw/bmn_run0084_digi.root") {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs();
    BmnMonitor *bm = new BmnMonitor();
    bm->Monitor("/bmn/run/seans_53/", "");  
    delete bm;
}
