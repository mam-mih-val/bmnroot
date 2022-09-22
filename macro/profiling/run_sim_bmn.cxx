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
#include "TGeoManager.h"

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
#include "FairTarget.h"
#include "FairPrimaryGenerator.h"
#include "FairParticleGenerator.h"
#include "FairBoxGenerator.h"
#include "FairIonGenerator.h"
#include "FairRootFileSink.h"

// BmnRoot includes
#include "BmnBd.h"
#include "BmnBdDigitizer.h"
#include "BmnCSC.h"
#include "BmnCSCConfiguration.h"
#include "BmnCSCDigitizer.h"
#include "BmnDch.h"
#include "BmnEcal.h"
#include "BmnEcalDigitizer.h"
#include "BmnFD.h"
#include "BmnFHCal.h"
#include "BmnFHCalDigitizer.h"
#include "BmnFieldConst.h"
#include "BmnFieldMap.h"
#include "BmnFieldPar.h"
#include "BmnFunctionSet.h"
#include "BmnHodo.h"
#include "BmnHodoDigitizer.h"
#include "BmnGemStripDigitizer.h"
#include "BmnGemStripMedium.h"
#include "BmnInnTrackerAlign.h"
#include "BmnMwpc.h"
#include "BmnNewFieldMap.h"
#include "BmnScWall.h"
#include "BmnScWallDigitizer.h"
#include "BmnSiBT.h"
#include "BmnSiBTConfiguration.h"
#include "BmnSiBTDigitizer.h"
#include "BmnSiMD.h"
#include "BmnSiMDDigitizer.h"
#include "BmnSilicon.h"
#include "BmnSiliconDigitizer.h"
#include "BmnTOF.h"
#include "BmnTOF1.h"
#include "BmnZdc.h"
#include "BmnZdcDigitizer.h"
#include "CbmStack.h"
#include "CbmSts.h"
#include "MpdDCMSMMGenerator.h"
#include "MpdGetNumEvents.h"
#include "MpdLAQGSMGenerator.h"
#include "MpdPHSDGenerator.h"
#include "MpdUrqmdGenerator.h"
#include "UniRun.h"

// GEANT3 and 4 includes
#include "TGeant4.h"
#include "TG4RunConfiguration.h"
#include "TPythia6Decayer.h"
#include "TVirtualMC.h"
//#include "TGeant3.h"
//#include "TGeant3TGeo.h"

#include "../../gconfig/g4Config.C"
//#include "../../gconfig/g3Config.C"
#include "../../gconfig/SetCuts.C"

// C++ includes
#include <iostream>
using namespace std;
#endif

void geant3_setup() { Config(); SetCuts(); }
void geant4_setup() { Config(); }

#define GEANT4  // Choose: GEANT3 GEANT4
// enumeration of generator names corresponding input files
enum enumGenerators{URQMD, QGSM, HSD, BOX, PART, ION, DCMQGSM, DCMSMM};
// inFile - input file with generator data, if needed
// outFile - output file with MC data, default: bmnsim.root
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// generatorName - generator name for the input file (enumeration above)
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)
void run_sim_bmn(TString inFile = "/opt/data/ArCu_3.2AGeV_mb_156.r12", TString outFile = "$VMCWORKDIR/macro/run/bmnsim.root",
                 Int_t nStartEvent = 0, Int_t nEvents = 10, enumGenerators generatorName = BOX, Bool_t useRealEffects = kFALSE)
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* fRun = new FairRunSim();

    // Choose the Geant Navigation System
#ifdef GEANT4
    fRun->SetSimSetup(geant4_setup);
    fRun->SetName("TGeant4");
#else
    fRun->SetSimSetup(geant3_setup);
    fRun->SetName("TGeant3");
