#include <Rtypes.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

R__LOAD_LIBRARY(libPhysics.so)

void readSteeringFile(TString file = "") {
    bmnloadlibs(); // load BmnRoot libraries

    BmnSteering* steer = new BmnSteering(file);
    steer->PrintParamTable();

    delete steer;
}