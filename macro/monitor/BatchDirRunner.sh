#!/bin/bash
# 
# File:   MonRunner.sh
# Author: ilnur
#
# Created on Dec 1, 2016, 10:26:29 AM
#

BMNROOT=$HOME/bmnroot

source $BMNROOT/build/config.sh

/opt/fairsoft/install/bin/root.exe -b -q '$BMNROOT/macro/monitor/BatchDirectory.C("/bmn/run/current/")'
