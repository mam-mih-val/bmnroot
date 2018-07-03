R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/bmnloadlibs.C"
#include "macro/run/geometry.C"

#define BOX  // Choose generator: URQMD PART ION BOX HSD LAQGSM
#define GEANT3  // Choose: GEANT3 GEANT4

// inFile - input file with generator data, default: dc4mb.r12 for LAQGSM event generator (deuteron - carbon target, mbias, 4 GeV)
// outFile - output file with MC data, default: evetest.root
// nStartEvent - for compatibility, any number
// nEvents - number of events to transport, default: 1
// flag_store_FairRadLenPoint
void run_sim_bmn(TString inFile = "dC.04gev.mbias.100k.urqmd23.f14", TString outFile = "$VMCWORKDIR/macro/run/evetest.root", Int_t nStartEvent = 0, Int_t nEvents = 10,
        Bool_t flag_store_FairRadLenPoint = kFALSE, Bool_t isFieldMap = kTRUE)
{
    TStopwatch timer;
    timer.Start();
    gDebug = 0;

    bmnloadlibs(); // load libraries

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* fRun = new FairRunSim();

    // Choose the Geant Navigation System
#ifdef GEANT3
    fRun->SetName("TGeant3");
#else
    fRun->SetName("TGeant4");
#endif

    geometry(fRun); // load bmn geometry

    // Use the experiment specific MC Event header instead of the default one
    // This one stores additional information about the reaction plane
    //MpdMCEventHeader* mcHeader = new MpdMCEventHeader();
    //fRun->SetMCEventHeader(mcHeader);

    // Create and Set Event Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    // smearing of beam interaction point
    //primGen->SetBeam(0.0,0.0,0.1,0.1);
    //primGen->SetTarget(0.0,24.0);
    //primGen->SmearGausVertexZ(kTRUE);
    //primGen->SmearVertexXY(kTRUE);

#ifdef URQMD
    // ------- Urqmd  Generator
    if (!CheckFileExist(inFile)) return;

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
    FairIonGenerator *fIongen =
            new FairIonGenerator(79, 197, 79, 1, 0., 0., 2., 0., -3.5, -21.7); // FairIonGenerator(6, 12, 6, 1, 0., 0., 4.4, 0., 0., -21.7);
    primGen->AddGenerator(fIongen);

#else
#ifdef BOX
    gRandom->SetSeed(0);
    // ------- Box Generator
    FairBoxGenerator* boxGen = new FairBoxGenerator(2212, 10); // 13 = muon; 1 = multipl.
    boxGen->SetPRange(1., 1.);  // GeV/c, setPRange vs setPtRange
    boxGen->SetPhiRange(0, 360); // Azimuth angle range [degree]
    boxGen->SetThetaRange(10, 15); // Polar angle in lab system range [degree]
    boxGen->SetXYZ(0., -3.5, -21.7); // Approximate position of target (RunSpring2017)
    primGen->AddGenerator(boxGen);

#else
#ifdef HSD
    // ------- HSD/PHSD Generator
    if (!CheckFileExist(inFile)) return;

    MpdPHSDGenerator* hsdGen = new MpdPHSDGenerator(inFile.Data());
    //hsdGen->SetPsiRP(0.); // set fixed Reaction Plane angle instead of random
    primGen->AddGenerator(hsdGen);
    if (nStartEvent > 0) hsdGen->SkipEvents(nStartEvent);

    // if nEvents is equal 0 then all events (start with nStartEvent) of the given file should be processed
    if (nEvents == 0)
        nEvents = MpdGetNumEvents::GetNumPHSDEvents(inFile.Data()) - nStartEvent;

#else
#ifdef LAQGSM
    // ------- LAQGSM Generator
    if (!CheckFileExist(inFile)) return;

    MpdLAQGSMGenerator* guGen = new MpdLAQGSMGenerator(inFile.Data(), kFALSE);
    guGen->SetXYZ(0., -3.5, -21.7); // IP = (0., 0., 0.)
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

    fRun->SetOutputFile(outFile.Data());

    // -----   Create magnetic field   ----------------------------------------
    BmnFieldMap* magField = NULL;
    if (isFieldMap) {
        Double_t fieldScale = 1.33;
        // magField = new BmnNewFieldMap("field_sp41v2_ascii_noExtrap.dat");
        magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
        // Double_t fieldZ = 124.5; // field centre z position
        // magField->SetPosition(0., 0., fieldZ);
        magField->SetScale(fieldScale);
        fRun->SetField(magField);
    } else {
        BmnFieldConst* magField = new BmnFieldConst();
        magField->SetFieldRegion(-300., 300., -300., 300., -300., 300);
        magField->SetField(0., -9. * 0.44, 0.);
        fRun->SetField(magField);
    }

    fRun->SetStoreTraj(kTRUE);
    fRun->SetRadLenRegister(flag_store_FairRadLenPoint); // radiation length manager

    // SI-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSpring2017;
    //BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSpring2018;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetOnlyPrimary(kFALSE);
    fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSpring2017;
    //BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSpring2018;
    BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARCO2_70_30_E_1000_2500_3750_6300_B_0_59T);
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetOnlyPrimary(kFALSE);
    gemDigit->SetStripMatching(kTRUE);
    fRun->AddTask(gemDigit);

    fRun->Init();
    if (isFieldMap) magField->Print();

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

    fRun->CreateGeometryFile("geofile_full.root");  // save the result setup geometry to the additional file

#ifdef LAQGSM
    TString Pdg_table_name = TString::Format("%s%s%c%s", gSystem->BaseName(inFile.Data()), ".g", (fRun->GetName())[6], ".pdg_table.dat");
    (TDatabasePDG::Instance())->WritePDGTable(Pdg_table_name.Data());
#endif

    timer.Stop();
    Double_t rtime = timer.RealTime(), ctime = timer.CpuTime();
    printf("RealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
    cout<<"Macro finished successfully."<<endl; // marker of successfully execution for CDASH
}
