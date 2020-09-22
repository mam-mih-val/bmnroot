#!/bin/bash

#RUN_RANGE=3756-4704
RUN_RANGE=4651-4655

#MACHINES_TO_RUN="ncx111*|ncx112*|ncx113*|ncx114*|ncx115*|ncx116*|ncx117*|ncx121*|ncx122*|ncx123*|ncx124*|ncx125*|ncx126*|ncx127*|ncx130*|ncx131*|ncx132*|ncx133*|ncx134*|ncx135*|ncx136*|ncx137*|ncx138*|ncx139*|ncx140*|ncx141*|ncx142*|ncx143*|ncx144*|ncx145*|ncx146*|ncx147*|ncx148*|ncx149*|ncx150*|ncx151*|ncx152*|ncx153*|ncx154*|ncx155*|ncx156*|ncx157*|ncx158*|ncx159*|ncx160*|ncx161*|ncx162*|ncx163*|ncx164*|ncx165*|ncx166*|ncx167*|ncx168*|ncx169*|ncx170*|ncx171*|ncx172*|ncx174*|ncx175*|ncx176*|ncx180*|ncx181*|ncx182*|ncx184*|ncx185*|ncx186*|ncx201*|ncx202*|ncx211*|ncx212*|ncx214*|ncx215*|ncx216*|ncx217*|ncx218*|ncx223*|ncx224*|ncx225*|ncx226*|ncx228*|ncx231*|ncx232*|ncx233*|ncx234*|ncx235*|ncx236*|ncx237*|ncx238*"

#qsub -l 'hostname='${MACHINES_TO_RUN}'' -t ${RUN_RANGE} read_data_r7_parallel.sh
qsub  -t ${RUN_RANGE} run_pv_parallel.sh