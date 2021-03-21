/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HopeRF.cpp
 * Author: paulalexander66
 * 
 * Created on 11. Mai 2017, 08:56
 */

#include "HopeRF.h"

HopeRF::HopeRF() {
    m_freq = 0;
    m_txfreq=0;
    m_sf=0;
    m_bw=0;
    m_cr=0;
    m_bSF=0x00;     
    m_bBW=0x00;
    m_bCR=0x00;
    cp_nb_rx_rcv=0;
    cp_nb_rx_ok=0;
    cp_nb_rx_bad=0;
    cp_nb_rx_nocrc=0;
    cp_up_pkt_fwd=0;
    outString = "";
}

HopeRF::HopeRF(const HopeRF& orig) {
}

HopeRF::~HopeRF() {
}

/*Setup the HopeRf Receiver 
 * Return 
 0  success
 -1 failure
 */
int HopeRF::setupHopeRF(){
    
    int iReturn=0;
    if(m_freq==0 || m_sf==0 || m_bw==0 ||  m_cr ==0){
        return -1;
    }
    wiringPiSetup () ;
    pinMode(m_ssPin, OUTPUT);
    pinMode(m_dio0, INPUT);
    pinMode(m_RST, OUTPUT);
    //int fd = 
    wiringPiSPISetup(CHANNEL, 500000);

    digitalWrite(m_RST, HIGH);
    delay(100);
    digitalWrite(m_RST, LOW);
    delay(100);
    byte version = readRegister(REG_VERSION);
    printf("Version Number %#04x \n", version);
    if (version == 0x22) {
        //Version code of the chip        
        // HopeRF96
        printf("HopeRF96 detected, starting.\n");
    } else {
        // ?
        digitalWrite(m_RST, LOW);
        delay(100);
        digitalWrite(m_RST, HIGH);
        delay(100);
        version = readRegister(REG_VERSION);
        if (version == 0x12) {
            // HopeRF95??
            printf("HopeRF95 detected, starting.\n");
        } else {
            printf("Unrecognized transceiver.\n");
            printf("Version: 0x%x\n",version);
            return -1;
        }
    }

    writeRegister(REG_OPMODE, SX72_MODE_SLEEP);
    // set frequency
    //@APA 434.400.000 = 0x6c 99 99  = 108, 153, 153
    //@APA 433.650.000 = 0x6c 69 99  = 108, 105, 153
    //    
    uint64_t frf = ((uint64_t)m_freq << 19) / 32000000;
    writeRegister(REG_FRF_MSB, (uint8_t)(frf>>16) );
    writeRegister(REG_FRF_MID, (uint8_t)(frf>> 8) );
    writeRegister(REG_FRF_LSB, (uint8_t)(frf>> 0) );
    //DebugPrint
    byte freq_MSB = readRegister(REG_FRF_MSB);
    byte freq_MID = readRegister(REG_FRF_MID);
    byte freq_LSB = readRegister(REG_FRF_LSB);
    printf("Freq is  %#04x  %#04x  %#04x \n", freq_MSB,freq_MID,freq_LSB);

    //@APA HopeRF can't set a SyncWord
    //writeRegister(REG_SYNC_WORD, 0x34); // LoRaWAN public sync word
    //version = readRegister(REG_SYNC_WORD);
    //printf("Sync_Word: 0x%x\n",version);

    //@APA HopRF Setting doing it by Hand for Raspberry Pi GW
    //  REG_MODEM_CONFIG    = 0101 001 0  = 0x52 = 82   //Config mit BW 41,7
    //  REG_MODEM_CONFIG    = 0111 001 0  = 0x72 = 114  //Config mit BW 125
    //  REG_MODEM_CONFIG2   = 1100 0 1 11 = 0xC7 = 199
    //  REG_MODEM_CONFIG3   = 0000 1 0 00 = 0x08 = 8 --> is setting Mobile Note Bit Nececssery ??
    byte Conf1=m_bBW<<4|m_bCR<<1|0x00;  
    byte Conf2=m_bSF<<4|0x00<<3|0x01<<2|0x03;
    byte Conf3=0x01<<3;
    /*
    printf("Conf1 %#04x \n",Conf1);
    printf("Conf2 %#04x \n",Conf2);
    printf("Conf3 %#04x \n",Conf3);
    */
    writeRegister(REG_MODEM_CONFIG,Conf1);
    writeRegister(REG_MODEM_CONFIG2,Conf2);
    writeRegister(REG_MODEM_CONFIG3,Conf3);

    //@APA write further Config
    //RegOop 0x0B (OopTimer default; OopOn off = 0x0B) 
    writeRegister(0x0B, 0x0B);
    //@APA REG_SYMB_TIMEOUT_LSB defines TimeOut in Receiving
    // in HopeRf project it is set to 0xFF
    writeRegister(REG_SYMB_TIMEOUT_LSB,0xFF);
    /*
    //Original
    if (sf == SF10 || sf == SF11 || sf == SF12) {
        writeRegister(REG_SYMB_TIMEOUT_LSB,0x05);
    } else {
        writeRegister(REG_SYMB_TIMEOUT_LSB,0x08);
    }
    */
    //@APA Preamble Lenght definde 0X0C  default 0x08
    //RegPreambleLsb 0x21
    writeRegister(0x21,0x0C);

    
    //@APA HopeRf is running im explicit Mode 
    // Register are not set we running with default
    writeRegister(REG_MAX_PAYLOAD_LENGTH,0xFF);
    writeRegister(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);
    //@APA FrequHop disable 0 default therefore not set
    //writeRegister(REG_HOP_PERIOD,0xFF);
    
    //@APA Set Flags
    writeRegister(REG_IRQ_FLAGS, 0xFF);
    //Debug
    int irqTest = readRegister(REG_IRQ_FLAGS);
    printf("Set up IRQ: 0x%x\n",irqTest);
    
    writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_BASE_AD));

    //@APA Set DIO Mapping
    //to get Interrupt on dio0 faster
    //writeRegister(REG_DIO_MAPPING_1,REG1);

    // Set Continous Receive Mode
    writeRegister(REG_LNA, LNA_MAX_GAIN);  // max lna gain
    writeRegister(REG_OPMODE, SX72_MODE_RX_CONTINUOS);
    
    return iReturn;
}

