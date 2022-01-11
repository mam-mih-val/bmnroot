/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnOnlineDecoder.cxx
 * Author: ilnur
 * 
 * Created on January 11, 2017, 11:08 AM
 */

#include <regex>
#include <thread>
#include <dirent.h>
#include <sys/inotify.h>
#include <zmq.h>

#include "BmnOnlineDecoder.h"

BmnOnlineDecoder::BmnOnlineDecoder() {
    fRunID = UNKNOWN_RUNID;
    rawDataDecoder = NULL;
    _ctx = NULL;
    fBmnSetup = kBMNSETUP;
    //fRecoChain = NULL;
}

BmnOnlineDecoder::~BmnOnlineDecoder() {
    if (rawDataDecoder) delete rawDataDecoder;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
    //if (fRecoChain) delete fRecoChain;
}

BmnStatus BmnOnlineDecoder::InitDecoder(TString fRawFileName) {
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder(fRawFileName);
    rawDataDecoder->SetAdcDecoMode(kBMNADCSM);
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
    Bool_t setup[11]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 1; // MWPC
    setup[2] = 1; // SILICON
    setup[3] = 1; // GEM
    setup[4] = 1; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 1; // ZDC
    setup[8] = 1; // ECAL
    setup[9] = 1; // LAND
    setup[10] = 1; // CSC
    rawDataDecoder->SetDetectorSetup(setup);
    rawDataDecoder->SetBmnSetup(fBmnSetup);
    TString PeriodSetupExt = Form("%d%s.txt", fPeriodID, ((fBmnSetup == kBMNSETUP) ? "" : "_SRC"));
    //    rawDataDecoder->SetAdcDecoMode(period < 6 ? kBMNADCSM : kBMNADCMK);
    rawDataDecoder->SetAdcDecoMode(kBMNADCSM);
    rawDataDecoder->SetTof400Mapping(TString("TOF400_PlaceMap_RUN") + PeriodSetupExt, TString("TOF400_StripMap_RUN") + PeriodSetupExt);
    rawDataDecoder->SetTOF700ReferenceRun(-1);
    rawDataDecoder->SetTof700Geom(TString("TOF700_geometry_run") + PeriodSetupExt);
    if (rawDataDecoder->GetRunId() >= 4278 && rawDataDecoder->GetPeriodId() == 7)
        rawDataDecoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d_from_run_4278.txt", fPeriodID));
    else
        rawDataDecoder->SetTof700Mapping(TString("TOF700_map_period_") + Form("%d.txt", fPeriodID));
    rawDataDecoder->SetZDCMapping("ZDC_map_period_5.txt");
    //    rawDataDecoder->SetZDCCalibration("zdc_muon_calibration.txt");
    rawDataDecoder->SetECALMapping(TString("ECAL_map_period_") + PeriodSetupExt);
    rawDataDecoder->SetECALCalibration("");
    rawDataDecoder->SetLANDMapping("land_mapping_jinr_triplex.txt");
    rawDataDecoder->SetLANDPedestal("r0030_land_clock.hh");
    rawDataDecoder->SetLANDTCal("r0030_land_tcal.hh");
    rawDataDecoder->SetLANDDiffSync("r352_cosmic1.hh");
    rawDataDecoder->SetLANDVScint("neuland_sync_2.txt");
    rawDataDecoder->SetTrigPlaceMapping(TString("Trig_PlaceMap_Run") + PeriodSetupExt);
    rawDataDecoder->SetTrigChannelMapping(TString("Trig_map_Run") + PeriodSetupExt);
    rawDataDecoder->SetSiliconMapping(TString("SILICON_map_run") + PeriodSetupExt);
    rawDataDecoder->SetGemMapping(TString("GEM_map_run") + PeriodSetupExt);
    rawDataDecoder->SetCSCMapping(TString("CSC_map_period") + PeriodSetupExt);
    rawDataDecoder->SetMwpcMapping(TString("MWPC_map_period") + ((fPeriodID == 6 && rawDataDecoder->GetRunId() < 1397) ? 5 : PeriodSetupExt));
    rawDataDecoder->SetMSCMapping(TString("MSC_map_Run") + PeriodSetupExt);
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

BmnStatus BmnOnlineDecoder::OpenStream() {
    DBG("started")
    _ctx = zmq_ctx_new();
    _decoSocket = zmq_socket(_ctx, ZMQ_PUB);
    //    _socket_mcast = zmq_socket(_ctx, ZMQ_XSUB);
    Int_t rcvBuf = 0;
    size_t vl = sizeof (rcvBuf);
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBuf);
    rcvBuf = 8192; //MAX_BUF_LEN;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBuf = 0;
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBuf);
    TString localDecoStr = Form("tcp://*:%d", RAW_DECODER_SOCKET_PORT);
    if (zmq_bind(_decoSocket, localDecoStr.Data()) != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    _socket_data = zmq_socket(_ctx, ZMQ_STREAM);
    Char_t endpoint_addr[MAX_ADDR_LEN];
    snprintf(endpoint_addr, MAX_ADDR_LEN, "tcp://%s:%d", DAQ_IP, DAQ_PORT);
    if (zmq_connect(_socket_data, endpoint_addr) != 0) {
        DBGERR("zmq connect")
        return kBMNERROR;
    } else {
        printf("connected to %s\n", endpoint_addr);
    }
    UInt_t rcvBufLen = MAX_BUF_LEN;
    if (zmq_setsockopt(_socket_data, ZMQ_RCVBUF, &rcvBufLen, sizeof (rcvBufLen)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_socket_data, ZMQ_SNDBUF, &rcvBufLen, sizeof (rcvBufLen)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBufLen = 0;
    if (zmq_getsockopt(_socket_data, ZMQ_RCVBUF, &rcvBufLen, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBufLen);
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
    rcvBuf = 8192; //MAX_BUF_LEN;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBuf = 0;
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBuf);
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
    Char_t conID[MAX_ADDR_LEN];
    Int_t conID_size;
    UInt_t msg_len = 0;
    UInt_t frame_size = 0;
    Int_t iEv = 0;
    Int_t lastEv = -1;
    BmnStatus convertResult = kBMNSUCCESS;
    Int_t sendRes = 0;
    TBufferFile t(TBuffer::kWrite);
    UInt_t syncCounter = 0;
    Bool_t isListening = kTRUE;
    while ((isListening) && (msg_len < MAX_BUF_LEN)) {
        conID_size = zmq_recv(_socket_data, &conID, sizeof (conID), 0);
        if (conID_size == -1) {
            printf("Receive error #%d : %s\n", errno, zmq_strerror(errno));
            switch (errno) {
                case EAGAIN:
                    usleep(MSG_TIMEOUT);
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
            //            printf("ID size =  %d\n Id:%x\n", conID_size, conID);
        }
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        Int_t recv_more = 0;
        UInt_t *msgPtr;
        UInt_t *word;
        do {
            frame_size = zmq_msg_recv(&msg, _socket_data, 0); // ZMQ_DONTWAIT
            //frame_size = zmq_recv(_socket_data, buf, MAX_BUF_LEN, 0);
            if (frame_size == -1) {
                printf("Receive error № %d #%s\n", errno, zmq_strerror(errno));
                switch (errno) {
                    case EAGAIN:
                        usleep(MSG_TIMEOUT);
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
                //                UChar_t *str = (UChar_t*) malloc((frame_size + 1) * sizeof (UChar_t));
                //                msgPtr = (UInt_t*) zmq_msg_data(&msg);
                if (frame_size) {
                    if (msg_len + frame_size > MAX_BUF_LEN) {
                        UInt_t dropped = msg_len + frame_size - MAX_BUF_LEN;
                        msg_len -= dropped;
                        printf("buf overflow!\t %d will move by %d bytes\n", msg_len, dropped);
                        memmove(buf, &buf[frame_size], msg_len);
                    }
                    memcpy(buf + msg_len/* / kNBYTESINWORD*/, zmq_msg_data(&msg), frame_size); // sizeof(UInt_t) == kNBYTESINWORD always?
                    msg_len += frame_size;
                }
                //                printf("frame_size = %d\n", frame_size);
                //                printf("msg_len    = %d\n", msg_len);
                //                Int_t res = msg_len % kNBYTESINWORD;
                //                if (res)
                //                    printf("WTF?\n");
                //                memcpy(str, zmq_msg_data(&msg), frame_size);
                //                str[frame_size] = '\0';
                //                printf("Frame size =  %d\n Msg:%x\n", frame_size, str);
                //                free(str);
            }
            size_t opt_size = sizeof (recv_more);
            if (zmq_getsockopt(_socket_data, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                return;
            }
            //            printf("ZMQ rcvmore = %d\n", recv_more);
            zmq_msg_close(&msg);
        } while (recv_more);

        if (msg_len < 256 * sizeof (UInt_t)) // number doesn't mean anything, just avoid segfault
            continue;
        UInt_t i = 0;
        Bool_t evExit = false;
        UInt_t lenBytes = 0;
        UInt_t lenWords = 0;
        UInt_t runlen = 0;
        UInt_t runID = 0;
        while ((i < msg_len) && (!evExit)/* / kNBYTESINWORD*/) {
            word = (UInt_t*) (&buf[i]);
            UInt_t payLen = 0;
            switch (*word) {
                case kRUNSTARTSYNC:
                    printf("i = %d\n", i);
                    printf("start run\n");
                    payLen = *(++word);
                    printf("payLen = %d\n", payLen);
                    for (Int_t iss = 0; iss < payLen; iss++) {
                        if (*(++word) == kRUNNUMBERSYNC) {
                            printf("RunNumberSync\n");
                            runlen = *(++word);
                            runID = *(++word);
                            printf("runID = %d\n", runID);
                            if (fRunID != runID) {
                                fRunID = runID;
                                printf("fRunID %d\n", fRunID);
                                if (rawDataDecoder) {
                                    rawDataDecoder->DisposeDecoder();
                                    delete rawDataDecoder;
                                    rawDataDecoder = NULL;
                                }
                                if (InitDecoder(runID) == kBMNERROR) {
                                    printf("\n\tError in InitDecoder !!\n\n");
                                    evExit = kTRUE;
                                    break;
                                }
                                rawDataDecoder->SetRunId(runID);
                            }
                        }
                    }
                    lenBytes = (payLen + 2) * sizeof (UInt_t);
                    printf(" lenBytes %d \n", lenBytes);
                    msg_len -= lenBytes; //lenWords * kNBYTESINWORD;
                    printf(" %d will move by %d bytes\n", msg_len, lenBytes);
                    memmove(&buf[0], &buf[lenBytes], msg_len);
                    i = 0;
                    evExit = kTRUE;
                    break;
                case kRUNSTOPSYNC:
                    printf("i = %d\n", i);
                    printf("stop run\n");
                    payLen = *(++word);
                    printf("payLen = %d\n", payLen);
                    for (Int_t iss = 0; iss < payLen; iss++) {
                        if (*(++word) == kRUNNUMBERSYNC) {
                            printf("RunNumberSync\n");
                            runlen = *(++word);
                            runID = *(++word);
                            fRunID = runID;
                            printf("runID = %d, runlen = %d\n", runID, runlen);
                            if (rawDataDecoder) {
                                rawDataDecoder->DisposeDecoder();
                                delete rawDataDecoder;
                                rawDataDecoder = NULL;
                            }
                        }
                    }
                    lenBytes = (payLen + 2) * sizeof (UInt_t);
                    printf(" lenBytes %d \n", lenBytes);
                    msg_len -= lenBytes; //lenWords * kNBYTESINWORD;
                    printf(" %d will move by %d bytes\n", msg_len, lenBytes);
                    memmove(&buf[0], &buf[lenBytes], msg_len);
                    i = 0;
                    evExit = kTRUE;
                    break;
                case kSYNC1_OLD:
                    //            printf("i = %d\n", i);
                    //                    if (/*(fRunID > 0) &&*/ (buf[i] == kSYNC1)) 
                    //                    printf("found ksync1\n");
                    lenBytes = *(++word);
                    lenWords = lenBytes / kNBYTESINWORD + 1;
                    //                    printf("lenBytes == %d\n", lenBytes);
                    //                    printf("lenWords == %d\n", lenWords);
                    //            if (fDat >= 100000) { // what the constant?
                    //                printf("Wrong data size: %d:  skip this event\n", fDat);
                    //                printf("captured %d\n", ((msg_len - i) / kNBYTESINWORD));
                    //                    printf("(msg_len - i) / kNBYTESINWORD = %d  lenWords  %d\n", ((msg_len - i) / kNBYTESINWORD), lenWords);
                    if ((msg_len - i) / kNBYTESINWORD >= lenWords + MPD_EVENT_HEAD_WORDS) {
                        if (!rawDataDecoder)
                            if (InitDecoder(fRunID) == kBMNERROR) {
                                printf("\n\tError in InitDecoder !!\n\n");
                                evExit = kTRUE;
                                break;
                            }
                        rawDataDecoder->SetRunId(fRunID);
                        //                    printf("captured enough\n");
                        //                    UInt_t *p = &buf[++i];
                        //                        i++;
                        //                            UInt_t* p = &buf[i];
                        ++word;
                        convertResult = rawDataDecoder->ConvertRawToRootIterate(word, lenWords);
                        if (convertResult == kBMNERROR) {
                            printf("convert failed\n");
                            evExit = kTRUE;
                            break;
                        }
                        BmnStatus decostat = rawDataDecoder->DecodeDataToDigiIterate();
                        //                        printf(" decostat %d \n", decostat);
                        fEvents++;
                        if (decostat == kBMNSUCCESS) {
                            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
                            if (iterDigi.header == NULL)
                                continue;
                            BmnEventHeader* head = iterDigi.header;
                            if (head->GetEventType() != kBMNPAYLOAD)
                                continue;
                            t.WriteObject(&iterDigi);
                            sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), ZMQ_NOBLOCK);
                            //                            printf("sendRes %d\n", sendRes);
                            t.Reset();
                            if (sendRes == -1) {
                                printf("Send error № %d #%s\n", errno, zmq_strerror(errno));
                            }
                        }
                        lenBytes += MPD_EVENT_HEAD_WORDS * sizeof (UInt_t) + i;
                        //                        printf(" lenBytes %d \n", lenBytes);
                        msg_len -= lenBytes; //lenWords * kNBYTESINWORD;
                        //                        printf(" %d will move by %d bytes\n", msg_len, lenBytes);
                        memmove(&buf[0], &buf[lenBytes], msg_len);
                        i = 0;
                    }
                    evExit = kTRUE;
                    break;
                default:
                    break;
            }
            i++;
        }
        if (i >= MAX_BUF_LEN / kNBYTESINWORD) {
            printf("Wrong data, resetting array!\n");
            msg_len = 0;
        }
    }
    CloseStream();
}

void BmnOnlineDecoder::ProcessFileRun(TString rawFileName, UInt_t timeLimit) {
    Int_t iEv = 0;
    Int_t lastEv = -1;
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
            rawDataDecoder->DecodeDataToDigiIterate();
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
                printf("Send error № %d #%s\n", errno, zmq_strerror(errno));

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
            if (runID > 3800) {
                //                            if (regex_match(namelist[i]->d_name, re)) {
                if (runCount == 0) {
                    if (InitDecoder(_curDir + _curFile) == kBMNERROR)
                        continue;
                } else {
                    rawDataDecoder->ResetDecoder(_curDir + _curFile);
                    rawDataDecoder->SetRunId(runID);
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

ClassImp(BmnOnlineDecoder);
