//run eventdisplay in online mode for raw data files
void run_raw_online()
{
    //eventdisplay.C(char* input_file = 0, char* add_file = 0, char* out_file = 0, bool is_online = false, int data_source = 0);
    gROOT->ProcessLine(".x eventdisplay.C(\"/home/soul/bmnroot/macro/eventdisplay/2014-12-29_18-06-03_hrb046f\", 0, 0, true, 1)");
}
