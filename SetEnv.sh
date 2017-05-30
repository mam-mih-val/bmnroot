#!/bin/bash

FAIREXTERNALS=$1

SIMMACHINE=$(uname -n)

# change SIMPATH if you installed FairSoft not to the '/opt/fairsoft/install' directory
export SIMPATH=/opt/fairsoft/install
export ROOTSYS=$SIMPATH

export PATH=$SIMPATH/bin:$PATH
export LD_LIBRARY_PATH=$SIMPATH/lib:$SIMPATH/lib/root:$LD_LIBRARY_PATH

source geant4.sh

platform=$(root-config --arch)
echo SIMPATH is pointing to $SIMPATH
