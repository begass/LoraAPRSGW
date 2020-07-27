# iot4pi_LoraGW_01
# 1. Test 

CC=g++
CFLAGS=-c -Wall
LIBS=-lwiringPi -lArduiPi_OLED

all: iot4pi_LoraGW_01

iot4pi_LoraGW_01:  main.o APRS_Connector.o ConfigParam.o ParamList.o HopeRF.o HMI.o TempPressHum.o bme280.o
	$(CC) main.o  APRS_Connector.o ConfigParam.o  ParamList.o HopeRF.o HMI.o TempPressHum.o bme280.o $(LIBS) -o iot4pi_LoraGW_01

main.o: main.cpp ParamList.h APRS_Connector.h HopeRF.h TempPressHum.h
	$(CC) $(CFLAGS) main.cpp

APRS_Connector.o: APRS_Connector.cpp
	$(CC) $(CFLAGS) APRS_Connector.cpp

ConfigParam.o: ConfigParam.cpp
	$(CC) $(CFLAGS) ConfigParam.cpp

ParamList.o: ParamList.cpp ConfigParam.h
	$(CC) $(CFLAGS) ParamList.cpp

HopeRF.o: HopeRF.cpp
	$(CC) $(CFLAGS) HopeRF.cpp

HMI.o: HMI.cpp
	$(CC) $(CFLAGS) HMI.cpp

bme280.o: bme280.c
	gcc -c bme280.c

TempPressHum.o:TempPressHum.c bme280.h  
	gcc -c TempPressHum.c

clean:
	rm *.o
