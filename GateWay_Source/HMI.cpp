

#include "HMI.h"

HMI::HMI(){
    m_width=0;
    m_height=0;
}
    

HMI::HMI(const HMI& orig){

}
    
HMI::~HMI(){
}
    

int HMI::setupHMI(){
    //printf("Set up GPIO's..\n");    
    wiringPiSetup ();
    
    pinMode(SW_1,INPUT);
    pinMode(SW_2,INPUT);
    pinMode(SW_3,INPUT);
    pinMode(SW_4,INPUT);
    pinMode(SW_4,INPUT);
    pinMode(Power_Oled,OUTPUT);
    
    //printf("Power OLED on..\n");
    digitalWrite(Power_Oled,HIGH);
    usleep(mTimout);
    
    //Set up OLED display
    //printf("Set up display..\n");
    if ( !display.init(OLED_I2C_RESET,OLED_ADAFRUIT_I2C_128x64) ){
        //printf("Error initialising display..\n");
        return (1);
    }
    display.begin();

    m_width=display.width();
    m_height=display.height();

    // init done
    display.clearDisplay();   // clears the screen  buffer
    display.display();   		// display it (clear display)

    return 0;
}


//Gives back Number of Button
//pressed, if nothing is pressed then 0
int HMI::readButton(){
    int iButton=0;
    if(digitalRead(SW_1)){
        usleep(mTimout);
        //printf("SW_1 pressed..\n");
        iButton=1;
    }else if(digitalRead(SW_2)){
        usleep(mTimout);
        //printf("SW_2 pressed..\n");
        iButton=2;
    }else if(digitalRead(SW_3)){
        usleep(mTimout);        
        //printf("SW_3 pressed..\n");
        iButton=3;
    }else if(digitalRead(SW_4)){
        usleep(mTimout);
        //printf("SW_4 pressed..\n");
        iButton=4;
    }else if(digitalRead(SW_5)){
        usleep(mTimout);
        //printf("SW_5 pressed..\n");
        iButton=5;
    }else{
        iButton=0;
    }             
    return iButton;

}

void HMI::drawMenue(const uint16_t x0, const uint16_t zeile_len,const char* sText){
    drawMenue(x0,display.height()-10, zeile_len,sText);
}



void HMI::drawMenue(const uint16_t x0, const uint16_t y0, const uint16_t zeile_len,const char* sText){
    display.fillRect(x0-2,y0-2, zeile_len, 20,WHITE);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(x0,y0);
    display.print(sText);
    display.display();
}

void HMI::clearDisplay(){
    display.clearDisplay();   // clears the screen  buffer
    display.display();   		// display it (clear display)
}

void HMI::drawWelcome(){
    display.fillRect(0,0, m_width, 50,WHITE);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(5,5);
    display.print(Welcome_Text_1);
    display.setCursor(15,30);
    display.print(Welcome_Text_2);
    display.display();
}

void HMI::printGoodby(){
    printMessage("Goodby","Hope, to see you    again..");
    usleep(3000000);
    display.clearDisplay();   // clears the screen  buffer
    display.display();   		// display it (clear display)
}

void HMI::printError(string Error){
    printMessage("Error",Error);
}

void HMI::printInfo(string sMessage){
    printMessage("Message", sMessage);
}

void HMI::printMenue(string sMessage){
    printMessage("Menue", sMessage);
}

void HMI::printMessage(string sHeader,string Error){
    int iThik = 5;
    display.clearDisplay();   // clears the screen  buffer
    display.fillRect(0,0, m_width, m_height,WHITE);
    display.fillRect(0,iThik*4, m_width, m_height,BLACK);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(30,2);
    display.print(sHeader.c_str());
    
    //Calc how many Lines
    int Lines=calcLine(Error);
    //Split Text into Lines
    if(Lines!=0){
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setTextWrap(true);
        for(int i=0;i<Lines;i++){
            if((i+1)*Max_Len_Line<=(int)Error.length()){
                //printf("i = %d\n",i);
                string sTemp=Error.substr(i*Max_Len_Line,Max_Len_Line);
                //printf("%s\n",sTemp.c_str());
                display.setCursor(5,iThik*5+5+i*10);
                display.print(sTemp.c_str());
            }else{
                string sTemp=Error.substr(i*Max_Len_Line,Error.length()-i*Max_Len_Line);
                //printf("%s\n",sTemp.c_str());
                display.setCursor(5,iThik*5+5+i*10);
                display.print(sTemp.c_str());
            }   
        } 
    }
    display.display();
}

void HMI::printPacket(string sPacket){
    showPackets();
    //Calc how many Lines
    int Lines=calcLine(sPacket);
    //Split Text into Lines
    if(Lines!=0){
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setTextWrap(true);
        for(int i=0;i<Lines;i++){
            if((i+1)*Max_Len_Line<=(int)sPacket.length()){
                //printf("i = %d\n",i);
                string sTemp=sPacket.substr(i*Max_Len_Line,Max_Len_Line);
                //printf("%s\n",sTemp.c_str());
                display.setCursor(5,10+i*10);
                display.print(sTemp.c_str());
            }else{
                string sTemp=sPacket.substr(i*Max_Len_Line,sPacket.length()-i*Max_Len_Line);
                //printf("%s\n",sTemp.c_str());
                display.setCursor(5,10+i*10);
                display.print(sTemp.c_str());
            }   
        } 
    }
    display.display();
}

void HMI::drawTriangle_down(){
    const uint16_t x0 = display.width()-15;
    const uint16_t y0 =display.height()-15;
    display.fillTriangle(x0, y0, x0+10, y0, x0+5, y0+5, WHITE);
    display.display();
}

void HMI::drawTriangle_up(){
    const uint16_t x0 = display.width()-15;
    const uint16_t y0 = 10;
    display.fillTriangle(x0, y0, x0+10, y0, x0+5, y0-5, WHITE);
    display.display();
}

void HMI::showConfig(){
    int iThik = 5;
    display.clearDisplay();   // clears the screen  buffer
    display.fillRect(0,0, m_width-20, m_height,WHITE);
    display.fillRect(0,iThik*2, m_width, m_height,BLACK);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(30,2);
    display.print("Config");
    display.display();

}

void HMI::showStatistic(){
    int iThik = 5;
    display.clearDisplay();   // clears the screen  buffer
    display.fillRect(0,0, m_width-20, m_height,WHITE);
    display.fillRect(0,iThik*2, m_width, m_height,BLACK);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(30,2);
    display.print("Statistic");
    display.display();

}

void HMI::showPackets(){
    int iThik = 4;
    display.clearDisplay();   // clears the screen  buffer
    display.fillRect(0,0, m_width-20, m_height,WHITE);
    display.fillRect(0,iThik*2, m_width, m_height,BLACK);
    display.setTextSize(1);
    display.setTextColor(BLACK);
    display.setCursor(30,2);
    display.print("Packets");
    drawMenue(68,28,"Back");
    display.display();

}

void HMI::showLine(int Line, string Text){
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5,Line*10+15);
    display.print(Text.c_str());
    display.display();
}

int HMI::calcLine(string sText){
    int iLine =0;
    int iLen=sText.length();
    if(iLen!=0){
        if(iLen<Max_Len_Line){
            iLine=1;
        }else{
            iLine=iLen/Max_Len_Line;
            int iRest=iLen%Max_Len_Line;
            if(iRest)iLine++;
            //printf("sText= %s | iLen %d | iRest= %d | iLine= %d \n",sText.c_str(),iLen,iRest,iLine);
        }
    }
    if (iLine>Max_Line)iLine =Max_Line;
    return iLine;
}
