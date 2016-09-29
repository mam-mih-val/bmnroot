#!/bin/bash

PERIOD=4
RUN=$1
N_EVENTS=$2

PATH_TO_DATA="/home/merz/BMN/dataRun4/"
PATH_TO_BMN="/home/merz/BMN/26_01_16"
PATH_TO_BUILD=$PATH_TO_BMN"/build"
PATH_TO_RAW=$PATH_TO_BMN"/macro/raw"
PATH_TO_RUN=$PATH_TO_BMN"/macro/run"

cd $PATH_TO_BUILD
make -j2
. config.sh

DATA_FILE="mpd_run_0$RUN.data"
DIGI_FILE="bmn_run00$RUN\_digi.root"
DST_FILE="bmndst.root"

cd $PATH_TO_RAW
root -l -q "BmnDataToRoot.C(\"$PATH_TO_DATA$DATA_FILE\", $N_EVENTS)"
cd $PATH_TO_RUN
root -l -q "run_reco_bmn.C(\"run$PERIOD-$RUN:$PATH_TO_RAW/$DIGI_FILE\", \"$PATH_TO_RUN/$DST_FILE\", 0, $N_EVENTS, kTRUE, kTRUE)"
