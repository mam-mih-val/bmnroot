#!/bin/bash
# 
# File:   MonRunner.sh
# Author: ilnur
#
# Created on Dec 1, 2016, 10:26:29 AM
#

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
/opt/fairsoft/install/bin/root.exe -b -q /home/ilnur/bmnroot/macro/raw/monDecoder.C
sleep 40

done
