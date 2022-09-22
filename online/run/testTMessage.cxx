#include <TFile.h>
#include <TMessage.h>
#include <TTree.h>

#include <iostream>

class BmnTMessage : public TMessage {
   public:
    BmnTMessage(void *buf, Int_t len) : TMessage(buf, len) { }
};

int main(int argc, char const *argv[]) {
    auto fEventsFile = std::unique_ptr<TFile>(TFile::Open("$VMCWORKDIR/macro/run/bmn_run4004_digi.root", "read"));
    auto fEventsData = std::unique_ptr<TTree>(fEventsFile->Get<TTree>("bmndata"));
    auto eventData = std::unique_ptr<TTree>(fEventsData->CopyTree("", "", 1, 0));

    //eventData->Print();

    auto message = std::make_unique<TMessage>(kMESS_OBJECT);
    message->WriteObject(eventData.get());

    auto buffer_ = message->Buffer();
    auto size_ = message->BufferSize();

    void *b_ = nullptr;

    std::memcpy(b_, buffer_, size_);

    //std::cout << "Message size: " << size_ << ' ' << sizeof(buffer_) << std::endl;

    //auto tm = BmnTMessage(b_, size_);

    //auto tree = std::unique_ptr<TTree>(static_cast<TTree*>(tm.ReadObjectAny(tm.GetClass())));

    //tree->Print();

    message.release();
    eventData.release();
    fEventsData.release();
    fEventsFile->Close();

    return 0;
}
