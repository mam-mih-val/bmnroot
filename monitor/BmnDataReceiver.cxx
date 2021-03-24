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


#include "BmnDataReceiver.h"
#include <zmq.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include </usr/include/netdb.h>
#include <vector>
#include <thread>
#include <string>
#include "libxml/tree.h"
//for old fairsoft
#define ZMQ_XSUB 10
#define ZMQ_STREAM 11
#define ZMQ_ROUTER_RAW 41

BmnDataReceiver::BmnDataReceiver() {
    //gSystem->Load("libxml2");
    //    gSystem->Load("libzmq");
    InitSocks();
}

BmnDataReceiver::~BmnDataReceiver() {
    DeinitSocks();
}

void BmnDataReceiver::InitSocks() //:
//_ctx(1),
//_socket_mcast(_ctx, ZMQ_SUB)
{
    _ctx = zmq_ctx_new();
    _socket_mcast = zmq_socket(_ctx, ZMQ_XSUB);
    _socket_data = zmq_socket(_ctx, ZMQ_STREAM);
}

void BmnDataReceiver::DeinitSocks() {
    //_socket_mcast.close();
    zmq_close(_socket_mcast);
    zmq_close(_socket_data);
    zmq_ctx_destroy(_ctx);
}

/*void BmnDataReceiver::HandleSignal(int signal){
    switch (signal)
    {
        case SIGINT:
        {
            isListening = kFALSE;
            printf("SIGINT received\n");
            break;
        }
        default:
        {
            
        }    
    }
}*/

int BmnDataReceiver::ParsePNPMsg(Char_t* msgStr, serverInfo* sInfo) {
    xmlDocPtr doc = xmlParseDoc((xmlChar*) msgStr);
    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNodePtr cur_node = root;
    while (cur_node) {
        if ((cur_node->type == XML_ELEMENT_NODE) || (cur_node->type == XML_TEXT_NODE)) {
            if (strcmp((Char_t*) cur_node->name, "program") == 0) {
                strcpy(sInfo->hostName, (Char_t*) xmlGetProp(cur_node, (xmlChar*) "hostName"));
                strcpy(sInfo->index, (Char_t*) xmlGetProp(cur_node, (xmlChar*) "index"));
                strcpy(sInfo->name, (Char_t*) xmlGetProp(cur_node, (xmlChar*) "name"));
                strcpy(sInfo->type, (Char_t*) xmlGetProp(cur_node, (xmlChar*) "type"));
                cur_node = cur_node->children;
                continue;
            }
            if (strcmp((Char_t*) cur_node->name, "interfaces") == 0) {
                sInfo->interfaces.clear();
                cur_node = cur_node->children;
                continue;
            }
            if (strcmp((Char_t*) cur_node->name, "interface") == 0) {
                serverIface newPort;
                Char_t* portStr = (Char_t*) xmlGetProp(cur_node, (xmlChar*) "port");
                if (portStr != NULL)
                    newPort.port = atoi(portStr);
                newPort.isFree = ((Char_t*) xmlGetProp(cur_node, (xmlChar*) "isFree") == "1") ? 1 : 0;
                newPort.enabled = ((Char_t*) xmlGetProp(cur_node, (xmlChar*) "enabled") == "1") ? 1 : 0;
                strcpy(newPort.type, (Char_t*) xmlGetProp(cur_node, (xmlChar*) "type"));
                //if (newPort.type == "data flow")
                sInfo->interfaces.push_back(newPort);
                if (cur_node->next == NULL)
                    cur_node = cur_node->parent->next;
                else
                    cur_node = cur_node->next;
                continue;
            }
            cur_node = cur_node->next;
        }
    }
    xmlFreeDoc(doc);
    return 0;

}

