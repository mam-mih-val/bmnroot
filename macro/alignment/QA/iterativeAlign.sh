#!/bin/bash

## $1 - num of iterations
## $2 - output directory

if [ -z "$1" ] || [ -z "$2" ]; then
echo "Arguments not defined !!! Exiting ..."
exit;
fi

## directory with instruments and workdir
INSTRUMENTS=$VMCWORKDIR/macro/alignment/instruments
WORKDIR=$VMCWORKDIR/macro/alignment/QA

## Definition of supplementary functions

#
make_start_file () {
    echo "Preparing start file ..."
    root -b -q ''$INSTRUMENTS'/makeAlignFile.C("align_start.root")' >& /dev/null
    mv align_start.root $ALIGNFILE
    echo "Done!"
}

#
reco () {
    echo "Starting reco ... "
    STEER_PATH=$VMCWORKDIR/macro/steering
    STEER_ORIG=gemTrackingSteer.dat
    STEER_CURRENT=steer_"$1".dat
	
    if [[ $1 == 1 ]]; 
    then
        X0=$(cat $STEER_PATH/$STEER_ORIG | grep rough | cut -d" " -f2)
        Y0=$(cat $STEER_PATH/$STEER_ORIG | grep rough | cut -d" " -f3)
        Z0=$(cat $STEER_PATH/$STEER_ORIG | grep rough | cut -d" " -f4)
        echo $X0 $Y0 $Z0 >> fitParams.txt
    fi  
	
    root -b -q ''$INSTRUMENTS'/extractVertAppr.C("bmndst_RUN7_GEM.root")' >& /dev/null
	
    X=$(cat fitParams.txt | cut -d" " -f1)
    Y=$(cat fitParams.txt | cut -d" " -f2)
    Z=$(cat fitParams.txt | cut -d" " -f3)      
	
    cp $STEER_PATH/$STEER_ORIG $STEER_PATH/$STEER_CURRENT
    cat $STEER_PATH/$STEER_CURRENT | sed 's/^.*rough.*$/roughVertex '$X' '$Y' '$Z'/' >> tmp
    mv tmp $STEER_PATH/$STEER_CURRENT
    root -b -q ''$WORKDIR'/run_reco_bmn.C("'$WORKDIR'/evetest_RUN7_GEM.root", "bmndst_RUN7_GEM.root", 0, 1000, "'$2'", "'$STEER_CURRENT'")' #>& /dev/null
    rm $STEER_PATH/$STEER_CURRENT	
    #fi
    echo "Done!"
}

#
alignment () {
    echo "Starting alignment ..."
    root -b -q ''$WORKDIR'/globAlignment.C(1e6, "bmndst_RUN7_GEM.root", "", 7)' #>& /dev/null
    echo "Done!"
}

#
summation () {
    echo "Starting summation ..."
    mv $ALIGNFILE it0.root
    root -b -q ''$INSTRUMENTS'/corrSummator.C("it0.root", "align_RUN7_GEM.root", "it1.root")' >& /dev/null
    mv it1.root $ALIGNFILE
    echo "Done!"
}

#
final () {
    echo "Finalizing ..."
    file="fitParams.txt"

    if [ -f $file ] ; then
	rm $file
    fi

    root -b -q ''$INSTRUMENTS'/readCorrections.C("it.root")' >& /dev/null
    mv introducedMisalign.dat $1
    mv obtainedCorrections.dat $1
    mv misAlignment.root $1/introducedMisalign.root
    mv it.root $1/obtainedCorrections.root
    mv bmndst_RUN7_GEM.root $1/bmndst_RUN7_GEM.root
    mv Millepede.res $1/Millepede_lastAddition.res
    echo "Done!"
}

## align. filename
ALIGNFILE=it.root

echo "Starting iterations ..."

for (( iter=0; iter<$1; iter++ )); 
do
    echo "Current iteration# " $iter

    ## 1. Preparing start file if doing from scratch ...
    if [[ $iter == 0 ]];
    then
	make_start_file $iter
    fi

    ## 2. Reconstruction
    if [[ $iter != 0 ]];
    then
        reco $iter $ALIGNFILE
    fi

    ## 3. Alignment
    alignment 

    ## 4. Summation
    summation 

    #rm -rf $PWD/$DIRNAME
done

## 5. Last actions
mkdir -p $2
final $2



