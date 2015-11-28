#!/bin/bash

#This line checks to see if fire_control is already running.  If so, the
#  process terminates.
if [ "x$(ps -ef|grep $0|grep bash|awk '{print $2}'|grep -v $$ |grep -v $(( $$ + 1 ))|xargs)" != "x" ]; then
  exit 0;
fi  

# Change these to the locations of your fifo and generic dev name
DEV="$(ls -1 /dev/cu.usb*|tail -1)"
FIFO="/Users/Shared/commandfifo" #create this manually using mkfifo prior

# Checks to make sure root is running this script, terminates if not.
if [ $UID != 0 ]; then
   echo ERROR: Run as root!
   exit 1
fi

#This is convoluted, but required in order to hold the device open in bash.  In
#  essence, everything inside the parenthesis is sent to the arduino until the
#  command quit is received.  The device is kept open the whole time, if it's 
#  not held open, it will reset every time the device is accessed

( sleep 2;

  while [ "${REPLY}" != "quit" ]; do

    read -p "Enter command (arm/shot/salvo/stop/quit):  " >&2 < $FIFO; 


    echo $REPLY

    sleep 2

  done

  echo stop

)>$DEV
    
