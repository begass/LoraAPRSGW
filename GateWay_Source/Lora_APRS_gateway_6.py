#!/usr/bin/python

# Copyright 2012 Leigh L. Klotz, Jr. WA5ZNU Leigh@WA5ZNU.org
# MIT License: see LICENSE.txt

#Edit by IoT4pi <office@iot4pi.com> under Apache License Version 2.0 (APLv2)
import socket
import select
from ConfigParser import SafeConfigParser
import time
import datetime


#Path to config File
strPath="/home/pi/iot4pi/APRS.conf"
bDebug=True

# Configure this part with your call and APRS-IS passcode
APRS_IS_CALL=""

#APRS_IS_PASSCODE = "99999"		# Edit this to be your passcode
APRS_IS_PASSCODE=""

#Gateway can transmitt
TRANSMIT="True"

#Server-side Filter Commands
FILTER=""

# Configure this part with your location, PHG (antenna description), and brief info
LATITUDE=""
LONGITUDE=""
PHG=""
INFO=""

# You can change APRS_IS_HOST to a server geographically close to you.
# See http://www.aprs2.net/
#http://185.18.149.99:14501/
APRS_IS_HOST=""
APRS_IS_PORT=0

# Edit these if you need to change where this UDP server listens.
LISTEN_IP=""			# listen on all IP addresses this server has
LISTEN_UDP_PORT=8080		# Listen on UDP port 8080
addr=None                   #Adrress

REPLACE_PATH=False		# If true, add ",qAR,APRS_IS_CALL" to path
#VERSION="LoRa-APRS_iGate"
VERSION=""
udp_sock = None
aprs_is_sock = None

#counter of received packets
numPackets=0
#time of last received packet from aprs Server
LTime=0
#timeout in seconds between new authetication
Auth_Timeout =0
#BME280
BME280=""
#Temperature Huminity and Pressure Storage
TempHumPress=""

def read_config():
    global APRS_IS_CALL
    global APRS_IS_PASSCODE
    global TRANSMIT
    global FILTER
    global LATITUDE
    global LONGITUDE
    global PHG
    global INFO
    global APRS_IS_HOST
    global APRS_IS_PORT
    global LISTEN_IP
    global LISTEN_UDP_PORT
    global REPLACE_PATH
    global Version
    global Auth_Timeout
    global BME280
    
    #open parser file
    config = SafeConfigParser()

    try:
        config.read(strPath)
    except:
        if bDebug:
            print getTime()+ ": Error open config file !"
        return False
    #parse SETUP Variales
    try:    
        if(config.get("SETUP", "REPLACE_PATH")=='True'):
	        REPLACE_PATH=True
        else:
            REPLACE_PATH=False

        APRS_IS_CALL = config.get("SETUP", "APRS_IS_CALL")
        APRS_IS_PASSCODE = config.get("SETUP", "APRS_IS_PASSCODE")
        TRANSMIT=config.get("SETUP", "TRANSMIT")
        FILTER=config.get("SETUP", "Filter")
        LATITUDE = config.get("SETUP", "LATITUDE")
        LONGITUDE = config.get("SETUP", "LONGITUDE")
        PHG = config.get("SETUP", "PHG")
        INFO = config.get("SETUP", "INFO")
        APRS_IS_HOST = config.get("SETUP", "APRS_IS_HOST")
        APRS_IS_PORT = int(config.get("SETUP", "APRS_IS_PORT"))
        LISTEN_IP = config.get("SETUP", "LISTEN_IP")
        LISTEN_UDP_PORT = int(config.get("SETUP", "LISTEN_UDP_PORT"))
        Version = config.get("SETUP", "Version")
        Auth_Timeout=int(config.get("SETUP", "Auth_Timeout"))
        BME280=config.get("SETUP", "BME280")

    except:
        if bDebug:
            print getTime()+ ": Error reading SETUP Variable from Config File !"
        return False       

    if bDebug:
        print getTime()+ ": APRS_IS_CALL = " +APRS_IS_CALL
        print getTime()+ ": APRS_IS_PASSCODE = " +APRS_IS_PASSCODE
        print getTime()+ ": TRANSMIT = " +TRANSMIT
        print getTime()+ ": FILTER = " +FILTER
        print getTime()+ ": LATITUDE = " +LATITUDE
        print getTime()+ ": LONGITUDE = " +LONGITUDE
        print getTime()+ ": PHG = " +PHG
        print getTime()+ ": INFO = " +INFO
        print getTime()+ ": APRS_IS_HOST = " +APRS_IS_HOST
        print getTime()+ ": APRS_IS_PORT = " +str(APRS_IS_PORT)
        print getTime()+ ": LISTEN_IP = " +LISTEN_IP
        print getTime()+ ": LISTEN_UDP_PORT = " +str(LISTEN_UDP_PORT)
        print getTime()+ ": REPLACE_PATH = " +str(REPLACE_PATH)
        print getTime()+ ": Version = " +Version
        print getTime()+ ": Auth_Timeout = " +str(Auth_Timeout)
        print getTime()+ ": BME280 = " +BME280


    return True

    
    
    
#######################################
#Helper function
# Function to get present Time as String
def getTime():
    ts = time.time()
    strTime = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    return strTime
###### End Function



def open_connections():
    global udp_sock, aprs_is_sock
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
    udp_sock.bind((LISTEN_IP, LISTEN_UDP_PORT))
    try:
        aprs_is_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # TCP
        aprs_is_sock.connect((APRS_IS_HOST, APRS_IS_PORT))
        #print 'Open ceonnection started..\n'
    except:
        print 'Error establishing tcp connection\n'

