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
#include <../zmq.h>
#include <TNamed.h>
//#include <netinet/udp.h>

class BmnDataReceiver: public TNamed {
public:
    BmnDataReceiver();
    virtual ~BmnDataReceiver();
    
    int ConnectRaw();
    int Connect();
    int SendHello();
private:
    //zmq::context_t _ctx;
    //zmq::socket_t _socket_mcast;
    void * _ctx;
    void * _socket_mcast;
    
    
    
    ClassDef(BmnDataReceiver, 1)

};

#endif /* BMNDATARECEIVER_H */

