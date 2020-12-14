/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   APRS_Connector.h
 * Author: paulalexander66
 *
 * Created on 10. Mai 2017, 19:33
 */

#ifndef APRS_CONNECTOR_H
#define APRS_CONNECTOR_H

#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <string.h>

//##############################################
//Change Variable here !!
#define m_PythonApp "Lora_APRS_gateway_6.py"
#define Python_Server "127.0.0.1"



//##############################################
using namespace std;

class APRS_Connector {
public:
    APRS_Connector();
    APRS_Connector(const APRS_Connector& orig);
    virtual ~APRS_Connector();
    int APRS_ping(string S_Server);
    int startAPRS_Connector(string PythonApp);
    int process_activ();
    int socket_activ();
    void setPort(int Port);
    int sendudp(char *msg, int length);
    int receiveUDP(char* Receiv,int Len);
    int proofACK(char* buffer, int size);
    string getIP_eth0();
    string getIP_wlan0();

private:
    std::string exec(const char* cmd);
    string m_Path;
    struct sockaddr_in si_other;
    int s;
    int slen;
    struct ifreq ifr;
    string m_Server;
    int m_Port;
    int m_isACK;
};

#endif /* APRS_CONNECTOR_H */

