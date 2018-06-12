#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

R__LOAD_LIBRARY(libPhysics.so)
        
void readSteeringFile(TString file = "gemTrackingSteer.dat") {
     bmnloadlibs(); // load BmnRoot libraries
     
     BmnSteeringGemTracking* steer = new BmnSteeringGemTracking(file);
     steer->PrintParamTable();
     
     delete steer;
}