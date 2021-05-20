#!/bin/bash
#$ -N VertexFinder
#$ -j y
#$ -cwd
# request Bourne again shell as shell for job
#$ -S /bin/bash

# Start in batch mode
#MACHINES_TO_RUN="ncx238*"
#qsub -l 'hostname='${MACHINES_TO_RUN}'' -t 4649-4649 read_data_r7_vasilii.sh

if [ "${SGE_TASK_ID}" == "" ]; then RUN_ID=${1} ; else RUN_ID=${SGE_TASK_ID} ; qstat ; pwd; ls -alhF ; fi

#RUN_ID=${1}
PATH_TO_SCRIPTS="$(pwd)"
PATH_TO_BMNROOT_FROM_GLEB="/nica/mpd19/plotnikov/run7_Ar_automation/cbmroot-extended/bmnroot-root6-sigemcsc/"
PATH_TO_BMNROOT="/lhep/users/ilnur/bmnroot/"
#PATH_TO_TRACKS="/nica/mpd21/plotnikov/run7_Ar_automation/backup/zp0_6_yp2_3_sigemcscextend/identification/"
#PATH_TO_TRACKS="/home/ilnour/filesbmn/"
#PATH_TO_TRACKS="/eos/nica/bmn/users/batyuk/bmnDst_InnTracker+tof400+csc+tof700+dch/linksToData/"
PATH_TO_TRACKS="/nica/mpd19/plotnikov/run7_Ar_automation/identification/"
PATH_TO_DIGIS="/eos/nica/bmn/exp/digi/run7/20.02.0/3590-4707_BMN_Argon/"
#DST_NAME="bmndst_${RUN_ID}.root"
DST_OUT_NAME="bmn-pv-${RUN_ID}.root"
DIGI_NAME="bmn_run${RUN_ID}_digi.root"
DST_NAME="${RUN_ID}.root"

MACRO_NAME="run_pv_bmn.C"
# Check if Si+GEM tracks are exists
ls ${PATH_TO_TRACKS}/${DST_NAME}
if [ "$(echo $?)" == "2" ]; then echo "${PATH_TO_TRACKS}/${DST_NAME} is absent! Exit" ; exit 1 ; fi 

ls ${PATH_TO_SCRIPTS}/bmn-pv-${RUN_ID}.root
if [ "$(echo $?)" == "2" ]; then
  echo "Find primary vertex"

  cd ${PATH_TO_BMNROOT}/build
  . config.sh
  cd ${PATH_TO_SCRIPTS}

current_date_time="`date +%Y%m%d%H%M%S.%N`";
inner_path="VF7Parallel.${RUN_ID}.${JOB_ID}.${current_date_time}"
echo ${inner_path}
  mkdir ${inner_path}
  cd ${inner_path}
  ln -s ../../run/${MACRO_NAME} .
#  ln -s ../rootlogon.C .
#  ln -s ../SIGEMS_r7.root .
#  ln -s ../sts_v1_BMN_GEM.digi.par .
  ln -s ${PATH_TO_TRACKS}/${DST_NAME} .
  ln -s ${PATH_TO_DIGIS}/${DIGI_NAME} .
  echo "macro started"
  root -b -q ${MACRO_NAME}'("'${DIGI_NAME}'","'${DST_NAME}'","'${DST_OUT_NAME}'",0,0)'
  echo "macro finished"
  mv my_hist_file*.root ../my_hist_file_${RUN_ID}.root
  mv ${DST_OUT_NAME} ../

  cd ..
  rm -rf ${inner_path}
fi