int BmnDataReceiver::ConnectRaw() {
    DBG("started")
    socklen_t addrlen = 0;
    struct ip_mreq mreq;
    mreq.imr_interface.s_addr = htons(INADDR_ANY);
    mreq.imr_multiaddr.s_addr = inet_addr(PNP_DISCOVER_IP_ADDR);
    struct sockaddr_in mcast_addr;
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_port = htons(PNP_DISCOVER_PORT);
    mcast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&mcast_addr.sin_zero, 0, sizeof (mcast_addr.sin_zero));

    _sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sfd == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return -1;
    }
    UInt_t reusable = 1;
    if (setsockopt(_sfd, SOL_SOCKET, SO_REUSEADDR, &reusable, sizeof (reusable))) {
        close(_sfd);
        fprintf(stderr, "Setting reusable error: %s\n", strerror(errno));
        return -1;
    }
    addrlen = sizeof (mcast_addr);
    if (bind(_sfd, (sockaddr*) & mcast_addr, addrlen) == -1) {
        close(_sfd);
        fprintf(stderr, "Bind error: %s\n", strerror(errno));
        return -1;
    }
    if (setsockopt(_sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof (mreq))) {
        close(_sfd);
        fprintf(stderr, "Adding multicast group error: %s\n", strerror(errno));
        return -1;
    }
    Int_t nbytes;
    Char_t buf[MAX_BUF_LEN];
    //signal(SIGINT, HandleSignal);
    isListening = kTRUE;
    while (isListening) {
        if ((nbytes = recvfrom(_sfd, buf, MAX_BUF_LEN, 0, (sockaddr*) & mcast_addr, &addrlen)) == -1) {
            close(_sfd);
            fprintf(stderr, "Receive error: %s\n", strerror(errno));
            return -1;
        }
        buf[nbytes] = '\0';
        //        printf("%s\n", buf);
        ParsePNPMsg(buf, &_dataServer);
        printf("index = %s\n", _dataServer.index);
        printf("type  = %s\n", _dataServer.type);
        if ((strcmp(_dataServer.index, "Glob") == 0) && (_dataServer.interfaces.size() > 0)) {
            isAddr = true;
            DBG("Glob EvB found")
            break;
        }
    }
    RecvData();

    close(_sfd);
    return 0;
}

