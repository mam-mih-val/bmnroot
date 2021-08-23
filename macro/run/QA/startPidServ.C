#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

void startPidServ() {
	TFile* file = new TFile("pidStorage.root");
	BmnDrawOnline* drawCanv = new BmnDrawOnline(file);
}
    



