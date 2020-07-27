
/* 
 * File:   HMI.h
 * Author: paulalexander66
 *
 * Created on 10. Mai 2017, 19:33
 */

#ifndef HMI_H
#define HMI_H

#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <time.h> 

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include <ArduiPi_OLED_lib.h>
#include <ArduiPi_OLED.h>
#include <Adafruit_GFX.h>

//##############################################
//Change Variable here !!

//Raspberry connections TEST BOARD
/*
#define SW_1  24          //GPIO 19   WiringPI 24     Phisycal 35
#define SW_2  27          //GPIO 16   WiringPI 27     Phisycal 36
#define SW_3  25          //GPIO 26   WiringPI 25     Phisycal 37
#define SW_4  28          //GPIO 20   WiringPI 28     Phisycal 38
#define SW_5  29          //GPIO 21   WiringPI 29     Phisycal 40
#define Power_Oled  21    //GPIO 5    WiringPI 21     Phisycal 29
*/
//Raspberry connections Shield
#define SW_4  24          //GPIO 19   WiringPI 24     Phisycal 35
#define SW_1  27          //GPIO 16   WiringPI 27     Phisycal 36
#define SW_5  25          //GPIO 26   WiringPI 25     Phisycal 37
#define SW_2  28          //GPIO 20   WiringPI 28     Phisycal 38
#define SW_3  29          //GPIO 21   WiringPI 29     Phisycal 40
#define Power_Oled  23    //GPIO 5    WiringPI 21     Phisycal 29


#define mTimout     500000       //in Milliseconds usleep(s*1000000)*/);  = seconds
#define Welcome_Text_1 "LoRa APRS"
#define Welcome_Text_2 "Gateway"
#define Max_Len_Line 18
#define Max_Line 4


//##############################################
using namespace std;

class HMI {
public:
    HMI();
    HMI(const HMI& orig);
    virtual ~HMI();
    int setupHMI();
    void clearDisplay();
    void drawWelcome();
    void printError(string Error);
    void printInfo(string sMessage);
    void printMenue(string sMessage);
    void printMessage(string sHeader,string Error);
    void printGoodby();
    void drawMenue(const uint16_t x0, const uint16_t zeile_len,const char* sText);
    void drawMenue(const uint16_t x0, const uint16_t y0, const uint16_t zeile_len,const char* sText);
    void drawTriangle_down();
    void drawTriangle_up();
    void showConfig();
    void showStatistic();
    void showPackets();
    void printPacket(string sPacket);
    void showLine(int Line, string Text);
    int readButton();
    
private:
    ArduiPi_OLED display;
    uint16_t m_width;
    uint16_t m_height;
    int calcLine(string sText);
    
};

#endif /* HMI_H */
