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
    rawDataDecoder = NULL;
    _ctx = NULL;
    fBmnSetup = kBMNSETUP;
}

BmnOnlineDecoder::~BmnOnlineDecoder() {
    if (rawDataDecoder) delete rawDataDecoder;
    if (_ctx) {
        zmq_ctx_destroy(_ctx);
        _ctx = NULL;
    }
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
        //        runID = 1234; // @TODO remove
        if (runID < 0) {
            printf("!!! Error Could not detect runID\n");
            return kBMNERROR;
        }
        rawDataDecoder->SetRunId(runID);
    }
    rawDataDecoder->SetPeriodId(6);
    /*if (rawDataDecoder->InitMaps() == kBMNERROR) {
        printf("InitMaps failed\n");
        delete rawDataDecoder;
        return kBMNERROR;
    }*/
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
    rawDataDecoder->SetDetectorSetup(setup);
    rawDataDecoder->SetBmnSetup(fBmnSetup);
    rawDataDecoder->SetTrigMapping((rawDataDecoder->GetBmnSetup() == kBMNSETUP) ? "Trig_map_Run6.txt" : "Trig_map_Run7_SRC.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_RUN6.txt", "TOF400_StripMap_RUN6.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_6.txt");
    rawDataDecoder->SetZDCMapping("ZDC_map_period_5.txt");
    rawDataDecoder->SetZDCCalibration("zdc_muon_calibration.txt");
    rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    rawDataDecoder->SetECALMapping("ECAL_map_period_5.txt");
    rawDataDecoder->SetLANDMapping("land_mapping_jinr_triplex.txt");
    rawDataDecoder->SetLANDPedestal("r0030_land_clock.hh");
    rawDataDecoder->SetLANDTCal("r0030_land_tcal.hh");
    rawDataDecoder->SetLANDDiffSync("r352_cosmic1.hh");
    rawDataDecoder->SetLANDVScint("neuland_sync_2.txt");
    rawDataDecoder->InitConverter(fRawFileName);
    return rawDataDecoder->InitDecoder();
}

BmnStatus BmnOnlineDecoder::DecodeStream() {
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

    return kBMNSUCCESS;
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
            if (iterDigi.header->GetEntriesFast() == 0)
                continue;
            BmnEventHeader* head = (BmnEventHeader*) iterDigi.header->At(0);
            if (head->GetType() != kBMNPAYLOAD)
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
            if (runID > 0) {
                //            if (regex_match(namelist[i]->d_name, re)) {
                if (runCount == 0) {
                    if (InitDecoder(_curDir + _curFile) == kBMNERROR)
                        continue;
                } else {
                    rawDataDecoder->ResetDecoder(_curDir + _curFile);
                    rawDataDecoder->SetRunId(runID);
                    rawDataDecoder->SetPeriodId(6);
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