/*Setup the HopeRf Transmitter 
Attention: It's first configured in 
Continous Receive Mode
 * Return 
 0  success
 -1 failure
 */
int HopeRF::setupTxHopeRF(){
    int iReturn=0;
    if(m_freq==0 || m_sf==0 || m_bw==0 ||  m_cr ==0){
        return -1;
    }
    writeRegister(REG_OPMODE, SX72_MODE_SLEEP);
    // set frequency
    //@APA 434.400.000 = 0x6c 99 99  = 108, 153, 153
    //@APA 433.650.000 = 0x6c 69 99  = 108, 105, 153
    //    
    uint64_t frf = ((uint64_t)m_txfreq << 19) / 32000000;
    writeRegister(REG_FRF_MSB, (uint8_t)(frf>>16) );
    writeRegister(REG_FRF_MID, (uint8_t)(frf>> 8) );
    writeRegister(REG_FRF_LSB, (uint8_t)(frf>> 0) );
    //DebugPrint
    byte freq_MSB = readRegister(REG_FRF_MSB);
    byte freq_MID = readRegister(REG_FRF_MID);
    byte freq_LSB = readRegister(REG_FRF_LSB);
    printf("TX Freq is  %#04x  %#04x  %#04x \n", freq_MSB,freq_MID,freq_LSB);
    //Write Config1 - Config3
    byte Conf1=m_bBW<<4|m_bCR<<1|0x00;  
    byte Conf2=m_bSF<<4|0x00<<3|0x01<<2|0x03;
    byte Conf3=0x01<<3;
    
    writeRegister(REG_MODEM_CONFIG,Conf1);
    writeRegister(REG_MODEM_CONFIG2,Conf2);
    writeRegister(REG_MODEM_CONFIG3,Conf3);

    // Set Continous Receive Mode
    writeRegister(REG_LNA, LNA_MAX_GAIN);  // max lna gain
    writeRegister(REG_OPMODE, SX72_MODE_RX_CONTINUOS);
    
    return iReturn;
}

