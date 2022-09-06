#include "BmnOnlineSampler.h"

#include <RootSerializer.h>

#include <thread>

BmnOnlineSampler::BmnOnlineSampler()
    : fair::mq::Device(),
      fVerbose(0),
      fOutputChannelName("bmn-online-data-1"),
      fEventsFile(nullptr),
      fEventsData(nullptr),
      fNEvents(0),
      fCurrentEvent(0)
{}

BmnOnlineSampler::~BmnOnlineSampler()
{}

void BmnOnlineSampler::Init() {
    fVerbose = fConfig->GetValue<Int_t>("verbose");
    fOutputChannelName = fConfig->GetValue<std::string>("output-channel");
}

void BmnOnlineSampler::Reset() {}

void BmnOnlineSampler::InitTask() {
    if (fVerbose > 3) LOG(info) << "BmnOnlineSampler::InitTask(): Starting...";

    // !!! Change the path to your file !!!
    fEventsFile = std::unique_ptr<TFile>(TFile::Open("$VMCWORKDIR/macro/run/bmn_run4004_digi.root", "read"));
    fEventsData = std::unique_ptr<TTree>(fEventsFile->Get<TTree>("bmndata"));
    fNEvents = fEventsData->GetEntries();

    if (fVerbose > 3) LOG(info) << "BmnOnlineSampler::InitTask(): nEvents: " << fNEvents;

    if (fVerbose > 3) LOG(info) << "BmnOnlineSampler::InitTask(): Complete!";
}

void BmnOnlineSampler::ResetTask() {
    if (fVerbose > 3) LOG(info) << "BmnOnlineSampler::ResetTask(): Starting...";

    fEventsData.release();
    fEventsFile->Close();

    if (fVerbose > 3) LOG(info) << "BmnOnlineSampler::ResetTask(): Complete!";
}

Bool_t BmnOnlineSampler::ConditionalRun() {
    if (fVerbose > 3) LOG(info) << "BmnOnlineSampler::ConditionalRun(): Event #" << fCurrentEvent;
    if (fCurrentEvent > fNEvents) return false;

    std::unique_ptr<TTree> eventData(fEventsData->CopyTree("", "", 1, fCurrentEvent));
    auto message = NewMessage();
    RootSerializer().Serialize(*message, eventData);

    if (Send(message, fOutputChannelName) < 0) {
        LOG(error) << "BmnOnlineSampler::ConditionalRun(): Sending message failed";
        return kFALSE;
    }

    fCurrentEvent++;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return kTRUE;
}
