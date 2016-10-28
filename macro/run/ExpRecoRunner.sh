#!/bin/bash
#
# RUN: number of data file (62, 65, 81, ...)
# N_EVENTS: number of events to process (if 0, process whole file)
# DO_COMPILE: to compile BmnRoot before run or not (1 - compile, 0 - don't compile)
#

PERIOD=4
RUN=$1
N_EVENTS=$2
DO_COMPILE=$3

PATH_TO_DATA=$HOME"/BMN/dataRun4/"
PATH_TO_BMN=$HOME"/BMN/26_01_16"
PATH_TO_BUILD=$PATH_TO_BMN"/build"
PATH_TO_RAW=$PATH_TO_BMN"/macro/raw"
PATH_TO_RUN=$PATH_TO_BMN"/macro/run"

if [ "$DO_COMPILE" -eq 1 ]
then
cd $PATH_TO_BUILD
make -j2
. config.sh
fi

DATA_FILE="mpd_run_0$RUN.data"
DIGI_FILE="bmn_run00$RUN\_digi.root"
DST_FILE="bmndst.root"

cd $PATH_TO_RAW
root -l -q "BmnDataToRoot.C(\"$PATH_TO_DATA$DATA_FILE\", $N_EVENTS)"
cd $PATH_TO_RUN
if [ "$N_EVENTS" -eq 0 ]
then 
N_EVENTS=100000000
fi
root -l -q "run_reco_bmn.C(\"run$PERIOD-$RUN:$PATH_TO_RAW/$DIGI_FILE\", \"$PATH_TO_RUN/$DST_FILE\", 0, $N_EVENTS, kTRUE)"
