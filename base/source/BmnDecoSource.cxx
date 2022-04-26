
#include "BmnDecoSource.h"

BmnDecoSource::BmnDecoSource(TString addr) {
    _addrString = Form("tcp://%s:%d", addr.Data(), 5555);
    iEventNumber = 0;
    //    fEventHeader = NULL;
    fGemDigits = NULL;
    fSilDigits = NULL;
    fCscDigits = NULL;
    fT0Digits = NULL;
    fTof400Digits = NULL;
    fTof700Digits = NULL;
    fDigiArrays = NULL;
    fRunId = 0;
    fPeriodId = 7;
    fFirstEvent = kTRUE;
    fT0BranchName = (fPeriodId < 7) ? "T0" : "BC2";
}

BmnDecoSource::~BmnDecoSource() {
}

Bool_t BmnDecoSource::Init() {
    printf("BmnDecoSource::Init()\n");
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
    fEventHeader = new BmnEventHeader();
    ioman->RegisterInputObject("BmnEventHeader.", fEventHeader);

    fGemDigits = new TClonesArray("BmnGemStripDigit"); //::Class());
    ioman->RegisterInputObject("GEM", fGemDigits);


    fSilDigits = new TClonesArray("BmnSiliconDigit"); //::Class());
    ioman->RegisterInputObject("SILICON", fSilDigits);

    fCscDigits = new TClonesArray("BmnCSCDigit"); //::Class());
    ioman->RegisterInputObject("CSC", fCscDigits);

    fT0Digits = new TClonesArray("BmnTrigDigit");
    ioman->RegisterInputObject(fT0BranchName.c_str(), fT0Digits);

    fTof400Digits = new TClonesArray("BmnTof1Digit"); //::Class());
    ioman->RegisterInputObject("TOF400", fTof400Digits);

    fTof700Digits = new TClonesArray("BmnTof2Digit"); //::Class());
    ioman->RegisterInputObject("TOF700", fTof700Digits);
    return kTRUE;
}

void BmnDecoSource::Close() {
    if (fEventHeader) {
//        fEventHeader->Delete();
        delete fEventHeader;
    }
    if (fGemDigits) {
        fGemDigits->Delete();
        delete fGemDigits;
    }
    if (fSilDigits) {
        fSilDigits->Delete();
        delete fSilDigits;
    }
    if (fTof400Digits) {
        fTof400Digits->Delete();
        delete fTof400Digits;
    }
    if (fTof700Digits) {
        fTof700Digits->Delete();
        delete fTof700Digits;
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
        //        printf("Received frame_size = %d\n", frame_size);
        if (fDigiArrays) {
            fDigiArrays->Clear();
            delete fDigiArrays;
            fDigiArrays = NULL;
        }
        _tBuf->Reset();
        _tBuf->SetBuffer(zmq_msg_data(&_msg), zmq_msg_size(&_msg));
        fDigiArrays = (DigiArrays*) (_tBuf->ReadObject(DigiArrays::Class()));
        //        cout << "EventID: " << fDigiArrays->header->GetEventId() << endl;
        //        cout << "Accepted: " << fDigiArrays->header->GetTrigInfo()->GetTrigAccepted() << endl;
        //        cout << "Count of GEM digits: " << fDigiArrays->gem->GetEntriesFast() << endl;
        //        cout << "Count of Sil digits: " << fDigiArrays->silicon->GetEntriesFast() << endl;
        //        cout << "Count of TOF400 digits: " << fDigiArrays->tof400->GetEntriesFast() << endl;
        //        cout << "Count of TOF700 digits: " << fDigiArrays->tof700->GetEntriesFast() << endl;

        // move result TClonesArray to registered TClonesArray
        fT0Digits->Delete();
        fGemDigits->Delete();
        fSilDigits->Delete();
        fTof400Digits->Delete();
        fTof700Digits->Delete();
        BmnEventHeader *header = fDigiArrays->header;
        fEventHeader->SetRunId(header->GetRunId());
        fEventHeader->SetEventId(header->GetEventId());
        fEventHeader->SetEventTimeTS(header->GetEventTimeTS());
        fEventHeader->SetEventTime(header->GetEventTime());
        fEventHeader->SetEventType(header->GetEventType());
        fEventHeader->SetTripWord(kFALSE);
        fEventHeader->SetTrigInfo(header->GetTrigInfo());
        fEventHeader->SetTimeShift(header->GetTimeShift());
        fEventHeader->SetStartSignalInfo(header->GetStartSignalTime(), header->GetStartSignalWidth());
        if (fFirstEvent && fDigiArrays->trigAr) {
            for (Int_t iTrig = 0; iTrig < fDigiArrays->trigAr->size(); iTrig++) {
                TClonesArray *ar = (*fDigiArrays->trigAr)[iTrig];
                if (strcmp(ar->GetName(), "BC2") == 0)
                    iT0BranchIndex = iTrig;
            }
        }

        fGemDigits->AbsorbObjects(fDigiArrays->gem);
        fSilDigits->AbsorbObjects(fDigiArrays->silicon);
        fCscDigits->AbsorbObjects(fDigiArrays->csc);
        fT0Digits->AbsorbObjects((*fDigiArrays->trigAr)[iT0BranchIndex]);
        fTof400Digits->AbsorbObjects(fDigiArrays->tof400);
        fTof700Digits->AbsorbObjects(fDigiArrays->tof700);
        _tBuf->DetachBuffer();
        zmq_msg_close(&_msg);
    }

    return 0;
}

void BmnDecoSource::FillEventHeader(FairEventHeader* feh) {

    ////    printf("fDigiArrays->header->GetEntriesFast() = %d\n", fEventHeader->GetEntriesFast());
    if (feh) {

        Int_t prevRunId = fRunId;
        fRunId = feh->GetRunId();

        if (prevRunId != feh->GetRunId()) {
            printf("New Run Id: %5d\n", fRunId);
            fRunInst->GetSink()->Close();
            TString outDstName = GetDstNameFromRunId(fRunId);
            fRunInst->SetSink(new FairRootFileSink(outDstName));
            //            if (prevRunId != 0)
            //                return 0;
            //            else
            //                printf("Start Run Id: %5d\n", fRunId);
        }
        //            printf("feh run id = %d\n", feh->GetRunId());
        //        feh->SetMCEntryNumber(fEvtHeader->GetMCEntryNumber());
    }
    ////    feh->SetInputFileId(0);

    return;
}


ClassImp(BmnDecoSource)
