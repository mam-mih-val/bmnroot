

#include "BmnProfilometerSource.h"

BmnProfilometerSource::BmnProfilometerSource(vector<string> addr, vector<string> boardIds) {
    fAddrs = move(addr);
    fBoardIds = move(boardIds);
    iEventNumber = 0;
    fRunId = 0;
    fPeriodId = 8;
    fFirstEvent = kTRUE;
    fArr = nullptr;
}

BmnProfilometerSource::~BmnProfilometerSource() {
    for (auto &it : fBoardsMap)
        delete it.second;
}

Bool_t BmnProfilometerSource::Init() {
    printf("BmnProfilometerSource::Init()\n");
    _ctx = zmq_ctx_new();
    _rawSocket = zmq_socket(_ctx, ZMQ_SUB);
    for (string &addr : fAddrs) {
        if (zmq_connect(_rawSocket, addr.c_str()) != 0) {
            //DBGERR("zmq connect")
            fprintf(stderr, "Error connecting to ZMQ socket: %s\n", strerror(errno));
            return kFALSE;
        } else {
            printf("connected to %s\n", addr.c_str());
        }
    }
    if (zmq_setsockopt(_rawSocket, ZMQ_SUBSCRIBE, NULL, 0) == -1) {
        //DBGERR("zmq subscribe")
        fprintf(stderr, "Error subscribing to ZMQ socket: %s\n", strerror(errno));
        return kFALSE;
    }

    FairRootManager* ioman = FairRootManager::Instance();
    fArr = new TClonesArray(BmnADCDigit::Class());
    ioman->RegisterInputObject("ADC192ASIC", fArr);
    TPRegexp re("\\w+(\\d+)");
    for (string &id : fBoardIds) {
        ProfBoard* b = new ProfBoard();
        TString str(id);
        re.Substitute(str, "$1");
        b->board_id = str.Atoi();
        fBoardsMap.insert(make_pair(id, b));
    }
    return kTRUE;
}

