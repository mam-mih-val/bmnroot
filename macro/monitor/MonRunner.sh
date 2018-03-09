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
cd "${BMNROOT}/macro/monitor/"
/opt/fairsoft/install/bin/root.exe -b -q 'monitor.C( 7, "/home/ilnur/bmnroot/build/", "/home/ilnur/bmnroot/build/", "localhost", 9001)'
sleep 10
/opt/fairsoft/install/bin/root.exe -b -q 'monitor.C( 7, "/home/ilnur/bmnroot/build/", "/home/ilnur/bmnroot/build/", "localhost", 9000)'
sleep 10

done