//1. set to TX Channel (434,050 MHz) to listen
//2. dedect if Send Frequency is clear (example: 433,850 MHz)
//Return:
//      0 ....Channel free
//      1 ....Not able to send 
int HopeRF::TxCarrierSense(){
    int iReturn =1;
    printf("TxCarrierSense !\n");
    //If there are messages to send
    //1. set to TX Channel (434,050 MHz) to listen
    if(setupTxHopeRF()!=0){
        printf("Error setting up HopeRF !\n");
        return 1;
    }
    //2. dedect if Send Frequency is clear (433,850 MHz)
    int e = 1;
    int max_retry=5;
    while(e && max_retry){
        e = doCAD(3);
        printf("%d Run: e = %d\n",max_retry,e);
        max_retry--;
        usleep(500000); //= 0,5sec
    }
    if (max_retry){         
        if(!e){
            //Test showed the RSSI is not 
            // a good indication, because 
            // sometimes the receiving signal is 
            // as low as no signal present  
            e=getRSSI();
            printf("e = %d\n",e);
            printf("RSSI = %d\n",_RSSI);
            iReturn=0;
        } 
    }else{
        //printf("Channel activ! Can't transmit !\n");
        //Channel is not able to send 
        iReturn=1;               
    }

    return iReturn;
}

/*Sets the HopeRf Parameter 
 * Return 
 0  success
 -1 failure
 */
int HopeRF::setParam(uint32_t RxFrequ,uint32_t TxFrequ,int SF, double BW,int CR){
    int iReturn =0;
    if(RxFrequ<400000000 || RxFrequ>999000000){
        return -1;
    }
    if(TxFrequ!=0){
        if(TxFrequ<400000000 || TxFrequ>999000000){
            return -1;
        }
    }
    if(SF<5 || SF > 12){
        return -1;
    }
    if(BW<7.8 || BW > 500){
        return -1;
    }
    /*Error coding rate
        1 = 4/5
        2 = 4/6
        3 = 4/7
        4 = 4/8
     */
    if(CR<1 || CR>5){
        return -1;
    }
             /*   
        Signal bandwidth:
        0000 -> 7.8 kHz
        0001 -> 10.4 kHz
        0010 -> 15.6 kHz
        0011 -> 20.8kHz
        0100 -> 31.25 kHz
        0101 -> 41.7 kHz
        0110 -> 62.5 kHz
        0111 -> 125 kHz
        1000 -> 250 kHz
        1001 -> 500 kHz
        */
    if(BW==7.8){
        m_bBW=0x00;
    }else if(BW==10.4){
        m_bBW=0x01;
    }else if(BW==15.6){
        m_bBW=0x02;
    }else if(BW==20.8){
        m_bBW=0x03;
    }else if(BW==31.25){
        m_bBW=0x04;
    }else if(BW==41.7){
        m_bBW=0x05;
    }else if(BW==62.6){
        m_bBW=0x06;
    }else if(BW==125.0){
        m_bBW=0x07;
    }else if(BW==250.0){
        m_bBW=0x08;
    }else if(BW==500.0){
        m_bBW=0x09;
    }else{
        //printf("BW does not match! \n");
        return -1;
    }
    
        /*
        Error coding rate
            001 -> 4/5
            010 -> 4/6
            011 -> 4/7
            100 -> 4/8
        */
    m_bCR=CR;
    m_bSF=SF;

    m_freq = RxFrequ;
    m_txfreq = TxFrequ;
    m_sf=SF;
    m_bw=BW;
    m_cr=CR;
    
    return iReturn;
}

string HopeRF::getReceivePkt(){
    return outString;
}