void BmnProfilometerSource::Close() {
    zmq_close(_rawSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
}

Int_t BmnProfilometerSource::ReadEvent(UInt_t i) {
    fArr->Delete();
    bool keepWorking = kTRUE;
    bool isReceiving = kTRUE;
    const Int_t MaxStrLen = 100;
    bool isIdFound = kFALSE;
    bool isHeaderFound = kFALSE;
    bool isTrailerFound = kFALSE;
    string BoardName;
    ProfBoard * Board = nullptr;
    do {
        Int_t recv_more = 0;
        isIdFound = kFALSE;

        do {
            //            gSystem->ProcessEvents();
            //            fServer->ProcessRequests();
            zmq_msg_t msg;
            zmq_msg_init(&msg);
            Int_t frame_size = zmq_msg_recv(&msg, _rawSocket, 0); //  ZMQ_DONTWAIT
            printf("recv %d\n", frame_size);
            if (frame_size == -1) {
                //                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
                switch (errno) {
                    case EAGAIN:
                        printf("EAGAIN\n");
                        usleep(50000);
                        break;
                    case EINTR:
                        printf("EINTR\n");
                        isReceiving = kFALSE;
                        keepWorking = kFALSE;
                        printf("Exit!\n");
                        break;
                    case EFAULT:
                        printf("EFAULT\n");
                        zmq_close(_rawSocket);
                        isReceiving = kFALSE;
                        keepWorking = kFALSE;
                        break;
                    default:
                        break;
                }
            } else {
                if (frame_size < MaxStrLen) {
                    string str(static_cast<char*> (zmq_msg_data(&msg)), zmq_msg_size(&msg));
                    //                    printf("str %s\n", str.c_str());
                    if (isIdFound) {
                        if (str == DataTrailer) {
                            isTrailerFound = kTRUE;
                            printf("trailer\n");
                            keepWorking = kFALSE;
                        }
                        if (str == DataHeader) {
                            isHeaderFound = kTRUE;
                            printf("header\n");
                        }
                    } else {
                        //                        if (str == TargetBoardId) {
                        BoardName = str;
                        auto it = fBoardsMap.find(BoardName);
                        if (it != fBoardsMap.end())
                            Board = it->second;
                        isIdFound = kTRUE;
                        printf("id %s\n", str.c_str());
                        //                        }
                    }
                } else {
                    if (isHeaderFound && Board) {
                        ProcessBuffer(static_cast<uint32_t*> (zmq_msg_data(&msg)), zmq_msg_size(&msg) / 4, Board);
                    }
                }
            }
            size_t opt_size = sizeof (recv_more);
            if (zmq_getsockopt(_rawSocket, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                return -1;
            }
            //            printf("ZMQ rcvmore = %d\n", recv_more);
            zmq_msg_close(&msg);
        } while (recv_more && isReceiving && (!isTrailerFound));
    } while (keepWorking);
    printf("FullReceive\n");
    if (isTrailerFound) {
        printf("SPILLEND for %s\n", BoardName.c_str());
        isHeaderFound = kFALSE;
        isTrailerFound = kFALSE;
    }

    //    }

    return 0;
}

void BmnProfilometerSource::FillEventHeader(FairEventHeader* feh) {

    //    ////    printf("fDigiArrays->header->GetEntriesFast() = %d\n", fEventHeader->GetEntriesFast());
    //    if (feh) {
    //
    //        Int_t prevRunId = fRunId;
    //        fRunId = feh->GetRunId();
    //
    //    }
    //    ////    feh->SetInputFileId(0);

    return;
}

int BmnProfilometerSource::ProcessBuffer(uint32_t *word, size_t len, ProfBoard * board) {
    uint32_t holdb_temp = 0;
    uint32_t holdb = 0;
    //    string str(reinterpret_cast<char*> (word), 6);
    //    printf("str %s\n", str.c_str());
    for (uint32_t i = 0; i < len; i++) {
        uint32_t data = word[i];
        //                                                printf("data %08X  i = %05u\n",data, i);
        // Check is it data or a trigger:
        if (BmnProfRawTools::data_or_trig(data)) {
            holdb = BmnProfRawTools::holdb_cntr(data); //get holdb counter
            if (holdb_temp == holdb) {
                //                printf("data  %08X\n", data);
                // Divide ADC0 and ADC1 data
                if (!BmnProfRawTools::adc_num(data)) { // 1st ADC
                    board->adc1_word.push_back(data);
                } else if (BmnProfRawTools::adc_num(data)) { // 2nd ADC
                    board->adc2_word.push_back(data);
                }
            } else {
                if (board->adc1_word.size() == BmnProfRawTools::ChannelDigitCnt())
                    new((*fArr)[fArr->GetEntriesFast()]) BmnADCDigit(board->board_id, 0, BmnProfRawTools::ChannelDigitCnt(), board->adc1_word);
                //                if (board->adc2_word.size() == BmnProfRawTools::ChannelDigitCnt())
                //                    new((*fArr)[fArr->GetEntriesFast()]) BmnADCDigit(board->board_id, 1, BmnProfRawTools::ChannelDigitCnt(), board->adc2_word);
                //                printf("adc1_word  %lu\n", adc1_word.size());
                //                printf("adc1  %lu\n", adc1.size());
                board->adc1_word.clear();
                board->adc2_word.clear();
                // Divide ADC0 and ADC1 data
                if (!BmnProfRawTools::adc_num(data)) { // 1st ADC
                    board->adc1_word.push_back(data);
                } else { // 2nd ADC
                    board->adc2_word.push_back(data);
                }
                holdb_temp = holdb;
            }
        } else {
            //            if (BmnProfRawTools::trig_psd(data)) {
            //                trigger_pside.push_back(data);
            //            } else if (BmnProfRawTools::trig_nsd(data)) {
            //                trigger_nside.push_back(data);
            //            }
        }
    }
    return 0;
}

ClassImp(BmnProfilometerSource)
