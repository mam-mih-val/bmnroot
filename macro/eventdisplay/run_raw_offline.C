//run eventdisplay in offline mode for raw data files
void run_raw_offline()
{
    //eventdisplay.C(char* input_file = 0, char* add_file = 0, char* out_file = 0, bool is_online = false, int data_source = 0);
    gROOT->ProcessLine(".x eventdisplay.C(\"$VMCWORKDIR/macro/eventdisplay/2014-12-29_18-06-03_hrb046f\", \"$VMCWORKDIR/macro/run/evetest.root\", \"tmp.root\", false, 1)");
}
