
#include "BmnMQSource.h"

BmnMQSource::BmnMQSource(std::string addr, Bool_t toFile) :
iEventNumber(0),
_tBuf(nullptr),
fEventHeader(nullptr),
fServer(nullptr) {
    _addrString = addr; //Form("tcp://%s:%d", addr.Data(), 6666);
    fRunId = 0;
    fPeriodId = 7;
    fFirstEvent = kTRUE;
    fToFile = toFile;
}

BmnMQSource::~BmnMQSource() {
}

Bool_t BmnMQSource::Init() {
    printf("BmnMQSource::Init()\n");
    if (!InitZMQ())
        return kFALSE;
    zmq_msg_init(&_msg);
    Int_t frame_size = 0;
    do {
        if (fServer)
            fServer->ProcessRequests();
        frame_size = zmq_msg_recv(&_msg, _decoSocket, ZMQ_DONTWAIT);
        if (frame_size == -1) {
            //                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    usleep(TimeDelta);
                    break;
                case EINTR:
                    printf("EINTR\n");
                    printf("Exit!\n");
                    return 1;
                    break;
                case EFAULT:
                    fprintf(stderr, "Receive error № %d #%s\n", errno, zmq_strerror(errno));
                    return 1;
                    break;
                default:
                    break;
            }
            gSystem->ProcessEvents();
        }
    } while (frame_size <= 0);
    //        printf("Received frame_size = %d\n", frame_size);
    BmnParts * parts = nullptr;
    _tBuf->Reset();
    _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
    parts = static_cast<BmnParts *> (_tBuf->ReadObject(BmnParts::Class()));
    //        cout << "TCA len : " << parts->GetArrays().size() << endl;
    //        cout << "Obj len : " << parts->GetObjects().size() << endl;

    FairRootManager* ioman = FairRootManager::Instance();
    for (TClonesArray * ar : parts->GetArrays()) {
        TClonesArray * newAr = new TClonesArray(ar->GetClass());
        newAr->SetName(ar->GetName());
        //            printf("Register %30s  %30s\n", ar->GetName(), newAr->GetName());
        ioman->RegisterInputObject(newAr->GetName(), newAr);
        //            ioman->Register(newAr->GetName(), ".", newAr, fToFile);
        fArrVec.push_back(newAr);
    }
    for (TNamed * tn : parts->GetObjects()) {
        //            printf("Register %20s\n", tn->GetName());
        //            printf("ClassName %20s  Class_Name %20s  GetName %20s\n", tn->ClassName(), tn->Class_Name(), tn->GetName());
        TClass * cl = tn->Class();
        TNamed* ob = static_cast<TNamed*> (tn->Clone()); // cl->New());
        ioman->RegisterInputObject(tn->GetName(), ob);
        fNamVec.push_back(ob);
    }

    _tBuf->DetachBuffer();
    zmq_msg_close(&_msg);
    return kTRUE;
}

