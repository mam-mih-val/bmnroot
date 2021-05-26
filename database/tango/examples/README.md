tango_db/examples
=====================
Examples for working with Tango data:

1. tango_avg_field.C - average magnetic field functions:
- tango_avg_field(int period = 6, int run = 1886)
Default Function for getting average magnetic field, magnet current and ratio for a given run (1886 run of 6-th period by default)
which returns average magnetic field coefficient (in case of errors the return value <= -1).
- tango_avg_field_write_db(int period = 7)
Function for calculating Tango average magnetic field for all runs of the given period/session and writing it to the Database.
- show_field_graph(int period_begin = 6, int run_begin = 1886, int period_end = -1, int run_end = -1)
Function for displaying  magnetic field for the given period of tjme from start run to end run,
if period_end == -1 and run_end == -1 then only magnetic field for one run will be showed,
if period_end == -1 and run_end != -1 then the end period is the same as begin period.
- compare_avg_field(int period = 7, bool isOnlyDifferent = false)
Function to compare magnetic field value (current, A) from ELog database and average magnetic field (voltage, mV) from the Tango database.
- compare_avg_field_graph(int period = 7)
Function to compare magnetic field value (current, A) from Elog database and average magnetic field (voltage, mV) from UniDb saved from the Tango, and show in TGraph object.

2.