int HopeRF::receivepacket(){
    
    //const int PayloadArraySize = 5;   // LAT, LON, ALT, CALL, Volt
    char tempstring[128]="";
    char message[MAX_MSG_LEN];
    long int SNR;
    char stat_timestamp[24];
    time_t t;
    
    //@APA Test if Receive IRQ 
    int irqTest = readRegister(REG_IRQ_FLAGS);
    if(irqTest & 0x40){
        printf("Receive IRQ read !\n");
        printf("IRQ in receivepacket: 0x%x\n",irqTest);
        cp_nb_rx_rcv++;
        if(receivePkt(message)) {

            //if PacketType 60- 0x3c proceed
            if(message[0]==0x3c){
                //if GPS Fix len of Packet has to be 42
                //if no GPS Fix len of Packet 14
                if((int)receivedbytes>29 && message[1]==0xff){
                    printf("GPS Packet\n");            
                }else if((int)receivedbytes<15 && message[1]==0xff){
                    printf("NO GPS Packet\n");
                    cp_nb_rx_bad++;
                    return 1;
                }else{
                    printf("Packet Len does not fit\n");
                    cp_nb_rx_bad++;
                    return 1;
                }
            }else{
                printf("Packet Type does not fit\n");
                cp_nb_rx_bad++;
                return 1;   
            }//end if PacketType 0x3c

            byte value = readRegister(REG_PKT_SNR_VALUE);
            if( value & 0x80 ) // The SNR sign bit is 1
            {
                // Invert and divide by 4
                value = ( ( ~value + 1 ) & 0xFF ) >> 2;
                SNR = -value;
            }
            else
            {
                // Divide by 4
                SNR = ( value & 0xFF ) >> 2;
            }
            
            printf("Debug Print payload\n");
            printf(message);
            printf("\n");
            
            /* get timestamp for statistics */
            t = time(NULL);
            strftime(stat_timestamp, sizeof stat_timestamp, "%F %T %Z", gmtime(&t));
            printf("Time: %s,\n ",stat_timestamp);

            byte lora_PacketSNR=readRegister(0x1A)-rssicorr;
            printf("Packet RSSI: %d, ",lora_PacketSNR);
            printf("RSSI: %d, ",readRegister(0x1B)-rssicorr);
            printf("SNR: %li, ",SNR);
            printf("Length: %i",(int)receivedbytes);
            printf("\n");

            printf("Message[0] = %x   PacketType\n",message[0]); //PacketType
            printf("Message[1] = %x   Destination\n",message[1]); //Destination
            printf("Message[2] = %x   Source\n",message[2]); //Source
            
            

            int iSNR=readRegister(REG_PKT_SNR_VALUE);
            int iRSSI =readRegister(0x1A)-rssicorr;
            printf("iSNR = %i\n",iSNR);


            //################################################
            //The new System
            for (int i = 3; i < (int)receivedbytes; i++)
            {
                tempstring[i-3]=message[i];
            }
            //////////////////
            // Get Packet Type
            // IoT4Pi3>APRS:!/641pRXYW>@[Q  --> Position Packet     --> Type 0
            // IoT4Pi3>APRS::IoT4Pi3  :ack2  --> Ack from Message   --> Type 1
            int m_Type;
            for(uint8_t x=0;x<strlen(tempstring);x++){
                if(tempstring[x]==':'){
                    if(tempstring[x+1]=='!'){
                        m_Type=0;
                        break;
                    }else if(tempstring[x+1]==':'){
                        m_Type=1;
                        break;
                    }
                }
            }//end for

            //printf("-----Temp Output tempstring = %s\n",tempstring);
            outString=tempstring;

            if (m_Type == 0){
                printf(" Packet Type 0 \n");
                //printf("-----Temp Output String = %s\n",outString.c_str());
                outString += " SNR="; 

                if (iSNR> 127){
                    outString += "-";
                    outString +=double2string((255 - iSNR) / 4);
                }else{
                    outString += "+";
                    outString += double2string( (iSNR) / 4);
                }
                outString += "dB RSSI=";
                outString += double2string(iRSSI);
                outString += ("db");

            }else if (m_Type == 1){
                printf(" Packet Type 1 \n");
            }

            
            
            //################################################

            //printf("Output String = %s\n",outString.c_str());
            cp_nb_rx_ok++;
            return 0;
        }else{ //else received a message OK
            cp_nb_rx_bad++;
            return 1;
        } //end if received a message OK
    }else{ 
        //printf("NO IRQ read !\n");
        return 1;    
    }// dio0=1
    return 0;
}

