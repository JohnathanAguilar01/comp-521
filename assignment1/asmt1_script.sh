#!/bin/bash

cd $HOME/projects/comp521/assignment1/kernal_modules/
make clean
make
sudo insmod jiffies.ko
sudo insmod seconds.ko
lsmod | head
sleep 8
cat /proc/jiffies
cat /proc/seconds
sudo rmmod jiffies
sudo rmmod seconds
make clean
