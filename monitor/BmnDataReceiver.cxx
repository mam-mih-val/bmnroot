/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BmnDataReceiver.cxx
 * Author: ilnur
 * 
 * Created on October 18, 2016, 5:22 PM
 */

#define PNP_DISCOVER_PORT  33304
#define PNP_DISCOVER_IP_ADDR "239.192.1.2"
#define INPUT_IFACE "enp3s0"
 // "224.0.1.38"
#include "BmnDataReceiver.h"

BmnDataReceiver::BmnDataReceiver() //:
//_ctx(1),
//_socket_mcast(_ctx, ZMQ_SUB)
{
    _ctx = zmq_ctx_new();
    _socket_mcast = zmq_socket(_ctx, ZMQ_XSUB);
}

BmnDataReceiver::~BmnDataReceiver() {
    //_socket_mcast.close();
    zmq_close(_socket_mcast);
    zmq_ctx_destroy(_ctx);
}

int BmnDataReceiver::Connect(){
    const int maxlen = 255;
    char endpoint_addr[maxlen];
    snprintf(endpoint_addr, maxlen, "epgm://%s;%s:%d", INPUT_IFACE, PNP_DISCOVER_IP_ADDR, PNP_DISCOVER_PORT);
    Int_t rc = 0;
    rc = zmq_connect(_socket_mcast, endpoint_addr);
    printf("%s\n", endpoint_addr);
    if (rc != 0)
        printf("Error: %s\n", zmq_strerror (errno));
    char * buf = (char*)malloc(255);
    Int_t frame_size = 0;
    for (Int_t i = 0; i < 10; i++){
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        Int_t recv_more = 0;
        do {
            frame_size = zmq_msg_recv(&msg, _socket_mcast, 0);
            //frame_size = zmq_recv(_socket_mcast, buf, 255, 0);
            if (frame_size == -1){
                printf("Receive error #%s\n", zmq_strerror (errno));
                break;
            } else {
                char *str = (char*)malloc ((frame_size + 1) * sizeof(char));
                memcpy(str, zmq_msg_data(&msg), frame_size);
                str[frame_size] = '\0';
                printf("Frame size =  %d\n Msg:%s\n", frame_size, str);
            }
            size_t opt_size = sizeof(Int_t);
            if (zmq_getsockopt(_socket_mcast, ZMQ_RCVMORE, &recv_more, &opt_size) == -1){
                printf("ZMQ socket options error #%s\n", zmq_strerror (errno));
                break;
            }
            
            
            
        } while (recv_more);
        printf("Received msg # %d\n", i);
        zmq_msg_close(&msg);
    }    
    return 0;
}

int BmnDataReceiver::SendHello()
{
    void * sender = zmq_socket(_ctx, ZMQ_XPUB);
    const int maxlen = 255;
    char s[maxlen];
    snprintf(s, maxlen, "epgm://%s;%s:%d", INPUT_IFACE, PNP_DISCOVER_IP_ADDR, PNP_DISCOVER_PORT);//enp3s0
    Int_t rc = 0;
    //_socket_mcast.connect(s);
    rc = zmq_bind(sender, s);
    printf("%s\n", s);
    if (rc != 0)
        printf("Error: %s\n", zmq_strerror (errno));
    
    char text[11] = "Hello port";
    int len = strlen(text);
    text[len] = '\0';
    for (int i = 0; i < 5; i++){
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, len);
        memcpy(zmq_msg_data(&msg), text, len);
        rc = zmq_msg_send(&msg, sender, 0);
        if (rc == -1)
            printf("Send error: %s\n", zmq_strerror (errno));
        else
            printf("Sended bytes: %d\n", rc);
        zmq_msg_close(&msg);
        usleep(1000000);
    }
    zmq_close(sender);
    
    return 0;
}

ClassImp(BmnDataReceiver);