void BmnMQSource::Close() {
    for (TClonesArray * ar : fArrVec)
        if (ar) {
            ar->Delete();
            delete ar;
        }
    for (TNamed * ar : fNamVec)
        if (ar)
            delete ar;

    if (fEventHeader) {
        delete fEventHeader;
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    delete _tBuf;
}

Int_t BmnMQSource::ReadEvent(UInt_t i) {
//    printf("ReadEvent(%4u)\n", i);
    zmq_msg_init(&_msg);
    Int_t frame_size = 0;
    do {
        //        if (fServer)
        //            fServer->ProcessRequests();
        frame_size = zmq_msg_recv(&_msg, _decoSocket, ZMQ_DONTWAIT);
        if (frame_size == -1) {
            //                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    usleep(TimeDelta);
                    break;
                case EINTR:
                    printf("EINTR\n");
                    printf("Exit!\n");
                    return 1;
                    break;
                case EFAULT:
                    fprintf(stderr, "Receive error № %d #%s\n", errno, zmq_strerror(errno));
                    return 1;
                    break;
                default:
                    break;
            }
            gSystem->ProcessEvents();
        }
    } while (frame_size <= 0);
    BmnParts * parts = nullptr;

    _tBuf->Reset();
    _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
    parts = static_cast<BmnParts *> (_tBuf->ReadObject(BmnParts::Class()));
    //        cout << "TCA len : " << parts->GetArrays().size() << endl;
    //        cout << "Obj len : " << parts->GetObjects().size() << endl;
    //        cout << "Accepted: " << fDigiArrays->header->GetTrigInfo()->GetTrigAccepted() << endl;

    // move result TClonesArray to registered TClonesArray
    for (UInt_t iAr = 0; iAr < fArrVec.size(); iAr++) {
        fArrVec[iAr]->Delete();
        fArrVec[iAr]->AbsorbObjects(parts->GetArrays()[iAr]);
        //            cout << "Count of " << fArrVec[iAr]->GetName() << " digits: " << fArrVec[iAr]->GetEntriesFast() << endl;
    }
    for (UInt_t iAr = 0; iAr < fNamVec.size(); iAr++) {
        //            printf("ClassName %20s  Class_Name %20s  GetName %20s\n", fNamVec[iAr]->ClassName(), fNamVec[iAr]->Class_Name(), fNamVec[iAr]->GetName());
        if (!strcmp(fNamVec[iAr]->ClassName(), "DstEventHeader")) {
            static_cast<DstEventHeader*> (fNamVec[iAr])->CopyFrom(
                    static_cast<DstEventHeader*> (parts->GetObjects()[iAr]));
            //                cout << "Object " << fNamVec[iAr]->GetName() << endl;
            cout << "EventID: " << static_cast<DstEventHeader*> (fNamVec[iAr])->GetEventId() << endl;
        }
        if (!strcmp(fNamVec[iAr]->ClassName(), "CbmVertex")) {
            static_cast<CbmVertex*> (fNamVec[iAr])->CopyFrom(
                    static_cast<CbmVertex*> (parts->GetObjects()[iAr]));
//                            cout << "VZ " << static_cast<CbmVertex*> (fNamVec[iAr])->GetZ() << endl;
        }
    }
    _tBuf->DetachBuffer();
    zmq_msg_close(&_msg);
    return 0;
}

void BmnMQSource::FillEventHeader(FairEventHeader* feh) {

    ////    printf("fDigiArrays->header->GetEntriesFast() = %d\n", fEventHeader->GetEntriesFast());
    if (feh) {
        //        feh->SetRunId(feh->GetRunId());

        Int_t prevRunId = fRunId;
        fRunId = feh->GetRunId();

        //        if (prevRunId != feh->GetRunId()) {
        //            printf("New Run Id: %5d\n", fRunId);
        //            fRunInst->GetSink()->Close();
        //            TString outDstName = GetDstNameFromRunId(fRunId);
        //            fRunInst->SetSink(new FairRootFileSink(outDstName));
        //            //            if (prevRunId != 0)
        //            //                return 0;
        //            //            else
        //            //                printf("Start Run Id: %5d\n", fRunId);
        //        }
        //            printf("feh run id = %d\n", feh->GetRunId());
        //        feh->SetMCEntryNumber(fEvtHeader->GetMCEntryNumber());
    }
    ////    feh->SetInputFileId(0);

    return;
}

Bool_t BmnMQSource::InitZMQ() {
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_SUB);
    if (zmq_setsockopt(_decoSocket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
        //DBGERR("zmq subscribe")
        fprintf(stderr, "Error subscribing to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    }
    if (zmq_connect(_decoSocket, _addrString.c_str()) != 0) {
        //DBGERR("zmq connect")
        fprintf(stderr, "Error connecting to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    } else {
        printf("connected to %s\n", _addrString.c_str());
    }
    _tBuf = new TBufferFile(TBuffer::kRead);
    return kTRUE;
}

ClassImp(BmnMQSource)
