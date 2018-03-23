#!/bin/bash

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

while true;

do
nodejs mscWatcher.js
sleep 1

done