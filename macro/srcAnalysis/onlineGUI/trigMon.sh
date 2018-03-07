#!/bin/bash

# Defining directories to be used
MONDIR=$VMCWORKDIR"/macro/srcAnalysis/onlineGUI"
OUTPUT=$MONDIR"/output"
DIGITIZER=$VMCWORKDIR"/macro/raw"

rm output/*.root > $OUTPUT/out.dat
read -p 'Current Run Number: ' runNum

scp segarrae@nc3.jinr.ru:/ceph/bmn/run/current/mpd_run_trigCode_${runNum}.data ./output/

FILE=$OUTPUT'/mpd_run_trigCode_'${runNum}'.data'
echo "Reading File: "${FILE}

# First analyze the raw binary file that we collected and create the digi file
cd $VMCWORKDIR"/macro/raw"
root -l -b -q "BmnDataToRoot.C(\"$FILE\")"
echo ""
echo "==========================================================="
echo ""
echo "FINISHED DECODING"
echo ""
mv bmn_run1234_* $OUTPUT > $OUTPUT/out.dat


# Now analyze that 1 digi file for all of the detectors
cd $MONDIR"/src" > $OUTPUT"/out.dat"

DIGI=$OUTPUT'/bmn_run1234_digi.root'
CURR_TQDC_BCs=$OUTPUT'/det_histos_curr_TQDC_BCs.root'
CURR_TDC_BCs=$OUTPUT'/det_histos_curr_TDC_BCs.root'
CURR_TQDC_Arms=$OUTPUT'/det_histos_curr_TQDC_Arms.root'
CURR_TDC_Arms=$OUTPUT'/det_histos_curr_TDC_Arms.root'
CURR_Others=$OUTPUT'/det_histos_curr_other.root'

PATH_TO_REF=$MONDIR'/ref/bmn_runSRCtest105_digi.root'
REF_TQDC_BCs=$OUTPUT'/det_histos_ref_TQDC_BCs.root'
REF_TDC_BCs=$OUTPUT'/det_histos_ref_TDC_BCs.root'
REF_TQDC_Arms=$OUTPUT'/det_histos_ref_TQDC_Arms.root'
REF_TDC_Arms=$OUTPUT'/det_histos_ref_TDC_Arms.root'
REF_Others=$OUTPUT'/det_histos_ref_other.root'

root -l -b -q "getNum.C(\"$DIGI\",\"$PATH_TO_REF\")"

echo ""
echo "Now analyzing created digi file. Please choose how many events to skim (from START to END)"
echo "Keep in mind the number of events in the reference and current files"
echo ""
read -p 'Starting event: ' startEvent
read -p 'Ending event [-1 = all; typical 10,000]: ' stopEvent

# 	Doing the analysis for our trigger detectors, on the current run
root -l -b -q   "BCs_TQDC_digitize.C(\"$DIGI\",\"$CURR_TQDC_BCs\",$startEvent,$stopEvent)"
root -l -b -q    "BCs_TDC_digitize.C(\"$DIGI\",\"$CURR_TDC_BCs\",$startEvent,$stopEvent)"
root -l -b -q "Arms_TQDC_digitize.C(\"$DIGI\",\"$CURR_TQDC_Arms\",$startEvent,$stopEvent)"
root -l -b -q  "Arms_TDC_digitize.C(\"$DIGI\",\"$CURR_TDC_Arms\",$startEvent,$stopEvent)"
#root -l -b -q "Others_digitize.C(\"$DIGI\",\"$CURR_Others\",$startEvent,$stopEvent)"

# 	WE NEED TO REDO THE REFERENCE RUN
root -l -b -q "BCs_TQDC_digitize.C(\"$PATH_TO_REF\",\"$REF_TQDC_BCs\",$startEvent,$stopEvent)"
root -l -b -q "BCs_TDC_digitize.C(\"$PATH_TO_REF\",\"$REF_TDC_BCs\",$startEvent,$stopEvent)"
root -l -b -q "Arms_TQDC_digitize.C(\"$PATH_TO_REF\",\"$REF_TQDC_Arms\",$startEvent,$stopEvent)"
root -l -b -q "Arms_TDC_digitize.C(\"$PATH_TO_REF\",\"$REF_TDC_Arms\",$startEvent,$stopEvent)"
#root -l -b -q "Others_digitize.C(\"$PATH_TO_REF\",\"$REF_Others\",$startEvent,$stopEvent)"

echo ""
echo "==========================================================="
echo "FINISHED ANALYZING"

root -l "monGUI.C"
cd $MONDIR
