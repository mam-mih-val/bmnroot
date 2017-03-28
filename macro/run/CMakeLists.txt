add_test(run_sim ${ROOTSYS}/bin/root -b -q -l ${PROJECT_SOURCE_DIR}/macro/run/run_sim_bmn.C)
SET_TESTS_PROPERTIES (run_sim PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES (run_sim PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")

add_test(run_reco ${ROOTSYS}/bin/root -b -q -l ${PROJECT_SOURCE_DIR}/macro/run/run_reco_bmn.C)
SET_TESTS_PROPERTIES (run_reco PROPERTIES TIMEOUT "200")
SET_TESTS_PROPERTIES (run_reco PROPERTIES PASS_REGULAR_EXPRESSION "Macro finished successfully.")
