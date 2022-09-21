#if !defined(__CLING__) || defined(__MAKECLING__)
// ROOT includes
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TPRegexp.h"
#include "TRandom.h"
#include "TKey.h"
#include "TApplication.h"
#include "TDatabasePDG.h"
#include "TVirtualMC.h"

// FairRoot includes
#include "FairRunSim.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairTrajFilter.h"
#include "FairModule.h"
#include "FairDetector.h"
#include "FairCave.h"
#include "FairPipe.h"
#include "FairMagnet.h"
#include "FairPrimaryGenerator.h"
#include "FairParticleGenerator.h"
#include "FairBoxGenerator.h"
#include "FairIonGenerator.h"
#include "FairRootFileSink.h"

// BmnRoot includes
#include "UniRun.h"
#include "BmnFieldMap.h"
#include "BmnFieldConst.h"
#include "BmnNewFieldMap.h"
#include "BmnTOF.h"
#include "BmnFD.h"
#include "CbmSts.h"
#include "BmnTOF1.h"
#include "BmnDch.h"
#include "BmnMwpc.h"
#include "BmnBd.h"
#include "BmnEcal.h"
#include "BmnSilicon.h"
#include "BmnBC.h"
#include "BmnArmTrig.h"
#include "MpdUrqmdGenerator.h"
#include "BmnSiliconDigitizer.h"
#include "BmnGemStripDigitizer.h"
#include "BmnGemStripMedium.h"
#include "BmnFieldPar.h"
#include "BmnZdc.h"
#include "BmnCSC.h"
#include "BmnFunctionSet.h"
#include "MpdGetNumEvents.h"
#include "MpdPHSDGenerator.h"
#include "MpdLAQGSMGenerator.h"
#include "MpdDCMSMMGenerator.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCDigitizer.h"
#include "BmnZdcDigitizer.h"
#include "BmnEcalDigitizer.h"
#include "BmnInnTrackerAlign.h"
#include "CbmStack.h"

// GEANT3 includes
#include "TGeant3.h"
#include "TGeant3TGeo.h"
#include "../../gconfig/g3Config.C"
#include "../../gconfig/SetCuts.C"

// C++ includes
#include <iostream>
using namespace std;
#endif

void geant3_setup() { Config(); SetCuts(); }

#define GEANT3  // Choose: GEANT3 GEANT4
#define BOX
// inFile - input file with generator data, if needed
// outFile - output file with MC data, default: bmnsim.root
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// generatorName - generator name for the input file (enumeration above)
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)
void run_sim_src(TString inFile = "", TString outFile = "$VMCWORKDIR/macro/run/srcsim.root", Int_t nStartEvent = 0, Int_t nEvents = 10, Bool_t useRealEffects = kFALSE)
{
TStopwatch timer;
    timer.Start();
    gDebug = 0;

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* fRun = new FairRunSim();
    fRun->SetSimSetup(geant3_setup);

    // Choose the Geant Navigation System
#ifdef GEANT3
    fRun->SetName("TGeant3");
#else
    fRun->SetName("TGeant4");
#endif

    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    FairModule* magnetSP57 = new FairMagnet("MAGNET_SP57");
    magnetSP57->SetGeometryFileName("magnetSP57_1.root");
    fRun->AddModule(magnetSP57);
    
    // -----   Create detectors        -------------------------

    FairDetector* mwpc = new BmnMwpc("MWPC", kTRUE);
    mwpc->SetGeometryFileName("MWPC_RunSRCSpring2018.root");
    fRun->AddModule(mwpc);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_RunSRCSpring2018.root");
    fRun->AddModule(silicon);

    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName("GEMS_RunSRCSpring2018.root");
    fRun->AddModule(sts);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("CSC_RunSRCSpring2018.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN7_SRC_AllignmentZY_v3.root");
    fRun->AddModule(tof1);

    FairDetector* tof = new BmnTOF("TOF", kTRUE);
    tof->SetGeometryFileName("tof700_run7_with_support.root");
    fRun->AddModule(tof);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_RunSpring2018.root");
    fRun->AddModule(dch);

    BmnZdc* zdc = new BmnZdc("ZDC", kTRUE);
    zdc->SetGeometryFileName("ZDC_RunSpring2018.root");
    fRun->AddModule(zdc);
    
    FairDetector* arm = new BmnArmTrig("SRCArmTriggers", kTRUE);
    arm->SetGeometryFileName("SRCArmTriggers_Spring2018.root");
    fRun->AddModule(arm);

    FairDetector* bc = new BmnBC("BC", kTRUE);
    bc->SetGeometryFileName("BC_RunSRCSpring2018.root");
    fRun->AddModule(bc);

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Smearing of beam interaction point, if needed, and primary vertex position
    // DO NOT do it in corresponding gen. sections to avoid incorrect summation!!!
    primGen->SetBeam(0.5, -4.6, 0.0, 0.0);
    primGen->SetTarget(-647.5, 0.0);
    primGen->SmearVertexZ(kFALSE);
    primGen->SmearVertexXY(kFALSE);

#ifdef URQMD
    // ------- UrQMD Generator
    if (!BmnFunctionSet::CheckFileExist(inFile)) return;

    MpdUrqmdGenerator* urqmdGen = new MpdUrqmdGenerator(inFile);
    //urqmdGen->SetEventPlane(0., 360.);
    primGen->AddGenerator(urqmdGen);
    if (nStartEvent > 0) urqmdGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumURQMDEvents(inFile.Data()) - nStartEvent;

#else
#ifdef PART
    // ------- Particle Generator
    FairParticleGenerator* partGen =
            new FairParticleGenerator(211, 10, 1, 0, 3, 1, 0, 0);
    primGen->AddGenerator(partGen);

#else
#ifdef ION
    // ------- Ion Generator
    // Start beam from a far point to check mom. reconstruction procedure
    FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, 0., 0., 4., 0., 0., -647.);
    primGen->AddGenerator(fIongen);

