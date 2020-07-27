/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HopeRF.h
 * Author: paulalexander66
 *
 * Created on 11. Mai 2017, 08:56
 */

#ifndef HOPERF_H
#define HOPERF_H

#include <stdint.h>
#include <string>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include "ParamList.h"

#define REG_FIFO                    0x00
#define REG_FIFO_ADDR_PTR           0x0D
#define REG_PA_CONFIG               0x09

#define REG_FIFO_TX_BASE_AD         0x0E
#define REG_FIFO_RX_BASE_AD         0x0F
#define REG_RX_NB_BYTES             0x13
#define REG_OPMODE                  0x01
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_DIO_MAPPING_1           0x40
#define REG_DIO_MAPPING_2           0x41
#define REG_MODEM_CONFIG            0x1D
#define REG_MODEM_CONFIG2           0x1E
#define REG_MODEM_CONFIG3           0x26
#define REG_SYMB_TIMEOUT_LSB  		0x1F
#define REG_PKT_SNR_VALUE			0x19
#define REG_PAYLOAD_LENGTH          0x22
#define REG_IRQ_FLAGS_MASK          0x11
#define REG_MAX_PAYLOAD_LENGTH 		0x23
#define REG_HOP_CHANNEL             0x1C
#define REG_HOP_PERIOD              0x24
#define REG_SYNC_WORD				0x39
#define REG_VERSION	  				0x42

#define SX72_MODE_RX_CONTINUOS      0x8D
#define SX72_MODE_TX                0x83
#define SX72_MODE_SLEEP             0x80
#define SX72_MODE_STANDBY           0x89
#define LORA_CAD_MODE               0x87


#define        REG_FRF_MSB              0x06
#define        REG_FRF_MID              0x07
#define        REG_FRF_LSB              0x08

#define PAYLOAD_LENGTH              0x80
// LOW NOISE AMPLIFIER
#define REG_LNA                     0x0C
#define LNA_MAX_GAIN                0x23
#define LNA_OFF_GAIN                0x00
#define LNA_LOW_GAIN		    	0x20

#define MAX_MSG_LEN                 128

//! MACROS //
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)  // read a bit


typedef unsigned char byte;
static const int CHANNEL = 0;
static const int rssicorr = 137; //in HopeRF Spec page 82
static const int rssi_threshold= -70;

//##############################################
//Change Variable here !!
// SX1272 - Raspberry connections
#define m_ssPin     10 //CS    GPIO 8 wiringPI 10
#define m_dio0      21 //DIO 0 GPIO 5 wiringPI 21
#define m_RST       22 //Reset GPIO 6 wiringPI 22



//##############################################
using namespace std;

class HopeRF {
public:

    int _RSSI;

    HopeRF();
    HopeRF(const HopeRF& orig);
    virtual ~HopeRF();
    int setParam(uint32_t RxFrequ,uint32_t TxFrequ,int SF, double BW, int CR);
    int setupHopeRF();
    int setupTxHopeRF();
    int TxCarrierSense();
    int TXSendPacket(char* buffer, int size,int Ack=0);
    int receivepacket();
    string getReceivePkt();
    int getNumReceivedPkt();
    int getNumReveivedPktOK();
    int getNumReveivedPktNOK();
    int doCAD(int counter);
    int getRSSI();
    int proofACK(char* buffer, int size);

private:
    
    int m_sf;       //spreading factor (SF7 - SF12)
    double m_bw;    //Bandwith
    int m_cr;       //Coding Rate
    byte m_bSF;     
    byte m_bBW;
    byte m_bCR;
    uint32_t cp_nb_rx_rcv;
    uint32_t cp_nb_rx_ok;
    uint32_t cp_nb_rx_bad;
    uint32_t cp_nb_rx_nocrc;
    uint32_t cp_up_pkt_fwd;
    byte receivedbytes;
    // Set center frequency
    uint32_t  m_freq; // in hz! (433650000)
    uint32_t m_txfreq;
    string outString;
    /* Not used anymore */
    
    char LAT[10];
    char LON[10];
    char ALT[6];
    char CALL[12];
    char VOLT[6];
    /*         */

    string double2string(float iNumber);
    void copy2field(byte fieldNr, char* strTemp, byte len);
    int receivePkt(char *payload);
    void selectreceiver();
    void unselectreceiver();
    byte readRegister(byte addr);
    void writeRegister(byte addr, byte value);
};

#endif /* HOPERF_H */

