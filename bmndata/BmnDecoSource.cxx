
#include <zmq.h>

#include "BmnDecoSource.h"

BmnDecoSource::BmnDecoSource(TString addr) {
    _addrString = addr;
    iEventNumber = 0;
    fEventHeader = NULL;
    fGemDigits = NULL;
    fT0Digits = NULL;
    fTof1Digits = NULL;
}

BmnDecoSource::BmnDecoSource(const BmnDecoSource& orig) {
}

BmnDecoSource::~BmnDecoSource() {
}

Bool_t BmnDecoSource::Init() {
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);
    if (zmq_setsockopt(_decoSocket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
        //DBGERR("zmq subscribe")
        fprintf(stderr, "Error subscribing to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    }
    if (zmq_connect(_decoSocket, _addrString.Data()) != 0) {
        //DBGERR("zmq connect")
        fprintf(stderr, "Error connecting to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    } else {
        printf("connected to %s\n", _addrString.Data());
    }

    _tBuf = new TBufferFile(TBuffer::kRead);
    //    _tBuf->SetReadMode();

    FairRootManager* ioman = FairRootManager::Instance();
    fEventHeader = new TClonesArray("BmnEventHeader");
    ioman->Register("EventHeader", "Event", fEventHeader, kFALSE);

    fGemDigits = new TClonesArray("BmnGemStripDigit");
    ioman->RegisterInputObject("GEM", fGemDigits);

//    fT0Digits = new TClonesArray("BmnTrigDigit");
//    ioman->Register("T0", "T0DIR", fT0Digits, kFALSE);

    fTof1Digits = new TClonesArray("BmnTof1Digit");
    ioman->Register("TOF400", "TOFDIR", fTof1Digits, kFALSE);
    return kTRUE;
}

void BmnDecoSource::Close() {
    if (fEventHeader) {
        fEventHeader->Delete();
        delete fEventHeader;
    }
    if (fGemDigits) {
        fGemDigits->Delete();
        delete fGemDigits;
    }
    if (fTof1Digits) {
        fTof1Digits->Delete();
        delete fTof1Digits;
    }
    if (fT0Digits) {
        fT0Digits->Delete();
        delete fT0Digits;
    }

    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    delete _tBuf;
}

Int_t BmnDecoSource::ReadEvent(UInt_t i) {
    zmq_msg_init(&_msg);
    Int_t frame_size = zmq_msg_recv(&_msg, _decoSocket, 0); // ZMQ_DONTWAIT
    if (frame_size == -1) {
        //        if (errno == EAGAIN) {
        //            usleep(DECO_SOCK_WAIT_PERIOD * 1000);
        //            decoTimeout += DECO_SOCK_WAIT_PERIOD;
        //            if ((decoTimeout > DECO_SOCK_WAIT_LIMIT) && (fState == kBMNWORK)) {
        //                //FinishRun();
        //                fState = kBMNWAIT;
        //                //keepWorking = false; // @TODO Remove
        //                //fServer->SetTimer(50, kTRUE);
        //                DBG("state changed to kBMNWAIT")
        //            }
        //        } else {
        fprintf(stderr, "Receive error № %d #%s\n", errno, zmq_strerror(errno));
        //            return;
        //        }
    } else {
        //decoTimeout = 0;
        _tBuf->Reset();
        _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
        fDigiArrays = (DigiArrays*) (_tBuf->ReadObject(DigiArrays::Class()));
        //    if (fInChain->GetEntry(i))
        //        return 0;
        BmnEventHeader* head = (BmnEventHeader*) fDigiArrays->header->At(0);
        cout<<"Current Run Id: "<<head->GetRunId()<<endl;
        cout<<"Count of BmnEventHeader: "<<fDigiArrays->header->GetEntriesFast()<<endl;
        cout << "Count of GEM digits: " << fDigiArrays->gem->GetEntriesFast() << endl;
        cout << "Count of TOF digits: " << fDigiArrays->tof400->GetEntriesFast() << endl;

        // move result TClonesArray to registered TClonesArray
        fEventHeader->Delete();
        fGemDigits->Delete();
//        fT0Digits->Delete();
        fTof1Digits->Delete();
        fEventHeader->AbsorbObjects(fDigiArrays->header);
        fGemDigits->AbsorbObjects(fDigiArrays->gem);
//        fT0Digits->AbsorbObjects(fDigiArrays->t0);
        fTof1Digits->AbsorbObjects(fDigiArrays->tof400);
    }

    return 1;
}


ClassImp(BmnDecoSource)
