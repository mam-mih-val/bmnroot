#!/bin/bash

if [ "$#" -lt "1" ]; then
  export CDASHUSERPATH=$(pwd)
else
  export CDASHUSERPATH=$1
fi

export SIMPATH=/opt/fairsoft/install
export FAIRROOTPATH=/opt/fairroot/install
export SOURCEDIR=$CDASHUSERPATH
export BUILDDIR=$HOME/CDASH/bmnroot_build_$(hostname -s)
export LINUX_FLAVOUR=$(uname -o)
export FAIRSOFT_VERSION=oct17p1

if [ "$#" -gt "1" ]; then
if [ "$#" -gt "2" ]; then
export FAIRSOFT_VERSION=$3
else
export FAIRSOFT_VERSION=oct17p1
fi
export BUILDDIR=$2
fi

echo "CDASHUSERPATH: " $CDASHUSERPATH
echo "SIMPATH: " $SIMPATH
echo "FAIRROOTPATH: " $FAIRROOTPATH
echo "SOURCEDIR: " $SOURCEDIR
echo "BUILDDIR: " $BUILDDIR
echo "LINUX_FLAVOUR: " $LINUX_FLAVOUR
echo "FAIRSOFT_VERSION: " $FAIRSOFT_VERSION

DARTMACHINE=$(uname -n)

source $BUILDDIR/config.sh