/*send Packet to FIFO and send it over HF
Return:
    1..... Error
    0......Success 
*/
int HopeRF::TXSendPacket(char* buffer, int size,int Ack){
    int iReturn = 1;
    byte st0;
    st0 = readRegister(REG_OPMODE);	// Save the previous status

    // Stdby LoRa mode to write in FIFO
	writeRegister(REG_OPMODE, SX72_MODE_STANDBY);
	//Set IRQ Flags
	writeRegister(REG_IRQ_FLAGS,0xFF);
	writeRegister(REG_IRQ_FLAGS_MASK,0xF7);
	
	//Set pointer
    // Setting address pointer in FIFO data buffer
    writeRegister(REG_FIFO_ADDR_PTR, 0x80);  
    
	//////////////////////
	//Header
	uint8_t Message[3+size];
    if(Ack==0){
	    Message[0]=0x3C;		// Write the packet type without Ack
    }else{
        Message[0]=0x3D;		// Write the packet type with Ack
    }
	Message[1]=0xFF;		// Destination node
	Message[2]=0x01;		// Source node

	//now the payload
	for(int i = 0; i<size;i++){
		Message[i+3]=buffer[i];
	}
    printf("Sending Message...len =%d | size=%d |",sizeof(Message),size);


    for(uint8_t i = 0; i < size+3; i++)
    {
        writeRegister(REG_FIFO,Message[i]);  // Writing the payload in FIFO
        printf("%c",Message[i]);
    }
    printf("\n");

	//Set payload length
	//define PAYLOAD_LENGTH    0x40 = 64
	//writeRegister(REG_PAYLOAD_LENGTH,PAYLOAD_LENGTH);
	writeRegister(REG_PAYLOAD_LENGTH,size+3);

    int value = readRegister(REG_PAYLOAD_LENGTH);
    if (value != size+3){
        printf("Error writing payload to HopeRF !\n");
        // Getting back to previous status
	    writeRegister(REG_OPMODE,st0);
        return 1;
    }

	uint8_t lora_LTXPower=10;
	uint8_t lora_Lvar1 = lora_LTXPower + 0xEE;
	writeRegister(REG_PA_CONFIG,lora_Lvar1);					// set TX power

	//Setting Modem to Transmit Mode
	writeRegister(REG_OPMODE,SX72_MODE_TX);
	//Waiting for Modem if sending is done
	uint8_t done = 0;
	uint8_t irqTest;
	while (done !=1){
		//Ask if IRQ TxDone came
		irqTest = readRegister(REG_IRQ_FLAGS);
		if(irqTest & 0x08){
			done=1;
			//Resetting IRQ again
			writeRegister(REG_IRQ_FLAGS,0x08);
			iReturn =0;
		}
	}
	// Getting back to previous status
	writeRegister(REG_OPMODE,st0);
	
    return iReturn;
}

