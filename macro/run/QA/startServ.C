#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)

void startServ() {
	TFile* file = new TFile("canvStorage.root");
	BmnDrawOnline* drawCanv = new BmnDrawOnline(file);
}
    



