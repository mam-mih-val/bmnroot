#include "BmnOnlineProcessor.h"
#include "BmnCSCHitMaker.h"
#include "BmnDchHitProducer.h"
#include "BmnEventHeader.h"
#include "BmnFHCalReconstructor.h"
#include "BmnGemStripHitMaker.h"
#include "BmnNewFieldMap.h"
#include "BmnSiBTHitMaker.h"
#include "BmnSiliconHitMaker.h"
#include "BmnTof1HitProducer.h"
#include "BmnTofHitProducer.h"
#include "UniRun.h"

#include <RootSerializer.h>
#include <TFile.h>
#include <TIterator.h>
#include <TKey.h>
#include <TList.h>
#include <TRandom2.h>
#include <TString.h>
#include <TTree.h>

#include <string>

BmnOnlineProcessor::BmnOnlineProcessor()
: fair::mq::Device(),
  fVerbose(0),
  fInputChannelName("bmn-online-data-1"),
  fOutputChannelName("bmn-online-data-2"),
  fCurrentEvent(0),
  fTasks(0)
{}

BmnOnlineProcessor::~BmnOnlineProcessor()
{}

void BmnOnlineProcessor::Init() {
    fVerbose = fConfig->GetValue<Int_t>("verbose");
    fInputChannelName = fConfig->GetValue<std::string>("input-channel");
    fOutputChannelName = fConfig->GetValue<std::string>("output-channel");

    fTasks.clear();

    OnData(fInputChannelName, &BmnOnlineProcessor::HandleData);
}

void BmnOnlineProcessor::Reset() { fTasks.clear(); }

void BmnOnlineProcessor::InitTask() {
    if (fVerbose > 3) LOG(info) << "BmnOnlineProcessor::InitTask(): Starting...";

    if (fVerbose > 3) LOG(info) << "BmnOnlineProcessor::InitTask(): Complete!";
}

void BmnOnlineProcessor::ResetTask() {}

Bool_t BmnOnlineProcessor::SetUpExRun(std::unique_ptr<TTree> &dataTree) {
    Int_t runPeriod = 7, runNumber = -1;
    Double_t fieldScale = 0.;

    BmnEventHeader *eventHeader = nullptr;
    if (dataTree->SetBranchAddress("BmnEventHeader.", &eventHeader)) {
        LOG(fatal) << "BmnOnlineProcessor::SetUpExRun(): branch BmnEventHeader. not found!";
        return kFALSE;
    }

    dataTree->GetEntry(0);
    runNumber = eventHeader->GetRunId();
    if (runNumber > 2000)
        runPeriod = 7;
    else if (runNumber > 1100)
        runPeriod = 6;
    else if (runNumber > 400)
        runPeriod = 5;
    else
        runPeriod = 4;

    dataTree->ResetBranchAddresses();
    delete eventHeader;

    auto random = std::make_unique<TRandom2>(0);
    TString geoFileName = Form("current_geo_file_%d.root", UInt_t(random->Integer(UINT32_MAX)));
    if (UniRun::ReadGeometryFile(runPeriod, runNumber, const_cast<char *>(geoFileName.Data()))) {
        LOG(error) << "ERROR: could not read geometry file from the database";
        return kFALSE;
    }

    auto geoFile = std::make_unique<TFile>(geoFileName, "READ");
    if (!geoFile->IsOpen()) {
        LOG(error) << "ERROR: could not open ROOT file with geometry: " + geoFileName;
        return kFALSE;
    }
    TIter next(geoFile->GetListOfKeys());
    auto key = std::unique_ptr<TKey>(static_cast<TKey *>(next()));
    TString className(key->GetClassName());
    if (className.BeginsWith("TGeoManager"))
        key->ReadObj();
    else {
        LOG(error) << "ERROR: TGeoManager is not top element in geometry file " + geoFileName;
        return kFALSE;
    }

    key.release();
    geoFile->Close();
    remove(geoFileName.Data());

    auto currentRun = std::unique_ptr<UniRun>(UniRun::GetRun(runPeriod, runNumber));
    if (!currentRun) return kFALSE;
    auto fieldVoltage = std::unique_ptr<Double_t>(currentRun->GetFieldVoltage());
    if (!fieldVoltage) {
        LOG(error) << "ERROR: no field voltage was found for run " << runPeriod << ":" << runNumber;
        return kFALSE;
    }
    Double_t mapCurrent = 55.87;
    if (*fieldVoltage < 10) {
        fieldScale = 0;
    } else
        fieldScale = (*fieldVoltage) / mapCurrent;

    auto field = std::make_unique<BmnNewFieldMap>("field_sp41v5_ascii_Extrap.root");
    field->SetScale(fieldScale);
    fField = std::unique_ptr<FairField>(static_cast<FairField *>(field.release()));
    fField->Init();

    TString targ;
    Bool_t isTarget = kTRUE;
    if (!currentRun->GetTargetParticle()) {
        targ = "-";
        isTarget = kFALSE;
    } else {
        targ = (currentRun->GetTargetParticle())[0];
        isTarget = kTRUE;
    }

    TString beam = currentRun->GetBeamParticle();

    std::cout << "\n|||||||||||||||| EXPERIMENTAL RUN SUMMARY ||||||||||||||||" << std::endl;
    std::cout << "||\t\t\t\t\t\t\t||" << std::endl;
    std::cout << "||\t\tPeriod:\t\t" << runPeriod << "\t\t\t||" << std::endl;
    std::cout << "||\t\tNumber:\t\t" << runNumber << "\t\t\t||" << std::endl;
    std::cout << "||\t\tBeam:\t\t" << beam << "\t\t\t||" << std::endl;
    std::cout << "||\t\tTarget:\t\t" << targ << "\t\t\t||" << std::endl;
    std::cout << "||\t\tField scale:\t" << setprecision(4) << fieldScale << "\t\t\t||" << std::endl;
    std::cout << "||\t\t\t\t\t\t\t||" << std::endl;
    std::cout << "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n" << std::endl;

    RegisterTasks(runPeriod, runNumber);

    return kTRUE;
}

