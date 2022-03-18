#!/bin/bash
# 
# test  socat -u FILE:/ncx/eos/nica/bmn/exp/raw/run8/src_test/Top_2021_Dec/mpd_run_Top_090.data TCP4-LISTEN:32999,reuseaddr

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true; 

do
/opt/fairsoft/install/bin/root.exe -b -q $BMNROOT'/macro/monitor/monStreamDecoder.C("10.18.86.85:21323",8, kSRCSETUP)'
sleep 40

done
