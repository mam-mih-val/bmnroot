#include <Rtypes.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TFile.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"       

using namespace std;
using namespace TMath;

void residAnal(TString dataPath = "", TString mcPath = "") {
    BmnDataAnalRun7* dAnal = new BmnDataAnalRun7(dataPath);
    
    dAnal->SetMcData(mcPath); // if necessary
    dAnal->SetGeometryFile(""); // path to geometry 
    
    // Some setters to speed up the procedure by limiting number of files / events to be processed (if necessary)
    // dAnal->SetNFilesInList(10);
    // dAnal->SetNEventsInFile(10000);
    dAnal->SetTarget(""); // target should be passed obligatory (C, Al, Cu, Sn, Pb are available within the anal.)

    dAnal->doResidAnal();
}