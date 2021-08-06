#bin/bash
#. ~/bmnroot/build/config.sh

valgrind \
--leak-check=full \
--show-leak-kinds=all \
--track-origins=yes \
--log-file=val_reco.log \
--suppressions=valgrind.supp \
./run_reco_bmn #root.exe -l -b -q $VMCWORKDIR/macro/run/run_reco_bmn.C
