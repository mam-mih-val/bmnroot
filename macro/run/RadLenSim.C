// Macro for running BM@N simulation with GEANTINO to exclude interactions

void RadLenSim(Int_t nEvents = 50, TString outFile = "RadLenSim.root") {

    gROOT->LoadMacro("$VMCWORKDIR/macro/run/bmnloadlibs.C");
    bmnloadlibs(); // load libraries

    FairRunSim *fRun = new FairRunSim();
    fRun->SetName("TGeant3");
    fRun->SetOutputFile(outFile.Data()); //output file
    gROOT->LoadMacro("$VMCWORKDIR/macro/run/geometry.C");
    geometry(fRun); // load bmn geometry

    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    FairBoxGenerator* box = new FairBoxGenerator(0, 10000);
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