void BmnOnlineProcessor::RegisterTasks(Int_t runPeriod, Int_t runNumber) {
    fTasks.clear();

    // Hits in front of the target
    if (runPeriod >= 8) {
        // TODO: Check for errors!!!
        auto sibtHM = std::make_unique<BmnSiBTHitMaker>(runPeriod, runNumber, kTRUE);
        sibtHM->SetHitMatching(kFALSE);
        fTasks.push_back(std::move(sibtHM));
    }

    // Silicon hit finder
    auto siliconHM = std::make_unique<BmnSiliconHitMaker>(runPeriod, runNumber, kTRUE);
    siliconHM->SetHitMatching(kFALSE);
    fTasks.push_back(std::move(siliconHM));

    // GEM hit finder
    auto gemHM = std::make_unique<BmnGemStripHitMaker>(runPeriod, runNumber, kTRUE);
    gemHM->SetHitMatching(kFALSE);
    fTasks.push_back(std::move(gemHM));

    // CSC hit finder
    auto cscHM = std::make_unique<BmnCSCHitMaker>(runPeriod, runNumber, kTRUE);
    cscHM->SetHitMatching(kFALSE);
    fTasks.push_back(std::move(cscHM));

    // TOF1 hit finder
    auto tof1HP = std::make_unique<BmnTof1HitProducer>("TOF1", kFALSE, fVerbose, kFALSE);
    tof1HP->SetPeriodRun(runPeriod, runNumber);
    fTasks.push_back(std::move(tof1HP));

    // TOF2 hit finder
    // TODO: Is not working! Segmentation violation!
    /*auto tof2HP = std::make_unique<BmnTofHitProducer>("TOF", "TOF700_geometry_run7.txt", kFALSE, fVerbose, kFALSE);
    tof2HP->SetTimeResolution(0.115);
    tof2HP->SetProtonTimeCorrectionFile("bmn_run9687_digi_calibration.root");
    tof2HP->SetMCTimeFile("TOF700_MC_argon_qgsm_time_run7.txt");
    tof2HP->SetMainStripSelection(0);   // 0 - minimal time, 1 - maximal amplitude
    tof2HP->SetSelectXYCalibration(2);  // 0 - Petukhov, 1 - Panin SRC, 2 - Petukhov Argon (default)
    tof2HP->SetTimeMin(-2.f);           // minimal digit time
    tof2HP->SetTimeMax(+39.f);          // Maximal digit time
    tof2HP->SetDiffTimeMaxSmall(1.2f);  // Abs maximal difference for small chambers
    tof2HP->SetDiffTimeMaxBig(3.5f);    // Abs maximal difference for big chambers
    fTasks.push_back(std::move(tof2HP));*/

    // FHCAL
    if (runPeriod >= 8) {
        // TODO: Check for errors!!!
        auto fhcalReco = std::make_unique<BmnFHCalReconstructor>("FHCAL_map_dry_run_2022.txt", kTRUE);
        fTasks.push_back(std::move(fhcalReco));
    }

    for (auto &&task : fTasks) {
        task->SetVerbose(fVerbose);
        task->SetField(fField);
        if (task->OnlineInit() != kSUCCESS) task->SetActive(kFALSE);
    };
}

Bool_t BmnOnlineProcessor::HandleData(fair::mq::MessagePtr &inputMessage, Int_t) {
    std::unique_ptr<TTree> eventData(nullptr);
    RootSerializer().Deserialize(*inputMessage, eventData);

    auto resultData = std::make_unique<TTree>();

    if (fCurrentEvent == 0)
        if (!SetUpExRun(eventData)) return kFALSE;

    LOG(info) << "BmnOnlineProcessor::HandleData() Event #" << fCurrentEvent;

    for (auto &&task : fTasks) task->OnlineRead(eventData, resultData);

    eventData->GetEntry(0);

    for (auto &&task : fTasks) {
        task->Exec("0");
        task->OnlineWrite(resultData);
    }

    auto message = NewMessage();
    RootSerializer().Serialize(*message, resultData);

    if (Send(message, fOutputChannelName) < 0) {
        LOG(error) << "BmnOnlineProcessor::HandleData() Sending message failed";
        return kFALSE;
    }

    fCurrentEvent++;

    return kTRUE;
}
