#ifndef BMN_ONLINE_SINK_H
#define BMN_ONLINE_SINK_H

#include <fairmq/Device.h>

#include <TTree.h>

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_semaphore.hpp>

namespace bIpc = boost::interprocess;

class BmnOnlineSink : public fair::mq::Device {
   public:
    BmnOnlineSink();
    ~BmnOnlineSink();

    BmnOnlineSink(const BmnOnlineSink &) = delete;
    BmnOnlineSink &operator=(BmnOnlineSink &) = delete;

   protected:
    virtual void Init();
    virtual void Reset();
    virtual void InitTask();
    virtual void ResetTask();

    Bool_t HandleData(fair::mq::MessagePtr &inputMessage, Int_t);

   private:
    Int_t fVerbose;

    std::string fInputChannelName;
    std::string fShmName;
    std::string fWriteSemName;
    std::string fReadSemName;
    std::string fMsgName;

    std::unique_ptr<bIpc::managed_shared_memory> fShmSegment;
    std::unique_ptr<bIpc::named_semaphore> fWriteSem;
    std::unique_ptr<bIpc::named_semaphore> fReadSem;
};

#endif  // BMN_ONLINE_SINK_H
