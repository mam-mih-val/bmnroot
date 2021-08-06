#bin/bash
#. ~/bmnroot/build/config.sh

valgrind \
--leak-check=full \
--show-leak-kinds=all \
--track-origins=yes \
--log-file=val_sim.log \
--suppressions=valgrind.supp \
./run_sim_bmn #root.exe -l -b -q $VMCWORKDIR/macro/run/run_sim_bmn.C