def auth_packet():
    global Version
    global APRS_IS_CALL
    global APRS_IS_PASSCODE
    global FILTER
    
    return "user %s pass %s vers %s filter %s\r\n" % (APRS_IS_CALL, APRS_IS_PASSCODE, Version, FILTER)

def position_packet():
    return ("%s>APOTW1,TCPIP*:!%sI%s&%s/%s\r\n" % (APRS_IS_CALL, LATITUDE, LONGITUDE, PHG, INFO+" "+TempHumPress))

def send_packet(packet):
    global udp_sock, aprs_is_sock
    print "->", packet
    message = ("%s\r\n" % (packet))
    aprs_is_sock.sendall(message)

def process_packets():
    global udp_sock, aprs_is_sock,numPackets,LTime,addr 
    global TempHumPress
    # Await a read event for 5 seconds
    rlist, wlist, elist = select.select([udp_sock, aprs_is_sock], [], [], 5)
    for sock in rlist:
	    if sock == udp_sock:
	        inpacket, addr = udp_sock.recvfrom(1024) # buffer size is 1024 bytes
	        inpacket = inpacket.strip()
	        if (inpacket != ""):
	            print 'received from APRS GW  packet '+ inpacket + '\n'
                if(inpacket=="Test"):
                    message=str(numPackets)+";"+getTime()+'\x00\r'
                    #print message
                    udp_sock.sendto(message, addr)
                elif inpacket.startswith("Temp:") :
                    print 'Temperature Packet received by APRS GW'
                    TempHumPress=inpacket
                else:
                    print 'APRS GW received packet and sends to APRS Server :'
                    message=str(numPackets)+";"+getTime()+'\x00\r'
                    #print message
                    udp_sock.sendto(message, addr)
                    send_packet(replace_path(inpacket))
                    numPackets=1+numPackets

	    elif sock == aprs_is_sock:
                #Meassages from APRS Server
                reply = aprs_is_sock.recv(4096)
                #ReceivedStr=aprs_is_sock.recv(1024)
                if reply.startswith('#'):
	            print "<-"+ reply.strip()
                else:
                    temp1=reply.find('>')
                    #######################################
                    ## Rework with regular expresions !!!!!!!
                    #######################################
                    #Proof for Message Type  :123456789:
                    #if temp1:
                    if reply.rfind(':') and reply[reply.rfind(':')-10:reply.rfind(':')-9]==':':
                        print 'Message Type received :\n'
                        
                        print reply
                        Call=reply[0:temp1]
                        print 'Call ' + Call
                        temp2=reply.find('::')
                        if temp2:
                            Via = reply[reply.rfind(',')+1:reply.find('::')]
                            print 'message from ' + Via
                            To=reply[temp2+2:temp2+2+9]
                            print 'message to ' + To
                            Text=reply[temp2+2+9+1:]
                            Text=Text.rstrip()
                            print 'message : ' + Text

                            message='M|'+Call+'|'+Via +'|'+To+'|'+Text
                            
                            messageSEND=Call+'>LORA::'+To+':'+Text
                            
                            temp3=reply.rfind('{')
                            MesNo=''
                            if temp3>0:
                                print 'ACK requested !'
                                
                                MesNo=reply[temp3+1:]
                                print 'message # ' + MesNo
                                temp3=message.rfind('{')
                                message=message[:temp3]
                                message=message+'|A|'+MesNo
                                ack_message = '1'+message
                                messageSEND='1'+messageSEND
                                if (TRANSMIT=="True"):
                                    #udp_sock.sendto(ack_message+'\x00\r', addr)
                                    udp_sock.sendto(messageSEND+'\x00\r', addr)
                                    print 'send to c++ ',ack_message
                                    print 'messageSEND =', messageSEND
                                elif (TRANSMIT=="False"):
                                    print 'GW is not alowed to transmit !'
                                    #sending reject to APRS Server
                                    #IoT4Pi3>APRS::OE1KEB   :rej1
                                    while(len(Call)<9):
                                        Call+=' '
                                    resp=To.strip().upper()+'>APRS::'+Call.upper()+':rej'+MesNo
                                    print 'send to APRS Server ',resp
                                    send_packet(replace_path(resp))

                            else:
                                if (TRANSMIT=="True"):
                                    #udp_sock.sendto(message+'\x00\r', addr)
                                    udp_sock.sendto(messageSEND+'\x00\r', addr)
                                    print 'send to c++ ',message
                                    print 'messageSEND =', messageSEND
                                    
                                elif (TRANSMIT=="False"):
                                    print 'GW is not alowed to send !'
                            
                LTime=time.time()
                #print  reply


def replace_path(packet):
    if REPLACE_PATH:
        if ":" in packet:
            (path,data) = packet.split(':', 1)
            packet = path + ",qAO," + APRS_IS_CALL + ':' + data
        else:
            print "no : in packet"
    return packet

def process():
    print "Starting Lora_APRS_gateway_6.py"
    if(read_config()):
        open_connections()
        send_packet(auth_packet())
        time.sleep(2)
        send_packet(position_packet())
        LTime=time.time()
        while True:
            process_packets()
            if(time.time()-LTime)>Auth_Timeout:
                send_packet(auth_packet())
                send_packet(position_packet())
                LTime=time.time()

process()
