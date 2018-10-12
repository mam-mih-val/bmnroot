#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"

R__LOAD_LIBRARY(libPhysics.so)
        
void readSteeringFile(TString file = "gemTrackingSteerCA.dat") {
     bmnloadlibs(); // load BmnRoot libraries
     
     BmnSteeringCA* steer = new BmnSteeringCA(file);
     steer->PrintParamTable();
     
     delete steer;
}