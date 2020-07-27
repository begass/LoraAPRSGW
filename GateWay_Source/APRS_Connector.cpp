/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   APRS_Connector.cpp
 * Author: paulalexander66
 * 
 * Created on 10. Mai 2017, 19:33
 */

#include <valarray>

#include "APRS_Connector.h"

using namespace std;

APRS_Connector::APRS_Connector() {
    m_Path = "";
    slen=sizeof(si_other);
    m_Port=0;
    m_isACK=0;
}

APRS_Connector::APRS_Connector(const APRS_Connector& orig) {
}

APRS_Connector::~APRS_Connector() {
}

//Setting the Port and Server of the Python App
void APRS_Connector::setPort(int Port){
    m_Port=Port;
}

int APRS_Connector::sendudp(char *msg, int length) {
    //send the update
    inet_aton(Python_Server , &si_other.sin_addr);
    if (sendto(s, (char *)msg, length, 0 , (struct sockaddr *) &si_other, slen)==-1)
    {
        return -1;
    }
    return 0;
}

/* Return 
  0  success
 -1  failure
  1  No Error but did not receive paket
 */
int APRS_Connector::receiveUDP(char* Receiv,int Len){
    //printf("in receiveUDP\n");
    int iRec=-1;
    //int iRec=recv (int __fd, void *__buf, size_t __n, int __flags)
    try{
        iRec=recv(s, Receiv, Len, MSG_DONTWAIT);
    }catch(...) {
        //printf("Error recceiving from UDP \n");
        iRec=-1;
    }
    //printf("Received Return Code %d \n",iRec);
    if(iRec>0){
        //printf("Received message = %s \n", Receiv);
        iRec=0;
    }else{
        //printf("Received message = 0 !! But no Error");
        iRec=1;
    }
    return iRec;
}


int APRS_Connector::proofACK(char* buffer, int size){
    int iReturn=0;
    char Temp[size];
    if (buffer[0]=='1'){
        int i;
        for (i=1;i<size;i++){
            Temp[i-1]=buffer[i];
        }
        Temp[i-1]='\0';
        //printf("new string %s\n",Temp);
        strcpy(buffer,Temp);
        iReturn=1;
    }
    
    return iReturn;
}

/*Pings the Aprs server 
 * Return 
 0  success
 -1 failure
 1   connection is there but some packets were lost
 */
int APRS_Connector::APRS_ping(string S_Server) {
    int iReturn = -1;
    //ping -c 4 aprs.fi
    string s_ping = "ping -c 4 ";
    s_ping.append(S_Server);
    try {
        const string ping_Result = exec(s_ping.c_str());
        //printf("ping Result = %s",ping_Result.c_str());
        int Beg_iFound = ping_Result.find("received,") + 9;
        int End_iFound = ping_Result.find(" packet loss,");
        int iError = ping_Result.find(" +4 errors, ");
        if (iError > 0) {
            //printf("%s is NOT availibel\n", S_Server.c_str());
            iReturn = -1;
        } else {
            string sValue = ping_Result.substr(Beg_iFound, End_iFound - Beg_iFound);

            //printf("ping Strip Result = %s\n",sValue.c_str());
            if (sValue.compare(" 0%") == 0) {
                //printf("%s is availibel \n", S_Server.c_str());
                iReturn = 0;
            } else if (sValue.compare(" 100%") == 0) {
                //printf("%s is NOT availibel\n", S_Server.c_str());
                iReturn = -1;
            } else {
                printf("%s availibel with %s packet loss\n", S_Server.c_str(), sValue.c_str());
                iReturn = 1;
            }
        }
    } catch (...) {
        printf("Error ping Server %s \n", S_Server.c_str());
    }

    return iReturn;
}

/*Starting Aprs Connector
 * Return 
  0  success
 -1  failure
 */
int APRS_Connector::startAPRS_Connector(string PythonApp) {
    int iReturn = -1;
    //string s_pwd = "pwd";
    //const string ping_Result = exec(s_pwd.c_str());
    //int iLen = ping_Result.length();
    //printf("pwd Result = %s", ping_Result.c_str());
    m_Path.append("python ");
    //m_Path.append(ping_Result.substr(0, iLen - 1));
    //m_Path.append("/");
    //m_Path.append(m_PythonApp);
    m_Path.append(PythonApp);
    m_Path.append(" &");
    //printf("Path to python app %s \n", m_Path.c_str());
    
    if(m_Port==0){
        //printf("Port of APRS Connector not set \n");
        return -1;
    }
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(m_Port);

    try {
        int a = system(m_Path.c_str());
        if(a < 0){
            iReturn=-1;
        }else{
            //printf("APRS Connector started\n");
            iReturn = 0;
        }
    } catch (...) {
        //printf("Error starting APRS Connector \n");
        iReturn = -1;
    }
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        iReturn=-1;
    }
    
    return iReturn;
}

/*is  Aprs Connector python app running 
 * Return 
  0  success
 -1  failure
 */
int APRS_Connector::process_activ(){
    int iReturn=-1;
    string s_process = "ps aux | grep python";
    const string process_Result = exec(s_process.c_str());
    //int iLen = process_Result.length();
    //printf("process Result = %s", process_Result.c_str());
    //printf("Path search process = %s\n",m_Path.substr(0,m_Path.length()-2).c_str());
    int iFound =process_Result.find(m_Path.substr(0,m_Path.length()-2));
    if (iFound >0 ){
        iReturn =0;
    }
    return iReturn;
}

std::string APRS_Connector::exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

string APRS_Connector::getIP_eth0(){
    string eth0="";
    string s_process = "ifconfig eth0";
    const string process_Result = exec(s_process.c_str());
    //printf("%s \n",process_Result.c_str());
    try{
        
        int iTest=process_Result.find(" inet addr:");
        //printf("Result Test %d \n",iTest);
        if(iTest<0){return eth0;} //No Ip Address found
        int Beg_iFound = process_Result.find(" inet addr:") + 11;
        int End_iFound = process_Result.find(" Bcast:");
        string sValue = process_Result.substr(Beg_iFound, End_iFound - Beg_iFound);
        //printf("Result %s \n",sValue.c_str());
        return sValue;
    }catch (...) {
        printf("Error reading eth0 \n");
    }
    return eth0;
}

string APRS_Connector::getIP_wlan0(){
    string eth0="";
    string s_process = "ifconfig wlan0";
    const string process_Result = exec(s_process.c_str());
    //printf("%s \n",process_Result.c_str());
    try{
        int iTest=process_Result.find(" inet ");
        //printf("Result Test %d \n",iTest);
        if(iTest<0){return eth0;} //No Ip Address found
        int Beg_iFound = process_Result.find(" inet ") + 6;
        int End_iFound = process_Result.find(" netmask");
        string sValue = process_Result.substr(Beg_iFound, End_iFound - Beg_iFound);
        printf("Wlan0 Result %s \n",sValue.c_str());
        return sValue;
    }catch (...) {
        printf("Error reading wlan0 \n");
    }
    return eth0;
}
