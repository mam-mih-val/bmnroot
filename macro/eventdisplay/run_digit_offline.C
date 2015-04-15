//run eventdisplay in offline mode for raw data files
void run_digit_offline()
{
    //eventdisplay.C(char* input_file = 0, char* geo_file = 0, char* out_file = 0, bool is_online = false, int data_source = 0);
    gROOT->ProcessLine(".x eventdisplay.C(\"$VMCWORKDIR/macro/run/bmn_run0607_digit.root\", \"$VMCWORKDIR/macro/run/evetest_r3.root\", \"tmp.root\", false, 2)");
}
