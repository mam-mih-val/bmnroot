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

#include <thread>
#include <dirent.h>

#include "BmnOnlineDecoder.h"

BmnOnlineDecoder::BmnOnlineDecoder() {
    fRawDecoSocket = new TServerSocket(RAW_DECODER_SOCKET_PORT, kTRUE);
    
    
}

BmnOnlineDecoder::~BmnOnlineDecoder() {
    fRawDecoSocket->Close();
    delete rawDataDecoder;
}

void BmnOnlineDecoder::InitDecoder() {
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder();
    rawDataDecoder->SetRunId(1000);
    rawDataDecoder->SetPeriodId(5);
    rawDataDecoder->SetTrigMapping("Trig_map_Run5.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_Period5.txt", "TOF400_StripMap_Period5.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_5.txt");
    rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    rawDataDecoder->InitConverter();
    rawDataDecoder->InitDecoder();
    
}

void BmnOnlineDecoder::InitDecoder(TString file) {
    DBG("started")
    rawDataDecoder = new BmnRawDataDecoder(file, 0, 5);
    rawDataDecoder->SetTrigMapping("Trig_map_Run5.txt");
    rawDataDecoder->SetTrigINLFile("TRIG_INL.txt");
    rawDataDecoder->SetTof400Mapping("TOF400_PlaceMap_Period5.txt", "TOF400_StripMap_Period5.txt");
    rawDataDecoder->SetTof700Mapping("TOF700_map_period_5.txt");
    rawDataDecoder->SetMwpcMapping("MWPC_mapping_period_5.txt");
    rawDataDecoder->InitConverter();
    rawDataDecoder->InitDecoder();
    
}

BmnStatus BmnOnlineDecoder::Accept(){
    client = fRawDecoSocket->Accept();
    if (client == NULL){
        DBGERR("TServerSocket")
        return kBMNERROR;
    }else{
        printf("New connection accepted\n");
    }
    client->Send("ready");
   // Check some options of socket 0.
   int val;
   client->GetOption(kSendBuffer, val);
   printf("sendbuffer size: %d\n", val);
   client->GetOption(kRecvBuffer, val);
   printf("recvbuffer size: %d\n", val);
   // Get the remote addresses (informational only).
   TInetAddress adr = client->GetInetAddress();
   adr.Print();
   adr = client->GetLocalInetAddress();
   adr.Print();
   return kBMNSUCCESS;
}

BmnStatus BmnOnlineDecoder::Decode(TString dirname, TString startFile, Bool_t runCurrent){
    DBG("started")
    _curFile = startFile;
    _curDir = dirname;
    Accept();
   
    if (!runCurrent) {
        _curFile = "";
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    } else
        if (_curFile.Length() == 0) {
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
        }
    InitDecoder(_curDir + _curFile);

    while (kTRUE) {
        ProcessFileRun(_curFile);
        _curFile = WatchNext(_curDir, _curFile, RUN_FILE_CHECK_PERIOD);
    }
    client->Close();
    delete client;
    return kBMNSUCCESS;
}


void BmnOnlineDecoder::ProcessFileRun(TString rawFileName) {
   TMessage::EnableSchemaEvolutionForAll(kFALSE);
   TMessage mess(kMESS_OBJECT);
    printf("File %s \n", TString(_curDir + rawFileName).Data());
    Int_t iEv = 0;
    Int_t lastEv = 0;
    TString nextFile;
    BmnStatus convertResult = kBMNSUCCESS;

    const UInt_t kRUNNUMBERSYNC = 0x236E7552;
    const size_t kWORDSIZE = sizeof (UInt_t);
    const Short_t kNBYTESINWORD = 4;
    Int_t runId = -1;
    FILE * file = fopen(TString(_curDir + rawFileName).Data(), "rb");
    if (file == NULL) {
        printf("File %s is not open!!!\n", TString(_curDir + rawFileName).Data());
        return;
    }
    UInt_t word;
    while (fread(&word, kWORDSIZE, 1, file)) {
        if (word == kRUNNUMBERSYNC) {
            fread(&word, kWORDSIZE, 1, file); //skip word
            fread(&runId, kWORDSIZE, 1, file);
            break;
        }
    }
    fclose(file);
    printf("run id = %d\n", runId);
    rawDataDecoder->ResetDecoder(_curDir + rawFileName);    

    while (kTRUE) {
        convertResult = rawDataDecoder->ConvertRawToRootIterateFile();
        if (convertResult == kBMNFINISH) {
            printf("finish\n");
            //_curFile = "";
            break;
        }
        lastEv = iEv;
        iEv = rawDataDecoder->GetEventId();
        if (iEv > lastEv) {
            rawDataDecoder->DecodeDataToDigiIterate();
//            ProcessDigi(iEv);
            mess.Reset();
            DigiArrays iterDigi = rawDataDecoder->GetDigiArraysObject();
//            TH1F *qq = new TH1F("qq", "qq", 100, 0, 100);
//            mess.WriteObject(qq);
//            TBuffer:: t(TBuffer::kWrite);
//            iterDigi.Streamer(t);
//            printf("buf len %d\n", t.BufferSize());
//            iterDigi.header->BypassStreamer(kFALSE);
//            iterDigi.bc1->BypassStreamer(kFALSE);
//            iterDigi.bc2->BypassStreamer(kFALSE);
//            iterDigi.bd->BypassStreamer(kFALSE);
//            iterDigi.dch->BypassStreamer(kFALSE);
//            iterDigi.fd->BypassStreamer(kFALSE);
//            iterDigi.gem->BypassStreamer(kFALSE);
//            iterDigi.mwpc->BypassStreamer(kFALSE);
//            iterDigi.t0->BypassStreamer(kFALSE);
//            iterDigi.tof400->BypassStreamer(kFALSE);
//            iterDigi.tof700->BypassStreamer(kFALSE);
//            iterDigi.veto->BypassStreamer(kFALSE);
            mess.WriteObject(&iterDigi);
            client->Send(mess);
//            delete iterDigi;
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

BmnStatus BmnOnlineDecoder::BatchDirectory(TString dirname) {
    _curDir = dirname;
    Accept();
    struct dirent **namelist;
    regex re("\\w+\\.data");
    Int_t n;
    n = scandir(dirname, &namelist, 0, versionsort);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (regex_match(namelist[i]->d_name, re)) {
                _curFile = TString(namelist[i]->d_name);
                break;
            }
        }
    }

    InitDecoder(_curDir + _curFile);
    n = scandir(dirname, &namelist, 0, versionsort);
    if (n < 0) {
        perror("scandir");
        return kBMNERROR;
    } else {
        for (Int_t i = 0; i < n; ++i) {
            if (regex_match(namelist[i]->d_name, re))
                ProcessFileRun(TString(namelist[i]->d_name));
            free(namelist[i]);
        }
        free(namelist);
    }
    client->Close();
    delete client;
    return kBMNSUCCESS;
}


ClassImp(BmnOnlineDecoder);
