/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnDataReceiver.h
 * Author: ilnur
 *
 * Created on October 18, 2016, 5:22 PM
 */

#ifndef BMNDATARECEIVER_H
#define BMNDATARECEIVER_H 1
#include <stdlib.h>
#include <deque>
#include <TNamed.h>

#include <BmnMath.h>

#define PNP_DISCOVER_PORT  33304
#define PNP_DISCOVER_IP_ADDR "239.192.1.2"
#define INPUT_IFACE "eno1"
#define MAX_BUF_LEN 32 * 1024 * 1024//1048576
#define MAX_ADDR_LEN 255
#define MAX_PORT_LEN 7
#define MSG_TIMEOUT       400000
#define CONN_TIMEOUT     3000000

using namespace std;

class BmnDataReceiver: public TNamed {
public:
    BmnDataReceiver();
    virtual ~BmnDataReceiver();
    
    Int_t ConnectRaw();
    Int_t Connect();
    Int_t SendHello();
    Int_t InitRecvStream();
    void  FreeRecvStream();
    Int_t IterRecvStream();
    Int_t RecvData();

    void SetDataQueue(deque<UInt_t> v) {
        this->data_queue = v;
    }

    deque<UInt_t>* GetDataQueue() {
        return &data_queue;
    }
    
//    void *GetQueMutex(){ return _deque_mutex;}
//    void SetQueMutex(void *v){_deque_mutex = v;}
    
private:
    //zmq::context_t _ctx;
    //zmq::socket_t _socket_mcast;
    struct serverIface
    {
        Bool_t enabled;
        Bool_t isFree;
        Int_t id;
        UShort_t port;
        Char_t type[MAX_ADDR_LEN];
    };
    struct serverInfo
    {
        Char_t hostName[MAX_ADDR_LEN];
        Char_t index[MAX_ADDR_LEN];
        Char_t name[MAX_ADDR_LEN];
        Char_t type[MAX_ADDR_LEN];
        vector<serverIface> interfaces;
    };
    
    deque<UInt_t> data_queue;
    void * _ctx;
    void * _socket_mcast;
    void * _socket_data;
    Int_t _sfd;
    struct serverInfo _dataServer;
    struct addrinfo *dataAddrInfo;
    Bool_t isListening;
    Bool_t isAddr = kFALSE;
//    void *_deque_mutex; // actually std::mutex
    //static void HandleSignal(int signal);
    void InitSocks();
    void DeinitSocks();
    static Int_t ParsePNPMsg(char *msgStr, serverInfo *sInfo);
    
    
    
    ClassDef(BmnDataReceiver, 1)

};

#endif /* BMNDATARECEIVER_H */

