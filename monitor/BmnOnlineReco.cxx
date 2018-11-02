#include <thread>
#include <time.h>
#include <chrono>
#include <ctime>
#include <fcntl.h>


#include "BmnOnlineReco.h"

BmnOnlineReco::BmnOnlineReco() {
    keepWorking = kTRUE;
    fHistOut = NULL;
    fHistOutTemp = NULL;
    fServer = NULL;
    fRawDecoSocket = NULL;
    fRunID = 0;
    fEvents = 0;
    fState = kBMNRECON;
    itersToUpdate = 1000;
    runPub = new TList();
    runPub->SetName("CurRun");
    fDigiArrays = NULL;
    _ctx = NULL;
    CurRun = new BmnRunInfo();
    runPub->Add((TObject*) CurRun);

    fDecoSource = new BmnDecoSource();

    fPeriodID = 7;
    fRunID = 4649;
    Bool_t isField = kTRUE; // flag for tracking (to use mag.field or not)
    Bool_t isTarget = kTRUE; //kFALSE; // flag for tracking (run with target or not)
    Bool_t isExp = kFALSE; // flag for hit finder (to create digits or take them from data-file)
    // Verbosity level (0=quiet, 1=event-level, 2=track-level, 3=debug)
    Int_t iVerbose = 2;
    Double_t fieldScale = 0.;
    TString inputFileName = TString(getenv("VMCWORKDIR")) + "/macro/run/evetest.root";
    TString alignCorrFileName = "default";
    TString steerGemTrackingFile = "gemTrackingSteer.dat";
    fRunAna = new FairRunAna();
    fRunAna->SetOutputFile(TString(getenv("VMCWORKDIR")) + "/macro/run/bmndst.root");
    fRunAna->SetGenerateRunInfo(false); // set magnet field with factor corresponding to the given run
    // Get Geometry
    TString geoFileName = "current_geo_file.root";
    Int_t res_code = UniDbRun::ReadGeometryFile(fPeriodID, fRunID, (char*) geoFileName.Data());
    if (res_code != 0) {
        cout << "Geometry file can't be read from the database" << endl;
        exit(-2);
    }
    // get gGeoManager from ROOT file (if required)
    TFile* geoFile = new TFile(geoFileName, "READ");
    if (!geoFile->IsOpen()) {
        cout << "Error: could not open ROOT file with geometry: " + geoFileName << endl;
        exit(-3);
    }
    TList* keyList = geoFile->GetListOfKeys();
    TIter next(keyList);
    TKey* key = (TKey*) next();
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        cout << "Error: TGeoManager isn't top element in geometry file " + geoFileName << endl;
        exit(-4);
    }
    UniDbRun* pCurrentRun = UniDbRun::GetRun(fPeriodID, fRunID);
    if (pCurrentRun == 0)
        exit(-5);
    Double_t* field_voltage = pCurrentRun->GetFieldVoltage();
    if (field_voltage == NULL) {
        cout << "Error: no field voltage was found for run " << fPeriodID << ":" << fRunID << endl;
        exit(-6);
    }
    Double_t map_current = 55.87;
    if (*field_voltage < 10) {
        fieldScale = 0;
        isField = kFALSE;
    } else
        fieldScale = (*field_voltage) / map_current;

    BmnFieldMap* magField = new BmnNewFieldMap("field_sp41v4_ascii_Extrap.root");
    magField->SetScale(fieldScale);
    magField->Init();
    isExp = kTRUE;
    TString targ;
    if (pCurrentRun->GetTargetParticle() == NULL) {
        targ = "-";
        isTarget = kFALSE;
    } else {
        targ = (pCurrentRun->GetTargetParticle())[0];
        isTarget = kTRUE;
    }
    TString beam = pCurrentRun->GetBeamParticle();
    cout << "\n\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << endl;
    cout << "||\t\t\t\t\t\t\t||" << endl;
    cout << "||\t\tPeriod:\t\t" << fPeriodID << "\t\t\t||" << endl;
    cout << "||\t\tNumber:\t\t" << fRunID << "\t\t\t||" << endl;
    cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << endl;
    cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << endl;
    cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << endl;
    cout << "||\t\t\t\t\t\t\t||" << endl;
    cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n" << endl;
    fRunAna->SetField(magField);
    fRunAna->SetSource(fDecoSource);
    TList* parFileNameList = new TList();
    TObjString stsDigiFile = TObjString(Form("%s%s", getenv("VMCWORKDIR"), "/parameters/sts_v15a_gem.digi.par"));
    parFileNameList->Add(&stsDigiFile);

    TObjString tofDigiFile = TObjString(Form("%s%s", getenv("VMCWORKDIR"), "/parameters/tof_standard.geom.par"));
    parFileNameList->Add(&tofDigiFile);

    //    if (iVerbose == 0) { // print only progress bar in terminal in quiet mode
    //        BmnCounter* cntr = new BmnCounter(nEvents);
    //        fRunAna->AddTask(cntr);
    //    }
    // ====================================================================== //
    // ===                           Check Triggers                       === //
    // ====================================================================== //
    //        BmnTriggersCheck* triggs = new BmnTriggersCheck(isExp, fPeriodID, fRunID);
    //        fRunAna->AddTask(triggs);
    // ====================================================================== //
    // ===                           MWPC hit finder                      === //
    // ====================================================================== //
    //    BmnMwpcHitFinder* mwpcHM = new BmnMwpcHitFinder(isExp);
    //    mwpcHM->SetUseDigitsInTimeBin(kFALSE);
    //    fRunAna->AddTask(mwpcHM);
    //    // ====================================================================== //
    //    // ===                         Silicon hit finder                     === //
    //    // ====================================================================== //
    //    BmnSiliconConfiguration::SILICON_CONFIG si_config;
    //    switch (run_period) {
    //        case 6: //BM@N RUN-6
    //            si_config = BmnSiliconConfiguration::RunSpring2017;
    //            break;
    //        case 7: //BM@N RUN-7
    //            si_config = BmnSiliconConfiguration::RunSpring2018;
    //            break;
    //        default:
    //            si_config = BmnSiliconConfiguration::RunSpring2017;
    //            //si_config = BmnSiliconConfiguration::RunSpring2018;
    //    }
    //    BmnSiliconHitMaker* siliconHM = new BmnSiliconHitMaker(isExp);
    //    siliconHM->SetCurrentConfig(si_config);
    //    fRunAna->AddTask(siliconHM);
    // ====================================================================== //
    // ===                         GEM hit finder                         === //
    // ====================================================================== //
    BmnGemStripConfiguration::GEM_CONFIG gem_config;
    switch (fPeriodID) {
        case 5: //BM@N RUN-5
            gem_config = BmnGemStripConfiguration::RunWinter2016;
            break;
        case 6: //BM@N RUN-6
            gem_config = BmnGemStripConfiguration::RunSpring2017;
            break;
        case 7: //BM@N RUN-7
            gem_config = BmnGemStripConfiguration::RunSpring2018;
            break;
        default:
            gem_config = BmnGemStripConfiguration::RunSpring2017;
            //gem_config = BmnGemStripConfiguration::RunSpring2018;
    }
    BmnGemStripHitMaker* gemHM = new BmnGemStripHitMaker(fPeriodID, fRunID, isExp);
    gemHM->SetCurrentConfig(gem_config);
    gemHM->SetHitMatching(kTRUE);
    fRunAna->AddTask(gemHM);

    // ====================================================================== //
    // ===                           ALIGNMENT (GEM + SI)                 === //
    // ====================================================================== //

    //    // ====================================================================== //
    //    // ===                           TOF1 hit finder                      === //
    //    // ====================================================================== //
    //    BmnTof1HitProducer* tof1HP = new BmnTof1HitProducer("TOF1", !isExp, iVerbose, kTRUE);
    //    tof1HP->SetPeriod(run_period);
    //    //tof1HP->SetOnlyPrimary(kTRUE);
    //    fRunAna->AddTask(tof1HP);
    //    // ====================================================================== //
    //    // ===                           TOF2 hit finder                      === //
    //    // ====================================================================== //
    //    BmnTofHitProducer* tof2HP = new BmnTofHitProducer("TOF", "TOF700_geometry_run6.txt", !isExp, iVerbose, kTRUE);
    //    //    fRunAna->AddTask(tof2HP);
    //    // ====================================================================== //
    //    // ===                           Tracking (MWPC)                      === //
    //    // ====================================================================== //
    //    BmnMwpcTrackFinder* mwpcTF = new BmnMwpcTrackFinder(isExp);
    //    //    fRunAna->AddTask(mwpcTF);
    // ====================================================================== //
    // ===                           Tracking (GEM)                       === //
    // ====================================================================== //

    //    BmnGemTracking* gemTF = new BmnGemTracking(fPeriodID, isField, isTarget, steerGemTrackingFile);
    //    if (!isExp) gemTF->SetRoughVertex(TVector3(0.0, 0.0, 0.0)); //for MC case use correct vertex
    //    fRunAna->AddTask(gemTF);

    //    // ====================================================================== //
    //    // ===                           Tracking (DCH)                       === //
    //    // ====================================================================== //
    //    BmnDchTrackFinder* dchTF = new BmnDchTrackFinder(isExp);
    //    dchTF->SetTransferFunction("pol_coord00813.txt");
    //    //    fRunAna->AddTask(dchTF);
    // ====================================================================== //
    // ===                          Global Tracking                       === //
    // ====================================================================== //
    //    BmnGlobalTracking* globalTF = new BmnGlobalTracking(isExp);
    //    globalTF->SetField(isField);
    //    fRunAna->AddTask(globalTF);

    // ====================================================================== //
    // ===                     Primary vertex finding                     === //
    // ====================================================================== //
    //    BmnVertexFinder* gemVF = new BmnVertexFinder(fPeriodID, isField);
    //    // gemVF->SetVertexApproximation(TVector3(0., 0., 0.));
    //    fRunAna->AddTask(gemVF);

    // Residual analysis
    //    if (isExp) {
    //        BmnGemResiduals* residAnalGem = new BmnGemResiduals(fPeriodID, fRunID, fieldScale);
    //        // residAnal->SetPrintResToFile("file.txt");
    //        // residAnal->SetUseDistance(kTRUE); // Use distance instead of residuals
    //        fRunAna->AddTask(residAnalGem);
    //        // BmnSiResiduals* residAnalSi = new BmnSiResiduals(run_period, run_number, fieldScale);
    //        // fRunAna->AddTask(residAnalSi);
    //    }
    // -----   Parameter database   --------------------------------------------
    FairRuntimeDb* rtdb = fRunAna->GetRuntimeDb();
    FairParRootFileIo* parIo1 = new FairParRootFileIo();
    FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
    parIo1->open(inputFileName.Data());
    parIo2->open(parFileNameList, "in");
    //    rtdb->setFirstInput(parIo1);
    rtdb->setSecondInput(parIo2);
    rtdb->setOutput(parIo1);
    rtdb->saveOutput();
    // -------------------------------------------------------------------------
    // -----   Initialize and run   --------------------------------------------
    fRunAna->GetMainTask()->SetVerbose(iVerbose);
    fRunAna->Init();
}