Int_t BmnDataReceiver::InitRecvStream() {
    //    Int_t isRaw = 1;
    //    if (zmq_setsockopt(_socket_data, ZMQ_ROUTER_RAW, &isRaw, sizeof(isRaw)) == -1)
    //        DBGERR("zmq_setsockopt of ZMQ_ROUTER_RAW")
    Char_t endpoint_addr[MAX_ADDR_LEN];
    struct addrinfo hints;
    struct addrinfo *info, *p;
    memset(&hints, 0, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    Int_t g = 0;
    if (g = getaddrinfo(_dataServer.hostName, NULL, &hints, &info) != 0) {
        printf("getaddrinfo error: %s\n", strerror(errno));
        return -1;
    }
    Char_t *ip = NULL;
    for (p = info; p != NULL; p = p->ai_next) {
        ip = inet_ntoa(((sockaddr_in*) (p->ai_addr))->sin_addr);
        printf("found family %d  %s addr: %s\n", p->ai_family, p->ai_canonname, ip);
        Int_t port = 0;
        for (auto iface : _dataServer.interfaces)
            if (strcmp(iface.type, "Monitor output data flow") == 0) {
                port = iface.port;
                break;
            }
        snprintf(endpoint_addr, MAX_ADDR_LEN, "tcp://%s:%d", ip, port);
        if (zmq_connect(_socket_data, endpoint_addr) != 0) {
            DBGERR("zmq connect")
            continue;
        } else {
            printf("%s\n", endpoint_addr);
            break;
        }
    }
    if (p == NULL) {
        printf("Valid address not found\n");
        return -1;
    }
    Int_t rcvBuf = MAX_BUF_LEN;
    size_t vl = sizeof (rcvBuf);
    if (zmq_setsockopt(_socket_data, ZMQ_RCVBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_socket_data, ZMQ_SNDBUF, &rcvBuf, sizeof (rcvBuf)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBuf = 0;
    if (zmq_getsockopt(_socket_data, ZMQ_RCVBUF, &rcvBuf, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBuf);
    Char_t id[MAX_ADDR_LEN];
    
    return 0;
}

void BmnDataReceiver::FreeRecvStream(){
    freeaddrinfo(dataAddrInfo);
}

Int_t BmnDataReceiver::IterRecvStream(){
//    while ((isListening) && (msg_len < MAX_BUF_LEN)) {
//        id_size = zmq_recv(_socket_data, &id, sizeof (id), 0);
//        if (id_size == -1) {
//            printf("Receive error #%s\n", zmq_strerror(errno));
//            if (errno == EAGAIN)
//                usleep(MSG_TIMEOUT);
//            else
//                return -1;
//        } else {
//            printf("ID size =  %d\n Id:%x\n", id_size, id);
//        }
//        zmq_msg_t msg;
//        zmq_msg_init(&msg);
//        Int_t recv_more = 0;
//        UInt_t *msgPtr;
//        do {
//            frame_size = zmq_msg_recv(&msg, _socket_data, 0); // ZMQ_DONTWAIT
//            //frame_size = zmq_recv(_socket_data, buf, MAX_BUF_LEN, 0);
//            if (frame_size == -1) {
//                printf("Receive error № %d #%s\n", errno, zmq_strerror(errno));
//                if (errno == EAGAIN)
//                    usleep(MSG_TIMEOUT);
//                else
//                    return -1;
//            } else {
//                //                UChar_t *str = (UChar_t*) malloc((frame_size + 1) * sizeof (UChar_t));
//                msgPtr = (UInt_t*) zmq_msg_data(&msg);
//                //                memcpy(buf, zmq_msg_data(&msg), frame_size);
//                //                ((mutex*)_deque_mutex)->lock();
//                for (Int_t offset = 0; offset < frame_size; offset++)
//                    data_queue.push_back(*(msgPtr + offset));
//                //                ((mutex*)_deque_mutex)->unlock();
//                //                memcpy(str, zmq_msg_data(&msg), frame_size);
//                //                str[frame_size] = '\0';
//                msg_len += frame_size;
//                //                printf("Frame size =  %d\n Msg:%x\n", frame_size, str);
//                //                free(str);
//            }
//            size_t opt_size = sizeof (recv_more);
//            if (zmq_getsockopt(_socket_data, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
//                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
//                return -1;
//            }
//            printf("ZMQ rcvmore = %d\n", recv_more);
//
//            zmq_msg_close(&msg);
//        } while (recv_more);
//    }
    return 0;
}

Int_t BmnDataReceiver::RecvData() {
    //    Int_t isRaw = 1;
    //    if (zmq_setsockopt(_socket_data, ZMQ_ROUTER_RAW, &isRaw, sizeof(isRaw)) == -1)
    //        DBGERR("zmq_setsockopt of ZMQ_ROUTER_RAW")
    Char_t endpoint_addr[MAX_ADDR_LEN];
    struct addrinfo hints;
    struct addrinfo *p;
    memset(&hints, 0, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    Int_t g = 0;
    if (g = getaddrinfo(_dataServer.hostName, NULL, &hints, &dataAddrInfo) != 0) {
        printf("getaddrinfo error: %s\n", strerror(errno));
        return -1;
    }
    Char_t *ip = NULL;
    for (p = dataAddrInfo; p != NULL; p = p->ai_next) {
        ip = inet_ntoa(((sockaddr_in*) (p->ai_addr))->sin_addr);
        printf("found family %d  %s addr: %s\n", p->ai_family, p->ai_canonname, ip);
        Int_t port = 0;
        for (auto iface : _dataServer.interfaces)
            if (strcmp(iface.type, "Monitor output data flow") == 0) {
                port = iface.port;
                break;
            }
        snprintf(endpoint_addr, MAX_ADDR_LEN, "tcp://%s:%d", ip, port);
        if (zmq_connect(_socket_data, endpoint_addr) != 0) {
            DBGERR("zmq")
            continue;
        } else {
            printf("%s\n", endpoint_addr);
            break;
        }
    }
    if (p == NULL) {
        printf("Valid address not found\n");
        return -1;
    }
    Int_t rcvBufLen = MAX_BUF_LEN;
    size_t vl = sizeof (rcvBufLen);
    if (zmq_setsockopt(_socket_data, ZMQ_RCVBUF, &rcvBufLen, sizeof (rcvBufLen)) == -1)
        DBGERR("zmq_setsockopt of ZMQ_RCVBUF")
        if (zmq_setsockopt(_socket_data, ZMQ_SNDBUF, &rcvBufLen, sizeof (rcvBufLen)) == -1)
            DBGERR("zmq_setsockopt of ZMQ_SNDBUF")
            rcvBufLen = 0;
    if (zmq_getsockopt(_socket_data, ZMQ_RCVBUF, &rcvBufLen, &vl) == -1)
        DBGERR("zmq_getsockopt of ZMQ_RCVBUF")
        printf("rcvbuf = %d\n", rcvBufLen);
//    UInt_t *buf = (UInt_t*) malloc(MAX_BUF_LEN);
    Char_t conID[MAX_ADDR_LEN];
    Int_t conID_size;
    Int_t msg_len = 0;
    Int_t frame_size = 0;
    isListening = kTRUE;
    while ((isListening) && (msg_len < MAX_BUF_LEN)) {
        conID_size = zmq_recv(_socket_data, &conID, sizeof (conID), 0);
        if (conID_size == -1) {
            printf("Receive error #%s\n", zmq_strerror(errno));
            if (errno == EAGAIN)
                usleep(MSG_TIMEOUT);
            else
                return -1;
        } else {
            printf("ID size =  %d\n Id:%s\n", conID_size, conID);
        }
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        Int_t recv_more = 0;
        UInt_t *msgPtr;
        do {
            frame_size = zmq_msg_recv(&msg, _socket_data, 0); // ZMQ_DONTWAIT
            //frame_size = zmq_recv(_socket_data, buf, MAX_BUF_LEN, 0);
            if (frame_size == -1) {
                printf("Receive error № %d #%s\n", errno, zmq_strerror(errno));
                if (errno == EAGAIN)
                    usleep(MSG_TIMEOUT);
                else
                    return -1;
            } else {
                //                UChar_t *str = (UChar_t*) malloc((frame_size + 1) * sizeof (UChar_t));
                msgPtr = (UInt_t*) zmq_msg_data(&msg);
                //                memcpy(buf, zmq_msg_data(&msg), frame_size);
                //                ((mutex*)_deque_mutex)->lock();
                for (Int_t offset = 0; offset < frame_size; offset++)
                    data_queue.push_back(*(msgPtr + offset));
                //                ((mutex*)_deque_mutex)->unlock();
                //                memcpy(str, zmq_msg_data(&msg), frame_size);
                //                str[frame_size] = '\0';
                msg_len += frame_size;
                //                printf("Frame size =  %d\n Msg:%x\n", frame_size, str);
                //                free(str);
            }
            size_t opt_size = sizeof (recv_more);
            if (zmq_getsockopt(_socket_data, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                return -1;
            }
            printf("ZMQ rcvmore = %d\n", recv_more);

            zmq_msg_close(&msg);
        } while (recv_more);
    }
//    free(buf);
    freeaddrinfo(dataAddrInfo);
    return 0;
}

int BmnDataReceiver::Connect() {
    Char_t endpoint_addr[MAX_ADDR_LEN];
    snprintf(endpoint_addr, MAX_ADDR_LEN, "epgm://%s;%s:%d", INPUT_IFACE, PNP_DISCOVER_IP_ADDR, PNP_DISCOVER_PORT);
    Int_t rc = 0;
    rc = zmq_connect(_socket_mcast, endpoint_addr);
    if (rc != 0)
        printf("Error: %s\n", zmq_strerror(errno));
    else
        printf("%s\n", endpoint_addr);
    Char_t * buf = (Char_t*) malloc(255);
    Int_t frame_size = 0;
    for (Int_t i = 0; i < 10; i++) {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        Int_t recv_more = 0;
        do {
            frame_size = zmq_msg_recv(&msg, _socket_mcast, 0);
            //frame_size = zmq_recv(_socket_mcast, buf, 255, 0);
            if (frame_size == -1) {
                printf("Receive error #%s\n", zmq_strerror(errno));
                break;
            } else {
                Char_t *str = (Char_t*) malloc((frame_size + 1) * sizeof (Char_t));
                memcpy(str, zmq_msg_data(&msg), frame_size);
                str[frame_size] = '\0';
                printf("Frame size =  %d\n Msg:%s\n", frame_size, str);
            }
            size_t opt_size = sizeof (Int_t);
            if (zmq_getsockopt(_socket_mcast, ZMQ_RCVMORE, &recv_more, &opt_size) == -1) {
                printf("ZMQ socket options error #%s\n", zmq_strerror(errno));
                break;
            }



        } while (recv_more);
        printf("Received msg # %d\n", i);
        zmq_msg_close(&msg);
    }
    return 0;
}

int BmnDataReceiver::SendHello() {
    void * sender = zmq_socket(_ctx, ZMQ_XPUB);
    const int maxlen = 255;
    Char_t s[maxlen];
    snprintf(s, maxlen, "epgm://%s;%s:%d", INPUT_IFACE, PNP_DISCOVER_IP_ADDR, PNP_DISCOVER_PORT); //enp3s0
    Int_t rc = 0;
    //_socket_mcast.connect(s);
    rc = zmq_bind(sender, s);
    printf("%s\n", s);
    if (rc != 0)
        printf("Error: %s\n", zmq_strerror(errno));

    Char_t text[11] = "Hello port";
    int len = strlen(text);
    text[len] = '\0';
    for (int i = 0; i < 5; i++) {
        zmq_msg_t msg;
        zmq_msg_init_size(&msg, len);
        memcpy(zmq_msg_data(&msg), text, len);
        rc = zmq_msg_send(&msg, sender, 0);
        if (rc == -1)
            printf("Send error: %s\n", zmq_strerror(errno));
        else
            printf("Sended bytes: %d\n", rc);
        zmq_msg_close(&msg);
        usleep(1000000);
    }
    zmq_close(sender);

    return 0;
}

ClassImp(BmnDataReceiver);
