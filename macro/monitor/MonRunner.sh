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
/opt/fairsoft/install/bin/root.exe -b -q 'monitor.C( 8, "/bmn/test/mon/", "/bmn/test/mon/", "bmn-mon", 9001)'
sleep 5
/opt/fairsoft/install/bin/root.exe -b -q 'monitor.C( 8, "/bmn/test/mon/", "/bmn/test/mon/", "bmn-mon", 9000)'
sleep 5

done
