//run eventdisplay in online mode for simulation data
void run_mc_online()
{
    //eventdisplay(char* input_file, char* geo_file, char* out_file, bool is_online = false, int data_source)
    gROOT->ProcessLine(".x eventdisplay.C(\"$VMCWORKDIR/macro/run/bmndst.root\", \"$VMCWORKDIR/macro/run/evetest.root\", \"tmp.root\", true, 0)");
}
