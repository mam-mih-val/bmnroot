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
//    fRawDecoSocket = new TServerSocket(RAW_DECODER_SOCKET_PORT, kTRUE);
//    fRawDecoSocket->SetOption(kNoBlock, 1);
//    fRawDecoSocket->SetOption(kKeepAlive, 1);
    rawDataDecoder = NULL;
    iClients = 0;
    _ctx = NULL;


}

BmnOnlineDecoder::~BmnOnlineDecoder() {
//    fRawDecoSocket->Close();
    if (rawDataDecoder) delete rawDataDecoder;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
}

BmnStatus BmnOnlineDecoder::InitDecoder(Int_t periodID, Int_t runID, deque<UInt_t> *dq) {
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder();
    rawDataDecoder->SetRunId(runID);
    rawDataDecoder->SetPeriodId(periodID);
    rawDataDecoder->InitMaps();
    Bool_t setup[9]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 1; // MWPC
    setup[2] = 1; // SILICON
    setup[3] = 1; // GEM
    setup[4] = 1; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 1; // ZDC
    setup[8] = 0; // ECAL
    rawDataDecoder->SetDetectorSetup(setup);
    rawDataDecoder->SetTrigMapping("Trig_map_Run6.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_6.txt");
    rawDataDecoder->SetZDCMapping("ZDC_map_period_5.txt");
    rawDataDecoder->SetZDCCalibration("zdc_muon_calibration.txt");
    rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    rawDataDecoder->InitConverter(dq);
    return rawDataDecoder->InitDecoder();
}
BmnStatus BmnOnlineDecoder::InitDecoder(TString fRawFileName) {
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder();
    Int_t runID = 0;
    if (runID < 1) {
        printf("raw file %s\n", fRawFileName.Data());
        regex re(".*mpd_run_Glob_(\\d+).data");
        string idstr = regex_replace(fRawFileName.Data(), re, "$1");
        runID = atoi(idstr.c_str());
        if (runID == 0) {
            printf("!!! Error Could not detect runID\n");
            return kBMNERROR;
        }
    }
    rawDataDecoder->SetRunId(runID);
    rawDataDecoder->SetPeriodId(6);
    if (rawDataDecoder->InitMaps() == kBMNERROR){
        printf("InitMaps failed\n");
        delete rawDataDecoder;
        return kBMNERROR;
    }
    Bool_t setup[9]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 1; // MWPC
    setup[2] = 1; // SILICON
    setup[3] = 1; // GEM
    setup[4] = 1; // TOF-400
    setup[5] = 1; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 1; // ZDC
    setup[8] = 0; // ECAL
    rawDataDecoder->SetDetectorSetup(setup);
    rawDataDecoder->SetTrigMapping("Trig_map_Run6.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_6.txt");
    rawDataDecoder->SetZDCMapping("ZDC_map_period_5.txt");
    rawDataDecoder->SetZDCCalibration("zdc_muon_calibration.txt");
    rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    rawDataDecoder->InitConverter(fRawFileName);
    return rawDataDecoder->InitDecoder();
}

