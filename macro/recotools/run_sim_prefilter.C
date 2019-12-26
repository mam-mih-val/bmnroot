R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__ADD_INCLUDE_PATH($SIMPATH / include / fairmq)

#include <cstdint>

#include "macro/run/geometry_run/geometry_run7.C"


#define GEANT3  // Choose: GEANT3 GEANT4
// enumeration of generator names corresponding input files
enum enumGenerators {
    URQMD, QGSM, HSD, BOX, PART, ION, DQGSM
};


void run_sim_prefilter(
        TString dir = "/ncx/eos/nica/bmn/sim/gen/DCMQGSM/ArAl_3.2AGeV_mb/",
        TString outFile = "$VMCWORKDIR/macro/run/Filtered_ArAl_3.2AGeV.root",
        Int_t SelPDG = 3122) {
    TStopwatch timer;
    timer.Start();
    gDebug = 0;
    Int_t fVerbose = 2;

    // -----   Create simulation run   ----------------------------------------
    BmnFairRunSim* fRun = new BmnFairRunSim();

    // Choose the Geant Navigation System
#ifdef GEANT3
    fRun->SetName("TGeant3");
#else
    fRun->SetName("TGeant4");
#endif

//    geometry(fRun); // load BM@N geometry
    // Set Material file Name
    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Smearing of beam interaction point, if needed, and primary vertex position
    // DO NOT do it in corresponding gen. sections to avoid incorrect summation!!!
    primGen->SetBeam(0.0, 0.0, 0.0, 0.0);
    primGen->SetTarget(0.0, 0.0);
    primGen->SmearVertexZ(kFALSE);
    primGen->SmearVertexXY(kFALSE);
    //    FairMQProxy * sink = new FairMQProxy();
    //    fRun->SetSink((FairSink*)sink);
    FairRootFileSink* sink = new FairRootFileSink(outFile.Data());
    fRun->SetSink(sink);
    sink->InitSink();
    fRun->SetIsMT(false);

//    Int_t lastSlash = dir.Last('/');
//    TString simDir = (lastSlash == TString::kNPOS) ? "" : TString(dir(0, lastSlash + 1));
    vector<TString> fileVec = BmnRecoTools::GetFileVecFromDir(dir);
    printf("Found %lu files\n", fileVec.size());
    MpdLAQGSMGeneratorExt* guGen = new MpdLAQGSMGeneratorExt(fileVec, kFALSE, 0, 0, SelPDG, kTRUE);
    guGen->SetRunSimInst(fRun);

    primGen->AddGenerator(guGen);

    // -----   Create magnetic field   ----------------------------------------
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    Double_t fieldScale = 1200. / 900.;
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager

    fRun->Init();
    magField->Print();
//    // Trajectories Visualization (TGeoManager only)
//    FairTrajFilter* trajFilter = FairTrajFilter::Instance();
//    // Set cuts for storing the trajectories
//    trajFilter->SetStepSizeCut(0.01); // 1 cm
//    trajFilter->SetVertexCut(-200., -200., -150., 200., 200., 1100.);
//    trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
//    trajFilter->SetEnergyCut(0., 4.); // 0 < Etot < 1.04 GeV //
//    trajFilter->SetStorePrimaries(kTRUE);
//    trajFilter->SetStoreSecondaries(kTRUE); //kFALSE
    // Fill the Parameter containers for this run
    FairRuntimeDb *rtdb = fRun->GetRuntimeDb();

    BmnFieldPar* fieldPar = (BmnFieldPar*) rtdb->getContainer("BmnFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    fieldPar->setInputVersion(fRun->GetRunId(), 1);
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* output = new FairParRootFileIo(kParameterMerged);
    //AZ output->open(parFile.Data());
    output->open(gFile);
    rtdb->setOutput(output);

    rtdb->saveOutput();
    rtdb->print();

    Int_t doNEvents = 1000000;//INT32_MAX;
    TTree * tree = sink->GetOutTree();
    TFile *fRootFile = tree->GetCurrentFile();
    printf("frootfile name %s\n", fRootFile->GetName());
    fRootFile->cd();
    fRun->Run(doNEvents);

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
}
