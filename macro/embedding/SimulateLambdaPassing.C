//#include <Rtypes.h>
#include <FairPrimaryGenerator.h>

R__ADD_INCLUDE_PATH($VMCWORKDIR)
#include "macro/run/geometry_run/geometry_run7.C"

void SimulateLambdaPassing(
        Int_t nEvents = 1,
        Double_t fieldScale = 2.,
        Double_t P = 1., Double_t eta = 2., Double_t phi = 180.,
        Double_t X = 0.5, Double_t Y = -4.6, Double_t Z = -2.3,
        TString outFile = "test.root") {
    FairRunSim* fRun = new FairRunSim();
    fRun->SetName("TGeant3");

    geometry(fRun);

    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    fRun->SetGenerator(primGen);

    primGen->SetBeam(X, Y, 0.0, 0.0);
    primGen->SetTarget(Z, 0.0);
    primGen->SmearVertexZ(kFALSE);
    primGen->SmearVertexXY(kFALSE);

    gRandom->SetSeed(0);
    FairBoxGenerator* boxGen1 = new FairBoxGenerator(3122, 1);
    boxGen1->SetPRange(P, P); // GeV/c, setPRange vs setPtRange
    boxGen1->SetPhiRange(phi, phi); // Azimuth angle range [degree]
    boxGen1->SetEtaRange(eta, eta); // Polar angle in lab system range [degree]
    primGen->AddGenerator(boxGen1);

    fRun->SetSink(new FairRootFileSink(outFile));
    fRun->SetIsMT(false);

    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    magField->SetScale(fieldScale);
    fRun->SetField(magField);

    // SILICON-Digitizer
    BmnSiliconConfiguration::SILICON_CONFIG si_config = BmnSiliconConfiguration::RunSpring2018;
    BmnSiliconDigitizer* siliconDigit = new BmnSiliconDigitizer();
    siliconDigit->SetCurrentConfig(si_config);
    siliconDigit->SetOnlyPrimary(kFALSE);
    siliconDigit->SetUseRealEffects(kTRUE);
    fRun->AddTask(siliconDigit);

    // GEM-Digitizer
    BmnGemStripConfiguration::GEM_CONFIG gem_config = BmnGemStripConfiguration::RunSpring2018;
    BmnGemStripDigitizer* gemDigit = new BmnGemStripDigitizer();
    BmnGemStripMedium::GetInstance().SetCurrentConfiguration(BmnGemStripMediumConfiguration::ARC4H10_80_20_E_1720_2240_3230_3730_B_0_6T);
    gemDigit->SetCurrentConfig(gem_config);
    gemDigit->SetOnlyPrimary(kFALSE);
    gemDigit->SetUseRealEffects(kTRUE);
    gemDigit->SetStripMatching(kTRUE);
    fRun->AddTask(gemDigit);
    
    // CSC-Digitizer 
    BmnCSCConfiguration::CSC_CONFIG csc_config = BmnCSCConfiguration::RunSpring2018;
    BmnCSCDigitizer* cscDigit = new BmnCSCDigitizer();
    cscDigit->SetCurrentConfig(csc_config);
    cscDigit->SetOnlyPrimary(kFALSE);
    cscDigit->SetStripMatching(kTRUE);
    fRun->AddTask(cscDigit);

    fRun->Init();

    fRun->Run(nEvents);

    delete fRun;
}