BmnStatus BmnOnlineDecoder::Accept() {
    if (clients.size() == MAX_CLIENTS)
        return kBMNSUCCESS;
    while (kTRUE) {
        //        if (iClients + 1 == MAX_CLIENTS)
        if (clients.size() == MAX_CLIENTS)
            break;
        client = fRawDecoSocket->Accept();
        if (client == (TSocket*) 0) {
            DBGERR("TServerSocket");
            return kBMNERROR;
        } else {
            if (client == (TSocket*) - 1)
                break;
            //            client->SetOption(kNoBlock, 1);
            clients.push_back(client);
            //            clients[iClients++] = client;
            printf("New connection accepted\n");
            client->Send("ready");
            // Check some options of socket 0.
            Int_t val;
            client->GetOption(kKeepAlive, val);
            printf("kKeepAlive: %d\n", val);
            client->GetOption(kNoBlock, val);
            printf("kNoBlock: %d\n", val);
            client->GetOption(kSendBuffer, val);
            printf("sendbuffer size: %d\n", val);
            client->GetOption(kRecvBuffer, val);
            printf("recvbuffer size: %d\n", val);
            // Get the remote addresses (informational only).
            TInetAddress adr = client->GetInetAddress();
            adr.Print();
            adr = client->GetLocalInetAddress();
            adr.Print();
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::DecodeStream() {
//    Accept();
    dataReceiver = new BmnDataReceiver();
    dataQue = dataReceiver->GetDataQueue();
    thread rcvThread(threadReceiveWrapper, dataReceiver);

    while (kTRUE) {

    }

    return kBMNSUCCESS;
}

void BmnOnlineDecoder::threadReceiveWrapper(BmnDataReceiver* dr) {
    dr->ConnectRaw();
}

BmnStatus BmnOnlineDecoder::OpenStream() {
    dataReceiver = new BmnDataReceiver();
    dataQue = dataReceiver->GetDataQueue();
    //    FILE *data_stream = istream_iterator<UInt_t>(data_queue);
    //    istream<UInt_t> qstream(data_queue);
    //    rdd->SetRawFileIn(data_stream);

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
    rcvBuf = MAX_BUF_LEN;
    if (zmq_setsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_decoSocket, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBuf = 0;
    if (zmq_getsockopt(_decoSocket, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBuf);
    if (zmq_bind(_decoSocket, "tcp://*:5555") != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
    _curFile = startFile;
    _curDir = dirname;
//    Accept();

    if (!runCurrent) {
        _curFile = "";
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    } else
        if (_curFile.Length() == 0) {
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }

    while (kTRUE) {
        InitDecoder(_curDir + _curFile);
        ProcessFileRun(_curDir + _curFile);
        rawDataDecoder->DisposeDecoder();
        delete rawDataDecoder;
        rawDataDecoder = NULL;
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        //        break; // @TODO Remove
    }
    zmq_close(_decoSocket);
    zmq_ctx_destroy(_ctx);
    _ctx = NULL;
    return kBMNSUCCESS;
}

void BmnOnlineDecoder::ProcessFileRun(TString rawFileName) {
    TMessage mess(kMESS_OBJECT);
    printf("File %s \n", TString(rawFileName).Data());
    Int_t iEv = 0;
    Int_t lastEv = 0;
    BmnStatus convertResult = kBMNSUCCESS;
    Int_t zflags;
    zflags = zflags | ZMQ_NOBLOCK;

    Int_t runId = -1;
    runId = rawDataDecoder->GetRunId();
    //    if (runId < 1216)
    //        return;
    printf("run id = %d\n", runId);

    UInt_t *buf = (UInt_t*) malloc(MAX_BUF_LEN);
    Int_t sendRes = 0;
    while (kTRUE) {
        convertResult = rawDataDecoder->ConvertRawToRootIterateFile();
        if (convertResult == kBMNFINISH) {
            printf("finish\n");
            break;
        }
        lastEv = iEv;
        iEv = rawDataDecoder->GetEventId();
        if (iEv > lastEv) {
//            Accept();
            rawDataDecoder->DecodeDataToDigiIterate();
            fEvents++;
            mess.Reset();
            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
            mess.WriteObject(&iterDigi);

            //            TBuffer t(TBuffer::kWrite);
            //            iterDigi.Streamer(t);
            TBufferFile t(TBuffer::kWrite);
            t.Reset();
            t.WriteObject(&iterDigi);
            //            iterDigi.Streamer(t);
            //            t.SetReadMode();
            //            sendRes = t.ReadBuf(buf, MAX_BUF_LEN);
            //            sendRes = zmq_send(_decoSocket, buf, sendRes, 0);
            sendRes = zmq_send(_decoSocket, t.Buffer(), t.Length(), 0);
            if (sendRes == -1) {
                printf("Send error № %d #%s\n", errno, zmq_strerror(errno));

            }
            //            zmq_send(_decoSocket, mess.Buffer(), mess.BufferSize(), zflags);


//            for (auto cl = begin(clients); cl != end(clients); cl++) {
//                //                Int_t sel = (*cl)->Select(2, 1); // kWrite == 2
//                ////                printf("select == %d\n", sel);
//                //                if (sel ==  - 1) { // timeout
//                //                    continue;
//                //                }
//                sendRes = (*cl)->Send(mess);
//                //                printf("sendRes == %d\n", sendRes);
//                if (sendRes == -1) {
//                    clients.erase(cl);
//                    cl--;
//                    DBGERR("TSocket Send")
//                    printf("Client disconnected\n");
//                }
//            }
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
        n = scandir(dirname, &namelist, 0, versionsort);
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
                delete event;
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
    if (zmq_bind(_decoSocket, "tcp://*:5555") != 0) {
        DBGERR("zmq bind")
        return kBMNERROR;
    }
//    Accept();
    struct dirent **namelist;
    regex re(".*mpd_run_Glob_(\\d+).data");
    Int_t runCount = 0;
    Int_t n;
    n = scandir(dirname, &namelist, 0, versionsort);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (regex_match(namelist[i]->d_name, re)) {
                _curFile = TString(namelist[i]->d_name);
                if (runCount == 0){
                    if (InitDecoder(_curDir + _curFile) == kBMNERROR)
                        continue;
                }else {
                    Int_t runID = 0;
                    if (runID < 1) {
                        string idstr = regex_replace(_curFile.Data(), re, "$1");
                        runID = atoi(idstr.c_str());
                        if (runID == 0) {
                            printf("!!! Error Could not detect runID\n");
                            break;
                        }
                    }

                    rawDataDecoder->ResetDecoder(_curDir + _curFile);
                    rawDataDecoder->SetRunId(runID);
                    rawDataDecoder->SetPeriodId(6);
                }
                ProcessFileRun(_curFile);
                runCount++;
                //                rawDataDecoder->DisposeDecoder();
                //                delete rawDataDecoder;
                //                rawDataDecoder = NULL;
            }
            free(namelist[i]);
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


ClassImp(BmnOnlineDecoder);