#endif

    fRun->SetMaterials("media.geo");

    // -----   Create passive volumes   -------------------------
    FairModule* cave = new FairCave("CAVE");
    cave->SetGeometryFileName("cave.geo");
    fRun->AddModule(cave);

    FairModule* magnet = new FairMagnet("MAGNET");
    magnet->SetGeometryFileName("magnet_modified.root");
    fRun->AddModule(magnet);

    FairModule* target = new FairTarget("Target");
    target->SetGeometryFileName("target_CsI.geo");
    fRun->AddModule(target);
    
    // -----   Create detectors        -------------------------

    FairDetector* sibt = new BmnSiBT("SiBT", kTRUE);
    sibt->SetGeometryFileName("SiBT_Run8.root");
    fRun->AddModule(sibt);

    FairDetector* simd = new BmnSiMD("SiMD", kTRUE);
    simd->SetGeometryFileName("SiMD_run8_v1.root");
    fRun->AddModule(simd);

    FairDetector* bd = new BmnBd("BD", kTRUE);
    bd->SetGeometryFileName("BD_run8_v1.root");
    fRun->AddModule(bd);

    FairDetector* fd = new BmnFD("FD", kTRUE);
    fd->SetGeometryFileName("FD_run8.root");
    fRun->AddModule(fd);

    FairDetector* silicon = new BmnSilicon("SILICON", kTRUE);
    silicon->SetGeometryFileName("Silicon_Run8_3stations_detailed.root");
    fRun->AddModule(silicon);

    FairDetector* gems = new CbmSts("GEM", kTRUE);
    gems->SetGeometryFileName("GEMS_Run8_detailed.root");
    fRun->AddModule(gems);

    FairDetector* csc = new BmnCSC("CSC", kTRUE);
    csc->SetGeometryFileName("CSC_Run8_detailed.root");
    fRun->AddModule(csc);

    FairDetector* tof1 = new BmnTOF1("TOF1", kTRUE);
    tof1->SetGeometryFileName("TOF400_RUN7.root");
    fRun->AddModule(tof1);

    FairDetector* dch = new BmnDch("DCH", kTRUE);
    dch->SetGeometryFileName("DCH_Run8.root");
    fRun->AddModule(dch);

    FairDetector* tof2 = new BmnTOF("TOF", kTRUE);
    tof2->SetGeometryFileName("tof700_run8_with_support.root");
    fRun->AddModule(tof2);

    FairDetector* ecal = new BmnEcal("ECAL", kTRUE);
    ecal->SetGeometryFileName("ECAL_v3_run8_pos5.root");
    fRun->AddModule(ecal);

    FairDetector* scwall = new BmnScWall("SCWALL", kTRUE);
    scwall->SetGeometryFileName("ScWall_run8_with_hole_in_box_with_hole_XeCsI_3.9GeV_field_Extrap_scale_1800_900_Zpos_875.0cm_Xshift_78.0cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(scwall);

    FairDetector* hodo = new BmnHodo("HODO", kTRUE);
    hodo->SetGeometryFileName("Hodo_with_box_XeCsI_3.9GeV_field_Extrap_scale_1800_900_Zpos_892.0cm_Xshift_55.50cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(hodo);

    FairDetector* fhcal = new BmnFHCal("FHCAL", kTRUE);
    //zdc->SetBirk();
    fhcal->SetGeometryFileName("FHCal_54mods_hole_XeCsI_3.9GeV_field_Extrap_scale_1800_900_Zpos_900.0cm_Xshift_55.5cm_Yshift_0.0cm_rotationY_0.0deg_v1.root");
    fRun->AddModule(fhcal);
    // Use the experiment specific MC Event header instead of the default one
    // This one stores additional information about the reaction plane
    //MpdMCEventHeader* mcHeader = new MpdMCEventHeader();
    //fRun->SetMCEventHeader(mcHeader);

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Smearing of beam interaction point, if needed, and primary vertex position
    // DO NOT do it in corresponding gen. sections to avoid incorrect summation!!!
    //parameters for period 8
    primGen->SetBeam(0.0, 0.0, 0.3, 0.3);  // (beamX0, beamY0, beamSigmaX, beamSigmaY)
    primGen->SetBeamAngle(0.0, 0.0, 0.0, 0.0);  // (beamAngleX0, beamAngleY0, beamAngleSigmaX, beamAngleSigmaY)
    primGen->SetTarget(0.0, 0.175);    // (targetZ, targetDz)
    primGen->SmearVertexZ(kTRUE);
    primGen->SmearGausVertexXY(kTRUE);
    gRandom->SetSeed(0);

    switch (generatorName) {
        // ------- UrQMD Generator
    case URQMD: {
        if (!BmnFunctionSet::CheckFileExist(inFile, 1)) exit(-1);

        MpdUrqmdGenerator* urqmdGen = new MpdUrqmdGenerator(inFile);
        //urqmdGen->SetEventPlane(0., 360.);
        primGen->AddGenerator(urqmdGen);
        if (nStartEvent > 0) urqmdGen->SkipEvents(nStartEvent);

        // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
        if (nEvents == 0)
            nEvents = MpdGetNumEvents::GetNumURQMDEvents(inFile.Data()) - nStartEvent;
        break;
    }

              // ------- Particle Generator
    case PART: {
        // FairParticleGenerator generates a single particle type (PDG, mult, [GeV] px, py, pz, [cm] vx, vy, vz)
        FairParticleGenerator* partGen = new FairParticleGenerator(211, 10, 1, 0, 3, 1, 0, 0);
        primGen->AddGenerator(partGen);
        break;
    }

             // ------- Ion Generator
    case ION: {
        // Start beam from a far point to check mom. reconstruction procedure (Z, A, q, mult, [GeV] px, py, pz, [cm] vx, vy, vz)
        FairIonGenerator* fIongen = new FairIonGenerator(54, 131, 54, 1, 0., 0., 4.8, 0., 0., 0.); //Xe
        primGen->AddGenerator(fIongen);
        break;
    }

            // ------- Box Generator
    case BOX: {
        gRandom->SetSeed(0);
        FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 1); //(PDG code, multiplicity)
        boxGen->SetPRange(0.2, 5.0);      // GeV/c, setPRange vs setPtRange
        boxGen->SetPhiRange(0, 360);    // Azimuth angle range [degree]
        boxGen->SetThetaRange(0, 40.0);  // Polar angle in lab system range [degree]
        primGen->AddGenerator(boxGen);
        break;
    }

            // ------- HSD/PHSD Generator
    case HSD: {
        if (!BmnFunctionSet::CheckFileExist(inFile, 1)) exit(-1);

        MpdPHSDGenerator* hsdGen = new MpdPHSDGenerator(inFile.Data());
        //hsdGen->SetPsiRP(0.); // set fixed Reaction Plane angle instead of random
        primGen->AddGenerator(hsdGen);
        if (nStartEvent > 0) hsdGen->SkipEvents(nStartEvent);

        // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
        if (nEvents == 0)
            nEvents = MpdGetNumEvents::GetNumPHSDEvents(inFile.Data()) - nStartEvent;
        break;
    }

            // ------- LAQGSM/DCM-QGSM Generator
    case QGSM:
    case DCMQGSM: {

        if (!BmnFunctionSet::CheckFileExist(inFile, 1)) exit(-1);

        MpdLAQGSMGenerator* guGen = new MpdLAQGSMGenerator(inFile.Data(), kFALSE);
        primGen->AddGenerator(guGen);
        if (nStartEvent > 0) guGen->SkipEvents(nStartEvent);

        // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
        if (nEvents == 0)
            nEvents = MpdGetNumEvents::GetNumQGSMEvents(inFile.Data()) - nStartEvent;
        break;
    }
    case DCMSMM: {

        if (!BmnFunctionSet::CheckFileExist(inFile, 1)) exit(-1);

        MpdDCMSMMGenerator* smmGen = new MpdDCMSMMGenerator(inFile.Data());
        primGen->AddGenerator(smmGen);
        if (nStartEvent > 0) smmGen->SkipEvents(nStartEvent);

        // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
        if (nEvents == 0)
            nEvents = MpdGetNumEvents::GetNumDCMSMMEvents(inFile.Data()) - nStartEvent;
        break;
    }

    default: { cout << "ERROR: Generator name was not pre-defined: " << generatorName << endl; exit(-3); }
    }// end of switch (generatorName)

    // if directory for the output file does not exist, then create
    if (BmnFunctionSet::CreateDirectoryTree(outFile, 1) < 0) exit(-2);
    fRun->SetSink(new FairRootFileSink(outFile.Data()));
    fRun->SetIsMT(false);

    // -----   Create magnetic field   ----------------------------------------
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v5_ascii_Extrap.root");
    Double_t fieldScale = 1800. / 900.;
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager


    // -----   Digitizers: converting MC points to detector digits   -----------

    // SiBT-Digitizer
    BmnSiBTConfiguration::SiBT_CONFIG sibt_config = BmnSiBTConfiguration::Run8;
    BmnSiBTDigitizer* sibtDigit = new BmnSiBTDigitizer();
    sibtDigit->SetCurrentConfig(sibt_config);
    fRun->AddTask(sibtDigit);

    // BD-Digitizer
    BmnBdDigitizer* bdDigit = new BmnBdDigitizer();
    fRun->AddTask(bdDigit);

    // SiMD-Digitizer
    BmnSiMDDigitizer* simdDigit = new BmnSiMDDigitizer();
    fRun->AddTask(simdDigit);

    // SI-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::Run8_3stations;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetUseRealEffects(useRealEffects);
    fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::Run8;
    if (useRealEffects)
        BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_8T);
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetUseRealEffects(useRealEffects);
    fRun->AddTask(gemDigit);

    // CSC-Digitizer
    BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::Run8;
    BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
    cscDigit->SetCurrentConfig(csc_config);
    fRun->AddTask(cscDigit);

    // // ZDC-Digitizer (for period < 8)
    // BmnZdcDigitizer* zdcDigit = new BmnZdcDigitizer();
    // zdcDigit->SetScale(39e3);
    // zdcDigit->SetThreshold(500.);
    // fRun->AddTask(zdcDigit);

    //FHCal-Digitizer (for period >= 8)
    BmnFHCalDigitizer* fhcalDigit = new BmnFHCalDigitizer();
    fhcalDigit->SetGeV2MIP(0.005); // 5 MeV from 1 MIP in FHCal section
    fhcalDigit->SetMIP2Pix(15); // 15 pix for 1 MIP in SiPM
    fhcalDigit->SetScale(28.2e3); // scale to convert visible to full energy
    fhcalDigit->SetMIPNoise(0.2); // electronic noise in MIP scale
    fhcalDigit->SetThreshold(0.5*0.005*28.2e3); //0.5 MIP noise cut
    fRun->AddTask(fhcalDigit);

    //ScWall-Digitizer
    BmnScWallDigitizer * fScWallDigit = new BmnScWallDigitizer();
    fScWallDigit->SetScale(1.);
    fScWallDigit->SetThreshold(0.);
    fScWallDigit->SetGeV2MIP(0.0021); // 2.1 MeV from 1 MIP in 1cm plastic
    fScWallDigit->SetMIP2Pix(15); // 15 pix for 1 MIP in SiPM
    fScWallDigit->SetMIPNoise(0.1); // electronic noise in MIP scale
    fRun->AddTask(fScWallDigit);

    //Hodo-Digitizer
    BmnHodoDigitizer * fHodoDigit = new BmnHodoDigitizer();
    fHodoDigit->SetScale(1.);
    fHodoDigit->SetThreshold(0.);
    fHodoDigit->SetGeV2MIP(0.00083); // 0.83 MeV from 1 MIP in 4mm plastic
    fHodoDigit->SetMIP2Pix(15); // 15 pix for 1 MIP in SiPM
    fHodoDigit->SetMIPNoise(0.1); // electronic noise in MIP scale
    fRun->AddTask(fHodoDigit);


    // // ECAL-Digitizer (for period < 8)
    // BmnEcalDigitizer* ecalDigit = new BmnEcalDigitizer();
    // fRun->AddTask(ecalDigit);

    fRun->Init();
    magField->Print("");

    // Trajectories Visualization (TGeoManager only)
    FairTrajFilter* trajFilter = FairTrajFilter::Instance();
    // Set cuts for storing the trajectories
    trajFilter->SetStepSizeCut(0.01); // [cm]
    trajFilter->SetVertexCut(-200., -200., -1000., 200., 200., 1100.); // (vxMin, vyMin, vzMin, vxMax, vyMax, vzMax)
    trajFilter->SetMomentumCutP(30e-3); // p_lab > 30 MeV
    trajFilter->SetEnergyCut(0., 10.);  // 0 < Etot < 10 GeV
    trajFilter->SetStorePrimaries(kTRUE);
    trajFilter->SetStoreSecondaries(kTRUE); //kFALSE

    // Fill the Parameter containers for this run
    FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
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

    if ((generatorName == QGSM) || (generatorName == DCMQGSM)) {
        TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(inFile.Data()), ".g", (fRun->GetName())[6], ".pdg_table.dat");
        (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
    }

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems

    delete fRun;
    delete magField;
}

int main(int argc, char** arg)
{
   run_sim_bmn();
}
