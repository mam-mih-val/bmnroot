#!/bin/bash

# $1 - output directory

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ] || [ -z "$4" ]; then
echo "Arguments not defined !!! Exiting ..."
echo "You should define them:"
echo "[1] - output directory (like outDir in current folder)" 
echo "[2] - sigX of introd. misalign" 
echo "[3] - sigY of introd. misalign" 
echo "[4] - sigZ of introd. misalign"
echo "P.S.: Also, DO NOT FORGET to install a correct VMCWORKDIR (via source config.sh, for example) !!!"
exit;
fi

WORKDIR=$VMCWORKDIR/macro/alignment/QA
OUTDIR=$WORKDIR/$1
mkdir -p $OUTDIR

clear
echo "Preparing shifted geometry (see either $VMCWORKDIR/gem/XMLConfigs/GemRunSpring2018_misAlign.xml or $VMCWORKDIR/geometry/GEMS_RunSpring2018_misAlign.root)..."
sleep 5;
root -b -q 'create_rootgeom_GEMS_RunSpring2018_misAlign.C('$2', '$3', '$4')'

clear
echo "Starting MC with shifted geometry (sample consisting of 1kEvs: ten protons per event @ zero mag. field) ..."
sleep 5;
root -b -q run_sim_bmn.C

## Start "pure" reco from scratch w/o any misalignment introduced ...
clear
echo "Starting reco without misalignment introduced ..."
sleep 5;
root -b -q run_reco_bmn.C >& /dev/null

./iterativeAlign.sh 10 $OUTDIR
