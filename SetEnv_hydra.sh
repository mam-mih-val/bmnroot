#!/bin/bash

SIMMACHINE=$(uname -n)

# change SIMPATH if you installed FairSoft not to the '/opt/fairsoft/install' directory
export SIMPATH=/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairsoft/install
# change FAIRROOTPATH if you installed FairRoot not to the '/opt/fairroot/install' directory
export FAIRROOTPATH=/cvmfs/hybrilit.jinr.ru/sw/slc7_x86-64/FAIR/fairroot/install

export ROOTSYS=$SIMPATH

export PATH=$SIMPATH/bin:$PATH
export LD_LIBRARY_PATH=$SIMPATH/lib:$SIMPATH/lib/root:$LD_LIBRARY_PATH

source geant4.sh

platform=$(root-config --arch)
echo SIMPATH is pointing to $SIMPATH
echo FAIRROOTPATH is pointing to $FAIRROOTPATH
