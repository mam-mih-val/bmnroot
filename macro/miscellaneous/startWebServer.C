#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

void startWebServer() {
    BmnQaMonitor* mon = new BmnQaMonitor();
    mon->SetHistoDir("/nfs/RUN7_res/QA/output"); // absolute path to histo files
} 