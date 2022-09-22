#include "BmnOnlineShmSource.h"

#include <FairRootManager.h>

#include <TMessage.h>
#include <TSystem.h>

#include "json.hpp"
using json = nlohmann::json;

const auto configFilePath = gSystem->ExpandPathName("$VMCWORKDIR/config/shm-config.json");

class BmnTMessage : public TMessage {
   public:
    BmnTMessage(void *buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
};

BmnOnlineShmSource::BmnOnlineShmSource() : FairOnlineSource(), fEventHeader(nullptr), fGemDigits(nullptr)
{}

BmnOnlineShmSource::~BmnOnlineShmSource()
{}

Bool_t BmnOnlineShmSource::Init() {
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

    fShmSegment = std::make_unique<bIpc::managed_shared_memory>(bIpc::open_only, fShmName.c_str());

    fWriteSem = std::make_unique<bIpc::named_semaphore>(bIpc::open_only, fWriteSemName.c_str());
    fReadSem = std::make_unique<bIpc::named_semaphore>(bIpc::open_only, fReadSemName.c_str());

    /*fEventHeader = std::make_unique<TClonesArray>("BmnEventHeader");
    FairRootManager::Instance()->RegisterInputObject("BmnEventHeader.", fEventHeader.get());*/

    fGemDigits = std::make_unique<TClonesArray>("BmnGemStripHit");
    FairRootManager::Instance()->Register("BmnGemStripHit", "GEM", fGemDigits.get(), kFALSE);

    return kTRUE;
}

Int_t BmnOnlineShmSource::ReadEvent(UInt_t) {
    fReadSem->wait();

    auto msg = fShmSegment->find<char>(fMsgName.c_str());

    auto tMsg = std::make_unique<BmnTMessage>(msg.first, msg.second);
    auto tree = std::unique_ptr<TTree>(static_cast<TTree *>(tMsg->ReadObjectAny(tMsg->GetClass())));

    // fEventHeader->Delete();
    fGemDigits->Delete();

    auto gemDigits = new TClonesArray("BmnGemStripHit");
    tree->SetBranchAddress("BmnGemStripHit", &gemDigits);

    tree->GetEntry(0);

    fGemDigits->AbsorbObjects(gemDigits);
    delete gemDigits;

    tMsg.release();
    fShmSegment->destroy<char>(fMsgName.c_str());
    fWriteSem->post();

    return 0;
}

void BmnOnlineShmSource::Close()
{}

ClassImp(BmnOnlineShmSource)
