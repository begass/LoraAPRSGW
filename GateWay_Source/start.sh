#!/bin/sh
Pfad="/home/pi/LoRaGateway/iot4_LoraGW_01/"
echo "Waiting to wake up..."
sleep 30
echo "Starting UDP..GW.."

sudo $Pfad"iot4pi_LoraGW_01" >> $Pfad"LogAPRS.txt"
#echo "cd $Pfad;sudo python $Pfad"iot4pi_LoraGW_01" >> $Pfad"LogAPRS.txt""
