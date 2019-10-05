#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

void startWebServer() {
    TString pathToHistos = "";
    BmnQaMonitor* mon = new BmnQaMonitor(pathToHistos);
} 
