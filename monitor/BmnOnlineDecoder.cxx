/* 
 * File:   BmnOnlineDecoder.cxx
 * Author: ilnur
 * 
 * Created on January 11, 2017, 11:08 AM
 */

#include "BmnOnlineDecoder.h"
#include "RawTypes.h"

#include <regex>
#include <thread>
#include <dirent.h>
#include <sys/inotify.h>
#include <zmq.h>

BmnOnlineDecoder::BmnOnlineDecoder() {
    fRunID = UNKNOWN_RUNID;
    rawDataDecoder = NULL;
    _ctx = NULL;
    fBmnSetup = kBMNSETUP;
    //fRecoChain = NULL;
}

BmnOnlineDecoder::~BmnOnlineDecoder() {
    if (rawDataDecoder) {
        rawDataDecoder->DisposeDecoder();
        delete rawDataDecoder;
        rawDataDecoder = NULL;
    }
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
    //if (fRecoChain) delete fRecoChain;
}

BmnStatus BmnOnlineDecoder::InitDecoder(TString fRawFileName) {
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder(fRawFileName);
    Int_t runID = rawDataDecoder->GetRunId();
    if (runID < 1) {
        runID = GetRunIdFromName(_curFile);
        //        regex re(".*mpd_run_.*_(\\d+).data");
        //        string idstr = regex_replace(fRawFileName.Data(), re, "$1");
        //        runID = atoi(idstr.c_str());
        if (runID < 0) {
            printf("!!! Error Could not detect runID\n");
            return kBMNERROR;
        }
    }
    InitDecoder(runID);

    DBG("fin")
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::InitDecoder(Int_t runID) {
    printf("InitDecoder runID = %d\n", runID);
    if (!rawDataDecoder)
        rawDataDecoder = new BmnRawDataDecoder();
    rawDataDecoder->SetRunId(runID);
    rawDataDecoder->SetPeriodId(fPeriodID);
    rawDataDecoder->SetDetectorSetup(fDetectorSetup);
    rawDataDecoder->SetBmnSetup(fBmnSetup);
    rawDataDecoder->SetAdcDecoMode(kBMNADCSM);
    rawDataDecoder->SetVerbose(1);
    TString PeriodSetupExt = Form("%d%s.txt", fPeriodID, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC"));
    rawDataDecoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    rawDataDecoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    rawDataDecoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    rawDataDecoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    rawDataDecoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    rawDataDecoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
    // in case comment out the line rawDataDecoder->SetTof400Mapping("...")  
    // the maps of TOF400 will be read from DB (only for JINR network)
    rawDataDecoder->SetTOF700ReferenceRun(-1);
    rawDataDecoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt);
    rawDataDecoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt, TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    if (rawDataDecoder->GetRunId() >= 4278 && rawDataDecoder->GetPeriodId() == 7)
        rawDataDecoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", fPeriodID));
    else
        rawDataDecoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", fPeriodID));
    rawDataDecoder->SetZDCMapping("ZDC_map_period_5.txt");
    rawDataDecoder->SetZDCCalibration("zdc_muon_calibration.txt");
    rawDataDecoder->SetScWallMapping("SCWALL_map_dry_run_2022.txt");
    rawDataDecoder->SetScWallCalibration("SCWALL_calibration_2022.txt");
    rawDataDecoder->SetFHCalMapping("FHCAL_map_dry_run_2022.txt");
    rawDataDecoder->SetFHCalCalibration("FHCAL_calibration_2022.txt");
    rawDataDecoder->SetHodoMapping("HODO_Q_map_dry_run_2022.txt");
    rawDataDecoder->SetHodoCalibration("HODO_Q_calibration_2022.txt");
    rawDataDecoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
    rawDataDecoder->SetECALCalibration("");
    rawDataDecoder->SetMwpcMapping(TString("MWPC_map_period") + ((fPeriodID == 6 && rawDataDecoder->GetRunId() < 1397) ? 5 : PeriodSetupExt));
    rawDataDecoder->SetLANDMapping("land_mapping_jinr_triplex_2022.txt");
    rawDataDecoder->SetLANDPedestal("r0030_land_clock_2022.hh");
    rawDataDecoder->SetLANDTCal("r0030_land_tcal_2022.hh");
    rawDataDecoder->SetLANDDiffSync("r352_cosmic1.hh");
    rawDataDecoder->SetLANDVScint("neuland_sync_2.txt");
    rawDataDecoder->SetTofCalMapping("tofcal_mapping_jinr_triplex.txt.t0");
    rawDataDecoder->SetTofCalPedestal("tofcal_ped_JK.hh");
    rawDataDecoder->SetTofCalTCal("tofcal_tcal_JK.hh");
    rawDataDecoder->SetTofCalDiffSync("tofcal_diffsync_cosmic1.hh");
    rawDataDecoder->SetTofCalVScint("tofcal_sync_2022.txt");
    rawDataDecoder->InitMaps();
    if (_curFile.Length() > 0)
        rawDataDecoder->InitConverter(_curDir + _curFile);
    else
        rawDataDecoder->InitConverter();
    BmnStatus iniStatus = rawDataDecoder->InitDecoder();
    if (iniStatus == kBMNSUCCESS) {
        //InitReco();
    }
    return iniStatus;

}

BmnStatus BmnOnlineDecoder::InitReco() {
    TTree *digiTree = rawDataDecoder->GetDigiTree();
    fRecoChain = new TChain();
    fRecoChain->AddFriend(digiTree);
    fRunAna = new FairRunAna();
    //fRunAna->SetSource(fRecoChain);

    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::IterReco() {


    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::DecodeStream() {
    //    dataReceiver = new BmnDataReceiver();
    //    dataQue = dataReceiver->GetDataQueue();
    //    thread rcvThread(threadReceiveWrapper, dataReceiver);
    //
    //    while (kTRUE) {
    //
    //    }

    return kBMNSUCCESS;
}

void BmnOnlineDecoder::threadReceiveWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

BmnStatus BmnOnlineDecoder::ConnectDataSocket() {
    _socket_data = zmq_socket(_ctx, ZMQ_STREAM);
    Char_t endpoint_addr[MAX_ADDR_LEN];
    //    snprintf(endpoint_addr, MAX_ADDR_LEN, "tcp://%s:%d", DAQ_IP, DAQ_PORT);
    snprintf(endpoint_addr, MAX_ADDR_LEN, "tcp://%s", fDAQAddr.Data());
    if (zmq_connect(_socket_data, endpoint_addr) != 0) {
        DBGERR("zmq connect")
        return kBMNERROR;
    } else {
        printf("\"connected\" to %s\n", endpoint_addr);
    }
    UInt_t rcvBufLen = MAX_BUF_LEN;
    Int_t rcvBuf = 0;
    size_t vl = sizeof (rcvBuf);
    if (zmq_getsockopt(_socket_data, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("data rcvbuf = %d\n", rcvBuf);

    if (zmq_setsockopt(_socket_data, ZMQ_RCVBUF, &rcvBufLen, sizeof (rcvBufLen)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_socket_data, ZMQ_SNDBUF, &rcvBufLen, sizeof (rcvBufLen)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBufLen = 0;

    if (zmq_getsockopt(_socket_data, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("data rcvbuf = %d\n", rcvBuf);
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::ConnectDigiSocket() {
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    //    _socket_mcast = zmq_socket(_ctx, ZMQ_XSUB);
    Int_t rcvBuf = 0;
    Int_t id = 0;
    size_t vl = sizeof (rcvBuf);
    //    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
    //        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    //        printf("rcvbuf = %d\n", rcvBuf);
    rcvBuf = 8192; //MAX_BUF_LEN;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBuf = 0;
    if (zmq_getsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("digi sndbuf = %d\n", rcvBuf);
    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::OpenStream() {
    DBG("started")
    _ctx = zmq_ctx_new();
    if (ConnectDataSocket() != kBMNSUCCESS)
        return kBMNERROR;
    if (ConnectDigiSocket() != kBMNSUCCESS)
        return kBMNERROR;
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::CloseStream() {
    DBG("started")
    //    zmq_close(_socket_mcast);
    zmq_close(_socket_data);
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    return kBMNFINISH;
}

BmnStatus BmnOnlineDecoder::Decode(TString dirname, TString startFile, Bool_t runCurrent) {
    DBG("started")
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    Int_t rcvBuf = 0;
    size_t vl = sizeof (rcvBuf);
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBuf);
    //    rcvBuf = 8192; //MAX_BUF_LEN;
    //    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
    //        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
    //        if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
    //            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
    //            rcvBuf = 0;
    //    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
    //        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
    //        printf("rcvbuf = %d\n", rcvBuf);
    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    _curFile = startFile;
    _curDir = dirname;

    if (!runCurrent) {
        _curFile = "";
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    } else
        if (_curFile.Length() == 0) {
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }

    while (kTRUE) {
        if (InitDecoder(_curDir + _curFile) == kBMNERROR)
            continue;
        ProcessFileRun(_curDir + _curFile);
        rawDataDecoder->DisposeDecoder();
        delete rawDataDecoder;
        rawDataDecoder = NULL;
        //                break; // @TODO Remove
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    return kBMNSUCCESS;
}

// bmn-hrb-3.jinr.ru
// 10.18.11.193

void BmnOnlineDecoder::ProcessStream() {
    DBG("started")
    OpenStream();
    //    UInt_t *buf = (UInt_t*) malloc(MAX_BUF_LEN);
    Char_t conID[MAX_ADDR_LEN] = {0};
    Int_t conID_size = 0;
    UInt_t msg_len = 0;
    UInt_t frame_size = 0;
    Int_t iEv = 0;
    Int_t lastEv = -1;
    fEvents = 0;
    BmnStatus convertResult = kBMNSUCCESS;
    Int_t sendRes = 0;
    TBufferFile t(TBuffer::kWrite);
    UInt_t syncCounter = 0;
    Bool_t isListening = kTRUE;
    while ((isListening)/* && (msg_len > MAX_BUF_LEN)*/) {

        //        if ((conID == 0) || (msg_len < MIN_REMNANT_LEN)) {
        conID_size = zmq_recv(_socket_data, &conID, sizeof (conID),
                ((conID == 0) || (msg_len < MIN_REMNANT_LEN)) ? 0 : ZMQ_DONTWAIT);
        //                    printf("ID Recv %u\n", conID_size);
        if (conID_size == -1) {
            printf("ID Receive error #%d : %s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    //                        if ((msg_len < MIN_REMNANT_LEN) || (conID == 0))
                    usleep(MSG_TIMEOUT);
                    //                        else
                    //                            printf("no sleep\n");
                    break;
                case EINTR:
                    isListening = kFALSE;
                    printf("Exit!\n");
                    continue;
                    break;
                default:
                    break;
            }
        } else {
            //                printf("ID size =  %d\n Id:%x\n", conID_size, conID);
        }
        //        }
        if (conID_size < 0)
            continue;
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        Int_t recv_more = 0;
        UInt_t *msgPtr;
        UInt_t *word;
        Bool_t isReceiving = kTRUE;
        do {
            frame_size = zmq_msg_recv(&msg, _socket_data, ZMQ_DONTWAIT); //  ZMQ_DONTWAIT
            //            printf("recv %u\n", frame_size);
            //frame_size = zmq_recv(_socket_data, buf, MAX_BUF_LEN, 0);
            if (frame_size == -1) {
                printf("Receive error # %d #%s\n", errno, zmq_strerror(errno));
                switch (errno) {
                    case EAGAIN:
                        if ((msg_len < MPD_EVENT_HEAD_WORDS_OLD))
                            usleep(MSG_TIMEOUT);
                        break;
                    case EINTR:
                        //                        printf("EINTR\n");
                        isReceiving = kFALSE;
                        isListening = kFALSE;
                        printf("Exit!\n");
                        break;
                    case EFAULT:
                        printf("EFAULT\n");
                        zmq_close(_socket_data);
                        ConnectDataSocket();
                        isReceiving = kFALSE;
                        break;
                    default:
                        break;
                }
            } else {
                if (frame_size) {
                    if (msg_len + frame_size > MAX_BUF_LEN) {
                        printf("buf overflow! msg len %u frame size %u\n", msg_len, frame_size);
                        printf("Something wrong! Exit!\n");
                        isReceiving = kFALSE;
                        isListening = kFALSE;
                        msg_len = 0;
                        //                        if (msg_len % kNBYTESINWORD) {
                        //                            printf("Something wrong! Resetting buffer!\n");
                        //                            msg_len = 0;
                        //                        } else {
                        //                            UInt_t dropped = msg_len + frame_size - MAX_BUF_LEN;
                        //                            msg_len -= dropped;
                        //                            printf("Frame_size %u\t Message length %d, dropped %d bytes\n", frame_size, msg_len, dropped);
                        //                            memmove(buf, &buf[frame_size], msg_len);
                        //                        }
                    }
                    memcpy(buf + msg_len, zmq_msg_data(&msg), frame_size);
                    msg_len += frame_size;
                    //                    printf("msg_len    = %d\n", msg_len);
                }
            }
            size_t opt_size = sizeof (recv_more);
            if (zmq_getsockopt(_socket_data, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                return;
            }
            //            printf("ZMQ rcvmore = %d\n", recv_more);
            zmq_msg_close(&msg);
        } while (recv_more && isReceiving);

        if (msg_len < MPD_EVENT_HEAD_WORDS_OLD * sizeof (UInt_t))
            continue;
        UInt_t iWord = 0;
        Bool_t evExit = false;
        UInt_t lenBytes = 0;
        UInt_t lenWords = 0;
        UInt_t runID = 0;
        word = reinterpret_cast<UInt_t*> (buf);
        while ((iWord < msg_len / kNBYTESINWORD) && (!evExit) && (msg_len > MPD_EVENT_HEAD_WORDS_OLD)) {
            //            printf("iter iWord  = %u   msg_len %u   %u\n", iWord, msg_len, MIN_REMNANT_LEN);
            switch (UInt_t val = *(word + iWord)) {
                case SYNC_RUN_START:
                    printf("iWord = %u\n", iWord);
                    printf("start run\n");
                    lenWords = *(word + ++iWord) / sizeof (UInt_t);
                    printf("payLen = %u words\n", lenWords);
                    if (lenWords + iWord > MAX_BUF_LEN) {
                        printf("Wrong payload size!\n");
                        lenBytes = iWord * sizeof (UInt_t);
                        msg_len -= lenBytes;
                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                        break;
                    }
                    BmnRawDataDecoder::ParseRunTLV((word + ++iWord), lenWords, runID);
                    printf("runID = %u\n", runID);
                    if (fRunID != runID) {
                        fRunID = runID;
                        printf("fRunID %u\n", fRunID);
                        if (rawDataDecoder) {
                            rawDataDecoder->DisposeDecoder();
                            delete rawDataDecoder;
                            rawDataDecoder = NULL;
                        }
                        if (InitDecoder(runID) == kBMNERROR) {
                            printf("\n\tError in InitDecoder !!\n\n");
                            if (rawDataDecoder) {
                                delete rawDataDecoder;
                                rawDataDecoder = NULL;
                            }
                            //                                    evExit = kTRUE;
                            break;
                        }
                        rawDataDecoder->SetRunId(runID);
                    }
                    iWord += lenWords;
                    lenBytes = iWord * sizeof (UInt_t);
                    printf(" lenBytes %u \n", lenBytes);
                    msg_len -= lenBytes;
                    printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                    memmove(&buf[0], &buf[lenBytes], msg_len);
                    iWord = 0;
                    //                    evExit = kTRUE;
                    break;
                case SYNC_RUN_STOP:
                    printf("stop run\n");
                    lenWords = *(word + ++iWord) / sizeof (UInt_t);
                    printf("payLen = %d words\n", lenWords);
                    if (lenWords + iWord > MAX_BUF_LEN) {
                        printf("Wrong payload size!\n");
                        lenBytes = iWord * sizeof (UInt_t);
                        msg_len -= lenBytes;
                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;

                        break;
                    }
                    BmnRawDataDecoder::ParseRunTLV((word + ++iWord), lenWords, runID);
                    fRunID = runID;
                    printf("runID = %d\n", runID);
                    if (rawDataDecoder) {
                        rawDataDecoder->DisposeDecoder();
                        delete rawDataDecoder;
                        rawDataDecoder = NULL;
                    }
                    iWord += lenWords;
                    lenBytes = iWord * sizeof (UInt_t);
                    msg_len -= lenBytes;
                    //                    printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                    memmove(&buf[0], &buf[lenBytes], msg_len);
                    iWord = 0;
                    //                    evExit = kTRUE;
                    break;
                case SYNC_EVENT:
                case SYNC_EVENT_OLD:
                    //                    printf(ANSI_COLOR_BLUE"SYNC_EVENT\n"ANSI_COLOR_RESET);
                    lenBytes = *(word + ++iWord);
                    lenWords = lenBytes / kNBYTESINWORD + (fPeriodID <= 7 ? 1 : 0);
                    //                    printf("iWord    == %u\n", iWord);
                    //                    printf("lenBytes == %u\n", lenBytes);
                    //                    printf("lenWords == %u\n", lenWords);
                    if (msg_len / kNBYTESINWORD >= lenWords + (fPeriodID > 7 ? MPD_EVENT_HEAD_WORDS : MPD_EVENT_HEAD_WORDS_OLD)) {
                        //                    printf("captured enough\n");
                        if (!rawDataDecoder)
                            if (InitDecoder(fRunID) == kBMNERROR) {
                                printf("\n\tError in InitDecoder !!\n\n");
                                //                                evExit = kTRUE;
                                break;
                            }
                        //                        rawDataDecoder->SetRunId(fRunID);
                        convertResult = rawDataDecoder->ConvertRawToRootIterate(word + ++iWord, lenWords);
                        //                        printf(" convertResult %d \n", convertResult);
                        if (convertResult == kBMNERROR) {
                            printf("convert failed\n");
                            //                            evExit = kTRUE;
                            lenBytes = iWord * sizeof (UInt_t);
                            msg_len -= lenBytes;
                            printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                            memmove(&buf[0], &buf[lenBytes], msg_len);
                            iWord = 0;
                            break;
                        }
                        BmnStatus decostat = rawDataDecoder->DecodeDataToDigiIterate();
                        //                        printf(" decostat %d \n", decostat);
                        fEvents++;
                        if (decostat == kBMNSUCCESS) {
                            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
                            BmnEventHeader* head = iterDigi.header;
                            if (head) {
                                if (head->GetEventType() == kBMNPAYLOAD) {
                                    t.WriteObject(&iterDigi);
                                    sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
                                    //                            printf("sendRes %d\n", sendRes);
                                    t.Reset();
                                    if (sendRes == -1) {
                                        printf("Send error # %d : %s\n", errno, zmq_strerror(errno));
                                    }
                                }
                            }
                        }
                        iWord += lenWords;
                        lenBytes = iWord * sizeof (UInt_t);
                        msg_len -= lenBytes;
                        //                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                    } else {
                        //                        printf("Not enough data in the buffer!\n");
                        iWord = 0;
                        evExit = kTRUE;
                    }
                    break;
                case SYNC_STAT: // just skip at this point
                    //                    printf(ANSI_COLOR_BLUE "STAT\n" ANSI_COLOR_RESET);
                    lenBytes = *(word + ++iWord);
                    lenWords = lenBytes / kNBYTESINWORD + (fPeriodID <= 7 ? 1 : 0);
                    //                    printf("iWord    == %u\n", iWord);
                    //                    printf("lenBytes == %u\n", lenBytes);
                    //                    printf("lenWords == %u\n", lenWords);
                    if (msg_len / kNBYTESINWORD >= MPD_EVENT_HEAD_WORDS_OLD + lenWords) {
                        ++iWord; // if other is commented
                        //                        if (!rawDataDecoder)
                        //                            if (InitDecoder(fRunID) == kBMNERROR) {
                        //                                printf("\n\tError in InitDecoder !!\n\n");
                        //                                //                                evExit = kTRUE;
                        //                                break;
                        //                            }
                        //                        //                        rawDataDecoder->SetRunId(fRunID);
                        //                        convertResult = rawDataDecoder->ConvertRawToRootIterate(word + ++iWord, lenWords);
                        //                        printf(" convertResult %d \n", convertResult);
                        //                        if (convertResult == kBMNERROR) {
                        //                            printf("convert failed\n");
                        //                            //                            evExit = kTRUE;
                        //                            lenBytes = iWord * sizeof (UInt_t);
                        //                            msg_len -= lenBytes;
                        //                            printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        //                            memmove(&buf[0], &buf[lenBytes], msg_len);
                        //                            iWord = 0;
                        //                            break;
                        //                        }
                        //                        BmnStatus decostat = rawDataDecoder->DecodeDataToDigiIterate();
                        //                        printf(" decostat %d \n", decostat);
                        //                        fEvents++;
                        //                        if (decostat == kBMNSUCCESS) {
                        //                            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
                        //                            BmnEventHeader* head = iterDigi.header;
                        //                            if (head) {
                        //                                if (head->GetEventType() == kBMNPAYLOAD) {
                        //                                    t.WriteObject(&iterDigi);
                        //                                    sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
                        //                                    //                            printf("sendRes %d\n", sendRes);
                        //                                    t.Reset();
                        //                                    if (sendRes == -1) {
                        //                                        printf("Send error # %d : %s\n", errno, zmq_strerror(errno));
                        //                                    }
                        //                                }
                        //                            }
                        //                        }
                        iWord += lenWords;
                        lenBytes = iWord * sizeof (UInt_t);
                        msg_len -= lenBytes;
                        //                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                    } else {
                        //                        printf("Not enough data in the buffer!\n");
                        iWord = 0;
                        evExit = kTRUE;
                    }
                    break;
                case SYNC_JSON:
                    //                    printf(ANSI_COLOR_BLUE"SYNC_JSON\n"ANSI_COLOR_RESET);
                    lenBytes = *(word + ++iWord);
                    lenWords = lenBytes / kNBYTESINWORD + (fPeriodID <= 7 ? 1 : 0);
                    //                    printf("iWord    == %u\n", iWord);
                    //                    printf("lenBytes == %u\n", lenBytes);
                    //                    printf("lenWords == %u\n", lenWords);
                    if (msg_len / kNBYTESINWORD >= lenWords + MPD_EVENT_HEAD_WORDS) {
                        ++iWord; // if other is commented
                        //                    printf("captured enough\n");
                        //                        if (!rawDataDecoder)
                        //                            if (InitDecoder(fRunID) == kBMNERROR) {
                        //                                printf("\n\tError in InitDecoder !!\n\n");
                        //                                //                                evExit = kTRUE;
                        //                                break;
                        //                            }
                        //                        //                        rawDataDecoder->SetRunId(fRunID);
                        //                        convertResult = rawDataDecoder->ConvertRawToRootIterate(word + ++iWord, lenWords);
                        //                        printf(" convertResult %d \n", convertResult);
                        //                        if (convertResult == kBMNERROR) {
                        //                            printf("convert failed\n");
                        //                            //                            evExit = kTRUE;
                        //                            lenBytes = iWord * sizeof (UInt_t);
                        //                            msg_len -= lenBytes;
                        //                            printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        //                            memmove(&buf[0], &buf[lenBytes], msg_len);
                        //                            iWord = 0;
                        //                            break;
                        //                        }
                        //                        BmnStatus decostat = rawDataDecoder->DecodeDataToDigiIterate();
                        //                        printf(" decostat %d \n", decostat);
                        //                        fEvents++;
                        //                        if (decostat == kBMNSUCCESS) {
                        //                            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
                        //                            BmnEventHeader* head = iterDigi.header;
                        //                            if (head) {
                        //                                if (head->GetEventType() == kBMNPAYLOAD) {
                        //                                    t.WriteObject(&iterDigi);
                        //                                    sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
                        //                                    //                            printf("sendRes %d\n", sendRes);
                        //                                    t.Reset();
                        //                                    if (sendRes == -1) {
                        //                                        printf("Send error # %d : %s\n", errno, zmq_strerror(errno));
                        //                                    }
                        //                                }
                        //                            }
                        //                        }
                        iWord += lenWords;
                        lenBytes = iWord * sizeof (UInt_t);
                        msg_len -= lenBytes;
                        //                        printf(" %u will move by %u bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        iWord = 0;
                    } else {
                        //                        printf("Not enough data in the buffer!\n");
                        iWord = 0;
                        evExit = kTRUE;
                    }
                    break;
                default:
                    iWord++;
                    break;
            }
        }
        if (iWord >= MAX_BUF_LEN) {
            printf("Wrong data, resetting array!\n");
            msg_len = 0;
        }
    }
    CloseStream();
}

void BmnOnlineDecoder::ProcessFileRun(TString rawFileName, UInt_t timeLimit) {
    UInt_t iEv = 0;
    UInt_t lastEv = 0;
    BmnStatus convertResult = kBMNSUCCESS;
    Int_t sendRes = 0;
    TBufferFile t(TBuffer::kWrite);
    while (kTRUE) {
        convertResult = rawDataDecoder->ConvertRawToRootIterateFile(timeLimit);
        if (convertResult == kBMNFINISH) {
            printf("finish\n");
            break;
        }
        lastEv = iEv;
        iEv = rawDataDecoder->GetEventId();
        if (iEv > lastEv) {
            BmnStatus decodeResult = rawDataDecoder->DecodeDataToDigiIterate();
            //        printf("iev %u  convert %d decode %d\n", iEv, convertResult, decodeResult);
            fEvents++;
            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
            if (iterDigi.header == NULL)
                continue;
            BmnEventHeader* head = (BmnEventHeader*) iterDigi.header;
            if (head->GetEventType() != kBMNPAYLOAD)
                continue;
            t.WriteObject(&iterDigi);
            sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
            t.Reset();
            if (sendRes == -1) {
                printf("Send error %d #%s\n", errno, zmq_strerror(errno));

            }
        }
        if (convertResult == kBMNTIMEOUT) {
            printf("timeout\n");
            break;
        }
    }
}

TString BmnOnlineDecoder::WatchNext(TString dirname, TString filename, Int_t cycleWait) {
    DBG("started")
            struct dirent **namelist;
    regex re("\\w+\\.data");
    Int_t n;
    TString ret;
    while (kTRUE) {
        n = scandir(dirname.Data(), &namelist, 0, versionsort);
        if (n < 0)
            perror("scandir");
        else {
            for (Int_t i = 0; i < n; ++i) {
                if (regex_match(namelist[i]->d_name, re))
                    ret = namelist[i]->d_name;
                free(namelist[i]);
            }
            free(namelist);
        }
        if (strcmp(filename.Strip().Data(), ret.Strip().Data()) != 0)
            return ret;
        gSystem->ProcessEvents();
        usleep(cycleWait);
    }

    return ret;
}

TString BmnOnlineDecoder::WatchNext(Int_t inotifDir, Int_t cycleWait) {
    TString fileName = "";
    Char_t evBuf[INOTIF_BUF_LEN];
    while (kTRUE) {
        Int_t len, i = 0;
        len = read(inotifDir, evBuf, INOTIF_BUF_LEN);
        if ((len == -1) && (errno != EAGAIN))
            DBG("inotify read error!")
        else {
            while (i < len) {
                struct inotify_event *event = (struct inotify_event*) &evBuf[i];
                if (event->len) {
                    if ((event->mask & IN_CREATE) && !(event->mask & IN_ISDIR)) {
                        fileName = TString(event->name);
                        printf("File %s was created!\n", fileName.Data());
                        break;
                    }
                }
                i += sizeof (struct inotify_event) +event->len;
            }
            if (cycleWait > 0)
                usleep(cycleWait);
            else
                break;
        }
        gSystem->ProcessEvents();
    }
    return fileName;

}

BmnStatus BmnOnlineDecoder::BatchDirectory(TString dirname) {
    _curDir = dirname;
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    struct dirent **namelist;
    //    const regex re(".*mpd_run_.*_(\\d+).data");


    Int_t runCount = 0;
    Int_t n;
    n = scandir(dirname, &namelist, 0, versionsort);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            _curFile = TString(namelist[i]->d_name);
            Int_t runID = GetRunIdFromName(_curFile);
            if (runID > 4000) {
                //                            if (regex_match(namelist[i]->d_name, re)) {
                if (runCount == 0) {
                    if (InitDecoder(_curDir + _curFile) == kBMNERROR)
                        continue;
                } else {
                    rawDataDecoder->ResetDecoder(_curDir + _curFile);
                    rawDataDecoder->SetRunId(runID);
                    rawDataDecoder->SetBmnSetup(fBmnSetup);
                    rawDataDecoder->SetPeriodId(fPeriodID);
                }
                ProcessFileRun(_curFile, 0);
                runCount++;
            } else {
                //                printf("!!! Could not detect runID for %s\n", namelist[i]->d_name);
                continue;
            }
            free(namelist[i]);
        }
        if (rawDataDecoder) {
            rawDataDecoder->DisposeDecoder();
            delete rawDataDecoder;
            rawDataDecoder = NULL;
        }
        free(namelist);
    }
    //    for (auto cl : clients) {
    //        cl->Close();
    //        delete cl;
    //    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    return kBMNSUCCESS;
}

Int_t BmnOnlineDecoder::GetRunIdFromName(TString name) {
    TPRegexp re(".*mpd_run_.*_(\\d+).data");
    TObjArray *subStr = re.MatchS(name.Data());
    if (subStr->GetEntriesFast() > 1) {
        TString str = ((TObjString*) subStr->At(1))->GetString();
        Int_t runID = atoi(str.Data());
        return runID;
    } else
        return -1;
}

#define PAD_WIDTH_SIL  1280
#define PAD_HEIGHT_SIL  840
#define COLS  2

void BmnOnlineDecoder::StripView(Int_t periodID, Int_t runID, BmnSetup fSetup) {
    //    TString inFileNameMK = Form("bmn_run%04i_sidigitthr2all.root", runID); //Form("MK_digi_%04i.root", runID);//Form("MK_digi_%04i_newest.root", runID);
    TString OrigFileName = Form("bmn_run%04i_digi_test_MK.root", runID); //Form("MK_digi_%04i.root", runID);//Form("MK_digi_%04i_newest.root", runID);
    //    TString inFileNameMK = Form("MK_digi_%04i.root", runID); //Form("MK_digi_%04i.root", runID);//Form("MK_digi_%04i_newest.root", runID);
    //    TString inFileNameBmn = Form("/ncx/nica/mpd22/batyuk/digi/run7/bmn/bmn_run%04i_digi.root", runID); //Form("MK_digi_%04i.root", runID);//Form("MK_digi_%04i_newest.root", runID);
    //    TString inFileNameBmn = Form("bmn_run%04i_digi_test_MK_r.root", runID); //Form("MK_digi_%04i.root", runID);//Form("MK_digi_%04i_newest.root", runID);
    TString TestFileName = Form("bmn_run%04i_digi_test_SM.root", runID); //Form("MK_digi_%04i.root", runID);//Form("MK_digi_%04i_newest.root", runID);
    StripView(OrigFileName, TestFileName, periodID, runID, fSetup);
}

void BmnOnlineDecoder::StripView(TString OrigFileName, TString TestFileName, Int_t periodID, Int_t runID, BmnSetup fSetup) {
    gStyle->SetOptStat(0);
    Int_t sumMods = 0;
    Int_t maxLayers = 0;

    TString fnames[COLS] = {OrigFileName, TestFileName};
    TString treeNames[COLS] = {"bmndata", "bmndata"};
    TString hdrNames[COLS] = {"BmnEventHeader.", "BmnEventHeader."};
    TString runhdrNames[COLS] = {"DigiRunHeader", "DigiRunHeader"};
    TString silNames[COLS] = {"SILICON", "SILICON"};
    TString gemNames[COLS] = {"GEM", "GEM"};
    TString cscNames[COLS] = {"CSC", "CSC"};
    TFile * files[COLS] = {NULL};
    TTree * trees[COLS] = {NULL};
    TClonesArray * hits[COLS] = {NULL};
    TClonesArray * tracks[COLS] = {NULL};
    DigiRunHeader * runHeaders[COLS] = {NULL, NULL};
    BmnEventHeader * headers[COLS] = {NULL, NULL};
    TClonesArray * silDigit[COLS] = {NULL, NULL};
    TClonesArray * gemDigit[COLS] = {NULL, NULL};
    TClonesArray * cscDigit[COLS] = {NULL, NULL};
    vector<vector<vector<TH1F* > > > histStrip[COLS];
    vector<vector<vector<TH1F* > > > histStripGem[COLS];
    vector<vector<vector<TH1F* > > > histStripCsc[COLS];
    TCanvas *canStrip;
    TCanvas *canStripGem;
    TCanvas *canStripCsc;
    vector<PadInfo*> canStripPads;
    vector<PadInfo*> canStripPadsGem;
    vector<PadInfo*> canStripPadsCsc;

    Int_t hf = 2400;
    const Int_t kNStrips = 640;
    TCanvas *canProf = new TCanvas("canprof", "can", 1920, 1920);
    canProf->Divide(1, 4);
    TH2F* hfilter = new TH2F("hfilter", "hfilter", kNStrips + 1, 0, kNStrips, hf + 1, 0, hf);
    TH2F* hfilterMK = new TH2F("hfilterMK", "hfilterMK", kNStrips + 1, 0, kNStrips, hf + 1, 0, hf);
    TH1F* hsig = new TH1F("hsig", "hsig", hf + 1, 0, hf);
    hsig->SetLineColor(kRed);
    TH1F* hsigMK = new TH1F("hsigMK", "hsigMK", hf + 1, 0, hf);

    TString name;
    TString title;

    // ********************
    // silicon pads
    // ********************
    sumMods = 0;
    maxLayers = 0;
    unique_ptr<BmnSiliconStationSet> stationSet = BmnAdcProcessor::GetSilStationSet(periodID, fSetup);
    printf("xmlConf stations %i\n", stationSet->GetNStations());
    for (Int_t iCol = 0; iCol < COLS; iCol++) {
        for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
            vector<vector<TH1F*> > rowGEM;
            BmnSiliconStation* st = stationSet->GetSiliconStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TH1F*> colGEM;
                BmnSiliconModule *mod = st->GetModule(iModule);
                //                printf("iStation %i iModule %i nlayers %i sumMods %i\n", iStation, iModule, mod->GetNStripLayers(), sumMods);
                //                if (maxLayers < mod->GetNStripLayers())
                maxLayers = 2;
                for (Int_t iLayer = 0; iLayer < 2; iLayer++) {
                    BmnSiliconLayer lay = mod->GetStripLayer(iLayer);
                    name = Form("Silicon_%d_Station_%d_module_%d_layer_%d", iCol, iStation, iModule, iLayer);
                    title = Form("Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                    TH1F *h = new TH1F(name, title, lay.GetNStrips(), 0, lay.GetNStrips());
                    h->SetTitleSize(0.06, "XY");
                    h->SetLabelSize(0.08, "XY");
                    h->GetXaxis()->SetTitle("Strip Number");
                    h->GetXaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitle("Activation Count");
                    h->GetYaxis()->SetTitleColor(kOrange + 10);
                    colGEM.push_back(h);
                }
                rowGEM.push_back(colGEM);

            }
            histStrip[iCol].push_back(rowGEM);

        }
    }
    sumMods = sumMods / COLS;
    name = "SilCanvas";
    canStrip = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
    canStrip->Divide(maxLayers, sumMods);
    Int_t modCtr = 0;
    canStripPads.resize(sumMods * maxLayers);
    for (auto &pad : canStripPads) {
        pad = nullptr;
    }
    for (Int_t iStation = 0; iStation < stationSet->GetNStations(); iStation++) {
        BmnSiliconStation * st = stationSet->GetSiliconStation(iStation);
        //        printf("st[%d]->GetNModules()=%d\n", iStation, st->GetNModules());
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnSiliconModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < 2; iLayer++) {
                Int_t iPad = modCtr * maxLayers + iLayer;
                PadInfo *p = new PadInfo();
                p->current = histStrip[0][iStation][iModule][iLayer];
                p->ref = histStrip[1][iStation][iModule][iLayer];
                p->ref->SetLineColor(kRed);
                canStripPads[iPad] = p;
                canStrip->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }
    // ********************
    // gem pads
    // ********************
    sumMods = 0;
    maxLayers = 0;
    BmnGemStripStationSet *gemStationSet = BmnAdcProcessor::GetGemStationSet(periodID, fSetup);
    for (Int_t iCol = 0; iCol < COLS; iCol++) {
        for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
            vector<vector<TH1F*> > rowGEM;
            BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TH1F*> colGEM;
                BmnGemStripModule *mod = st->GetModule(iModule);
                if (maxLayers < mod->GetNStripLayers())
                    maxLayers = mod->GetNStripLayers();
                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    BmnGemStripLayer lay = mod->GetStripLayer(iLayer);
                    name = Form("GEM_%i_Station_%d_module_%d_layer_%d", iCol, iStation, iModule, iLayer);
                    title = Form("Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                    TH1F *h = new TH1F(name, title, lay.GetNStrips(), 0, lay.GetNStrips());
                    h->SetTitleSize(0.07, "XY");
                    h->SetLabelSize(0.07, "XY");
                    h->GetXaxis()->SetTitle("Strip Number");
                    h->GetXaxis()->SetTitleColor(kOrange + 10);
                    h->GetXaxis()->SetTitleOffset(0.7);
                    h->GetYaxis()->SetTitle("Activation Count");
                    h->GetYaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitleOffset(0.7);
                    colGEM.push_back(h);

                }
                rowGEM.push_back(colGEM);

            }
            histStripGem[iCol].push_back(rowGEM);

        }
    }
    sumMods = sumMods / COLS;
    name = "GemCanvas";
    canStripGem = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
    canStripGem->Divide(maxLayers, sumMods, 0.002, 0.002);
    modCtr = 0;
    canStripPadsGem.resize(sumMods * maxLayers);
    for (Int_t iStation = 0; iStation < gemStationSet->GetNStations(); iStation++) {
        BmnGemStripStation * st = gemStationSet->GetGemStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnGemStripModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = histStripGem[0][iStation][iModule][iLayer];
                p->ref = histStripGem[1][iStation][iModule][iLayer];
                p->ref->SetLineColor(kRed);
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripPadsGem[iPad] = p;
                canStripGem->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }
    // ********************
    // csc pads
    // ********************
    sumMods = 0;
    maxLayers = 0;
    BmnCSCStationSet *StationSet = BmnAdcProcessor::GetCSCStationSet(periodID, fSetup);
    for (Int_t iCol = 0; iCol < COLS; iCol++) {
        for (Int_t iStation = 0; iStation < StationSet->GetNStations(); iStation++) {
            vector<vector<TH1F*> > rowGEM;
            BmnCSCStation * st = StationSet->GetStation(iStation);
            sumMods += st->GetNModules();
            for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
                vector<TH1F*> colGEM;
                BmnCSCModule *mod = st->GetModule(iModule);
                if (maxLayers < mod->GetNStripLayers())
                    maxLayers = mod->GetNStripLayers();
                for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                    BmnCSCLayer lay = mod->GetStripLayer(iLayer);
                    name = Form("CSC_%i_Station_%d_module_%d_layer_%d", iCol, iStation, iModule, iLayer);
                    title = Form("Station_%d_module_%d_layer_%d", iStation, iModule, iLayer);
                    TH1F *h = new TH1F(name, title, lay.GetNStrips(), 0, lay.GetNStrips());
                    h->SetTitleSize(0.06, "XY");
                    h->SetLabelSize(0.08, "XY");
                    h->GetXaxis()->SetTitle("Strip Number");
                    h->GetXaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitle("Activation Count");
                    h->GetYaxis()->SetTitleColor(kOrange + 10);
                    h->GetYaxis()->SetTitleOffset(1.0);
                    colGEM.push_back(h);

                }
                rowGEM.push_back(colGEM);

            }
            histStripCsc[iCol].push_back(rowGEM);

        }
    }
    sumMods = sumMods / COLS;
    name = "CscCanvas";
    canStripCsc = new TCanvas(name, name, PAD_WIDTH_SIL * maxLayers, PAD_HEIGHT_SIL * sumMods);
    canStripCsc->Divide(maxLayers, sumMods);
    modCtr = 0;
    canStripPadsCsc.resize(sumMods * maxLayers);
    for (Int_t iStation = 0; iStation < StationSet->GetNStations(); iStation++) {
        BmnCSCStation * st = StationSet->GetStation(iStation);
        for (Int_t iModule = 0; iModule < st->GetNModules(); iModule++) {
            BmnCSCModule *mod = st->GetModule(iModule);
            for (Int_t iLayer = 0; iLayer < mod->GetNStripLayers(); iLayer++) {
                PadInfo *p = new PadInfo();
                p->opt = "";
                p->current = histStripCsc[0][iStation][iModule][iLayer];
                p->ref = histStripCsc[1][iStation][iModule][iLayer];
                p->ref->SetLineColor(kRed);
                Int_t iPad = modCtr * maxLayers + iLayer;
                canStripPadsCsc[iPad] = p;
                canStripCsc->GetPad(iPad + 1)->SetGrid();
            }
            modCtr++;
        }
    }

    // ********************
    // Trees processing
    // ********************

    Int_t curEv = 0;
    Int_t preEv = 0;
    Long64_t nEvs = LONG_MAX;
    for (Int_t i = 0; i < COLS; i++) {
        files[i] = new TFile(fnames[i], "READ");
        if (files[i]->IsOpen())
            printf("file %s opened\n", fnames[i].Data());
        else {
            fprintf(stderr, "file %s open error\n", fnames[i].Data());
            return;
        }
        trees[i] = (TTree*) files[i]->Get(treeNames[i]);
        nEvs = Min(nEvs, trees[i]->GetEntries());
        cout << "#recorded entries = " << trees[i]->GetEntries() << endl;
        if (i > 0)
            runHeaders[i] = (DigiRunHeader*) files[i]->Get(runhdrNames[i].Data());
        //                runHeaders[i]);
        trees[i]->SetBranchAddress(hdrNames[i], &headers[i]);
        trees[i]->SetBranchAddress(silNames[i], &silDigit[i]);
        trees[i]->SetBranchAddress(gemNames[i], &gemDigit[i]);
        trees[i]->SetBranchAddress(cscNames[i], &cscDigit[i]);

    }
    for (Int_t i = 0; i < COLS; i++) {
        cout << "tree # " << i << endl;
        if (i > 0 && runHeaders[i]) {
            printf("START (event 1):\t%s\n", runHeaders[i]->GetRunStartTime().AsString());
            printf("FINISH (event %lld):\t%s\n", nEvs, runHeaders[i]->GetRunEndTime().AsString());
        }
        for (Int_t iEv = 0; iEv < nEvs; iEv++) {
            trees[i]->GetEntry(iEv);
            if (iEv % 10000 == 0)
                cout << "iEv = " << iEv << endl;
            BmnEventHeader *hdr = (BmnEventHeader*) headers[i];
            BmnTrigInfo * trigInfo = hdr->GetTrigInfo();
            preEv = curEv;
            curEv = hdr->GetEventId();
            if (curEv - preEv > 1)
                printf("Events between %d & %d missed\n", preEv, curEv);
            //            printf("cand %04u, acc %04u, bef %04u, after %04u, rjct %04u, all %04u, avail %04u\n",
            //                    trigInfo->GetTrigCand(),
            //                    trigInfo->GetTrigAccepted(),
            //                    trigInfo->GetTrigBefo(),
            //                    trigInfo->GetTrigAfter(),
            //                    trigInfo->GetTrigRjct(),
            //                    trigInfo->GetTrigAll(),
            //                    trigInfo->GetTrigAvail());
            // silicon
            if (silDigit[i])
                for (Int_t iDig = 0; iDig < silDigit[i]->GetEntriesFast(); iDig++) {
                    //                                                printf("iDig %d Silicon\n", iDig);
                    BmnSiliconDigit *dig = (BmnSiliconDigit*) silDigit[i]->UncheckedAt(iDig);
                    if (!(dig->IsGoodDigit())) {
                        //                        printf("ev %d is not good\n", curEv);
                        continue;
                    }
                    Int_t module = dig->GetModule();
                    Int_t station = dig->GetStation();
                    Int_t layer = dig->GetStripLayer();
                    Int_t strip = dig->GetStripNumber();
                    if (periodID == 6 && i == 0) {
                        module--;

                    }
                    //                    if (i == 1)
                    //                        printf("station %d module %d layer %d strip %d\n", station, module, layer, strip);
                    histStrip[i][station][module][layer]->Fill(strip);
                    //                    if (station == 0 && module == 0 && layer == 0) {
                    //                        if (i == 0) {
                    //                            hfilterMK->Fill(strip, dig->GetStripSignal());
                    //                            if (dig->GetStripNumber() > 511)
                    //                                hsigMK->Fill(dig->GetStripSignal());
                    //                        } else {
                    //                            hfilter->Fill(strip, dig->GetStripSignal());
                    //
                    //                            if (dig->GetStripNumber() > 511)
                    //                                hsig->Fill(dig->GetStripSignal());
                    //                        }
                    //                    }
                }
            // gem
            if (gemDigit[i])
                for (Int_t iDig = 0; iDig < gemDigit[i]->GetEntriesFast(); iDig++) {
                    //                                                printf("iDig %d GEM of %d\n", iDig, gemDigit[i]->GetEntriesFast());
                    BmnGemStripDigit *dig = (BmnGemStripDigit*) gemDigit[i]->UncheckedAt(iDig);
                    if (!(dig->IsGoodDigit()))
                        continue;
                    Int_t module = dig->GetModule();
                    Int_t station = dig->GetStation();
                    Int_t layer = dig->GetStripLayer();
                    Int_t strip = dig->GetStripNumber();
                    if (periodID == 6 && i == 0) {
                        station--;
                    }
                    //                                        if (i == 1)
                    //                                            printf("station %d module %d layer %d strip %d\n", station, module, layer, strip);
                    histStripGem[i][station][module][layer]->Fill(strip);
                }
            // csc
            if (cscDigit[i])
                for (Int_t iDig = 0; iDig < cscDigit[i]->GetEntriesFast(); iDig++) {
                    //                                                printf("iDig %d CSC\n", iDig);
                    BmnCSCDigit *dig = (BmnCSCDigit*) cscDigit[i]->UncheckedAt(iDig);
                    if (!(dig->IsGoodDigit()))
                        continue;
                    Int_t module = dig->GetModule();
                    Int_t station = dig->GetStation();
                    Int_t layer = dig->GetStripLayer();
                    Int_t strip = dig->GetStripNumber();
                    histStripCsc[i][station][module][layer]->Fill(strip);
                }
        }
        printf("Last event %d\n", curEv);
    }

    //    for (Int_t col = 0; col < maxLayers; col++)
    //        for (Int_t row = 0; row < sumMods; row++) {
    //            TVirtualPad *pad = canStrip->cd(row * maxLayers + col + 1);
    //            pad->Clear();
    //            //                if (row > 2)
    //            //                    h[col][row]->Draw();
    //            //                else
    //            TString s = Form("%s.>>%s_X_Vertex(200, -10, 10)", names[col].Data()), "PrimaryVertex.fNTracks>1", "");
    //            for (Int_t iTrack = 0; iTrack < nColors; iTrack++)
    //                if (iTrack)
    //                    h[col][row][iTrack]->Draw("SAME");
    //                else
    //                    h[col][row][iTrack]->Draw("");
    //        }

    BmnHist::DrawRef(canStrip, &canStripPads);
    canStrip->SaveAs(Form("can-run-%d-sil.png", runID));
    BmnHist::DrawRef(canStripGem, &canStripPadsGem);
    canStripGem->SaveAs(Form("can-run-%d-gem.png", runID));
    BmnHist::DrawRef(canStripCsc, &canStripPadsCsc);
    canStripCsc->SaveAs(Form("can-run-%d-csc.png", runID));
    canProf->cd(1);
    hfilter->Draw("colz");
    canProf->cd(2);
    hfilterMK->Draw("colz");
    canProf->cd(3);
    hsig->Draw("");
    //    canProf->cd(4);
    hsigMK->Draw("same");
    canProf->SaveAs("can-prof.png");
    //    canStrip->SaveAs(Form("can-run-%d.eps", runID));
    //    canStrip->SaveAs(Form("can-run-%d.pdf", runID));
}




ClassImp(BmnOnlineDecoder);
