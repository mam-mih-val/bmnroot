#include "BmnOnlineSink.h"

#include <RootSerializer.h>
#include <TFile.h>
#include <TSystem.h>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <fstream>

#include "json.hpp"

using json = nlohmann::json;

const auto configFilePath = gSystem->ExpandPathName("$VMCWORKDIR/config/shm-config.json");

BmnOnlineSink::BmnOnlineSink()
    : fair::mq::Device(), fVerbose(0), fInputChannelName("bmn-online-data-2"), fShmSegment(nullptr)
{}

BmnOnlineSink::~BmnOnlineSink()
{}

void BmnOnlineSink::Init() {
    fVerbose = fConfig->GetValue<Int_t>("verbose");
    fInputChannelName = fConfig->GetValue<std::string>("input-channel");

    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        configFile.close();
        LOG(fatal) << "BmnOnlineSink::Init(): File " << configFilePath << " not found!";
    }

    json shmConfig;
    configFile >> shmConfig;
    configFile.close();

    try {
        fShmName = shmConfig["bmnShmOptions"]["shmName"];
        fWriteSemName = shmConfig["bmnShmOptions"]["writeSemName"];
        fReadSemName = shmConfig["bmnShmOptions"]["readSemName"];
        fMsgName = shmConfig["bmnShmOptions"]["msgName"];
    } catch (const json::type_error &e) {
        LOG(fatal) << "BmnOnlineSink::Init(): File " << configFilePath
                   << " does not contain the required data or the calorimeter type is incorrectly set!";
    }

    OnData(fInputChannelName, &BmnOnlineSink::HandleData);
}

void BmnOnlineSink::Reset() {}

void BmnOnlineSink::InitTask() {
    if (fVerbose > 3) LOG(info) << "BmnOnlineSink::InitTask(): Starting...";

    bIpc::shared_memory_object::remove(fShmName.c_str());

    bIpc::named_semaphore::remove(fWriteSemName.c_str());
    bIpc::named_semaphore::remove(fReadSemName.c_str());

    fShmSegment = std::make_unique<bIpc::managed_shared_memory>(bIpc::create_only, fShmName.c_str(), 1024 * 1024);

    fWriteSem = std::make_unique<bIpc::named_semaphore>(bIpc::create_only, fWriteSemName.c_str(), 1);
    fReadSem = std::make_unique<bIpc::named_semaphore>(bIpc::create_only, fReadSemName.c_str(), 0);

    if (fVerbose > 3) LOG(info) << "BmnOnlineSink::InitTask(): Complete!";
}

void BmnOnlineSink::ResetTask() {
    bIpc::shared_memory_object::remove(fShmName.c_str());

    bIpc::named_semaphore::remove(fWriteSemName.c_str());
    bIpc::named_semaphore::remove(fReadSemName.c_str());
}

Bool_t BmnOnlineSink::HandleData(fair::mq::MessagePtr &inputMessage, Int_t) {
    if (fVerbose > 3) LOG(info) << "BmnOnlineSink::HandleData(): Starting...";

    fWriteSem->wait();

    fShmSegment->destroy<char>(fMsgName.c_str());

    auto msgSize = inputMessage->GetSize();
    auto shmContainer = fShmSegment->construct<char>(fMsgName.c_str())[msgSize](0);

    std::memcpy(shmContainer, inputMessage->GetData(), msgSize);

    fReadSem->post();

    if (fVerbose > 3) LOG(info) << "BmnOnlineSink::HandleData(): Complete!";

    return kTRUE;
}
