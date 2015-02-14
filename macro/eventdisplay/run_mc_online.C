//run eventdisplay in online mode for simulation data
void run_mc_online()
{
    //eventdisplay.C(char* input_file = 0, char* add_file = 0, char* out_file = 0, bool is_online = false, int data_source = 0);
    gROOT->ProcessLine(".x eventdisplay.C(0, 0, 0, true, 0)");
}
