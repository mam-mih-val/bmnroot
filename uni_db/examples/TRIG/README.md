uni_db/examples/TRIG
=====================
Examples for working with trigger data:

1. run_beam_info.C - show summary information about beam spills for a given run in a period (or for a whole period)
Main function: run_beam_info(int period = 7, int run = 0, TString target = "",
                             TString txtfile_path= "$VMCWORKDIR/uni_db/macros/parse_schemes/spill_run7/summaryMK.txt",
                             TString scheme_path = "$VMCWORKDIR/uni_db/macros/parse_schemes/spill_run7/spill_run7.xslt")
If 'run' parameter is zero or absent then spill data for all runs in the period will be shown.

2.
