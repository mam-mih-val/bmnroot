R__ADD_INCLUDE_PATH($VMCWORKDIR)
R__ADD_INCLUDE_PATH($SIMPATH / include / fairmq)

#include <cstdint>
#include <root/TDirectory.h>

#include "macro/run/geometry_run/geometry_run7.C"


#define GEANT3  // Choose: GEANT3 GEANT4
// enumeration of generator names corresponding input files
enum enumGenerators {
    URQMD, QGSM, HSD, BOX, PART, ION, DQGSM
};

// inFile - input file with generator data, if needed
// outFile - output file with MC data, default: evetest.root
// nStartEvent - start event in the input generator file to begin transporting, default: 0
// nEvents - number of events to transport
// generatorName - generator name for the input file (enumeration above)
// useRealEffects - whether we use realistic effects at simulation (Lorentz, misalignment)

void run_sim_filter(
        TString expFile = "bmn_run4649_digi.root",
        TString inFile = "$VMCWORKDIR/macro/run/Filtered_ArAl_3.2AGeV.root",
//        TString inFile = "/ncx/eos/nica/bmn/sim/gen/DCMQGSM/ArAl_3.2AGeV_mb/ArAl_3.2AGeV_mb_2.r12",
        TString outFile = "$VMCWORKDIR/macro/run/evetest.root", Int_t nStartEvent = 0, Int_t nEvents = 10,
        enumGenerators generatorName = QGSM, Bool_t useRealEffects = kTRUE, Int_t selPDG = 3122, Int_t fMinHitsR = 4) {
    TStopwatch timer;
    timer.Start();
    gDebug = 0;
    Int_t fVerbose = 0;
    // -----   Create simulation run   ----------------------------------------
    BmnFairRunSim* fRun = new BmnFairRunSim();

    // Choose the Geant Navigation System
#ifdef GEANT3
    fRun->SetName("TGeant3");
#else
    fRun->SetName("TGeant4");
#endif

    geometry(fRun); // load BM@N geometry

    // Use the experiment specific MC Event header instead of the default one
    // This one stores additional information about the reaction plane
    //MpdMCEventHeader* mcHeader = new MpdMCEventHeader();
    //fRun->SetMCEventHeader(mcHeader);

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // Smearing of beam interaction point, if needed, and primary vertex position
    // DO NOT do it in corresponding gen. sections to avoid incorrect summation!!!
    primGen->SetBeam(0.0, 0.0, 0.0, 0.0);
    primGen->SetTarget(0.0, 0.0);
    primGen->SmearVertexZ(kFALSE);
    primGen->SmearVertexXY(kFALSE);

    BmnPVGetter* pvg = new BmnPVGetter(expFile, nEvents);
    pvg->SetVerbose(fVerbose);
    pvg->SetGenerator(primGen);

    switch (generatorName) {
            // ------- UrQMD Generator
        case URQMD:
        {
            if (!BmnFunctionSet::CheckFileExist(inFile)) return;

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
        case PART:
        {
            FairParticleGenerator* partGen =
                    new FairParticleGenerator(211, 10, 1, 0, 3, 1, 0, 0);
            primGen->AddGenerator(partGen);
            break;
        }

            // ------- Ion Generator
        case ION:
        {
            // Start beam from a far point to check mom. reconstruction procedure
            FairIonGenerator* fIongen = new FairIonGenerator(6, 12, 6, 1, 0., 0., 4.4, 0., 0., -647.);
            primGen->AddGenerator(fIongen);
            break;
        }

            // ------- Box Generator
        case BOX:
        {
            gRandom->SetSeed(0);
            FairBoxGenerator* boxGen = new FairBoxGenerator(3122, 1); // 13 = muon; 1 = multipl. // 2212 = proton, 3122 - Lambda
            boxGen->SetPRange(2.5, 3.); // GeV/c, setPRange vs setPtRange
            boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
            boxGen->SetThetaRange(5, 15); // Polar angle in lab system range [degree]
            primGen->AddGenerator(boxGen);
            //            if (nEvents == 0)
            //                nEvents = pvg->GetNFileEvents();
            break;
        }

            // ------- HSD/PHSD Generator
        case HSD:
        {
            if (!BmnFunctionSet::CheckFileExist(inFile)) return;

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
        case DQGSM:
        {
//
//            Int_t lastSlash = inFile.Last('/');
//            TString simDir = (lastSlash == TString::kNPOS) ? "" : TString(inFile(0, lastSlash + 1));
//            vector<TString> fileVec = BmnRecoTools::GetFileVecFromDir(simDir);
            MpdLAQGSMGeneratorExt* guGen = new MpdLAQGSMGeneratorExt({inFile}, kFALSE, 0, 0, selPDG, kFALSE);
            primGen->AddGenerator(guGen);
            break;
        }

        default:
        {
            cout << "ERROR: Generator name was not pre-defined: " << generatorName << endl;
            return;
        }
    }// end of switch (generatorName)

    pvg->SetRunSimInst(fRun);
    fRun->AddTask(pvg);

    //    FairMQProxy * sink = new FairMQProxy();
    //    fRun->SetSink((FairSink*)sink);
    fRun->SetSink(new FairRootFileSink(outFile.Data()));
    fRun->SetIsMT(false);

    // -----   Create magnetic field   ----------------------------------------
    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    Double_t fieldScale = 1200. / 900.;
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(kFALSE); // radiation length manager

    // recunstructability checker
    BmnCheckMCTask* checker = new BmnCheckMCTask(nEvents, fMinHitsR);
    checker->SetVerbose(fVerbose);
    checker->SetRunSimInst(fRun);
    fRun->AddTask(checker);

    // SI-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSpring2018;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetOnlyPrimary(kFALSE);
    siliconDigit->SetStripMatching(kFALSE);
    siliconDigit->SetUseRealEffects(useRealEffects);
    siliconDigit->SetVerbose(fVerbose);
    fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSpring2018;
    if (useRealEffects)
        BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_6T);
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetOnlyPrimary(kFALSE);
    gemDigit->SetStripMatching(kFALSE);
    gemDigit->SetUseRealEffects(useRealEffects);
    gemDigit->SetVerbose(fVerbose);
    fRun->AddTask(gemDigit);

    // CSC-Digitizer
    BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::RunSpring2018;
    BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
    cscDigit->SetCurrentConfig(csc_config);
    cscDigit->SetOnlyPrimary(kFALSE);
    cscDigit->SetStripMatching(kFALSE);
    cscDigit->SetVerbose(fVerbose);
    fRun->AddTask(cscDigit);

    fRun->Init();
    magField->Print();

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
    Int_t doNEvents = (generatorName == QGSM || generatorName == BOX) ? INT32_MAX : nEvents;
    fRun->Run(doNEvents);
    //gGeoManager->CheckOverlaps(0.0001);
    //gGeoManager->PrintOverlaps();

    //fRun->CreateGeometryFile("full_geometry.root");  // save the full setup geometry to the additional file

    if ((generatorName == QGSM) || (generatorName == DQGSM)) {
        TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(inFile.Data()), ".g", (fRun->GetName())[6], ".pdg_table.dat");
        (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
    }

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout << "Macro finished successfully." << endl; // marker of successfully execution for software testing systems
}
