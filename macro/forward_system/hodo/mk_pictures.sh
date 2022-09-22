#!/bin/bash
export FILEPATTERN=/data/bmn_data/converted/mpd_run_Top_FwdSysSH_BT_NoT_MF1700

events=-1
NUM_PROCS=7
i=10
num_jobs="\j"   # The prompt escape for number of jobs currently running
for file in ${FILEPATTERN}*.root; do
    ((i=i+1))
    #trap "exit" INT TERM ERR
    trap "kill 0" EXIT
    echo "working on run $file"
    root -l -b -q "simple_analysis.cpp(\"${file}\", \"${file}_siann_result\", ${events})" &
    background=( $(jobs -p) )
    if (( ${#background[@]} >= $NUM_PROCS )); then
        wait
    fi
done
wait