/*
 Function: Configures the module to perform CAD.
 Returns: Integer that determines if the number of requested CAD have been successfull
   state = 2  --> The command has not been executed
   state = 1  --> There has been an error while executing the command
   state = 0  --> The command has been executed with no errors
*/
int HopeRF::doCAD(int counter){
    int state = 2;
    byte value = 0x00;
    unsigned long startDoCad,startCAD;
    unsigned long  endCAD, endDoCad;
    unsigned long previous;
    uint16_t wait = 100;
    bool failedCAD=false;
    uint8_t retryCAD = 3;
    uint8_t save_counter;
    byte st0;

    st0 = readRegister(REG_OPMODE);	// Save the previous status
    //printf("HopeRF::Starting 'doCAD'\n");
    save_counter = counter;
    startDoCad=millis();

    writeRegister(REG_OPMODE, SX72_MODE_STANDBY);

    do {

        // wait to CadDone flag
        startCAD = previous = millis();

        // Initializing flags
        writeRegister(REG_IRQ_FLAGS, 0xFF);	// LoRa mode flags register

        writeRegister(REG_OPMODE, LORA_CAD_MODE);  // LORA mode - Cad

        value = readRegister(REG_IRQ_FLAGS);
        // Wait until CAD ends (CAD Done flag) or the timeout expires
        while ((bitRead(value, 2) == 0) && (millis() - previous < wait))
        {
            value = readRegister(REG_IRQ_FLAGS);
            // Condition to avoid an overflow (DO NOT REMOVE)
            if( millis() < previous )
            {
                previous = millis();
            }
        }
        state = 1;

        endCAD = millis();

        if( bitRead(value, 2) == 1 )
        {
            state = 0;	// CAD successfully performed
			  
            printf("HopeRF::CAD duration ");
            printf("%lu\n", endCAD-startCAD);
            printf("HopeRF::CAD successfully performed\n");  
            value = readRegister(REG_IRQ_FLAGS);

            // look for the CAD detected bit
            if( bitRead(value, 0) == 1 )
            {
                // we detected activity
                failedCAD=true;
                
                printf("HopeRF::CAD exits after ");
                printf("%d\n", save_counter-counter); 		
                
            }

            counter--;
        }
        else
        {
				
            printf("HopeRF::CAD duration ");
            printf("%lu\n", endCAD-startCAD);
			 
            if( state == 1 )
            {
                printf("HopeRF::Timeout has expired\n");
            }else{
                printf("HopeRF::Error and CAD has not been performed\n");
            }
            
            retryCAD--;

            // to many errors, so exit by indicating that channel is not free
            if (!retryCAD)
                failedCAD=true;
        }

    } while (counter && !failedCAD);


    writeRegister(REG_OPMODE, st0);

    endDoCad=millis();

    // Initializing flags
    writeRegister(REG_IRQ_FLAGS, 0xFF);	// LoRa mode flags register

	
    printf("HopeRF::doCAD duration ");
    printf("%lu\n", endDoCad-startDoCad);
    

    if (failedCAD)
        return 2;

    return state;
}


/*
 Function: Gets the current value of RSSI.
 Returns: Integer that determines if there has been any error
   state = 2  --> The command has not been executed
   state = 1  --> There has been an error while executing the command
   state = 0  --> The command has been executed with no errors
*/
int HopeRF::getRSSI(){
    
    uint8_t state = 2;
    int rssi_mean = 0;
    int total = 5;


    printf("\n");
    printf("Starting 'getRSSI'\n");

    /// LoRa mode
    // get mean value of RSSI
    for(int i = 0; i < total; i++)
    {
        _RSSI = readRegister(0x1B)-rssicorr;
        //printf("RSSI measured %d \n",_RSSI);
        rssi_mean += _RSSI;
    }

    rssi_mean = rssi_mean / total;
    _RSSI = rssi_mean;
    
    state = 0;
    /*
    printf("## RSSI value is ");
    printf("%d", _RSSI);
    printf(" ##\n");
    printf("\n");
    */
    return state;
}

int HopeRF::getNumReceivedPkt(){
    return cp_nb_rx_rcv;
}


int HopeRF::getNumReveivedPktOK(){
    return cp_nb_rx_ok;
}


