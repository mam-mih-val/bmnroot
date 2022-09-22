#ifndef BMN_ONLINE_SAMPLER_H
#define BMN_ONLINE_SAMPLER_H

#include <fairmq/Device.h>

#include <TFile.h>
#include <TTree.h>

#include <string>

class BmnOnlineSampler : public fair::mq::Device {
   public:
    BmnOnlineSampler();
    ~BmnOnlineSampler();

    BmnOnlineSampler(const BmnOnlineSampler &) = delete;
    BmnOnlineSampler &operator=(BmnOnlineSampler &) = delete;

   protected:
    virtual void Init();
    virtual void Reset();
    virtual void InitTask();
    virtual void ResetTask();
    virtual Bool_t ConditionalRun();

   private:
    Int_t fVerbose;
    std::string fOutputChannelName;

    std::unique_ptr<TFile> fEventsFile;
    std::unique_ptr<TTree> fEventsData;
    Long64_t fNEvents;
    Long64_t fCurrentEvent;
};

#endif  // BMN_ONLINE_SAMPLER_H
