#!/bin/bash

if [ "$#" != "5" ]
then
 echo "Five parameters are required at least: macro name, input file, start event, event count and output file"
 return 1
fi

#Parallel for all up nodes
count=`pbsnodes -l up | wc -l`

#Parallel for free processor
#count=`showbf | grep "procs available" | tr -d ' '`
#count=`expr "$count" : '\([0-9]*\)'`

#Parallel for all processor
#count=`showq | grep "Processors Active" | tr -d ' '`
#indf=`expr index "$count" f`
#indP=`expr index "$count" P`
#indf=$indf
#indP=$indP-$indf-1
#count=${count:$indf:$indP}

if [ "$#" < "5" ]
then
 echo "Five parameters are required at least: macro name, input file, start event, event count and output file"
 return 1
fi

if ! [[ "$3" =~ ^[0-9]+$ ]] ; then
   exec >&2; echo "error: Start event must be a number"; return 2
fi

start=$3

if ! [[ "$4" =~ ^[0-9]+$ ]] ; then
   exec >&2; echo "error: Event count must be a number"; return 2
fi

event=$4

indPoint=`expr index "$5" .`
indPoint=$((indPoint-1))
output=${5:0:$indPoint}"\$PBS_JOBID"${5:$indPoint}

jobsID=""
par=""

i=0
while [ "$i" != "$count" ]
do
 event_per_proc=$(((event+i)/count))
 echo $start
 echo $event_per_proc

 if [ "$event_per_proc" != "0" ]
 then
  ID=`qsub mpd.qsub -v "macro=$1, in=$2, event=$start, count=$event_per_proc, out=$output"`
  jobsID=$jobsID":"$ID
  par=$par" "${output//\$PBS_JOBID/`expr "$ID" : '\([0-9]*\)'`}
 fi

 i=$(( $i + 1 ))
 start=$((start+event_per_proc))
done

sParameters=$5$par
qsub -W depend=afterok$jobsID union.qsub -v "sParameters=$sParameters"