BmnOnlineReco::~BmnOnlineReco() {
    DBG("started")
    if (fHistOut != NULL)
        delete fHistOut;
    DBG("delete fHistOut")
    if (fServer) delete fServer;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
    if (CurRun)
        delete CurRun;
    if (runPub)
        delete runPub;
    fDecoSource->Close();
    delete fDecoSource;
    delete fRunAna;
}

void BmnOnlineReco::RecoStream(TString dirname, TString refDir, TString decoAddr, Int_t webPort) {
    DBG("started")
    fRunAna->Run();
}

void BmnOnlineReco::ProcessDigi(Int_t iEv) {
    fEvents++;
    BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
    if (fEvents % itersToUpdate == 0) {
    }
}

void BmnOnlineReco::FinishRun() {
    DBG("started")
    if (fHistOut) {
        printf("fHistOut is gona be written\n");
        printf("fHistOut  Write result = %d\n", fHistOut->Write());
        fHistOut->Close();
        fHistOut = NULL;
    }

    //    if (fRecoTree4Show){
    //        printf("fRecoTree4Show Write result = %d\n", fRecoTree4Show->Write());
    //    }
    //    if (fHistOutTemp) {
    //        printf("fHistOutMem Write result = %d\n", fHistOutTemp->Write());
    //        fHistOutTemp->Close();
    //        printf("fHistOutMem closed\n");
    //        fHistOutTemp = NULL;
    //    }
}

ClassImp(BmnOnlineReco);