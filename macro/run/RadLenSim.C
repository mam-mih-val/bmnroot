// Macro for running BM@N simulation with GEANTINO to exclude interactions
#include <Rtypes.h>
R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
#include "macro/run/geometry.C"

void RadLenSim(Int_t nEvents = 50000, TString outFile = "RadLenSim.root") {

    bmnloadlibs(); // load BmnRoot libraries

    FairRunSim *fRun = new FairRunSim();
    fRun->SetName("TGeant3");
    fRun->SetOutputFile(outFile.Data()); //output file
    geometry(fRun); // load bmn geometry

    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    FairBoxGenerator* box = new FairBoxGenerator(0, 10);
    box->SetPRange(.2, .2); // GeV/c
    box->SetPhiRange(0, 360);
    box->SetThetaRange(0, 180);
    box->SetXYZ(0., 0., 0.);
    primGen->AddGenerator(box);

    fRun->SetStoreTraj(kFALSE);
    fRun->SetRadLenRegister(kTRUE); // radiation length manager 

    fRun->Init();

    // Fill the Parameter containers for this run
    //-------------------------------------------

    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
    rtdb->print();
    fRun->Run(nEvents);
}
