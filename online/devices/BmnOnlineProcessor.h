#ifndef BMN_ONLINE_PROCESSOR_H
#define BMN_ONLINE_PROCESSOR_H

#include "BmnTask.h"

#include <FairField.h>
#include <fairmq/Device.h>

#include <string>
#include <vector>

class BmnOnlineProcessor : public fair::mq::Device {
   public:
    BmnOnlineProcessor();
    ~BmnOnlineProcessor();

    BmnOnlineProcessor(const BmnOnlineProcessor &) = delete;
    BmnOnlineProcessor &operator=(BmnOnlineProcessor &) = delete;

   protected:
    virtual void Init();
    virtual void Reset();
    virtual void InitTask();
    virtual void ResetTask();

    Bool_t HandleData(fair::mq::MessagePtr &inputMessage, Int_t);

   private:
    Int_t fVerbose;
    std::string fInputChannelName;
    std::string fOutputChannelName;
    Long64_t fCurrentEvent;

    std::unique_ptr<FairField> fField;
    std::vector<std::unique_ptr<BmnTask>> fTasks;

    Bool_t SetUpExRun(std::unique_ptr<TTree> &dataTree);
    void RegisterTasks(Int_t runPeriod, Int_t runNumber);
};

#endif  // BMN_ONLINE_PROCESSOR_H
