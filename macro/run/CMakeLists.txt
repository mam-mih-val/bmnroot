add_test(run_sim ${ROOTSYS}/bin/root -b -q -l "${PROJECT_SOURCE_DIR}/macro/run/run_sim_bmn.C")
SET_TESTS_PROPERTIES (run_sim PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES (run_sim PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")

add_test(run_reco ${ROOTSYS}/bin/root -b -q -l "${PROJECT_SOURCE_DIR}/macro/run/run_reco_bmn.C")
SET_TESTS_PROPERTIES (run_reco PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES (run_reco PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")

add_test(run_digi ${ROOTSYS}/bin/root -b -q -l "${PROJECT_SOURCE_DIR}/macro/run/run_reco_bmn.C(\"run6-1889:/nica/data4mpd1/dataBMN/bmndata3/run6/root/digi/bmn_run1889_digi.root\", \"$VMCWORKDIR/macro/run/bmndst_exp.root\", 0, 100)")
SET_TESTS_PROPERTIES (run_digi PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES (run_digi PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")