int HopeRF::getNumReveivedPktNOK(){
    return cp_nb_rx_bad;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++
//Starting private section

//@APA Helper function 
//Double to String
string HopeRF::double2string(float iNumber){
    std::ostringstream ss;
    ss << iNumber;
    std::string s(ss.str());
    return s;
}


//@APA copy string to Payload Field
void HopeRF::copy2field(byte fieldNr, char* strTemp, byte len){
    byte i=0;
    /*
    printf("Nr = %x\t",fieldNr);
    printf("len = %x\t",len);
    printf("Field =");
    printf(strTemp);
    printf("\n");
    */
    switch(fieldNr){
        case 0:
            //LAN
            memset((char *) &LAT, 0, sizeof(LAT));
            for(i=0;i<len;i++){
                LAT[i]=strTemp[i];
            }
                     
            break;
        case 1:
            ///LON
            memset((char *) &LON, 0, sizeof(LON));
            for(i=0;i<len;i++){
                LON[i]=strTemp[i];
            }
            break;
        case 2:
            //ALT
            memset((char *) &ALT, 0, sizeof(ALT));
            for(i=0;i<len;i++){
                ALT[i]=strTemp[i];
            }
            break;
        case 3:
            //CALL
            memset((char *) &CALL, 0, sizeof(CALL));
            for(i=0;i<len;i++){
                CALL[i]=strTemp[i];
            }
            break;
        case 4:
            //VOLT
            memset((char *) &VOLT, 0, sizeof(VOLT));
            for(i=0;i<len;i++){
                VOLT[i]=strTemp[i];
            }
            break;
        default:
            break;
    }//end switch
}


int HopeRF::receivePkt(char *payload)
{
    
///////////////////DEBUG
/*
Erhalte, neben RXDone (0x40) ich noch Info über 0x16 = ValidHeader (In Summe ist IRQ 0x50) und wenn 
ich mit 0x40 RXDone zurücksetze geht verwunderlicher weise die Info ValidHeader verloren -> was komisch ist da in Doku steht :
„Packet reception complete interrupt: writing a 1 clears the IRQ“
Und genauso bei Valid Header:
„Valid header received in Rx: writing a 1 clears the IRQ“

Somit die Vermutung, dass man sich dadurch auch den PayloadCrcError zerstört  ?

*/
    // clear rxDone
    //writeRegister(REG_IRQ_FLAGS, 0x40);
    int irqflags = readRegister(REG_IRQ_FLAGS);
    printf("IRQ in receivePkt: 0x%x\n",irqflags);
    long int SNR=0;
    int hop_channel=readRegister(REG_HOP_CHANNEL);
    printf("Hop_channel: 0x%x\n",hop_channel);
    byte value = readRegister(REG_PKT_SNR_VALUE);
    if( value & 0x80 ) // The SNR sign bit is 1
    {
        // Invert and divide by 4
        value = ( ( ~value + 1 ) & 0xFF ) >> 2;
        SNR = -value;
    }
    else
    {
        // Divide by 4
        SNR = ( value & 0xFF ) >> 2;
    }
    printf("SNR: %li, \n",SNR);
///////////////DEBUG END




    //  payload crc: 0x20
    if((irqflags & 0x20) == 0x20)
    {
        printf("CRC error\n");
        //@APA Set Flags again
        writeRegister(REG_IRQ_FLAGS, 0xFF);
        return 0;
    }else {
	    //printf("Read Register Receive Register\n");
        //@APA Set Flags again
        writeRegister(REG_IRQ_FLAGS, 0xFF);
        
        byte currentAddr = readRegister(REG_FIFO_RX_CURRENT_ADDR);
        byte receivedCount = readRegister(REG_RX_NB_BYTES);

        if(receivedCount>MAX_MSG_LEN){
            //Message len too long
            printf("Message Len is too long !!\n");
            return 0;
        }
        receivedbytes = receivedCount;

        writeRegister(REG_FIFO_ADDR_PTR, currentAddr);

        for(int i = 0; i < receivedCount; i++)
        {
            payload[i] = (char)readRegister(REG_FIFO);
        }
        payload[receivedCount] ='\0';
         
    }
    return 1;
}

void HopeRF::selectreceiver()
{
    digitalWrite(m_ssPin, LOW);
}

void HopeRF::unselectreceiver()
{
    digitalWrite(m_ssPin, HIGH);
}

byte HopeRF::readRegister(byte addr)
{
    unsigned char spibuf[2];

    selectreceiver();
    spibuf[0] = addr & 0x7F;
    spibuf[1] = 0x00;
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);
    unselectreceiver();

    return spibuf[1];
}

void HopeRF::writeRegister(byte addr, byte value)
{
    unsigned char spibuf[2];

    spibuf[0] = addr | 0x80;
    spibuf[1] = value;
    selectreceiver();
    wiringPiSPIDataRW(CHANNEL, spibuf, 2);

    unselectreceiver();
}