#else
#ifdef BOX
    gRandom->SetSeed(0);
    // ------- Box Generator
    FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 10); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(2., 2.); // GeV/c, setPRange vs setPtRange
    boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
    boxGen->SetThetaRange(0., 40.); // Polar angle in lab system range [degree]
    primGen->AddGenerator(boxGen);

#else
#ifdef HSD
    // ------- HSD/PHSD Generator
    if (!BmnFunctionSet::CheckFileExist(inFile)) return;

    MpdPHSDGenerator* hsdGen = new MpdPHSDGenerator(inFile.Data());
    //hsdGen->SetPsiRP(0.); // set fixed Reaction Plane angle instead of random
    primGen->AddGenerator(hsdGen);
    if (nStartEvent > 0) hsdGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumPHSDEvents(inFile.Data()) - nStartEvent;

#else
#ifdef QGSM
    // ------- LAQGSM/DCM-QGSM Generator
    if (!BmnFunctionSet::CheckFileExist(inFile)) return;

    MpdLAQGSMGenerator* guGen = new MpdLAQGSMGenerator(inFile.Data(), kFALSE);
    primGen->AddGenerator(guGen);
    if (nStartEvent > 0) guGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumQGSMEvents(inFile.Data()) - nStartEvent;

#endif
#endif
#endif
#endif
#endif
#endif

    fRun->SetSink(new FairRootFileSink(outFile.Data()));
    fRun->SetIsMT(false);

    // -----   Create magnetic field   ----------------------------------------
    Double_t fieldScale = 1800. / 900.;
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v5_ascii_Extrap.root");
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager

    // SI-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSRCSpring2018;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetOnlyPrimary(kFALSE);
    fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSRCSpring2018;
    if (useRealEffects)
        BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_90_10_E_880_1920_2780_3160_B_0_9T);
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetOnlyPrimary(kFALSE);
    gemDigit->SetStripMatching(kTRUE);
    fRun->AddTask(gemDigit);

    // CSC-Digitizer
    BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::RunSRCSpring2018;
    BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
    cscDigit->SetCurrentConfig(csc_config);
    cscDigit->SetOnlyPrimary(kFALSE);
    cscDigit->SetStripMatching(kTRUE);
    fRun->AddTask(cscDigit);

    fRun->Init();
    magField->Print("");

    // Trajectories Visualization (TGeoManager only)
    FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    // Set cuts for storing the trajectories
    trajFilter->SetStepSizeCut(0.01); // 1 cm
    trajFilter->SetVertexCut(-200., -200., -150., 200., 200., 1100.);
    trajFilter->SetMomentumCutP(10e-3); // p_lab > 10 MeV
    trajFilter->SetEnergyCut(0., 4.); // 0 < Etot < 1.04 GeV //
    trajFilter->SetStorePrimaries(kTRUE);
    trajFilter->SetStoreSecondaries(kTRUE); //kFALSE

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

    // Transport nEvents
    fRun->Run(nEvents);

    //gGeoManager->CheckOverlaps(0.0001);
    //gGeoManager->PrintOverlaps();
    //fRun->CreateGeometryFile("full_geometry.root");  // save the full setup geometry to the additional file

#ifdef QGSM
    TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(inFile.Data()), ".g", (fRun->GetName())[6], ".pdg_table.dat");
    (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
#endif

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
    delete fRun;
    delete magField;
}

int main(int argc, char** arg)
{
   run_sim_src();
}
